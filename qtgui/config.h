#ifndef CONFIG_H
#define CONFIG_H

#include <QList>
#include <QString>

class Config
{
    int nbpops;

    QList <int> m_implicit_pops;
    QList <double> m_calib_oth_landings;
    QList <double> m_calib_weight_at_szgroup;
    QList <double> m_calib_cpue_multiplier;

public:
    Config();

    bool save (QString path, QString modelname, QString outputname);

    static Config readFromFile (QString path, QString modelname, QString outputname);

    int getNbpops() const;
    void setNbpops(int value);
    const QList<int> &implicit_pops() const;
    void setImplicit_pops(const QList<int> &implicit_pops);
    const QList<double> &calib_oth_landings() const;
    void setCalib_oth_landings(const QList<double> &calib_oth_landings);
    const QList<double> &calib_weight_at_szgroup() const;
    void setCalib_weight_at_szgroup(const QList<double> &calib_weight_at_szgroup);
    const QList<double> &calib_cpue_multiplier() const;
    void setCalib_cpue_multiplier(const QList<double> &calib_cpue_multiplier);
};

#endif // CONFIG_H
