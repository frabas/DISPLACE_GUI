// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012, 2013, 2014, 2015, 2016, 2017 Francois Bastardie <fba@aqua.dtu.dk>

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
#ifndef CONFIG_H
#define CONFIG_H

#include <idtypes.h>

#include <QList>
#include <QString>

class Config
{
    int nbpops;
    int nbbenthospops;
    int szGroups;

    QList <int> m_implicit_pops;
    QList <int> m_implicit_pops_level2;
    QList <int> m_grouped_tacs;
    QList <double> m_calib_oth_landings;
    QList <double> m_calib_weight_at_szgroup;
    QList <double> m_calib_cpue_multiplier;

public:
    QList <types::NodeId> m_interesting_harbours;

public:
    Config();

    bool save (QString path, QString modelname, QString outputname, QString *error);

    static Config readFromFile (QString path, QString modelname, QString outputname);

    int getNbpops() const;
    void setNbpops(int value);
    int getNbbenthospops() const;
    void setNbbenthospops(int value);
    const QList<int> &implicit_pops() const;
    const QList<int> &implicit_pops_level2() const;
    const QList<int> &grouped_tacs() const;
    void setImplicit_pops(const QList<int> &implicit_pops);
    void setImplicit_pops_level2(const QList<int> &implicit_pops_level2);
    void setGrouped_tacs(const QList<int> &grouped_tacs);
    const QList<double> &calib_oth_landings() const;
    void setCalib_oth_landings(const QList<double> &calib_oth_landings);
    const QList<double> &calib_weight_at_szgroup() const;
    void setCalib_weight_at_szgroup(const QList<double> &calib_weight_at_szgroup);
    const QList<double> &calib_cpue_multiplier() const;
    void setCalib_cpue_multiplier(const QList<double> &calib_cpue_multiplier);
    int getSzGroups() const;
    void setSzGroups(int value);
};

#endif // CONFIG_H
