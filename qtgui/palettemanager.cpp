#include "palettemanager.h"
#include <QDomDocument>
#include <QTextStream>
#include <QDebug>
#include <QFile>

PaletteManager *PaletteManager::mInstance = 0;

const QString PaletteManager::defaultPaletteFileNames[] = {
    ":/palettes/iso1996_2.p2c",
    ":/palettes/pop_colors.p2c"
};

Palette::Palette()
    : mRole(ValueRole), m_name(""), m_palette()
{
}

Palette::Palette(PaletteRole role, const QString &name)
    : mRole(role), m_name(name),
      m_palette()
{
}

Palette::Palette(const Palette &p)
    : m_name(p.m_name),
      m_palette(p.m_palette),
      mSpecials(p.mSpecials),
      m_min(p.m_min), m_max(p.m_max)
{
}

Palette::~Palette()
{
}

void Palette::setNumSpecialColor (int i)
{
    while (mSpecials.size() < i)
        mSpecials.push_back(QColor());
    while (mSpecials.size() > i)
        mSpecials.pop_back();
}

bool Palette::loadFromFile(QIODevice *device)
{
    QDomDocument doc;
    QString err;
    int line, col;
    if (!doc.setContent(device, &err, &line, &col)) {
        return false;   // ---
    }

    bool ok;
    QDomElement root = doc.documentElement();
    QString n = root.attribute("name");
    PaletteRole r = static_cast<PaletteRole> (root.attribute("role").toInt(&ok));
    if (!ok) return false;

    QVector<QColor> slist;
    Container cols;

    QDomNode nd = root.firstChild();
    while (!nd.isNull()) {
        QDomElement col = nd.toElement();
        if (col.tagName() == "color") {
            QString val = col.attribute("value");
            double dv = val.toDouble(&ok);
            if (!ok) return false;

            QString cs = col.attribute("rgb");
            QStringList components = cs.split(QChar(','));
            if (components.size() == 3) {
                QColor color (components[0].toInt(), components[1].toInt(), components[2].toInt());
                cols.insert(dv,color);
            } else {
                return false;
            }
        } else if (col.tagName() == "special") {
            QString cs = col.attribute("rgb");
            QStringList components = cs.split(QChar(','));
            if (components.size() == 3) {
                QColor color (components[0].toInt(), components[1].toInt(), components[2].toInt());
                slist.push_back(color);
            } else {
                return false;
            }
        }
        nd = nd.nextSibling();
    }


    // other things...

    mRole = r;
    m_name = n;
    m_palette = cols;
    mSpecials = slist;
    return true;
}

bool Palette::saveToFile(QIODevice *device)
{
    QDomDocument doc;
    QDomElement root = doc.createElement("palette");
    root.setAttribute("type", "displace");
    root.setAttribute("name", m_name);
    root.setAttribute("role", static_cast<int>(mRole));
    doc.appendChild(root);

    Iterator it = begin();
    while (it != end()) {
        QDomElement ce = doc.createElement("color");
        ce.setAttribute("value", it.key());
        ce.setAttribute("rgb",
                        QString("%1,%2,%3")
                        .arg(it->red())
                        .arg(it->green())
                        .arg(it->blue()));

        root.appendChild(ce);
        ++it;
    }
    foreach (QColor col, mSpecials) {
        QDomElement ce = doc.createElement("special");
        ce.setAttribute("rgb",
                        QString("%1,%2,%3")
                        .arg(col.red())
                        .arg(col.green())
                        .arg(col.blue()));

        root.appendChild(ce);
    }

    QTextStream stream (device);
    stream.setCodec("UTF-8");

    doc.save(stream, 0, QDomNode::EncodingFromTextStream);
    return true;
}

// --- Palette manager

PaletteManager::PaletteManager()
{
    for (int i = 0; i < LastRole; ++i) {
        std::shared_ptr<Palette> p (new Palette());
        QFile pf(defaultPaletteFileNames[i]);
        Q_ASSERT(p->loadFromFile(&pf));
        m_list.append(p);
    }
}

PaletteManager::~PaletteManager()
{

}

const Palette &PaletteManager::palette(PaletteRole n) const
{
    return *(m_list[(int)n].get());
}

std::shared_ptr<Palette> PaletteManager::palette(const QString &n) const
{
    PaletteMapContainer::const_iterator it = m_map.find(n);
    if (it != m_map.end()) {
        return (*it);
    }
    return std::shared_ptr<Palette>();
}

void PaletteManager::setPalette(PaletteRole n, const Palette &palette)
{
    m_list[n] = std::shared_ptr<Palette> (new Palette(palette));
    m_map.remove(palette.name());
    m_map.insert(palette.name(), m_list[n]);
}

int PaletteManager::paletteCount() const
{
    return m_list.size();
}

bool PaletteManager::isPalettePresent (const QString &name) const
{
    return (m_map.find(name) != m_map.end());
}

void PaletteManager::removePalette (const QString &name)
{
    m_map.remove(name);

    int n = m_list.size();
    for (int i = 0; i < n; ++i) {
        if (m_list[i]->name() == name) {
            m_list.removeAt(i);
            --i;
            --n;
        }
    }
}

void PaletteManager::removePalette (const Palette &palette)
{
    removePalette(palette.name());
}


// eof
