#include "scenario.h"
#include <readdata.h>
#include <exceptions.h>
#include <options.h>

#include <QFile>
#include <QTextStream>


Scenario::Scenario()
    : dyn_alloc_sce("baseline"),
      dyn_pop_sce("baseline"),
      biolsce("1"),
      graph(0),
      nrow_coord(0),
      nrow_graph(0),
      a_port(0),
      graph_res(10)
{
}

QStringList Scenario::getDyn_alloc_sce() const
{
    return dyn_alloc_sce;
}

std::vector<string> Scenario::getDyn_alloc_sce_asVector() const
{
    std::vector<string> vec;
    foreach (QString s, dyn_alloc_sce)
        vec.push_back(s.toStdString());
    return vec;
}

void Scenario::setDyn_alloc_sce(const QStringList &value)
{
    dyn_alloc_sce = value;
}

QStringList Scenario::getDyn_pop_sce() const
{
    return dyn_pop_sce;
}

std::vector<string> Scenario::getDyn_pop_sce_asVector() const
{
    std::vector<string> vec;
    foreach (QString s, dyn_pop_sce)
        vec.push_back(s.toStdString());
    return vec;
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

bool Scenario::save(QString path, QString modelname, QString outputname, QString *error)
{
    QString realpath = path + "/simusspe_" + modelname +"/" + outputname + ".dat";
    QFile file (realpath);

    if (!file.open(QFile::WriteOnly | QFile::Text)) {
        if (error) *error = file.errorString();
        return false;
    }

    QTextStream stream (&file);

    stream << "# dyn_alloc_sce"<< endl;
    foreach (QString a, dyn_alloc_sce)
        stream << a << " ";
    stream << endl;

    stream <<"# dyn_pop_sce"<< endl;
    foreach (QString a, dyn_pop_sce)
        stream << a << " ";
    stream << endl;

    stream << "# biolsce\n" << biolsce << endl;
    stream << "# a_graph\n" << graph << endl;
    stream << "# nrow_coord\n" << nrow_coord << endl;
    stream << "# nrow_graph\n" << nrow_graph << endl;
    stream << "# a_port\n" << a_port << endl;
    stream << "# grid res km\n" << graph_res << endl;

    file.close();
    return true;
}

Scenario Scenario::readFromFile(QString path, QString modelname, QString outputname)
{
    // scenarios for dynamic allocation of effort and biol sce
    DynAllocOptions dyn_alloc_sce;
    PopSceOptions dyn_pop_sce;
    string biolsce;				 // default is 1
    int a_graph;
    int a_port;
    int nrow_coord;
    int nrow_graph;
    double graph_res = 0.0;

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

    QStringList alsce = QString::fromStdString(dyn_alloc_sce.toString()).split(" ", QString::SkipEmptyParts);
    QStringList popsce = QString::fromStdString(dyn_pop_sce.toString()).split(" ", QString::SkipEmptyParts);

    s.setDyn_alloc_sce(alsce);
    s.setDyn_pop_sce(popsce);
    s.setBiolsce(QString(biolsce.c_str()));
    s.setGraph(a_graph);
    s.setNrow_coord(nrow_coord);
    s.setNrow_graph(nrow_graph);
    if (graph_res > 1e-3)
        s.setGraph_res(graph_res);
    s.setA_port(a_port);

    return s;
}
