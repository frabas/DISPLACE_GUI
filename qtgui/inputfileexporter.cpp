// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

#include "inputfileexporter.h"

#include <displacemodel.h>
#include <modelobjects/nodedata.h>
#include <utils/fileformathelper.h>

InputFileExporter::InputFileExporter()
{
}

bool InputFileExporter::exportGraph(QString graphpath, QString coordspath,
                                    QString landpath, QString areacodepath, QString closedpath,
                                    QString closedpath_month, QString closedpath_vessz,
                                    bool export_closedpoly,
                                    DisplaceModel *currentModel, QString *error)
{
    closedpath = closedpath.replace("?", "%1");
    closedpath_month = closedpath_month.replace("?", "%1");
    closedpath_vessz = closedpath_vessz.replace("?", "%1");

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
    for (int i = 0; i < 4; ++i) {
        QString fn = closedpath.arg(i+1);
        clsfile[i].setFileName(fn);
        if (!clsfile[i].open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            if (error)
                *error = QString(QObject::tr("Cannot open closed polygons file %1: %2"))
                    .arg(fn).arg(clsfile[i].errorString());
            return false;
        }
    }

    int n = currentModel->getNodesCount();
    for (int i = 0; i < n; ++i) {
        auto nd = currentModel->getNodesList()[i];
        if (nd) {
            double x = nd->get_x();
            cstrm << x << endl;
            if (acfile.isOpen())
                acstream << x << endl;
            if (landfile.isOpen())
                landstream << nd->get_marine_landscape() << endl;
        }
    }
    for (int i = 0; i < n; ++i) {
        auto nd = currentModel->getNodesList()[i];
        if (nd) {
            double y = nd->get_y();
            cstrm << y << endl;
            if (acfile.isOpen())
                acstream << y << endl;
        }
    }
    for (int i = 0; i < n; ++i) {
        auto nd = currentModel->getNodesList()[i];
        if (nd) {
            cstrm << nd->get_harbour() << endl;
            if (acfile.isOpen())
                acstream << nd->get_code_area() << endl;
            }
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
            auto nd = currentModel->getNodesList()[i];
            if (nd) {
                int m = nd->getAdiacencyCount();
                for (int j = 0; j < m; ++j) {
                    gstrm << nd->get_idx_node() << endl;
                }
            }
        }
        for (int i = 0; i < n; ++i) {
            auto nd = currentModel->getNodesList()[i];
            if (nd) {
                int m = nd->getAdiacencyCount();
                for (int j = 0; j < m; ++j) {
                    std::shared_ptr<NodeData> n = nd->getAdiacencyByIdx(j)->target.lock();
                    if (n.get() != nullptr)
                        gstrm << n->get_idx_node() << endl;
                }
            }
        }
        for (int i = 0; i < n; ++i) {
            auto nd = currentModel->getNodesList()[i];
            if (nd) {
                int m = nd->getAdiacencyCount();
                for (int j = 0; j < m; ++j) {
                    gstrm << FileFormatHelper::roundWeight(currentModel->getNodesList()[i]->getAdiacencyWeight(j)) << endl;
                }
            }
        }

        gfile.close();
    }

    if (export_closedpoly) {
        for (int q = 0; q < 4; ++q) {
            if (clsfile[q].isOpen()) {
                clsstream.setDevice(&clsfile[q]);
                auto &pl = currentModel->getPenaltyCollection();
                for (auto p : pl) {
                    if (p.closed && p.q[q]) {
                        clsstream << p.polyId << " " << p.nodeId;
                        for (auto m : p.metiers)
                            clsstream << " " << m;
                        clsstream << "\n";
                    }
                }
                clsfile[q].close();
            }
        }

        // Months
        for (int month = 0; month < 12; ++month) {
            QString fn = closedpath_month.arg(month+1);

            auto monthSelectorFunc = [month](const displace::NodePenalty &penalty) { return penalty.closed && penalty.months[month]; };
            auto outputMetierFunc = [](const displace::NodePenalty &penalty) { return penalty.metiers; };

            bool r = outputClosedPolyFile(fn, currentModel, monthSelectorFunc, outputMetierFunc, error);
            if (!r)
                return false;

            fn = closedpath_vessz.arg(month+1);

            auto outputVesSizeFunc = [](const displace::NodePenalty &penalty) {
                return penalty.vesSizes;
            };

            r = outputClosedPolyFile(fn, currentModel, monthSelectorFunc, outputVesSizeFunc, error);
            if (!r)
                return false;
        }
    }
    return true;

}

bool InputFileExporter::outputClosedPolyFile(QString filename, DisplaceModel *currentModel,
                                             std::function<bool(const displace::NodePenalty &)> selector,
                                             std::function<std::vector<int>(const displace::NodePenalty &)> dataGetter,
                                             QString *error)
{
    QFile f(filename);
    if (!f.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
        if (error)
            *error = QString(QObject::tr("Cannot open closed polygons file %1: %2"))
                .arg(filename).arg(f.errorString());
        return false;
    }

    QTextStream clsstream;
    clsstream.setDevice(&f);

    auto &pl = currentModel->getPenaltyCollection();
    for (auto p : pl) {
        if (selector(p)) {
            clsstream << p.polyId << " " << p.nodeId;
            for (auto &m : dataGetter(p))
                clsstream << " " << m;
            clsstream << "\n";
        }
    }

    f.close();
    return true;
}
