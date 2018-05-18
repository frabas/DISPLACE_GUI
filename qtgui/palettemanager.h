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
#ifndef PALETTEMANAGER_H
#define PALETTEMANAGER_H

#include <QString>
#include <QVector>
#include <QColor>
#include <QMap>
#include <QList>
#include <QIODevice>
#include <QVariant>
#include <cmath>
#include <memory>

enum PaletteRole {
    ValueRole = 0, PopulationRole, TariffsRole, BenthosRole, FishfarmRole, BathyRole,
    SalinityRole, SSTRole, WindRole, NitrogenRole, PhosphorusRole, OxygenRole, DissolvedCarbonRole,

    LastRole
};


class Palette {
public:
    typedef QMap<double,QColor> Container;
    typedef Container::const_iterator Iterator;

    Palette();
    explicit Palette(PaletteRole role, const QString &name);
    Palette(const Palette &);
    ~Palette();

    // Role
    PaletteRole role() const { return mRole; }
    void setRole (PaletteRole r) { mRole = r; }

    // Name
    void setName (const QString &n) { m_name = n; }
    QString name() const { return m_name; }

    // Palette Colors
    int colorCount() const { return m_palette.count(); }

    Iterator begin() const { return m_palette.cbegin(); }
    Iterator end() const { return m_palette.cend(); }
    void addColor (double value, const QColor &col) { m_palette.insert(value, col); }

    const QColor &color(double val) const {
        Iterator it = m_palette.lowerBound(val);
        if (it != m_palette.begin())
            --it;
        return *it;
    }

    const QColor &colorByIndex(int idx) const {
        Iterator it = m_palette.begin();
        it += (idx % m_palette.size());
        return *it;
    }

    // Palette Parameters

    // Palette Special Color
    int specialColorCount() const { return mSpecials.size(); }
    QColor specialColor(int i) const { return mSpecials[i]; }
    const QVector<QColor> &specialColors() const { return mSpecials; }

    void setSpecialColor (int i, const QColor &col) { mSpecials[i] = col; }
    void addSpecialColor (const QColor &col) { mSpecials.push_back(col); }
    void setNumSpecialColor (int i);

    // Reset
    void clear() {
        m_palette.clear();
        mSpecials.clear();
    }

    // I/O
    bool loadFromFile(QIODevice *device);
    bool saveToFile (QIODevice *device);

protected:
    PaletteRole mRole;
    QString m_name;
    Container m_palette;
    QVector<QColor> mSpecials;

    double m_min, m_max;
};

class PaletteManager
{
public:
    PaletteManager();
    ~PaletteManager();

    const Palette &palette(PaletteRole) const;
    std::shared_ptr<Palette> palette(const QString &n) const;
    void setPalette(PaletteRole n, const Palette &palette);

    int paletteCount() const;

    bool isPalettePresent (const QString &name) const;
    void removePalette (const QString &name);
    void removePalette (const Palette &view);

    static PaletteManager *instance() {
        if (mInstance == 0)
            mInstance = new PaletteManager();
        return mInstance;
    }

protected:
    typedef QMap<QString, std::shared_ptr<Palette> > PaletteMapContainer;
    typedef QList<std::shared_ptr<Palette> > PaletteListContainer;

    PaletteMapContainer m_map;
    PaletteListContainer m_list;

    static PaletteManager *mInstance;
    static const QString defaultPaletteFileNames[];
};

#endif // PALETTEMANAGER_H
