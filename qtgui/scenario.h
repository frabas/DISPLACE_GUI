#ifndef SCENARIO_H
#define SCENARIO_H

#include <QString>
#include <QList>
#include <QStringList>

class Scenario
{
private:
    QStringList dyn_alloc_sce;
    QStringList dyn_pop_sce;
    QString biolsce;
    int graph;
    int nrow_coord;
    int nrow_graph;
    int a_port;
    double graph_res;

public:
    Scenario();

    QStringList getDyn_alloc_sce() const;
    void setDyn_alloc_sce(const QStringList &value);
    QStringList getDyn_pop_sce() const;
    void setDyn_pop_sce(const QStringList &value);
    QString getBiolsce() const;
    void setBiolsce(const QString &value);
    int getGraph() const;
    void setGraph(int value);
    int getNrow_coord() const;
    void setNrow_coord(int value);
    int getNrow_graph() const;
    void setNrow_graph(int value);
    int getA_port() const;
    void setA_port(int value);
    double getGraph_res() const;
    void setGraph_res(double value);

    bool save (QString path, QString modelname, QString outputname);

    static Scenario readFromFile (QString path, QString modelname, QString outputname);
};

#endif // SCENARIO_H
