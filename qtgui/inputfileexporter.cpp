#include "inputfileexporter.h"

#include <displacemodel.h>
#include <modelobjects/nodedata.h>
#include <utils/fileformathelper.h>

InputFileExporter::InputFileExporter()
{
}

bool InputFileExporter::exportGraph(QString graphpath, QString coordspath,
                                    QString landpath, QString areacodepath, QString closedpath,
                                    DisplaceModel *currentModel, QString *error)
{
    bool multiple_closed_path = false;
    QString curr_clspath = closedpath;
    if (closedpath.contains("?")) {
        closedpath = closedpath.replace("?", "%1");
        multiple_closed_path = true;

        curr_clspath = closedpath.arg(1);
    }

    QFile cfile(coordspath);
    if (!cfile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (error)
            *error = QString(QObject::tr("Error Opening coords file %1 for write: %2")).arg(coordspath).arg(cfile.errorString());
        return false;
    }

    // Coord Stream file
    QTextStream cstrm(&cfile);

    QFile landfile(landpath);
    QTextStream landstream;
    if (!landpath.isEmpty()) {
        if (!landfile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            if (error)
                *error = QString(QObject::tr("Cannot open landscape file %1: %2"))
                    .arg(landpath).arg(landfile.errorString());
            return false;
        }
        landstream.setDevice(&landfile);
    }

    QFile acfile(areacodepath);
    QTextStream acstream;
    if (!areacodepath.isEmpty()) {
        if (!acfile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            if (error)
                *error = QString(QObject::tr("Cannot open area code file %1: %2"))
                    .arg(areacodepath).arg(acfile.errorString());
            return false;
        }
        acstream.setDevice(&acfile);
    }

    QFile clsfile(curr_clspath);
    QTextStream clsstream;
    if (!closedpath.isEmpty()) {
        if (!clsfile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            if (error)
                *error = QString(QObject::tr("Cannot open closed polygons file %1: %2"))
                    .arg(closedpath).arg(clsfile.errorString());
            return false;
        }
        clsstream.setDevice(&clsfile);
    }

    int n = currentModel->getNodesCount();
    for (int i = 0; i < n; ++i) {
        double x = currentModel->getNodesList()[i]->get_x();
        cstrm << x << endl;
        if (acfile.isOpen())
            acstream << x << endl;
        if (landfile.isOpen())
            landstream << currentModel->getNodesList()[i]->get_marine_landscape() << endl;
    }
    for (int i = 0; i < n; ++i) {
        double y = currentModel->getNodesList()[i]->get_y();
        cstrm << y << endl;
        if (acfile.isOpen())
            acstream << y << endl;
    }
    for (int i = 0; i < n; ++i) {
        cstrm << currentModel->getNodesList()[i]->get_harbour() << endl;
        if (acfile.isOpen())
            acstream << currentModel->getNodesList()[i]->get_code_area() << endl;
    }

    cfile.close();
    acfile.close();
    landfile.close();

    if (!graphpath.isEmpty()) {
        QFile gfile(graphpath);
        if (!gfile.open(QIODevice::WriteOnly)) {
            if (error)
                *error = QString(QObject::tr("Error Opening %1 for write: %2")).arg(coordspath).arg(cfile.errorString());
            return false;
        }

        // Graph stream file
        QTextStream gstrm(&gfile);

        for (int i = 0; i < n; ++i) {
            int m = currentModel->getNodesList()[i]->getAdiacencyCount();
            for (int j = 0; j < m; ++j) {
                gstrm << currentModel->getNodesList()[i]->get_idx_node() << endl;
            }
        }
        for (int i = 0; i < n; ++i) {
            int m = currentModel->getNodesList()[i]->getAdiacencyCount();
            for (int j = 0; j < m; ++j) {
                gstrm << currentModel->getNodesList()[i]->getAdiacencyByIdx(j) << endl;
            }
        }
        for (int i = 0; i < n; ++i) {
            int m = currentModel->getNodesList()[i]->getAdiacencyCount();
            for (int j = 0; j < m; ++j) {
                gstrm << FileFormatHelper::roundWeight(currentModel->getNodesList()[i]->getAdiacencyWeight(j)) << endl;
            }
        }

        gfile.close();
    }

    if (clsfile.isOpen()) {
        int N = currentModel->countPenaltyPolygons();
        for (int i = 0; i < N; ++i) {
            foreach (int ndx, currentModel->getPenaltyPolygonsAt(i)) {
                clsstream << (i+1) << " " << ndx << endl;
            }
        }
    }
    clsfile.close();

    if (multiple_closed_path) {
        QFile src (curr_clspath);

        for (int i = 2; i <= 4; ++i) {
            QString dst(closedpath.arg(i));
            QFile::remove(dst);
            src.copy(dst);
        }
    }

    return true;

}
