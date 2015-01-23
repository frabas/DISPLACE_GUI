#include "inputfileexporter.h"

#include <displacemodel.h>
#include <modelobjects/nodedata.h>

InputFileExporter::InputFileExporter()
{
}

bool InputFileExporter::exportGraph(QString graphpath, QString coordspath, DisplaceModel *currentModel, QString *error)
{
    QFile cfile(coordspath);
    if (!cfile.open(QIODevice::WriteOnly)) {
        if (error)
            *error = QString(QObject::tr("Error Opening coords file %1 for write: %2")).arg(coordspath).arg(cfile.errorString());
        return false;
    }

    QTextStream cstrm(&cfile);

    int n = currentModel->getNodesCount();
    for (int i = 0; i < n; ++i) {
        cstrm << currentModel->getNodesList()[i]->get_x() << endl;
    }
    for (int i = 0; i < n; ++i) {
        cstrm << currentModel->getNodesList()[i]->get_y() << endl;
    }
    for (int i = 0; i < n; ++i) {
        cstrm << currentModel->getNodesList()[i]->get_harbour() << endl;
    }

    cfile.close();

    if (!graphpath.isEmpty()) {
        QFile gfile(graphpath);
        if (!gfile.open(QIODevice::WriteOnly)) {
            if (error)
                *error = QString(QObject::tr("Error Opening %1 for write: %2")).arg(coordspath).arg(cfile.errorString());
            return false;
        }

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
                gstrm << static_cast<int>(std::floor(currentModel->getNodesList()[i]->getAdiacencyWeight(j) + 0.5)) << endl;
            }
        }

        gfile.close();
    }

    return true;

}
