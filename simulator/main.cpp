// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2021 Francois Bastardie <fba@aqua.dtu.dk>

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

#include "runtime_tests/RuntimeTests.h"

#include <msqlitecpp/v1/sqlitestorage.h>

#include <boost/filesystem.hpp>

using namespace sqlite;

#include <msqlitecpp/v2/storage.h>

#include "helpers.h"

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

#include "Calendar.h"
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

#ifdef PROFILE
#include <profiler.h>
#endif

#include <version.h>
#include <mutex>
#include <chrono>
#include <thread>
#include <runtime_tests/NodeTester.h>
#include <TextfileModelLoader.h>
#include <DatabaseModelLoader.h>

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
vector<Benthos *> benthoss;
vector<int> tariff_pop;
int freq_update_tariff_code;
int update_tariffs_based_on_lpue_or_dpue_code;
vector<double> arbitary_breaks_for_tariff;
vector<double> met_multiplier_on_arbitary_breaks_for_tariff;
int total_amount_credited;
double tariff_annual_hcr_percent_change;

double tech_creeping_multiplier = 1;
bool enable_sqlite_out = true;
std::string outSqlitePath;

std::shared_ptr<SQLiteOutputStorage> outSqlite = nullptr;

vector<int> explicit_pops;
ClosureOptions closure_opts;
adjacency_map_t adjacency_map;
vector<string> vertex_names;
vector<types::NodeId> relevant_nodes;
multimap<int, int> nodes_in_polygons;
multimap<types::NodeId, int> possible_metiers;
multimap<types::NodeId, double> freq_possible_metiers;
multimap<types::NodeId, double> gshape_cpue_per_stk_on_nodes;
multimap<types::NodeId, double> gscale_cpue_per_stk_on_nodes;
multimap<types::NodeId, string> harbour_names;
vector<int> name_metiers;
ofstream freq_cpue;
ofstream freq_profit;
ofstream freq_distance;
ofstream vmslike2;
ofstream vmslike3;
vector<Metier *> metiers;
extern ofstream Q_DECL_IMPORT export_individual_tacs;
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
            (",p", po::value(&use_static_paths)->implicit_value(0), "Use static paths")
            (",e", po::value(&export_vmslike)->implicit_value(1), "Export VMSLike data")
            ("huge", po::value(&export_hugefiles)->implicit_value(0), "Export huge files data")
            (",v", po::value(&selected_vessels_only)->implicit_value(0), "Selected vessels only")
            (",d", po::value(&dparam), "dparam")
            ("indb", po::value(&inputdb), "Read input data from sqlite db, relative to Input Folder")
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
int app_main(int argc, char const* argv[])
{
#ifdef _WIN32
    string outdir = "C:";
#else
    bool DTU_HPC_SCRATCH = false;
    string home;
    if (DTU_HPC_SCRATCH) {
        home = "/SCRATCH/fbas"; // => DTU SCRATCH for HPC
    }
    else {
        home = getenv("HOME");
    }
    //outdir = home + "/ibm_vessels";
    outdir = home;
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

    auto simModel = std::make_shared<SimModel>(namesimu);

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
        boost::filesystem::path inpath = boost::filesystem::path{ inputfolder } / inputdb;
        inpath = boost::filesystem::absolute(inpath);
        std::cout << "Loading input db: " << inpath.string() << "\n";

        if (!boost::filesystem::exists(inpath)) {
            std::cerr << "Input file " << inpath << " doesn't exist.\n";
            return 2;
        }

        indb = std::make_shared<sql::Storage>(inpath.string(), sql::Storage::OpenMode::DelayedOpen);
        indb->open();

        modelLoader = std::make_unique<DatabaseModelLoader>(simModel, indb);
    }
    else {
        modelLoader = std::make_unique<TextfileModelLoader>(simModel,
            folder_name_parameterization, inputfolder,
            namefolderoutput);
    }

    // create a specific output directory for the ibm outcomes
    string an_output_folder;
    string a_basic_output_folder;
    string namefolder;

#ifdef _WIN32
    an_output_folder = outdir + "/DISPLACE_outputs";
    mkdir(an_output_folder.c_str());


    a_basic_output_folder = outdir + "/DISPLACE_outputs/" + namefolderinput;
    mkdir(a_basic_output_folder.c_str());
    // create a specific output directory for this simu
    namefolder = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput;
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
    const char* const path = "\"C:\\Program Files (x86)\\gnuplot\\bin\\gnuplot\"";
#else
    char* path = 0;
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

    auto const& scenario = simModel->scenario();

    if (scenario.dyn_alloc_sce.option(Options::fishing_credits)) {
        tariff_pop = scenario.tariff_pop;
        freq_update_tariff_code = scenario.freq_update_tariff_code;
        update_tariffs_based_on_lpue_or_dpue_code = scenario.update_tariffs_based_on_lpue_or_dpue_code;
        arbitary_breaks_for_tariff = scenario.arbitary_breaks_for_tariff;
        total_amount_credited = scenario.total_amount_credited;
        tariff_annual_hcr_percent_change = scenario.tariff_annual_hcr_percent_change;
    }


    stringstream graphnum;
    graphnum << scenario.a_graph;
    a_graph_name = a_graph_name + graphnum.str();

    // Create simulation scenario
    // ****
    // NOTE: This access a singleton and loads it with the time seriest and dtrees.
    // it is then used in other parts of the simulation, in particular by TimeSeriesEvaluator.
    // it should be advised to remove the singleon because it creates an hidden dependency through hidden
    // global object (the singleton).
    displace::simulation::Simulation* sim_scenario = displace::simulation::Simulation::instance();

    if (scenario.use_dtrees) {
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

    test::config(a_graph_name, *simModel);

    // implicit_pops is a vector of the index of pop (see pop_names.txt)
    // for which we do not have any info on the pops_N_at_szgroup because not assessed stock by ICES....
    // so the dynamic of these pops are not truly simulated but vessels can do some catches on them in any cases
    // using vessel and species-specific historic cpue data... (just like in paper Bastardie et al 2010)

    // make a random delay
    // to avoid chosing the seed in the same second!
    auto randomDelayUs = rand() % 10000;
    std::this_thread::sleep_for(std::chrono::microseconds{ randomDelayUs });

    // set a fixed seed
    simModel->initRandom(namesimu);


    // special case for random stochastic spatial pop distribution
    if (scenario.dyn_pop_sce.option(Options::white_noise_on_avai_deprecated)) {
        // pick up a file name randomly later on
        int rand_avai_file = (rand() % 50) + 1;
        char buffer[100];
        sprintf(buffer, "%03d", rand_avai_file);
        str_rand_avai_file = buffer;
        outc(cout << "the avai file randomly chosen is indexed  " << str_rand_avai_file << endl);
    }
    else {
        // no stochastic variation
        str_rand_avai_file = "baseline";
    }

    if (!OutputExporter::instantiate(outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput,
        namesimu)) {
        std::cerr << "Cannot open output files." << std::endl;
        throw std::runtime_error("this went wrong with instantiating the OutputExporter, quitting");
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
    }
    catch (SQLiteException& x) {
        std::cerr << "Cannot open output sqlite file: " << x.what() << "\n";
        throw std::runtime_error("this went wrong with opening output sqlite file, quitting");
    }

    if (metadata) {
        metadata->setNbPops(simModel->config().nbpops);
        metadata->setNbMets(simModel->config().nbmets);
        metadata->setNbBenthos(simModel->config().nbbenthospops);
        metadata->setNbSizes(NBSZGROUP);
    }

    filename = outdir + "/DISPLACE_outputs/" + namefolderinput + "/" + namefolderoutput + "/export_individual_tac_" +
        namesimu + ".dat";
    export_individual_tacs.open(filename.c_str());


    // NOTE: do this test if the proper command line argument is passed...
    {
        test::myRUtils();
        test::myUtils();

        dout(cout << "---------------------------" << endl);
        dout(cout << "---------------------------" << endl);
        dout(cout << " NODE-RELATED STUFFS      " << endl);
        dout(cout << "---------------------------" << endl);
        dout(cout << "---------------------------" << endl);

        NodeTester nodeTester(simModel->config().nbpops, simModel->config().nbmets, simModel->config().nbbenthospops);
        nodeTester.test();
    }



    // ---------------------------------------------- //

    auto a_graph_s = std::to_string(scenario.a_graph);

    if (!modelLoader->loadNodesAndGraphs()) {
        throw std::runtime_error("Cannot load Nodes and Graphs");
    }

#ifdef PROFILE
    memInfo.update();
    guiSendMemoryInfo(memInfo);

    mLoadNodesProfileResult = mLoadProfile.elapsed_ms();
#endif
    createRTreeFromNodes(simModel->nodes(), simModel->diffusionTree());
    
    // check 
    //bool ns = simModel->nodes().at(0)->get_is_harbour();
    //cout << ns;


    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " BENTHOS-RELATED STUFFS    " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);

    benthoss = modelLoader->loadBenthos(scenario.dyn_pop_sce, scenario.dyn_alloc_sce, scenario.biolsce,
        scenario.fleetsce);

    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " FISHFARMS-RELATED STUFFS " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);

    // TODO do not ignore return values?
    modelLoader->loadFishFarms();

    for (auto fishfarm : simModel->fishfarms()) {
        if (outSqlite) {
            outSqlite->exportFishfarmDef(*fishfarm);
        }
    }

    cout << "all fishfarms created...." << endl;


    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " WINDMILLS-RELATED STUFFS " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);

    // TODO do not ignore return values, or remove them
    modelLoader->loadWindmills();

    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " POPULATION-RELATED STUFFS " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);

#ifdef PROFILE
    mLoadProfile.start();
#endif


    // creation of a vector of simModel->populations()
    modelLoader->loadPopulations(1);

    
    //(re-)create a multimap for later use
    multimap<int, double> weight_per_szgroup;
    for (unsigned int ipop = 0; ipop < simModel->populations().size(); ++ipop) {
        vector<double> weight = simModel->populations().at(ipop)->get_weight_at_szgroup();
        for (unsigned int iw = 0; iw < weight.size(); ++iw) {
            weight_per_szgroup.insert(std::pair<int, double>(ipop, weight.at(iw)));

        }
    }
    simModel->setInitWeightPerSzgroup(weight_per_szgroup);

    
    // a check
    vector<double> some_Ns_at_szgroup = simModel->populations().at(0)->get_tot_N_at_szgroup();
    for (unsigned int i = 0; i < some_Ns_at_szgroup.size(); i++)
    {
        cout << some_Ns_at_szgroup[i] << " ";
    }
    cout << endl;



    // a check
    // nb of nodes for this pop this quarter:
    int nb_of_nodes_this_pop0_this_quarter = simModel->populations().at(0)->get_list_nodes().size();
    cout << "nb_of_nodes_this_pop0_this_quarter " << nb_of_nodes_this_pop0_this_quarter << endl;
    cout << endl;
    if (nb_of_nodes_this_pop0_this_quarter == 0) {
        cout << "something wrong: check the code for this message" << endl; return -1;
    }


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
    simModel->populations()[0]->aggregate_N();
    vector<double> a_Ns_at_szgroup_pop0 = simModel->populations()[0]->get_tot_N_at_szgroup();
   outc(cout << "check aggregate_N() " << endl);
    for(unsigned int i=0 ; i<a_Ns_at_szgroup_pop0.size();  i++)
    {
       outc(cout << a_Ns_at_szgroup_pop0[i] << " " );
    }
   outc(cout << endl);

    // restore back and check on node side
    simModel->populations()[0]->distribute_N();
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
    for (unsigned int i=0; i<simModel->populations().size(); i++)
    {
        cout << " the tac for this pop is " <<
             simModel->populations().at(i)->get_tac()->get_current_tac() << " " << endl;

        cout << " the tac percent_for_simulated_vessels for this pop is " <<
             simModel->populations().at(i)->get_tac()->get_percent_for_simulated_vessels() << endl;;
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

    if (scenario.dyn_alloc_sce.option(Options::fishing_credits)) {
     
        for (int metidx = 0; metidx < simModel->config().nbmets; metidx++)
        {
            auto initial_tariffs_on_nodes = read_initial_tariffs_on_nodes(folder_name_parameterization, inputfolder,
                a_graph_name, metidx);
     

           // init
           for (unsigned int a_idx = 0; a_idx < simModel->nodes().size(); a_idx++) {

            auto idx_node = simModel->nodes().at(a_idx)->get_idx_node();

            // initial tariff for this particular node
            auto lower_init_cr = initial_tariffs_on_nodes.lower_bound(idx_node);
            auto upper_init_cr = initial_tariffs_on_nodes.upper_bound(idx_node);
            double init_tariff;
            for (auto pos = lower_init_cr; pos != upper_init_cr; pos++)
                init_tariff = pos->second;

            if (initial_tariffs_on_nodes.count(idx_node) == 0) {
                init_tariff = 0;
            } // put 0 if this node is not informed

            simModel->nodes().at(a_idx)->set_tariffs(metidx, init_tariff);            }


           // check
           for (unsigned int a_idx = 0; a_idx < simModel->nodes().size(); a_idx++)
           {
            dout(cout << "this node " << simModel->nodes().at(a_idx)->get_idx_node() <<
                " has tariffs 0 " << simModel->nodes().at(a_idx)->get_tariffs().at(0) << endl);

            dout(cout << "this node " << simModel->nodes().at(a_idx)->get_idx_node() <<
                " has tariffs 1 " << simModel->nodes().at(a_idx)->get_tariffs().at(1) << endl);
           }
        }
    }
    else {
        // need to inform with a vector of zeros at least
        vector<double> init_tariffs(simModel->config().nbmets, 0);
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

    if (scenario.dyn_pop_sce.option(Options::sizeSpectra)) {
        cout << "sizeSpectra option is on..." << endl;

        // compute a predKernel and a searchVol
        // predKernel.at(j).at(kprey).at(k).at(name_pop)

        // read-in multimap on diet of stocks per stock
        multimap<int, double> adults_diet_preference_per_stock_allstks = read_adults_diet_preference_per_stock_allstks(
            folder_name_parameterization, inputfolder, scenario.biolsce);
        multimap<int, double> juveniles_diet_preference_per_stock_allstks = read_juveniles_diet_preference_per_stock_allstks(
            folder_name_parameterization, inputfolder, scenario.biolsce);


        cout << "compute Ws_at_szgroup..." << endl;
        for (unsigned int j = 0; j < simModel->config().nbpops; ++j) {  // loop over predators
            vector<double> W_this_pop = simModel->populations().at(j)->get_weight_at_szgroup();
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
        // beta_end + (beta_begin - beta_end) *(1+ exp(0.0005*(w0 -w(85))))/(1+ exp(0.0005*(w  -w(85))))  with beta_begin=100 and beta_end=500 so that larger fish eats on much smaller fish
        // Get weight per length bin matrix, apply the formula to it.
        //vector<vector<double> > beta (simModel->config().nbpops, vector<double>(NBSZGROUP));
        //for (unsigned int pop = 0; pop < simModel->config().nbpops; ++pop) {  // loop over pops
        //    for (unsigned int k = 0; k < NBSZGROUP; ++k) {  // loop over sizes
        //        beta.at(pop).at(k) = 500 + (100 - 500) * (1 + exp(0.0005 * (0.001 - 3383.912))) / (1 + exp(0.0005 * (Ws_at_szgroup.at(pop).at(k) - 3383.912))); // Sorry for hardcoding some values...
        //    }
        //}

       

        for (unsigned int prey = 0; prey < simModel->config().nbpops; ++prey) {  // loop over prey
            for (unsigned int j = 0; j < simModel->config().nbpops; ++j) {  // loop over predators
                vector<double> beta_this_pop = simModel->populations().at(j)->get_beta_ssm_at_szgroup();
                for (unsigned int k = 0; k < NBSZGROUP; ++k) {  // loop over predator sizes
                    for (unsigned int kprey = 0; kprey < NBSZGROUP; ++kprey) {  // loop over prey sizes
                        if (Ws_at_szgroup.at(j).at(k) < (beta_this_pop.at(k) * Ws_at_szgroup.at(prey).at(kprey))) {
                            predKernel.at(j).at(kprey).at(k).at(prey) =
                                exp(-pow(log((beta_this_pop.at(k) * Ws_at_szgroup.at(prey).at(kprey)) /
                                    Ws_at_szgroup.at(j).at(k)), 2) / (2 * pow(sigma.at(prey), 2)));
                            //cout <<  "predKernel.at("<<j<<").at("<<kprey<<").at("<<k<<").at("<<prey<<") is " << predKernel.at(j).at(kprey).at(k).at(prey) << endl;
                        }
                        else {
                            predKernel.at(j).at(kprey).at(k).at(prey) =
                                exp(-pow(log((beta_this_pop.at(k) * Ws_at_szgroup.at(prey).at(kprey)) /
                                    Ws_at_szgroup.at(j).at(k)), 2) / (2 * pow(4*sigma.at(prey), 2)));
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
                vector<double> beta_this_pop = simModel->populations().at(j)->get_beta_ssm_at_szgroup();
                for (unsigned int k = 0; k < NBSZGROUP; ++k) {
                    double alphae = sqrt(2 * PI) * sigma.at(prey) * pow(beta_this_pop.at(k), (lambda - 2)) *
                        exp(pow(lambda - 2, 2) * pow(sigma.at(prey), 2) / 2);

                    //cout << " this prey " << prey << " alphae is " << alphae << endl;
                    //cout << " given sigma.at(prey) is " << sigma.at(prey) << " beta_this_pop.at(k) is " << beta_this_pop.at(k) << " lambda is " << lambda << endl;

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
                //simModel->populations().at(prey)->set_adults_diet_preference_per_stock(adults_diet_preference.at(j));
                //simModel->populations().at(prey)->set_juveniles_diet_preference_per_stock(juveniles_diet_preference.at(j));
            }
        }


        cout << "Initial objects for sizeSpectra option...ok" << endl;
    }

    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " METIER-RELATED STUFFS     " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);

    modelLoader->loadMetiers(1, simModel->month(), simModel->quarter(), simModel->semester());
    
    int nb_mets = simModel->metiers().size();

    if (simModel->scenario().dyn_alloc_sce.option(Options::fishing_credits))
    {

        int a_count = -1;
        for (auto metier : simModel->metiers()) 
        {
            a_count++;
            metier->set_met_multiplier_on_arbitary_breaks_for_tariff(scenario.met_multiplier_on_arbitary_breaks_for_tariff.at(a_count));
        
        }
    }


    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " SHIP-RELATED STUFFS       " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);

    modelLoader->loadShips();

    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << " VESSEL-RELATED STUFFS     " << endl);
    dout(cout << "---------------------------" << endl);
    dout(cout << "---------------------------" << endl);

#ifdef PROFILE
    mLoadProfile.start();
#endif

    modelLoader->loadVessels(1, simModel->month(), simModel->quarter(), simModel->semester());
    for (auto vessel: simModel->vessels()) {
        if (enable_sqlite_out) {
            outSqlite->getVesselDefTable()->feedVesselsDefTable(vessel);
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

    unsigned int export_discards_in_logbooks = 1;
    if (scenario.dyn_alloc_sce.option(Options::doNotExportDiscardsInLogbooks)) 
    {
        export_discards_in_logbooks = 0;
    }

    // read nodes in closed area this month for area-based management,
    // (and setAreaType on the fly for displacing other_land if closed_to_other_as_well)
    if (scenario.dyn_alloc_sce.option(Options::area_monthly_closure)) {

        if (!read_metier_monthly_closures(simModel->nodes(), modelLoader->monthString(), a_graph_name,
                                          folder_name_parameterization,
                                          inputfolder)) {
            throw std::runtime_error("this went wrong with reading metier_monthly_closures files, quitting");
        }
        if (!read_vsize_monthly_closures(simModel->nodes(), modelLoader->monthString(), a_graph_name,
                                         folder_name_parameterization,
                                         inputfolder)) {
            throw std::runtime_error("this went wrong with reading vsize_monthly_closures files, quitting");
        }

    }
    if (scenario.dyn_alloc_sce.option(Options::area_closure)) {

        if (!read_metier_quarterly_closures(simModel->nodes(), modelLoader->quarterString(), a_graph_name,
                                            folder_name_parameterization,
                                            inputfolder)) {
            throw std::runtime_error("this went wrong with reading metier_quarterly_closures files, quitting");
        }
    }

    // init individual tacs
    if (simModel->is_tacs()) {
        for (unsigned int sp = 0; sp < simModel->populations().size(); sp++) {
            for (auto vessel : simModel->vessels()) {
                vessel->set_individual_tac_this_pop(export_individual_tacs, 0, simModel->populations(),
                                                    simModel->config().implicit_pops, sp, 1,
                                                    0.0);
            }
        }
    }


    // TODO: remove this hard-coded value!!!
//#ifdef BALTICSEA
    if (folder_name_parameterization == "BalticSea" || folder_name_parameterization == "Baltic21") {
        for (auto vessel : simModel->vessels()) {
            vessel->set_tankcapacity(vessel->get_tankcapacity() *
                3); // ACCOUNT FOR MISREPORTING in KW engine THAT CAN INTERFERE WITH STOPFISHING DTREE IN A BAD WAY i.e. limiting factor making 0 catch when triggered to return to port immediately.

        }
    }
//#endif





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
                simModel->vessels().begin(),
                simModel->vessels().end(),
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
    fill_from_graph(graph, graph_idx_dep, graph_idx_arr, graph_dist_km, scenario.nrow_graph);

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
    modelLoader->loadCalendar();

    if (enable_sqlite_out) {
        outSqlite->exportCalendar(simModel->calendar().months(), simModel->calendar().quarters(),
                                  simModel->calendar().semesters(), simModel->calendar().years());
    }

    int count_quarters = 1;
    int count_months = 1;
    double a_year = 1.0;

    // get a vector v filled in with 1 to n
    glob_mutex.lock();
    ve = vector<int>(simModel->vessels().size());
    for (unsigned int idx = 0; idx < ve.size(); idx++) {
        ve[idx] = idx;
    }
    glob_mutex.unlock();

    // init
    vector<vector<double> > a_catch_pop_at_szgroup(simModel->config().nbpops, vector<double>(NBSZGROUP));
    vector<vector<double> > a_ping_catch_pop_at_szgroup(simModel->config().nbpops, vector<double>(NBSZGROUP));
    vector<vector<double> > a_discards_pop_at_szgroup(simModel->config().nbpops, vector<double>(NBSZGROUP));


    // a check
    vector<double> here_some_Ns_at_szgroup = simModel->populations().at(0)->get_tot_N_at_szgroup();
    cout << "check here" << endl;
    for (unsigned int i = 0; i < here_some_Ns_at_szgroup.size(); i++)
    {
        cout << here_some_Ns_at_szgroup[i] << " ";
    }
    cout << endl;

    // a check
    // nb of nodes for this pop this quarter:
    int nb_of_nodes_this_pop_this_quarter = simModel->populations().at(0)->get_list_nodes().size();
    cout << "nb_of_nodes_this_pop_this_quarter " << nb_of_nodes_this_pop_this_quarter << endl;


    // write down initial pop number in popdyn
    for (unsigned int sp = 0; sp < simModel->populations().size(); sp++) {

        dout(cout << "write down the popdyn...");
        // get total N from summing up N over nodes
        simModel->populations().at(sp)->aggregate_N();
        popdyn_N << setprecision(0) << fixed;
        // tstep / pop / tot N at szgroup
        popdyn_N << 0 << " " << sp << " ";
        vector<double> tot_N_at_szgroup = simModel->populations().at(sp)->get_tot_N_at_szgroup();
        for (unsigned int sz = 0; sz < tot_N_at_szgroup.size(); sz++) {
            // output in thousands of individuals
            popdyn_N << tot_N_at_szgroup.at(sz) / 1000 << " ";
        }
        popdyn_N << " " << endl;
    }

    popdyn_N.flush();
    guiSendUpdateCommand(popdyn_N_filename, 0);

    // check input
    //vector<double> W_at_szgroup = simModel->populations().at(0)->get_weight_at_szgroup();
    //vector<double> prop_recru = simModel->populations().at(2)->get_proprecru_at_szgroup();
    //vector<double> mort = simModel->populations().at(2)->get_M_at_szgroup();
    //multimap<types::NodeId, double> mm =simModel->populations().at(2)->get_full_spatial_availability();


    // a check
    vector<double> there_some_Ns_at_szgroup = simModel->populations().at(0)->get_tot_N_at_szgroup();
    cout << "... and there" << endl;
    for (unsigned int i = 0; i < there_some_Ns_at_szgroup.size(); i++)
    {
        cout << there_some_Ns_at_szgroup[i] << " ";
    }
    cout << endl;




    //AT THE VERY START: export biomass pop on nodes for mapping e.g. in GIS
    if (enable_sqlite_out) {
        outSqlite->startDayLoop();
    }


    for (unsigned int n = 0; n < simModel->nodes().size(); n++) {
        simModel->nodes()[n]->export_popnodes(popnodes_start, simModel->initWeightPerSzgroup(), 0);
        if (enable_sqlite_out) {
            outSqlite->getNodesDefTable()->insert(simModel->nodes()[n]);
            bool r = outSqlite->getPopTable()->insert(0, simModel->nodes()[n], simModel->initWeightPerSzgroup());
        }
    }

    if (enable_sqlite_out) {
        outSqlite->endDayLoop();
    }

    popnodes_start.flush();
    // signals the gui that the filename has been updated.
    guiSendUpdateCommand(popnodes_start_filename, 0);


    // initial export at t=0
    // if(scenario.dyn_pop_sce.option(Options::include_forcing_layers))
    //{

    // Flush and updates all statistics for nodes envt
    if (use_gui) {
        nodes_envt.flush();
        for (unsigned int n = 0; n < simModel->nodes().size(); n++) {
            simModel->nodes().at(n)->export_nodes_envt(nodes_envt, simModel->timestep());
        }
        guiSendUpdateCommand(nodes_envt_filename, simModel->timestep());
    }

    if (enable_sqlite_out) {
        for (unsigned int n = 0; n < simModel->nodes().size(); n++) {
            outSqlite->exportEnvtNodes(simModel->timestep(), simModel->nodes().at(n));
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

    simModel->initTimestep();
    while (simModel->timestep() < nbsteps) {
        if (enable_sqlite_out && (simModel->timestep() % numStepTransactions) == 0) {
            std::cout << "Start Transaction " << simModel->timestep() << "\n";
            outSqlite->startDayLoop();
        }

#ifdef PROFILE
            mLoopProfile.start();
#endif

        dout(cout << endl);
        dout(cout << endl);
        dout(cout << "---------------" << endl);

        guiSendCurrentStep(simModel->timestep());

        if (!use_gui) {
            cout << "tstep: " << simModel->timestep() << endl;
        }
        ostringstream os;
        os << "tstep " << simModel->timestep() << endl;
        guiSendTerminalMessage(os.str());

        dout(cout << "---------------" << endl);

        //----------------------------------------//
        //----------------------------------------//
        // BIOLOGICAL MODULE----------------------//
        //----------------------------------------//
        //----------------------------------------//



        if (!applyBiologicalModule2(simModel->timestep(),
                                    simModel->month(),
                                    simModel->quarter(),
                                    simModel->year(),
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
                                    simModel->config().nbmets,
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
                                    simModel->calendar().quarters(),
                                    simModel->calendar().semesters(),
                                    simModel->calendar().years(),
                                    simModel->calendar().months(),
                                    simModel->config().implicit_pops,
                                    simModel->config().calib_oth_landings,
                                    selectivity_per_stock_ogives_for_oth_land,
                                    simModel->is_tacs(),
                                    simModel->is_other_land_as_multiplier_on_sp(),
                                    simModel->is_oth_land_per_metier(),
                                    export_vmslike,
                                    scenario.freq_do_growth,
                                    simModel->initWeightPerSzgroup(),
                                    simModel->species_interactions_mortality_proportion_matrix(),
                                    simModel->populations(),
                                    simModel->nodes(),
                                    simModel->vessels(),
                                    benthoss,
                                    scenario.dyn_pop_sce,
                                    scenario.dyn_alloc_sce,
                                    Ws_at_szgroup,
                                    predKernel,
                                    searchVolMat,
                                    juveniles_diet_preference,
                                    adults_diet_preference,
                                    mat_cats
        )) {
            throw std::runtime_error("Error while executing: applyBiologicalModule2");
        }


        if (scenario.dyn_pop_sce.option(Options::diffusePopN) &&
            simModel->calendar().isFirstDayOfMonth(simModel->timestep())) {
            // diffusion of pops on neighbour nodes
            // field_of_coeff_diffusion_this_pop give the node specific coeffs of diffusion
            // we can assume that this coeff is larger when the node is just transitional vs. lower when the node is a residential area
            // so a relevant proxy could actually be (the inverse of) full_avai_szgroup_nodes_with_pop
            // converted in a point porportion field....
            for (unsigned int sp = 0; sp < simModel->populations().size(); sp++) {
                outc(cout << "...pop " << sp << endl;)
                if (!binary_search(simModel->config().implicit_pops.begin(),
                                   simModel->config().implicit_pops.end(), sp)) {
                    outc(cout << "......pop " << sp << endl;)
                    simModel->populations().at(sp)->diffuse_N_from_field(adjacency_map); // per sz group
                }
            }
        }








        //----------------------------------------//
        //----------------------------------------//
        // READ READ DATA WHEN NEEDED-------------//
        //----------------------------------------//
        //----------------------------------------//

        if (simModel->calendar().isFirstDayOfYear(simModel->timestep())) {
            a_year += 1;


            if (scenario.dyn_alloc_sce.option(Options::TechCreeping3Per)) {
                tech_creeping_multiplier = pow(1.03, a_year); //  this is a rate per year...so multiply over years
            } else {
                tech_creeping_multiplier = 1.0;
            }



            // fishfarms for new year
            for (auto fishfarm : simModel->fishfarms()) {
                fishfarm->set_is_running(1);
            }


        }
        //cout << "a_year " << a_year << endl;



        dout(cout << "RE-READ DATA----------" << endl);
        multimap<string, double> reloaded_fcredits;

        // RE-READ DATA FOR EVENT => change of month
        // TODO use a wall clock object instead
        int a_quarter_i, a_semester_i;
        if (simModel->calendar().isFirstDayOfMonth(simModel->timestep())) {
            CurrentMonth += 1;

            {
                count_months += 1;
                auto a_month_i = count_months % 12;
          
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
                simModel->setYear(a_year);
            }


            // vector <double> a_tot_N_at_szgroup_here = simModel->populations().at(1)->get_tot_N_at_szgroup();
            // for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
            //  cout << "BEFORE RE-READ DATA: a_tot_N_at_szgroup[" << sz << "] is "<< a_tot_N_at_szgroup_here[sz]  << endl;

            // this month, re-read for vessel-related data
            if (scenario.dyn_alloc_sce.option(Options::area_monthly_closure)) {
                cout << "a_month: " << simModel->month() << ", a_quarter: " << simModel->quarter()
                     << ", simModel->semester():" << simModel->semester() << endl;

                for (auto vessel: simModel->vessels()) {
                    vessel->reinitDaysSpentInRestrictedAreaThisMonthtoZero();
                }
                // update the monthly closures on nodes
                if (!read_metier_monthly_closures(simModel->nodes(), modelLoader->monthString(), a_graph_name,
                                                  folder_name_parameterization,
                                                  inputfolder)) {
                    throw std::runtime_error("this went wrong with reading metier_monthly_closures files, quitting");
                }
                if (!read_vsize_monthly_closures(simModel->nodes(), modelLoader->monthString(), a_graph_name,
                                                 folder_name_parameterization,
                                                 inputfolder)) {
                    throw std::runtime_error("this went wrong with reading vsize_monthly_closures files, quitting");
                }

                // check for myfish graph1
                // cout << " isMetierBanned   "  << nodes.at(13)->isMetierBanned(vessels.at(v)->get_metier()->get_name()) << endl;
                // cout << " isVsizeBanned   " << nodes.at(13)->isVsizeBanned(vessels.at(v)->get_length_class()) << endl;


                cout << "re-read area closure setting this month....OK" << endl;
            }



            // this month, re-read for population-related data
            // CAUTION: THE ONLY POP READING DONE ON MONTH simModel->timestep()...THE OTHERS ARE DONE ON QUARTER BASIS in reLoad of LoadPopulations
            for (unsigned int i = 0; i < simModel->populations().size(); i++) {
                // read a other landings per node for this species
                auto oth_land = read_oth_land_nodes_with_pop(modelLoader->semesterString(),
                                                             modelLoader->monthString(), i,
                                                             folder_name_parameterization,
                                                             inputfolder, scenario.fleetsce);
                simModel->populations().at(i)->set_oth_land(oth_land);
            }

            if (scenario.dyn_alloc_sce.option(Options::otherLandPerMetPerPop))
            {
                for (unsigned int i = 0; i < simModel->populations().size(); i++) {
                    int met = -1, er = 0;
                    vector<map<types::NodeId, double> > oth_land_map_per_met;
                    do {
                        met += 1;
                        map<types::NodeId, double> a_map;
                        er = read_oth_land_map_per_met_on_nodes(a_map,
                                                                  modelLoader->semesterString(),
                                                                  modelLoader->monthString(), i, met,
                                                                  folder_name_parameterization,
                                                                  inputfolder, scenario.fleetsce);
                        if (er == 0) oth_land_map_per_met.push_back(a_map);
                    } while (er != -1);
                    simModel->populations().at(i)->set_oth_land_map_per_met(oth_land_map_per_met); 
                }
            }


            cout << "re-read oth_land_nodes setting this month....OK" << endl;


        }


        // RE-READ DATA FOR EVENT => change of quarter
        if (simModel->timestep() > 2000 && simModel->calendar().isFirstDayOfQuarter(simModel->timestep()))
            //   if(simModel->timestep()==3 || simModel->timestep()==4) // use this to start from another quarter if test...
        {

            cout << "a_month: " << simModel->month() << ", a_quarter: " << simModel->quarter() << ", a_semester:" << simModel->semester() << endl;

            //...but first compute and track the past experience
            int y = simModel->year() -1;
            int q = simModel->quarter() - 1;
            // caution with q from 1...nbyearquarters. given hardcoded limit in horizon time i.e. see "nbyearquarters"
            for (auto vessel : simModel->vessels())
            {
                //cout << "heho q" << q << endl;
                //cout << "heho y" << y << endl;
                vessel->compute_experiencedcpue_fgrounds_per_yearquarter_per_pop(y,q);
                //vector<vector<vector<double> > > dd = vessel->get_experiencedcpue_fgrounds_per_yearquarter_per_pop();
                vessel->clear_cumeffort_per_yearquarter();
            }           
                
            // RE-READ VESSEL DATA
            // fill in with new input files for fgrounds and harbours, etc.
            // if change of year-quarter or semester, to be quarter or semester-specific.

            // RE-read general vessel features: do not forget to clear the vectors!
            // not-quarter specific, clear anyway...
            // actually those variables do not change from a quarter to the next (see IBM_param_step4_vessels)

            modelLoader->loadVessels(a_year, simModel->month(), simModel->quarter(), simModel->semester());

            // RE-read for metiers
            cout << "re-read metiers..." << endl;
            modelLoader->loadMetiers(a_year, simModel->month(), simModel->quarter(), simModel->semester());
            cout << "re-read metiers...OK" << endl;
        } // END RE-READ DATA FOR VESSEL AND METIER...




        int redispatch_the_pop = 0;
        switch (scenario.freq_redispatch_the_pop) {
            case 0:
                if ((simModel->timestep() % 24) == 7) { redispatch_the_pop = 1; }
                // daily update
                break;
            case 1:
                if ((simModel->timestep() % 168) == 7) { redispatch_the_pop = 1; }
                // weekly update
                break;
            case 2:
                if (simModel->calendar().isFirstDayOfMonth(simModel->timestep())) { redispatch_the_pop = 1; }
                // monthly update
                break;
            case 3:
                if (simModel->calendar().isFirstDayOfQuarter(simModel->timestep())) { redispatch_the_pop = 1; }
                // quartely update
                break;
            case 4:
                if (simModel->calendar().isFirstDayOfYear(simModel->timestep())) { redispatch_the_pop = 1; }
                // semester update
                break;
        }


        if (redispatch_the_pop)     // EVENT => re-read pop data
        {

            cout << "Reload population data" << endl;

            modelLoader->loadPopulations(a_quarter_i);



            
            // CHECK...CHECK...CHECK...
            // write done  pop number in popdyn_test
            /*for (unsigned int sp=0; sp<simModel->populations().size(); sp++)
            {
                if (!binary_search (implicit_pops.begin(), implicit_pops.end(),  sp  ) )
                {

                    dout(cout  << "write down AFTER re-read pop in the popdyn_test file for checking...");
                    // get total N from summing up N over nodes
                    simModel->populations().at(sp)->aggregate_N();
                    popdyn_test << setprecision(0) << fixed;
                    // simModel->timestep() / pop / tot N at szgroup
                    popdyn_test << simModel->timestep() << " " << sp << " ";
                    vector <double>tot_N_at_szgroup=simModel->populations().at(sp)->get_tot_N_at_szgroup();
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

        if (simModel->timestep() == 0 || simModel->calendar().isFirstDayOfQuarter(simModel->timestep())) {

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
                            auto some_grounds = simModel->vessels().at(vids_on_harbours.at(vi))->get_fgrounds();
                            auto some_cpues = simModel->vessels().at(
                                    vids_on_harbours.at(vi))->get_experiencedcpue_fgrounds();

                            const auto &poss_met = simModel->vessels().at(
                                    vids_on_harbours.at(vi))->get_possible_metiers();
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


            if (scenario.dyn_alloc_sce.option(Options::shared_harbour_knowledge)) {

                for (auto vessel : simModel->vessels()) {
                    //1. draw a ground
                    auto grds = vessel->get_fgrounds();
                    auto freq_grds = vessel->get_freq_fgrounds();
                    // need to convert in array, see myRutils.cpp
                    if (!grds.empty()) {
                        int idx_max = max_element(freq_grds.begin(), freq_grds.end()) - freq_grds.begin();
                        auto ground = grds.at(idx_max);
                        cout << vessel->get_name() << ": ground is " << ground << endl;


                        //2. get possible metiers on this ground
                        const auto &poss_met = vessel->get_possible_metiers();
                        const auto &freq_poss_met = vessel->get_freq_possible_metiers();
                        auto metiers_on_grd = find_entries(poss_met, ground);
                        auto freq_metiers_on_grd = find_entries(freq_poss_met, ground);
                        // need to convert in array, see myRutils.cpp
                        auto a_met = do_sample(1, metiers_on_grd.size(), metiers_on_grd, freq_metiers_on_grd);

                        if (metiers_on_grd.size() != 0) {
                            vessel->set_metier(metiers[a_met.at(0)]);
                        } else {
                            vessel->set_metier(metiers[0]);   // dangerous fix
                        }

                        auto harbs = vessel->get_harbours();
                        auto freq_harbs = vessel->get_freq_harbours();
                        if (freq_harbs.empty()) {
                            cout << "check why..." << endl;
                        }

                        int idx_max2 = max_element(freq_harbs.begin(), freq_harbs.end()) - freq_harbs.begin();
                        auto a_node = harbs.at(idx_max2);  // cause the decision is taken in harbour...
                        cout << vessel->get_name() << ": "
                             << simModel->nodes().at(a_node.toIndex())->get_idx_node()
                             << " is in harb?`" <<
                             simModel->nodes().at(a_node.toIndex())->get_is_harbour()
                             << " ...cause the decision is taken in harbour..." << endl;
                        int current_metier = vessel->get_metier()->get_name();
                        cout << vessel->get_name() << ": current_metier is " << current_metier << endl;
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
                            grounds_from_harbours = vessel->get_fgrounds();
                            freq_grounds_from_harbours = vessel->get_freq_fgrounds();
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

                        cout << vessel->get_name() << ": grounds_from_harbours is " << endl;
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
                        vessel->set_spe_fgrounds(grounds_from_harbours); // CHANGED
                        vessel->set_spe_freq_fgrounds(freq_grounds_from_harbours); // CHANGED
                        vessel->set_experienced_bycatch_prop_on_fgrounds(
                                freq_grounds_from_harbours);// re-dimensioned
                        vessel->set_experienced_avoided_stks_bycatch_prop_on_fgrounds(
                                freq_grounds_from_harbours);// re-dimensioned
                        vessel->set_cumcatch_fgrounds(experiencedcpue_fgrounds);// re-dimensioned
                        vessel->set_cumcatch_fgrounds_per_pop(experiencedcpue_fgrounds_per_pop);// re-dimensioned
                        vessel->set_cumdiscard_fgrounds(experiencedcpue_fgrounds);// re-dimensioned
                        //vessel->set_cumdiscard_fgrounds_per_pop(experiencedcpue_fgrounds_per_pop);// re-dimensioned
                        vessel->set_cumeffort_per_trip_per_fgrounds(freq_grounds_from_harbours);// re-dimensioned
                        vessel->set_experiencedcpue_fgrounds(experiencedcpue_fgrounds); // re-dimensioned
                        vessel->set_experiencedcpue_fgrounds_per_pop(
                                experiencedcpue_fgrounds_per_pop); // re-dimensioned
                        vessel->set_freq_experiencedcpue_fgrounds(
                                freq_experiencedcpue_fgrounds); // re-dimensioned
                        vessel->set_freq_experiencedcpue_fgrounds_per_pop(
                                freq_experiencedcpue_fgrounds_per_pop); // re-dimensioned
                        vessel->set_spe_possible_metiers(possible_metiers_from_harbours); // CREATED
                        vessel->set_spe_freq_possible_metiers(freq_possible_metiers_from_harbours); // CREATED

                        // inform grounds in closed areas
                        auto new_grds = vessel->get_fgrounds();
                        vector<types::NodeId> fgrounds_in_closed_areas;
                        for (unsigned int i = 0; i < new_grds.size(); ++i) {
                            if (simModel->nodes().at(new_grds.at(i).toIndex())->evaluateAreaType() == 1) {
                                fgrounds_in_closed_areas.push_back(new_grds.at(i));
                            }
                        }
                        vessel->set_fgrounds_in_closed_areas(fgrounds_in_closed_areas);
                    } else {
                        // otherwise, no activity this vessel this quarter
                        cout << " --> no activity for " << vessel->get_name() << endl;


                    }
                } // end v
            } // end sce











        } // end re-read harbour



   
        
   dout(cout << "THE TARIFF MAP UPDATE----------" << endl);
   // compute tarif map update      
   if (scenario.dyn_alloc_sce.option(Options::fishing_credits)) {

     int is_first_tstep_of_the_year = binary_search(simModel->calendar().years().begin(), simModel->calendar().years().end(), simModel->timestep());
     int is_first_tstep_of_the_month = binary_search(simModel->calendar().months().begin(), simModel->calendar().months().end(), simModel->timestep());
   
     if (!computeTariffMapUpdate(
           scenario.dyn_alloc_sce,
           is_first_tstep_of_the_year,     
           is_first_tstep_of_the_month,
           simModel->timestep(),
           simModel->config().nbpops,
           simModel->config().nbmets,
           simModel->nodes(),
           simModel->populations(),
           simModel->vessels(),
           arbitary_breaks_for_tariff,
           tariff_pop,
           tariff_annual_hcr_percent_change,
           update_tariffs_based_on_lpue_or_dpue_code,
           freq_update_tariff_code,
           total_amount_credited,
           reloaded_fcredits
       )) {
           throw std::runtime_error("Error while executing: computeTariffMapUpdate()");
       }
   }
 




        dout(cout << "THE WINDFARM LOOP----------" << endl);
        for (unsigned int i = 0; i < simModel->windmills().size(); i++) {

            if (simModel->windmills().at(i)->get_is_active() == 1) {
                simModel->windmills().at(i)->compute_kWproduction_in_farm(); // discrete event
                //cout << "kW production in farm " << i << " is " << windmills.at(i)->get_kWproduction_in_farm() << endl;
                if (export_hugefiles) {
                    simModel->windmills().at(i)->export_windmills_indicators(windmillslogs,
                                                                             simModel->timestep()); // export event to file...
                }

                if (enable_sqlite_out) {
                    outSqlite->exportWindmillsLog(simModel->windmills().at(i), simModel->timestep());
                }
            }

        }

        // Flush and updates all statistics for fsihfarms
        if (use_gui) {

            if (simModel->calendar().isFirstDayOfMonth(simModel->timestep())) {
                windmillslogs.flush();
                guiSendUpdateCommand(windmillslogs_filename, simModel->timestep());
            }
        }


        dout(cout << "THE SHIP LOOP----------" << endl);
        for (auto ship : simModel->ships()) {
            if (ship->get_is_active() == 1) {
                ship->compute_emissions_in_ship(); // discrete event
                //cout << "Emission in ships " << i << " is " << ships.at(i)->get_NOxEmission() << endl;
                if (export_hugefiles) {
                    ship->export_ships_indicators(shipslogs, simModel->timestep()); // export event to file...
                }
                if (enable_sqlite_out) {
                    outSqlite->exportShip(simModel->timestep(), ship);
                }
            }

        }

        // Flush and updates all statistics for ships
        if (use_gui) {

            if (simModel->calendar().isFirstDayOfMonth(simModel->timestep())) {
                shipslogs.flush();
                guiSendUpdateCommand(shipslogs_filename, simModel->timestep());
            }
        }


        dout(cout << "THE FISHFARM LOOP----------" << endl);
        for (auto fishfarm : simModel->fishfarms()) {

            if (fishfarm->get_is_active() == 1) {
                int start = fishfarm->get_start_day_growing();
                int end = fishfarm->get_end_day_harvest();
                if ((int) ((simModel->timestep() + 1) / 24 - (8762.0 / 24 * (a_year - 1))) == end &&
                    fishfarm->get_is_running() == 1) {
                    fishfarm->compute_profit_in_farm(); // discrete event
                    //cout << "profit in farm " << i << " is " << fishfarms.at(i)->get_sim_annual_profit() << endl;
                    fishfarm->export_fishfarms_indicators(fishfarmslogs,
                                                          simModel->timestep()); // export event to file...

                    if (enable_sqlite_out) {
                        outSqlite->exportFishfarmLog(fishfarm, simModel->timestep());
                    }

                    //...and reset
                    fishfarm->set_is_running(0);
                    fishfarm->set_sim_individual_mean_kg(0.0);
                    fishfarm->set_sim_kg_harvested(0.0);
                    fishfarm->set_sim_kg_eggs_harvested(0.0);
                    fishfarm->set_sim_net_discharge_N(0.0);
                    fishfarm->set_sim_net_discharge_P(0.0);
                    fishfarm->set_sim_annual_profit(0.0);
                } else {
                    if (fishfarm->get_is_running() == 1 &&
                        (int) ((simModel->timestep() + 1) / 24 - (8762.0 / 24 * (a_year - 1))) > start) {
                        // fish growth...
                        fishfarm->compute_current_sim_individual_mean_kg_in_farm(simModel->timestep(), a_year);

                        //...environmental impact
                        fishfarm->compute_discharge_on_farm(simModel->timestep());
                        //cout << "discharge N from farm " << i << " is " << fishfarms.at(i)->get_sim_net_discharge_N() << "kg" << endl;
                        //cout << "discharge P from farm " << i << " is " << fishfarms.at(i)->get_sim_net_discharge_P() << "kg" << endl;

                        if (simModel->calendar().isFirstDayOfMonth(simModel->timestep())) {

                            if (enable_sqlite_out) {
                                outSqlite->exportFishfarmLog(fishfarm, simModel->timestep());
                            }

                            if (export_hugefiles) {
                                fishfarm->export_fishfarms_indicators(fishfarmslogs,
                                                                      simModel->timestep()); // export event to file
                            }
                        }
                    }
                }
            }

        }

        // Flush and updates all statistics for fishfarms
        if (use_gui) {

            if (simModel->calendar().isFirstDayOfMonth(simModel->timestep())) {
                fishfarmslogs.flush();
                guiSendUpdateCommand(fishfarmslogs_filename, simModel->timestep());
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
        if (simModel->timestep() == 0) {
            for (unsigned int i = 0; i < ve.size(); i++) {
                bool is_harbour = simModel->vessels()[i]->get_loc()->get_is_harbour();
                cout << simModel->vessels()[i]->get_name() << " departure from an harbour? " << is_harbour
                     << " idx node: " << simModel->vessels()[i]->get_loc()->get_idx_node() << endl;


                //cout << "simModel->timestep(): "<< simModel->timestep() << "export loglike for " << listVesselIdForLogLikeToExport.at(idx)<< endl;
                OutputExporter::instance().exportVmsLike(simModel->timestep(),
                                                         simModel->vessels()[i]); // at simModel->timestep()=0


            }

        }

        ///------------------------------///
        ///------------------------------///
        ///  THE EFFORT CONTROL          ///
        /// (ON GoFishing dtree)         ///
        ///------------------------------///
        ///------------------------------///

        if (scenario.dyn_alloc_sce.option(Options::EffortMinControl)) {
            if (simModel->calendar().isFirstDayOfYear(simModel->timestep())) {
                if (nb_y_left_to_tgrt_year > 1) {
                    nb_y_left_to_tgrt_year = nb_y_left_to_tgrt_year -
                                             1;
                } // target year for reaching the FMSY. HARDCODED FOR NOW

                // will alter GoFishing dtree final leaf proba
                if (!computeEffortMultiplier(simModel->populations(),
                                             simModel->vessels(),
                                             nb_y_left_to_tgrt_year,
                                             1)) {
                    throw std::runtime_error("Error while executing: computeEffortMultiplier");
                }
            }
        }

        if (scenario.dyn_alloc_sce.option(Options::EffortMaxControl)) {
            if (simModel->calendar().isFirstDayOfYear(simModel->timestep())) {
                if (nb_y_left_to_tgrt_year > 1) {
                    nb_y_left_to_tgrt_year = nb_y_left_to_tgrt_year -
                                             1;
                } // target year for reaching the FMSY. HARDCODED FOR NOW

                // will alter GoFishing dtree final leaf proba
                if (!computeEffortMultiplier(simModel->populations(),
                                             simModel->vessels(),
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

        for (unsigned int s = 0; s < simModel->ships().size(); s++) {
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

        if (scenario.dyn_alloc_sce.option(Options::TACs)) {

            for (unsigned int pop = 0; pop < simModel->populations().size(); pop++) {

                outc(cout << "...pop " << pop << endl;)
                if (!binary_search(simModel->config().implicit_pops.begin(), simModel->config().implicit_pops.end(),
                                   pop)) {

                    double so_far = simModel->populations().at(pop)->get_landings_so_far();
                    simModel->globalQuotasUptake().at(pop) =
                            (so_far / 1000) / (simModel->populations().at(pop)->get_tac()->get_current_tac());

                    simModel->populations().at(pop)->set_quota_uptake(simModel->globalQuotasUptake().at(pop));
                    simModel->populations().at(pop)->set_quota(
                            simModel->populations().at(pop)->get_tac()->get_current_tac());

                    if (enable_sqlite_out) {
                        outSqlite->exportPopQuotas(simModel->populations().at(pop), pop, simModel->timestep());
                    }

                    //cout <<"pop "<< pop << ": simModel->globalQuotasUptake() is " << simModel->globalQuotasUptake().at(pop) << endl;

                    // export in file
                    quotasuptake << setprecision(6) << fixed;
                    quotasuptake << simModel->timestep() << " " << pop << " " <<
                                 simModel->globalQuotasUptake().at(pop) << " " <<
                                 simModel->populations().at(pop)->get_tac()->get_current_tac() << endl;



                    // if more than x% of vessels choked then declare this stock as choking fisheries
                    int nbchoked = 0;
                    for (auto vessel : simModel->vessels()) {
                        nbchoked += vessel->get_is_choked().at(pop);
                    }
                    // HARDCODED threshold...
                    if (nbchoked >= ceil(0.3 * simModel->vessels().size())) {
                        simModel->populations().at(pop)->set_is_choking_fisheries(1);
                    }

                }
            }
        }



        ///------------------------------///
        ///------------------------------///
        ///  THE DIFFUSIVE ENVT          ///
        ///------------------------------///
        ///------------------------------///
        if (scenario.dyn_pop_sce.option(Options::diffuseNutrients)) {
            int numStepDiffusions = 100; // e.g. diffuse every 100 tsteps
            if ((simModel->timestep() % numStepDiffusions) == (numStepDiffusions - 1)) {

                // naive diffusion
                double coeff_diffusion = 0.4;
                //bool r= diffuse_Nitrogen_in_every_directions(nodes, adjacency_map, coeff_diffusion);

                // gradient diffusion
                // using the rtree
                bool r = diffuse_Nitrogen_with_gradients(simModel->nodes(), adjacency_map,
                                                         simModel->diffusionTree(), coeff_diffusion);
                // bool r=  diffuse_Phosphorus_with_gradients(simModel->nodes(), adjacency_map, rtree, coeff_diffusion);
                // bool r=  diffuse_Oxygen_with_gradients(simModel->nodes(), adjacency_map, rtree, coeff_diffusion);
                // bool r=  diffuse_Dissolvedcarbon_with_gradients(simModel->nodes(), adjacency_map, rtree, coeff_diffusion);



                if (enable_sqlite_out) {
                    for (unsigned int n = 0; n < simModel->nodes().size(); n++) {
                        outSqlite->exportEnvtNodes(simModel->timestep(), simModel->nodes().at(n));
                    }
                }


            }
        }

        // Flush and updates all statistics for simModel->nodes() envt
        if (use_gui) {
            if (simModel->calendar().isFirstDayOfMonth(simModel->timestep())) {
                nodes_envt.flush();
                for (unsigned int n = 0; n < simModel->nodes().size(); n++) {
                    simModel->nodes().at(n)->export_nodes_envt(nodes_envt, simModel->timestep());
                }
                guiSendUpdateCommand(nodes_envt_filename, simModel->timestep());
            }
        }

        ///------------------------------///
        ///------------------------------///
        ///  THE DIFFUSIVE BENTHOS       ///
        ///------------------------------///
        ///------------------------------///
        if (scenario.dyn_pop_sce.option(Options::diffuseBenthos)) {
            int numStepDiffusions = 100; // e.g. diffuse every 100 tsteps
            if ((simModel->timestep() % numStepDiffusions) == (numStepDiffusions - 1)) {

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

        if (scenario.dyn_pop_sce.option(Options::modelShippingOnBenthos)) {
            if (simModel->calendar().isFirstDayOfMonth(simModel->timestep())) {
                double shippingdensity = 0;
                double bathymetry = 0;
                for (unsigned int i = 0; i < simModel->nodes().size(); i++) {
                    shippingdensity = simModel->nodes().at(i)->get_shippingdensity();
                    if (scenario.dyn_alloc_sce.option(Options::halfShippingDensity)) {
                        shippingdensity = shippingdensity / 2;
                    }
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

                            if (scenario.dyn_pop_sce.option(Options::modelBenthosInN)) {
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
        if (scenario.dyn_pop_sce.option(Options::modelBenthosInLongevity)) {

            for (unsigned int b = 0; b < benthoss.size(); b++) {
                benthoss.at(b)->recover_benthos_tot_biomass_per_funcgroup(1);
            }
        }


        ///------------------------------///
        ///------------------------------///
        ///  REINIT SOME VALUES ON NODES ///
        ///------------------------------///
        ///------------------------------///

        if (simModel->calendar().isFirstDayOfYear(simModel->timestep())) {
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
                    //cout << "simModel->timestep(): "<< simModel->timestep() << "export vmslike for " << listVesselIdForVmsLikeToExport.at(idx)<< endl;

                    OutputExporter::instance().exportVmsLike(simModel->timestep(),
                                                             simModel->vessels()[listVesselIdForVmsLikeToExport.at(
                                                                     idx)]);
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

                OutputExporter::instance().exportVmsLikeFPingsOnly(simModel->timestep(),
                                                                   simModel->vessels()[listVesselIdForVmsLikeFPingsOnlyToExport.at(
                                                                           idx)], simModel->populations(),
                                                                   simModel->config().implicit_pops);
                simModel->vessels()[listVesselIdForVmsLikeFPingsOnlyToExport.at(
                        idx)]->clear_ping_catch_pop_at_szgroup();
            }
            listVesselIdForVmsLikeFPingsOnlyToExport.clear();


            for (unsigned int idx = 0; idx < listVesselIdForLogLikeToExport.size(); idx++) {
                //cout << "simModel->timestep(): "<< simModel->timestep() << "export loglike for " << listVesselIdForLogLikeToExport.at(idx)<< endl;
                OutputExporter::instance().exportLogLike(simModel->timestep(),
                                                         simModel->vessels()[listVesselIdForLogLikeToExport.at(idx)],
                                                         simModel->populations(), simModel->config().implicit_pops, 
                                                         export_discards_in_logbooks);
                simModel->vessels()[listVesselIdForLogLikeToExport.at(idx)]->reinit_after_a_trip();
            }
            listVesselIdForLogLikeToExport.clear();


           

            for (unsigned int idx = 0; idx < listVesselIdForTripCatchPopPerSzgroupExport.size(); idx++) {
                OutputExporter::instance().exportTripCatchPopPerSzgroup(simModel->timestep(),
                                                                        simModel->vessels()[listVesselIdForTripCatchPopPerSzgroupExport.at(
                                                                                idx)], simModel->populations(),
                                                                        simModel->config().implicit_pops);
            }
            listVesselIdForTripCatchPopPerSzgroupExport.clear();

        }


        // EXPORT: vessel_loglike - disabled
        /*
        if (use_gui) {
            loglike.flush();
            guiSendUpdateCommand(loglike_filename, simModel->timestep());
        }*/

        // move the ships along the ship lanes
        //for(unsigned int s=0; s<ships.size(); s++)
        //{
        //      ships[ s ]->move();
        //}


        if (metadata) {
            metadata->setLastTStep(simModel->timestep());
        }

        if (enable_sqlite_out && (simModel->timestep() % numStepTransactions) == (numStepTransactions - 1)) {
            std::cout << "End Transaction " << simModel->timestep() << "\n";
            outSqlite->endDayLoop();
        }

        simModel->nextTimestep();
    }                             // end FOR LOOP OVER TIME

    CALLGRIND_STOP_INSTRUMENTATION;
    CALLGRIND_DUMP_STATS;

    if (enable_sqlite_out) {
        std::cout << "End Transaction " << simModel->timestep() << "\n";
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

#if !defined(DEBUG)
    try {
#endif
    return app_main(argc, argv);

#if !defined(DEBUG)
    } catch (std::exception &x) {
        std::cerr << "Unhandled exception : " << x.what() << "\n\n";

        thread_vessel_signal_exit();
        finalizeIpcQueue();

        return_code = 1;
    }

    return return_code;
#endif
}
