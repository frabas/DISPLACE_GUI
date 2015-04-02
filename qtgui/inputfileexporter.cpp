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
    closedpath = closedpath.replace("?", "%1");

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

    QTextStream clsstream;
    QFile clsfile[4];
    if (!closedpath.isEmpty()) {
        for (int i = 0; i < 4; ++i) {
            QString fn = closedpath.arg(i);
            clsfile[i].setFileName(fn);
            if (!clsfile[i].open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                if (error)
                    *error = QString(QObject::tr("Cannot open closed polygons file %1: %2"))
                        .arg(fn).arg(clsfile[i].errorString());
                return false;
            }
        }
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
                std::shared_ptr<NodeData> n = currentModel->getNodesList()[i]->getAdiacencyByIdx(j)->target.lock();
                if (n.get() != nullptr)
                    gstrm << n->get_idx_node() << endl;
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

    for (int q = 0; q < 4; ++q) {
        if (clsfile[q].isOpen()) {
            clsstream.setDevice(&clsfile[q]);
            int N = currentModel->countPenaltyPolygons(q);
            for (int i = 0; i < N; ++i) {
                foreach (int ndx, currentModel->getPenaltyPolygonsAt(q,i)) {
                    clsstream << (i+1) << " " << ndx << endl;
                }
            }
            clsfile[q].close();
        }
    }

    return true;

}
