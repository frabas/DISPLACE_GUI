#include "displacemodel.h"
#include <exceptions.h>

#include <mapobjects/harbourmapobject.h>

#include <readdata.h>
#include <qdebug.h>

DisplaceModel::DisplaceModel()
{
}

bool DisplaceModel::load(QString path, QString modelname, QString outputname)
{
    qDebug() << "Loading model" << modelname << "from folder" << path;

    mName = modelname;
    mBasePath = path;
    mOutputName = outputname;

    /* Load files ... */

    try {
        mScenario = Scenario::readFromFile(mName, mBasePath, mOutputName);
        mConfig = Config::readFromFile(mName, mBasePath, mOutputName);

        loadNodes();
    } catch (DisplaceException &ex) {
        mLastError = ex.what();
        return false;
    }

    return true;
}

bool DisplaceModel::save()
{
    if (!mScenario.save(mName, mBasePath, mOutputName))
        return false;
    if (!mConfig.save(mName, mBasePath, mOutputName))
        return false;

    return true;
}

int DisplaceModel::getHarboursCount() const
{
    return mHarbours.size();
}

QString DisplaceModel::getHarbourId(int idx) const
{
    return QString::fromStdString(mHarbours.at(idx)->get_name());
}

int DisplaceModel::getNodesCount() const
{
    return mNodes.size();
}

QString DisplaceModel::getNodeId(int idx) const
{
    return QString::fromStdString(mNodes.at(idx)->get_name());
}

Scenario DisplaceModel::scenario() const
{
    return mScenario;
}

void DisplaceModel::setScenario(const Scenario &scenario)
{
    mScenario = scenario;
}


/* Warn: copy and pasted from simulator's main.cpp */
#define NBSZGROUP 14

bool DisplaceModel::loadNodes()
{
    int nrow_coord = mScenario.getNrow_coord();
    int a_port = mScenario.getA_port();
    vector<string> dyn_alloc_sce = mScenario.getDyn_alloc_sce_asVector();
    int nbpops = mConfig.getNbpops();
    string a_quarter= "quarter1";// start quarter

    // input data, coord nodes of the graph
    ifstream coord_graph;

    // casting a_pop into a string
    stringstream out;
    out << mScenario.getGraph();
    string a_graph_s = out.str();
    string filename_graph = mBasePath.toStdString() +
            "/graphsspe/coord" + a_graph_s + ".dat";

    string filename_code_area_graph = mBasePath.toStdString() +
            "/graphsspe/code_area_for_graph" + a_graph_s + "_points.dat";

    string filename_code_marine_landscape_graph = mBasePath.toStdString() +
            "/graphsspe/coord" + a_graph_s + "_with_landscape.dat";

    coord_graph.open(filename_graph.c_str());
    if(coord_graph.fail()) {
        throw DisplaceException(QString(QObject::tr("Cannot load %1: %2"))
                                .arg(filename_graph.c_str())
                                .arg(strerror(errno)));
    }

    vector<double> graph_coord_x;
    vector<double> graph_coord_y;
    vector<int> graph_coord_harbour;
    fill_from_coord(coord_graph, graph_coord_x, graph_coord_y, graph_coord_harbour, nrow_coord);
    coord_graph.close();

    // input data, code area for each point of the graph (e.g. 1: NS, 2: BW, 3: BE, 10: open sea)
    ifstream code_area_graph;
    code_area_graph.open(filename_code_area_graph.c_str());
    if(code_area_graph.fail())
    {
        throw DisplaceException(QString(QObject::tr("Cannot load %1: %2"))
                                .arg(filename_code_area_graph.c_str())
                                .arg(strerror(errno)));
    }
    vector<int> graph_point_code_area;
    fill_from_code_area(code_area_graph, graph_point_code_area, nrow_coord);

    // input data, for the marine landscape for each point of the graph (e.g. 111, 112, etc. e.g. see the BALANCE map coding)
    ifstream code_landscape_graph;
    code_landscape_graph.open(filename_code_marine_landscape_graph.c_str());
    if(code_landscape_graph.fail())
    {
        throw DisplaceException(QString(QObject::tr("Cannot load %1: %2"))
                                .arg(filename_code_marine_landscape_graph.c_str())
                                .arg(strerror(errno)));
    }
    vector<int> graph_point_code_landscape;
    fill_from_code_marine_landscape(code_landscape_graph, graph_point_code_landscape, nrow_coord);

    // read harbour specific files
    multimap<int, string> harbour_names = read_harbour_names(mName.toStdString(), mBasePath.toStdString());

    // creation of a vector of nodes from coord
    // and check with the coord in input.
    // use inheritance i.e. a Harbour is child of a Node
    // use polymorphism i.e. store either Harbour or Node in the vector of pointers 'nodes'

//    vector <Node* > nodes(graph_coord_x.size());
    // REPLACE boost::shared_ptr<Node> by Node* in every files if you want to remove boost

    for (unsigned int i=0; i<graph_coord_x.size(); i++)
    {
        if(graph_coord_harbour[i])
        {
            string a_name="none";
            int a_point=0;
            // get the name of this harbour
            multimap<int,string>::iterator lower_g = harbour_names.lower_bound(i);
            multimap<int,string>::iterator upper_g = harbour_names.upper_bound(i);
            for (multimap<int, string>::iterator pos=lower_g; pos != upper_g; pos++)
            {
                a_point= pos->first;
                a_name= pos->second;
            }

            map<string,double> init_fuelprices;
            multimap<int, double> fishprices_each_species_per_cat;
            if(a_name!="none" && a_point== i)
            {

                cout << "load prices for port " << a_name << " which is point " << a_point << endl;
                //int er = read_prices_per_harbour(a_point, a_quarter, prices, folder_name_parameterization);
                int er2 = read_prices_per_harbour_each_pop_per_cat(a_point,  a_quarter, fishprices_each_species_per_cat, mName.toStdString(), mBasePath.toStdString());
                // if not OK then deadly bug: possible NA or Inf in harbour files need to be checked (step 7)
                cout << "....OK" << endl;
            }
            else
            {

                cout << a_point << " : harbour not found in the harbour names (probably because no declared landings from studied vessels in those ports)" << endl;
                //int er = read_prices_per_harbour(a_port, "quarter1", prices, folder_name_parameterization); // delete later on when final parameterisation
                int er2 = read_prices_per_harbour_each_pop_per_cat(a_port, "quarter1", fishprices_each_species_per_cat, mName.toStdString(), mBasePath.toStdString());

            }

            // read fuel price (vessel size dependent for the time being)
            if (!binary_search (dyn_alloc_sce.begin(), dyn_alloc_sce.end(),
                                "fuelprice_plus20percent"))
            {
                read_fuel_prices_per_vsize(init_fuelprices, mName.toStdString(), mBasePath.toStdString());
            }
            else
            {
                read_fuel_prices_per_vsize(init_fuelprices, mName.toStdString(), mBasePath.toStdString());

                map<string,double>::iterator pos;
                for (pos=init_fuelprices.begin(); pos != init_fuelprices.end(); pos++)
                {
                    pos->second = (pos->second)*1.2;
                }

                for (pos=init_fuelprices.begin(); pos != init_fuelprices.end(); pos++)
                {
                    cout << pos->first << " " << pos->second;
                }

            }

            Harbour *h = new Harbour(i,
                                       graph_coord_x[i],
                                       graph_coord_y[i],
                                       graph_coord_harbour[i],
                                       graph_point_code_area[i],
                                       graph_point_code_landscape[i],
                                       nbpops,
                                       NBSZGROUP,
                                       a_name,
                                       //prices,
                                       fishprices_each_species_per_cat,
                                       init_fuelprices
                                       );
            mHarbours.push_back(h);
            /*
            cout << "Harbour " <<  nodes[i]->get_name() << " " <<
                    nodes[i]->get_x() << " " << nodes[i]->get_y() << " " <<
                    nodes[i]->get_is_harbour()<< " " <<endl;*/
        }
        else
        {
            Node *n = new Node(i,
                                    graph_coord_x[i],
                                    graph_coord_y[i],
                                    graph_coord_harbour[i],
                                    graph_point_code_area[i],
                                    graph_point_code_landscape[i],
                                    nbpops,
                                    NBSZGROUP);

            mNodes.push_back(n);
            /*
            dout <<  nodes[i]->get_x() << " " << nodes[i]->get_y() << " " << nodes[i]->get_is_harbour()
                  << " " << nodes[i]->get_code_area() << endl;*/

        }
    }

    /* Not sure if we need this...
    // init
    for (unsigned int i=0; i< nodes.size(); i++)
    {
        nodes.at(i)->init_Ns_pops_at_szgroup(nbpops, NBSZGROUP);
        // caution:magic number...i.e. from the R poisson regression on cpue
        nodes.at(i)->init_avai_pops_at_selected_szgroup(nbpops,SEL_NBSZGROUP);
    }
    */

    return true;
}

