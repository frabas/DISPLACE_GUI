// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2023 Francois Bastardie <fba@aqua.dtu.dk>

//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.

//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.

//    You should have received a copy of the GNU General Public License along
//    with this program; if not, write to the Free Software Foundation, Inc.,
//    51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
// --------------------------------------------------------------------------

#include "readdata.h"
#include "utils/safe_strerror.h"

#include <helpers.h>
#include <idtypes.h>
#include <idtypeshelpers.h>

#include <utils/LineNumberReader.h>
#include <boost/lexical_cast.hpp>
#include <utils/vectorsdata.h>
#include <boost/algorithm/string.hpp>

#include <numeric> // iota()

#include <legacy/binarygraphfilereader.h>
#include "db/ConfigTable.h"
#include "db/ScenarioConfigTable.h"

#include <msqlitecpp/v2/storage.h>
#include <msqlitecpp/v2/selectstatement.h>

using namespace displace::formats;

#define NBSZGROUP 14
#define NBAGE 11				 // nb age classes max

/*
prints file opening error message.
@param filename, ...
*/

static std::string error_msg;

void open_file_error(string filename)
{
    error_msg = "error opening file " + filename;
    cout << error_msg << "\n";

    throw std::runtime_error(error_msg);
}

/**
read the settings for the siums given the case study
@param the vectors to be filled in, ...
*/
bool read_config_file(std::shared_ptr<msqlitecpp::v2::Storage> indb,
        string folder_name_parameterization,
                      string inputfolder,
                      int& nbpops,
                      int& nbmets,
                      int& nbbenthospops,
                      vector<int>& implicit_pops,
                      vector<int>& implicit_pops_level2,
                      vector<int>& grouped_tacs,
                      vector<int>& nbcp_coupling_pops,
                      vector<double>& calib_oth_landings,
                      vector<double>& calib_w,
                      vector<double>& calib_cpue,
                      vector<types::NodeId> &interesting_harbours)
{

    if (indb) {
        try {
            displace::in::ConfigTable t;
            t.query(*indb);

            nbpops = t.getNbPops();
            nbmets = t.getNbMets();
            nbbenthospops = t.getNbBenthosPops();
            implicit_pops = t.getImplicitStocks();
            implicit_pops_level2 = t.getImplicitPopLevels2();
            grouped_tacs = t.getGroupedTacs();
            nbcp_coupling_pops = t.getNbCouplingPops();
            calib_oth_landings = t.getCalibLandingsStock();
            calib_w = t.getCalibW();
            calib_cpue = t.getCalibCpue();
            interesting_harbours = t.getInterestingHarbours();
            return true;
        } catch (std::exception &x) {
            std::cerr << "Cannot read config database: " << x.what() << "\n";
            return false;
        }
        return true;
    } else {
    string filename = inputfolder+"/simusspe_"+folder_name_parameterization+"/config.dat";
    std::cout << "Reading config file from " << filename << std::endl;

    std::ifstream fstream (filename.c_str(), std::ios_base::in);
    return read_config_file(fstream, nbpops, nbmets, nbbenthospops, implicit_pops, implicit_pops_level2, grouped_tacs, nbcp_coupling_pops, calib_oth_landings,
                            calib_w, calib_cpue, interesting_harbours);
    }
}

bool read_config_file(std::istream &stream,
                      int& nbpops,
                      int& nbmets,
                      int& nbbenthospops,
                      vector<int>& implicit_pops,
                      vector<int>& implicit_pops_level2,
                      vector<int>& grouped_tacs,
                      vector<int>& nbcp_coupling_pops,
                      vector<double>& calib_oth_landings,
                      vector<double>& calib_w,
                      vector<double>& calib_cpue,
                      vector<types::NodeId> &interesting_harbours)
{
    helpers::LineNumberReader reader;
    static const helpers::LineNumberReader::Specifications specs {
        {1,"nbpops"},{3,"nbmets"},{5,"nbbenthospops"},{7,"implicit_pops"},{9,"calib_oth_landings"},
        {11,"calib_weight_at_szgroup"},{13,"calib_cpue_multiplier"},{15,"int_harbours"},
        {17,"implicit_pops_level2"}, {19,"grouped_tacs"},{21,"nbcp_coupling_pops"}, 
    };

    if (!reader.importFromStream(stream, specs))
        return false;

    try {
        nbpops = reader.getAs<int>("nbpops");
        nbmets = reader.getAs<int>("nbmets");
        nbbenthospops= reader.getAs<int>("nbbenthospops");
        implicit_pops = displace::formats::utils::stringToVector<int>(reader.get("implicit_pops"), " ");
        implicit_pops_level2 = displace::formats::utils::stringToVector<int>(reader.get("implicit_pops_level2"), " ");
        grouped_tacs= displace::formats::utils::stringToVector<int>(reader.get("grouped_tacs"), " ");
        nbcp_coupling_pops= displace::formats::utils::stringToVector<int>(reader.get("nbcp_coupling_pops"), " ");
        calib_oth_landings = displace::formats::utils::stringToVector<double>(reader.get("calib_oth_landings"), " ");
        calib_w = displace::formats::utils::stringToVector<double>(reader.get("calib_weight_at_szgroup"), " ");
        calib_cpue = displace::formats::utils::stringToVector<double>(reader.get("calib_cpue_multiplier"), " ");
        interesting_harbours = types::helpers::toIdVector<types::NodeId>(displace::formats::utils::stringToVector<int>(reader.get("int_harbours"), " "));

        if(calib_oth_landings.size()!=nbpops)
             throw std::runtime_error("Error while reading: config.dat: check the dimension i.e. inconsistency with nbpops");
        if(calib_w.size()!=nbpops)
             throw std::runtime_error("Error while reading: config.dat: check the dimension i.e. inconsistency with nbpops");
        if(calib_cpue.size()!=nbpops)
             throw std::runtime_error("Error while reading: config.dat: check the dimension i.e. inconsistency with nbpops");

        if(grouped_tacs.empty()){
            grouped_tacs.resize(nbpops);
            iota (std::begin(grouped_tacs), std::end(grouped_tacs), 0); // Fill with 0, 1, ...,
            }
        if(nbcp_coupling_pops.empty()){
            grouped_tacs.push_back(0);
            }
        } catch (displace::formats::FormatException &x) {
#ifdef VERBOSE_ERRORS
        cerr << x.what() << endl;
#endif
        return false;
    }

#ifdef VERBOSE_ERRORS
    cout << "read config file...OK" << endl << flush;
#endif

    return true;
}

template<typename Reader>
bool importScenario(Reader &reader, displace::commons::Scenario &scenario)
{
    try {
        auto dasf = reader.get("dyn_alloc_sce");
        std::vector<std::string> das;
        boost::split(das, dasf, boost::is_any_of(" "));
        for (auto d : das) {
            scenario.dyn_alloc_sce.setOption(d, true);
        }

        auto dpsf = reader.get("dyn_pop_sce");
        std::vector<std::string> dps;
        boost::split(dps, dpsf, boost::is_any_of(" "));
        for (auto d : dps) {
            scenario.dyn_pop_sce.setOption(d, true);
        }

        std::vector<int> metier_closures;
        auto met_c = reader.get("metier_closures");
        std::vector<std::string> closures;
        boost::trim(met_c);
        if (!met_c.empty()) {
            boost::split(closures, met_c, boost::is_any_of(" "));
            for (auto cl : closures) {
                metier_closures.push_back(boost::lexical_cast<int>(cl));
            }
        }
        scenario.closure_opts.setOption(Options::Closure_Opt::banned_metiers, metier_closures);

        scenario.biolsce = reader.get("biolsce");
        scenario.fleetsce = reader.get("fleetsce");
        scenario.freq_do_growth = reader.template getAs<int>("freq_do_growth");
        scenario.freq_redispatch_the_pop = reader.template getAs<int>("freq_redispatch_the_pop");
        scenario.a_graph = reader.template getAs<int>("a_graph");
        scenario.a_graph_name = "a_graph" + std::to_string(scenario.a_graph);
        scenario.nrow_coord = reader.template getAs<int>("nrow_coord");
        scenario.nrow_graph = reader.template getAs<int>("nrow_graph");
        scenario.a_port = types::NodeId(reader.template getAs<int>("a_port"));
        scenario.graph_res = displace::formats::utils::stringToVector<double>(reader.get("graph_res"), " ");
        if (scenario.graph_res.size() == 1) {
            scenario.graph_res.push_back(scenario.graph_res.at(0));
        } //res x and y required
        scenario.is_individual_vessel_quotas = (reader.template getAs<int>("is_individual_vessel_quotas") != 0);
        scenario.check_all_stocks_before_going_fishing = (
                reader.template getAs<int>("check_all_stocks_before_going_fishing") !=
                0);
        scenario.dt_go_fishing = reader.get("dt_go_fishing");
        scenario.dt_choose_ground = reader.get("dt_choose_ground");
        scenario.dt_start_fishing = reader.get("dt_start_fishing");
        scenario.dt_change_ground = reader.get("dt_change_ground");
        scenario.dt_stop_fishing = reader.get("dt_stop_fishing");
        scenario.dt_change_port = reader.get("dt_change_port");
        scenario.use_dtrees = (reader.template getAs<int>("use_dtrees") != 0);

        scenario.tariff_pop = displace::formats::utils::stringToVector<int>(reader.get("tariff_pop"), " ");
        scenario.freq_update_tariff_code = reader.template getAs<int>("freq_update_tariff_code");
        scenario.arbitary_breaks_for_tariff = displace::formats::utils::stringToVector<double>(
                reader.get("arbitary_breaks_for_tariff"), " ");
        scenario.met_multiplier_on_arbitary_breaks_for_tariff = displace::formats::utils::stringToVector<double>(
            reader.get("met_multiplier_on_arbitary_breaks_for_tariff"), " ");
        

        scenario.total_amount_credited = reader.template getAs<int>("total_amount_credited", 0);
        scenario.tariff_annual_hcr_percent_change = reader.template getAs<double>("tariff_annual_hcr_percent_change",
                                                                                  0);
        scenario.update_tariffs_based_on_lpue_or_dpue_code = reader.template getAs<int>(
                "update_tariffs_based_on_lpue_or_dpue_code", 0);

    } catch (displace::formats::FormatException &x) {
#ifdef VERBOSE_ERRORS
        cerr << x.what() << endl;
#else
        (void) x;
#endif
        return false;
    }
#ifdef VERBOSE_ERRORS
    cout << "read scenario config file...OK" <<  endl << flush;
    cout << "...e.g. graph is " << scenario.a_graph <<  endl << flush;
    cout << "...e.g. check_all_stocks_before_going_fishing is " << scenario.check_all_stocks_before_going_fishing <<  endl << flush;
#endif

    // Update the internals when needed
    scenario.closure_opts.update();

    return true;
}


/**
read the scenario specific settings for the siums given the case study
@param the vectors to be filled in, ...
*/
bool read_scenario_config_file(std::shared_ptr<msqlitecpp::v2::Storage> db,
                               string folder_name_parameterization,
                               string inputfolder,
                               string namefolderoutput,
                               displace::commons::Scenario &scenario)
{
    if (db) {
        displace::in::ScenarioConfigTable table;
        table.query(*db);

        return importScenario(table, scenario);
    } else {
        string filename = inputfolder + "/simusspe_" + folder_name_parameterization + "/" + namefolderoutput + ".dat";
        std::cout << "Reading Scenario file from " << filename << std::endl;

        std::ifstream f(filename.c_str(), std::ios_base::in);
        return read_scenario_config_file(f, scenario);
    }
}

bool read_scenario_config_file(std::istream &stream, displace::commons::Scenario &scenario)
{
    helpers::LineNumberReader reader;

    static const helpers::LineNumberReader::Specifications specs {
        {1,"dyn_alloc_sce"},{3,"dyn_pop_sce"},{5,"biolsce"},{7,"fleetsce"},{9,"freq_do_growth"},{11,"freq_redispatch_the_pop"},
        {13,"a_graph"},{15,"nrow_coord"},{17,"nrow_graph"},{19,"a_port"},{21,"graph_res"},
        {23,"is_individual_vessel_quotas"},{25,"check_all_stocks_before_going_fishing"},{27,"dt_go_fishing"},
        {29,"dt_choose_ground"},{31,"dt_start_fishing"},{33,"dt_change_ground"},{35,"dt_stop_fishing"},
        {37,"dt_change_port"},{39,"use_dtrees"},
        {41,"tariff_pop"},{43,"freq_update_tariff_code"},
        {45,"arbitary_breaks_for_tariff"},
        {47,"met_multiplier_on_arbitary_breaks_for_tariff"},    
        {49,"total_amount_credited"},
        {51,"tariff_annual_hcr_percent_change"},
        {53,"update_tariffs_based_on_lpue_or_dpue_code"},
        {55,"metier_closures"}
    };

    if (!reader.importFromStream(stream, specs))
        return false;

    return importScenario(reader, scenario);
}

vector <int> read_tsteps_quarters(string folder_name_parameterization, string inputfolder)
{

    vector<int> tsteps_quarters;

    string filename=  inputfolder+"/simusspe_"+folder_name_parameterization+"/tstep_quarters.dat";
    ifstream in;
    in.open(filename.c_str());
    if (in.fail())
    {
        filename = inputfolder + "/simusspe_" + folder_name_parameterization + "/tstep_quarters_2009_2015.dat";
        in.open(filename.c_str());
        if (in.fail())
        {
            open_file_error(filename.c_str());
        }
    }
    bool dd =true;
    int val;
    while(dd)
    {
        in >> val;
        if(val==-1) dd=false;
        else
        {
            tsteps_quarters.push_back(val);
        }

    }

    /*
    // check
    cout << "tsteps_quarters: " << endl;
    for(unsigned int i=0; i<tsteps_quarters.size(); i++)
    {
        cout << tsteps_quarters[i] << " " << endl;
    }
    cout << endl;
    */

    return(tsteps_quarters);
}


//-----------
vector <int> read_tsteps_semesters(string folder_name_parameterization, string inputfolder)
{

    vector<int> tsteps_semesters;

    string filename=  inputfolder+"/simusspe_"+folder_name_parameterization+"/tstep_semesters.dat";
    ifstream in;
    in.open(filename.c_str());
    if (in.fail())
    {
        filename = inputfolder + "/simusspe_" + folder_name_parameterization + "/tstep_semesters_2009_2015.dat";
        in.open(filename.c_str());
        if (in.fail())
        {
            open_file_error(filename.c_str());
        }
    }


    bool dd =true;
    int val;
    while(dd)
    {
        in >> val;
        if(val==-1) dd=false;
        else
        {
            tsteps_semesters.push_back(val);
        }
    }

    /*
    // check
    cout << "tsteps_semesters: " << endl;
    for(unsigned int i=0; i<tsteps_semesters.size(); i++)
    {
        cout << tsteps_semesters[i] << " " << endl;
    }
    cout << endl;
    */

    return(tsteps_semesters);
}


//-----------
vector <int> read_tsteps_months(string folder_name_parameterization, string inputfolder)
{

    vector<int> tsteps_months;

    string filename=  inputfolder+"/simusspe_"+folder_name_parameterization+"/tstep_months.dat";
    ifstream in;
    in.open(filename.c_str());
    if (in.fail())
    {
        filename = inputfolder + "/simusspe_" + folder_name_parameterization + "/tstep_months_2009_2015.dat";
        in.open(filename.c_str());
        if (in.fail())
        {
            open_file_error(filename.c_str());
        }
    }


    bool dd =true;
    int val;
    while(dd)
    {
        in >> val;
        if(val==-1) dd=false;
        else
        {
            tsteps_months.push_back(val);
        }
    }

    /*
    // check
    cout << "tsteps_months: " << endl;
    for(unsigned int i=0; i<tsteps_months.size(); i++)
    {
        cout << tsteps_months[i] << " " << endl;
    }
    cout << endl;
    */

    return(tsteps_months);
}


//-----------
vector <int> read_tsteps_years(string folder_name_parameterization, string inputfolder)
{

    vector<int> tsteps_years;

    string filename = inputfolder + "/simusspe_" + folder_name_parameterization + "/tstep_years.dat";
    ifstream in;
    in.open(filename.c_str());
    if (in.fail())
    {
       filename=  inputfolder+"/simusspe_"+folder_name_parameterization+"/tstep_years_2009_2015.dat";
       in.open(filename.c_str());
       if(in.fail())
       {
        open_file_error(filename.c_str());
       }
    }

    bool dd =true;
    int val;
    while(dd)
    {
        in >> val;
        if(val==-1) dd=false;
        else
        {
            tsteps_years.push_back(val);
        }
    }

    /*
    // check
    cout << "tsteps_years: " << endl;
    for(unsigned int i=0; i<tsteps_years.size(); i++)
    {
        cout << tsteps_years[i] << " " << endl;
    }
    cout << endl;
    */

    return(tsteps_years);
}




//----------------
bool read_firms_features(vector<int>& firm_ids,
                         vector<string>& firm_names,
                         vector<int>& nb_vessels,
                         vector<double>& longs,
                         vector<double>& lats,
                         string folder_name_parameterization,
                         string inputfolder
                         )
{

    string filename=  inputfolder+"/firmsspe_"+folder_name_parameterization+"/firms_specs.dat";

    ifstream firms_features;
    firms_features.open(filename.c_str());
    if(firms_features.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }

    bool r = fill_from_firms_specifications(firms_features, firm_ids,
                                            firm_names, nb_vessels,
                                            longs, lats);
    firms_features.close();

    return r;
}




multimap<types::NodeId, string> read_harbour_names(string folder_name_parameterization, string inputfolder)
{

    //input data,
    string filename=  inputfolder+"/harboursspe_"+folder_name_parameterization+"/names_harbours.dat";
    ifstream harbour_names_file;
    harbour_names_file.open(filename.c_str());
    if(harbour_names_file.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    //key=fground, value=harbour name
    multimap<int, string> harbour_names;
    fill_multimap_from_specifications_i_s(harbour_names_file,  harbour_names);
    harbour_names_file.close();

    /*
    // check input
    multimap<int,string>::iterator lower_g = harbour_names.lower_bound(2478);
    multimap<int,string>::iterator upper_g = harbour_names.upper_bound(2478);
    cout << "for this harbour, the harbour name is: ";
    for (multimap<int, string>::iterator pos=lower_g; pos != upper_g; pos++)
    {
        cout << pos->second << " ";
    }
    cout << endl;*/

    return(types::helpers::toKeyIdMultimap<types::NodeId>(harbour_names));
}


int read_prices_per_harbour(types::NodeId i, string a_quarter, multimap<string, double>& prices_per_harbour,
                            string folder_name_parameterization, string inputfolder)
{

    // casting sp into a string
    stringstream out;
    out << i.toIndex();

    //input data, harbour characteristics
    string filename=  inputfolder+"/harboursspe_"+folder_name_parameterization+"/"+out.str()+"_quarter"+a_quarter+".dat";
    ifstream fish_prices_per_harbour;
    fish_prices_per_harbour.open(filename.c_str());
    if(fish_prices_per_harbour.fail())
    {
        cout << "fail to load the file for price per met per pop for this port" << endl;
        open_file_error(filename.c_str());

        return -1;
    }
    else
    {
        fill_multimap_from_specifications_s_d(fish_prices_per_harbour,  prices_per_harbour);
        fish_prices_per_harbour.close();

        // check input
        // multimap<string,double>::iterator lower3 = prices_per_harbour.lower_bound("met0");
        //multimap<string,double>::iterator upper3 = prices_per_harbour.upper_bound("met0");
        //dout << "metier-specific fish price on " << i <<": ";
        //for (multimap<string, double>::iterator pos=lower3; pos != upper3; pos++)
        //{
        //    dout << pos->second << " ";
        //}
        //dout << endl;

        return(0);
    }
}



void read_fuel_prices_per_vsize(types::NodeId i, string a_quarter,
                                map<int, double>& fuel_prices_per_vsize,
                                string folder_name_parameterization, string inputfolder)
{

    // casting sp into a string
    stringstream out;
    out << i.toIndex();

    //input data, harbour characteristics
    string filename = inputfolder+"/harboursspe_"+folder_name_parameterization+"/"+out.str()+"_quarter"+a_quarter+"_fuel_price_per_vessel_size.dat";
    ifstream file_fuel_prices_per_vsize;
    file_fuel_prices_per_vsize.open(filename.c_str());
    if(file_fuel_prices_per_vsize.fail())
    {
        cout << "fail to load the file for fuel price per vsize for this port...search in vesselspe instead..." << endl;

       // by default:
       filename=  inputfolder+"/vesselsspe_"+folder_name_parameterization+"/fuel_price_per_vessel_size.dat";
       file_fuel_prices_per_vsize.open(filename.c_str());
       if(file_fuel_prices_per_vsize.fail())
       {
           cout << "fail to load the file for fuel price in vesselspe..." << endl;
           open_file_error(filename.c_str());

       }
    }


    fill_map_from_specifications_i_d(file_fuel_prices_per_vsize,  fuel_prices_per_vsize,  inputfolder);
    file_fuel_prices_per_vsize.close();



}

int read_prices_per_harbour_each_pop_per_cat(types::NodeId i, string a_quarter,
                                             multimap<int, double>& prices_per_harbour_each_species_per_cat,
                                             string folder_name_parameterization, string inputfolder)
{

    // casting sp into a string
    stringstream out;
    out << i.toIndex();

    //input data, harbour characteristics
    string filename = inputfolder+"/harboursspe_"+folder_name_parameterization+"/"+out.str()+"_quarter"+a_quarter+"_each_species_per_cat.dat";
    ifstream fish_prices_per_harbour;
    fish_prices_per_harbour.open(filename.c_str());
    if(fish_prices_per_harbour.fail())
    {
        cout << "fail to load the file for price per pop per cat for this port" << endl;
        open_file_error(filename.c_str());

        return -1;
    }
    else
    {
        fill_multimap_from_specifications_i_d(fish_prices_per_harbour,  prices_per_harbour_each_species_per_cat);
        fish_prices_per_harbour.close();

        // check input
        // multimap<string,double>::iterator lower3 = prices_per_harbour.lower_bound("met0");
        //multimap<string,double>::iterator upper3 = prices_per_harbour.upper_bound("met0");
        //dout << "metier-specific fish price on " << i <<": ";
        //for (multimap<string, double>::iterator pos=lower3; pos != upper3; pos++)
        //{
        //    dout << pos->second << " ";
        //}
        //dout << endl;

        return(0);
    }
}




multimap<types::NodeId, double> read_initial_tariffs_on_nodes(string folder_name_parameterization, string inputfolder, string a_graph_name, int metidx)
{

    stringstream ametidx;
    ametidx << metidx;
    
    // obtained from looking at average cpue on nodes for the explicit species
    //string filename=  inputfolder+"/graphsspe_"+folder_name_parameterization+"/initial_tariffs_on_nodes.dat";
    string filename=  inputfolder+"/graphsspe/initial_tariffs_on_nodes_"+a_graph_name+"met"+ ametidx.str() +".dat";

    ifstream initial_tariffs_on_nodes_file;
    initial_tariffs_on_nodes_file.open(filename.c_str());
    if(initial_tariffs_on_nodes_file.fail())
    {
        open_file_error(filename.c_str());
        // return 1;
    }
    //key=fground, value=gamma scale param
    multimap<int, double> initial_tariffs_on_nodes;
    fill_multimap_from_specifications_i_d(initial_tariffs_on_nodes_file,  initial_tariffs_on_nodes);
    initial_tariffs_on_nodes_file.close();

#ifdef VERBOSE
    // check input
    bool check = 0;
    if(check)
    {
        multimap<int,double>::iterator lower_gr = initial_tariffs_on_nodes.lower_bound(1600);
        multimap<int,double>::iterator upper_gr = initial_tariffs_on_nodes.upper_bound(1600);
        dout(cout << "for this specific ground (1600): ");
        for (multimap<int, double>::iterator pos2=lower_gr; pos2 != upper_gr; pos2++)
        {
            dout(cout << pos2->second << " ");
        }
        dout(cout << endl);
    }
#endif

    return(types::helpers::toKeyIdMultimap<types::NodeId>(initial_tariffs_on_nodes));
}



spp::sparse_hash_map<types::NodeId::type, types::NodeId::type> read_maps_previous(types::NodeId source, string namesimu,  string inputfolder, string a_graph_name)
{
    //dout(cout <<"BEGIN: read map previous" << endl);

    stringstream out;
    out << source.toIndex();
    string source_s = out.str();
    string filename= inputfolder+"/shortPaths_"+namesimu+"_"+a_graph_name+"/previous_"+source_s+".bin";

    bool r;
    spp::sparse_hash_map<types::NodeId::type, types::NodeId::type> previous;		 //key, value
    try {
        displace::formats::legacy::BinaryGraphFileReader rdr;
        r = rdr.importFromStream<uint16_t,uint16_t>(filename, [&previous](uint16_t key, uint16_t value) {
            previous.insert(std::pair<int,int>(key,value));
            return true;
        });
    } catch (std::exception &x) {
        cerr << "Exception : " << x.what() << endl;
        r = false;
    }

    if (!r) {
        cerr << "Error loading graph file " << filename << " " << safe_strerror(errno) << endl;
        throw std::runtime_error("Error loading graph file " + filename + " " + safe_strerror(errno));
    }
    //dout(cout <<"END: read map previous" << endl);

    return previous;
}

spp::sparse_hash_map<types::NodeId::type, int> read_min_distance(types::NodeId source, string namesimu, string inputfolder, string a_graph_name)
{
    //dout(cout <<"BEGIN: read min_distance" << endl);

    stringstream out;
    out << source.toIndex();
    string source_s = out.str();
    string filename= inputfolder+"/shortPaths_"+namesimu+"_"+a_graph_name+"/min_distance_"+source_s+".bin";

    bool r;
    spp::sparse_hash_map<types::NodeId::type, int> min_distance;		 //key, value
    try {
        displace::formats::legacy::BinaryGraphFileReader rdr;
        r = rdr.importFromStream<uint16_t,uint16_t>(filename, [&min_distance](uint16_t key, uint16_t value) {
            min_distance.insert(std::pair<int,int>(key,value));
            return true;
        });
    } catch (std::exception &x) {
        cerr << "Exception : " << x.what() << endl;
        r = false;
    }

    if (!r) {
        cerr << "Error loading graph file " << filename << " " << safe_strerror(errno) << endl;
        throw std::runtime_error("Error loading graph file " + filename + " " + safe_strerror(errno));
    }


    //dout(cout <<"END: read min_distance" << endl);

    return min_distance;
}

PathShop read_graph_details(types::NodeId source, string namesimu,  string inputfolder, string a_graph_name)
{
    //dout(cout <<"BEGIN: read map previous" << endl);

    stringstream out1;
    out1 << source.toIndex();
    string source_s1 = out1.str();
    string filename_previous= inputfolder+"/shortPaths_"+namesimu+"_"+a_graph_name+"/previous_"+source_s1+".bin";

    //dout(cout <<"BEGIN: read min_distance" << endl);

    stringstream out2;
    out2 << source.toIndex();
    string source_s2 = out2.str();
    string filename_weight= inputfolder+"/shortPaths_"+namesimu+"_"+a_graph_name+"/min_distance_"+source_s2+".bin";

    try{
     return   PathShop::readFromFiles(filename_previous, filename_weight);
    //    return a_shop;
    } catch (std::exception &x) {
        cout << " for node " << source_s1 << "," << endl;
        cerr << " Error in reading graphsspe\\shortPaths_ .bin files : " << x.what() << endl;
        throw;
    }
}

multimap<int, int> read_nodes_in_polygons(string a_quarter, string a_graph, string folder_name_parameterization, string inputfolder)
{
    UNUSED(folder_name_parameterization);

    string filename = inputfolder+"/graphsspe/nodes_in_polygons_"+a_graph+"_quarter"+a_quarter+".dat";

    ifstream file_nodes_in_polygons;
    file_nodes_in_polygons.open(filename.c_str());
    if(file_nodes_in_polygons.fail())
    {
        open_file_error(filename);
        //return 1;
    }
    multimap<int, int> nodes_in_polygons;
    fill_from_nodes_in_polygons (file_nodes_in_polygons, nodes_in_polygons);
    file_nodes_in_polygons.close();

    // check input
    //multimap<int,int>::iterator lower = nodes_in_polygons.lower_bound();
    //multimap<int,int>::iterator upper = nodes_in_polygons.upper_bound();
    //dout << " nodes_in_polygons " << endl;
    //for (multimap<int, double>::iterator pos=lower; pos != upper; pos++)
    //{
    //    dout << pos->second << " ";
    //}
    //dout << endl;

    return(nodes_in_polygons);
}

/* [DEPRECATED]
bool read_metier_quarterly_closures (vector <Node*> &nodes, string a_quarter, string a_graph, string folder_name_parameterization, string inputfolder)
{
    UNUSED(folder_name_parameterization);

    const string separator=" ";

    string filename = inputfolder+"/graphsspe/metier_closure_"+a_graph+"_quarter"+a_quarter+".dat";

    ifstream is;
    is.open(filename.c_str());
    if(is.fail())
    {
        open_file_error(filename);
        return false;
    }

    std::vector<NodeBanningInfo> banning;
    bool r = read_metier_closures(is, separator, banning);

    if (r) {
        for (auto &info : banning) {
            int count =0;
            for (auto id : info.banned) {
                nodes.at(info.nodeId.toIndex())->setBannedMetier(id);
                nodes.at(info.nodeId.toIndex())->setNbOfDaysClosedPerMonth(id, info.nbOfDaysClosedPerMonth.at(count));
                ++count;
            }

        }
    }

    is.close();
    return r;
}
*/

bool read_metier_monthly_closures (vector <Node*> &nodes, string a_month, string a_graph, string folder_name_parameterization, string inputfolder)
{
    UNUSED(folder_name_parameterization);

    const string separator=" ";

    string filename = inputfolder+"/graphsspe/metier_closure_"+a_graph+"_month"+a_month+".dat";

    ifstream is;
    is.open(filename.c_str());
    if(is.fail())
    {
        open_file_error(filename);
        return false;
    }

    std::vector<NodeBanningInfo> banning;
    bool r = read_metier_closures(is, separator, banning);
    if (r) {
        for (auto &info : banning) {
            int count =0;
            for (auto id : info.banned) {
                nodes.at(info.nodeId.toIndex())->setBannedMetier(id);
                nodes.at(info.nodeId.toIndex())->setNbOfDaysClosedPerMonth(id, info.nbOfDaysClosedPerMonth.at(count));
                ++count;
            }

        }
    }


    is.close();
    return r;
}

bool read_metier_closures(istream &stream, const std::string &separator, vector<NodeBanningInfo> &nodes)
{
    // Format:
    // PolyId nbOfDaysClosed NodeId Metier [Metier[ Metier...]]

    int linenum = 0;
    try {
        while (stream) {
            std::string line;
            std::getline(stream, line);

            boost::trim(line);
            if (line.empty())
                continue;

            std::vector<std::string> sr;
            boost::split(sr, line, boost::is_any_of(separator));
            NodeBanningInfo info;
            info.nodeId = types::NodeId(boost::lexical_cast<int>(sr[2]));
            for (size_t i = 3; i < sr.size(); ++i) {
                int m = boost::lexical_cast<int>(sr[i]);
                info.banned.push_back(m);
                info.nbOfDaysClosedPerMonth.push_back(boost::lexical_cast<double>(sr[1]));
            }
            nodes.push_back(info);
            ++linenum;
        }
    } catch (boost::bad_lexical_cast &ex) {
//#ifdef VERBOSE_ERRORS
        cerr << "Bad Conversion on read_metier_closures file line " << linenum <<
                " : " << ex.what() << "\n";
//#endif
        return false;
    }

    return true;
}

bool read_vsize_monthly_closures (vector <Node*> &nodes, string a_month, string a_graph, string folder_name_parameterization, string inputfolder)
{
    UNUSED(folder_name_parameterization);

    const string separator=" ";

    string filename = inputfolder+"/graphsspe/vsize_closure_"+a_graph+"_month"+a_month+".dat";

    ifstream is;
    is.open(filename.c_str());
    if(is.fail())
    {
        open_file_error(filename);
        return false;
    }

    std::vector<NodeBanningInfo> banning;
    bool r = read_vsize_closures(is, separator, banning);

    if (r) {
        for (auto &info : banning) {
            for (auto id : info.banned) {
                nodes.at(info.nodeId.toIndex())->setBannedVsize(id);
                nodes.at(info.nodeId.toIndex())->setAreaType(1);
            }
        }
    }

    is.close();
    return r;
}

bool read_vsize_closures(istream &stream, const std::string &separator, vector<NodeBanningInfo> &nodes)
{
    // Format:
    // PolyId nbOfDaysClosed NodeId Vessel Size [Vessel Size[ Vessel Size...]]

    int linenum = 0;
    try {
        while (stream) {
            std::string line;
            std::getline(stream, line);

            boost::trim(line);
            if (line.empty())
                continue;

            std::vector<std::string> sr;
            boost::split(sr, line, boost::is_any_of(separator));


            NodeBanningInfo info;
            info.nodeId = types::NodeId(boost::lexical_cast<int>(sr[2]));
            for (size_t i = 3; i < sr.size(); ++i) {
                int m = boost::lexical_cast<int>(sr[i]);
                info.banned.push_back(m);
            }
            nodes.push_back(info);
            ++linenum;
        }
    } catch (boost::bad_lexical_cast &ex) {
//#ifdef VERBOSE_ERRORS
        cerr << "Bad Conversion on read_vsize_closure file line " << linenum <<
                " : " << ex.what() << "\n";
//#endif
        return false;
    }

    return true;
}


bool read_nation_monthly_closures(vector <Node*>& nodes, string a_month, string a_graph, string folder_name_parameterization, string inputfolder)
{
    UNUSED(folder_name_parameterization);

    const string separator = " ";

    string filename = inputfolder + "/graphsspe/nation_closure_" + a_graph + "_month" + a_month + ".dat";

    ifstream is;
    is.open(filename.c_str());
    if (is.fail())
    {
        open_file_error(filename);
        return false;
    }

    std::vector<NodeBanningInfo> banning;
    bool r = read_nation_closures(is, separator, banning);

    if (r) {
        for (auto& info : banning) {
            for (auto id : info.banned) {
                nodes.at(info.nodeId.toIndex())->setBannedNation(id);
                nodes.at(info.nodeId.toIndex())->setAreaType(1);
            }
        }
    }

    is.close();
    return r;
}

bool read_nation_closures(istream& stream, const std::string& separator, vector<NodeBanningInfo>& nodes)
{
    // Format:
    // PolyId nbOfDaysClosed NodeId Nation [Nation[ Nation...]]

    int linenum = 0;
    try {
        while (stream) {
            std::string line;
            std::getline(stream, line);

            boost::trim(line);
            if (line.empty())
                continue;

            std::vector<std::string> sr;
            boost::split(sr, line, boost::is_any_of(separator));


            NodeBanningInfo info;
            info.nodeId = types::NodeId(boost::lexical_cast<int>(sr[2]));
            for (size_t i = 3; i < sr.size(); ++i) {
                int m = boost::lexical_cast<int>(sr[i]);
                info.banned.push_back(m);
            }
            nodes.push_back(info);
            ++linenum;
        }
    }
    catch (boost::bad_lexical_cast& ex) {
        //#ifdef VERBOSE_ERRORS
        cerr << "Bad Conversion on read_nation_closure file line " << linenum <<
            " : " << ex.what() << "\n";
        //#endif
        return false;
    }

    return true;
}



bool read_biological_traits_params(istream &stream, const std::string &separator, std::vector <std::tuple< string, double, double, double, double,
                                   double, double, double, double,
                                   double, double, double, double,
                                   double, double, double, double,
                                   double, double, double, double,
                                   double, double, double, double,
                                   double, double, double, double,
                                   double, double, double, double,
                                   double, string> >  & biological_traits_params)
{
    // Format:
    //0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	32	33	34
    //stock	Winf	k	Linf	K	t0	a	b	L50	alpha	beta	r_age	tac_tons	fbar_age_min	fbar_age_max	F_target	F_percent	TAC_percent	B_trigger	FMSY	fbar_assessment	ssb_assessment	mls_cat	mls	size_bin_cm	unit_sizebin	CV_recru	mat	mat_cat	etha_m	kappa	q	n	fzeroest	species

    // TODO: FOR NOW, more or less ONLY Winf k ARE USED....BUT THINK ABOUT REPLACING INPUTS FOR OTHER PARAMS FROM HERE.

    cout << "Reading biological_traits_params..." << endl;

    std::string dummystring;
    getline (stream, dummystring); // eat the heading

    int linenum = 0;
    try {
        while (stream) {
            std::string line;
            std::getline(stream, line);

            boost::trim(line);
            if (line.empty())
                continue;

            std::vector<std::string> sr;
            boost::split(sr, line, boost::is_any_of(separator));

            std::tuple< string, double, double, double, double,
                                               double, double, double, double,
                                               double, double, double, double,
                                               double, double, double, double,
                                               double, double, double, double,
                                               double, double, double, double,
                                               double, double, double, double,
                                               double, double, double, double,
                                               double, string>  a_tuple;

            std::get<0>(a_tuple)=boost::lexical_cast<string>(sr[0]);
            std::get<1>(a_tuple)=boost::lexical_cast<double>(sr[1]);
            std::get<2>(a_tuple)=boost::lexical_cast<double>(sr[2]);
            std::get<3>(a_tuple)=boost::lexical_cast<double>(sr[3]);
            std::get<4>(a_tuple)=boost::lexical_cast<double>(sr[4]);
            std::get<5>(a_tuple)=boost::lexical_cast<double>(sr[5]);
            std::get<6>(a_tuple)=boost::lexical_cast<double>(sr[6]);
            std::get<7>(a_tuple)=boost::lexical_cast<double>(sr[7]);
            std::get<8>(a_tuple)=boost::lexical_cast<double>(sr[8]);
            std::get<9>(a_tuple)=boost::lexical_cast<double>(sr[9]);
            std::get<10>(a_tuple)=boost::lexical_cast<double>(sr[10]);
            std::get<11>(a_tuple)=boost::lexical_cast<double>(sr[11]);
            std::get<12>(a_tuple)=boost::lexical_cast<double>(sr[12]);
            std::get<13>(a_tuple)=boost::lexical_cast<double>(sr[13]);
            std::get<14>(a_tuple)=boost::lexical_cast<double>(sr[14]);
            std::get<15>(a_tuple)=boost::lexical_cast<double>(sr[15]);
            std::get<16>(a_tuple)=boost::lexical_cast<double>(sr[16]);
            std::get<17>(a_tuple)=boost::lexical_cast<double>(sr[17]);
            std::get<18>(a_tuple)=boost::lexical_cast<double>(sr[18]);
            std::get<19>(a_tuple)=boost::lexical_cast<double>(sr[19]);
            std::get<20>(a_tuple)=boost::lexical_cast<double>(sr[20]);
            std::get<21>(a_tuple)=boost::lexical_cast<double>(sr[21]);
            std::get<22>(a_tuple)=boost::lexical_cast<double>(sr[22]);
            std::get<23>(a_tuple)=boost::lexical_cast<double>(sr[23]);
            std::get<24>(a_tuple)=boost::lexical_cast<double>(sr[24]);
            std::get<25>(a_tuple)=boost::lexical_cast<double>(sr[25]);
            std::get<26>(a_tuple)=boost::lexical_cast<double>(sr[26]);
            std::get<27>(a_tuple)=boost::lexical_cast<double>(sr[27]);
            std::get<28>(a_tuple)=boost::lexical_cast<double>(sr[28]);
            std::get<29>(a_tuple)=boost::lexical_cast<double>(sr[29]);
            std::get<30>(a_tuple)=boost::lexical_cast<double>(sr[30]);
            std::get<31>(a_tuple)=boost::lexical_cast<double>(sr[31]);
            std::get<32>(a_tuple)=boost::lexical_cast<double>(sr[32]);
            std::get<33>(a_tuple)=boost::lexical_cast<double>(sr[33]);
            std::get<34>(a_tuple)=boost::lexical_cast<string>(sr[34]);

            // check
            //cout << "0: " << std::get<0>(a_tuple) << endl;
            //cout << "1: " << std::get<1>(a_tuple) << endl;
            //cout << "2: " << std::get<2>(a_tuple) << endl;

            // TO DO: other params...
            biological_traits_params.push_back(a_tuple);


            ++linenum;
        }
        cout << "Reading biological_traits_params...ok" << endl;

    } catch (boost::bad_lexical_cast &ex) {
        cerr << "Bad Conversion on biological_traits_params file line " << linenum <<
                " : " << ex.what() << "\n";
        return false;
    }

    return true;
}


bool read_environment_on_coord(istream &stream, const std::string &separator, std::vector <EnvironmentDataRecord> & environment_on_coord)
{
    // Format:
    //"x"                     "y"                     "harb"                  "pt_graph"              "code_area"             "landscapes_code"       "landscape_norm"
    //"landscape_alpha"       "sst"                   "sst_norm"              "sst_alpha"             "salinity"              "salinity_norm"         "salinity_alpha"        "nitrogen"
    //"nitrogen_norm"         "nitrogen_alpha"        "phosphorus"            "phosphorus_norm"       "phosphorus_alpha"      "oxygen"                "oxygen_norm"
    //"oxygen_alpha"          "dissolvedcarbon"       "dissolvedcarbon_norm"  "dissolvedcarbon_alpha" "bathymetry" "shippingdensity" "siltfraction"

    cout << "Reading environment_on_coord..." << endl;

    std::string dummystring;
    getline (stream, dummystring); // eat the heading

    int linenum = 0;
    try {
        while (stream) {
            std::string line;
            std::getline(stream, line);

            boost::trim(line);
            if (line.empty())
                continue;

            std::vector<std::string> sr;
            boost::split(sr, line, boost::is_any_of(separator));

            EnvironmentDataRecord a_tuple;
            
            if (sr.size() != 33) cout << "Some missing field(s) in the input data environment_on_coord.txt" << endl;
            a_tuple.x=boost::lexical_cast<double>(sr[0]);
            a_tuple.y=boost::lexical_cast<double>(sr[1]);
            a_tuple.harb=boost::lexical_cast<int>(sr[2]);
            a_tuple.pt_graph=boost::lexical_cast<int>(sr[3]);
            a_tuple.code_area=boost::lexical_cast<int>(sr[4]);
            a_tuple.landscapes_code=boost::lexical_cast<int>(sr[5]);
            a_tuple.landscape_norm=boost::lexical_cast<double>(sr[6]);
            a_tuple.landscape_alpha=boost::lexical_cast<double>(sr[7]);
            a_tuple.wind = boost::lexical_cast<double>(sr[8]);
            a_tuple.wind_norm = boost::lexical_cast<double>(sr[9]);
            a_tuple.wind_alpha = boost::lexical_cast<double>(sr[10]);
            a_tuple.sst=boost::lexical_cast<double>(sr[11]);
            a_tuple.sst_norm=boost::lexical_cast<double>(sr[12]);
            a_tuple.sst_alpha=boost::lexical_cast<double>(sr[13]);
            a_tuple.salinity=boost::lexical_cast<double>(sr[14]);
            a_tuple.salinity_norm=boost::lexical_cast<double>(sr[15]);
            a_tuple.salinity_alpha=boost::lexical_cast<double>(sr[16]);
            a_tuple.nitrogen=boost::lexical_cast<double>(sr[17]);
            a_tuple.nitrogen_norm=boost::lexical_cast<double>(sr[18]);
            a_tuple.nitrogen_alpha=boost::lexical_cast<double>(sr[19]);
            a_tuple.phosphorus=boost::lexical_cast<double>(sr[20]);
            a_tuple.phosphorus_norm=boost::lexical_cast<double>(sr[21]);
            a_tuple.phosphorus_alpha=boost::lexical_cast<double>(sr[22]);
            a_tuple.oxygen=boost::lexical_cast<double>(sr[23]);
            a_tuple.oxygen_norm=boost::lexical_cast<double>(sr[24]);
            a_tuple.oxygen_alpha=boost::lexical_cast<double>(sr[25]);
            a_tuple.dissolvedcarbon=boost::lexical_cast<double>(sr[26]);
            a_tuple.dissolvedcarbon_norm=boost::lexical_cast<double>(sr[27]);
            a_tuple.dissolvedcarbon_alpha= boost::lexical_cast<double>(sr[28]);
            a_tuple.bathymetry=boost::lexical_cast<double>(sr[29]);
            a_tuple.shippingdensity=boost::lexical_cast<double>(sr[30]);
            a_tuple.siltfraction=boost::lexical_cast<double>(sr[31]);
            a_tuple.icesrectanglecode = boost::lexical_cast<double>(sr[32]);

            // check
            //cout << "reading  environment_on_coord: " << endl;
            //cout << "0: " << std::get<0>(a_tuple) << endl;
            //cout << "1: " << std::get<1>(a_tuple) << endl;
            //cout << "2: " << std::get<2>(a_tuple) << endl;

            // TO DO: other params...
            environment_on_coord.push_back(a_tuple);


            ++linenum;
        }
        cout << "Reading environment_on_coord...ok" << endl;

    } catch (boost::bad_lexical_cast &ex) {
        cerr << "Bad Conversion on environment_on_coord file line " << linenum <<
                " : " << ex.what() << "\n";
        return false;
    }

    return true;
}



void write_SMS_OP_N_in_file(ofstream& SMS_N_in,
                            vector<Population* >& populations,
                            vector<int> stock_numbers,
                            vector<int> some_units,
                            vector<int> some_max_nb_ages)
{

    // SMS-OM needs two inputs for forecasting Ny, which are the initial N i.e. Ny-1 and F applied during y-1
    // furthermore SMS needs quartely-based Fs

    SMS_N_in << setprecision(2) << fixed;
    for (unsigned int i=0; i<stock_numbers.size(); i++)
    {

        vector< vector<double> > percent_szgroup_per_age_matrix(NBSZGROUP, vector<double>(NBAGE));
        percent_szgroup_per_age_matrix=populations.at( stock_numbers.at(i) )->get_percent_szgroup_per_age_matrix();

        // convert N from size to age
        vector<double> tot_N_at_szgroup=populations.at( stock_numbers.at(i) )->get_tot_N_at_szgroup();
        vector<double> tot_N_at_age(some_max_nb_ages.at(i));
        dout(cout << "-- st" << stock_numbers.at(i)  << endl);
        for(unsigned int sz=0; sz<tot_N_at_szgroup.size(); sz++)
        {
            for(int a=0; a<some_max_nb_ages.at(i); a++)
            {
                tot_N_at_age[a] +=  percent_szgroup_per_age_matrix[sz][a] * tot_N_at_szgroup[sz] ;
                dout(cout << tot_N_at_age[a] << "= " << percent_szgroup_per_age_matrix[sz][a] << " * " << tot_N_at_szgroup[sz] << endl);
            }
        }

        // write N.in SMS files
        dout(cout << "write down the SMS n.in file...");

        for(unsigned int a = 0; a < tot_N_at_age.size(); a++)
        {
            SMS_N_in  << tot_N_at_age.at(a) / some_units.at(i) << " " ;
        }
        SMS_N_in << endl;

    }
    dout(cout << "OK...\n");

}


void write_SMS_OP_F_in_file(ofstream& SMS_F_in,
                            vector<Population* >& populations,
                            vector<int> stock_numbers)
{

    SMS_F_in << "# quarter: xx "<< endl;
    int nb_ages_in_sms=8;		 // MAGIC NUMBER

    for (unsigned int i=0; i<stock_numbers.size(); i++)
    {

        vector <double> tot_F_at_age;
        vector <double> tot_F_at_age_last_quarter;
        // obtain the quartely Fs
        // at this stage, this is a cumul...
        tot_F_at_age              = populations.at(stock_numbers.at(i))->get_tot_F_at_age();
        tot_F_at_age_last_quarter = populations.at(stock_numbers.at(i))->get_tot_F_at_age_last_quarter();

        // get rid of the cumul with a diff with last quarter
        for(int a=0; a<nb_ages_in_sms; a++)
        {
            tot_F_at_age[a]=tot_F_at_age[a] - tot_F_at_age_last_quarter[a];
        }

        // write F.in SMS files
        dout(cout << "write down the SMS f.in file...");

        for(int a = 0; a < nb_ages_in_sms; a++)
        {
            SMS_F_in  << tot_F_at_age.at(a) << " " ;
        }
        SMS_F_in << endl;
    }

}


void read_SMS_OP_N_out_file(vector<Population* >& populations,
                            vector<int> stock_numbers,
                            vector<int> some_units,
                            vector<int> some_max_nb_ages,
                            string sms_folder,
                            string namesimu)
{
    UNUSED(some_max_nb_ages);

    // read the input file
    string filename=  sms_folder+"/"+namesimu+"/op_n.out";


    ifstream in;
    in.open(filename.c_str());
    if(in.fail())
    {
        open_file_error(filename);
        //return 1;
    }

    // cod
    vector<double> a_vector_line2;
    // herring
    vector<double> a_vector_line3;
    // sprat
    vector<double> a_vector_line4;

    string line;
    int counter=0;
    while(!getline(in, line).eof())
    {
        std::stringstream linestream(line);
        counter+=1;
        if(counter==2)
        {
            double val;
            while(linestream >> val)
            {
                a_vector_line2.push_back(val*some_units.at(0));
            }
        }

        if(counter==3)
        {
            double val;
            while(linestream >> val)
            {
                a_vector_line3.push_back(val*some_units.at(1));
            }
        }
        if(counter==4)
        {
            double val;
            while(linestream >> val)
            {
                a_vector_line4.push_back(val*some_units.at(2));
            }
        }
    }
    dout(cout << "read SMS op_n.out...OK" << endl << flush);

    // caution: SMS OP provides 8 age classes while DISPLACE deals with NBAGE (e.g. 11 age classes)
    // so, adjustment needed:
    while (a_vector_line2.size()<NBAGE)
    {
        a_vector_line2.push_back(0);
    }
    while (a_vector_line3.size()<NBAGE)
    {
        a_vector_line3.push_back(0);
    }
    while (a_vector_line4.size()<NBAGE)
    {
        a_vector_line4.push_back(0);
    }
    dout(cout << "make nb of age classes consistent...OK" << endl << flush);

    // assign to the population objects
    for (unsigned int i=0; i<stock_numbers.size(); i++)
    {

        vector<double> tot_N_at_age;
        if(i==0) tot_N_at_age=a_vector_line2 ; // cod - 10
        if(i==1) tot_N_at_age=a_vector_line3 ; // herring - 3
        if(i==2) tot_N_at_age=a_vector_line4 ; // sprat - 7

        populations.at( stock_numbers.at(i) )->set_tot_N_at_age(tot_N_at_age);

        vector< vector<double> > percent_age_per_szgroup_matrix(NBSZGROUP, vector<double>(NBAGE));
        percent_age_per_szgroup_matrix=populations.at( stock_numbers.at(i) )->get_percent_age_per_szgroup_matrix();

        // convert back to size classes
        // (also accounting for the units.)
        vector<double>N_at_szgroup(NBSZGROUP);
        for(unsigned int a=0; a<tot_N_at_age.size(); a++)
        {
            for(unsigned int sz=0; sz<N_at_szgroup.size(); sz++)
            {
                N_at_szgroup[sz]+=percent_age_per_szgroup_matrix[sz][a] *tot_N_at_age[a];

                /*
                if(i==0)
                {
                    dout(cout << "age" << a << " sz" << sz << endl);
                    dout(cout << N_at_szgroup[sz] << "+= " << percent_age_per_szgroup_matrix[sz][a] << " * " << tot_N_at_age[a] << endl);
                }*/
            }
        }

        // ...and fill in with the predicted N
        populations.at(stock_numbers.at(i))->set_tot_N_at_szgroup(N_at_szgroup);

    }
    dout(cout << "set Ns at szgroup from the SMS outcomes...OK" << endl << flush);

}


string getLastErrorMessage()
{
    return error_msg;
}


multimap<int, int> read_metier_target_stocks(int a_met, string folder_name_parameterization, string inputfolder)
{

    // casting a_met into a string
    stringstream out;
    out << a_met;
    string a_met_s = out.str();

    string filename = inputfolder + "/metiersspe_" + folder_name_parameterization + "/met_target_names.dat";

    ifstream file_metier_target_stocks;
    file_metier_target_stocks.open(filename.c_str());
    if (file_metier_target_stocks.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, int> metier_target_stocks;
    fill_multimap_from_specifications_i_i(file_metier_target_stocks, metier_target_stocks);
    file_metier_target_stocks.close();

    return (metier_target_stocks);
}

multimap<int, int>
read_metier_suitable_seabottomtypes(int a_met, string folder_name_parameterization, string inputfolder)
{

    // casting a_met into a string
    stringstream out;
    out << a_met;
    string a_met_s = out.str();

    string filename =
            inputfolder + "/metiersspe_" + folder_name_parameterization + "/metier_suitable_seabottomtypes.dat";

    ifstream file_metier_suitable_seabottomtypes;
    file_metier_suitable_seabottomtypes.open(filename.c_str());
    if (file_metier_suitable_seabottomtypes.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, int> metier_suitable_seabottomtypes;
    fill_multimap_from_specifications_i_i(file_metier_suitable_seabottomtypes, metier_suitable_seabottomtypes);
    file_metier_suitable_seabottomtypes.close();

    return (metier_suitable_seabottomtypes);
}


// FOR METIER
map<int, int> read_metiers_types(string folder_name_parameterization, string inputfolder)
{

    string filename = inputfolder + "/metiersspe_" + folder_name_parameterization + "/combined_met_types.dat";

    ifstream metier_types;
    metier_types.open(filename.c_str());
    if (metier_types.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    map<int, int> met_types;
    fill_map_from_specifications_i_i(metier_types, met_types, folder_name_parameterization);
    metier_types.close();

    return (met_types);
}


// FOR METIER
map<int, double> read_percent_revenue_completenesses(string folder_name_parameterization, string inputfolder)
{

    string filename =
            inputfolder + "/metiersspe_" + folder_name_parameterization + "/percent_revenue_completenesses.dat";

    ifstream metier_percent_revenue_completenesses;
    metier_percent_revenue_completenesses.open(filename.c_str());
    if (metier_percent_revenue_completenesses.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    map<int, double> percent_revenue_completenesses;
    fill_map_from_specifications_i_d(metier_percent_revenue_completenesses, percent_revenue_completenesses,
                                     folder_name_parameterization);
    metier_percent_revenue_completenesses.close();

    return (percent_revenue_completenesses);
}


// FOR METIER
map<int, double> read_metiers_fspeed(string folder_name_parameterization, string inputfolder)
{

    string filename = inputfolder + "/metiersspe_" + folder_name_parameterization + "/metier_fspeed.dat";

    ifstream metier_fspeed;
    metier_fspeed.open(filename.c_str());
    if (metier_fspeed.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    map<int, double> fspeed;
    fill_map_from_specifications_i_d(metier_fspeed, fspeed, folder_name_parameterization);
    metier_fspeed.close();

    return (fspeed);
}


// FOR METIER
map<int, double> read_gear_widths_param_a(string folder_name_parameterization, string inputfolder)
{

    string filename = inputfolder + "/metiersspe_" + folder_name_parameterization + "/metier_gear_widths_param_a.dat";

    ifstream metier_gear_widths_param_a;
    metier_gear_widths_param_a.open(filename.c_str());
    if (metier_gear_widths_param_a.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }

    //cout << "here:" << filename << endl;

    map<int, double> gear_widths_param_a;
    fill_map_from_specifications_i_d(metier_gear_widths_param_a, gear_widths_param_a, folder_name_parameterization);
    metier_gear_widths_param_a.close();

    //cout << "here:" << gear_widths_param_a[0] << endl;
    //cout << "here:" << gear_widths_param_a[1] << endl;
    //=> do not forget the heading in input files given first line is skipped



    return (gear_widths_param_a);
}


// FOR METIER
map<int, double> read_gear_widths_param_b(string folder_name_parameterization, string inputfolder)
{

    string filename = inputfolder + "/metiersspe_" + folder_name_parameterization + "/metier_gear_widths_param_b.dat";

    ifstream metier_gear_widths_param_b;
    metier_gear_widths_param_b.open(filename.c_str());
    if (metier_gear_widths_param_b.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    map<int, double> gear_widths_param_b;
    fill_map_from_specifications_i_d(metier_gear_widths_param_b, gear_widths_param_b, folder_name_parameterization);
    metier_gear_widths_param_b.close();

    return (gear_widths_param_b);
}


// FOR METIER
map<int, string> read_gear_widths_model_type(string folder_name_parameterization, string inputfolder)
{

    string filename =
            inputfolder + "/metiersspe_" + folder_name_parameterization + "/metier_gear_widths_model_type.dat";

    ifstream metier_gear_widths_model_type;
    metier_gear_widths_model_type.open(filename.c_str());
    if (metier_gear_widths_model_type.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    map<int, string> gear_widths_model_type;
    fill_map_from_specifications_i_s(metier_gear_widths_model_type, gear_widths_model_type,
                                     folder_name_parameterization);
    metier_gear_widths_model_type.close();

    return (gear_widths_model_type);
}


// FOR METIER
multimap<int, double> read_dis_ogives(string folder_name_parameterization, string inputfolder)
{

    string filename = inputfolder + "/metiersspe_" + folder_name_parameterization + "/metier_discards_ogives.dat";

    ifstream metier_discards_ogives;
    metier_discards_ogives.open(filename.c_str());
    if (metier_discards_ogives.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> dis_ogives;
    fill_multimap_from_specifications_i_d(metier_discards_ogives, dis_ogives);
    metier_discards_ogives.close();

    return (dis_ogives);
}


multimap<int, double> read_metiers_betas(string a_semester, string folder_name_parameterization, string inputfolder)
{

    string filename =
            inputfolder + "/metiersspe_" + folder_name_parameterization + "/metierspe_betas_semester" + a_semester +
            ".dat";

    ifstream metierspe_betas_file;
    metierspe_betas_file.open(filename.c_str());
    if (metierspe_betas_file.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> metiers_betas;
    fill_multimap_from_specifications_i_d(metierspe_betas_file, metiers_betas);
    metierspe_betas_file.close();

    return (metiers_betas);
}

multimap<int, double>
read_discardratio_limits(string a_semester, string folder_name_parameterization, string inputfolder)
{

    string filename =
            inputfolder + "/metiersspe_" + folder_name_parameterization + "/metierspe_discardratio_limits_semester" +
            a_semester + ".dat";

    ifstream discardratio_limits_file;
    discardratio_limits_file.open(filename.c_str());
    if (discardratio_limits_file.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> discardratio_limits;
    fill_multimap_from_specifications_i_d(discardratio_limits_file, discardratio_limits);
    discardratio_limits_file.close();

    return (discardratio_limits);
}


multimap<int, int> read_is_avoided_stocks(string a_semester, string folder_name_parameterization, string inputfolder)
{

    string filename =
            inputfolder + "/metiersspe_" + folder_name_parameterization + "/metierspe_is_avoided_stocks_semester" +
            a_semester + ".dat";

    ifstream is_avoided_stocks_file;
    is_avoided_stocks_file.open(filename.c_str());
    if (is_avoided_stocks_file.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, int> is_avoided_stocks;
    fill_multimap_from_specifications_i_i(is_avoided_stocks_file, is_avoided_stocks);
    is_avoided_stocks_file.close();

    return (is_avoided_stocks);
}


multimap<int, double>
read_loss_after_1_passage_per_landscape_per_func_group(int a_met, string folder_name_parameterization,
                                                       string inputfolder)
{

    // casting a_met into a string
    stringstream out;
    out << a_met;
    string a_met_s = out.str();

    string filename = inputfolder + "/metiersspe_" + folder_name_parameterization + "/" + a_met_s +
                      "loss_after_one_passage_per_landscape_per_func_group.dat";

    ifstream metiers_loss_after_one_passage_per_landscape_per_func_group;
    metiers_loss_after_one_passage_per_landscape_per_func_group.open(filename.c_str());
    if (metiers_loss_after_one_passage_per_landscape_per_func_group.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> metiers_loss_after_one_passage;
    fill_multimap_from_specifications_i_d(metiers_loss_after_one_passage_per_landscape_per_func_group,
                                          metiers_loss_after_one_passage);
    metiers_loss_after_one_passage_per_landscape_per_func_group.close();

    return (metiers_loss_after_one_passage);
}


multimap<int, int> read_metiers_mls_cat(string a_semester, string folder_name_parameterization, string inputfolder)
{

    string filename =
            inputfolder + "/metiersspe_" + folder_name_parameterization + "/metierspe_mls_cat_semester" + a_semester +
            ".dat";

    ifstream metierspe_mls_cat_file;
    metierspe_mls_cat_file.open(filename.c_str());
    if (metierspe_mls_cat_file.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, int> metierspe_mls_cat;
    fill_multimap_from_specifications_i_i(metierspe_mls_cat_file, metierspe_mls_cat);
    metierspe_mls_cat_file.close();

    return (metierspe_mls_cat);
}


vector<vector<double> > read_selectivity_per_stock_ogives(int a_met,
                                                          int nbpops,
                                                          int nbszgroup,
                                                          string folder_name_parameterization,
                                                          string inputfolder,
                                                          string fleetsce)
{

    // casting a_met into a string
    stringstream out;
    out << a_met;
    string a_met_s = out.str();

    string filename = inputfolder + "/metiersspe_" + folder_name_parameterization + "/" + a_met_s +
                      "metier_selectivity_per_stock_ogives_fleetsce" + fleetsce + ".dat";

    ifstream file_selectivity_per_stock_ogives;
    file_selectivity_per_stock_ogives.open(filename.c_str());
    if (file_selectivity_per_stock_ogives.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    vector<vector<double> > selectivity_per_stock_ogives(nbpops, vector<double>(nbszgroup));
    if (!fill_in_selectivity_per_stock(file_selectivity_per_stock_ogives, selectivity_per_stock_ogives)) {
        throw std::runtime_error("Error while executuing: fill_in_selectivity_per_stock");
    }

    file_selectivity_per_stock_ogives.close();

    return (selectivity_per_stock_ogives);
}


vector<vector<double> > read_selectivity_per_stock_ogives_for_oth_land(int nbpops,
                                                                       int nbszgroup,
                                                                       string folder_name_parameterization,
                                                                       string inputfolder,
                                                                       string fleetsce)
{

    string filename = inputfolder + "/metiersspe_" + folder_name_parameterization +
                      "/metier_selectivity_per_stock_ogives_fleetsce" + fleetsce + "_for_oth_land.dat";

    ifstream file_selectivity_per_stock_ogives_for_oth_land;
    file_selectivity_per_stock_ogives_for_oth_land.open(filename.c_str());
    if (file_selectivity_per_stock_ogives_for_oth_land.fail()) {
        vector<vector<double> > selectivity_per_stock_ogives_for_oth_land;
        return selectivity_per_stock_ogives_for_oth_land; // caution: returns an empty object
    }
    vector<vector<double> > selectivity_per_stock_ogives_for_oth_land(nbpops, vector<double>(nbszgroup));
    if (!fill_in_selectivity_per_stock(file_selectivity_per_stock_ogives_for_oth_land,
                                       selectivity_per_stock_ogives_for_oth_land)) {
        throw std::runtime_error("Error while executuing: fill_in_selectivity_per_stock");
    }

    file_selectivity_per_stock_ogives_for_oth_land.close();

    return (selectivity_per_stock_ogives_for_oth_land);
}

