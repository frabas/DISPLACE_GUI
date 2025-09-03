//
// Created by happycactus on 12/28/19.
//

#include "TextfileModelLoader.h"
#include "TextImpl/Impl.h"
#include "helpers.h"
#include "readdata.h"
#include "SimModel.h"
#include "comstructs.h"
#include "Node.h"
#include "Harbour.h"
#include "Calendar.h"
#include "../simulator/values.h"
#include "shortestpath/GeoGraphLoader.h"
#include "shortestpath/GeoGraph.h"
#include "shortestpath/AStarShortestPathFinder.h"

TextfileModelLoader::TextfileModelLoader(std::shared_ptr<SimModel> model,
                                         std::string folder_name_parameterization,
                                         std::string inputfolder,
                                         std::string scenarioname)
        : ModelLoader(model),
          p(spimpl::make_unique_impl<Impl>())
{
    p->folder_name_parameterization = folder_name_parameterization;
    p->inputfolder = inputfolder;
    p->scenarioname = scenarioname;
}

/*
ModelLoader::BenthosData TextfileModelLoader::loadBenthosData(
        PopSceOptions const &dyn_pop_sce,
        DynAllocOptions const &dyn_alloc_sce,
        std::string const &biolsce,
        std::string const &fleetsce
)
{
    Dataloaderbenthos bl;
    bl.features(p->folder_name_parameterization,
                p->inputfolder,
                dyn_pop_sce,
                dyn_alloc_sce,
                biolsce,
                fleetsce);

    return bl.loadedData();
}*/

bool TextfileModelLoader::loadConfigImpl(int &nbpops,
                                         int& nbmets,
                                         int &nbbenthospops,
                                         std::vector<int> &implicit_pops,
                                         std::vector<int> &implicit_pops_level2,
                                         std::vector<int> &grouped_tacs,
                                         std::vector<int> &nbcp_coupling_pops,
                                         std::vector<double> &calib_oth_landings,
                                         std::vector<double> &calib_w,
                                         std::vector<double> &calib_cpue,
                                         std::vector<types::NodeId> &interesting_harbours)
{
    return read_config_file(nullptr,
                            p->folder_name_parameterization,
                            p->inputfolder,
                            nbpops,
                            nbmets,
                            nbbenthospops,
                            implicit_pops,
                            implicit_pops_level2,
                            grouped_tacs,
                            nbcp_coupling_pops,
                            calib_oth_landings,
                            calib_w,
                            calib_cpue,
                            interesting_harbours
    );
}

bool TextfileModelLoader::loadScenarioImpl(displace::commons::Scenario &scenario)
{
    return read_scenario_config_file(nullptr,
                                     p->folder_name_parameterization,
                                     p->inputfolder,
                                     p->scenarioname,
                                     scenario);
}

bool TextfileModelLoader::loadNodesAndGraphsDataImpl()
{
    auto a_graph_s = std::to_string(model().scenario().a_graph);

    // input data, coord nodes of the graph
    ifstream coord_graph;

    // casting a_pop into a string
    string filename_graph = p->inputfolder + "/graphsspe/coord" + a_graph_s + ".dat";
    string filename_code_area_graph = p->inputfolder + "/graphsspe/code_area_for_graph" + a_graph_s + "_points.dat";
    string filename_code_marine_landscape_graph =
            p->inputfolder + "/graphsspe/coord" + a_graph_s + "_with_landscape.dat";
    string filename_wind_graph = p->inputfolder + "/graphsspe/coord" + a_graph_s + "_with_wind.dat";
    string filename_sst_graph = p->inputfolder + "/graphsspe/coord" + a_graph_s + "_with_sst.dat";
    string filename_salinity_graph = p->inputfolder + "/graphsspe/coord" + a_graph_s + "_with_salinity.dat";
    string filename_Nitrogen_graph = p->inputfolder + "/graphsspe/coord" + a_graph_s + "_with_nitrogen.dat";
    string filename_Phosphorus_graph = p->inputfolder + "/graphsspe/coord" + a_graph_s + "_with_phosphorus.dat";
    string filename_Oxygen_graph = p->inputfolder + "/graphsspe/coord" + a_graph_s + "_with_oxygen.dat";
    string filename_DissolvedCarbon_graph =
            p->inputfolder + "/graphsspe/coord" + a_graph_s + "_with_dissolvedcarbon.dat";
    string filename_bathymetry_graph = p->inputfolder + "/graphsspe/coord" + a_graph_s + "_with_bathymetry.dat";
    string filename_shippingdensity_graph =
            p->inputfolder + "/graphsspe/coord" + a_graph_s + "_with_shippingdensity.dat";
    string filename_siltfraction_graph = p->inputfolder + "/graphsspe/coord" + a_graph_s + "_with_siltfraction.dat";
    string filename_icesrectanglecode_graph = p->inputfolder + "/graphsspe/coord" + a_graph_s + "_with_icesrectanglecode.dat";
    string filename_code_benthos_biomass_graph =
            p->inputfolder + "/graphsspe/coord" + a_graph_s + "_with_benthos_total_biomass.dat";
    string filename_code_benthos_number_graph =
            p->inputfolder + "/graphsspe/coord" + a_graph_s + "_with_benthos_total_number.dat";

    coord_graph.open(filename_graph.c_str());
    if (coord_graph.fail()) {
        open_file_error(filename_graph.c_str());
        return 1;
    }
    vector<double> graph_coord_x;
    vector<double> graph_coord_y;
    vector<int> graph_coord_harbour;

    auto nrow_coord = model().scenario().nrow_coord;
    if (!fill_from_coord(coord_graph, graph_coord_x, graph_coord_y, graph_coord_harbour, nrow_coord)) {
        std::cerr << "Cannot parse " << filename_graph << " Bad format\n";
        return 1;
    }
    coord_graph.close();

    // input data, code area for each point of the graph (e.g. 1: NS, 2: BW, 3: BE, 10: open sea)
    ifstream code_area_graph;
    code_area_graph.open(filename_code_area_graph);
    if (code_area_graph.fail()) {
        open_file_error(filename_code_area_graph);
        return 1;
    }
    vector<int> graph_point_code_area;
    if (!fill_from_code_area(code_area_graph, graph_point_code_area, nrow_coord)) {
        std::cerr << "Cannot parse " << filename_code_area_graph << " Bad format\n";
        return 1;
    }

    // input data, for the marine landscape for each point of the graph (e.g. 111, 112, etc. e.g. see the BALANCE map coding)
    ifstream code_landscape_graph;
    code_landscape_graph.open(filename_code_marine_landscape_graph);
    if (code_landscape_graph.fail()) {
        open_file_error(filename_code_marine_landscape_graph);
        return 1;
    }
    vector<int> v;
    if (!fill_from_code_marine_landscape(code_landscape_graph, v, nrow_coord)) {
        std::cerr << "Cannot parse " << filename_code_marine_landscape_graph << " Bad format\n";
        return 1;
    }
    model().set_graph_point_code_landscape(std::move(v));

    // input data, for the WIND for each point of the graph
    ifstream wind_graph;
    wind_graph.open(filename_wind_graph);
    if (wind_graph.fail()) {
        open_file_error(filename_wind_graph);
        return 1;
    }
    vector<double> graph_point_wind;
    if (!fill_from_wind(wind_graph, graph_point_wind, nrow_coord)) {
        std::cerr << "Cannot parse " << filename_wind_graph << " Bad format\n";
        return 1;
    }

    // input data, for the SST for each point of the graph
    ifstream sst_graph;
    sst_graph.open(filename_sst_graph);
    if (sst_graph.fail()) {
        open_file_error(filename_sst_graph);
        return 1;
    }
    vector<double> graph_point_sst;
    if (!fill_from_sst(sst_graph, graph_point_sst, nrow_coord)) {
        std::cerr << "Cannot parse " << filename_sst_graph << " Bad format\n";
        return 1;
    }

    // input data, for the SST for each point of the graph
    ifstream salinity_graph;
    salinity_graph.open(filename_salinity_graph);
    if (salinity_graph.fail()) {
        open_file_error(filename_salinity_graph);
        return 1;
    }
    vector<double> graph_point_salinity;
    if (!fill_from_salinity(salinity_graph, graph_point_salinity, nrow_coord)) {
        std::cerr << "Cannot parse " << filename_salinity_graph << " Bad format\n";
        return 1;
    }


    ifstream Nitrogen_graph;
    Nitrogen_graph.open(filename_Nitrogen_graph.c_str());
    if (Nitrogen_graph.fail()) {
        open_file_error(filename_Nitrogen_graph.c_str());
        return 1;
    }
    vector<double> graph_point_Nitrogen;
    if (!fill_from_Nitrogen(Nitrogen_graph, graph_point_Nitrogen, nrow_coord)) {
        std::cerr << "Cannot parse " << filename_Nitrogen_graph << " Bad format\n";
        return 1;
    }

    ifstream Phosphorus_graph;
    Phosphorus_graph.open(filename_Phosphorus_graph.c_str());
    if (Phosphorus_graph.fail()) {
        open_file_error(filename_Phosphorus_graph.c_str());
        return 1;
    }
    vector<double> graph_point_Phosphorus;
    if (!fill_from_Phosphorus(Phosphorus_graph, graph_point_Phosphorus, nrow_coord)) {
        std::cerr << "Cannot parse " << filename_Phosphorus_graph << " Bad format\n";
        return 1;
    }

    ifstream Oxygen_graph;
    Oxygen_graph.open(filename_Oxygen_graph.c_str());
    if (Oxygen_graph.fail()) {
        open_file_error(filename_Oxygen_graph.c_str());
        return 1;
    }
    vector<double> graph_point_Oxygen;
    if (!fill_from_Oxygen(Oxygen_graph, graph_point_Oxygen, nrow_coord)) {
        std::cerr << "Cannot parse " << filename_Oxygen_graph << " Bad format\n";
        return 1;
    }

    ifstream DissolvedCarbon_graph;
    DissolvedCarbon_graph.open(filename_DissolvedCarbon_graph.c_str());
    if (DissolvedCarbon_graph.fail()) {
        open_file_error(filename_DissolvedCarbon_graph.c_str());
        return 1;
    }
    vector<double> graph_point_DissolvedCarbon;
    if (!fill_from_DissolvedCarbon(DissolvedCarbon_graph, graph_point_DissolvedCarbon, nrow_coord)) {
        std::cerr << "Cannot parse " << filename_DissolvedCarbon_graph << " Bad format\n";
        return 1;
    }

    ifstream bathymetry_graph;
    bathymetry_graph.open(filename_bathymetry_graph.c_str());
    if (bathymetry_graph.fail()) {
        open_file_error(filename_bathymetry_graph.c_str());
        return 1;
    }
    vector<double> graph_point_bathymetry;
    if (!fill_from_bathymetry(bathymetry_graph, graph_point_bathymetry, nrow_coord)) {
        std::cerr << "Cannot parse " << filename_bathymetry_graph << " Bad format\n";
        return 1;
    }

    ifstream shippingdensity_graph;
    shippingdensity_graph.open(filename_shippingdensity_graph.c_str());
    if (shippingdensity_graph.fail()) {
        open_file_error(filename_shippingdensity_graph.c_str());
        return 1;
    }
    vector<double> graph_point_shippingdensity;
    if (!fill_from_shippingdensity(shippingdensity_graph, graph_point_shippingdensity, nrow_coord)) {
        std::cerr << "Cannot parse " << filename_shippingdensity_graph << " Bad format\n";
        return 1;
    }

    ifstream siltfraction_graph;
    siltfraction_graph.open(filename_siltfraction_graph.c_str());
    if (siltfraction_graph.fail()) {
        open_file_error(filename_siltfraction_graph.c_str());
        return 1;
    }
    vector<double> graph_point_siltfraction;
    if (!fill_from_siltfraction(siltfraction_graph, graph_point_siltfraction, nrow_coord)) {
        std::cerr << "Cannot parse " << filename_siltfraction_graph << " Bad format\n";
        return 1;
    }
   
    
    ifstream icesrectanglecode_graph;
    icesrectanglecode_graph.open(filename_icesrectanglecode_graph.c_str());
    if (icesrectanglecode_graph.fail()) {
       // open_file_error(filename_icesrectanglecode_graph.c_str());
       // return 1;
    }
    vector<double> graph_point_icesrectanglecode(graph_coord_x.size(), 0);
    if (!fill_from_icesrectanglecode(icesrectanglecode_graph, graph_point_icesrectanglecode, nrow_coord)) {
        std::cout << "Caution: cannot parse " << filename_icesrectanglecode_graph << " File does not exist, or bad format\n";
    }

    vector<double> graph_point_landscape_norm(graph_coord_x.size(), 0);
    vector<double> graph_point_landscape_alpha(graph_coord_x.size(), 0);
    vector<double> graph_point_wind_norm(graph_coord_x.size(), 0);
    vector<double> graph_point_wind_alpha(graph_coord_x.size(), 0);
    vector<double> graph_point_sst_norm(graph_coord_x.size(), 0);
    vector<double> graph_point_sst_alpha(graph_coord_x.size(), 0);
    vector<double> graph_point_salinity_norm(graph_coord_x.size(), 0);
    vector<double> graph_point_salinity_alpha(graph_coord_x.size(), 0);
    vector<double> graph_point_Nitrogen_norm(graph_coord_x.size(), 0);
    vector<double> graph_point_Nitrogen_alpha(graph_coord_x.size(), 0);
    vector<double> graph_point_Phosphorus_norm(graph_coord_x.size(), 0);
    vector<double> graph_point_Phosphorus_alpha(graph_coord_x.size(), 0);
    vector<double> graph_point_Oxygen_norm(graph_coord_x.size(), 0);
    vector<double> graph_point_Oxygen_alpha(graph_coord_x.size(), 0);
    vector<double> graph_point_DissolvedCarbon_norm(graph_coord_x.size(), 0);
    vector<double> graph_point_DissolvedCarbon_alpha(graph_coord_x.size(), 0);


    // input data, for the benthos total BIOMASS for each point of the graph
    ifstream benthos_biomass_graph;
    benthos_biomass_graph.open(filename_code_benthos_biomass_graph.c_str());
    if (benthos_biomass_graph.fail()) {
        open_file_error(filename_code_benthos_biomass_graph.c_str());
        return 1;
    }
    vector<double> graph_point_benthos_biomass;
    if (!fill_from_benthos_biomass(benthos_biomass_graph, graph_point_benthos_biomass, nrow_coord)) {
        std::cerr << "Cannot parse " << filename_code_benthos_biomass_graph << " Bad format\n";
        return 1;
    }

    // input data, for the benthos total NUMBER for each point of the graph
    ifstream benthos_number_graph;
    benthos_number_graph.open(filename_code_benthos_number_graph.c_str());
    if (benthos_number_graph.fail()) {
        open_file_error(filename_code_benthos_number_graph.c_str());
        return 1;
    }
    vector<double> graph_point_benthos_number;
    if (!fill_from_benthos_number(benthos_number_graph, graph_point_benthos_number, nrow_coord)) {
        std::cerr << "Cannot parse " << filename_code_benthos_number_graph << " Bad format\n";
        return 1;
    }


    auto const &dyn_pop_sce = model().scenario().dyn_pop_sce;

    // overwriting for GRAPH ENVT FORCING VARIABLES
    if (dyn_pop_sce.option(Options::includeForcingLayers)) {
        cout << "import environmental variables in one shoot..." << "\n";
        const string separator = ",";
        string filename = p->inputfolder + "/graphsspe/environment_on_coord" + a_graph_s + ".dat";

        ifstream is;
        is.open(filename.c_str());
        if (is.fail()) {
            cout << "Fail to open the file " << filename << "\n";
            open_file_error(filename);
            return false;
        }

        std::vector<EnvironmentDataRecord> environment_on_coord;
        bool r = read_environment_on_coord(is, separator, environment_on_coord);

        //"x" 0                    "y" 1                    "harb" 2
        //"pt_graph" 3             "code_area" 4
        //"landscapes_code" 5      "landscape_norm" 6       "landscape_alpha" 7
        //"wind" 8                 "wind_norm" 9            "wind_alpha" 10
        //"sst" 11                 "sst_norm" 12            "sst_alpha" 13
        //"salinity" 14            "salinity_norm" 15       "salinity_alpha"  16
        //"nitrogen" 17            "nitrogen_norm"   18     "nitrogen_alpha" 19
        //"phosphorus" 20          "phosphorus_norm" 21     "phosphorus_alpha" 22
        //"oxygen" 23              "oxygen_norm" 24         "oxygen_alpha" 25
        //"dissolvedcarbon" 26      "dissolvedcarbon_norm" 27  "dissolvedcarbon_alpha" 28
        //"bathymetry" 29
        //"shippingdensity" 30
        //"siltfraction" 31 
        //"icesrect" 32

        cout << "environment_on_coord.size() " << environment_on_coord.size() << "\n";
        for (unsigned int n = 0; n < environment_on_coord.size(); n++) {
            graph_coord_x.at(n) = environment_on_coord.at(n).x; // #0
            graph_coord_y.at(n) = environment_on_coord.at(n).y; // #1
            graph_coord_harbour.at(n) = environment_on_coord.at(n).harb; // #2
            graph_point_code_area.at(n) = environment_on_coord.at(n).code_area; // #4
            model().graph_point_code_landscape().at(n) = environment_on_coord.at(n).landscapes_code; // #5
            graph_point_landscape_norm.at(n) = environment_on_coord.at(n).landscape_norm;
            graph_point_landscape_alpha.at(n) = environment_on_coord.at(n).landscape_alpha;
            graph_point_wind.at(n) = environment_on_coord.at(n).wind; // #8
            graph_point_wind_norm.at(n) = environment_on_coord.at(n).wind_norm;
            graph_point_wind_alpha.at(n) = environment_on_coord.at(n).wind_alpha;
            graph_point_sst.at(n) = environment_on_coord.at(n).sst;  // #11
            graph_point_sst_norm.at(n) = environment_on_coord.at(n).sst_norm;
            graph_point_sst_alpha.at(n) = environment_on_coord.at(n).sst_alpha;
            graph_point_salinity.at(n) = environment_on_coord.at(n).salinity;  // #14
            graph_point_salinity_norm.at(n) = environment_on_coord.at(n).salinity_norm;
            graph_point_salinity_alpha.at(n) = environment_on_coord.at(n).salinity_alpha;
            graph_point_Nitrogen.at(n) = environment_on_coord.at(n).nitrogen;  // #17
            graph_point_Nitrogen_norm.at(n) = environment_on_coord.at(n).nitrogen_norm;
            graph_point_Nitrogen_alpha.at(n) = environment_on_coord.at(n).nitrogen_alpha;
            graph_point_Phosphorus.at(n) = environment_on_coord.at(n).phosphorus;  // #20
            graph_point_Phosphorus_norm.at(n) = environment_on_coord.at(n).phosphorus_norm;
            graph_point_Phosphorus_alpha.at(n) = environment_on_coord.at(n).phosphorus_alpha;
            graph_point_Oxygen.at(n) = environment_on_coord.at(n).oxygen;  // #23
            graph_point_Oxygen_norm.at(n) = environment_on_coord.at(n).oxygen_norm;
            graph_point_Oxygen_alpha.at(n) = environment_on_coord.at(n).oxygen_alpha;
            graph_point_DissolvedCarbon.at(n) = environment_on_coord.at(n).dissolvedcarbon;  // #26
            graph_point_DissolvedCarbon_norm.at(n) = environment_on_coord.at(n).dissolvedcarbon_norm;
            graph_point_DissolvedCarbon_alpha.at(n) = environment_on_coord.at(n).dissolvedcarbon_alpha;
            graph_point_bathymetry.at(n) = environment_on_coord.at(n).bathymetry; // 29
            graph_point_shippingdensity.at(n) = environment_on_coord.at(n).shippingdensity; // 30
            graph_point_siltfraction.at(n) = environment_on_coord.at(n).siltfraction; // 31
            graph_point_icesrectanglecode.at(n) = environment_on_coord.at(n).icesrectanglecode; // 32
        }

        //check
        cout << "prior check of environment_on_coord:" << "\n";
        cout << environment_on_coord.at(0).x << " " << environment_on_coord.at(0).y << " " <<
             environment_on_coord.at(0).harb << " " << environment_on_coord.at(0).code_area << " " <<
             environment_on_coord.at(0).landscapes_code << " " << environment_on_coord.at(0).wind << " " <<
             environment_on_coord.at(0).sst << " " << environment_on_coord.at(0).salinity << " " <<
             environment_on_coord.at(0).nitrogen << " " << environment_on_coord.at(0).phosphorus << " " <<
             environment_on_coord.at(0).oxygen << " " << environment_on_coord.at(0).dissolvedcarbon << " " << "\n";


        cout << "posterior check of environment_on_coord:" << "\n";
        cout << graph_coord_x.at(0) << " " << graph_coord_y.at(0) << " " << graph_coord_harbour.at(0) << " " <<
             graph_point_code_area.at(0) << " " << model().graph_point_code_landscape().at(0) << " "
             << graph_point_wind.at(0)
             << " " <<
             graph_point_sst.at(0) << " " << graph_point_salinity.at(0) << " " << graph_point_Nitrogen.at(0) << " "
             <<
             graph_point_Phosphorus.at(0) << " " << graph_point_Oxygen.at(0) << " "
             << graph_point_DissolvedCarbon.at(0)
             << " " <<
             "\n";
    }

    // read harbour specific files
    auto harbour_names = read_harbour_names(p->folder_name_parameterization, p->inputfolder);
    // creation of a vector of nodes from coord
    // and check with the coord in input.
    // use inheritance i.e. a Harbour is child of a Node
    // use polymorphism i.e. store either Harbour or Node in the vector of pointers 'nodes'
    auto nodes = vector<Node *>(graph_coord_x.size());
    for (unsigned int i = 0; i < graph_coord_x.size(); i++) {

        if (graph_coord_harbour[i]>0) {
            types::NodeId nId(i);

            string a_name = "none";
            types::NodeId a_point;
            // get the name of this harbour
            auto lower_g = harbour_names.lower_bound(nId);
            auto upper_g = harbour_names.upper_bound(nId);
            for (auto pos = lower_g; pos != upper_g; pos++) {
                a_point = pos->first;
                a_name = pos->second;
            }

            map<int, double> init_fuelprices;
            multimap<int, double> fishprices_each_species_per_cat;
            if (a_name != "none" && a_point == nId) {
                outc(cout << "load prices for port " << a_name << " which is point " << a_point << "\n");
                cout << "load prices for port " << a_name << " which is point " << a_point << "\n";
                int er2 = read_prices_per_harbour_each_pop_per_cat(a_point, quarterString(),
                                                                   fishprices_each_species_per_cat,
                                                                   p->folder_name_parameterization, p->inputfolder);
                // if not OK then deadly bug: possible NA or Inf in harbour files need to be checked (step 7)
                assert(er2 == 0);

                double a_multiplier_on_fishprices = 1.0;
                if (model().scenario().dyn_alloc_sce.option(Options::fishprice_plus100percent))
                {
                    a_multiplier_on_fishprices = 2.0;
                }
                if (model().scenario().dyn_alloc_sce.option(Options::fishprice_plus700percent))
                {
                    a_multiplier_on_fishprices = 7.0;
                }
                if (model().scenario().dyn_alloc_sce.option(Options::fishprice_plus800percent))
                {
                    a_multiplier_on_fishprices = 8.0;
                }
                if (model().scenario().dyn_alloc_sce.option(Options::fishprice_plus900percent))
                {
                    a_multiplier_on_fishprices = 9.0;
                }
                if (model().scenario().dyn_alloc_sce.option(Options::fishprice_plus1000percent))
                {
                    a_multiplier_on_fishprices = 10.0;
                }
                if (model().scenario().dyn_alloc_sce.option(Options::fishprice_plus1100percent))
                {
                    a_multiplier_on_fishprices = 11.0;
                }
                if (model().scenario().dyn_alloc_sce.option(Options::fishprice_plus1200percent))
                {
                    a_multiplier_on_fishprices = 12.0;
                }
                if (model().scenario().dyn_alloc_sce.option(Options::fishprice_plus1300percent))
                {
                    a_multiplier_on_fishprices = 13.0;
                }
                if (model().scenario().dyn_alloc_sce.option(Options::fishprice_plus1400percent))
                {
                    a_multiplier_on_fishprices = 14.0;
                }
                if (model().scenario().dyn_alloc_sce.option(Options::fishprice_plus1500percent))
                {
                    a_multiplier_on_fishprices = 15.0;
                }

                multimap<int, double>::iterator pos;
                for (pos = fishprices_each_species_per_cat.begin(); pos != fishprices_each_species_per_cat.end(); pos++) {
                    pos->second = (pos->second) * a_multiplier_on_fishprices;
                }

                for (pos = fishprices_each_species_per_cat.begin(); pos != fishprices_each_species_per_cat.end(); pos++) {
                    outc(cout << pos->first << " " << pos->second);
                }

                outc(cout << "....OK" << "\n");
            } else {
                outc(cout << a_point
                          << " : harbour not found in the harbour names (probably because no declared landings from studied vessels in those ports)"
                          << "\n");
                outc(cout << "...then go for the port: " << model().scenario().a_port << " instead" << "\n");
                int er2 = read_prices_per_harbour_each_pop_per_cat(model().scenario().a_port, "1",
                                                                   fishprices_each_species_per_cat,
                                                                   p->folder_name_parameterization,
                                                                   p->inputfolder);

                assert(er2 == 0);
            }

            // read fuel price (vessel size dependent for the time being)
            double a_multiplier_on_fuelprices = 1.0;
            if (model().scenario().dyn_alloc_sce.option(Options::fuelprice_plus20percent))
            {
                a_multiplier_on_fuelprices = 1.2;
            }
            if (model().scenario().dyn_alloc_sce.option(Options::fuelprice_plus50percent))
            {
                a_multiplier_on_fuelprices = 1.5;
            }
            if (model().scenario().dyn_alloc_sce.option(Options::fuelprice_plus100percent))
            {
                a_multiplier_on_fuelprices = 2.0;
            }
            read_fuel_prices_per_vsize(model().scenario().a_port,
                                           quarterString(), init_fuelprices,
                                           p->folder_name_parameterization,
                                           p->inputfolder);

            map<int, double>::iterator pos;
            for (pos = init_fuelprices.begin(); pos != init_fuelprices.end(); pos++) {
                    pos->second = (pos->second) * a_multiplier_on_fuelprices;
                }

            for (pos = init_fuelprices.begin(); pos != init_fuelprices.end(); pos++) {
                    outc(cout << pos->first << " " << pos->second);
                }
            
            



            vector<types::NodeId> init_usual_fgrounds;
            init_usual_fgrounds.push_back(types::NodeId(0));

            vector<double> init_freq_usual_fgrounds;
            init_freq_usual_fgrounds.push_back(1.0);

            nodes[i] = (new Harbour(types::NodeId(i),
                                    graph_coord_x[i],
                                    graph_coord_y[i],
                                    graph_coord_harbour[i],
                                    graph_point_code_area[i],
                                    model().graph_point_code_landscape()[i],
                                    graph_point_landscape_norm[i],
                                    graph_point_landscape_alpha[i],
                                    graph_point_wind[i],
                                    graph_point_wind_norm[i],
                                    graph_point_wind_alpha[i],
                                    graph_point_sst[i],
                                    graph_point_sst_norm[i],
                                    graph_point_sst_alpha[i],
                                    graph_point_salinity[i],
                                    graph_point_salinity_norm[i],
                                    graph_point_salinity_alpha[i],
                                    graph_point_Nitrogen[i],
                                    graph_point_Nitrogen_norm[i],
                                    graph_point_Nitrogen_alpha[i],
                                    graph_point_Phosphorus[i],
                                    graph_point_Phosphorus_norm[i],
                                    graph_point_Phosphorus_alpha[i],
                                    graph_point_Oxygen[i],
                                    graph_point_Oxygen_norm[i],
                                    graph_point_Oxygen_alpha[i],
                                    graph_point_DissolvedCarbon[i],
                                    graph_point_DissolvedCarbon_norm[i],
                                    graph_point_DissolvedCarbon_alpha[i],
                                    graph_point_bathymetry[i],
                                    graph_point_shippingdensity[i],
                                    graph_point_siltfraction[i],
                                    graph_point_icesrectanglecode[i],
                                    graph_point_benthos_biomass[i],
                                    graph_point_benthos_number[i],
                                    0, // meanweight not set from a GIS layer....
                                    0, // biomass_K not set from a GIS layer....
                                    0, // number_K not set from a GIS layer....
                                    model().config().nbpops,
                                    model().config().nbmets,
                                    model().config().nbbenthospops,
                                    NBSZGROUP,
                                    a_name,
                                    fishprices_each_species_per_cat,
                                    init_fuelprices,
                                    init_usual_fgrounds,
                                    init_freq_usual_fgrounds
            ));

            dout(cout << "Harbour " << nodes[i]->get_name() << " " <<
                      nodes[i]->get_x() << " " << nodes[i]->get_y() << " " <<
                      nodes[i]->get_is_harbour() << " " << "\n");
        } else {
            nodes[i] = (new Node(types::NodeId(i),
                                 graph_coord_x[i],
                                 graph_coord_y[i],
                                 graph_coord_harbour[i],
                                 graph_point_code_area[i],
                                 model().graph_point_code_landscape()[i],
                                 graph_point_landscape_norm[i],
                                 graph_point_landscape_alpha[i],
                                 graph_point_wind[i],
                                 graph_point_wind_norm[i],
                                 graph_point_wind_alpha[i],
                                 graph_point_sst[i],
                                 graph_point_sst_norm[i],
                                 graph_point_sst_alpha[i],
                                 graph_point_salinity[i],
                                 graph_point_salinity_norm[i],
                                 graph_point_salinity_alpha[i],
                                 graph_point_Nitrogen[i],
                                 graph_point_Nitrogen_norm[i],
                                 graph_point_Nitrogen_alpha[i],
                                 graph_point_Phosphorus[i],
                                 graph_point_Phosphorus_norm[i],
                                 graph_point_Phosphorus_alpha[i],
                                 graph_point_Oxygen[i],
                                 graph_point_Oxygen_norm[i],
                                 graph_point_Oxygen_alpha[i],
                                 graph_point_DissolvedCarbon[i],
                                 graph_point_DissolvedCarbon_norm[i],
                                 graph_point_DissolvedCarbon_alpha[i],
                                 graph_point_bathymetry[i],
                                 graph_point_shippingdensity[i],
                                 graph_point_siltfraction[i],
                                 graph_point_icesrectanglecode[i],
                                 graph_point_benthos_biomass[i],
                                 graph_point_benthos_number[i],
                                 0, // meanweight not set from a GIS layer....
                                 0, // biomass_K not set from a GIS layer....
                                 0, // number_K not set from a GIS layer....
                                 model().config().nbpops,
                                 model().config().nbmets,
                                 model().config().nbbenthospops,
                                 NBSZGROUP));
            dout(cout << nodes[i]->get_x() << " " << nodes[i]->get_y() << " " << nodes[i]->get_is_harbour()
                      << " " << nodes[i]->get_code_area() << "\n");

        }

    }

    // TODO move this to proper sub function

    // check inputs
    for (unsigned int i = 0; i < graph_coord_harbour.size(); i++) {
        dout(cout << graph_coord_harbour[i] << " ");
    }
    dout(cout << "\n");

    // check inputs
    for (unsigned int i = 0; i < graph_point_code_area.size(); i++) {
        outc(cout << graph_point_code_area[i] << " ");
    }
    outc(cout << "\n");

    // fake to check
    types::NodeId from = nodes.at(0)->get_idx_node();
    cout << "from is " << from.toIndex() << "\n";
    types::NodeId to = nodes.at(nodes.size()-1)->get_idx_node();
    cout << "to is " << to.toIndex() << "\n";


    model().setNodes(std::move(nodes));

    // ASTAR TODO: Check Loading the nodes
    try {
        GeoGraph geoGraph;
        string filename_graph_test = p->inputfolder + "/graphsspe/graph" + a_graph_s + ".dat";
        GeoGraphLoader loader;
        loader.load(geoGraph, filename_graph, filename_graph_test);
        cout << "Loading the graph " << filename_graph << " ...ok" << "\n";
        AStarShortestPathFinder aStarPathFinder;
        list<types::NodeId> path = aStarPathFinder.findShortestPath(geoGraph, from.toIndex(), to.toIndex());
        //for (auto v : path) std::cout << v << "\n";
        //cout << "Check a shortest path  ...ok" << "\n";
        model().setGeoGraph(std::move(geoGraph));

    } catch (std::exception &x) {
        std::cerr << "Cannot read Node graphs: " << x.what();
        return 2;
    }

    return true;
}

void TextfileModelLoader::loadCalendar()
{
    vector<int> tsteps_quarters = read_tsteps_quarters(p->folder_name_parameterization, p->inputfolder);
    vector<int> tsteps_semesters = read_tsteps_semesters(p->folder_name_parameterization, p->inputfolder);
    vector<int> tsteps_years = read_tsteps_years(p->folder_name_parameterization, p->inputfolder);
    vector<int> tsteps_months = read_tsteps_months(p->folder_name_parameterization, p->inputfolder);

    auto calendar = std::make_unique<Calendar>();
    calendar->setCalendar(std::move(tsteps_months),
                          std::move(tsteps_quarters),
                          std::move(tsteps_semesters),
                          std::move(tsteps_years));

    model().setCalendar(std::move(calendar));
}
