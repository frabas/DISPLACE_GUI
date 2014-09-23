#include "scenario.h"
#include <readdata.h>
#include <exceptions.h>

#include <QFile>
#include <QTextStream>


Scenario::Scenario()
{
}

QStringList Scenario::getDyn_alloc_sce() const
{
    return dyn_alloc_sce;
}

void Scenario::setDyn_alloc_sce(const QStringList &value)
{
    dyn_alloc_sce = value;
}

QStringList Scenario::getDyn_pop_sce() const
{
    return dyn_pop_sce;
}

void Scenario::setDyn_pop_sce(const QStringList &value)
{
    dyn_pop_sce = value;
}

QString Scenario::getBiolsce() const
{
    return biolsce;
}

void Scenario::setBiolsce(const QString &value)
{
    biolsce = value;
}

int Scenario::getGraph() const
{
    return graph;
}

void Scenario::setGraph(int value)
{
    graph = value;
}

int Scenario::getNrow_coord() const
{
    return nrow_coord;
}

void Scenario::setNrow_coord(int value)
{
    nrow_coord = value;
}

int Scenario::getNrow_graph() const
{
    return nrow_graph;
}

void Scenario::setNrow_graph(int value)
{
    nrow_graph = value;
}

int Scenario::getA_port() const
{
    return a_port;
}

void Scenario::setA_port(int value)
{
    a_port = value;
}

double Scenario::getGraph_res() const
{
    return graph_res;
}

void Scenario::setGraph_res(double value)
{
    graph_res = value;
}

bool Scenario::save(QString path, QString modelname, QString outputname)
{
    QString realpath = path + "/simusspe_" + modelname +"/" + outputname + ".dat";
    QFile file (realpath);

    if (!file.open(QFile::WriteOnly))
        return false;

    QTextStream stream (&file);

    stream << "# dyn_alloc_sce\n";
    foreach (QString a, dyn_alloc_sce)
        stream << a << " ";
    stream << endl;

    stream <<"# dyn_pop_sce\n";
    foreach (QString a, dyn_pop_sce)
        stream << a << " ";
    stream << endl;

    stream << "# biolsce\n" << biolsce << "\n";
    stream << "# a_graph\n" << graph << "\n";
    stream << "# nrow_coord\n" << nrow_coord << "\n";
    stream << "# nrow_graph\n" << nrow_graph << "\n";
    stream << "# a_port\n" << a_port << "\n";
    stream << "# grid res km\n" << graph_res << "\n";

    file.close();
    return true;
}

Scenario Scenario::readFromFile(QString path, QString modelname, QString outputname)
{
    // scenarios for dynamic allocation of effort and biol sce
    vector<string> dyn_alloc_sce;
    vector<string> dyn_pop_sce;
    string biolsce;				 // default is 1
    int a_graph;
    int a_port;
    int nrow_coord;
    int nrow_graph;
    double graph_res;

    if (read_scenario_config_file (        
        path.toStdString(),
        modelname.toStdString(),
        outputname.toStdString(),
        dyn_alloc_sce,
        dyn_pop_sce,
        biolsce,
        a_graph,
        nrow_coord,
        nrow_graph,
        a_port,
        graph_res
        ) < 0)
        throw DisplaceException(QString(QObject::tr("Cannot load scenario file: %1 - %2"))
                                .arg(::getLastErrorMessage().c_str())
                                .arg(strerror(errno)));

    Scenario s;

    QStringList alsce;
    for (vector<string>::iterator it = dyn_alloc_sce.begin(); it != dyn_alloc_sce.end(); ++it)
        alsce << it->c_str();
    QStringList popsce;
    for (vector<string>::iterator it = dyn_pop_sce.begin(); it != dyn_pop_sce.end(); ++it)
        popsce << it->c_str();

    s.setDyn_alloc_sce(alsce);
    s.setDyn_pop_sce(popsce);
    s.setBiolsce(QString(biolsce.c_str()));
    s.setGraph(a_graph);
    s.setNrow_coord(nrow_coord);
    s.setNrow_graph(nrow_graph);
    s.setGraph_res(graph_res);
    s.setA_port(a_port);

    return s;
}
