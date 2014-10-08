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

class Palette {
public:
    enum Role {
        StandardRole
    };

    Palette();
    explicit Palette(Role role, const QString &name);
    Palette(const Palette &);
    ~Palette();

    // Role
    Role role() const { return mRole; }
    void setRole (Role r) { mRole = r; }

    // Name
    void setName (const QString &n) { m_name = n; }
    QString name() const { return m_name; }

    // Palette Colors
    int colorCount() const { return m_palette.count(); }

    void setColor (int i, const QColor &col) { m_palette[i] = col; }
    void addColor (const QColor &col) { m_palette.push_back(col); }

    QColor colorForIndex(int i) const { return m_palette[i]; }
    QColor color(float val) const {
        int idx = static_cast<int>(std::floor( (val - m_min) / getStep()) );
        if (idx < 0) idx = 0;
        if (idx > m_palette.count()-1) idx = m_palette.count()-1;
        return m_palette[ idx ];
    }
    const QVector<QColor> &colors() const { return m_palette; }

    // Palette Parameters
    float getMin() const { return m_min; }
    float getMax() const { return m_max; }
    float getStep() const {
        if (m_palette.count() >= 2)
            return (m_max - m_min) / (m_palette.count()-2);
        else
            return 0.0;     // non ha senso
    }

    void setMin(float m) { m_min = m; }
    void setMax(float m) { m_max = m; }
    void setNumColor (int i);
    void setStep (float s);

    // Palette Special Color
    int specialColorCount() const { return mSpecials.size(); }
    QColor specialColor(int i) const { return mSpecials[i]; }
    const QVector<QColor> &specialColors() const { return mSpecials; }

    void setSpecialColor (int i, const QColor &col) { mSpecials[i] = col; }
    void addSpecialColor (const QColor &col) { mSpecials.push_back(col); }
    void setNumSpecialColor (int i);

    // Reset
    void reset() {
        m_palette.clear();
        mSpecials.clear();
    }

    // I/O
    bool loadFromFile(QIODevice *device);
    bool saveToFile (QIODevice *device);

protected:
    Role mRole;
    QString m_name;
    QVector<QColor> m_palette;
    QVector<QColor> mSpecials;

    double m_min, m_max;
};

class PaletteManager
{
public:
    PaletteManager();
    ~PaletteManager();

    void addPalette(const Palette &view);
    const Palette &palette(int n) const;
    const Palette &palette(const QString &n) const;
    void setPalette(int n, const Palette &palette);

    int paletteCount() const;

    bool isPalettePresent (const QString &name) const;
    void removePalette (const QString &name);
    void removePalette (const Palette &view);

protected:
    typedef QMap<QString, std::shared_ptr<Palette> > PaletteMapContainer;
    typedef QList<std::shared_ptr<Palette> > PaletteListContainer;

    PaletteMapContainer m_map;
    PaletteListContainer m_list;
};

#endif // PALETTEMANAGER_H
