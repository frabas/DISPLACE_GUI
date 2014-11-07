#ifndef PALETTEMANAGER_H
#define PALETTEMANAGER_H

#include <QString>
#include <QVector>
#include <QColor>
#include <QMap>
#include <QList>
#include <QVariant>
#include <cmath>
#include <memory>

enum PaletteRole {
    ValueRole = 0, PopulationRole,

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

    QColor color(double val) const {
        Iterator it = m_palette.lowerBound(val);
        if (it != m_palette.begin())
            --it;
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
