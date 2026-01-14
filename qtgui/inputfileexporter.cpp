// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2026 Francois Bastardie <fba@aqua.dtu.dk>

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
                                    QString landpath,  QString windpath, QString sstpath, QString salinitypath,
                                    QString Nitrogenpath, QString Phosphoruspath, QString Oxygenpath, QString DissolvedCarbonpath,
                                    QString bathymetrypath, QString shippingdensitypath,  QString siltfractionpath, QString icesrectanglecodepath,
                                    QString benthospath, QString benthosnbpath, QString areacodepath, QString closedpath,
                                    QString closedpath_month, QString closedpath_vessz, QString closedpath_nations,
                                    bool export_closedpoly,
                                    DisplaceModel *currentModel, QString *error)
{
    closedpath = closedpath.replace("?", "%1");
    closedpath_month = closedpath_month.replace("?", "%1");
    closedpath_vessz = closedpath_vessz.replace("?", "%1");
    closedpath_nations = closedpath_nations.replace("?", "%1");

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

    QFile windfile(windpath);
    QTextStream windstream;
    if (!windpath.isEmpty()) {
        if (!windfile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            if (error)
                *error = QString(QObject::tr("Cannot open wind file %1: %2"))
                    .arg(windpath).arg(windfile.errorString());
            return false;
        }
        windstream.setDevice(&windfile);
    }

    QFile sstfile(sstpath);
    QTextStream sststream;
    if (!sstpath.isEmpty()) {
        if (!sstfile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            if (error)
                *error = QString(QObject::tr("Cannot open sst file %1: %2"))
                    .arg(sstpath).arg(sstfile.errorString());
            return false;
        }
        sststream.setDevice(&sstfile);
    }

    QFile salinityfile(salinitypath);
    QTextStream salinitystream;
    if (!salinitypath.isEmpty()) {
        if (!salinityfile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            if (error)
                *error = QString(QObject::tr("Cannot open sst file %1: %2"))
                    .arg(salinitypath).arg(salinityfile.errorString());
            return false;
        }
        salinitystream.setDevice(&salinityfile);
    }

    QFile Nitrogenfile(Nitrogenpath);
    QTextStream Nitrogenstream;
    if (!Nitrogenpath.isEmpty()) {
        if (!Nitrogenfile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            if (error)
                *error = QString(QObject::tr("Cannot open Nitrogen file %1: %2"))
                    .arg(Nitrogenpath).arg(Nitrogenfile.errorString());
            return false;
        }
        Nitrogenstream.setDevice(&Nitrogenfile);
    }

    QFile Phosphorusfile(Phosphoruspath);
    QTextStream Phosphorusstream;
    if (!Phosphoruspath.isEmpty()) {
        if (!Phosphorusfile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            if (error)
                *error = QString(QObject::tr("Cannot open Phosphorus file %1: %2"))
                    .arg(Phosphoruspath).arg(Phosphorusfile.errorString());
            return false;
        }
        Phosphorusstream.setDevice(&Phosphorusfile);
    }

    QFile Oxygenfile(Oxygenpath);
    QTextStream Oxygenstream;
    if (!Oxygenpath.isEmpty()) {
        if (!Oxygenfile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            if (error)
                *error = QString(QObject::tr("Cannot open Oxygen file %1: %2"))
                    .arg(Oxygenpath).arg(Oxygenfile.errorString());
            return false;
        }
        Oxygenstream.setDevice(&Oxygenfile);
    }

    QFile DissolvedCarbonfile(DissolvedCarbonpath);
    QTextStream DissolvedCarbonstream;
    if (!DissolvedCarbonpath.isEmpty()) {
        if (!DissolvedCarbonfile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            if (error)
                *error = QString(QObject::tr("Cannot open DissolvedCarbon file %1: %2"))
                    .arg(DissolvedCarbonpath).arg(DissolvedCarbonfile.errorString());
            return false;
        }
        DissolvedCarbonstream.setDevice(&DissolvedCarbonfile);
    }


    QFile bathymetryfile(bathymetrypath);
    QTextStream bathymetrystream;
    if (!bathymetrypath.isEmpty()) {
        if (!bathymetryfile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            if (error)
                *error = QString(QObject::tr("Cannot open bathymetry file %1: %2"))
                    .arg(bathymetrypath).arg(bathymetryfile.errorString());
            return false;
        }
        bathymetrystream.setDevice(&bathymetryfile);
    }

    QFile shippingdensityfile(shippingdensitypath);
    QTextStream shippingdensitystream;
    if (!shippingdensitypath.isEmpty()) {
        if (!shippingdensityfile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            if (error)
                *error = QString(QObject::tr("Cannot open shippingdensity file %1: %2"))
                    .arg(shippingdensitypath).arg(shippingdensityfile.errorString());
            return false;
        }
        shippingdensitystream.setDevice(&shippingdensityfile);
    }


    QFile siltfractionfile(siltfractionpath);
    QTextStream siltfractionstream;
    if (!siltfractionpath.isEmpty()) {
        if (!siltfractionfile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            if (error)
                *error = QString(QObject::tr("Cannot open siltfraction file %1: %2"))
                    .arg(siltfractionpath).arg(siltfractionfile.errorString());
            return false;
        }
        siltfractionstream.setDevice(&siltfractionfile);
    }

    QFile icesrectanglecodefile(icesrectanglecodepath);
    QTextStream icesrectanglecodestream;
    if (!icesrectanglecodepath.isEmpty()) {
        if (!icesrectanglecodefile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            if (error)
                *error = QString(QObject::tr("Cannot open icesrectanglecode file %1: %2"))
                .arg(icesrectanglecodepath).arg(icesrectanglecodefile.errorString());
            return false;
        }
        icesrectanglecodestream.setDevice(&icesrectanglecodefile);
    }

    QFile bfile(benthospath);
    QTextStream bstream;
    if (!benthospath.isEmpty()) {
        if (!bfile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            if (error)
                *error = QString(QObject::tr("Cannot open area code file %1: %2"))
                    .arg(benthospath).arg(bfile.errorString());
            return false;
        }
        bstream.setDevice(&bfile);
    }

    QFile bnbfile(benthosnbpath);
    QTextStream bnbstream;
    if (!benthosnbpath.isEmpty()) {
        if (!bnbfile.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
            if (error)
                *error = QString(QObject::tr("Cannot open area code file %1: %2"))
                    .arg(benthosnbpath).arg(bnbfile.errorString());
            return false;
        }
        bnbstream.setDevice(&bnbfile);
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
            cstrm << x << "\n";
            if (acfile.isOpen())
                acstream << x << "\n";
            if (bfile.isOpen())
                bstream << nd->get_init_benthos_biomass() << "\n";
            if (bnbfile.isOpen())
                bnbstream << nd->get_init_benthos_number() << "\n";
            if (landfile.isOpen())
                landstream << nd->get_marine_landscape() << "\n";
            if (windfile.isOpen())
                windstream << nd->get_wind() << "\n";
            if (sstfile.isOpen())
                sststream << nd->get_sst() << "\n";
            if (salinityfile.isOpen())
                salinitystream << nd->get_salinity() << "\n";
            if (Nitrogenfile.isOpen())
                Nitrogenstream << nd->get_Nitrogen() << "\n";
            if (Phosphorusfile.isOpen())
                Phosphorusstream << nd->get_Phosphorus() << "\n";
            if (Oxygenfile.isOpen())
                Oxygenstream << nd->get_Oxygen() << "\n";
            if (DissolvedCarbonfile.isOpen())
                DissolvedCarbonstream << nd->get_DissolvedCarbon() << "\n";
            if (bathymetryfile.isOpen())
                bathymetrystream << nd->get_bathymetry() << "\n";
            if (shippingdensityfile.isOpen())
                shippingdensitystream << nd->get_shippingdensity() << "\n";
            if (siltfractionfile.isOpen())
                siltfractionstream << nd->get_siltfraction() << "\n";
            if (icesrectanglecodefile.isOpen())
                icesrectanglecodestream << nd->get_icesrectanglecode() << "\n";


        }
    }
    for (int i = 0; i < n; ++i) {
        auto nd = currentModel->getNodesList()[i];
        if (nd) {
            double y = nd->get_y();
            cstrm << y << "\n";
            if (acfile.isOpen())
                acstream << y << "\n";
        }
    }
    for (int i = 0; i < n; ++i) {
        auto nd = currentModel->getNodesList()[i];
        if (nd) {
            cstrm << nd->get_harbour() << "\n";
            if (acfile.isOpen())
                acstream << nd->get_code_area() << "\n";
            }
    }

    cfile.close();
    acfile.close();
    landfile.close();
    windfile.close();
    sstfile.close();
    salinityfile.close();
    Nitrogenfile.close();
    Phosphorusfile.close();
    Oxygenfile.close();
    DissolvedCarbonfile.close();
    bathymetryfile.close();
    shippingdensityfile.close();
    siltfractionfile.close();
    bfile.close();
    bnbfile.close();

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
                    gstrm << nd->get_idx_node().toIndex() << "\n";
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
                        gstrm << n->get_idx_node().toIndex() << "\n";
                }
            }
        }
        for (int i = 0; i < n; ++i) {
            auto nd = currentModel->getNodesList()[i];
            if (nd) {
                int m = nd->getAdiacencyCount();
                for (int j = 0; j < m; ++j) {
                    //gstrm << FileFormatHelper::roundWeight(currentModel->getNodesList()[i]->getAdiacencyWeight(j)) << "\n";
                    gstrm << currentModel->getNodesList()[i]->getAdiacencyWeight(j) << "\n";
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
                    if (p.nbOfDaysClosed>0 && p.q[q]) {
                        clsstream << p.polyId << " " << p.nbOfDaysClosed << " " << p.nodeId;
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

            auto monthSelectorFunc = [month](const displace::NodePenalty &penalty) { return penalty.nbOfDaysClosed>0 && penalty.months[month]; };
            auto outputMetierFunc = [](const displace::NodePenalty &penalty) { return penalty.metiers; };

            bool r = outputClosedPolyFile(fn, currentModel,  monthSelectorFunc, outputMetierFunc, error);
            if (!r)
                return false;

            fn = closedpath_vessz.arg(month+1);
        
        
            auto outputVesSizeFunc = [](const displace::NodePenalty& penalty) {
                return penalty.vesSizes;
            };
            r = outputClosedPolyFile(fn, currentModel,  monthSelectorFunc, outputVesSizeFunc, error);
            if (!r)
                return false;
       
            
            fn = closedpath_nations.arg(month + 1);
            
            auto outputNationFunc = [](const displace::NodePenalty& penalty) {
                return penalty.Nations;
            };

            r = outputClosedPolyFile(fn, currentModel, monthSelectorFunc, outputNationFunc, error);
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
            clsstream << p.polyId << " " << p.nbOfDaysClosed << " " << p.nodeId;
            for (auto &m : dataGetter(p))
                clsstream << " " << m;
            clsstream << "\n";
        }
    }

    f.close();
    return true;
}
