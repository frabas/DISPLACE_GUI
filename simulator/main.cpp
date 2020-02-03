// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2020 Francois Bastardie <fba@aqua.dtu.dk>

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


/*! \mainpage A spatial model of fisheries to help sustainable fishing and maritime spatial planning
 *
 * \section intro_sec Introduction
 *
 * DISPLACE is a dynamic, individual-based model for spatial fishing planning and effort displacement
 * integrating underlying fish population models. The DISPLACE project develops and provides a
 * platform primarily for research purposes to transform the fishermen detailed knowledge into models,
 * evaluation tools, and methods that can provide the fisheries with research and advice.
 * By conducting a ecological and socio-economic impact assessment the model intends to serve as an aid to decision-making for (fishery) managers.
 * An impact assessment will help answering on what are the impacts of the different policy options and who will be affected.
 * By quantifying the effects the assessment will measure how the different options compare, for example
 * how different the options perform in achieving the objective(s) of the policy.
 *
 * \section Installation
 *
 * The DISPLACE software is available for download at www.displace-project.org.
 * Installation procedures for windows, Linux and iOS platforms are briefly described on this web site.
 *
 */


#include "idtypes.h"

#include "SimModel.h"
#include "storage/sqliteoutputstorage.h"
#include "storage/tables/vesseldeftable.h"
#include "storage/tables/vesselvmslikefpingsonlytable.h"
#include "storage/tables/nodesdeftable.h"
#include "storage/tables/poptable.h"
#include "storage/modelmetadataaccessor.h"
#include "utils/safe_strerror.h"

#include <msqlitecpp/v1/sqlitestorage.h>

#include <boost/filesystem.hpp>

using namespace sqlite;

#include "msqlitecpp/v2/storage.h"

#include <helpers.h>
#include <assert.h>

#include <utils/CrashHandler.h>
#include <getrss.h>

#if defined (__linux)
#include <memstats.h>
#endif


// for Windows
#ifdef _WIN32
                                                                                                                        #include <windows.h>
#include <direct.h>
#define GetCurrentDir _getcwd
#else

#include <unistd.h>

#define GetCurrentDir getcwd
#endif

#if !defined(__WIN32) && defined(INSTRUMENTATION)
#include <valgrind/callgrind.h>
#else
#define CALLGRIND_START_INSTRUMENTATION
#define CALLGRIND_STOP_INSTRUMENTATION
#define CALLGRIND_DUMP_STATS
#endif

#include <thread_vessels.h>
#include <dtree/decisiontreemanager.h>
#include <comstructs.h>
#include <simulation.h>
#include <tseries/timeseriesmanager.h>
#include "shortestpath/GeoGraph.h"
#include "shortestpath/GeoGraphLoader.h"

#ifndef NO_IPC

#include <ipc.h>
#include <outputqueuemanager.h>
#include <outputmessage.h>
#include <messages/genericconsolestringoutputmessage.h>

#endif

#include <biomodule2.h>
#include <fisheriesmanagmt.h>

#include <iomanip>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <map>
#include <list>
#include <set>
#include <cstdlib>
#include <cstdio>
#include <stdio.h>                 /* defines FILENAME_MAX */
#include <stdlib.h>                 /* system, NULL, EXIT_FAILURE */
#include <algorithm>
#include <ctime>
#include <cmath>
#include <cstring>
#include <errno.h>
#include <mkpath.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <values.h>
#include "Node.h"
#include "Benthos.h"
#include "Metier.h"
#include "Vessel.h"
#include "Firm.h"
#include "Ship.h"
#include "Population.h"
#include "Fishfarm.h"
#include "Windmill.h"
#include "Harbour.h"
#include "diffusion.h"

#include <outputexporter.h>

#include "readdata.h"
#include "myutils.h"
#include <memoryinfo.h>

#include "dataloader.h"
#include "dataloaderbenthos.h"
#include "dataloadervessels.h"
#include "dataloaderfishfarms.h"
#include "dataloaderwindmills.h"
#include "dataloadercommercialships.h"
#include "dataloaderpops.h"
#include "dataloadermetiers.h"


#ifdef PROFILE
#include <profiler.h>
#endif

#include <version.h>
#include <mutex>
#include <chrono>
#include <runtime_tests/NodeTester.h>
#include <TextfileModelLoader.h>

#include "boost/bind.hpp"
#include "boost/program_options.hpp"

//#include <boost/filesystem.hpp>


// for rtree
namespace bg = boost::geometry;
namespace bgi = boost::geometry::index;

typedef bg::model::point<double, 2, bg::cs::cartesian> point;
typedef bg::model::box<point> box;
typedef std::pair<box, unsigned> value;


using namespace std;

namespace po = boost::program_options;
namespace sql = msqlitecpp::v2;

// global variables
#ifdef _WIN32
                                                                                                                        FILE *pipe2;
FILE *pipe3;
FILE *pipe4;
#endif

#define MAXPATH 1024

std::string cwd;
char buf[MAXPATH];

#ifdef PROFILE
                                                                                                                        AverageProfiler mLoopProfile;
AverageProfiler mVesselLoopProfile;
AverageProfiler mPopExportProfile;
Profiler mLoadProfile;
double mLoadNodesProfileResult;
double mLoadVesselProfileResult;
double mLoadPopulationProfileResult;
double mLoadGraphProfileResult;
#endif

MemoryInfo memInfo;

//OutputQueueManager mOutQueue(std::cout);  // Use Text stream

std::mutex glob_mutex;
vector<int> ve;
vector<Vessel *> vessels;
vector<Ship *> ships;
vector<Benthos *> benthoss;
vector<Population *> populations;
vector<Fishfarm *> fishfarms;
vector<Windmill *> windmills;
int tstep;
int a_graph;
types::NodeId a_port;
int nrow_coord;
int nrow_graph;
vector<double> graph_res;
bool check_all_stocks_before_going_fishing;
vector<int> tariff_pop;
int freq_update_tariff_code;
int update_tariffs_based_on_lpue_or_dpue_code;
int freq_do_growth;
int freq_redispatch_the_pop;
vector<double> arbitary_breaks_for_tariff;
int total_amount_credited;
double tariff_annual_hcr_percent_change;

double tech_creeping_multiplier = 1;
bool enable_sqlite_out = true;
std::string outSqlitePath;

std::shared_ptr<SQLiteOutputStorage> outSqlite = nullptr;

bool use_dtrees;
vector<double> global_quotas_uptake;
vector<int> explicit_pops;
DynAllocOptions dyn_alloc_sce;
PopSceOptions dyn_pop_sce;
ClosureOptions closure_opts;
string biolsce;
string fleetsce;
adjacency_map_t adjacency_map;
vector<string> vertex_names;
vector<types::NodeId> relevant_nodes;
multimap<int, int> nodes_in_polygons;
multimap<types::NodeId, int> possible_metiers;
multimap<types::NodeId, double> freq_possible_metiers;
multimap<types::NodeId, double> gshape_cpue_per_stk_on_nodes;
multimap<types::NodeId, double> gscale_cpue_per_stk_on_nodes;
vector<types::NodeId> spe_fgrounds;
vector<types::NodeId> spe_fgrounds_init;
vector<types::NodeId> spe_harbours;
vector<double> spe_freq_fgrounds;
vector<double> spe_freq_fgrounds_init;
vector<double> spe_freq_harbours;
vector<double> spe_vessel_betas_per_pop;
vector<double> spe_percent_tac_per_pop;
vector<double> spe_fishing_credits;
multimap<types::NodeId, string> harbour_names;
vector<int> name_metiers;
ofstream freq_cpue;
ofstream freq_profit;
ofstream freq_distance;
ofstream vmslike2;
ofstream vmslike3;
vector<Metier *> metiers;
ofstream export_individual_tacs;
vector<PathShop> pathshops;
ofstream fishfarmslogs;
ofstream windmillslogs;
ofstream shipslogs;
vector<vector<double> > selectivity_per_stock_ogives_for_oth_land;

std::mutex listVesselMutex;
vector<int> listVesselIdForVmsLikeToExport;
vector<int> listVesselIdForVmsLikeFPingsOnlyToExport;
vector<int> listVesselIdForLogLikeToExport;
vector<int> listVesselIdForTripCatchPopPerSzgroupExport;

std::shared_ptr<sql::Storage> indb;

ParamsForLoad paramsForLoad;

#ifdef NO_IPC
#include <messages/noipc.h>
#endif

static size_t numStepTransactions = 200;

//vector <double> dist_to_ports;

/* GUI Protocol
 *
 * All cout strings that begins by the control character "=", will be treated as GUI protocol strings.
 * These will be sent only if --use-gui command line option is present.
 * The control character will be followed by a 1 character command code. Then the argument of the command will
 * follow.
 *
 * NOTE: All fields are ALWAYS separated by SPACES
 *
 * These are the commands:
 *
 * =Snnnn       Simulation Step nnnn (int) has been performed
 * =Vxxxx       Vessel has moved. xxxx is a multifield string, separated by spaces:
 *                   id,x,y,course,fuel,state
 * =v...        Vessel statistics. see loglike_*.dat
 * =Upath[ tstep]       Output file has been updated. path is the absolute path of the file.
 *                  optionally: includes the current sim step
 * =Ndata       Nodes stats update: format
 *                  stat,tstep,first,number,data...
 *              stat can be:
 *                  cumftime
 * =Dvxxx yyy ...   Debug / Profile information.
 *                      m: Memory info, RSS values (kb), Peak (kb)
 *                      c+: capture on
 *                      c-: capture off
 */

/* Command line arguments
 *
 * --use-gui                Enables the GUI protocol through stdout
 * --no-gui-move-vessels    Disables sending the Vessel position update command (=V) when using GUI
 * --rate                   Vessel Move Display Updating Rate
 * --num_threads x          Parallelize jobs by spawning x threads
 *
 * */

static void lock()
{
    glob_mutex.lock();
}

static void unlock()
{
    glob_mutex.unlock();
}

bool load_relevant_nodes(string folder_name_parameterization, string inputfolder, string ftype, string a_quarter,
                         set<types::NodeId> &nodes)
{
    string filename =
            inputfolder + "/vesselsspe_" + folder_name_parameterization + "/vesselsspe_" + ftype + "_" + a_quarter +
            ".dat";
    ifstream in;
    in.open(filename.c_str());
    if (in.fail()) {
        open_file_error(filename.c_str());
        return false;
    }

    string line;
    string vessel_name;
    int node;
    int n = 0;
    while (!in.eof()) {
        getline(in, line);
        ++n;
        if (n == 1) {
            continue;
        }
        std::stringstream ss(line);
        ss >> vessel_name;
        ss >> node;
        nodes.insert(types::NodeId(node));
    }

    outc(cout << "Loaded: " << filename << " " << n << " lines, " << nodes.size() << " relevant nodes");

    in.close();
    return true;
}

bool load_relevant_nodes(string folder_name_parameterization, string inputfolder, vector<types::NodeId> &ret)
{
    set<types::NodeId> nodes;

    if (!load_relevant_nodes(folder_name_parameterization, inputfolder, "fgrounds", "quarter1", nodes)) {
        return false;
    }
    if (!load_relevant_nodes(folder_name_parameterization, inputfolder, "fgrounds", "quarter2", nodes)) {
        return false;
    }
    if (!load_relevant_nodes(folder_name_parameterization, inputfolder, "fgrounds", "quarter3", nodes)) {
        return false;
    }
    if (!load_relevant_nodes(folder_name_parameterization, inputfolder, "fgrounds", "quarter4", nodes)) {
        return false;
    }
    if (!load_relevant_nodes(folder_name_parameterization, inputfolder, "harbours", "quarter1", nodes)) {
        return false;
    }
    if (!load_relevant_nodes(folder_name_parameterization, inputfolder, "harbours", "quarter2", nodes)) {
        return false;
    }
    if (!load_relevant_nodes(folder_name_parameterization, inputfolder, "harbours", "quarter3", nodes)) {
        return false;
    }
    if (!load_relevant_nodes(folder_name_parameterization, inputfolder, "harbours", "quarter4", nodes)) {
        return false;
    }

    auto it = nodes.begin();
    while (it != nodes.end()) {
        ret.push_back(types::NodeId(*it));
        ++it;
    }

    sort(ret.begin(), ret.end());
    unique(ret.begin(), ret.end());
    return true;
}


bool test_not_belong_to_firm(const Vessel *v, int id)
{
    return v->get_firm_id() != id;
}


vector<string> type_of_avai_field_to_read;


// parameters

bool crash_handler_enabled = true;
// default
string outdir;

string namefolderinput = "fake";
string namefolderoutput = "baseline";
string inputfolder = ".";
string namesimu = "sim1";
string inputdb;
int nbsteps = 10;
double dparam = 10.0;
int use_static_paths = 0;//used to speed-up the simus by using reduced (to minimal required) "previous" maps
int selected_vessels_only = 0; //use all vessels. if 1, then use a subset of vessels as defined in read_vessel_features()
bool use_gui = false;
bool gui_move_vessels = true;
bool use_gnuplot = false;
int num_threads = 4;
int nb_displayed_moves_out_of_twenty = 1;
int export_vmslike = 1;
int export_hugefiles = 1;

void parseCommandLine(int argc, char const *argv[])
{
    po::options_description desc("Allowed options");
    desc.add_options()
            ("help,h", "produce help message")
            (",f", po::value(&namefolderinput), "name of the input folder")
            ("f2,F", po::value(&namefolderoutput), "name of the output folder")
            (",a", po::value(&inputfolder), "Path of the input path")
            ("outdir,O", po::value(&outdir), "Directory for output files")
            (",s", po::value(&namesimu), "name of the simulation")
            (",i", po::value(&nbsteps), "lenght of the simulation in number of steps (hours)")
            (",V", po::value(&verbosity), "verbosity level")
            (",p", po::value(&use_static_paths), "Use static paths")
            (",e", po::value(&export_vmslike), "Export VMSLike data")
            ("huge", po::value(&export_hugefiles), "Export huge files data")
            (",v", po::value(&selected_vessels_only), "Selected vessels only")
            (",d", po::value(&dparam), "dparam")
            ("indb", po::value(&inputdb), "Read input data from sqlite db")
            ("commit-rate", po::value(&numStepTransactions),
             "Modify the number of loops before committing to sqlite db")
            ("use-gui", "Enable IPC channel to talk to the GUI")
            ("no-gui-move-vessels", "Disable the movement of the vessels/ships in the GUI")
            ("disable-sqlite", "Disable the SQLite output")
            ("num_threads", po::value(&num_threads), "Number of threads to use to move vessels")
            ("rate", po::value(&nb_displayed_moves_out_of_twenty), "Nb of displayed moves of of 20")
            ("disable-crash-handler", "Disable the crash handler")
            ("debug", "Enable debug mode")
            ("without-gnuplot", "Maintained for compatibility, no functions");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);

    if (vm.count("help")) {
        std::cout << desc << "\n";
        exit(0);
    }

    if (vm.count("use-gui")) {
        use_gui = true;
    }
    if (vm.count("no-gui-move-vessels")) {
        gui_move_vessels = false;
    }
    if (vm.count("disable-sqlite")) {
        enable_sqlite_out = false;
    }
    if (vm.count("disable-crash-handler")) {
        crash_handler_enabled = false;
    }
    if (vm.count("debug")) {
        crash_handler_enabled = false;
    }
}




/**---------------------------------------------------------------**/
/**---------------------------------------------------------------**/
/**---------------------------------------------------------------**/
/**---------------------------------------------------------------**/
/**---------------------------------------------------------------**/
/**---------------------------------------------------------------**/
/**---------------------------------------------------------------**/
/**---------------------------------------------------------------**/
/**---------------------------------------------------------------**/
/**---------------------------------------------------------------**/
/**---------------------------------------------------------------**/
/**---------------------------------------------------------------**/
int app_main(int argc, char const *argv[])
{
#ifdef _WIN32
    string outdir="C:";
#else
    bool DTU_HPC_SCRATCH = false;
    string home;
    if (DTU_HPC_SCRATCH) {
        home = "/SCRATCH/fbas"; // => DTU SCRATCH for HPC
    } else {
        home = getenv("HOME");
    }
    outdir = home + "/ibm_vessels";
#endif

    // example for setting up options for the command line
    // (in code::blocks, see Project>Set programs arguments in code::blocks menu)
    // e.g.  -f "balticonly" --f2 "baseline" -s "simu1" -i 8761 -p 1 -e 0 --huge 1 -v 0 --without-gnuplot
    // -f "balticonly" --f2 "baseline" -s "simu2" -i 8761 -p 1 -e 0 --huge 1 -v 0 --without-gnuplot  // disable the VMS file exporting, the most used.
    // -f "balticonly" --f2 "baseline"  -s "simu2" -i 8761 -p 1 -e 1 --huge 1 -v 0 --with-gnuplot    // enable the VMS file exporting
    // -f "balticonly" --f2 "baseline"  -s "simu2" -i 8761 -p 1 -e 1 --huge 1 -v 1 --with-gnuplot    // subset of vessels, see features.dat
    // -f "balticonly" --f2 "baseline"  -s "simu2" -i 8761 -p 0 -e 1 --huge 1 -v 0 --with-gnuplot    // here, dynamic path building: use with care because might need much more computation time...

    // -V xxx  Sets level of verbosity  (default: 0)
    // --use-gui => emits machine parsable data to stdout
    // --disable-crashhandler or --debug => Disables crash handling code.

    /* run in command line with:
      C:\Users\fbas\Documents\GitHub\DISPLACE_GUI\build\release>displace -f "balticonly" --f2 "baseline" -s
      "simu2" -i 8761 -p 1 -e 0 --huge 1 -v 0 --without-gnuplot -V 2 --num_threads 1 > output.txt
    */

    cout << "This is displace, version " << VERSION << " build " << VERSION_BUILD << endl;

    auto simModel = std::make_shared<SimModel>();

    memInfo.update();
    guiSendMemoryInfo(memInfo);

    parseCommandLine(argc, argv);

    CrashHandler handler;
    if (crash_handler_enabled) {
        handler.initialize();
    }

    initIpcQueue();
    thread_vessel_init(num_threads, simModel);

    cwd = std::string(getcwd(buf, MAXPATH));

    lock();
    cout << " nbsteps " << nbsteps
         << " namefolderinput " << namefolderinput << " " << use_static_paths << endl;
    unlock();

    simModel->setQuarter(1);
    simModel->setMonth(1);
    simModel->setSemester(1);

    //	if(namefolderinput=="fake") inputfolder="DISPLACE_input_test";

    // misc.
    string filename;

    // get the name of the input directory for this simu
    string folder_name_parameterization = namefolderinput;

    std::unique_ptr<ModelLoader> modelLoader;

    if (!inputdb.empty()) {
        boost::filesystem::path inpath = boost::filesystem::path{inputfolder} / inputdb;
        inpath = boost::filesystem::absolute(inpath);
        std::cout << "Loading input db: " << inpath.string() << "\n";

        if (!boost::filesystem::exists(inpath)) {
            std::cerr << "Input file " << inputdb << " doesn't exist.\n";
            return 2;
        }

        indb = std::make_shared<sql::Storage>(inpath.string(), sql::Storage::OpenMode::DelayedOpen);
        indb->open();

        // TODO instantiate here the model loader
        throw std::logic_error("Db Model Loader not implemented!");
    } else {
        modelLoader = std::make_unique<TextfileModelLoader>(simModel,
                                                            folder_name_parameterization, inputfolder,
                                                            namefolderoutput);
    }

    // create a specific output directory for the ibm outcomes
    string an_output_folder;
    string a_basic_output_folder;
    string namefolder;

#ifdef _WIN32
                                                                                                                            an_output_folder= outdir+"/DISPLACE_outputs";
    mkdir(an_output_folder.c_str());


    a_basic_output_folder= outdir+"/DISPLACE_outputs/"+namefolderinput;
    mkdir(a_basic_output_folder.c_str());
    // create a specific output directory for this simu
    namefolder= outdir+"/DISPLACE_outputs/"+namefolderinput+"/"+namefolderoutput;
    mkdir(namefolder.c_str());
#else

    // not Windows eg Linux on HPC DTU
    int status;
    an_output_folder = outdir + "/DISPLACE_outputs";

    status = mkpath(an_output_folder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (status < 0) {
        cerr << "could not create directory" << an_output_folder << ": " << safe_strerror(errno) << endl;
        return -1;
    }

    a_basic_output_folder = outdir + "/DISPLACE_outputs/" + namefolderinput;
    status = mkpath(a_basic_output_folder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);

    if (status < 0) {
        cerr << "could not create directory" << a_basic_output_folder << ": " << safe_strerror(errno) << endl;
        return -1;
    }

    // create a specific output directory for this simu
    namefolder = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput;
    status = mkpath(namefolder.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
    if (status < 0) {
        cerr << "could not create directory" << namefolder << ": " << safe_strerror(errno) << endl;
        return -1;
    }
#endif


#ifdef _WIN32
                                                                                                                            // for gnuplot installed for at least MinGW_with_gcc_4.6.2
const char *const path = "\"C:\\Program Files (x86)\\gnuplot\\bin\\gnuplot\"";
#else
    char *path = 0;
#endif


    // for the file name if stochastic variation on availability keys
    string str_rand_avai_file;

    // simModel->config().nbpops, etc.: caution= config numbers here, read from config files.
    // a mistake in these files are of great consequences.
    string a_graph_name = "a_graph";

    if (!modelLoader->loadConfig()) {
        // config at the simusspe level
        cerr << "Cannot read Config File: Bad format\n";
        return 2;
    }

    if (!modelLoader->loadScenario()) {
        cerr << "Cannot read scenario config file, Bad Format.\n";
        return 2;
    }

    auto const &scenario = simModel->scenario();

    dyn_alloc_sce = scenario.dyn_alloc_sce;
    dyn_pop_sce = scenario.dyn_pop_sce;
    biolsce = scenario.biolsce;
    fleetsce = scenario.fleetsce;
    freq_do_growth = scenario.freq_do_growth;
    freq_redispatch_the_pop = scenario.freq_redispatch_the_pop;
    a_graph = scenario.a_graph;
    nrow_coord = scenario.nrow_coord;
    nrow_graph = scenario.nrow_graph;
    a_port = scenario.a_port;
    graph_res = scenario.graph_res;
    check_all_stocks_before_going_fishing = scenario.check_all_stocks_before_going_fishing;
    use_dtrees = scenario.use_dtrees;
    if (dyn_alloc_sce.option(Options::fishing_credits)) {
        tariff_pop = scenario.tariff_pop;
        freq_update_tariff_code = scenario.freq_update_tariff_code;
        update_tariffs_based_on_lpue_or_dpue_code = scenario.update_tariffs_based_on_lpue_or_dpue_code;
        arbitary_breaks_for_tariff = scenario.arbitary_breaks_for_tariff;
        total_amount_credited = scenario.total_amount_credited;
        tariff_annual_hcr_percent_change = scenario.tariff_annual_hcr_percent_change;
    }


    stringstream graphnum;
    graphnum << a_graph;
    a_graph_name = a_graph_name + graphnum.str();

    // Create simulation scenario
    displace::simulation::Simulation *sim_scenario = displace::simulation::Simulation::instance();

    if (use_dtrees) {
        if (!sim_scenario->loadTimeSeries(inputfolder + "/timeseries", "")) {
            std::cerr << "Cannot read time series. aborting." << std::endl;
            return -1;
        }

        // Load dtrees
        if (dtree::DecisionTreeManager::manager()->readFromScenario(inputfolder + "/dtrees", scenario) <= 0) {
            std::cerr << "Cannot read decision trees, aborting." << std::endl;
            return -1;
        }
    }

    // check if config reading OK
    outc(cout << simModel->config().nbpops << endl);
    for (unsigned int a_pop = 0; a_pop < simModel->config().implicit_pops.size(); a_pop++) {
        outc(cout << " " << simModel->config().implicit_pops.at(a_pop));
    }
    outc(cout << endl);
    for (int a_pop = 0; a_pop < simModel->config().nbpops; a_pop++) {
        outc(cout << " " << simModel->config().calib_oth_landings.at(a_pop));
    }
    outc(cout << endl);
    for (int a_pop = 0; a_pop < simModel->config().nbpops; a_pop++) {
        outc(cout << " " << simModel->config().calib_weight_at_szgroup.at(a_pop));
    }
    outc(cout << endl);
    for (int a_pop = 0; a_pop < simModel->config().nbpops; a_pop++) {
        outc(cout << " " << simModel->config().calib_cpue_multiplier.at(a_pop));
    }
    outc(cout << endl);

    outc(cout << dyn_alloc_sce.toString() << endl);
    outc(cout << dyn_pop_sce.toString() << endl);
    outc(cout << "biolsce " << biolsce << endl);
    outc(cout << "fleetsce " << fleetsce << endl);
    outc(cout << "freq_do_growth " << freq_do_growth << endl);
    outc(cout << "freq_redispatch_the_pop " << freq_redispatch_the_pop << endl);
    outc(cout << "a_graph " << a_graph << endl);
    outc(cout << "a_graph_name " << a_graph_name << endl);
    outc(cout << "nrow_coord " << nrow_coord << endl);
    outc(cout << "nrow_graph " << nrow_graph << endl);
    outc(cout << "a_port " << a_port << endl);
    outc(cout << "graph res in km xy " << graph_res.at(0) << " " << graph_res.at(1) << endl);
    outc(cout << "is_individual_vessel_quotas " << simModel->scenario().is_individual_vessel_quotas << endl);
    outc(cout << "check_all_stocks_before_going_fishing " << check_all_stocks_before_going_fishing << endl);

    if (dyn_alloc_sce.option(Options::fishing_credits)) {
        outc(cout << "tariff_pop.at(0) " << tariff_pop.at(0) << endl);
        outc(cout << "freq_update_tariff_code " << freq_update_tariff_code << endl);
        outc(cout << "update_tariffs_based_on_lpue_or_dpue_code " << update_tariffs_based_on_lpue_or_dpue_code << endl);
        outc(cout << "arbitary_breaks_for_tariff.at(0) " << arbitary_breaks_for_tariff.at(0) << endl);
        outc(cout << "total_amount_credited " << total_amount_credited << endl);
        outc(cout << "tariff_annual_hcr_percent_change " << tariff_annual_hcr_percent_change << endl);
    }

    // implicit_pops is a vector of the index of pop (see pop_names.txt)
    // for which we do not have any info on the pops_N_at_szgroup because not assessed stock by ICES....
    // so the dynamic of these pops are not truly simulated but vessels can do some catches on them in any cases
    // using vessel and species-specific historic cpue data... (just like in paper Bastardie et al 2010)

    // set a fixed seed
    int a_rand = rand() % 10000;
    // to avoid chosing the seed in the same second!
    for (int waste = 0; waste < a_rand; waste++) {
    }

    // detect a integer in the name of the simu
    size_t iii = 0;
    for (; iii < namesimu.length(); iii++) { if (isdigit(namesimu[iii])) { break; }}

    // remove the first chars, which aren't digits
    string str = namesimu.substr(iii, namesimu.length() - iii);

    // convert the remaining text to an integer
    int a_seed = atoi(str.c_str());

    // print the result
    std::cout << "The random seed is set to: " << a_seed << std::endl;

    // set always the same seed for a given simu name (that should include a integer with pattern s100 or sim100 or simu100...)
    srand(a_seed);


    // special case for random stochastic spatial pop distribution
    if (dyn_pop_sce.option(Options::white_noise_on_avai_deprecated)) {
        // pick up a file name randomly later on
        int rand_avai_file = (rand() % 50) + 1;
        char buffer[100];
        sprintf(buffer, "%03d", rand_avai_file);
        str_rand_avai_file = buffer;
        outc(cout << "the avai file randomly chosen is indexed  " << str_rand_avai_file << endl);
    } else {
        // no stochastic variation
        str_rand_avai_file = "baseline";
    }

    if (!OutputExporter::instantiate(outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput,
                                     namesimu)) {
        std::cerr << "Cannot open output files." << std::endl;
        exit(1);
    }


    std::shared_ptr<ModelMetadataAccessor> metadata = nullptr;
    OutputExporter::instance().setUseSqlite(enable_sqlite_out);

    try {
        if (enable_sqlite_out) {
            outSqlitePath = namefolder + "/" + namefolderinput + "_" + namesimu + "_out.db";

            unlink(outSqlitePath.c_str());

            outSqlite = std::make_shared<SQLiteOutputStorage>(outSqlitePath);
            outSqlite->open();
            outSqlite->createAllTables();

            OutputExporter::instance().setSQLiteDb(outSqlite);
            guiSendOutputInfo(outSqlitePath);

            metadata = std::make_shared<ModelMetadataAccessor>(outSqlite->metadata());
        }
    } catch (SQLiteException &x) {
        std::cerr << "Cannot open output sqlite file: " << x.what() << "\n";
        exit(1);
    }

    if (metadata) {
        metadata->setNbPops(simModel->config().nbpops);
        metadata->setNbBenthos(simModel->config().nbbenthospops);
        metadata->setNbSizes(NBSZGROUP);
    }

    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/export_individual_tac_" +
               namesimu + ".dat";
    export_individual_tacs.open(filename.c_str());

    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " TEST MY R UTILS           " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);

    // Seed the random-number generator with current time so that
    // the numbers will be different every time we run.
    int seed = (int) time(NULL);     // random seed
    UNUSED(seed);

#if 0
    dout(cout  << " check do_sample() " << endl);
    int val[4]= {12,13,14,15};
    double p[4]= {0.01,0.200,0.300,0.490};
    // draw 100 numbers from these 4 values, see myRutils.cpp
    vector<int> res = do_sample(100, 4, val, p);
    for(unsigned int i=0; i<res.size(); i++)
    {
        dout(cout  << " " << res[i]);
    }
    dout(cout  << endl);
#endif

    dout(cout << " check rgamma() " << endl);
    double a_shape = 1;
    double a_scale = 60;
    vector<double> a_res;
    for (int i = 0; i < 100; i++) {
        a_res.push_back(rgamma(a_shape, a_scale));
        dout(cout << " " << a_res[i]);
    }
    dout(cout << endl);

    // test the splitting of trees
    // (NOTE: this method will fail if the same variables are present in several branches...use a data.frame instead?)
    string a_string = "0.0 last_trip_was 0.3 weather_is 0.5 fish_price_is 0.2 last_trip_was 0.8 remaining_quota_is 0.9";
    vector<string> a_split_string;
    string a_delimiter = " fish_price_is ";
    split(a_split_string, a_string, a_delimiter, splitX::no_empties);
    //	print( a_split_string );
    // second split....
    a_string = a_split_string[0];
    a_delimiter = " weather_is ";
    split(a_split_string, a_string, a_delimiter, splitX::no_empties);
    //	print( a_split_string );
    //system("PAUSE");


    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " TEST MY UTILS             " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);

    double MLS = 40;
    double L75 = 46.4;
    double L50 = 27.4;
    double S1 = L50 * log(3) / (L75 - L50);
    double S2 = S1 / L50;
    double result1 = simpson<myintegrand>(20, MLS, 100, S1, S2); // discards
    double result2 = simpson<myintegrand>(MLS, 70, 100, S1, S2); // landings
    dout(cout << result1 << "  " << result2 << endl);


    vector<double> sel;
    sel.push_back(0.4015599);
    sel.push_back(0.4155306);
    sel.push_back(0.4296385);
    sel.push_back(0.4438615);
    sel.push_back(0.4581772);
    sel.push_back(0.4725623);
    sel.push_back(0.4869931);
    sel.push_back(0.5014455);
    sel.push_back(0.5158956);
    sel.push_back(0.5303191);
    sel.push_back(0.5446922);
    sel.push_back(0.5589912);
    sel.push_back(0.5731930);
    sel.push_back(0.5872752);
    sel.push_back(0.6012159);
    sel.push_back(0.6149945);
    sel.push_back(0.6285911);
    sel.push_back(0.6419871);
    sel.push_back(0.6551650);
    sel.push_back(0.6681085);
    sel.push_back(0.6808029);
    sel.push_back(0.6932348);
    sel.push_back(0.7053919);
    sel.push_back(0.7172638);
    sel.push_back(0.7288412);
    sel.push_back(0.7401162);
    sel.push_back(0.7510826);
    sel.push_back(0.7617352);
    sel.push_back(0.7720702);
    sel.push_back(0.7820852);
    sel.push_back(0.7917788);
    sel.push_back(0.8011509);
    sel.push_back(0.8102022);
    sel.push_back(0.8189347);
    sel.push_back(0.8273510);
    sel.push_back(0.8354546);
    sel.push_back(0.8432500);
    sel.push_back(0.8507421);
    sel.push_back(0.8579364);
    sel.push_back(0.8648390);
    sel.push_back(0.8714565);
    sel.push_back(0.8777957);
    sel.push_back(0.8838640);
    sel.push_back(0.8896689);
    sel.push_back(0.8952180);
    sel.push_back(0.9005192);
    sel.push_back(0.9055805);
    sel.push_back(0.9104099);
    sel.push_back(0.9150155);
    sel.push_back(0.9194053);
    sel.push_back(0.9235872);
    double result3 = trapezoidal(20 - 20, MLS - 20, sel); // landings
    double result4 = trapezoidal(MLS - 20, 70 - 20, sel); // landings
    cout << result3 << "  " << result4 << endl;


    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " NODE-RELATED STUFFS      " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);

#ifdef PROFILE
                                                                                                                            memInfo.update();
    guiSendMemoryInfo(memInfo);
    mLoadProfile.start();
#endif

    // NOTE: do this test if the proper command line argument is passed...
    {
        NodeTester nodeTester(simModel->config().nbpops, simModel->config().nbbenthospops);
        nodeTester.test();
    }



    // ---------------------------------------------- //

    auto a_graph_s = std::to_string(a_graph);

    if (!modelLoader->loadNodesAndGraphs()) {
        throw std::runtime_error("Cannot load Nodes and Graphs");
    }

#ifdef PROFILE
                                                                                                                            memInfo.update();
    guiSendMemoryInfo(memInfo);

    mLoadNodesProfileResult = mLoadProfile.elapsed_ms();
#endif
    bgi::rtree<std::pair<point, int>, bgi::quadratic<16> > rtree;
    createRTreeFromNodes(simModel->nodes(), rtree);

    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " BENTHOS-RELATED STUFFS    " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);

    Loader *l = new Loader;

    //LoadedData loadedDataBenthos;

    paramsForLoad.sparam1 = std::to_string(simModel->month());
    paramsForLoad.sparam2 = std::to_string(simModel->quarter());
    paramsForLoad.sparam3 = std::to_string(simModel->semester());
    paramsForLoad.iparam1 = simModel->config().nbpops;
    paramsForLoad.iparam2 = NBAGE;
    paramsForLoad.iparam3 = NBSZGROUP;

    benthoss = modelLoader->loadBenthos(dyn_pop_sce, dyn_alloc_sce, biolsce, fleetsce);

    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " FISHFARMS-RELATED STUFFS " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);

    paramsForLoad.sparam1 = std::to_string(simModel->month());
    paramsForLoad.sparam2 = std::to_string(simModel->quarter());
    paramsForLoad.sparam3 = std::to_string(simModel->semester());
    paramsForLoad.iparam1 = simModel->config().nbpops;
    paramsForLoad.iparam2 = NBAGE;
    paramsForLoad.iparam3 = NBSZGROUP;

    LoadedData loadedDataFishfarms;

    Dataloaderfishfarms ffl;
    l->loadFeatures(&ffl,
                    indb,
                    folder_name_parameterization,
                    inputfolder,
                    dyn_pop_sce,
                    dyn_alloc_sce,
                    biolsce,
                    fleetsce,
                    paramsForLoad,
                    loadedDataFishfarms);


    cout << "create fishfarms..." << endl;

    vector<Fishfarm *> fishfarms(loadedDataFishfarms.vectiparam1.size());
    for (unsigned int i = 0; i < loadedDataFishfarms.vectiparam1.size(); i++) {

     cout << "create fishfarm num: " << i << endl;

        fishfarms[i] = new Fishfarm(
                loadedDataFishfarms.vectiparam1.at(i),
                loadedDataFishfarms.vectsparam1.at(i),
                simModel->nodes().at(loadedDataFishfarms.vectiparam2.at(i)),
                0,
                loadedDataFishfarms.vectiparam3.at(i),
                loadedDataFishfarms.vectdparam1.at(i),
                loadedDataFishfarms.vectdparam2.at(i),
                loadedDataFishfarms.vectdparam3.at(i),
                loadedDataFishfarms.vectdparam4.at(i),
                loadedDataFishfarms.vectdparam5.at(i),
                loadedDataFishfarms.vectdparam6.at(i),
                loadedDataFishfarms.vectdparam7.at(i),
                loadedDataFishfarms.vectdparam8.at(i),
                loadedDataFishfarms.vectdparam9.at(i),
                loadedDataFishfarms.vectdparam10.at(i),
                loadedDataFishfarms.vectdparam11.at(i),
                loadedDataFishfarms.vectdparam12.at(i),
                loadedDataFishfarms.vectdparam13.at(i),
                loadedDataFishfarms.vectsparam2.at(i),
                loadedDataFishfarms.vectiparam4.at(i),
                loadedDataFishfarms.vectiparam5.at(i),
                loadedDataFishfarms.vectiparam6.at(i),
                loadedDataFishfarms.vectiparam7.at(i),
                loadedDataFishfarms.vectiparam8.at(i),
                loadedDataFishfarms.vectdparam14.at(i),
                loadedDataFishfarms.vectdparam15.at(i),
                loadedDataFishfarms.vectdparam16.at(i),
                loadedDataFishfarms.vectiparam9.at(i),
                loadedDataFishfarms.vectdparam17.at(i),
                loadedDataFishfarms.vectdparam18.at(i),
                loadedDataFishfarms.vectdparam19.at(i),
                loadedDataFishfarms.vectdparam20.at(i),
                loadedDataFishfarms.vectdparam21.at(i),
                loadedDataFishfarms.vectdparam22.at(i),
                loadedDataFishfarms.vectsparam3.at(i),
                loadedDataFishfarms.vectdparam23.at(i),
                loadedDataFishfarms.vectdparam24.at(i),
                loadedDataFishfarms.vectdparam25.at(i),
                loadedDataFishfarms.vectdparam26.at(i),
                loadedDataFishfarms.vectdparam27.at(i),
                loadedDataFishfarms.vectdparam28.at(i),
                loadedDataFishfarms.vectsparam4.at(i),
                loadedDataFishfarms.vectdparam29.at(i),
                loadedDataFishfarms.vectdparam30.at(i),
                loadedDataFishfarms.vectdparam31.at(i),
                loadedDataFishfarms.vectdparam32.at(i),
                loadedDataFishfarms.vectdparam33.at(i),
                loadedDataFishfarms.vectdparam34.at(i),
                loadedDataFishfarms.vectdparam35.at(i),
                loadedDataFishfarms.vectdparam36.at(i),
                loadedDataFishfarms.vectdparam37.at(i),
                loadedDataFishfarms.vectdparam38.at(i),
                loadedDataFishfarms.vectdparam39.at(i),
                loadedDataFishfarms.vectdparam40.at(i),
                loadedDataFishfarms.vectdparam41.at(i),
                loadedDataFishfarms.vectdparam42.at(i),
                loadedDataFishfarms.vectdparam43.at(i));


        simModel->nodes().at(loadedDataFishfarms.vectiparam2.at(i))->set_ff_names_on_node(
                loadedDataFishfarms.vectiparam1.at(i));

        cout << fishfarms[i]->get_name() << endl;
        cout << "on node " << loadedDataFishfarms.vectiparam2.at(i) << " put ffarm "
             << simModel->nodes().at(loadedDataFishfarms.vectiparam2.at(i))->get_ff_names_on_node().at(0)
             << endl;
        cout << "at (" << fishfarms[i]->get_x() << "," << fishfarms[i]->get_y() << ") " << endl;
        cout << "end for harvest at " << loadedDataFishfarms.vectiparam5.at(i) << " given " << fishfarms[i]->get_end_day_harvest()
             << endl;

        if (outSqlite) {
            outSqlite->exportFishfarmDef(*fishfarms[i]);
        }
    }

    cout << "all fishfarms created...." << endl;


    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " WINDMILLS-RELATED STUFFS " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);

    paramsForLoad.sparam1 = std::to_string(simModel->month());
    paramsForLoad.sparam2 = std::to_string(simModel->quarter());
    paramsForLoad.sparam3 = std::to_string(simModel->semester());
    paramsForLoad.iparam1 = simModel->config().nbpops;
    paramsForLoad.iparam2 = NBAGE;
    paramsForLoad.iparam3 = NBSZGROUP;

    LoadedData loadedDataWindmills;

    Dataloaderwindmills wml;
    l->loadFeatures(&wml,
                    indb,
                    folder_name_parameterization,
                    inputfolder,
                    dyn_pop_sce,
                    dyn_alloc_sce,
                    biolsce,
                    fleetsce,
                    paramsForLoad,
                    loadedDataWindmills);

    //TODO: extend variables in read_size_per_windmill() e.g. read kWh from files etc.


    for (map<int, double>::iterator iter = loadedDataWindmills.mmapidparam1.begin();
         iter != loadedDataWindmills.mmapidparam1.end(); iter = loadedDataWindmills.mmapidparam1.upper_bound(iter->first)) {
        Windmill *wm = new Windmill(iter->first, "here_a_windfarm_name",
                                    simModel->nodes().at(iter->first), iter->second, 1, 500,
                                    1); // Caution: type is 1, kW is 500, is_active at 1
        windmills.push_back(wm);
    }

    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " POPULATION-RELATED STUFFS " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);

#ifdef PROFILE
    mLoadProfile.start();
#endif

    paramsForLoad.sparam1 = std::to_string(simModel->month());
    paramsForLoad.sparam2 = std::to_string(simModel->quarter());
    paramsForLoad.sparam3 = std::to_string(simModel->semester());
    paramsForLoad.iparam1 = simModel->config().nbpops;
    paramsForLoad.iparam2 = NBAGE;
    paramsForLoad.iparam3 = NBSZGROUP;
    paramsForLoad.iparam4 = SEL_NBSZGROUP;
    paramsForLoad.vdparam1 = simModel->config().calib_cpue_multiplier;
    paramsForLoad.vdparam2 = simModel->config().calib_weight_at_szgroup;

    LoadedData loadedDataPops;

    Dataloaderpops ppl;
    l->loadFeatures(&ppl,
                    indb,
                    folder_name_parameterization,
                    inputfolder,
                    dyn_pop_sce,
                    dyn_alloc_sce,
                    biolsce,
                    fleetsce,
                    paramsForLoad,
                    loadedDataPops);

    multimap<int, double> init_weight_per_szgroup = loadedDataPops.mmapidparam_init_weight_per_szgroup;
    vector<vector<double> > species_interactions_mortality_proportion_matrix = loadedDataPops.vovd_species_interactions_mortality_proportion_matrix;
    type_of_avai_field_to_read = loadedDataPops.vectsparam2;

    for (unsigned int i = 0; i < simModel->nodes().size(); i++) {
        simModel->nodes().at(i)->init_Ns_pops_at_szgroup(paramsForLoad.iparam1, paramsForLoad.iparam3);
        simModel->nodes().at(i)->init_avai_pops_at_selected_szgroup(paramsForLoad.iparam1, paramsForLoad.iparam4);
    }




    // FOR-LOOP OVER POP
    // creation of a vector of populations
    populations = vector<Population *>(simModel->config().nbpops);

    for (unsigned int sp = 0; sp < populations.size(); sp++) {
        dout(cout << endl);


        cout << " create pop... " << endl;
        populations[sp] = new Population(sp,
                                         loadedDataPops.vectsparam1.at(sp),
                                         loadedDataPops.vectdparam1.at(sp),
                                         loadedDataPops.vectdparam2.at(sp),
                                         loadedDataPops.vectdparam3.at(sp),
                                         loadedDataPops.vectdparam4.at(sp),
                                         loadedDataPops.vectdparam5.at(sp),
                                         loadedDataPops.vovi1.at(sp),
                                         loadedDataPops.vovd1.at(sp),
                                         loadedDataPops.vovd2.at(sp),
                                         loadedDataPops.vovd3.at(sp),
                                         loadedDataPops.vovd4.at(sp),
                                         loadedDataPops.vovi2.at(sp),
                                         loadedDataPops.vovd5.at(sp),
                                         loadedDataPops.vovd6.at(sp),
                                         loadedDataPops.vovd7.at(sp),
                                         loadedDataPops.vovd8.at(sp),
                                         loadedDataPops.vectmmapndparam1.at(sp),
                                         loadedDataPops.vectmmapndparam2.at(sp),
                                         loadedDataPops.vectmapndparam1.at(sp),
                                         loadedDataPops.vectmmapidparam1.at(sp),
                                         loadedDataPops.vectmapsdparam1.at(sp),
                                         loadedDataPops.vovovd2.at(sp),
                                         loadedDataPops.vovovd3.at(sp),
                                         loadedDataPops.vovovd1.at(sp),
                                         simModel->nodes(),
                                         loadedDataPops.vovd9.at(sp),
                                         loadedDataPops.vovd10.at(sp),
                                         loadedDataPops.mapiiparam1.at(sp),
                                         loadedDataPops.mapidparam1.at(sp),
                                         loadedDataPops.vectdparam6.at(sp),
                                         loadedDataPops.vectdparam7.at(sp)
        );




        cout << " Population creator()...done " << endl;

        global_quotas_uptake.push_back(0.0);


        if (!binary_search(simModel->config().implicit_pops.begin(), simModel->config().implicit_pops.end(), sp)) {
            outc(cout << "inform avai on nodes " << endl);

            outc(cout << "...then attach avai to each node for this pop (this quarter)" << endl);
            // init avai on each node (we know the presence...) for this pop for selected szgroup
            vector <types::NodeId> nodes_with_presence = loadedDataPops.vovn1.at(sp);
            multimap<types::NodeId, double> avai_szgroup_nodes_with_pop = loadedDataPops.vectmmapndparam1.at(sp);

            for (unsigned int n = 0; n < nodes_with_presence.size(); n++) {
                dout(cout << ".");
                auto spat_avai_this_pop_this_node = find_entries(avai_szgroup_nodes_with_pop,
                                                                 nodes_with_presence.at(n));

                vector<double> spat_avai_per_selected_szgroup;
                vector<int> selected_szgroups = populations.at(sp)->get_selected_szgroups();
                for (int sz = 0; sz < spat_avai_this_pop_this_node.size(); ++sz) {
                    auto it = find(selected_szgroups.begin(), selected_szgroups.end(), sz);
                    if (it != selected_szgroups.end()) {
                        spat_avai_per_selected_szgroup.push_back(spat_avai_this_pop_this_node.at(sz));
                    }
                }
                if (!spat_avai_per_selected_szgroup.empty()) {
                    simModel->nodes().at(nodes_with_presence.at(n).toIndex())->set_avai_pops_at_selected_szgroup(sp,
                                                                                                                 spat_avai_per_selected_szgroup);
                } else {
                    // inconsistence between lst_idx_nodes and avai files if this happen...
                    outc(cout << nodes_with_presence.at(n));
                }

            }

            // check
            /*
           outc(cout << "avai at selected szgroup for the pop " << sp << " on a given node xx:" << endl); // used in do_catch != the one used in distributeN()
            vector<double> avai_pops_at_selected_szgroup = nodes[792]->get_avai_pops_at_selected_szgroup(sp);
            vector<double>::iterator szgroup = avai_pops_at_selected_szgroup.begin();
            for( ; szgroup != avai_pops_at_selected_szgroup.end(); szgroup++)
            {
               outc(cout << *szgroup << " " );
            }
           outc(cout << endl);

            // check
           outc(cout << "tot N at szgroup for the pop " << sp << "on a given node xx:" << endl);
            vector<double> tot_N_at_szgroup = populations[sp]->get_tot_N_at_szgroup();
            vector<double>::iterator szgroup2 = tot_N_at_szgroup.begin();
            for( ; szgroup2 != tot_N_at_szgroup.end(); szgroup2++)
            {
               outc(cout << *szgroup << " " );
            }
           outc(cout << endl);
            */

            outc(cout
                         << "if you have a problem of out of range here then check if you forgot a blank at the end of N_at_szgroup.dat! "
                         << endl);
        }                         // end implicit pop
    }                             // end pop





#ifdef PROFILE
    mLoadPopulationProfileResult = mLoadProfile.elapsed_ms();
#endif



    /*
    // check on the node side
   outc(cout << "check on the node side e.g. for node 2579: " << endl);
    vector<double> a_Ns_at_szgroup = nodes[2579]->get_Ns_pops_at_szgroup(0);
    for(unsigned int i=0 ; i<a_Ns_at_szgroup.size();  i++)
    {
       outc(cout << a_Ns_at_szgroup[i] << " " );
    }
   outc(cout << endl);

    // check the function for aggregation from nodes
    populations[0]->aggregate_N();
    vector<double> a_Ns_at_szgroup_pop0 = populations[0]->get_tot_N_at_szgroup();
   outc(cout << "check aggregate_N() " << endl);
    for(unsigned int i=0 ; i<a_Ns_at_szgroup_pop0.size();  i++)
    {
       outc(cout << a_Ns_at_szgroup_pop0[i] << " " );
    }
   outc(cout << endl);

    // restore back and check on node side
    populations[0]->distribute_N();
    vector<double> a_Ns_at_szgroup_pop0_again = nodes[2579]->get_Ns_pops_at_szgroup(0);
   outc(cout << "check on the node side for the node 2579 " << endl);
    for(unsigned int i=0 ; i<a_Ns_at_szgroup_pop0_again.size();  i++)
    {
       outc(cout << a_Ns_at_szgroup_pop0_again[i] << " " );
    }
   outc(cout << endl);

    //  check on node side
    vector<double> a_Ns_at_szgroup_pop0_2580 = nodes[2580]->get_Ns_pops_at_szgroup(0);
   outc(cout << "check on the node side for the node 2580 " << endl);
    for(unsigned int i=0 ; i<a_Ns_at_szgroup_pop0_2580.size();  i++)
    {
       outc(cout << a_Ns_at_szgroup_pop0_2580[i] << " " );
    }
   outc(cout << endl);

    // check the update of a node (will be useful for the pop model and removals of catches)
    vector<int> names_on_node= nodes[2579]->get_pop_names_on_node();
   outc(cout << "pop names on this node " << nodes[2579]->get_idx_node().toIndex() << endl);
    for (unsigned int i=0; i<names_on_node.size(); i++)
    {
       outc(cout << names_on_node[i] << " ");
    }
   outc(cout << endl);

    // check the tacs
    for (unsigned int i=0; i<populations.size(); i++)
    {
        cout << " the tac for this pop is " <<
             populations.at(i)->get_tac()->get_current_tac() << " " << endl;

        cout << " the tac percent_for_simulated_vessels for this pop is " <<
             populations.at(i)->get_tac()->get_percent_for_simulated_vessels() << endl;;
    }

    // check the removals (should be at 0!)
    vector <double> N_at_szgroup_at_month_start= nodes.at(186)->get_Ns_pops_at_szgroup_at_month_start(1);
    vector <double> removals_per_szgroup= nodes.at(186)->get_removals_pops_at_szgroup(1);

   outc(cout << "N_at_szgroup_at_month_start" << endl);
    for(int i=0; i<N_at_szgroup_at_month_start.size(); i++)
    {
       outc(cout << N_at_szgroup_at_month_start.at(i) << endl);
    }
   outc(cout << "removals_per_szgroup" << endl);
    for(int i=0; i<removals_per_szgroup.size(); i++)
    {
       outc(cout << removals_per_szgroup.at(i) << endl);
    }
    */


    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " FISHING CREDITS-RELATED   " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);


    // TO DO: compute the composite tariff also from the benthos and risk of bycatch on localities?
    // types 0 and 1, say 0: benthos, 1: bycatch risk
    //double init_tariff0_on_localities=100.0;
    //double init_tariff1_on_localities=100.0;
    //vector<double> tariffs;
    //tariffs.push_back(init_tariff0_on_localities);
    //tariffs.push_back(init_tariff1_on_localities);

    if (dyn_alloc_sce.option(Options::fishing_credits)) {
        auto initial_tariffs_on_nodes = read_initial_tariffs_on_nodes(folder_name_parameterization, inputfolder,
                                                                      a_graph_name);


        // init
        for (unsigned int a_idx = 0; a_idx < simModel->nodes().size(); a_idx++) {

            auto idx_node = simModel->nodes().at(a_idx)->get_idx_node();

            // initial tariff for this particular node
            auto lower_init_cr = initial_tariffs_on_nodes.lower_bound(idx_node);
            auto upper_init_cr = initial_tariffs_on_nodes.upper_bound(idx_node);
            vector<double> init_tariffs;
            for (auto pos = lower_init_cr; pos != upper_init_cr; pos++)
                init_tariffs.push_back(pos->second);


            // complete to 3 values for tariff per node because we expect tariff all, tariff pop, and tariff benthos
            while (init_tariffs.size() <= 3) { init_tariffs.push_back(0); }


            if (initial_tariffs_on_nodes.count(idx_node) == 0) {
                init_tariffs.push_back(0);
            } // put 0 if this node is not informed

            simModel->nodes().at(a_idx)->set_tariffs(init_tariffs); // type 0
        }


        // check
        for (unsigned int a_idx = 0; a_idx < simModel->nodes().size(); a_idx++) {
            dout(cout << "this node " << simModel->nodes().at(a_idx)->get_idx_node() <<
                      " has tariffs 0 " << simModel->nodes().at(a_idx)->get_tariffs().at(0) << endl);

            dout(cout << "this node " << simModel->nodes().at(a_idx)->get_idx_node() <<
                      " has tariffs 1 " << simModel->nodes().at(a_idx)->get_tariffs().at(1) << endl);
        }
    } else {
        // need to inform with a vector of three zeros at least
        vector<double> init_tariffs(3, 0);
        for (unsigned int a_idx = 0; a_idx < simModel->nodes().size(); a_idx++) {
            simModel->nodes().at(a_idx)->set_tariffs(init_tariffs); // type 0
        }
    }


    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " SIZE-SPECTRA STUFFS       " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);

    cout << "initiate size-spectra-related objects..." << endl;
    vector<vector<double> > Ws_at_szgroup(simModel->config().nbpops, vector<double>(NBSZGROUP));
    vector<vector<vector<vector<double> > > > predKernel(simModel->config().nbpops,
                                                         vector<vector<vector<double>>>(NBSZGROUP,
                                                                                        vector<vector<double> >(
                                                                                                NBSZGROUP,
                                                                                                vector<double>(
                                                                                                        simModel->config().nbpops,
                                                                                                        0.0)
                                                                                        )
                                                         )
    );
    vector<vector<double> > searchVolMat(simModel->config().nbpops, vector<double>(NBSZGROUP));
    vector<vector<double> > juveniles_diet_preference(simModel->config().nbpops,
                                                      vector<double>(simModel->config().nbpops));
    vector<vector<double> > adults_diet_preference(simModel->config().nbpops,
                                                   vector<double>(simModel->config().nbpops));
    int mat_cat = 0; //init - split juveniles vs. adult categories
    vector<int> mat_cats(simModel->config().nbpops, 0);

    if (dyn_pop_sce.option(Options::sizeSpectra)) {
        cout << "sizeSpectra option is on..." << endl;

        // compute a predKernel and a searchVol
        // predKernel.at(j).at(kprey).at(k).at(name_pop)

        // read-in multimap on diet of stocks per stock
        multimap<int, double> adults_diet_preference_per_stock_allstks = read_adults_diet_preference_per_stock_allstks(
                folder_name_parameterization, inputfolder, biolsce);
        multimap<int, double> juveniles_diet_preference_per_stock_allstks = read_juveniles_diet_preference_per_stock_allstks(
                folder_name_parameterization, inputfolder, biolsce);


        cout << "compute Ws_at_szgroup..." << endl;
        for (unsigned int j = 0; j < simModel->config().nbpops; ++j) {  // loop over predators
            vector<double> W_this_pop = populations.at(j)->get_weight_at_szgroup();
            for (unsigned int k = 0; k < NBSZGROUP; ++k) {  // loop over predator sizes
                Ws_at_szgroup.at(j).at(k) = W_this_pop.at(k);
                //cout <<  "Ws_at_szgroup.at("<<j<<").at("<<k<<") is " << Ws_at_szgroup.at(j).at(k) << endl;
            }
        }

        cout << "initialize PredKernel..." << endl;
        for (unsigned int prey = 0; prey < simModel->config().nbpops; ++prey) {  // loop over prey
            for (unsigned int j = 0; j < simModel->config().nbpops; ++j) {  // loop over predators
                for (unsigned int k = 0; k < NBSZGROUP; ++k) {  // loop over predator sizes
                    for (unsigned int kprey = 0; kprey < NBSZGROUP; ++kprey) {  // loop over prey sizes
                        predKernel.at(j).at(kprey).at(k).at(prey) = Ws_at_szgroup.at(j).at(k); // init
                        //cout <<  "predKernel.at("<<j<<").at("<<kprey<<").at("<<k<<").at("<<prey<<") is " << predKernel.at(j).at(kprey).at(k).at(prey) << endl;
                    }
                }
            }
        }

        // check:
        //  cout << "check some initial values of PredKernel..." << endl;
        //  cout <<  "predKernel.at(0).at(0).at(0).at(0) is " << predKernel.at(0).at(0).at(0).at(0) << endl;
        //  cout <<  "predKernel.at(0).at(1).at(0).at(0) is " << predKernel.at(0).at(1).at(0).at(0) << endl;
        //  cout <<  "predKernel.at(0).at(2).at(0).at(0) is " << predKernel.at(0).at(2).at(0).at(0) << endl;
        //  cout <<  "predKernel.at(1).at(0).at(0).at(0) is " << predKernel.at(1).at(0).at(0).at(0) << endl;

        if (predKernel.at(0).at(1).at(0).at(0) !=
            predKernel.at(0).at(1).at(0).at(0))  // c++ trick for like testing for is.nan
        {
            int a;
            cout << "1: nan detected" << endl;
            cout << "1: nan detected in initial predKernel...Pause: type a number to continue";
            cin >> a;
        }

        cout << "compute PredKernel..." << endl;
        vector<double> sigma(simModel->config().nbpops,
                             1.3); // prey size selection parameter # see Mizer params@species_params // Width of size preference
        //vector<double> beta (simModel->config().nbpops, 100);   // prey size selection parameter # see Mizer params@species_params  // Predation/prey mass ratio
        // replace with logistic per 14 weight class
        // beta_end + (beta_begin - beta_end) *(1+ exp(1*(w0 -wend)))/(1+ exp(1*(w -wend)))  with beta_begin=100 and beta_end=300 so that larger fish eats on much smaller fish
        vector<double> beta{100.0001, 100.0215, 100.1079, 100.3115, 100.6974, 101.3550, 102.4202, 104.1142, 106.8150,
                            111.1882, 118.4140, 130.5108, 150.4701, 180.9963};

        for (unsigned int prey = 0; prey < simModel->config().nbpops; ++prey) {  // loop over prey
            for (unsigned int j = 0; j < simModel->config().nbpops; ++j) {  // loop over predators
                for (unsigned int k = 0; k < NBSZGROUP; ++k) {  // loop over predator sizes
                    for (unsigned int kprey = 0; kprey < NBSZGROUP; ++kprey) {  // loop over prey sizes
                        if (Ws_at_szgroup.at(prey).at(kprey) < predKernel.at(j).at(kprey).at(k).at(prey)) {
                            predKernel.at(j).at(kprey).at(k).at(prey) =
                                    exp(-pow(log((beta.at(kprey) * Ws_at_szgroup.at(prey).at(kprey)) /
                                                 Ws_at_szgroup.at(j).at(kprey)), 2) / (pow(2 * sigma.at(prey), 2)));;
                            //cout <<  "predKernel.at("<<j<<").at("<<kprey<<").at("<<k<<").at("<<prey<<") is " << predKernel.at(j).at(kprey).at(k).at(prey) << endl;
                        } else {
                            predKernel.at(j).at(kprey).at(k).at(prey) = 0.0;
                            //cout <<  "put 0 in predKernel.at("<<j<<").at("<<kprey<<").at("<<k<<").at("<<prey<<") is " << predKernel.at(j).at(kprey).at(k).at(prey) << endl;
                        }
                    }
                }
            }
        }

        // check:
        // cout << "check some values of PredKernel..." << endl;
        // cout <<  "predKernel.at(0).at(0).at(0).at(0) is " << predKernel.at(0).at(0).at(0).at(0) << endl;
        // cout <<  "predKernel.at(0).at(1).at(0).at(0) is " << predKernel.at(0).at(1).at(0).at(0) << endl;
        // cout <<  "predKernel.at(0).at(2).at(0).at(0) is " << predKernel.at(0).at(2).at(0).at(0) << endl;
        // cout <<  "predKernel.at(1).at(0).at(0).at(0) is " << predKernel.at(1).at(0).at(0).at(0) << endl;

        if (predKernel.at(0).at(1).at(0).at(0) !=
            predKernel.at(0).at(1).at(0).at(0))  // c++ trick for like testing for is.nan
        {
            int a;
            cout << "1: nan detected" << endl;
            cout << "1: nan detected in predKernel...Pause: type a number to continue";
            cin >> a;
        }


        const string separator = ";";

        string filename = inputfolder + "/popsspe_" + folder_name_parameterization +
                          "/Stock_biological_traits.csv"; // file location is an issue....

        ifstream is;
        is.open(filename.c_str());
        if (is.fail()) {
            cout << "Fail to open the file " << filename << endl;
            open_file_error(filename);
            return false;
        }

        cout << "import a few parameters ..." << endl;
        std::vector<std::tuple<string, double, double, double, double,
                double, double, double, double,
                double, double, double, double,
                double, double, double, double,
                double, double, double, double,
                double, double, double, double,
                double, double, double, double,
                double, double, double, double,
                double, string> > biological_traits_params;
        bool r = read_biological_traits_params(is, separator, biological_traits_params);

        //colnames:
        //0	1	2	3	4	5	6	7	8	9	10	11	12	13	14	15	16	17	18	19	20	21	22	23	24	25	26	27	28	29	30	31	32	33	34
        //stock	Winf	k	Linf	K	t0	a	b	L50	alpha	beta	r_age	tac_tons	fbar_age_min	fbar_age_max	F_target	F_percent	TAC_percent	B_trigger	FMSY	fbar_assessment	ssb_assessment	mls_cat	mls	size_bin_cm	unit_sizebin	CV_recru	mat	mat_cat	etha_m	kappa	q	n	fzeroest	species

        // parameters to compute the search volume (volumetric search rate)
        cout << "read few parameters ..." << endl;
        //auto param = std::make_tuple (2e8, 0.8, 0.75,  0.6); //TODO: AVOID HARDCODING FOR THESE BUNCH OF PARAMS
        //double kappa  = std::get<0>(param);
        //double q      = std::get<1>(param);     // Scaling of search volume
        //double n      = std::get<2>(param);
        //double f0est  = std::get<3>(param);     // equilibrium feeding level, for which h-bar was estimated


        cout << "compute the searchVolMat..." << endl;
        for (unsigned int prey = 0; prey < simModel->config().nbpops; ++prey) {  // loop over prey


            double eta_m = get<29>(biological_traits_params.at(prey));
            double kappa = get<30>(biological_traits_params.at(prey));
            double q = get<31>(biological_traits_params.at(prey));     // Scaling of search volume
            double n = get<32>(biological_traits_params.at(prey));
            double f0est = get<33>(
                    biological_traits_params.at(prey));     // equilibrium feeding level, for which h-bar was estimated
            mat_cat = get<28>(biological_traits_params.at(prey));
            mat_cats.at(prey) = mat_cat;

            double lambda = 2 + q - n;
            cout << " reading kappa is " << kappa << endl;
            cout << " reading q is " << q << endl;
            cout << " reading n is " << n << endl;
            cout << " reading f0est is " << f0est << endl;
            cout << " reading lambda is " << lambda << endl;


            for (unsigned int j = 0; j < simModel->config().nbpops; ++j) {  // loop over predators
                for (unsigned int k = 0; k < NBSZGROUP; ++k) {
                    double alphae = sqrt(2 * PI) * sigma.at(prey) * pow(beta.at(k), (lambda - 2)) *
                                    exp(pow(lambda - 2, 2) * pow(sigma.at(prey), 2) / 2);

                    //cout << " this prey " << prey << " alphae is " << alphae << endl;
                    //cout << " given sigma.at(prey) is " << sigma.at(prey) << " beta.at(k) is " << beta.at(k) << " lambda is " << lambda << endl;

                    // loop over predator sizes
                    double Wk = get<2>(biological_traits_params.at(j));
                    double Winf = get<1>(biological_traits_params.at(j));
                    double h = (3 / 0.36) * Wk * pow(Winf, (0.25)) * pow(eta_m, (-0.333333333)); // Calculate h from K
                    if (h <= 15) { h = 15; }
                    double gamma = (f0est * h / (alphae * kappa * 0.5 * (1 - f0est)));
                    //cout << "j: " << j << " k: " << k << endl;
                    //cout << "Wk: " << Wk << " Winf: " << Winf << endl;
                    //cout << "h: " << h << " gamma: " << gamma << endl;
                    //cout << "q: " << q  << " f0est:" << f0est << endl;
                    searchVolMat.at(j).at(k) = gamma * pow(Ws_at_szgroup.at(j).at(k), q);  // V_i(w) = gamma_i*w^q
                    //cout << "searchVolMat.at(j).at(k): " << searchVolMat.at(j).at(k)  << endl;
                }
            }


            // because it is a simplified version we do not compute phiprey, encounteredfood and feedinglevel
            // we will instead assume feeding level at 0.6
            // this is why this loop over prey seems useless for now




        }


        cout << "Read in the diet preference..." << endl;
        for (unsigned int j = 0; j < simModel->config().nbpops; ++j) {  // loop over predators
            multimap<int, double>::iterator lower_ia = adults_diet_preference_per_stock_allstks.lower_bound(j);
            multimap<int, double>::iterator upper_ia = adults_diet_preference_per_stock_allstks.upper_bound(j);
            multimap<int, double>::iterator lower_ij = juveniles_diet_preference_per_stock_allstks.lower_bound(j);
            multimap<int, double>::iterator upper_ij = juveniles_diet_preference_per_stock_allstks.upper_bound(j);
            vector<double> ad_diet_pref;
            for (multimap<int, double>::iterator pos = lower_ia; pos != upper_ia; pos++)
                ad_diet_pref.push_back(pos->second);
            vector<double> juv_diet_pref;
            for (multimap<int, double>::iterator pos = lower_ij; pos != upper_ij; pos++)
                juv_diet_pref.push_back(pos->second);
            if (ad_diet_pref.size() != simModel->config().nbpops) {
                cout << "error dim in input file for adults diet preference" << endl;
            }
            if (juv_diet_pref.size() != simModel->config().nbpops) {
                cout << "error dim in input file for juveniles diet preference" << endl;
            }

            for (unsigned int prey = 0; prey < simModel->config().nbpops; ++prey) {  // loop over prey
                // assign diet info to this stock
                adults_diet_preference.at(j).at(prey) = ad_diet_pref.at(prey);
                juveniles_diet_preference.at(j).at(prey) = juv_diet_pref.at(prey);

                // useless because getters not used
                //populations.at(prey)->set_adults_diet_preference_per_stock(adults_diet_preference.at(j));
                //populations.at(prey)->set_juveniles_diet_preference_per_stock(juveniles_diet_preference.at(j));
            }
        }


        cout << "Initial objects for sizeSpectra option...ok" << endl;

    }

    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " METIER-RELATED STUFFS     " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);


    paramsForLoad.sparam1 = std::to_string(simModel->month());
    paramsForLoad.sparam2 = std::to_string(simModel->quarter());
    paramsForLoad.sparam3 = std::to_string(simModel->semester());
    paramsForLoad.iparam1 = simModel->config().nbpops;
    paramsForLoad.iparam2 = NBAGE;
    paramsForLoad.iparam3 = NBSZGROUP;

    LoadedData loadedDataMetiers;

    Dataloadermetiers mtl;
    l->loadFeatures(&mtl,
                    indb,
                    folder_name_parameterization,
                    inputfolder,
                    dyn_pop_sce,
                    dyn_alloc_sce,
                    biolsce,
                    fleetsce,
                    paramsForLoad,
                    loadedDataMetiers);


    // creation of a vector of metier from input data...
   vector<int> name_metiers=loadedDataMetiers.vectiparam1;
    metiers = vector<Metier *>(name_metiers.size());

    for (unsigned int i = 0; i < name_metiers.size(); i++) {

        cout << "Create metier " << i << endl;


        metiers[i] = new Metier(loadedDataMetiers.vectiparam1.at(i),
                                 loadedDataMetiers.vectiparam2.at(i),
                                 loadedDataMetiers.vectdparam1.at(i),
                                 loadedDataMetiers.vovovd1.at(i),
                                 loadedDataMetiers.vovd1.at(i),
                                 loadedDataMetiers.vovd2.at(i),
                                 loadedDataMetiers.vovi1.at(i),
                                 loadedDataMetiers.vovi2.at(i),
                                 loadedDataMetiers.vectdparam2.at(i),
                                 loadedDataMetiers.vectdparam3.at(i),
                                 loadedDataMetiers.vectdparam4.at(i),
                                 loadedDataMetiers.vectsparam1.at(i),
                                 loadedDataMetiers.vectmmapidparam1.at(i),
                                 loadedDataMetiers.vovi3.at(i),
                                 loadedDataMetiers.vovi4.at(i));

        cout << "Create metier " << i << "...done" << endl;

        selectivity_per_stock_ogives_for_oth_land=loadedDataMetiers.vovd5;



  }


    // check selectivity per metier per stock
    vector<vector<double> > selectivity_per_stock = metiers[0]->get_selectivity_per_stock_ogives();
    for (unsigned int i = 0; i < selectivity_per_stock.size(); i++) {
        for (unsigned int j = 0; j < selectivity_per_stock[i].size(); j++) {

            cout << "pop is " << i << endl;
            cout << "szgroup is " << j << endl;
            cout << "selectivity_per_stock[i,j] is " << selectivity_per_stock[i][j] << endl;
        }
    }


    // check metier betas
    vector<double> met_betas = metiers[0]->get_betas_per_pop();
    cout << "met_betas of the metier 0" << endl;
    for (int i = 0; i < met_betas.size(); i++) {
        cout << " " << met_betas[i] << " ";
    }
    cout << endl;

    // check gear_width_a
    double gear_width_a = metiers[0]->get_gear_width_a();
    cout << "gear_width_a of the metier 0" << endl;
    cout << " " << gear_width_a << " ";
    cout << endl;


    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " SHIP-RELATED STUFFS       " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);


    paramsForLoad.sparam1 = std::to_string(simModel->month());
    paramsForLoad.sparam2 = std::to_string(simModel->quarter());
    paramsForLoad.sparam3 = std::to_string(simModel->semester());
    paramsForLoad.iparam1 = simModel->config().nbpops;
    paramsForLoad.iparam2 = NBAGE;
    paramsForLoad.iparam3 = NBSZGROUP;

    LoadedData loadedDataShips;

    Dataloaderships csl;
    l->loadFeatures(&csl,
                    indb,
                    folder_name_parameterization,
                    inputfolder,
                    dyn_pop_sce,
                    dyn_alloc_sce,
                    biolsce,
                    fleetsce,
                    paramsForLoad,
                    loadedDataShips);



    ships = vector<Ship *>(loadedDataShips.vectsparam1.size());
    for (unsigned int i = 0; i < loadedDataShips.vectsparam1.size(); i++) {
        cout << "create ship " << loadedDataShips.vectsparam1.at(i) << endl;

        vector <double> longs = find_entries_i_d(loadedDataShips.mmapidparam1, loadedDataShips.vectdparam16.at(i));
        vector <double> lats = find_entries_i_d(loadedDataShips.mmapidparam2, loadedDataShips.vectdparam16.at(i));
        ships[i] = new Ship(i,
                            loadedDataShips.vectsparam1.at(i),
                            1,
                            loadedDataShips.vectdparam1.at(i),
                            loadedDataShips.vectdparam2.at(i),
                            loadedDataShips.vectsparam2.at(i),
                            loadedDataShips.vectsparam3.at(i),
                            loadedDataShips.vectdparam3.at(i),
                            loadedDataShips.vectdparam5.at(i),
                            loadedDataShips.vectdparam4.at(i),
                            loadedDataShips.vectdparam6.at(i),
                            loadedDataShips.vectdparam7.at(i),
                            loadedDataShips.vectdparam8.at(i),
                            loadedDataShips.vectdparam9.at(i),
                            loadedDataShips.vectdparam10.at(i),
                            loadedDataShips.vectdparam11.at(i),
                            loadedDataShips.vectdparam12.at(i),
                            loadedDataShips.vectdparam13.at(i),
                            loadedDataShips.vectdparam14.at(i),
                            loadedDataShips.vectdparam15.at(i),
                            longs,
                            lats);
        ships[i]->set_idx_ship(i);

        cout << "at (" << ships[i]->get_x() << "," << ships[i]->get_y() << ") " << endl;

    }
    cout << "Number of ships created: " << ships.size() << endl;


    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " VESSEL-RELATED STUFFS     " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);

#ifdef PROFILE
    mLoadProfile.start();
#endif


    paramsForLoad.sparam1 = std::to_string(simModel->month());
    paramsForLoad.sparam2 = std::to_string(simModel->quarter());
    paramsForLoad.sparam3 = std::to_string(simModel->semester());
    paramsForLoad.iparam1 = simModel->config().nbpops;
    paramsForLoad.iparam2 = NBAGE;
    paramsForLoad.iparam3 = NBSZGROUP;

    LoadedData loadedDataVessels;

    Dataloadervessels vl;
    l->loadFeatures(&vl,
                    indb,
                    folder_name_parameterization,
                    inputfolder,
                    dyn_pop_sce,
                    dyn_alloc_sce,
                    biolsce,
                    fleetsce,
                    paramsForLoad,
                    loadedDataVessels);



    // LOOP OVER VESSELIDS:
    int nbvessels= loadedDataVessels.vectsparam1.size();
    vessels = vector<Vessel *>(nbvessels);
    multimap<types::NodeId, int> possible_metiers;
    multimap<types::NodeId, double> freq_possible_metiers;
    multimap<types::NodeId, double> gshape_cpue_per_stk_on_nodes;
    multimap<types::NodeId, double> gscale_cpue_per_stk_on_nodes;
    for (unsigned int i = 0; i < nbvessels; i++)
    {
        string vname= loadedDataVessels.vectsparam1.at(i);
        // read the even more complex objects (i.e. when several info for a same vessel and a same ground)...
        // for creating the vessel object, search into the multimaps
        possible_metiers             = loadedDataVessels.vectmmapniparam1.at(i);
        freq_possible_metiers        = loadedDataVessels.vectmmapndparam1.at(i);
        gshape_cpue_per_stk_on_nodes = loadedDataVessels.vectmmapndparam2.at(i);
        gscale_cpue_per_stk_on_nodes = loadedDataVessels.vectmmapndparam3.at(i);
        spe_fgrounds                 = find_entries(loadedDataVessels.mmapsnparam2, vname);
        spe_fgrounds_init            = find_entries(loadedDataVessels.mmapsnparam3, vname);
        spe_freq_fgrounds            = find_entries_s_d(loadedDataVessels.mmapsdparam2, vname);
        spe_freq_fgrounds_init       = find_entries_s_d(loadedDataVessels.mmapsdparam3, vname);
        spe_harbours                 = find_entries(loadedDataVessels.mmapsnparam1, vname);
        spe_freq_harbours            = find_entries_s_d(loadedDataVessels.mmapsdparam1, vname);
        spe_vessel_betas_per_pop     = find_entries_s_d(loadedDataVessels.mmapsdparam4, vname);
        spe_percent_tac_per_pop      = find_entries_s_d(loadedDataVessels.mmapsdparam5, vname);

        if (spe_vessel_betas_per_pop.size() != simModel->config().nbpops) {
            std::stringstream er;
            er << "Error while reading: vessel_betas_per_pop: check the dimension i.e. simModel->config().nbpops is" <<
               simModel->config().nbpops << " while spe_vessel_betas_per_pop.size() is " <<
               spe_vessel_betas_per_pop.size() << " for vessel " << vname;
            throw std::runtime_error(er.str());

            //possibly, fix dim in R for the oldest dataset:
            //ves <- do.call ("rbind.data.frame", lapply(split(ves, f=ves$VE_REF), function(x) x[1:simModel->config().nbpops,]))

        }

         // dyn sce.
        if (dyn_alloc_sce.option(Options::fishing_credits))
        {
            tout(cout << "Read in fishing credits for this vessel " << loadedDataVessels.vectsparam1.at(i) << endl);
            spe_fishing_credits = find_entries_s_d(loadedDataVessels.mmapsdparam6, loadedDataVessels.vectsparam1.at(i));
            for (int icr = 0; icr < spe_fishing_credits.size(); ++icr) {
                spe_fishing_credits.at(icr) = spe_fishing_credits.at(icr) * total_amount_credited;
            }

            // complete to 3 values for tariff per node because we expect tariff all, tariff pop, and tariff benthos
            while (spe_fishing_credits.size() <= 3) { spe_fishing_credits.push_back(0); }
            cout << "Fishing credits 0 for this vessel " << loadedDataVessels.vectsparam1.at(i) << " is " << spe_fishing_credits.at(0) << endl;

        }

        if (dyn_alloc_sce.option(Options::reduced_speed_20percent))
        {
            // a decrease of vessel speed by 20%...
            loadedDataVessels.vectdparam1.at(i) = loadedDataVessels.vectdparam1.at(i)*0.8;
            // corresponds to a decrease by 48.8% in fuelcons
            loadedDataVessels.vectdparam2.at(i) = loadedDataVessels.vectdparam2.at(i)* 0.512;
            // cubic law  c=v^3, see Ronen 1982
            // e.g. assuming a v at 10, the fuel conso is lowered by (in %) =>  (1- (((seq(0.1,1,by=0.1)*10)^3 ) / (1*10^3)) )*100
        }

        if (dyn_alloc_sce.option(Options::reduced_speed_30percent))
        {
            // a decrease by 30%...
            loadedDataVessels.vectdparam1.at(i) = loadedDataVessels.vectdparam1.at(i)*0.7;
            // corresponds to a decrease by 65.7% in fuelcons
            loadedDataVessels.vectdparam2.at(i) = loadedDataVessels.vectdparam2.at(i)* 0.343;
            // cubic law  c=v^3, see Ronen 1982
            // e.g. assuming a v at 10, the fuel conso is lowered by (in %) =>  (1- (((seq(0.1,1,by=0.1)*10)^3 ) / (1*10^3)) )*100
        }

        if (dyn_alloc_sce.option(Options::reduced_speed_10percent))
        {
            // a decrease by 10%...
             loadedDataVessels.vectdparam1.at(i) = loadedDataVessels.vectdparam1.at(i)*0.9;
            // corresponds to a decrease by 30% in fuelcons
            loadedDataVessels.vectdparam2.at(i) = loadedDataVessels.vectdparam2.at(i)* 0.7;
            // cubic law  c=v^3, see Ronen 1982
            // e.g. assuming a v at 10, the fuel conso is lowered by (in %) =>  (1- (((seq(0.1,1,by=0.1)*10)^3 ) / (1*10^3)) )*100
        }



        // choose a departure (node) harbour for this vessel according to the observed frequency in data
        types::NodeId start_harbour;
        if (!spe_harbours.empty()) {
            // need to convert in array, see myRutils.cpp
            auto one_harbour = do_sample(1, spe_harbours.size(), spe_harbours, spe_freq_harbours);
            start_harbour = one_harbour[0];
        } else {
            // if missing info for a given vessel for this quarter
            outc(cout << "no specified harbour in this quarter for this vessel..." << endl);
            // CAUTION: LIKE A MAGIC NUMBER HERE!!!
            start_harbour = find_entries(loadedDataVessels.mmapsnparam1, loadedDataVessels.vectsparam1.at(0))[0];
            spe_harbours.push_back(start_harbour);
            spe_freq_harbours.push_back(1);
            outc(cout << "then take node: " << start_harbour << endl);
        }

         vessels[i] = new Vessel(simModel->nodes().at(start_harbour.toIndex()),
                                i,
                                loadedDataVessels.vectsparam1.at(i),
                                simModel->config().nbpops,
                                NBSZGROUP,
                                spe_harbours,
                                spe_fgrounds,
                                spe_fgrounds_init,
                                spe_freq_harbours,
                                spe_freq_fgrounds,
                                spe_freq_fgrounds_init,
                                spe_vessel_betas_per_pop,
                                spe_percent_tac_per_pop,
                                possible_metiers,
                                freq_possible_metiers,
                                gshape_cpue_per_stk_on_nodes,
                                gscale_cpue_per_stk_on_nodes,
                                loadedDataVessels.vectiparam1.at(i),
                                loadedDataVessels.vectiparam2.at(i),
                                loadedDataVessels.vectdparam1.at(i),
                                loadedDataVessels.vectdparam2.at(i),
                                loadedDataVessels.vectdparam3.at(i),
                                loadedDataVessels.vectdparam4.at(i),
                                loadedDataVessels.vectdparam5.at(i),
                                loadedDataVessels.vectdparam6.at(i),
                                loadedDataVessels.vectdparam7.at(i),
                                loadedDataVessels.vectdparam8.at(i),
                                loadedDataVessels.vectdparam9.at(i),
                                loadedDataVessels.vectdparam10.at(i),
                                loadedDataVessels.vectdparam11.at(i),
                                loadedDataVessels.vectdparam12.at(i),
                                loadedDataVessels.vectdparam13.at(i),
                                loadedDataVessels.vectdparam14.at(i),
                                loadedDataVessels.vectiparam3.at(i),
                                loadedDataVessels.vectcalendar1.at(i),
                                i < loadedDataVessels.vectdparam16.size() ? loadedDataVessels.vectdparam16.at(i) : 0,
                                i < loadedDataVessels.vectdparam17.size() ? loadedDataVessels.vectdparam17.at(i) : 0,
                                i < loadedDataVessels.vectdparam18.size() ? loadedDataVessels.vectdparam18.at(i) : 0,
                                i < loadedDataVessels.vectdparam19.size() ? loadedDataVessels.vectdparam19.at(i) : 0,
                                i < loadedDataVessels.vectdparam20.size() ? loadedDataVessels.vectdparam20.at(i) : 0,
                                i < loadedDataVessels.vectdparam21.size() ? loadedDataVessels.vectdparam21.at(i) : 0,
                                i < loadedDataVessels.vectdparam22.size() ? loadedDataVessels.vectdparam22.at(i) : 0,
                                i < loadedDataVessels.vectdparam23.size() ? loadedDataVessels.vectdparam23.at(i) : 0,
                                i < loadedDataVessels.vectdparam24.size() ? loadedDataVessels.vectdparam24.at(i) : 0,
                                i < loadedDataVessels.vectdparam25.size() ? loadedDataVessels.vectdparam25.at(i) : 0,
                                i < loadedDataVessels.vectdparam26.size() ? loadedDataVessels.vectdparam26.at(i) : 0,
                                i < loadedDataVessels.vectdparam27.size() ? loadedDataVessels.vectdparam27.at(i) : 0,
                                i < loadedDataVessels.vectdparam28.size() ? loadedDataVessels.vectdparam28.at(i) : 0
        );


#if 0
         if(vessels[i]->get_other_variable_costs_per_unit_effort()==0 ) {
           cout << "debug here" << endl;
           cout << "i is " << i << endl;
           cout << "other_variable_costs_per_unit_efforts[i] is " << other_variable_costs_per_unit_efforts[i];
           cout << "other_variable_costs_per_unit_efforts.size() is " << other_variable_costs_per_unit_efforts.size();

            int aa; cin >> aa;
        }

#endif

        // Give super power to each vessel (so that he can consult the common tariff map for example)
        vessels[i]->set_map_of_nodes(simModel->nodes());


        if(namefolderinput=="BalticSea")
        {
            vessels[i]->set_tankcapacity(vessels[i]->get_tankcapacity()*3); // ACCOUNT FOR MISREPORTING in KW engine THAT CAN INTERFERE WITH STOPFISHING DTREE IN A BAD WAY i.e. limiting factor making 0 catch when triggered to return to port immediately.
        }

        if (dyn_alloc_sce.option(Options::fishing_credits))
        {
            vessels[i]->set_fishing_credits(spe_fishing_credits);
        }


        // initialise the individual quota from global_TAC*percent_in_simu*percent_this_vessel
        if (simModel->is_tacs()) {
            for (unsigned int sp = 0; sp < populations.size(); sp++) {
                vessels.at(i)->set_individual_tac_this_pop(export_individual_tacs, 0, populations,
                                                           simModel->config().implicit_pops, sp, 1,
                                                           0.0);
            }
        }


        // check
        outc(cout << "create vessel " << vessels[i]->get_idx() << " " << vessels[i]->get_name() << " "
                  << vessels[i]->get_nationality() << " on "
                  << vessels[i]->get_loc()->get_idx_node() << " with coordinates "
                  << vessels[i]->get_loc()->get_x() << " " << vessels[i]->get_loc()->get_y() << endl);

        if (enable_sqlite_out) {
            outSqlite->getVesselDefTable()->feedVesselsDefTable(vessels[i]);
        }
    }


#if 0
    //check vessel specifications
    outc(cout << " vessel" << vessels[0]->get_idx() << " have the specific harbours:" << endl);
    auto harbs = vessels[0]->get_harbours();
    for (unsigned int i = 0; i < harbs.size(); i++) {
        outc(cout << harbs[i] << " " << endl);
    }

    //check vessel specifications
    outc(cout << " vessel" << vessels[0]->get_idx() << " have the specfic grounds:" << endl);
    auto grds = vessels[0]->get_fgrounds();
    for (unsigned int i = 0; i < grds.size(); i++) {
        outc(cout << grds[i] << " " << endl);
    }

    //check vessel specifications
    outc(cout << " vessel" << vessels[0]->get_idx() << " have a max speed of "
              << vessels[0]->get_speed() << " " << endl);

    cout << " vessel" << vessels[0]->get_idx() << " have a kW of "
         << vessels[0]->get_KW() << " " << endl;


    /*  //check movement
      // (caution: take really care of this piece of code that is able to uncouple the vessel to the graph if altered,
      // i.e. the "go straight" symptom)
      int old_node = vessels[0]->get_loc()->get_idx_node();
      Node* p_node = new Node(1588, graph_coord_x, graph_coord_y, graph_coord_harbour, simModel->config().nbpops, 5);
      vessels[0]->move_to(p_node);
      dout << "move vessel "<< vessels[0]->get_idx() <<" on "
      << vessels[0]->get_loc()->get_x() << " " << vessels[0]->get_loc()->get_y() << endl;
      vessels[0]->move_to(nodes[old_node]);
      dout << "move vessel "<< vessels[0]->get_idx() <<" on "
      << vessels[0]->get_loc()->get_x() << " " << vessels[0]->get_loc()->get_y() << endl;

      // check the update of a node (will be useful for the pop model and removals of catches)
      vector<int> tab2= nodes[old_node]->get_vid();
      dout(cout  << "idx vessel(s) on this node " << nodes[old_node]->get_idx_node().toIndex() << endl);
      for (int i=0; i<tab2.size(); i++)
      {
          dout(cout  << tab2[i] << " ");
      }
      dout(cout  << endl);

    */

#endif


    // read nodes in closed area this month for area-based management,
    // (and setAreaType on the fly for displacing other_land if closed_to_other_as_well)
    if (dyn_alloc_sce.option(Options::area_monthly_closure)) {

        if (!read_metier_monthly_closures(simModel->nodes(), modelLoader->monthString(), a_graph_name,
                                          folder_name_parameterization,
                                          inputfolder)) {
            exit(1);
        }
        if (!read_vsize_monthly_closures(simModel->nodes(), modelLoader->monthString(), a_graph_name,
                                         folder_name_parameterization,
                                         inputfolder)) {
            exit(1);
        }

    }
    if (dyn_alloc_sce.option(Options::area_closure)) {

        if (!read_metier_quarterly_closures(simModel->nodes(), modelLoader->quarterString(), a_graph_name,
                                            folder_name_parameterization,
                                            inputfolder)) {
            exit(1);
        }
    }


    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " FIRM-RELATED STUFFS       " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);

    // read general firm features
    vector<int> all_firm_ids;
    vector<string> firm_names;
    vector<int> nb_vessels_per_firm;
    vector<double> some_longs;
    vector<double> some_lats;
    if (!read_firms_features(all_firm_ids, firm_names, nb_vessels_per_firm, some_longs, some_lats,
                             folder_name_parameterization, inputfolder)) {
        cerr << "Error loading firm features. Bailing out.\n";
        return 2;
    }


    cout << "find out vessels_of_the_firm for firm " << endl;


    vector<Firm *> firms(all_firm_ids.size());
    for (unsigned int i = 0; i < all_firm_ids.size(); i++) {

        // vector <Vessel*> vessels_of_the_firm(vessels.size());
        vector<Vessel *> vessels_of_the_firm;

        cout << "vessels_of_the_firm with " << vessels_of_the_firm.size() << " vessels..." << endl;


        // select from a vector of objects
        std::remove_copy_if(
                vessels.begin(),
                vessels.end(),
                std::back_inserter(vessels_of_the_firm),
                boost::bind(test_not_belong_to_firm, _1, all_firm_ids[i]));


        cout << "create firm " << all_firm_ids[i] << endl;
        cout << "with " << vessels_of_the_firm.size() << " vessels..." << endl;

        firms[i] = new Firm(i, all_firm_ids[i], firm_names[i], nb_vessels_per_firm[i],
                            some_longs[i], some_lats[i], vessels_of_the_firm);
        firms[i]->set_idx_firm(all_firm_ids[i]);

        cout << "at (" << firms[i]->get_x() << "," << firms[i]->get_y() << ") " << endl;
        //   cout<<" one of the vessel of this firm is " << firms[i]->get_bunch_of_vessels().at(0)->get_name()    << endl;


    }
    cout << "Number of firms created: " << firms.size() << endl;


#ifdef PROFILE
    mLoadVesselProfileResult = mLoadProfile.elapsed_ms();
#endif


    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " TEST GRAPH-RELATED STUFFS " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);

    /* input data, graph connections and distance */
    // CAUTION INDEXATION C++ from 0 to n while in R from 1 to n+1
    // so take care to have idx of node starting from 0 in the input file
    ifstream graph;
    string filename_graph_test = inputfolder + "/graphsspe/graph" + a_graph_s + ".dat";
    graph.open(filename_graph_test.c_str());
    if (graph.fail()) {
        open_file_error(filename_graph_test.c_str());
        return 1;
    }
    vector<int> graph_idx_dep;
    vector<int> graph_idx_arr;
    vector<int> graph_dist_km;     // caution: use integer here to speed up c++
    fill_from_graph(graph, graph_idx_dep, graph_idx_arr, graph_dist_km, nrow_graph);

    /* fill in an adjacency map */

    // TODO Check: graph_coord_x.size() == nodes.size()
    for (unsigned int i = 0; i < simModel->nodes().size(); i++) {
        string s;
        stringstream out;
        out << i;
        s = out.str();
        vertex_names.push_back(s);
    }

    // the graph is non-oriented so need to inform in both ways i.e. dep->arr, arr->dep....
    // col 1
    for (unsigned int i = 0; i < graph_idx_dep.size(); i++) {
        adjacency_map[graph_idx_dep[i]].push_back(edge(graph_idx_arr[i], graph_dist_km[i]));
        //dout(cout  << "dep " << graph_idx_dep[i] << " arr " << graph_idx_arr[i] << endl);
    }
    // col 2
    for (unsigned int i = 0; i < graph_idx_dep.size(); i++) {
        adjacency_map[graph_idx_arr[i]].push_back(edge(graph_idx_dep[i], graph_dist_km[i]));
        //dout(cout  << "arr " << graph_idx_arr[i] << " dep " << graph_idx_dep[i] << endl);
    }

    // then need to remove the duplicates if any.....
    adjacency_map_t::iterator it_adj;
    for (it_adj = adjacency_map.begin(); it_adj != adjacency_map.end(); it_adj++) {
        //  adjacency_map[(*it_adj).first].sort(); // first sort() before unique()
        //  adjacency_map[(*it_adj).first].unique(); // remove duplicate on a sorted list
    }

    graph.close();

    // test removing some nodes for steaming.....
    vector<int> nodes_to_be_closed;
    //nodes_to_be_closed.push_back(1908);
    //closeSomeNodes(nodes_to_be_closed, adjacency_map);

    // create a shop of paths


    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " BUILD A PATHS_SHOP        " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);

#ifdef PROFILE
    mLoadProfile.start();
#endif

    // bound the two vectors
    // copy
    if (!load_relevant_nodes(folder_name_parameterization, inputfolder, relevant_nodes)) {
        cerr << "*** cannot load file." << endl;
        return -1;
    }


    // check
    outc(cout << "relevant nodes: " << endl);
    for (unsigned int i = 0; i < relevant_nodes.size(); i++) {
        outc(cout << relevant_nodes.at(i).toIndex() << " ");
    }
    outc(cout << endl);


    if (!use_static_paths) {
        cout
                << "In Simulation Setup, you have chosen not to use pre-defined static paths... The computation of the paths on the fly might take more time"
                << endl;
    } else {
        cout << "reading pre-existing paths shop....wait" << endl;

        // for-loop over potential departure nodes
        // TO FILL IN THE PATH_SHOP and IDX_PATH_SHOP
        for (unsigned int i = 0; i < relevant_nodes.size(); i++) {
            outc(cout << ".");

            PathShop curr_path_shop = read_graph_details(relevant_nodes.at(i), namefolderinput, inputfolder,
                                                         a_graph_name);
            pathshops.push_back(curr_path_shop);

        }

#ifdef PROFILE
        mLoadGraphProfileResult = mLoadProfile.elapsed_ms();
#endif

        cout << "reading pre-existing paths shop....ok" << endl;


    }


    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " SETTING UP GNUPLOT        " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);

    // short note on the R code to get a map.dat file
    // for coastline map:
    // library(mapdata)
    // ss<-map('worldHires', c('Denmark','Sweden','Norway','Germany','UK','Netherlands','Belgium','France','poland'))
    // write.table(round(cbind(ss$x,ss$y),4),file="map.dat",row.names=FALSE,sep=" ")
    // then replace the NA by nothing

#ifdef _WIN32
                                                                                                                            // realtime gnuplot
    if(use_gnuplot)
    {
        pipe2 = _popen(path, "w");
        //pipe2 = popen("gnuplot", "w"); // for gnuplot installed for MinGW_with_gcc_4.5.2, a previous version
        //pipe3 = popen("gnuplot", "w"); //this opens gnuplot
        //this opens gnuplot
        pipe3 = _popen(path, "w");
        //this opens gnuplot
        pipe4 = _popen(path, "w");
        if (pipe2==NULL || pipe3==NULL)
        {
            printf("Error opening pipe to GNU plot. Check if you have it! \n");
            exit(0);
        }
        else
        {
            dout(cout  << "gnuplot opened...." << endl);
        }
        fprintf(pipe2, "set terminal windows 0 size 400,400 position 100,100\n");
        // i.e. canadian
        if(namefolderinput=="final" || namefolderinput=="fake")
        {
            //telling gnuplot the range for axes
            fprintf(pipe2, "set xrange [-9:22]\n");
            fprintf(pipe2, "set yrange [48:65]\n");
        }
        //
        if(namefolderinput=="balticonly")
        {
            //telling gnuplot the range for axes
            fprintf(pipe2, "set xrange [5:20]\n");
            fprintf(pipe2, "set yrange [54:59]\n");
        }
        fprintf(pipe3, "set terminal windows 1 size 400,400 position 100,500\n");
        //telling gnuplot the range for axes
        fprintf(pipe3, "set xrange [0:10000]\n");
        fprintf(pipe3, "set yrange [0:200000]\n");
        fprintf(pipe4, "set terminal windows 2 size 400,400 position 100,1000\n");
        //telling gnuplot the range for axes
        fprintf(pipe4, "set xrange [0:10000]\n");
        fprintf(pipe4, "set yrange [0:200000]\n");
    }
#endif

    dout(cout << "---------------------------------" << endl);
    dout(cout << "---------------------------------" << endl);
    dout(cout << " THE FOR-LOOP OVER TIME STEPS    " << endl);
    dout(cout << "---------------------------------" << endl);
    dout(cout << "---------------------------------" << endl);

    //	filename=pathoutput+"/DISPLACE_outputs/"+namefolderinput+"/"+namefolderoutput+"/vmslike_"+namesimu+".dat";
    //	vmslike.open(filename.c_str());
    //    std::string vmslike_filename = filename;

    //	filename=pathoutput+"/DISPLACE_outputs/"+namefolderinput+"/"+namefolderoutput+"/loglike_"+namesimu+".dat";
    //	loglike.open(filename.c_str());
    //    std::string loglike_filename = filename;

    ofstream loglike_prop_met;
    filename =
            outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/loglike_prop_met_" + namesimu +
            ".dat";
    loglike_prop_met.open(filename.c_str());

    ofstream popstats;
    filename =
            outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/popstats_" + namesimu + ".dat";
    popstats.open(filename.c_str());
    std::string popstats_filename = filename;

    ofstream quotasuptake;
    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/quotasuptake_" + namesimu +
               ".dat";
    quotasuptake.open(filename.c_str());
    std::string quotasuptake_filename = filename;

    ofstream popdyn_N;
    filename =
            outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/popdyn_" + namesimu + ".dat";
    popdyn_N.open(filename.c_str());
    std::string popdyn_N_filename = filename;

    ofstream popdyn_F;
    filename =
            outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/popdyn_F_" + namesimu + ".dat";
    popdyn_F.open(filename.c_str());
    std::string popdyn_F_filename = filename;

    ofstream popdyn_SSB;
    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/popdyn_SSB_" + namesimu +
               ".dat";
    popdyn_SSB.open(filename.c_str());
    std::string popdyn_SSB_filename = filename;

    ofstream popdyn_annual_indic;
    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/popdyn_annual_indic_" +
               namesimu + ".dat";
    popdyn_annual_indic.open(filename.c_str());

    ofstream popdyn_test;
    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/popdyn_test" + namesimu +
               ".dat";
    popdyn_test.open(filename.c_str());

    ofstream popdyn_test2;
    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/popdyn_test2" + namesimu +
               ".dat";
    popdyn_test2.open(filename.c_str());

    ofstream popnodes_start;
    filename =
            outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/popnodes_start_" + namesimu +
            ".dat";
    popnodes_start.open(filename.c_str());
    std::string popnodes_start_filename = filename;

    ofstream popnodes_inc;
    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/popnodes_inc_" + namesimu +
               ".dat";
    popnodes_inc.open(filename.c_str());
    std::string popnodes_inc_filename = filename;

    lock();
    ofstream popnodes_end;
    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/popnodes_end_" + namesimu +
               ".dat";
    popnodes_end.open(filename.c_str());
    std::string popnodes_end_filename = filename;
    unlock();

    ofstream popnodes_impact;
    filename =
            outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/popnodes_impact_" + namesimu +
            ".dat";
    popnodes_impact.open(filename.c_str());
    std::string popnodes_impact_filename = filename;

    ofstream popnodes_cumulcatches_per_pop;
    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput +
               "/popnodes_cumulcatches_per_pop_" + namesimu + ".dat";
    popnodes_cumulcatches_per_pop.open(filename.c_str());
    std::string popnodes_cumulcatches_per_pop_filename = filename;

    ofstream nodes_envt;
    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/nodes_envt_" + namesimu +
               ".dat";
    nodes_envt.open(filename.c_str());
    std::string nodes_envt_filename = filename;

    ofstream popnodes_impact_per_szgroup;
    filename =
            outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/popnodes_impact_per_szgroup_" +
            namesimu + ".dat";
    popnodes_impact_per_szgroup.open(filename.c_str());

    ofstream popnodes_cumftime;
    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/popnodes_cumftime_" +
               namesimu + ".dat";
    popnodes_cumftime.open(filename.c_str());
    std::string popnodes_cumftime_filename = filename;

    ofstream popnodes_cumsweptarea;
    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/popnodes_cumsweptarea_" +
               namesimu + ".dat";
    popnodes_cumsweptarea.open(filename.c_str());
    std::string popnodes_cumsweptarea_filename = filename;

    ofstream popnodes_cumcatches;
    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/popnodes_cumcatches_" +
               namesimu + ".dat";
    popnodes_cumcatches.open(filename.c_str());
    std::string popnodes_cumcatches_filename = filename;

    ofstream popnodes_cumcatches_with_threshold;
    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput +
               "/popnodes_cumcatches_with_threshold_" + namesimu + ".dat";
    popnodes_cumcatches_with_threshold.open(filename.c_str());
    std::string popnodes_cumcatches_with_threshold_filename = filename;

    ofstream popnodes_cumdiscards;
    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/popnodes_cumdiscards_" +
               namesimu + ".dat";
    popnodes_cumdiscards.open(filename.c_str());
    std::string popnodes_cumdiscards_filename = filename;

    ofstream popnodes_cumdiscardsratio;
    filename =
            outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/popnodes_cumdiscardsratio_" +
            namesimu + ".dat";
    popnodes_cumdiscardsratio.open(filename.c_str());
    std::string popnodes_cumdiscardsratio_filename = filename;

    ofstream popnodes_nbchoked;
    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/popnodes_nbchoked_" +
               namesimu + ".dat";
    popnodes_nbchoked.open(filename.c_str());
    std::string popnodes_nbchoked_filename = filename;

    ofstream popnodes_tariffs;
    filename =
            outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/popnodes_tariffs_" + namesimu +
            ".dat";
    popnodes_tariffs.open(filename.c_str());
    std::string popnodes_tariffs_filename = filename;

    ofstream benthosbiomassnodes;
    filename =
            outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/benthosnodes_tot_biomasses_" +
            namesimu + ".dat";
    benthosbiomassnodes.open(filename.c_str());
    std::string popnodes_benthos_biomass_filename = filename;

    ofstream benthosnumbernodes;
    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/benthosnodes_tot_numbers_" +
               namesimu + ".dat";
    benthosnumbernodes.open(filename.c_str());
    std::string popnodes_benthos_number_filename = filename;

    filename =
            outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/freq_cpue" + namesimu + ".dat";
    freq_cpue.open(filename.c_str());

    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/freq_profit" + namesimu +
               ".dat";
    freq_profit.open(filename.c_str());

    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/freq_distance" + namesimu +
               ".dat";
    freq_distance.open(filename.c_str());

    ofstream fishfarmslogs;
    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/fishfarmslogs_" + namesimu +
               ".dat";
    fishfarmslogs.open(filename.c_str());
    std::string fishfarmslogs_filename = filename;

    ofstream windmillslogs;
    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/windmillslogs_" + namesimu +
               ".dat";
    windmillslogs.open(filename.c_str());
    std::string windmillslogs_filename = filename;

    ofstream shipslogs;
    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/shipslogs_" + namesimu +
               ".dat";
    shipslogs.open(filename.c_str());
    std::string shipslogs_filename = filename;

    // read list of tsteps with discrete events
    vector<int> tsteps_quarters = read_tsteps_quarters(folder_name_parameterization, inputfolder);
    vector<int> tsteps_semesters = read_tsteps_semesters(folder_name_parameterization, inputfolder);
    vector<int> tsteps_years = read_tsteps_years(folder_name_parameterization, inputfolder);
    vector<int> tsteps_months = read_tsteps_months(folder_name_parameterization, inputfolder);

    if (enable_sqlite_out) {
        outSqlite->exportCalendar(tsteps_months, tsteps_quarters, tsteps_semesters, tsteps_years);
    }

    int count_quarters = 1;
    int count_months = 1;
    double a_year = 1.0;

    // get a vector v filled in with 1 to n
    glob_mutex.lock();
    ve = vector<int>(nbvessels);
    for (unsigned int idx = 0; idx < ve.size(); idx++) {
        ve[idx] = idx;
    }
    glob_mutex.unlock();

    // init
    vector<vector<double> > a_catch_pop_at_szgroup(simModel->config().nbpops, vector<double>(NBSZGROUP));
    vector<vector<double> > a_ping_catch_pop_at_szgroup(simModel->config().nbpops, vector<double>(NBSZGROUP));
    vector<vector<double> > a_discards_pop_at_szgroup(simModel->config().nbpops, vector<double>(NBSZGROUP));

    // write down initial pop number in popdyn
    for (unsigned int sp = 0; sp < populations.size(); sp++) {

        dout(cout << "write down the popdyn...");
        // get total N from summing up N over nodes
        populations.at(sp)->aggregate_N();
        popdyn_N << setprecision(0) << fixed;
        // tstep / pop / tot N at szgroup
        popdyn_N << 0 << " " << sp << " ";
        vector<double> tot_N_at_szgroup = populations.at(sp)->get_tot_N_at_szgroup();
        for (unsigned int sz = 0; sz < tot_N_at_szgroup.size(); sz++) {
            // output in thousands of individuals
            popdyn_N << tot_N_at_szgroup.at(sz) / 1000 << " ";
        }
        popdyn_N << " " << endl;
    }

    popdyn_N.flush();
    guiSendUpdateCommand(popdyn_N_filename, 0);



    //AT THE VERY START: export biomass pop on nodes for mapping e.g. in GIS
    if (enable_sqlite_out) {
        outSqlite->startDayLoop();
    }


    for (unsigned int n = 0; n < simModel->nodes().size(); n++) {
        simModel->nodes()[n]->export_popnodes(popnodes_start, init_weight_per_szgroup, 0);
        if (enable_sqlite_out) {
            outSqlite->getNodesDefTable()->insert(simModel->nodes()[n]);
            bool r = outSqlite->getPopTable()->insert(0, simModel->nodes()[n], init_weight_per_szgroup);
        }
    }

    if (enable_sqlite_out) {
        outSqlite->endDayLoop();
    }

    popnodes_start.flush();
    // signals the gui that the filename has been updated.
    guiSendUpdateCommand(popnodes_start_filename, 0);


    // initial export at t=0
    // if(dyn_pop_sce.option(Options::include_forcing_layers))
    //{

    // Flush and updates all statistics for nodes envt
    if (use_gui) {
        nodes_envt.flush();
        for (unsigned int n = 0; n < simModel->nodes().size(); n++) {
            simModel->nodes().at(n)->export_nodes_envt(nodes_envt, tstep);
        }
        guiSendUpdateCommand(nodes_envt_filename, tstep);
    }

    if (enable_sqlite_out) {
        for (unsigned int n = 0; n < simModel->nodes().size(); n++) {
            outSqlite->exportEnvtNodes(tstep, simModel->nodes().at(n));
        }
    }
    //}

    //----------------------//
    //----------------------//
    //----------------------//
    // THE FOR-LOOP OVER TIME
    //----------------------//
    //----------------------//
    //----------------------//

    int LastMonth = -1;
    int CurrentMonth = 0;
    int nb_y_left_to_tgrt_year = 5; // init for EffortControl Option

    /* CALLGRING -- Instrument */
    CALLGRIND_START_INSTRUMENTATION;

    for (tstep = 0; tstep < nbsteps; ++tstep) {
        if (enable_sqlite_out && (tstep % numStepTransactions) == 0) {
            std::cout << "Start Transaction " << tstep << "\n";
            outSqlite->startDayLoop();
        }

#ifdef PROFILE
            mLoopProfile.start();
#endif

        dout(cout << endl);
        dout(cout << endl);
        dout(cout << "---------------" << endl);

        guiSendCurrentStep(tstep);

        if (!use_gui) {
            cout << "tstep: " << tstep << endl;
        }
        ostringstream os;
        os << "tstep " << tstep << endl;
        guiSendTerminalMessage(os.str());

        dout(cout << "---------------" << endl);

        if (use_gnuplot) {
            filename =
                    outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/vmslike2_" + namesimu +
                    ".dat";
            vmslike2.open(filename.c_str());

            filename =
                    outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/vmslike3_" + namesimu +
                    ".dat";
            vmslike3.open(filename.c_str());
        }




        //----------------------------------------//
        //----------------------------------------//
        // BIOLOGICAL MODULE----------------------//
        //----------------------------------------//
        //----------------------------------------//



        if (!applyBiologicalModule2(tstep,
                                    simModel->month(),
                                    namesimu,
                                    namefolderinput,
                                    namefolderoutput,
                                    outdir,
                                    popstats,
                                    popdyn_N,
                                    popdyn_F,
                                    popdyn_SSB,
                                    popdyn_annual_indic,
                                    popnodes_inc,
                                    popnodes_impact,
                                    popnodes_cumulcatches_per_pop,
                                    nodes_envt,
                                    popnodes_cumftime,
                                    popnodes_cumsweptarea,
                                    popnodes_cumcatches,
                                    popnodes_cumcatches_with_threshold,
                                    popnodes_cumdiscards,
                                    popnodes_cumdiscardsratio,
                                    popnodes_nbchoked,
                                    popnodes_tariffs,
                                    export_individual_tacs,
                                    popnodes_end,
                                    benthosbiomassnodes,
                                    benthosnumbernodes,
                                    simModel->config().nbbenthospops,
                                    use_gui,
                                    popstats_filename,
                                    popdyn_N_filename,
                                    popdyn_F_filename,
                                    popdyn_SSB_filename,
                                    popnodes_inc_filename,
                                    popnodes_end_filename,
                                    popnodes_impact_filename,
                                    popnodes_cumulcatches_per_pop_filename,
                                    nodes_envt_filename,
                                    popnodes_cumftime_filename,
                                    popnodes_cumsweptarea_filename,
                                    popnodes_cumcatches_filename,
                                    popnodes_cumcatches_with_threshold_filename,
                                    popnodes_cumdiscards_filename,
                                    popnodes_cumdiscardsratio_filename,
                                    popnodes_nbchoked_filename,
                                    popnodes_tariffs_filename,
                                    popnodes_benthos_biomass_filename,
                                    popnodes_benthos_number_filename,
                                    tsteps_quarters,
                                    tsteps_semesters,
                                    tsteps_years,
                                    tsteps_months,
                                    simModel->config().implicit_pops,
                                    simModel->config().calib_oth_landings,
                                    selectivity_per_stock_ogives_for_oth_land,
                                    simModel->is_tacs(),
                                    export_vmslike,
                                    freq_do_growth,
                                    init_weight_per_szgroup,
                                    species_interactions_mortality_proportion_matrix,
                                    populations,
                                    simModel->nodes(),
                                    vessels,
                                    benthoss,
                                    dyn_pop_sce,
                                    dyn_alloc_sce,
                                    Ws_at_szgroup,
                                    predKernel,
                                    searchVolMat,
                                    juveniles_diet_preference,
                                    adults_diet_preference,
                                    mat_cats
        )) {
            throw std::runtime_error("Error while executing: applyBiologicalModule2");
        }


        if (dyn_pop_sce.option(Options::diffusePopN) &&
            binary_search(tsteps_months.begin(), tsteps_months.end(), tstep)) {
            // diffusion of pops on neighbour nodes
            // field_of_coeff_diffusion_this_pop give the node specific coeffs of diffusion
            // we can assume that this coeff is larger when the node is just transitional vs. lower when the node is a residential area
            // so a relevant proxy could actually be (the inverse of) full_avai_szgroup_nodes_with_pop
            // converted in a point porportion field....
            for (unsigned int sp = 0; sp < populations.size(); sp++) {
                outc(cout << "...pop " << sp << endl;)
                if (!binary_search(simModel->config().implicit_pops.begin(),
                                   simModel->config().implicit_pops.end(), sp)) {
                    outc(cout << "......pop " << sp << endl;)
                    populations.at(sp)->diffuse_N_from_field(adjacency_map); // per sz group
                }
            }
        }








        //----------------------------------------//
        //----------------------------------------//
        // READ READ DATA WHEN NEEDED-------------//
        //----------------------------------------//
        //----------------------------------------//

        if (binary_search(tsteps_years.begin(), tsteps_years.end(), tstep)) {
            a_year += 1;


            if (dyn_alloc_sce.option(Options::TechCreeping3Per)) {
                tech_creeping_multiplier = pow(1.03, a_year); //  this is a rate per year...so multiply over years
            } else {
                tech_creeping_multiplier = 1.0;
            }



            // fishfarms for new year
            for (unsigned int i = 0; i < fishfarms.size(); ++i) {
                fishfarms.at(i)->set_is_running(1);
            }


        }
        //cout << "a_year " << a_year << endl;



        dout(cout << "RE-READ DATA----------" << endl);
        multimap <string, double> reloaded_fcredits;

        // RE-READ DATA FOR EVENT => change of month
        if (binary_search(tsteps_months.begin(), tsteps_months.end(), tstep)) {
            CurrentMonth += 1;

            {
                count_months += 1;
                auto a_month_i = count_months % 12;
                int a_quarter_i, a_semester_i;

                if (a_month_i == 0) { a_month_i = 12; }
                if (a_month_i == 1 || a_month_i == 2 || a_month_i == 3) { a_quarter_i = 1; }
                if (a_month_i == 4 || a_month_i == 5 || a_month_i == 6) { a_quarter_i = 2; }
                if (a_month_i == 7 || a_month_i == 8 || a_month_i == 9) { a_quarter_i = 3; }
                if (a_month_i == 10 || a_month_i == 11 || a_month_i == 12) { a_quarter_i = 4; }
                if (a_quarter_i == 1 || a_quarter_i == 2) { a_semester_i = 1; }
                if (a_quarter_i == 3 || a_quarter_i == 4) { a_semester_i = 2; }

                simModel->setMonth(a_month_i);
                simModel->setQuarter(a_quarter_i);
                simModel->setSemester(a_semester_i);
            }


            // vector <double> a_tot_N_at_szgroup_here = populations.at(1)->get_tot_N_at_szgroup();
            // for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
            //  cout << "BEFORE RE-READ DATA: a_tot_N_at_szgroup[" << sz << "] is "<< a_tot_N_at_szgroup_here[sz]  << endl;

            // this month, re-read for vessel-related data
            if (dyn_alloc_sce.option(Options::area_monthly_closure)) {
                cout << "a_month: " << simModel->month() << ", a_quarter: " << simModel->quarter()
                     << ", simModel->semester():" << simModel->semester() << endl;

                for (unsigned int v = 0; v < vessels.size(); v++) {
                    vessels.at(v)->reinitDaysSpentInRestrictedAreaThisMonthtoZero();
                }
                // update the monthly closures
                if (!read_metier_monthly_closures(simModel->nodes(), modelLoader->monthString(), a_graph_name,
                                                  folder_name_parameterization,
                                                  inputfolder)) {
                    exit(1);
                }
                if (!read_vsize_monthly_closures(simModel->nodes(), modelLoader->monthString(), a_graph_name,
                                                 folder_name_parameterization,
                                                 inputfolder)) {
                    exit(1);
                }

                // check for myfish graph1
                // cout << " isMetierBanned   "  << nodes.at(13)->isMetierBanned(vessels.at(v)->get_metier()->get_name()) << endl;
                // cout << " isVsizeBanned   " << nodes.at(13)->isVsizeBanned(vessels.at(v)->get_length_class()) << endl;


                cout << "re-read area closure setting this month....OK" << endl;
            }



            // this month, re-read for population-related data
            // CAUTION: THE ONLY POP READING DONE ON MONTH TSTEP...THE OTHERS ARE DONE ON QUARTER BASIS
            for (unsigned int i = 0; i < populations.size(); i++) {
                // read a other landings per node for this species
                auto oth_land = read_oth_land_nodes_with_pop(modelLoader->semesterString(),
                                                             modelLoader->monthString(), i,
                                                             folder_name_parameterization,
                                                             inputfolder, fleetsce);
                populations.at(i)->set_oth_land(oth_land);
            }


        }


        // RE-READ DATA FOR EVENT => change of quarter
        if (tstep > 2000 && binary_search(tsteps_quarters.begin(), tsteps_quarters.end(), tstep))
            //   if(tstep==3 || tstep==4) // use this to start from another quarter if test...
        {

            outc(cout << "a_quarter: " << simModel->quarter() << ", a_semester:" << simModel->semester() << endl);

            // RE-READ VESSEL DATA
            // fill in with new input files for fgrounds and harbours, etc.
            // if change of year-quarter or semester, to be quarter or semester-specific.

            // RE-read general vessel features: do not forget to clear the vectors!
            // not-quarter specific, clear anyway...
            // actually those variables do not change from a quarter to the next (see IBM_param_step4_vessels)

            paramsForLoad.sparam1 = std::to_string(simModel->month());
            paramsForLoad.sparam2 = std::to_string(simModel->quarter());
            paramsForLoad.sparam3 = std::to_string(simModel->semester());
            paramsForLoad.iparam1 = simModel->config().nbpops;
            paramsForLoad.iparam2 = NBAGE;
            paramsForLoad.iparam3 = NBSZGROUP;

            Dataloadervessels vrl;
            l->loadFeatures(&vrl,
                            indb,
                            folder_name_parameterization,
                            inputfolder,
                            dyn_pop_sce,
                            dyn_alloc_sce,
                            biolsce,
                            fleetsce,
                            paramsForLoad,
                            loadedDataVessels);



            // LOOP OVER VESSELS
            for (unsigned int v = 0; v < vessels.size(); v++) {
                dout(cout << "re-read data for vessel " << vessels.at(v)->get_name() << endl);

                if (dyn_alloc_sce.option(Options::ExitVessels10Per)) {
                    double exit_vessels_per_year = 0.1; //  this is a rate of vessel leaving per year
                    if (binary_search(tsteps_years.begin(), tsteps_years.end(), tstep)) {

                        if ((rand() % 2) < exit_vessels_per_year) { vessels.at(v)->set_vessel_exited(1); }

                    }
                }

                if (simModel->quarter() == 1) {
                    double new_vessel_value =
                            vessels.at(v)->get_vessel_value() * (100 - vessels.at(v)->get_annual_depreciation_rate()) /
                            100;
                    vessels.at(v)->set_vessel_value(new_vessel_value); // capital depreciation

                    for (unsigned int pop = 0; pop < simModel->config().nbpops; ++pop)
                        vessels.at(v)->set_is_choked(pop, 0); // reinit at year start
                }

                possible_metiers = loadedDataVessels.vectmmapniparam1.at(v);
                freq_possible_metiers = loadedDataVessels.vectmmapndparam1.at(v);
                gshape_cpue_per_stk_on_nodes = loadedDataVessels.vectmmapndparam2.at(v);
                gscale_cpue_per_stk_on_nodes = loadedDataVessels.vectmmapndparam3.at(v);
                vector<string> vesselids = loadedDataVessels.vectsparam1;
                spe_fgrounds = find_entries(loadedDataVessels.mmapsnparam2, vesselids.at(v));
                spe_fgrounds_init = find_entries(loadedDataVessels.mmapsnparam3, vesselids.at(v));
                spe_harbours = find_entries(loadedDataVessels.mmapsnparam1, vesselids.at(v));
                spe_freq_fgrounds = find_entries_s_d(loadedDataVessels.mmapsdparam2, vesselids.at(v));
                spe_freq_fgrounds_init = find_entries_s_d(loadedDataVessels.mmapsdparam3, vesselids.at(v));
                spe_freq_harbours = find_entries_s_d(loadedDataVessels.mmapsdparam1, vesselids.at(v));
                spe_vessel_betas_per_pop = find_entries_s_d(loadedDataVessels.mmapsdparam4, vesselids.at(v));
                if (simModel->is_tacs()) {
                    spe_percent_tac_per_pop = find_entries_s_d(loadedDataVessels.mmapsdparam5, vesselids.at(v));
                }
                reloaded_fcredits = loadedDataVessels.mmapsdparam6;

                // correct if missing harbour for this quarter
                if (spe_harbours.empty()) {
                    // if missing info for a given vessel for this quarter
                    outc(cout << "no specified harbour in this quarter for this vessel..." << endl);
                    // CAUTION: TAKEN FROM THE PREVIOUS QUARTER!
                    auto start_harbour = vessels.at(v)->get_harbours()[0];
                    spe_harbours.push_back(start_harbour);
                    spe_freq_harbours.push_back(1);
                    outc(cout << "then take node: " << start_harbour << endl);
                }

                // RE-SET VESSELS..
                dout(cout << "re-set vessels step1..." << endl);
                vessels.at(v)->set_resttime_par1(loadedDataVessels.vectdparam8.at(v));
                vessels.at(v)->set_resttime_par2(loadedDataVessels.vectdparam9.at(v));
                vessels.at(v)->set_av_trip_duration(loadedDataVessels.vectdparam10.at(v));

                if (dyn_alloc_sce.option(Options::area_closure)) {
                    if (!read_metier_quarterly_closures(simModel->nodes(), modelLoader->quarterString(), a_graph_name,
                                                        folder_name_parameterization,
                                                        inputfolder)) {
                        exit(1);
                    }
                }


                vessels.at(v)->set_spe_fgrounds(spe_fgrounds);
                vessels.at(v)->set_spe_fgrounds_init(spe_fgrounds_init);
                vessels.at(v)->set_spe_harbours(spe_harbours);
                vessels.at(v)->set_spe_freq_fgrounds(spe_freq_fgrounds);
                vessels.at(v)->set_spe_freq_fgrounds_init(spe_freq_fgrounds_init);
                vessels.at(v)->set_spe_freq_harbours(spe_freq_harbours);
                vector<double> init_for_fgrounds(vessels.at(v)->get_fgrounds().size());
                for (unsigned int i = 0; i < init_for_fgrounds.size(); i++) {
                    init_for_fgrounds[i] = 0;
                }
                vessels.at(v)->set_spe_cumcatch_fgrounds(init_for_fgrounds);
                vessels.at(v)->set_spe_cumdiscard_fgrounds(init_for_fgrounds);
                vessels.at(v)->set_spe_experienced_bycatch_prop_on_fgrounds(init_for_fgrounds);
                vessels.at(v)->set_spe_experienced_avoided_stks_bycatch_prop_on_fgrounds(init_for_fgrounds);
                vessels.at(v)->set_spe_cumeffort_fgrounds(init_for_fgrounds);
                vessels.at(v)->set_spe_experiencedcpue_fgrounds(init_for_fgrounds);
                vessels.at(v)->set_spe_betas_per_pop(spe_vessel_betas_per_pop);

                // uncomment for use if TACs are semester-based....otherwise don´t refresh the annual TAC!
                //if (semester) vessels.at(v)->set_spe_percent_tac_per_pop(spe_percent_tac_per_pop);

                vessels.at(v)->set_spe_possible_metiers(possible_metiers);
                vessels.at(v)->set_spe_freq_possible_metiers(freq_possible_metiers);
                vessels.at(v)->updateCalendar(loadedDataVessels.vectcalendar1.at(v));


                // ...also for the particular cpue_nodes_species element
                dout(cout << "re-set vessels step2..." << endl);
                vector<types::NodeId> gshape_name_nodes_with_cpue;
                for (auto iter = gshape_cpue_per_stk_on_nodes.begin(); iter != gshape_cpue_per_stk_on_nodes.end();
                     iter = gshape_cpue_per_stk_on_nodes.upper_bound(iter->first)) {
                    gshape_name_nodes_with_cpue.push_back(iter->first);
                }
                // sort and unique
                sort(gshape_name_nodes_with_cpue.begin(), gshape_name_nodes_with_cpue.end());
                auto it = std::unique(gshape_name_nodes_with_cpue.begin(), gshape_name_nodes_with_cpue.end());
                gshape_name_nodes_with_cpue.resize(std::distance(gshape_name_nodes_with_cpue.begin(), it));

                // init cpue_nodes_species for this vessel
                int nbnodes = gshape_name_nodes_with_cpue.size();
                // init the vector of vector with Os
                vessels.at(v)->init_gshape_cpue_nodes_species(nbnodes, simModel->config().nbpops);
                // init the vector of vector with Os
                vessels.at(v)->init_gscale_cpue_nodes_species(nbnodes, simModel->config().nbpops);
                for (unsigned int n = 0; n < gshape_name_nodes_with_cpue.size(); n++) {
                    // look into the multimap...
                    auto gshape_cpue_species = find_entries(gshape_cpue_per_stk_on_nodes,
                                                            gshape_name_nodes_with_cpue[n]);
                    // look into the multimap...
                    auto gscale_cpue_species = find_entries(gscale_cpue_per_stk_on_nodes,
                                                            gshape_name_nodes_with_cpue[n]);
                    if (!gshape_cpue_species.empty()) {
                        // caution here: the n is the relative index of the node for this vessel i.e. this is not the graph index of the node (because it would have been useless to create a huge matrix filled in by 0 just to preserve the graph idex in this case!)
                        vessels.at(v)->set_gshape_cpue_nodes_species(n, gshape_cpue_species);
                        // caution here: the n is the relative index of the node for this vessel i.e. this is not the graph index of the node (because it would have been useless to create a huge matrix filled in by 0 just to preserve the graph idex in this case!)
                        vessels.at(v)->set_gscale_cpue_nodes_species(n, gscale_cpue_species);
                    }
                }

                // need to compute expected cpue (averaged over node but cumulated over species)
                // for this particular vessel, in order to scale the prior guess (see below)
                dout(cout << "re-set vessels step3..." << endl);
                double expected_cpue = 0;
                vector<vector<double> > gshape_cpue_nodes_species = vessels.at(v)->get_gshape_cpue_nodes_species();
                vector<vector<double> > gscale_cpue_nodes_species = vessels.at(v)->get_gscale_cpue_nodes_species();
                const auto &fgrounds = vessels.at(v)->get_fgrounds();
                vector<double> expected_cpue_this_pop(simModel->config().nbpops);
                for (int pop = 0; pop < simModel->config().nbpops; pop++) {
                    vector<double> cpue_per_fground(fgrounds.size());
                    // init
                    expected_cpue_this_pop.at(pop) = 0;

                    // compute cpue on nodes
                    for (unsigned int g = 0; g < fgrounds.size(); g++) {
                        // look into the vector of vector....
                        double a_shape = gshape_cpue_nodes_species.at(g).at(pop);
                        // look into the vector of vector....
                        double a_scale = gscale_cpue_nodes_species.at(g).at(pop);


                        // a dangerous fix:
                        if (a_shape < 0 || a_scale < 0) {

                            cout << "Something weird with the Gamma parameters: some negative values loaded...."
                                 << endl;
                            //for(size_t f = 0; f < fgrounds.size(); ++f)
                            //{
                            //cout <<  " this gr  gscale is: " << gscale_cpue_nodes_species.at(f).at(pop) << endl;
                            //cout <<  " this gr  of gshape is: " << gshape_cpue_nodes_species.at(f).at(pop) << endl;
                            //}
                            a_shape = 1;
                            a_scale = 0;
                        }

                        cpue_per_fground.at(g) = rgamma(a_shape, a_scale);
                        dout(cout << "cpue_per_fground.at(g)" << cpue_per_fground.at(g) << endl);
                        //if(vessels.at(v)->get_name()=="DNK000041435") cout  << "cpue_per_fground.at(g)" <<cpue_per_fground.at(g) << endl;
                    }

                    dout(cout << "re-set vessels step3.1..." << endl);

                    // compute the average cpue for this pop across all nodes
                    for (unsigned int g = 0; g < fgrounds.size(); g++) {
                        expected_cpue_this_pop.at(pop) += cpue_per_fground.at(g);
                    }
                    // do the mean
                    expected_cpue_this_pop.at(pop) = expected_cpue_this_pop.at(pop) / fgrounds.size();

                    // sum over pop
                    expected_cpue += expected_cpue_this_pop.at(pop);
                }

                dout(cout << "expected_cpue for this vessel is " << expected_cpue << endl);

                // init at 0 cumcatch and cumeffort per trip,
                // init at best guest the experiencedcpue_fgrounds
                dout(cout << "re-set vessels step4..." << endl);
                dout(cout << "init dynamic object related to fgrounds" << endl);
                const vector<double> &a_freq_fgrounds = vessels.at(v)->get_freq_fgrounds();
                vector<double> a_init_for_fgrounds(fgrounds.size());
                vector<double> a_cumeffort_fgrounds = a_init_for_fgrounds;
                vector<double> a_cumcatch_fgrounds = a_init_for_fgrounds;
                vector<double> a_cumdiscard_fgrounds = a_init_for_fgrounds;
                vector<double> a_experienced_bycatch_prop_on_fgrounds = a_init_for_fgrounds;
                vector<double> a_experienced_avoided_stks_bycatch_prop_on_fgrounds = a_init_for_fgrounds;
                vector<double> a_experiencedcpue_fgrounds = a_init_for_fgrounds;
                vector<double> a_freq_experiencedcpue_fgrounds = a_init_for_fgrounds;
                vector<vector<double> > a_cumcatch_fgrounds_per_pop(fgrounds.size(),
                                                                    vector<double>(simModel->config().nbpops));
                vector<vector<double> > a_cumdiscard_fgrounds_per_pop(fgrounds.size(),
                                                                      vector<double>(simModel->config().nbpops));
                vector<vector<double> > a_experiencedcpue_fgrounds_per_pop(fgrounds.size(),
                                                                           vector<double>(simModel->config().nbpops));
                vector<vector<double> > a_freq_experiencedcpue_fgrounds_per_pop(fgrounds.size(),
                                                                                vector<double>(
                                                                                        simModel->config().nbpops));

                for (unsigned int g = 0; g < fgrounds.size(); g++) {
                    a_cumcatch_fgrounds[g] = 0;
                    a_cumdiscard_fgrounds[g] = 0;
                    a_experienced_bycatch_prop_on_fgrounds[g] = 0;
                    a_experienced_avoided_stks_bycatch_prop_on_fgrounds[g] = 0;
                    a_cumeffort_fgrounds[g] = 0;
                    a_experiencedcpue_fgrounds[g] = a_freq_fgrounds[g] * expected_cpue;
                    // this should be init so that it constitutes a good qualified guess to be a prior in the bayesian formula...
                    // first condition: init different to 0 to allow the ground to be chosen even if it has not been visited yet...
                    // second condition: to avoid starting from 0 cpue, init accounting for prior from frequency of visit from the data
                    // third condition: to scale the start cpue, multiply by the expectancy of the cpue for this particular vessel
                    dout(cout << "a_experiencedcpue_fgrounds" << a_experiencedcpue_fgrounds[g] << endl);
                    // init the ones per pop
                    for (int pop = 0; pop < simModel->config().nbpops; pop++) {
                        // init
                        a_cumcatch_fgrounds_per_pop[g][pop] = 0;
                        //a_cumdiscard_fgrounds_per_pop[g][pop] = 0;
                        a_experiencedcpue_fgrounds_per_pop[g][pop] =
                                a_freq_fgrounds[g] * expected_cpue_this_pop.at(pop);
                    }
                }
                // per total...
                dout(cout << "re-set vessels step5..." << endl);
                vessels.at(v)->set_cumcatch_fgrounds(a_cumcatch_fgrounds);
                vessels.at(v)->set_cumdiscard_fgrounds(a_cumdiscard_fgrounds);
                vessels.at(v)->set_experienced_bycatch_prop_on_fgrounds(a_experienced_bycatch_prop_on_fgrounds);
                vessels.at(v)->set_experienced_avoided_stks_bycatch_prop_on_fgrounds(
                        a_experienced_avoided_stks_bycatch_prop_on_fgrounds);
                vessels.at(v)->set_cumeffort_fgrounds(a_cumeffort_fgrounds);
                vessels.at(v)->set_experiencedcpue_fgrounds(a_experiencedcpue_fgrounds);
                vessels.at(v)->set_freq_experiencedcpue_fgrounds(a_freq_experiencedcpue_fgrounds);
                // compute for the first time, to get freq_experiencedcpue_fgrounds...
                vessels.at(v)->compute_experiencedcpue_fgrounds();
                // ...or per pop
                vessels.at(v)->set_cumcatch_fgrounds_per_pop(a_cumcatch_fgrounds_per_pop);
                vessels.at(v)->set_cumdiscard_fgrounds_per_pop(a_cumdiscard_fgrounds_per_pop);
                vessels.at(v)->set_experiencedcpue_fgrounds_per_pop(a_experiencedcpue_fgrounds_per_pop);
                vessels.at(v)->set_freq_experiencedcpue_fgrounds_per_pop(a_freq_experiencedcpue_fgrounds_per_pop);
                // compute for the first time, to get freq_experiencedcpue_fgrounds_per_pop...
                vessels.at(v)->compute_experiencedcpue_fgrounds_per_pop();

                // note that, at the start of the simu, freq of visit will be equivalent to a_freq_fgrounds
                // and then freq of visit will be updated (via the bayes rule) trip after trip from this initial freqency...
                // the expected_cpue is to scale to the encountered cpue i.e. freq of visit will decrease if experienced cpue < expected cpue
                // and vice versa...

                //
                // to force re-computation of the fuel saving scenario

                // TODO check: nodeId=0 is a valid value. Perhaps it should be changed to InvalidNodeId ?
                vessels.at(v)->set_previous_harbour_idx(types::NodeId(0));

                // send a message to the vessel to force it for a change in fishing grounds (for the vessels that are fishing now or on their way to fish)
                // because we have just changed the list of fishing grounds! so maybe some vessels are fishing on some nodes
                // that are no longer in the list...
                // message 1 means: "please, change of grounds as soon as possible"
                vessels.at(v)->receive_message(1);

                dout(cout << "re-read data for this vessel..." << vessels.at(v)->get_name() << "...OK" << endl);
            }                     // end a_vesselid

            // RE-read for metiers
            dout(cout << "re-read metiers..." << endl);

            paramsForLoad.sparam1 = std::to_string(simModel->month());
            paramsForLoad.sparam2 = std::to_string(simModel->quarter());
            paramsForLoad.sparam3 = std::to_string(simModel->semester());
            paramsForLoad.iparam1 = simModel->config().nbpops;
            paramsForLoad.iparam2 = NBAGE;
            paramsForLoad.iparam3 = NBSZGROUP;

            Dataloadermetiers mrl;
            l->loadFeatures(&mrl,
                            indb,
                            folder_name_parameterization,
                            inputfolder,
                            dyn_pop_sce,
                            dyn_alloc_sce,
                            biolsce,
                            fleetsce,
                            paramsForLoad,
                            loadedDataMetiers);

            for (unsigned int m = 0; m < metiers.size(); m++) {
                // casting m into a string
                //stringstream out;
                //out << m;
                //string a_met = "met" + out.str();
                metiers[m]->set_betas_per_pop(loadedDataMetiers.vovd1.at(m));
                metiers[m]->set_discardratio_limits(loadedDataMetiers.vovd2.at(m));
                metiers[m]->set_is_avoided_stocks(loadedDataMetiers.vovi1.at(m));

            }                     // end a_met
            dout(cout << "re-read metiers...OK" << endl);


        } // END RE-READ DATA FOR VESSEL AND METIER...




        int redispatch_the_pop = 0;
        switch (freq_redispatch_the_pop) {
            case 0:
                if ((tstep % 24) == 7) { redispatch_the_pop = 1; }
                // daily update
                break;
            case 1:
                if ((tstep % 168) == 7) { redispatch_the_pop = 1; }
                // weekly update
                break;
            case 2:
                if (binary_search(tsteps_months.begin(), tsteps_months.end(), tstep)) { redispatch_the_pop = 1; }
                // monthly update
                break;
            case 3:
                if (binary_search(tsteps_quarters.begin(), tsteps_quarters.end(), tstep)) { redispatch_the_pop = 1; }
                // quartely update
                break;
            case 4:
                if (binary_search(tsteps_semesters.begin(), tsteps_semesters.end(), tstep)) { redispatch_the_pop = 1; }
                // semester update
                break;
        }


        if (redispatch_the_pop)     // EVENT => re-read pop data
        {

            paramsForLoad.sparam1 = std::to_string(simModel->month());
            paramsForLoad.sparam2 = std::to_string(simModel->quarter());
            paramsForLoad.sparam3 = std::to_string(simModel->semester());
            paramsForLoad.iparam1 = simModel->config().nbpops;
            paramsForLoad.iparam2 = NBAGE;
            paramsForLoad.iparam3 = NBSZGROUP;
            paramsForLoad.iparam4 = SEL_NBSZGROUP;
            paramsForLoad.vdparam1 = simModel->config().calib_cpue_multiplier;
            paramsForLoad.vdparam2 = simModel->config().calib_weight_at_szgroup;

            LoadedData loadedDataPops;

            cout << "Reload population data" << endl;
            Dataloaderpops pprl;
            l->loadFeatures(&pprl,
                            indb,
                            folder_name_parameterization,
                            inputfolder,
                            dyn_pop_sce,
                            dyn_alloc_sce,
                            biolsce,
                            fleetsce,
                            paramsForLoad,
                            loadedDataPops);






            cout << "redispatch the population over its spatial extent...." << endl;

            // aggregate from nodes to set the tot_N_at_szgroup per pop
            for (unsigned int sp = 0; sp < populations.size(); sp++) {
                // aggregate from nodes (caution: do it before changing of list_nodes)
                if (!binary_search(simModel->config().implicit_pops.begin(), simModel->config().implicit_pops.end(),
                                   sp)) {

                    /*
                    if(sp==1){
                        vector <double> a_tot_N_at_szgroup_here = populations.at(sp)->get_tot_N_at_szgroup();
                        for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
                         cout << "BEFORE AGGREGATE IN MAIN: a_tot_N_at_szgroup[" << sz << "] is "<< a_tot_N_at_szgroup_here[sz]  << endl;
                    }
                    */
                    // get total N from summing up N over nodes
                    populations.at(sp)->aggregate_N();

                    /*
                    if(sp==1){
                        vector <double> a_tot_N_at_szgroup_here = populations.at(sp)->get_tot_N_at_szgroup();
                        for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
                         cout << "AFTER AGGREGATE IN MAIN: a_tot_N_at_szgroup[" << sz << "] is "<< a_tot_N_at_szgroup_here[sz]  << endl;
                    }
                    */

                }
            }
            cout << "aggregate_N over all pops....done" << endl;



            if(dyn_pop_sce.option(Options::nbcpCoupling) ) {
                string a_command_for_R;

                for (unsigned int p = 0; p < populations.size(); p++) {
                    if (binary_search(simModel->config().nbcp_coupling_pops.begin(),
                                      simModel->config().nbcp_coupling_pops.end(), p)) {
                        type_of_avai_field_to_read.at(p) = "_updated";

                        stringstream out;
                        out << p;
                        string a_pop = out.str();

                        stringstream outtstep;
                        outtstep << tstep;
                        string atstep = outtstep.str();

#if defined(_WIN32)
                        cout << "if ERR here: Did you set the environmental variables with the Rscript path and restart the compiler env?" << endl;
                        a_command_for_R = "Rscript .\\interactiverscripts\\nbcp_displace_coupling_part02.r "+a_pop+" "+atstep+" "+namefolderoutput+" "+namesimu+" "+a_graph_s;
                        cout << "executing " << a_command_for_R << endl;
                        system(a_command_for_R.c_str());
#else
                        cout << "nbcp_coupling...done" << endl;
                        // caution with HPC, annoying lower cases in file names and paths required!
                        a_command_for_R =
                                "Rscript " + inputfolder + "/interactiverscripts/nbcp_displace_coupling_part02.r " +
                                a_pop + " " + atstep + " " + namefolderoutput + " " + namesimu + " " + a_graph_s;
                        system(a_command_for_R.c_str());
#endif
                    }  // end nbcp coupling pops
                }  // end pop
            }


            if (dyn_pop_sce.option(Options::avai_shuffler_on) || dyn_pop_sce.option(Options::avai_updater_on)) {


                // alter the availability field, if required
                for (unsigned int p = 0; p < populations.size(); p++) {
                    if (!binary_search(simModel->config().implicit_pops.begin(), simModel->config().implicit_pops.end(),
                                       p)) {
                        stringstream out;
                        out << p;
                        string a_pop = out.str();

                        stringstream out2;
                        out2 << nrow_coord;
                        string a_nrow_coord = out2.str();

                        string a_command;
                        string a_command_for_R;
                        stringstream outtstep;
                        outtstep << tstep;
                        string atstep = outtstep.str();

                        // the system command line
#if defined(_WIN32)
                                                                                                                                                if(dyn_pop_sce.option(Options::avai_updater_on) && tstep>744){
                           // note that nothing is done before end of 1st month (745) to get enough catch data for an update
                           type_of_avai_field_to_read.at(p)="_updated";
                           //system("dir");
                           // caution with HPC, annoying lower cases in file names and paths required!
                           cout << "if ERR here: Did you set the environmental variables with the Rscript path and restart the compiler env?" << endl;
                           a_command_for_R = "Rscript .\\interactiverscripts\\input2avaiupdater.r "+a_pop+" "+atstep+" "+namefolderoutput+" "+namesimu+" "+a_graph_s;
                           //a_command_for_R = "R CMD BATCH .\\interactiverscripts\\input2avaiupdater.r "+a_pop+" "+atstep;
                           cout << "look after " << a_command_for_R << endl;
                           cout << "This supposes StockId " << a_pop << " is informed in displace_input_for_data_merger.csv input file" << endl;
                           system(a_command_for_R.c_str());
                            a_command = "avaifieldupdater.exe -tstep " +atstep+" -f " +namefolderinput+ " -a " +inputfolder+ " -s " + std::to_string(simModel->semester())+ " -graph " +graphnum.str()+ " -nr "+a_nrow_coord+ " -dist 15 -shepard_p 0.5";
                            cout << "look after " << a_command << endl; // right now look into the data input folder, so need to have the exe here...TODO look into the displace.exe folder instead!!
                            system(a_command.c_str());
                       }
                       if(dyn_pop_sce.option(Options::avai_shuffler_on)){
                           type_of_avai_field_to_read.at(p)="_shuffled";
                           a_command = "avaifieldshuffler.exe -f " +namefolderinput+ " -s " + std::to_string(simModel->semester())+ " -p " +a_pop;
                           cout << "look after " << a_command << endl; // right now look into the data input folder, so need to have the exe here...TODO look into the displace.exe folder instead!!
                           system(a_command.c_str());
                       }
#else
                        if (dyn_pop_sce.option(Options::avai_updater_on) && tstep > 744) {
                            type_of_avai_field_to_read.at(p) = "_updated";
                            // caution with HPC, annoying lower cases in file names and paths required!
                            a_command_for_R =
                                    "Rscript " + inputfolder + "/interactiverscripts/input2avaiupdater.r " + a_pop +
                                    " " + atstep + " " + namefolderoutput + " " + namesimu + " " + a_graph_s;
                            system(a_command_for_R.c_str());
                            a_command =
                                    inputfolder + "/avaifieldupdatertool -tstep " + atstep + " -f " + namefolderinput +
                                    " -a " + inputfolder + " -s " + modelLoader->semesterString() + " -graph " +
                                    graphnum.str() +
                                    " -nr " + a_nrow_coord + " -dist 30 -shepard_p 0.5";
                            system(a_command.c_str());
                        }
                        if (dyn_pop_sce.option(Options::avai_shuffler_on)) {
                            a_command = inputfolder + "/avaifieldshufflertool -f " + namefolderinput + " -s " +
                                        modelLoader->semesterString() + " -p " + a_pop;
                            system(a_command.c_str());
                        }
                        cout << "avaifieldshuffler...done" << endl;
#endif

                    }
                }
            }


            // then, clean up all nodes before changing of spatial avai
            // (necessary to remove any fish in now wrong locations)
            cout << "clear pops on nodes" << endl;
            for (unsigned int i = 0; i < simModel->nodes().size(); i++) {
                simModel->nodes().at(i)->clear_pop_names_on_node();
                simModel->nodes().at(i)->clear_Ns_pops_at_szgroup();
                simModel->nodes().at(i)->clear_avai_pops_at_selected_szgroup();
            }
            cout << "clear pops on nodes...done" << endl;

            // RE-read for populations
            for (unsigned int i = 0; i < populations.size(); i++) {
                stringstream out;
                out << i;

                cout << "RE-read for population " << populations.at(i)->get_name() << " from " <<
                     folder_name_parameterization << " " << inputfolder << " " << type_of_avai_field_to_read.at(i)
                     << endl;

                auto full_avai_szgroup_nodes_with_pop = loadedDataPops.vectmmapndparam1.at(i);
                populations.at(i)->set_full_spatial_availability(full_avai_szgroup_nodes_with_pop);


                // read a other landings per node for this species (DEPRECATED - DONE AT MONTH TSTEP INSTEAD)
                //map<int, double> oth_land= read_oth_land_nodes_with_pop(simModel->semester(), simModel->month(), i, folder_name_parameterization, inputfolder, fleetsce);
                //populations.at(i)->set_oth_land(oth_land);

                multimap<int, double> overall_migration_fluxes = loadedDataPops.vectmmapidparam1.at(i);
                populations.at(i)->set_overall_migration_fluxes(overall_migration_fluxes);

                // apply the overall migration loss fluxes (i.e. on the overall N at szgroup)
                if (!dyn_pop_sce.option(Options::stop_mig_35065) || tstep < 35065) {
                    populations.at(i)->apply_overall_migration_fluxes(populations);
                }

                //then, re-set the list_nodes and the pop_names_on_node
                // from the new area distribution given by this new spatial avai
                vector<Node *> list_nodes;
                for (auto iter = full_avai_szgroup_nodes_with_pop.begin();
                     iter != full_avai_szgroup_nodes_with_pop.end();
                     iter = full_avai_szgroup_nodes_with_pop.upper_bound(iter->first)) {
                    list_nodes.push_back(simModel->nodes()[iter->first.toIndex()]);
                    simModel->nodes()[iter->first.toIndex()]->set_pop_names_on_node(i);
                    //   check per node
                    //   vector <int> pop_names = nodes[ iter->first ]->get_pop_names_on_node();
                    //   cout << "Node " << iter->first << endl;
                    //   for(int p=0;p<pop_names.size();p++) cout<< pop_names.at(p) << " ";
                    //   cout << endl;
                }
                    populations.at(i)->set_list_nodes(list_nodes);

                // add the current Ns to the vectors of vectors of the concerned nodes
                vector<double> tot_N_at_szgroup = populations.at(i)->get_tot_N_at_szgroup();

                /*if( populations.at(i)->get_name()==1){
                    vector <double> a_tot_N_at_szgroup_here = populations.at(i)->get_tot_N_at_szgroup();
                    for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
                       cout << "CHECK IN MAIN: a_tot_N_at_szgroup[" << sz << "] is "<< a_tot_N_at_szgroup_here[sz]  << endl;
                */


                for (unsigned int n = 0; n < list_nodes.size(); n++) {
                    list_nodes[n]->set_Ns_pops_at_szgroup(i, tot_N_at_szgroup);
                    dout(cout << list_nodes[n]->get_idx_node().toIndex() << " ");
                }
                dout(cout << endl);

                // distribute tot_N_at_szgroup on nodes knowing the avai spatial key
                // i.e. update the vectors of vectors Ns_pops_at_szgroup of the nodes as usual
                // divide on nodes according to avai
                if (!binary_search(simModel->config().implicit_pops.begin(), simModel->config().implicit_pops.end(),
                                   i)) {
                    populations.at(i)->distribute_N();

                    //if(populations.at(i)->get_name()==1){
                    //    vector <double> a_tot_N_at_szgroup_here = populations.at(i)->get_tot_N_at_szgroup();
                    //    for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
                    //      cout << "CHECK IN MAIN2: a_tot_N_at_szgroup[" << sz << "] is "<< a_tot_N_at_szgroup_here[sz]  << endl;
                    //}

                    //...and compute the Ns on nodes at the start of this month!
                    for (unsigned int n = 0; n < simModel->nodes().size(); n++) {
                        simModel->nodes().at(n)->set_Ns_pops_at_szgroup_at_month_start(i, simModel->nodes().at(
                                n)->get_Ns_pops_at_szgroup(i));
                    }
                }


                vector<types::NodeId> nodes_with_presence = loadedDataPops.vovn1.at(i);
                multimap<types::NodeId, double> avai_szgroup_nodes_with_pop = loadedDataPops.vectmmapndparam1.at(i);

                for (unsigned int n = 0; n < nodes_with_presence.size(); n++) {
                    dout(cout << ".");
                    auto spat_avai_this_pop_this_node = find_entries(avai_szgroup_nodes_with_pop,
                                                                     nodes_with_presence.at(n));

                        vector<double> spat_avai_per_selected_szgroup;
                        vector<int> selected_szgroups = populations.at(i)->get_selected_szgroups();
                        for (int sz=0; sz<spat_avai_this_pop_this_node.size(); ++sz)
                        {
                            auto it = find(selected_szgroups.begin(), selected_szgroups.end(), sz);
                             if (it != selected_szgroups.end())
                                 spat_avai_per_selected_szgroup.push_back(spat_avai_this_pop_this_node.at(sz));
                        }
                        if (!spat_avai_per_selected_szgroup.empty()) {
                            simModel->nodes().at(
                                    nodes_with_presence.at(n).toIndex())->set_avai_pops_at_selected_szgroup(i,
                                                                                                            spat_avai_per_selected_szgroup);
                        } else {
                            // inconsistence between lst_idx_nodes and avai files if this happen...
                            outc(cout << nodes_with_presence.at(n));
                        }

                    }


            }

            dout(cout << "re-read data for this period...OK" << endl);

            // CHECK...CHECK...CHECK...
            // write done  pop number in popdyn_test
            /*for (unsigned int sp=0; sp<populations.size(); sp++)
            {
                if (!binary_search (implicit_pops.begin(), implicit_pops.end(),  sp  ) )
                {

                    dout(cout  << "write down AFTER re-read pop in the popdyn_test file for checking...");
                    // get total N from summing up N over nodes
                    populations.at(sp)->aggregate_N();
                    popdyn_test << setprecision(0) << fixed;
                    // tstep / pop / tot N at szgroup
                    popdyn_test << tstep << " " << sp << " ";
                    vector <double>tot_N_at_szgroup=populations.at(sp)->get_tot_N_at_szgroup();
                    for(unsigned int sz = 0; sz < tot_N_at_szgroup.size(); sz++)
                    {
                        // output in thousands of individuals
                        popdyn_test  << tot_N_at_szgroup.at(sz) / 1000 << " " ;
                    }
                    popdyn_test << " " <<  endl;
                }
            }
            */

        }                         // END RE-READ DATA FOR POP...



        //----------------------------------------//
        //----------------------------------------//
        // RE-READ HARBOUR INFOS------------------//
        //----------------------------------------//
        //----------------------------------------//

        if (tstep == 0 || binary_search(tsteps_quarters.begin(), tsteps_quarters.end(), tstep)) {

            // fill in the usual_fgrounds on harbours
            for (unsigned int i = 0; i < simModel->nodes().size(); ++i) {
                if (simModel->nodes().at(i)->get_is_harbour()) {
                    vector<int> vids_on_harbours = simModel->nodes().at(i)->get_vid();
                    vector<types::NodeId> fgrounds;
                    multimap<types::NodeId, double> grounds_cpues_harbour_knowledge;
                    multimap<int, types::NodeId> grounds_mets_harbour_knowledge;
                    multimap<int, double> freq_grounds_mets_harbour_knowledge;
                    vector<double> cpue_fgrounds;
                    vector<double> freq_fgrounds;

                    if (vids_on_harbours.size() > 0) {
                        dout(cout << "there are some vids on " << simModel->nodes().at(i)->get_name() << endl);
                        for (unsigned int vi = 0; vi < vids_on_harbours.size(); ++vi) {
                            auto some_grounds = vessels.at(vids_on_harbours.at(vi))->get_fgrounds();
                            auto some_cpues = vessels.at(vids_on_harbours.at(vi))->get_experiencedcpue_fgrounds();

                            const auto &poss_met = vessels.at(vids_on_harbours.at(vi))->get_possible_metiers();
                            for (unsigned int gr = 0; gr < some_grounds.size(); ++gr) {

                                // cpues
                                auto a_ground = some_grounds.at(gr);
                                double a_cpue = some_cpues.at(gr);
                                grounds_cpues_harbour_knowledge.insert(make_pair(a_ground, a_cpue));
                                fgrounds.push_back(some_grounds.at(gr));

                                // mets
                                auto metiers_on_grd = find_entries(poss_met, a_ground);
                                if (metiers_on_grd.size() != 0) {

                                    int a_met = metiers_on_grd.at(0); // take only one, the first as it comes....


                                    bool is_present = insert_if_not_present(grounds_mets_harbour_knowledge,
                                                                            make_pair(a_met, a_ground));
                                    if (is_present) {
                                        // EQUAL FREQ (TO BE CHANGED, BUT DOESNT MATTER IF A ChooseGround DTREES IN USE)
                                        freq_grounds_mets_harbour_knowledge.insert(make_pair(a_met, 1.0));
                                    }
                                } else {}
                            }
                        }

                        // DEPRECATED? (still in use for enlarging the vessel horizon if no positive profit)
                        remove_dups(fgrounds);

                        // do an average of cpues for each fgrounds
                        for (unsigned int gr = 0; gr < fgrounds.size(); ++gr) {
                            int cnt = grounds_cpues_harbour_knowledge.count(fgrounds.at(gr));
                            double sum = 0;
                            for (auto it = grounds_cpues_harbour_knowledge.equal_range(fgrounds.at(gr)).first;
                                 it != grounds_cpues_harbour_knowledge.equal_range(fgrounds.at(gr)).second; ++it) {
                                sum += (*it).second;
                            }
                            dout(cout << "average for: " << fgrounds.at(gr) << " is => " << setprecision(3) << sum / cnt
                                      << endl);
                            cpue_fgrounds.push_back(sum / cnt);
                        }
                        //  ...and scale to 1 to transform into probas
                        double cum_cpue = 0;
                        for (unsigned int gr = 0; gr < fgrounds.size(); ++gr) {
                            cum_cpue += cpue_fgrounds.at(gr);
                        }
                        if (cum_cpue != 0) {
                            for (unsigned int gr = 0; gr < fgrounds.size(); ++gr) {
                                freq_fgrounds.push_back(cpue_fgrounds.at(gr) / cum_cpue);
                            }
                        } else {
                            for (unsigned int gr = 0; gr < fgrounds.size(); ++gr) {
                                freq_fgrounds.push_back(1 / fgrounds.size());
                            }
                        }

                        // remember to make sure size() of fgrounds == size() of freq_fgrounds
                        //ASSERT(fgrounds.size()==freq_fgrounds.size())

                        // cout << "assume equal probas "  << endl;
                        // vector<double> equal_proba (fgrounds.size(), 1/fgrounds.size());
                        // for (unsigned int gr=0; gr<equal_proba.size(); ++gr)
                        //    {
                        //    freq_fgrounds.push_back(equal_proba.at(gr));
                        //    }






                    } else {
                        dout(cout << "there are NO vids on " << simModel->nodes().at(i)->get_name() << endl);
                        fgrounds.push_back(types::NodeId(
                                i)); // CAUTION, an harbour should not be a fground! just used to detect that no fground informed
                        //freq_fgrounds.push_back(0.0000001); // CAUTION, an harbour should not be a fground! just used to detect that no fground informed
                    }

                    // update the harbour
                    dout(cout << "update the harbour for grounds (node: " << simModel->nodes().at(i)->get_name()
                              << endl);
                    simModel->nodes().at(i)->set_usual_fgrounds(fgrounds);
                    simModel->nodes().at(i)->set_freq_usual_fgrounds(freq_fgrounds);

                    // TO DO:
                    simModel->nodes().at(i)->set_usual_fgrounds_per_met(grounds_mets_harbour_knowledge);
                    simModel->nodes().at(i)->set_freq_usual_fgrounds_per_met(freq_grounds_mets_harbour_knowledge);

                    // a check
                    /*
               cout << "Harbour " <<  nodes.at(i)->get_name() << " has the usual grounds: " << endl;
               vector <int> usual_grounds =  nodes.at(i)->get_usual_fgrounds();
               for (unsigned int ii=0; ii<usual_grounds.size(); ++ii)
                   {
                   cout << usual_grounds.at(ii) << " " ;
                   }
               cout << endl;
               */


                }

            }


            if (dyn_alloc_sce.option(Options::shared_harbour_knowledge)) {

                for (unsigned int v = 0; v < vessels.size(); v++) {

                    //1. draw a ground
                    auto grds = vessels.at(v)->get_fgrounds();
                    auto freq_grds = vessels.at(v)->get_freq_fgrounds();
                    // need to convert in array, see myRutils.cpp
                    if (!grds.empty()) {
                        int idx_max = max_element(freq_grds.begin(), freq_grds.end()) - freq_grds.begin();
                        auto ground = grds.at(idx_max);
                        cout << vessels.at(v)->get_name() << ": ground is " << ground << endl;


                        //2. get possible metiers on this ground
                        const auto &poss_met = vessels.at(v)->get_possible_metiers();
                        const auto &freq_poss_met = vessels.at(v)->get_freq_possible_metiers();
                        auto metiers_on_grd = find_entries(poss_met, ground);
                        auto freq_metiers_on_grd = find_entries(freq_poss_met, ground);
                        // need to convert in array, see myRutils.cpp
                        auto a_met = do_sample(1, metiers_on_grd.size(), metiers_on_grd, freq_metiers_on_grd);

                        if (metiers_on_grd.size() != 0) {
                            vessels.at(v)->set_metier(metiers[a_met.at(0)]);
                        } else {
                            vessels.at(v)->set_metier(metiers[0]);   // dangerous fix
                        }

                        auto harbs = vessels.at(v)->get_harbours();
                        auto freq_harbs = vessels.at(v)->get_freq_harbours();
                        if (freq_harbs.empty()) {
                            cout << "check why..." << endl;
                        }

                        int idx_max2 = max_element(freq_harbs.begin(), freq_harbs.end()) - freq_harbs.begin();
                        auto a_node = harbs.at(idx_max2);  // cause the decision is taken in harbour...
                        cout << vessels.at(v)->get_name() << ": "
                             << simModel->nodes().at(a_node.toIndex())->get_idx_node()
                             << " is in harb?`" <<
                             simModel->nodes().at(a_node.toIndex())->get_is_harbour()
                             << " ...cause the decision is taken in harbour..." << endl;
                        int current_metier = vessels.at(v)->get_metier()->get_name();
                        cout << vessels.at(v)->get_name() << ": current_metier is " << current_metier << endl;
                        int nbpops = simModel->nodes().at(a_node.toIndex())->get_nbpops();
                        // TO DO:
                        auto grounds_from_harbours = simModel->nodes().at(
                                a_node.toIndex())->get_usual_fgrounds_this_met(
                                current_metier);
                        auto freq_grounds_from_harbours = simModel->nodes().at(
                                a_node.toIndex())->get_freq_usual_fgrounds_this_met(
                                current_metier);
                        //vector <int>            grounds_from_harbours        = nodes.at(a_node)->get_usual_fgrounds();
                        //vector <double>         freq_grounds_from_harbours   = nodes.at(a_node)->get_freq_usual_fgrounds();
                        if (grounds_from_harbours.size() ==
                            1) {   // few cases for which the harbour has been badly informed...
                            grounds_from_harbours = vessels.at(v)->get_fgrounds();
                            freq_grounds_from_harbours = vessels.at(v)->get_freq_fgrounds();
                        }
                        vector<vector<double> > experiencedcpue_fgrounds_per_pop(grounds_from_harbours.size(),
                                                                                 vector<double>(
                                                                                         simModel->config().nbpops));
                        vector<double> experiencedcpue_fgrounds(grounds_from_harbours.size());
                        vector<vector<double> > freq_experiencedcpue_fgrounds_per_pop(grounds_from_harbours.size(),
                                                                                      vector<double>(
                                                                                              simModel->config().nbpops));
                        vector<double> freq_experiencedcpue_fgrounds(grounds_from_harbours.size());
                        multimap<types::NodeId, int> possible_metiers_from_harbours;     // = nodes.at(a_node)->get_usual_metiers();
                        multimap<types::NodeId, double> freq_possible_metiers_from_harbours; //= nodes.at(a_node)->get_freq_usual_metiers();
                        for (unsigned int gr = 0; gr <
                                                  grounds_from_harbours.size(); ++gr) { // rebuild assuming deploying one metier spread over the entire range from this harbour...
                            possible_metiers_from_harbours.insert(
                                    std::make_pair(grounds_from_harbours.at(gr), current_metier));
                            freq_possible_metiers_from_harbours.insert(
                                    std::make_pair(grounds_from_harbours.at(gr), 1.0));
                        }

                        cout << vessels.at(v)->get_name() << ": grounds_from_harbours is " << endl;
                        for (unsigned int a_gr = 0; a_gr < grounds_from_harbours.size(); ++a_gr) {
                            cout << grounds_from_harbours.at(a_gr) << " ";
                        }
                        cout << endl;

                        // 4- create randomized cpues on grounds known from the harbours for
                        // experiencedcpue_fgrounds
                        // and experiencedcpue_fgrounds_per_pop
                        // NO OTHER CHOICE FOR NOW BECAUSE NO SHARED KNOWLEDGE ON THIS ASPECT
                        vector<double> cum_cpue_over_pop(grounds_from_harbours.size());
                        double cum_cpue = 0;
                        for (unsigned int a_node = 0; a_node < experiencedcpue_fgrounds_per_pop.size(); a_node++) {

                            double a_number = freq_grounds_from_harbours.at(
                                    a_node); // because in this case freq from harbours is actually a proxy for cpue (see grounds_cpues_harbour_knowledge)
                            experiencedcpue_fgrounds.at(a_node) = a_number;
                            cum_cpue += experiencedcpue_fgrounds.at(a_node);


                            cum_cpue_over_pop.push_back(0);

                            for (unsigned int pop = 0; pop < experiencedcpue_fgrounds_per_pop[a_node].size(); pop++) {
                                experiencedcpue_fgrounds_per_pop.at(a_node).at(
                                        pop) = a_number; // assuming the node effect on cpue larger than the pop effect to make it simple here

                                // cumul to scale to 1 (just below)
                                cum_cpue_over_pop.at(a_node) += experiencedcpue_fgrounds_per_pop.at(a_node).at(pop);

                                //  scale to 1 for use in do_sample() => freq_experiencedcpue_fgrounds_per_pop
                                if (cum_cpue_over_pop.at(a_node) != 0) {
                                    for (unsigned int pop = 0;
                                         pop < experiencedcpue_fgrounds_per_pop[a_node].size(); pop++) {
                                        freq_experiencedcpue_fgrounds_per_pop.at(a_node).at(pop) =
                                                experiencedcpue_fgrounds_per_pop.at(a_node).at(pop) /
                                                cum_cpue_over_pop.at(a_node);
                                    }
                                }

                            }

                        }

                        //  scale to 1 for use in do_sample() => freq_experiencedcpue_fgrounds
                        if (cum_cpue != 0) {
                            for (unsigned int a_node = 0; a_node < experiencedcpue_fgrounds.size(); a_node++) {
                                freq_experiencedcpue_fgrounds.at(a_node) =
                                        experiencedcpue_fgrounds.at(a_node) / cum_cpue;
                            }
                        }



                        // 5- then apply the changes to the vessel
                        vessels.at(v)->set_spe_fgrounds(grounds_from_harbours); // CHANGED
                        vessels.at(v)->set_spe_freq_fgrounds(freq_grounds_from_harbours); // CHANGED
                        vessels.at(v)->set_experienced_bycatch_prop_on_fgrounds(
                                freq_grounds_from_harbours);// re-dimensioned
                        vessels.at(v)->set_experienced_avoided_stks_bycatch_prop_on_fgrounds(
                                freq_grounds_from_harbours);// re-dimensioned
                        vessels.at(v)->set_cumcatch_fgrounds(experiencedcpue_fgrounds);// re-dimensioned
                        vessels.at(v)->set_cumcatch_fgrounds_per_pop(experiencedcpue_fgrounds_per_pop);// re-dimensioned
                        vessels.at(v)->set_cumdiscard_fgrounds(experiencedcpue_fgrounds);// re-dimensioned
                        //vessels.at(v)->set_cumdiscard_fgrounds_per_pop(experiencedcpue_fgrounds_per_pop);// re-dimensioned
                        vessels.at(v)->set_cumeffort_fgrounds(freq_grounds_from_harbours);// re-dimensioned
                        vessels.at(v)->set_experiencedcpue_fgrounds(experiencedcpue_fgrounds); // re-dimensioned
                        vessels.at(v)->set_experiencedcpue_fgrounds_per_pop(
                                experiencedcpue_fgrounds_per_pop); // re-dimensioned
                        vessels.at(v)->set_freq_experiencedcpue_fgrounds(
                                freq_experiencedcpue_fgrounds); // re-dimensioned
                        vessels.at(v)->set_freq_experiencedcpue_fgrounds_per_pop(
                                freq_experiencedcpue_fgrounds_per_pop); // re-dimensioned
                        vessels.at(v)->set_spe_possible_metiers(possible_metiers_from_harbours); // CREATED
                        vessels.at(v)->set_spe_freq_possible_metiers(freq_possible_metiers_from_harbours); // CREATED

                        // inform grounds in closed areas
                        auto new_grds = vessels.at(v)->get_fgrounds();
                        vector<types::NodeId> fgrounds_in_closed_areas;
                        for (unsigned int i = 0; i < new_grds.size(); ++i) {
                            if (simModel->nodes().at(new_grds.at(i).toIndex())->evaluateAreaType() == 1) {
                                fgrounds_in_closed_areas.push_back(new_grds.at(i));
                            }
                        }
                        vessels.at(v)->set_fgrounds_in_closed_areas(fgrounds_in_closed_areas);
                    } else {
                        // otherwise, no activity this vessel this quarter
                        cout << " --> no activity for " << vessels.at(v)->get_name() << endl;


                    }
                } // end v
            } // end sce











        } // end re-read harbour






        // UPDATE THE TARIFF MAP & RE-READ FISHING_CREDITS PER VESSEL
        if (dyn_alloc_sce.option(Options::fishing_credits)) {

            // 1- annual tariff HCR (currently pooling all tariff pops together)
            if (binary_search(tsteps_years.begin(), tsteps_years.end(), tstep)) {
                cout << "Annual tariff HCR... " << endl;
                double fbar_py_allpopav = 0.0;
                double ftarget_allpopav = 0.0;
                double change_per_year = tariff_annual_hcr_percent_change / 100;
                cout << "...change_per_year is " << change_per_year << endl;
                for (unsigned int ipop = 0; ipop < tariff_pop.size(); ++ipop) {
                    vector<double> fbar_ages_min_max = populations.at(tariff_pop.at(ipop))->get_fbar_ages_min_max();
                    double ftarget = fbar_ages_min_max.at(2);
                    cout << "...the ftarget at y-1 for this pop is " << ftarget << endl;
                    ftarget_allpopav += ftarget; // cumul...
                    cout << "...get fbar to help deciding in the annual tariff HCR...for pop" << tariff_pop.at(ipop)
                         << endl;
                    double fbar_py = populations.at(tariff_pop.at(ipop))->get_fbar();
                    fbar_py_allpopav += fbar_py; // cumul...
                    cout << "...the fbar at y-1 for this pop is " << fbar_py << endl;
                }
                ftarget_allpopav = ftarget_allpopav / tariff_pop.size(); // ...then average
                fbar_py_allpopav = fbar_py_allpopav / tariff_pop.size(); // ...then average

                cout << "...decide on the fmultiplier " << endl;
                double fmultiplier = 1.0;
                if (fbar_py_allpopav > ftarget_allpopav) {
                    // harvest rate is too high, we need more restrictive categories
                    fmultiplier = 1.0 - change_per_year; // or (Fpercent/100); ?
                } else {   // harvest rate is too low, we need less restrictive categories (e.g. higher lpue allowed for the same tariff)
                    fmultiplier = 1.0 + change_per_year; // or  (Fpercent/100); ?
                }
                cout << "The fmultiplier for the annual tariff HCR is then " << fmultiplier <<
                     " given the target F " << ftarget_allpopav << "  and the assessed F averaged over tariff pops "
                     << fbar_py_allpopav << endl;
                for (unsigned int icl = 0; icl < arbitary_breaks_for_tariff.size(); ++icl) {
                    arbitary_breaks_for_tariff.at(icl) * fmultiplier;
                }

                // 2 - Re-init vessel total credits
                tout(cout << "Re-init vessel total credits..." << endl);
                for (unsigned int v = 0; v < vessels.size(); v++) {
                    dout(cout << "RE-READ in fishing credits for this vessel " << vessels.at(v)->get_name() << endl);
                    vector<double> spe_fishing_credits = find_entries_s_d(reloaded_fcredits, vessels.at(v)->get_name());
                    for (int icr = 0; icr < spe_fishing_credits.size(); ++icr) {
                        spe_fishing_credits.at(icr) = spe_fishing_credits.at(icr) * total_amount_credited;
                    }

                    // complete to 3 values for tariff per node because we expect tariff all, tariff pop, and tariff benthos
                    while (spe_fishing_credits.size() <= 3) { spe_fishing_credits.push_back(0); }

                    vessels.at(v)->set_fishing_credits(spe_fishing_credits);
                }
            }



            // timing (update at 7 a.m.)
            int do_update = 0;
            switch (freq_update_tariff_code) {
                case 0:
                    if ((tstep % 24) == 7) { do_update = 1; }
                    // daily update
                    break;
                case 1:
                    if ((tstep % 168) == 7) { do_update = 1; }
                    // weekly update
                    break;
                case 2:
                    if (binary_search(tsteps_months.begin(), tsteps_months.end(), tstep)) { do_update = 1; }
                    // monthly update
                    break;
            }

            if (do_update) {
                cout << " Update the tariff map....at " << tstep << endl;

                // obtain the list of idx relevant nodes for these tariff pops
                vector<types::NodeId> list_nodes_idx;
                for (unsigned int ipop = 0; ipop < tariff_pop.size(); ++ipop) {
                    dout(cout << "Get the list of nodes for the tariff pop "
                              << populations.at(tariff_pop.at(ipop))->get_name() << endl);
                    vector<Node *> a_list_nodes = populations.at(tariff_pop.at(ipop))->get_list_nodes();
                    for (unsigned int inode = 0; inode < a_list_nodes.size(); ++inode) {
                        list_nodes_idx.push_back(a_list_nodes.at(inode)->get_idx_node());
                    }
                }

                // sort and unique
                remove_dups(list_nodes_idx);

                // check
                //cout << "nodes for the lpue computation are:" << endl;
                //for(int i=0; i<list_nodes_idx.size();++i)
                //{
                //    cout << list_nodes_idx.at(i) << " ";
                //}
                //cout << endl;


                // loop over to find out the mean pue
                double cumcatches = 0, cumdiscards = 0, cumeffort = 0, mean_pue;
                for (unsigned int inode = 0; inode < list_nodes_idx.size(); ++inode) {
                    if (update_tariffs_based_on_lpue_or_dpue_code == 1) {
                        dout(cout << "Updating tariffs based on lpue" << endl);
                        for (unsigned int ipop = 0; ipop < tariff_pop.size(); ++ipop) {
                            cumcatches += simModel->nodes()[list_nodes_idx.at(
                                    inode).toIndex()]->get_cumcatches_per_pop().at(ipop);
                        }
                    }
                    if (update_tariffs_based_on_lpue_or_dpue_code == 2) {
                        dout(cout << "Updating tariffs based on dpue" << endl);
                        for (unsigned int ipop = 0; ipop < tariff_pop.size(); ++ipop) {
                            cumdiscards += simModel->nodes()[list_nodes_idx.at(
                                    inode).toIndex()]->get_cumdiscards_per_pop().at(
                                    ipop);
                        }
                    }

                    cumeffort += simModel->nodes()[list_nodes_idx.at(inode).toIndex()]->get_cumftime();
                }
                //cout << " cumcatches of reference for the update is.... " << cumcatches << endl;
                //cout << " cumeffort of reference for the update is.... " << cumeffort << endl;
                if (cumeffort != 0) {
                    if (update_tariffs_based_on_lpue_or_dpue_code == 1) { mean_pue = cumcatches / cumeffort; }
                    if (update_tariffs_based_on_lpue_or_dpue_code == 2) { mean_pue = cumdiscards / cumeffort; }
                    dout(cout << " mean_pue of reference for the update is.... " << mean_pue << endl);


                    // loop over to scale the tariff (on each node) up or down (caution: by one category)
                    double tariff_this_node, node_pue, nb_times_diff, effort_on_this_node;
                    for (unsigned int inode = 0; inode < list_nodes_idx.size(); ++inode) {
                        tariff_this_node = simModel->nodes()[list_nodes_idx.at(inode).toIndex()]->get_tariffs().at(0);

                        effort_on_this_node = simModel->nodes()[list_nodes_idx.at(inode).toIndex()]->get_cumftime();
                        double cumcatches_this_node = 0;
                        if (update_tariffs_based_on_lpue_or_dpue_code == 1) {
                            for (unsigned int ipop = 0; ipop < tariff_pop.size(); ++ipop) {
                                cumcatches_this_node += simModel->nodes()[list_nodes_idx.at(
                                        inode).toIndex()]->get_cumcatches_per_pop().at(ipop);
                            }
                        }
                        double cumdiscards_this_node = 0;
                        if (update_tariffs_based_on_lpue_or_dpue_code == 2) {
                            for (unsigned int ipop = 0; ipop < tariff_pop.size(); ++ipop) {
                                cumdiscards_this_node += simModel->nodes()[list_nodes_idx.at(
                                        inode).toIndex()]->get_cumdiscards_per_pop().at(ipop);
                            }
                        }
                        if (update_tariffs_based_on_lpue_or_dpue_code == 1) {
                            node_pue = cumcatches_this_node / effort_on_this_node;
                            nb_times_diff = node_pue / mean_pue;
                        }

                        if (update_tariffs_based_on_lpue_or_dpue_code == 2) {
                            node_pue = cumdiscards_this_node / effort_on_this_node;
                            nb_times_diff = node_pue / mean_pue;
                        }

                        //cout << "nb_times_diff on the node" << nodes[list_nodes_idx.at(inode)]->get_idx_node() << " is .... " << nb_times_diff << endl;




                        // find out which category the tariff should be
                        unsigned int count1 = 0;
                        while (nb_times_diff > arbitary_breaks_for_tariff.at(count1)) {
                            if ((count1) >= arbitary_breaks_for_tariff.size() - 1) { break; }
                            count1 = count1 + 1;
                        }

                        // constraint +/-1 category
                        double updated_tariff;
                        //cout << "...tariff_this_node is "  << tariff_this_node << endl;
                        unsigned int count2 = 0;
                        while (tariff_this_node > arbitary_breaks_for_tariff.at(count2)) {
                            if ((count2) >= arbitary_breaks_for_tariff.size() - 1) { break; }
                            count2 = count2 + 1;
                        }

                        // if landing/effort or discard/effort increases we want to increase the tariff on this areas
                        if (count1 > count2 && ((count2 + 1) < arbitary_breaks_for_tariff.size())) {
                            updated_tariff = arbitary_breaks_for_tariff.at(count2 + 1);
                        }
                        if (count1 < count2) { updated_tariff = arbitary_breaks_for_tariff.at(count2 - 1); }
                        if (count1 == count2) { updated_tariff = arbitary_breaks_for_tariff.at(count2); }

                        // update the tariff (unless the effort on this node is 0)
                        if (effort_on_this_node != 0) {
                            simModel->nodes()[list_nodes_idx.at(inode).toIndex()]->set_tariffs(0, updated_tariff);
                        }
                        //cout << "...then set tariff on " << nodes[list_nodes_idx.at(inode)]->get_idx_node() << " as .... " <<  updated_tariff << endl;

                    }

                }
            }

        }


        dout(cout << "THE WINDFARM LOOP----------" << endl);
        for (unsigned int i = 0; i < windmills.size(); i++) {

            if (windmills.at(i)->get_is_active() == 1) {
                windmills.at(i)->compute_kWproduction_in_farm(); // discrete event
                //cout << "kW production in farm " << i << " is " << windmills.at(i)->get_kWproduction_in_farm() << endl;
                if (export_hugefiles) {
                    windmills.at(i)->export_windmills_indicators(windmillslogs, tstep); // export event to file...
                }

                if (enable_sqlite_out) {
                    outSqlite->exportWindmillsLog(windmills.at(i), tstep);
                }
            }

        }

        // Flush and updates all statistics for fsihfarms
        if (use_gui) {

            if (binary_search(tsteps_months.begin(), tsteps_months.end(), tstep)) {
                windmillslogs.flush();
                guiSendUpdateCommand(windmillslogs_filename, tstep);
            }
        }


        dout(cout << "THE SHIP LOOP----------" << endl);
        for (unsigned int i = 0; i < ships.size(); i++) {
            if (ships.at(i)->get_is_active() == 1) {
                ships.at(i)->compute_emissions_in_ship(); // discrete event
                //cout << "Emission in ships " << i << " is " << ships.at(i)->get_NOxEmission() << endl;
                if (export_hugefiles) {
                    ships.at(i)->export_ships_indicators(shipslogs, tstep); // export event to file...
                }
                if (enable_sqlite_out) {
                    outSqlite->exportShip(tstep, ships.at(i));
                }
            }

        }

        // Flush and updates all statistics for ships
        if (use_gui) {

            if (binary_search(tsteps_months.begin(), tsteps_months.end(), tstep)) {
                shipslogs.flush();
                guiSendUpdateCommand(shipslogs_filename, tstep);
            }
        }


        dout(cout << "THE FISHFARM LOOP----------" << endl);
        for (unsigned int i = 0; i < fishfarms.size(); i++) {

            if (fishfarms.at(i)->get_is_active() == 1) {
                int start = fishfarms.at(i)->get_start_day_growing();
                int end = fishfarms.at(i)->get_end_day_harvest();
                if ((int) ((tstep + 1) / 24 - (8762.0 / 24 * (a_year - 1))) == end &&
                    fishfarms.at(i)->get_is_running() == 1) {
                    fishfarms.at(i)->compute_profit_in_farm(); // discrete event
                    //cout << "profit in farm " << i << " is " << fishfarms.at(i)->get_sim_annual_profit() << endl;
                    fishfarms.at(i)->export_fishfarms_indicators(fishfarmslogs, tstep); // export event to file...

                    if (enable_sqlite_out) {
                        outSqlite->exportFishfarmLog(fishfarms.at(i), tstep);
                    }

                    //...and reset
                    fishfarms.at(i)->set_is_running(0);
                    fishfarms.at(i)->set_sim_individual_mean_kg(0.0);
                    fishfarms.at(i)->set_sim_kg_harvested(0.0);
                    fishfarms.at(i)->set_sim_kg_eggs_harvested(0.0);
                    fishfarms.at(i)->set_sim_net_discharge_N(0.0);
                    fishfarms.at(i)->set_sim_net_discharge_P(0.0);
                    fishfarms.at(i)->set_sim_annual_profit(0.0);
                } else {
                    if (fishfarms.at(i)->get_is_running() == 1 &&
                        (int) ((tstep + 1) / 24 - (8762.0 / 24 * (a_year - 1))) > start) {
                        // fish growth...
                        fishfarms.at(i)->compute_current_sim_individual_mean_kg_in_farm(tstep, a_year);

                        //...environmental impact
                        fishfarms.at(i)->compute_discharge_on_farm(tstep);
                        //cout << "discharge N from farm " << i << " is " << fishfarms.at(i)->get_sim_net_discharge_N() << "kg" << endl;
                        //cout << "discharge P from farm " << i << " is " << fishfarms.at(i)->get_sim_net_discharge_P() << "kg" << endl;

                        if (binary_search(tsteps_months.begin(), tsteps_months.end(), tstep)) {

                            if (enable_sqlite_out) {
                                outSqlite->exportFishfarmLog(fishfarms.at(i), tstep);
                            }

                            if (export_hugefiles) {
                                fishfarms.at(i)->export_fishfarms_indicators(fishfarmslogs,
                                                                             tstep); // export event to file
                            }
                        }
                    }
                }
            }

        }

        // Flush and updates all statistics for fishfarms
        if (use_gui) {

            if (binary_search(tsteps_months.begin(), tsteps_months.end(), tstep)) {
                fishfarmslogs.flush();
                guiSendUpdateCommand(fishfarmslogs_filename, tstep);
            }
        }


        dout(cout << "THE VESSEL LOOP----------" << endl);
        // get a random order for acting vessels
        // random permutation

        lock();
        random_shuffle(ve.begin(), ve.end());
        unlock();

        // check that departure is done from harbours that are
        // informed as such in graph_coord_harbour...if not then check spe_harbour
        if (tstep == 0) {
            for (unsigned int i = 0; i < ve.size(); i++) {
                bool is_harbour = vessels[i]->get_loc()->get_is_harbour();
                cout << vessels[i]->get_name() << " departure from an harbour? " << is_harbour
                     << " idx node: " << vessels[i]->get_loc()->get_idx_node() << endl;


                //cout << "tstep: "<< tstep << "export loglike for " << listVesselIdForLogLikeToExport.at(idx)<< endl;
                OutputExporter::instance().exportVmsLike(tstep, vessels[i]); // at tstep=0


            }

        }

        ///------------------------------///
        ///------------------------------///
        ///  THE EFFORT CONTROL          ///
        /// (ON GoFishing dtree)         ///
        ///------------------------------///
        ///------------------------------///

        if (dyn_alloc_sce.option(Options::EffortMinControl)) {
            if (binary_search(tsteps_years.begin(), tsteps_years.end(), tstep)) {
                if (nb_y_left_to_tgrt_year > 1) {
                    nb_y_left_to_tgrt_year = nb_y_left_to_tgrt_year -
                                             1;
                } // target year for reaching the FMSY. HARDCODED FOR NOW

                // will alter GoFishing dtree final leaf proba
                if (!computeEffortMultiplier(populations,
                                             vessels,
                                             nb_y_left_to_tgrt_year,
                                             1)) {
                                                 throw std::runtime_error("Error while executing: computeEffortMultiplier");
                }
            }
        }

        if (dyn_alloc_sce.option(Options::EffortMaxControl)) {
            if (binary_search(tsteps_years.begin(), tsteps_years.end(), tstep)) {
                if (nb_y_left_to_tgrt_year > 1) {
                    nb_y_left_to_tgrt_year = nb_y_left_to_tgrt_year -
                                             1;
                } // target year for reaching the FMSY. HARDCODED FOR NOW

                // will alter GoFishing dtree final leaf proba
                if (!computeEffortMultiplier(populations,
                                             vessels,
                                             nb_y_left_to_tgrt_year,
                                             2)) {
                                                 throw std::runtime_error("Error while executing: computeEffortMultiplier");
                }
            }
        }


        ///------------------------------///
        ///------------------------------///
        ///  THE FOR-LOOP OVER VESSELS   ///
        ///------------------------------///
        ///------------------------------///
        // LOOP OVER VESSELS
#ifdef PROFILE
        mVesselLoopProfile.start();
#endif

        thread_vessel_prepare();
        for (unsigned int idx_v = 0; idx_v < ve.size(); idx_v++) {
            int index_v = ve[idx_v];
            thread_vessel_insert_job(index_v);
        }

        for (unsigned int s = 0; s < ships.size(); s++) {
            thread_vessel_insert_job(5000 + s);
        }


        thread_vessel_wait_completed();

#ifdef PROFILE
        mVesselLoopProfile.elapsed_ms();
#endif

        ///------------------------------///
        ///------------------------------///
        ///  THE QUOTA UPTAKES & CHOKING ///
        ///------------------------------///
        ///------------------------------///

        if (dyn_alloc_sce.option(Options::TACs)) {

            for (unsigned int pop = 0; pop < populations.size(); pop++) {

                outc(cout << "...pop " << pop << endl;)
                if (!binary_search(simModel->config().implicit_pops.begin(), simModel->config().implicit_pops.end(),
                                   pop)) {

                    double so_far = populations.at(pop)->get_landings_so_far();
                    global_quotas_uptake.at(pop) =
                            (so_far / 1000) / (populations.at(pop)->get_tac()->get_current_tac());

                    populations.at(pop)->set_quota_uptake(global_quotas_uptake.at(pop));
                    populations.at(pop)->set_quota(populations.at(pop)->get_tac()->get_current_tac());

                    if (enable_sqlite_out) {
                        outSqlite->exportPopQuotas(populations.at(pop), pop, tstep);
                    }

                    //cout <<"pop "<< pop << ": global_quotas_uptake is " << global_quotas_uptake.at(pop) << endl;

                    // export in file
                    quotasuptake << setprecision(6) << fixed;
                    quotasuptake << tstep << " " << pop << " " <<
                                 global_quotas_uptake.at(pop) << " " <<
                                 populations.at(pop)->get_tac()->get_current_tac() << endl;



                    // if more than x% of vessels choked then declare this stock as choking fisheries
                    int nbchoked = 0;
                    for (unsigned int v = 0; v < vessels.size(); v++) {
                        nbchoked += vessels.at(v)->get_is_choked().at(pop);
                    }
                    // HARDCODED threshold...
                    if (nbchoked >= ceil(0.3 * vessels.size())) { populations.at(pop)->set_is_choking_fisheries(1); }

                }
            }
        }



        ///------------------------------///
        ///------------------------------///
        ///  THE DIFFUSIVE ENVT          ///
        ///------------------------------///
        ///------------------------------///
        if (dyn_pop_sce.option(Options::diffuseNutrients)) {
            int numStepDiffusions = 100; // e.g. diffuse every 100 tsteps
            if ((tstep % numStepDiffusions) == (numStepDiffusions - 1)) {

                // naive diffusion
                double coeff_diffusion = 0.4;
                //bool r= diffuse_Nitrogen_in_every_directions(nodes, adjacency_map, coeff_diffusion);

                // gradient diffusion
                // using the rtree
                bool r = diffuse_Nitrogen_with_gradients(simModel->nodes(), adjacency_map, rtree, coeff_diffusion);
                // bool r=  diffuse_Phosphorus_with_gradients(simModel->nodes(), adjacency_map, rtree, coeff_diffusion);
                // bool r=  diffuse_Oxygen_with_gradients(simModel->nodes(), adjacency_map, rtree, coeff_diffusion);
                // bool r=  diffuse_Dissolvedcarbon_with_gradients(simModel->nodes(), adjacency_map, rtree, coeff_diffusion);



                if (enable_sqlite_out) {
                    for (unsigned int n = 0; n < simModel->nodes().size(); n++) {
                        outSqlite->exportEnvtNodes(tstep, simModel->nodes().at(n));
                    }
                }


            }
        }

        // Flush and updates all statistics for simModel->nodes() envt
        if (use_gui) {
            if (binary_search(tsteps_months.begin(), tsteps_months.end(), tstep)) {
                nodes_envt.flush();
                for (unsigned int n = 0; n < simModel->nodes().size(); n++) {
                    simModel->nodes().at(n)->export_nodes_envt(nodes_envt, tstep);
                }
                guiSendUpdateCommand(nodes_envt_filename, tstep);
            }
        }

        ///------------------------------///
        ///------------------------------///
        ///  THE DIFFUSIVE BENTHOS       ///
        ///------------------------------///
        ///------------------------------///
        if (dyn_pop_sce.option(Options::diffuseBenthos)) {
            int numStepDiffusions = 100; // e.g. diffuse every 100 tsteps
            if ((tstep % numStepDiffusions) == (numStepDiffusions - 1)) {

                // naive diffusion
                double coeff_diffusion = 0.01;
                bool r = diffuse_Benthos_in_every_directions(simModel->nodes(), adjacency_map, coeff_diffusion);

                // gradient diffusion
                // using the rtree
                // bool r=  diffuse_Benthos_with_gradients(simModel->nodes(), adjacency_map, rtree, coeff_diffusion);





            }
        }




        ///------------------------------///
        ///------------------------------///
        ///  SHIPPING DISTURBING BENTHOS ///
        ///------------------------------///
        ///------------------------------///

        if (dyn_pop_sce.option(Options::modelShippingOnBenthos)) {
            if (binary_search(tsteps_months.begin(), tsteps_months.end(), tstep)) {
                double shippingdensity = 0;
                double bathymetry = 0;
                for (unsigned int i = 0; i < simModel->nodes().size(); i++) {
                    shippingdensity = simModel->nodes().at(i)->get_shippingdensity();
                    if (dyn_alloc_sce.option(Options::halfShippingDensity)) { shippingdensity = shippingdensity / 2; }
                    if (shippingdensity > 0) {
                        bathymetry = simModel->nodes().at(i)->get_bathymetry();
                        for (unsigned int funcid = 0;
                             funcid < simModel->nodes().at(i)->get_benthos_tot_biomass().size(); funcid++) {
                            double scaling = 1000;
                            double loss_after_1_month_shipping_here = -1;
                            if (bathymetry != 0) {
                                loss_after_1_month_shipping_here =
                                        (shippingdensity / 12) / scaling * 1 / abs(bathymetry);
                            }
                            //look at scaling range: 1-exp( (6000/12/(100:1000)) * 1/abs(-10)) // e.g at 10 m deep
                            // => just hypothetical for now...i.e. approx. 5% loss a month for max shippingdensity if 10 meter deep
                            double decrease_factor_on_benthos_funcgroup = 0;

                            if (dyn_pop_sce.option(Options::modelBenthosInN)) {
                                decrease_factor_on_benthos_funcgroup = 1 - exp(loss_after_1_month_shipping_here);
                                double current_nb = simModel->nodes().at(i)->get_benthos_tot_number(funcid);
                                double next_nb = current_nb * (1 + decrease_factor_on_benthos_funcgroup);
                                simModel->nodes().at(i)->set_benthos_tot_number(funcid, next_nb); // update
                            } else { // impact on biomass instead...
                                decrease_factor_on_benthos_funcgroup = 1 - exp(loss_after_1_month_shipping_here);
                                double current_bio = simModel->nodes().at(i)->get_benthos_tot_biomass(funcid);
                                double next_bio = current_bio * (1 + decrease_factor_on_benthos_funcgroup);
                                simModel->nodes().at(i)->set_benthos_tot_biomass(funcid, next_bio); // update
                            }

                        } // end funcid


                    }
                }
            }
        }



        ///------------------------------///
        ///------------------------------///
        ///  HOURLY BENTHOS RECOVERY     ///
        ///------------------------------///
        ///------------------------------///
        // van Denderen et al. 2019
        if (dyn_pop_sce.option(Options::modelBenthosInLongevity)) {

            for (unsigned int b = 0; b < benthoss.size(); b++) {
                benthoss.at(b)->recover_benthos_tot_biomass_per_funcgroup(1);
            }
        }


        ///------------------------------///
        ///------------------------------///
        ///  REINIT SOME VALUES ON NODES ///
        ///------------------------------///
        ///------------------------------///

        if (binary_search(tsteps_years.begin(), tsteps_years.end(), tstep)) {
            for (unsigned int i = 0; i < simModel->nodes().size(); i++) {
                simModel->nodes().at(i)->set_nbchoked(0);
            }
        }


        ///------------------------------///
        ///------------------------------///
        ///  EXPORTING TO DB             ///
        ///------------------------------///
        ///------------------------------///
        // export
        {
            std::unique_lock<std::mutex> m(listVesselMutex);
            if (export_hugefiles) {
                for (unsigned int idx = 0; idx < listVesselIdForVmsLikeToExport.size(); idx++) {
                    //cout << "tstep: "<< tstep << "export vmslike for " << listVesselIdForVmsLikeToExport.at(idx)<< endl;

                    OutputExporter::instance().exportVmsLike(tstep, vessels[listVesselIdForVmsLikeToExport.at(idx)]);
                }
            }
            listVesselIdForVmsLikeToExport.clear();

            for (unsigned int idx = 0; idx < listVesselIdForVmsLikeFPingsOnlyToExport.size(); idx++) {
                if (LastMonth < CurrentMonth) {
                    if (enable_sqlite_out) {
                        outSqlite->getVesselVmsLikeFPingsOnlyTable()->deleteAllVesselsBeforeMonth(CurrentMonth);
                    }
                    LastMonth = CurrentMonth;
                }

                OutputExporter::instance().exportVmsLikeFPingsOnly(tstep,
                                                                   vessels[listVesselIdForVmsLikeFPingsOnlyToExport.at(
                                                                           idx)], populations,
                                                                   simModel->config().implicit_pops);
                vessels[listVesselIdForVmsLikeFPingsOnlyToExport.at(idx)]->clear_ping_catch_pop_at_szgroup();
            }
            listVesselIdForVmsLikeFPingsOnlyToExport.clear();


            for (unsigned int idx = 0; idx < listVesselIdForLogLikeToExport.size(); idx++) {
                //cout << "tstep: "<< tstep << "export loglike for " << listVesselIdForLogLikeToExport.at(idx)<< endl;
                OutputExporter::instance().exportLogLike(tstep, vessels[listVesselIdForLogLikeToExport.at(idx)],
                                                         populations, simModel->config().implicit_pops);
                vessels[listVesselIdForLogLikeToExport.at(idx)]->reinit_after_a_trip();
            }
            listVesselIdForLogLikeToExport.clear();

            for (unsigned int idx = 0; idx < listVesselIdForTripCatchPopPerSzgroupExport.size(); idx++) {
                OutputExporter::instance().exportTripCatchPopPerSzgroup(tstep,
                                                                        vessels[listVesselIdForTripCatchPopPerSzgroupExport.at(
                                                                                idx)], populations,
                                                                        simModel->config().implicit_pops);
            }
            listVesselIdForTripCatchPopPerSzgroupExport.clear();

        }


        // EXPORT: vessel_loglike - disabled
        /*
        if (use_gui) {
            loglike.flush();
            guiSendUpdateCommand(loglike_filename, tstep);
        }*/

        // move the ships along the ship lanes
        //for(unsigned int s=0; s<ships.size(); s++)
        //{
        //      ships[ s ]->move();
        //}

#ifdef _WIN32
                                                                                                                                if(use_gnuplot)
        {
            // export ships current coordinates for plotting purpose
            for(int s=0; s<ships.size(); s++)
            {
                vmslike3   << ships[ s ]->get_x() << " "
                           << ships[ s ]->get_y() <<  endl;
            }
            vmslike3.close();

            vmslike2.close();
            fflush( pipe2 );
            fprintf(pipe2, "set terminal windows 0 size 400,400 position 100,100\n");

            Sleep( 50 );		 // used when sometimes the simulation is too quick to be captured by gnuplot
            // note that possible warning messages from gnuplot are harmless...these messages are just
            // related to the fact that gnuplot actually try to open the vmslike2.dat while this is too late...
            string command1 = "plot 'map.dat' with lines lt 3 , '"+outdir+"/DISPLACE_outputs/"+namefolderinput+"/"+namefolderoutput+"/vmslike2_"+namesimu+".dat' using 1:2,  '"+outdir+"/DISPLACE_outputs/"+namefolderinput+"/"+namefolderoutput+"/vmslike3_"+namesimu+".dat' using 1:2 with points pt 1\n";
            // polygons
            string command2 = "set object 1 polygon from 1,55 to 2,54 to 5,56 to 1,55";
            string command3 = "set object 1 fc rgb 'cyan' border lt 1";

            //plotting the .dat file
            fprintf(pipe2, command1.c_str());
            //fprintf(pipe2, command2.c_str()); //plotting the .dat file
            //fprintf(pipe2, command3.c_str()); //plotting the .dat file

        }
#endif

        if (metadata) {
            metadata->setLastTStep(tstep);
        }

        if (enable_sqlite_out && (tstep % numStepTransactions) == (numStepTransactions - 1)) {
            std::cout << "End Transaction " << tstep << "\n";
            outSqlite->endDayLoop();
        }
#ifdef PROFILE
                                                                                                                                mLoopProfile.elapsed_ms();

        if ((mLoopProfile.runs() % 50) == 0) {
            memInfo.update();
            guiSendMemoryInfo(memInfo);
        }

        if ((mLoopProfile.runs() % 500) == 0) {
            lock();
            cout << "Average loop performance after " << mLoopProfile.runs() << "runs: " << (mLoopProfile.avg() * 1000.0) << "ms total: " << mLoopProfile.total() << "s\n";
            unlock();
        }
#endif
    }                             // end FOR LOOP OVER TIME

    CALLGRIND_STOP_INSTRUMENTATION;
    CALLGRIND_DUMP_STATS;

    if (enable_sqlite_out) {
        std::cout << "End Transaction " << tstep << "\n";
        outSqlite->endDayLoop();
    }

    thread_vessel_signal_exit();

#ifdef PROFILE
                                                                                                                            guiSendCapture(true);

    std::ostringstream ss;

    ss << "*** Profilers statistics ***\n";
    ss << "Node Load: " << (mLoadNodesProfileResult * 1000.0) << " ms\n";
    ss << "Vessel load: " << (mLoadVesselProfileResult * 1000.0) << " ms\n";
    ss << "Pop Load: " << (mLoadPopulationProfileResult * 1000.0) << " ms\n";
    ss << "Graph Load: " << (mLoadGraphProfileResult * 1000.0) << " ms\n";
    ss << "Loop performance after " << mLoopProfile.runs() << " runs: " << (mLoopProfile.avg() * 1000.0) << " ms " << mLoopProfile.total() << " s total\n";
    ss << "Vessel Loop performance after " << mVesselLoopProfile.runs() << " runs: " << (mVesselLoopProfile.avg() * 1000.0) << " ms " << mVesselLoopProfile.total() << " s total\n";
    ss << "Population Export performance after " << mPopExportProfile.runs() << " runs: " << (mPopExportProfile.avg() * 1000.0) << " ms " << mPopExportProfile.total() << " s total\n";

    memInfo.update();
    ss << "*** Memory Info: RSS: " << memInfo.rss()/1024 << "Mb - Peak: " << memInfo.peakRss()/1024 << "Mb" << endl;

    if (use_gui) {
        guiSendTerminalMessage(ss.str());
        guiSendCapture(false);
    } else {
        cout << ss.str() << "\n";
    }
#endif

    finalizeIpcQueue();


    {
        string memstat =
                outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/memstats_" + namesimu +
                ".dat";


        std::ofstream stats(memstat);
        std::ostringstream sstat;

        sstat << "*** Memory Statistics:\n";
#if !defined (__linux)

        sstat << "  RSS: " << getCurrentRSS() / (1024 * 1024) << "Mb\n";
        sstat << " Peak: " << getPeakRSS() / (1024 * 1024) << "Mb\n";
        sstat << "\n\n";
#else
                                                                                                                                MemStats statsColl;
        statsColl.collect();

        sstat << "       VM: " << statsColl.vm()/(1024*1024) << "Mb\n";
        sstat << "      RSS: " << statsColl.rss()/(1024*1024) << "Mb\n";
        sstat << " Peak RSS: " << getPeakRSS()/(1024*1024) << "Mb\n";
#endif
        std::cout << sstat.str();
        stats << sstat.str();

#if 0
                                                                                                                                std::ostringstream ss;
        std::ifstream status("/proc/self/status", std::ios_base::in);
        std::string line;
        while (std::getline(status,line)) {
            ss << line << "\n";
        }

        status.close();
        std::cout << ss.str();
        stats << ss.str();
#endif
    }


    // close all....
    OutputExporter::instance().close();
    loglike_prop_met.close();
    popdyn_N.close();
    popdyn_F.close();
    popdyn_annual_indic.close();
    freq_cpue.close();
    freq_profit.close();
    popdyn_test.close();
    popdyn_test2.close();
    popnodes_start.close();
    popnodes_end.close();

    if (enable_sqlite_out) {
        try {
            std::cout << "Creating database indexes...\n";
            outSqlite->createAllIndexes();
            outSqlite->close();
        } catch (SQLiteException &x) {
            std::cerr << "An error occurred closing the SQLite db: " << x.what() << "\n";
        }
    }

    // disable gnuplot
#if 0
                                                                                                                            #ifdef _WIN32
    if(use_gnuplot)
    {
       outc(cout << "type a char to close" << endl);
        getchar();				 //This line keeps the gnuplot window open after the code runs through.
        pclose(pipe2);
        pclose(pipe3);
        pclose(pipe4);
    }
    #endif
#endif
    //delete[] simModel->nodes();
    //delete[] vessels;

    return 0;
}


int main(int argc, char const *argv[])
{
    int return_code = 0;
    try {
        return app_main(argc, argv);
    } catch (std::exception &x) {
        std::cerr << "Unhandled exception : " << x.what() << "\n\n";

        thread_vessel_signal_exit();
        finalizeIpcQueue();

        return_code = 1;
    }

    return return_code;
}
