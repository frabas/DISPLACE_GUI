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

/* --------------------------------------------------------------------------
 * DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
 * AND EFFORT DISPLACEMENT
 * Copyright (c) 2012, 2013, 2014 Francois Bastardie <fba@aqua.dtu.dk>
 *
 *    This program is free software; you can redistribute it and/or modify
 *    it under the terms of the GNU General Public License as published by
 *    the Free Software Foundation; either version 2 of the License, or
 *    (at your option) any later version.
 *
 *    This program is distributed in the hope that it will be useful,
 *    but WITHOUT ANY WARRANTY; without even the implied warranty of
 *    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *    GNU General Public License for more details.
 *
 *    You should have received a copy of the GNU General Public License along
 *    with this program; if not, write to the Free Software Foundation, Inc.,
 *    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * --------------------------------------------------------------------------
 */
#ifndef OUTPUTFILEPARSER_H
#define OUTPUTFILEPARSER_H

#include <QObject>
#include <QString>
#include <QFile>

#include <memory>

class DisplaceModel;
class VesselStats;

class OutputFileParser : public QObject
{
    Q_OBJECT

    DisplaceModel *mModel;

public:
    explicit OutputFileParser(DisplaceModel *model, QObject *parent = 0);

public slots:
    void parse (QString path, int tstep, int period = -1);

signals:
    void error(QString);
    void parseCompleted();

protected:
    void parsePopStart(QFile *file, int tstep, DisplaceModel *model, int period = -1);
    void parsePopCumftime(QFile *file, int tstep, DisplaceModel *model, int period = -1);
    void parsePopCumsweptarea(QFile *file, int tstep, DisplaceModel *model, int period = -1);
    void parsePopCumcatches(QFile *file, int tstep, DisplaceModel *model, int period = -1);
    void parsePopTariffs(QFile *file, int tstep, DisplaceModel *model, int period = -1);
    void parsePopImpact(QFile *file, int tstep, DisplaceModel *model, int period = -1);
    void parsePopCumcatchesPerPop(QFile *file, int tstep, DisplaceModel *model, int period = -1);
    void parsePopBenthosBiomass(QFile *file, int tstep, DisplaceModel *model, int period = -1);
    void parsePopBenthosNumber(QFile *file, int tstep, DisplaceModel *model, int period = -1);
    void parsePopdynF(QFile *file, int tstep, DisplaceModel *model, int period = -1);
    void parsePopdynSSB(QFile *file, int tstep, DisplaceModel *model, int period = -1);
    void parsePopdyn(QFile *file, int tstep, DisplaceModel *model, int period = -1);
    void parseVessels(QFile *file, int tstep, DisplaceModel *model, int period = -1);

    static int toInt(const QString x) {
        bool b;
        int r = x.toInt(&b);
        if (!b)
            throw std::exception();
        return r;
    }

    static double toDouble(const QString x) {
        bool b;
        double r = x.toDouble(&b);
        if (!b)
            throw std::exception();
        return r;
    }

public:
    VesselStats parseVesselStatLine(const QStringList &fields);
};

#endif // OUTPUTFILEPARSER_H
