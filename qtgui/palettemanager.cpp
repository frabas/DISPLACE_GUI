#include "palettemanager.h"
#include <QDomDocument>
#include <QTextStream>
#include <QDebug>

Palette::Palette()
    : mRole(StandardRole), m_name(""), m_palette()
{
}

Palette::Palette(Role role, const QString &name)
    : mRole(role), m_name(name),
      m_palette()
{
}

Palette::~Palette()
{
}

void Palette::setNumColor(int i)
{
    while (m_palette.size() < i)
        m_palette.push_back(QColor());
    while (m_palette.size() > i)
        m_palette.pop_back();
}

void Palette::setStep(float s)
{
    int n = 2 + std::ceil((m_max - m_min) / s);
    qDebug() << "Set" << m_min << m_max << s << n;
    setNumColor(n);
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
    Role r = static_cast<Role> (root.attribute("role").toInt(&ok));
    if (!ok) return false;

    double m = root.attribute("min").toFloat(&ok);
    if (!ok) return false;

    double M = root.attribute("max").toFloat(&ok);
    if (!ok) return false;

    QVector<QColor> clist, slist;

    QDomNode nd = root.firstChild();
    while (!nd.isNull()) {
        QDomElement col = nd.toElement();
        if (col.tagName() == "color") {
            QString cs = col.attribute("rgb");
            QStringList components = cs.split(QChar(','));
            if (components.size() == 3) {
                QColor color (components[0].toInt(), components[1].toInt(), components[2].toInt());
                clist.push_back(color);
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
    m_palette = clist;
    mSpecials = slist;
    m_min = m;
    m_max = M;
    return true;
}

bool Palette::saveToFile(QIODevice *device)
{
    QDomDocument doc;
    QDomElement root = doc.createElement("palette");
    root.setAttribute("type", "displace");
    root.setAttribute("name", m_name);
    root.setAttribute("role", static_cast<int>(mRole));
    root.setAttribute("min", m_min);
    root.setAttribute("max", m_max);
    doc.appendChild(root);

    foreach (QColor col, m_palette) {
        QDomElement ce = doc.createElement("color");
        ce.setAttribute("rgb",
                        QString("%1,%2,%3")
                        .arg(col.red())
                        .arg(col.green())
                        .arg(col.blue()));

        root.appendChild(ce);
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
}

PaletteManager::~PaletteManager()
{

}

void PaletteManager::addPalette(const Palette &palette)
{
    std::shared_ptr<Palette> v (new Palette(palette));

    m_map.insert(palette.name(), v);
    m_list.push_back(v);
}

const Palette &PaletteManager::palette(int n) const
{
    if (n < m_list.size())
        return *(m_list[n].get());
    return Palette();
}

const Palette &PaletteManager::palette(const QString &n) const
{
    PaletteMapContainer::const_iterator it = m_map.find(n);
    if (it != m_map.end()) {
        return *(*it);
    }
    return Palette();
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
