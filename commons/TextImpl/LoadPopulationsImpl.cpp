//
// Created by happycactus on 3/28/20.
//


#include <helpers.h>
#include "commons_global.h"

#include "TextfileModelLoader.h"
#include "Impl.h"
#include "Population.h"
#include "SimModel.h"
#include "idtypeshelpers.h"

#define NBSZGROUP 14
#define NBAGE 11                 // nb age classes max
#define SEL_NBSZGROUP 5             // DEPRECATED - according to the R glm on cpue (see R code)

namespace {

// TODO remove Loaded Data, it is useless
struct LoadedData {
    std::vector<string> vectsparam1;
    std::vector<string> vectsparam2;
    std::vector<string> vectsparam3;
    std::vector<string> vectsparam4;
    std::vector<string> vectsparam5;
    std::vector<VesselCalendar> vectcalendar1;
    std::vector<double> vectdparam1;
    std::vector<double> vectdparam2;
    std::vector<double> vectdparam3;
    std::vector<double> vectdparam4;
    std::vector<double> vectdparam5;
    std::vector<double> vectdparam6;
    std::vector<double> vectdparam7;
    std::vector<double> vectdparam8;
    std::vector<double> vectdparam9;
    std::vector<double> vectdparam10;
    std::vector<double> vectdparam11;
    std::vector<double> vectdparam12;
    std::vector<double> vectdparam13;
    std::vector<double> vectdparam14;
    std::vector<double> vectdparam15;
    std::vector<double> vectdparam16;
    std::vector<double> vectdparam17;
    std::vector<double> vectdparam18;
    std::vector<double> vectdparam19;
    std::vector<double> vectdparam20;
    std::vector<double> vectdparam21;
    std::vector<double> vectdparam22;
    std::vector<double> vectdparam23;
    std::vector<double> vectdparam24;
    std::vector<double> vectdparam25;
    std::vector<double> vectdparam26;
    std::vector<double> vectdparam27;
    std::vector<double> vectdparam28;
    std::vector<double> vectdparam29;
    std::vector<double> vectdparam30;
    std::vector<double> vectdparam31;
    std::vector<double> vectdparam32;
    std::vector<double> vectdparam33;
    std::vector<double> vectdparam34;
    std::vector<double> vectdparam35;
    std::vector<double> vectdparam36;
    std::vector<double> vectdparam37;
    std::vector<double> vectdparam38;
    std::vector<double> vectdparam39;
    std::vector<double> vectdparam40;
    std::vector<double> vectdparam41;
    std::vector<double> vectdparam42;
    std::vector<double> vectdparam43;
    std::vector<double> vectdparam44;
    std::vector<double> vectdparam45;
    std::vector<double> vectdparam46;
    std::vector<double> vectdparam47;
    std::vector<double> vectdparam48;
    std::vector<double> vectdparam49;
    std::vector<double> vectdparam50;
    std::vector<int> vectiparam1;
    std::vector<int> vectiparam2;
    std::vector<int> vectiparam3;
    std::vector<int> vectiparam4;
    std::vector<int> vectiparam5;
    std::vector<int> vectiparam6;
    std::vector<int> vectiparam7;
    std::vector<int> vectiparam8;
    std::vector<int> vectiparam9;
    std::vector<int> vectiparam10;
    std::multimap<int, double> mmapidparam1;
    std::multimap<int, double> mmapidparam2;
    std::multimap<int, double> mmapidparam3;
    std::multimap<int, double> mmapidparam4;
    std::multimap<int, double> mmapidparam5;
    std::multimap<int, double> mmapidparam6;
    std::multimap<int, double> mmapidparam7;
    std::multimap<int, double> mmapidparam8;
    std::multimap<string, double> mmapsdparam1;
    std::multimap<string, double> mmapsdparam2;
    std::multimap<string, double> mmapsdparam3;
    std::multimap<string, double> mmapsdparam4;
    std::multimap<string, double> mmapsdparam5;
    std::multimap<string, double> mmapsdparam6;
    std::multimap<string, double> mmapsdparam7;
    std::multimap<string, double> mmapsdparam8;
    std::multimap<string, types::NodeId> mmapsnparam1;
    std::multimap<string, types::NodeId> mmapsnparam2;
    std::multimap<string, types::NodeId> mmapsnparam3;
    std::multimap<string, types::NodeId> mmapsnparam4;
    std::multimap<types::NodeId, int> mmapniparam1;
    std::multimap<types::NodeId, int> mmapniparam2;
    std::multimap<types::NodeId, int> mmapniparam3;
    std::multimap<types::NodeId, int> mmapniparam4;
    std::multimap<types::NodeId, int> mmapniparam5;
    std::multimap<types::NodeId, int> mmapniparam6;
    std::multimap<types::NodeId, int> mmapniparam7;
    std::multimap<types::NodeId, int> mmapniparam8;
    std::multimap<types::NodeId, int> mmapniparam9;
    std::multimap<types::NodeId, int> mmapniparam10;
    std::multimap<types::NodeId, double> mmapndparam1;
    std::multimap<types::NodeId, double> mmapndparam2;
    std::vector<map<string, double> > vectmapsdparam1;
    std::vector<map<int, double> > vectmapsdparam2;
    std::vector<map<int, double> > vectmapsdparam3;
    std::vector<vector<map<types::NodeId, double> > > vovomapndparam1;
    std::map<int, int> mapiiparam1;
    std::map<int, int> mapiiparam2;
    std::map<int, double> mapidparam1;
    std::map<int, double> mapidparam2;
    std::map<int, double> mapidparam3;
    std::map<int, double> mapidparam4;
    std::map<int, string> mapisparam1;
    std::vector<map<int, int> > vectmapiiparam1;
    std::vector<map<int, double> > vectmapidparam1;
    std::vector<map<types::NodeId, double> > vectmapndparam1;
    std::vector<multimap<types::NodeId, int> > vectmmapniparam1;
    std::vector<multimap<types::NodeId, double> > vectmmapndparam1;
    std::vector<multimap<types::NodeId, double> > vectmmapndparam2;
    std::vector<multimap<types::NodeId, double> > vectmmapndparam3;
    std::vector<multimap<types::NodeId, double> > vectmmapndparam4;
    std::vector<multimap<int, double> > vectmmapidparam1;
    std::vector<vector<double> > vovd1;
    std::vector<vector<double> > vovd2;
    std::vector<vector<double> > vovd3;
    std::vector<vector<double> > vovd4;
    std::vector<vector<double> > vovd5;
    std::vector<vector<double> > vovd6;
    std::vector<vector<double> > vovd7;
    std::vector<vector<double> > vovd8;
    std::vector<vector<double> > vovd9;
    std::vector<vector<double> > vovd10;
    std::vector<vector<double> > vovd11;
    std::vector<vector<double> > vovd12;
    std::vector<vector<double> > vovd13;
    std::vector<vector<types::NodeId> > vovn1;
    std::vector<vector<int> > vovi1;
    std::vector<vector<int> > vovi2;
    std::vector<vector<int> > vovi3;
    std::vector<vector<int> > vovi4;
    std::vector<vector<int> > vovi5;
    std::vector<vector<vector<double> > > vovovd1;
    std::vector<vector<vector<double> > > vovovd2;
    std::vector<vector<vector<double> > > vovovd3;
    int int1;
    int int2;
    int int3;
    int int4;
    int int5;
    double double1;
    double double2;
    double double3;
    double double4;
    double double5;
    double double6;
    double double7;
    double double8;
    double double9;
    double double10;
    double double11;
    double double12;
    double double13;
    double double14;
    double double15;
    double double16;
    double double17;
    double double18;
    double double19;
    double double20;
    double double21;
    double double22;
    double double23;
    double double24;
    double double25;
    double double26;
    double double27;
    double double28;
    double double29;
    double double30;
    multimap<int, double> mmapidparam_init_weight_per_szgroup;
    vector<vector<double> > vovd_species_interactions_mortality_proportion_matrix;
};
}

void read_pop_names_in_string(map<int, string> &pop_names,
                              string folder_name_parameterization, string inputfolder)
{

    string filename = inputfolder + "/pop_names_" + folder_name_parameterization + ".txt";
    ifstream file_pop_names;
    file_pop_names.open(filename.c_str());
    if (file_pop_names.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    } else {
        fill_map_from_specifications_i_s(file_pop_names, pop_names, inputfolder);
        file_pop_names.close();

    }
}


// FOR POPULATION
multimap<int, double>
read_avai_betas(string a_semester, string szgroup, string folder_name_parameterization, string inputfolder)
{

    string filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/avai" + szgroup + "_betas_semester" +
                      a_semester + ".dat";

    //input data, pop characteristics: e.g. avai0_betas i.e. betas per pop for szgroup 0
    ifstream avai_betas_file;
    avai_betas_file.open(filename.c_str());
    if (avai_betas_file.fail()) {
        cout << "Unfortunately the /avaiXX_betas_semesterXX.dat vector is not informed " << "\n";
        cout << "You´ll have to stop the simu, correct input and re-run. " << "\n";
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    // it is actually only a map...but was simpler to reuse the multimap routines
    multimap<int, double> avai_betas;
    fill_multimap_from_specifications_i_d(avai_betas_file, avai_betas);
    avai_betas_file.close();

    return (avai_betas);
}


multimap<int, double>
read_init_pops_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce)
{

    string filename =
            inputfolder + "/popsspe_" + folder_name_parameterization + "/init_pops_per_szgroup_biolsce" + biolsce +
            ".dat";

    //input data, pop characteristics: initial N
    ifstream file_init_pops_per_szgroup;
    file_init_pops_per_szgroup.open(filename.c_str());
    if (file_init_pops_per_szgroup.fail()) {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/init_pops_per_szgroup_biolsce1.dat";
        file_init_pops_per_szgroup.open(filename.c_str());
    }
    if (file_init_pops_per_szgroup.fail()) {
        cout << "Unfortunately the init_pops_per_szgroup_biolsceXX.dat vector is not informed " << "\n";
        cout << "You´ll have to stop the simu, correct input and re-run. " << "\n";
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> init_pops_per_szgroup;
    fill_multimap_from_specifications_i_d(file_init_pops_per_szgroup, init_pops_per_szgroup);
    file_init_pops_per_szgroup.close();
    // Here lies a deadly bug: remember very hard bug to find out due to non-unique pop names in the input files!
    // was creating access violation from pointers misuse, etc.

#ifdef VERBOSE
    // check input
    multimap<int,double>::iterator lower_init = init_pops_per_szgroup.lower_bound(0);
    multimap<int,double>::iterator upper_init = init_pops_per_szgroup.upper_bound(0);
    dout(cout << "initial N at szgroup for pop0: ");
    for (multimap<int, double>::iterator pos=lower_init; pos != upper_init; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << "\n");
#endif

    return (init_pops_per_szgroup);
}


multimap<int, double>
read_adults_diet_preference_per_stock_allstks(string folder_name_parameterization, string inputfolder, string biolsce)
{

    string filename = inputfolder + "/popsspe_" + folder_name_parameterization +
                      "/_adults_diet_preference_per_stock_allstks_biolsce" + biolsce + ".dat";

    //input data
    ifstream file_adults_diet_preference_per_stock_allstks;
    file_adults_diet_preference_per_stock_allstks.open(filename.c_str());
    if (file_adults_diet_preference_per_stock_allstks.fail()) {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization +
                   "/adults_diet_preference_per_stock_allstks_biolsce1.dat";
        file_adults_diet_preference_per_stock_allstks.open(filename.c_str());
    }
    if (file_adults_diet_preference_per_stock_allstks.fail()) {
        cout << "Unfortunately the adults_diet_preference_per_stock_allstks_biolsceXX.dat vector is not informed "
             << "\n";
        cout << "You´ll have to stop the simu, correct input and re-run. " << "\n";
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> adults_diet_preference_per_stock_allstks;
    fill_multimap_from_specifications_i_d(file_adults_diet_preference_per_stock_allstks,
                                          adults_diet_preference_per_stock_allstks);
    file_adults_diet_preference_per_stock_allstks.close();
    // Here lies a deadly bug: remember very hard bug to find out due to non-unique pop names in the input files!
    // was creating access violation from pointers misuse, etc.

#ifdef VERBOSE
    // check input
    multimap<int,double>::iterator lower_init = adults_diet_preference_per_stock_allstks.lower_bound(0);
    multimap<int,double>::iterator upper_init = adults_diet_preference_per_stock_allstks.upper_bound(0);
    dout(cout << "adults_diet_preference_per_stock_allstks for pop0: ");
    for (multimap<int, double>::iterator pos=lower_init; pos != upper_init; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << "\n");
#endif

    return (adults_diet_preference_per_stock_allstks);
}


multimap<int, double>
read_juveniles_diet_preference_per_stock_allstks(string folder_name_parameterization, string inputfolder,
                                                 string biolsce)
{

    string filename = inputfolder + "/popsspe_" + folder_name_parameterization +
                      "/_juveniles_diet_preference_per_stock_allstks_biolsce" + biolsce + ".dat";

    //input data
    ifstream file_juveniles_diet_preference_per_stock_allstks;
    file_juveniles_diet_preference_per_stock_allstks.open(filename.c_str());
    if (file_juveniles_diet_preference_per_stock_allstks.fail()) {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization +
                   "/juveniles_diet_preference_per_stock_allstks_biolsce1.dat";
        file_juveniles_diet_preference_per_stock_allstks.open(filename.c_str());
    }
    if (file_juveniles_diet_preference_per_stock_allstks.fail()) {
        cout << "Unfortunately the juveniles_diet_preference_per_stock_allstks_biolsceXX.dat vector is not informed "
             << "\n";
        cout << "You´ll have to stop the simu, correct input and re-run. " << "\n";
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> juveniles_diet_preference_per_stock_allstks;
    fill_multimap_from_specifications_i_d(file_juveniles_diet_preference_per_stock_allstks,
                                          juveniles_diet_preference_per_stock_allstks);
    file_juveniles_diet_preference_per_stock_allstks.close();
    // Here lies a deadly bug: remember very hard bug to find out due to non-unique pop names in the input files!
    // was creating access violation from pointers misuse, etc.

#ifdef VERBOSE
    // check input
    multimap<int,double>::iterator lower_init = juveniles_diet_preference_per_stock_allstks.lower_bound(0);
    multimap<int,double>::iterator upper_init = juveniles_diet_preference_per_stock_allstks.upper_bound(0);
    dout(cout << "juveniles_diet_preference_per_stock_allstks for pop0: ");
    for (multimap<int, double>::iterator pos=lower_init; pos != upper_init; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << "\n");
#endif

    return (juveniles_diet_preference_per_stock_allstks);
}


multimap<int, double>
read_init_prop_migrants_pops_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce)
{

    string filename =
            inputfolder + "/popsspe_" + folder_name_parameterization + "/init_prop_migrants_pops_per_szgroup_biolsce" +
            biolsce + ".dat";

    //input data, pop characteristics: initial prop migrants
    ifstream file_init_prop_migrants_pops_per_szgroup;
    file_init_prop_migrants_pops_per_szgroup.open(filename.c_str());
    if (file_init_prop_migrants_pops_per_szgroup.fail()) {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization +
                   "/init_prop_migrants_pops_per_szgroup_biolsce1.dat";
        file_init_prop_migrants_pops_per_szgroup.open(filename.c_str());
    }
    if (file_init_prop_migrants_pops_per_szgroup.fail()) {
        cout << "Unfortunately the init_prop_migrants_pops_per_szgroup_biolsceXX.dat vector is not informed " << "\n";
        cout << "You´ll have to stop the simu, correct input and re-run. " << "\n";
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> init_prop_migrants_pops_per_szgroup;
    fill_multimap_from_specifications_i_d(file_init_prop_migrants_pops_per_szgroup,
                                          init_prop_migrants_pops_per_szgroup);
    file_init_prop_migrants_pops_per_szgroup.close();
    // Here lies a deadly bug: remember very hard bug to find out due to non-unique pop names in the input files!
    // was creating access violation from pointers misuse, etc.

#ifdef VERBOSE
    // check input
    multimap<int,double>::iterator lower_init = init_prop_migrants_pops_per_szgroup.lower_bound(0);
    multimap<int,double>::iterator upper_init = init_prop_migrants_pops_per_szgroup.upper_bound(0);
    dout(cout << "initial N at szgroup for pop0: ");
    for (multimap<int, double>::iterator pos=lower_init; pos != upper_init; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << "\n");
#endif

    return (init_prop_migrants_pops_per_szgroup);
}


multimap<int, double>
read_init_maturity_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce)
{

    string filename =
            inputfolder + "/popsspe_" + folder_name_parameterization + "/init_maturity_per_szgroup_biolsce" + biolsce +
            ".dat";

    //input data, pop characteristics: maturity_at_szgroup
    ifstream file_init_maturity_per_szgroup;
    file_init_maturity_per_szgroup.open(filename.c_str());
    if (file_init_maturity_per_szgroup.fail()) {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/init_maturity_per_szgroup_biolsce1.dat";
        file_init_maturity_per_szgroup.open(filename.c_str());
    }
    if (file_init_maturity_per_szgroup.fail()) {
        cout << "Unfortunately the init_maturity_per_szgroup_biolsceXX.dat vector is not informed " << "\n";
        cout << "You´ll have to stop the simu, correct input and re-run. " << "\n";
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> init_maturity_per_szgroup;
    fill_multimap_from_specifications_i_d(file_init_maturity_per_szgroup, init_maturity_per_szgroup);
    file_init_maturity_per_szgroup.close();

#ifdef VERBOSE
    // check input
    multimap<int,double>::iterator lower_init_mat = init_maturity_per_szgroup.lower_bound(0);
    multimap<int,double>::iterator upper_init_mat = init_maturity_per_szgroup.upper_bound(0);
    dout(cout << "initial maturity at szgroup for pop0: ");
    for (multimap<int, double>::iterator pos=lower_init_mat; pos != upper_init_mat; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << "\n");
#endif

    return (init_maturity_per_szgroup);
}


multimap<int, double>
read_init_fecundity_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce)
{

    string filename =
            inputfolder + "/popsspe_" + folder_name_parameterization + "/init_fecundity_per_szgroup_biolsce" + biolsce +
            ".dat";

    //input data, pop characteristics: maturity_at_szgroup
    ifstream file_init_fecundity_per_szgroup;
    file_init_fecundity_per_szgroup.open(filename.c_str());
    if (file_init_fecundity_per_szgroup.fail()) {
        filename =
                inputfolder + "/popsspe_" + folder_name_parameterization + "/init_fecundity_per_szgroup_biolsce1.dat";
        file_init_fecundity_per_szgroup.open(filename.c_str());
    }
    if (file_init_fecundity_per_szgroup.fail()) {
        cout << "Unfortunately the init_fecundity_per_szgroup_biolsceXX.dat vector is not informed " << "\n";
        cout << "You´ll have to stop the simu, correct input and re-run. " << "\n";
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> init_fecundity_per_szgroup;
    fill_multimap_from_specifications_i_d(file_init_fecundity_per_szgroup, init_fecundity_per_szgroup);
    file_init_fecundity_per_szgroup.close();

#ifdef VERBOSE
    // check input
    multimap<int,double>::iterator lower_init_fec = init_fecundity_per_szgroup.lower_bound(0);
    multimap<int,double>::iterator upper_init_fec = init_fecundity_per_szgroup.upper_bound(0);
    dout(cout << "initial fecundity at szgroup for pop0: ");
    for (multimap<int, double>::iterator pos=lower_init_fec; pos != upper_init_fec; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << "\n");
#endif

    return (init_fecundity_per_szgroup);
}


multimap<int, double>
read_init_weight_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce)
{

    string filename =
            inputfolder + "/popsspe_" + folder_name_parameterization + "/init_weight_per_szgroup_biolsce" + biolsce +
            ".dat";

    //input data, pop characteristics: weight_at_szgroup
    ifstream file_init_weight_per_szgroup;
    file_init_weight_per_szgroup.open(filename.c_str());
    if (file_init_weight_per_szgroup.fail()) {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/init_weight_per_szgroup_biolsce1.dat";
        file_init_weight_per_szgroup.open(filename.c_str());
    }
    if (file_init_weight_per_szgroup.fail()) {
        cout << "Unfortunately the init_weight_per_szgroup_biolsceXX.dat vector is not informed " << "\n";
        cout << "You´ll have to stop the simu, correct input and re-run. " << "\n";
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> init_weight_per_szgroup;
    fill_multimap_from_specifications_i_d(file_init_weight_per_szgroup, init_weight_per_szgroup);
    file_init_weight_per_szgroup.close();

#ifdef VERBOSE
    // check input
    multimap<int,double>::iterator lower_init_we = init_weight_per_szgroup.lower_bound(0);
    multimap<int,double>::iterator upper_init_we = init_weight_per_szgroup.upper_bound(0);
    dout(cout << "initial weight at szgroup for pop0: ");
    for (multimap<int, double>::iterator pos=lower_init_we; pos != upper_init_we; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << "\n");
#endif

    return (init_weight_per_szgroup);
}


multimap<int, int> read_init_comcat_per_szgroup(string folder_name_parameterization, string inputfolder)
{

    string filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/comcat_per_szgroup_done_by_hand.dat";

    //input data, pop characteristics: weight_at_szgroup
    ifstream file_comcat_per_szgroup;
    file_comcat_per_szgroup.open(filename.c_str());
    if (file_comcat_per_szgroup.fail()) {
        cout << "Unfortunately the comcat_per_szgroup_done_by_hand.dat vector is not informed " << "\n";
        cout << "You´ll have to stop the simu, correct input and re-run. " << "\n";
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, int> comcat_per_szgroup;
    fill_multimap_from_specifications_i_i(file_comcat_per_szgroup, comcat_per_szgroup);
    file_comcat_per_szgroup.close();

    /*
    // check input
    multimap<int,int>::iterator lower_init_we = comcat_per_szgroup.lower_bound(0);
    multimap<int,int>::iterator upper_init_we = comcat_per_szgroup.upper_bound(0);
    cout << "comcat at szgroup for pop0: ";
    for (multimap<int, int>::iterator pos=lower_init_we; pos != upper_init_we; pos++)
    {
        cout << pos->second << " ";
    }
    cout << "\n";
    */

    return (comcat_per_szgroup);
}


multimap<int, double> read_init_M_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce)
{

    string filename =
            inputfolder + "/popsspe_" + folder_name_parameterization + "/init_M_per_szgroup_biolsce" + biolsce + ".dat";

    //input data, pop characteristics: weight_at_szgroup
    ifstream file_init_M_per_szgroup;
    file_init_M_per_szgroup.open(filename.c_str());
    if (file_init_M_per_szgroup.fail()) {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/init_M_per_szgroup_biolsce1.dat";
        file_init_M_per_szgroup.open(filename.c_str());
    }
    if (file_init_M_per_szgroup.fail()) {
        cout << "Unfortunately the init_M_per_szgroup_biolsceXX.dat vector is not informed " << "\n";
        cout << "You´ll have to stop the simu, correct input and re-run. " << "\n";
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> init_M_per_szgroup;
    fill_multimap_from_specifications_i_d(file_init_M_per_szgroup, init_M_per_szgroup);
    file_init_M_per_szgroup.close();

#ifdef VERBOSE
    // check input
    multimap<int,double>::iterator lower_init_M = init_M_per_szgroup.lower_bound(0);
    multimap<int,double>::iterator upper_init_M = init_M_per_szgroup.upper_bound(0);
    dout(cout << "initial M at szgroup for pop 0: ");
    for (multimap<int, double>::iterator pos=lower_init_M; pos != upper_init_M; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << "\n");
#endif

    return (init_M_per_szgroup);
}


multimap<int, double>
read_beta_ssm_szgroup(string folder_name_parameterization, string inputfolder, string biolsce)
{

    string filename =
        inputfolder + "/popsspe_" + folder_name_parameterization + "/beta_ssm_biolsce" + biolsce + ".dat";

    //input data, pop characteristics: beta_ssm
    ifstream file_beta_ssm_per_szgroup;
    file_beta_ssm_per_szgroup.open(filename.c_str());
    if (file_beta_ssm_per_szgroup.fail()) {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/beta_ssm_biolsce1.dat";
        file_beta_ssm_per_szgroup.open(filename.c_str());
    }
    if (file_beta_ssm_per_szgroup.fail()) {
        cout << "Unfortunately the beta_ssm_per_szgroup_biolsceXX.dat vector is not informed " << "\n";
        cout << "Required by the dyn sizeSpectra Option. If this option is activated, you´ll have to fix this problem: stop the simu, correct input and re-run. " << "\n";
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        //exit(-1);
        multimap<int, double> empty;
        return empty;
    }
    multimap<int, double> beta_ssm_per_szgroup;
    fill_multimap_from_specifications_i_d(file_beta_ssm_per_szgroup, beta_ssm_per_szgroup);
    file_beta_ssm_per_szgroup.close();

#ifdef VERBOSE
    // check input
    multimap<int, double>::iterator lower_beta_ssm = beta_ssm_per_szgroup.lower_bound(0);
    multimap<int, double>::iterator upper_beta_ssm = beta_ssm_per_szgroup.upper_bound(0);
    dout(cout << "beta_ssm at szgroup for pop 0: ");
    for (multimap<int, double>::iterator pos = lower_beta_ssm; pos != upper_beta_ssm; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << "\n");
#endif

    return (beta_ssm_per_szgroup);
}


multimap<int, double>
read_background_mortality_szgroup(string folder_name_parameterization, string inputfolder, string biolsce)
{

    string filename =
        inputfolder + "/popsspe_" + folder_name_parameterization + "/background_mortality_biolsce" + biolsce + ".dat";

    //input data, pop characteristics: background_mortality
    ifstream file_background_mortality_per_szgroup;
    file_background_mortality_per_szgroup.open(filename.c_str());
    if (file_background_mortality_per_szgroup.fail()) {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/background_mortality_biolsce1.dat";
        file_background_mortality_per_szgroup.open(filename.c_str());
    }
    if (file_background_mortality_per_szgroup.fail()) {
        cout << "Unfortunately the background_mortality_per_szgroup_biolsceXX.dat vector is not informed " << "\n";
        cout << "Required by the dyn sizeSpectra Option. If this option is activated, you´ll have to fix this problem: stop the simu, correct input and re-run. " << "\n";
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        // exit(-1);
        multimap<int, double> empty;
        return empty;
    }
    multimap<int, double> background_mortality_per_szgroup;
    fill_multimap_from_specifications_i_d(file_background_mortality_per_szgroup, background_mortality_per_szgroup);
    file_background_mortality_per_szgroup.close();

#ifdef VERBOSE
    // check input
    multimap<int, double>::iterator lower_background_mortality = background_mortality_per_szgroup.lower_bound(0);
    multimap<int, double>::iterator upper_background_mortality = background_mortality_per_szgroup.upper_bound(0);
    dout(cout << "beta_ssm at szgroup for pop 0: ");
    for (multimap<int, double>::iterator pos = lower_background_mortality; pos != upper_background_mortality; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << "\n");
#endif

    return (background_mortality_per_szgroup);
}


multimap<int, double>
read_init_proprecru_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce)
{

    string filename =
            inputfolder + "/popsspe_" + folder_name_parameterization + "/init_proprecru_per_szgroup_biolsce" + biolsce +
            ".dat";

    //input data, pop characteristics: weight_at_szgroup
    ifstream file_init_proprecru_per_szgroup;
    file_init_proprecru_per_szgroup.open(filename.c_str());
    if (file_init_proprecru_per_szgroup.fail()) {
        filename =
                inputfolder + "/popsspe_" + folder_name_parameterization + "/init_proprecru_per_szgroup_biolsce1.dat";
        file_init_proprecru_per_szgroup.open(filename.c_str());
    }
    if (file_init_proprecru_per_szgroup.fail()) {
        cout << "Unfortunately the init_proprecru_per_szgroup_biolsceXX.dat vector is not informed " << "\n";
        cout << "You´ll have to stop the simu, correct input and re-run. " << "\n";
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> init_proprecru_per_szgroup;
    fill_multimap_from_specifications_i_d(file_init_proprecru_per_szgroup, init_proprecru_per_szgroup);
    file_init_proprecru_per_szgroup.close();

#ifdef VERBOSE
    // check input
    multimap<int,double>::iterator lower_init_proprecru = init_proprecru_per_szgroup.lower_bound(0);
    multimap<int,double>::iterator upper_init_proprecru = init_proprecru_per_szgroup.upper_bound(0);
    dout(cout << "initial proprecru at szgroup for pop 0: ");
    for (multimap<int, double>::iterator pos=lower_init_proprecru; pos != upper_init_proprecru; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << "\n");
#endif

    return (init_proprecru_per_szgroup);
}


multimap<int, types::NodeId>
read_lst_idx_nodes_per_pop(string a_semester, string folder_name_parameterization, string inputfolder,
                           string str_rand_avai_file)
{
    
    
    string filename;
    if (str_rand_avai_file == "baseline") {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/static_avai/" +
                   "lst_idx_nodes_per_pop_semester" + a_semester + ".dat";
    } else {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/static_avai/" +
                   "lst_idx_nodes_per_pop_semester" + a_semester + ".dat";
        // filename=  "popsspe_"+folder_name_parameterization+"/stochast_avai/"+"lst_idx_nodes_per_pop_semester"+a_semester+"_"+str_rand_avai_file+".dat";
    }

    //input data, pop characteristics: lst idx nodes with presence
    ifstream file_lst_idx_nodes_per_pop;
    file_lst_idx_nodes_per_pop.open(filename.c_str());
    if (file_lst_idx_nodes_per_pop.fail()) {
        cout << "Unfortunately the lst_idx_nodes_per_pop_.dat vector is not informed  " << "\n";
        cout << "You´ll have to stop the simu, correct input and re-run. " << "\n";
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, int> lst_idx_nodes_per_pop;
    fill_multimap_from_specifications_i_i(file_lst_idx_nodes_per_pop, lst_idx_nodes_per_pop);
    file_lst_idx_nodes_per_pop.close();

#ifdef VERBOSE
    // check input
    multimap<int,int>::iterator lower_idx = lst_idx_nodes_per_pop.lower_bound(0);
    multimap<int,int>::iterator upper_idx = lst_idx_nodes_per_pop.upper_bound(0);
    dout(cout << "lst idx nodes for pop 0: ");
    for (multimap<int, int>::iterator pos=lower_idx; pos != upper_idx; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << "\n");
#endif

    // TODO (fba#5#): remove possible replicates in the list of nodes per pop

    return (types::helpers::toValueIdMultimap<types::NodeId>(lst_idx_nodes_per_pop));
}


multimap<int, int> read_selected_szgroups_per_pop(string folder_name_parameterization, string inputfolder)
{

    string filename;
    filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" + "the_selected_szgroups.dat";

    //input data, pop characteristics: lst idx nodes with presence
    ifstream file_the_selected_szgroups;
    file_the_selected_szgroups.open(filename.c_str());
    if (file_the_selected_szgroups.fail()) {
        cout << "Unfortunately the the_selected_szgroups.dat vector is not informed " << "\n";
        cout << "You´ll have to stop the simu, correct input and re-run. " << "\n";
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, int> the_selected_szgroups;
    fill_multimap_from_specifications_i_i(file_the_selected_szgroups, the_selected_szgroups);
    file_the_selected_szgroups.close();

    /*
    // check input
    multimap<int,int>::iterator lower_idx = the_selected_szgroups.lower_bound(0);
    multimap<int,int>::iterator upper_idx = the_selected_szgroups.upper_bound(0);
    cout << "the_selected_szgroups for pop 0: ";
    for (multimap<int, int>::iterator pos=lower_idx; pos != upper_idx; pos++)
    {
        cout << pos->second << " ";
    }
    cout << "\n";*/

    return (the_selected_szgroups);
}

map<int, int> read_tac_percent_simulated(string folder_name_parameterization, string inputfolder)
{

    string filename =
            inputfolder + "/popsspe_" + folder_name_parameterization + "/percent_landings_from_simulated_vessels.dat";

    //input data, pop characteristics: percent_landings
    ifstream file_tac_percent_simulated;
    file_tac_percent_simulated.open(filename.c_str());
    if (file_tac_percent_simulated.fail()) {
        cout << "Unfortunately the percent_landings_from_simulated_vessels.dat vector is not informed" << "\n";
        cout << "You´ll have to stop the simu, correct input and re-run. " << "\n";
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    map<int, int> tac_percent_simulated;
    fill_map_from_specifications_i_i(file_tac_percent_simulated, tac_percent_simulated);
    file_tac_percent_simulated.close();

#ifdef VERBOSE
    // check input
    for ( map<int,int>::iterator it=tac_percent_simulated.begin() ; it != tac_percent_simulated.end(); it++ )
        dout(cout << (*it).first << " => " << (*it).second << "\n");
#endif

    return (tac_percent_simulated);
}


map<int, double> read_hyperstability_param(string folder_name_parameterization, string inputfolder)
{

    string filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/hyperstability_param.dat";

    //input data, pop characteristics: hyperstability
    ifstream file_hyperstability_param;
    file_hyperstability_param.open(filename.c_str());
    if (file_hyperstability_param.fail()) {
        cout << "Unfortunately the hyperstability_param.dat vector is not informed " << "\n";
        cout << "You´ll have to stop the simu, correct input and re-run. " << "\n";
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    map<int, double> hyperstability_param;
    fill_map_from_specifications_i_d(file_hyperstability_param, hyperstability_param, folder_name_parameterization);
    file_hyperstability_param.close();

#ifdef VERBOSE
    // check input
    for ( map<int,double>::iterator it=hyperstability_param.begin() ; it != hyperstability_param.end(); it++ )
        dout(cout << (*it).first << " => " << (*it).second << "\n");
#endif

    return (hyperstability_param);
}


map<types::NodeId, double>
read_oth_land_nodes_with_pop(string a_semester, string a_month, int a_pop, string folder_name_parameterization,
                             string inputfolder, string fleetsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename;
    if (fleetsce == "") {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" + a_pop_s +
                   "spe_stecf_oth_land_per_month_per_node_semester" + a_semester + ".dat";
    } else {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" + a_pop_s +
                   "spe_stecf_oth_land_per_month_per_node_month" + a_month + "_fleetsce" + fleetsce + ".dat";
    }

    ifstream file_oth_land;
    file_oth_land.open(filename.c_str());
    if (file_oth_land.fail()) {
        cout << "Unfortunately the spe_stecf_oth_land_per_month_per_node_ vector is not informed for stock " << a_pop_s
             << "\n";
        cout << "You´ll have to stop the simu, correct input and re-run. " << "\n";
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    map<types::NodeId, double> oth_land;
    fill_from_oth_land(file_oth_land, oth_land);
    file_oth_land.close();

#ifdef VERBOSE
    // check input
    map<int,double>::iterator pos;
    dout(cout << " oth_land " << "\n");
    for (pos=oth_land.begin(); pos != oth_land.end(); pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << "\n");
#endif

    return (oth_land);
}



int read_oth_land_map_per_met_on_nodes(map<types::NodeId, double>& oth_land, string a_semester, string a_month, int a_pop, int a_met, string folder_name_parameterization,
    string inputfolder, string fleetsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();
    stringstream out2;
    out2 << a_met;
    string a_met_s = out2.str();

    string filename;
    filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/other_landings_on_node_per_met_pop/" + a_pop_s +
            "spe_oth_land_met" + a_met_s + "_month" + a_month + "_fleetsce" + fleetsce + ".dat";
    //cout << "looking for ..." << filename << "\n";

    ifstream file_oth_land;
    file_oth_land.open(filename.c_str());
    if (file_oth_land.fail()) {
        return(-1);
    }
   
    fill_from_oth_land(file_oth_land, oth_land);
    file_oth_land.close();

    //cout << "closing ..." << filename << "\n";

#ifdef VERBOSE
    // check input
    map<int, double>::iterator pos;
    dout(cout << " oth_land " << "\n");
    for (pos = oth_land.begin(); pos != oth_land.end(); pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << "\n");
#endif

    return (0);
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
        cout << "Unfortunately the selectivity_per_stock_ogives_for_oth_land is not informed..."  << "\n";
        cout << "You´ll have to stop the simu, correct input in metiersspe and re-run. " << "\n";
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";
        exit(-1);
        //   return selectivity_per_stock_ogives_for_oth_land; // caution: returns an empty object
    }
    vector<vector<double> > selectivity_per_stock_ogives_for_oth_land(nbpops, vector<double>(nbszgroup));
    if (!fill_in_selectivity_per_stock(file_selectivity_per_stock_ogives_for_oth_land,
        selectivity_per_stock_ogives_for_oth_land)) {
        throw std::runtime_error("Error while executing: fill_in_selectivity_per_stock");
    }

    file_selectivity_per_stock_ogives_for_oth_land.close();

    return (selectivity_per_stock_ogives_for_oth_land);
}




multimap<int, double>
read_overall_migration_fluxes(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder,
                              string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" + a_pop_s +
                      "overall_migration_fluxes_semester" + a_semester + "_biolsce" + biolsce + ".dat";

    ifstream file_overall_migration_fluxes;
    file_overall_migration_fluxes.open(filename.c_str());
    if (file_overall_migration_fluxes.fail()) {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" + a_pop_s +
                   "overall_migration_fluxes_semester" + a_semester + "_biolsce1.dat";
        file_overall_migration_fluxes.open(filename.c_str());
        cout << "biolsce ignored for " << filename << "\n";
    }
    if (file_overall_migration_fluxes.fail()) {
        cout << "Unfortunately the overall_migration_fluxes_ vector is not informed for stock " << a_pop_s << "\n";
        cout << "You´ll have to stop the simu, correct input and re-run. " << "\n";
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> overall_migration_fluxes;
    fill_from_overall_migration_fluxes(file_overall_migration_fluxes, overall_migration_fluxes);
    file_overall_migration_fluxes.close();

#ifdef VERBOSE
    // check input
    multimap<int,double>::iterator pos;
    dout(cout << " overall_migration_fluxes " << "\n");
    for (pos=overall_migration_fluxes.begin(); pos != overall_migration_fluxes.end(); pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << "\n");
#endif

    return (overall_migration_fluxes);
}


map<string, double>
read_relative_stability_keys(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename;
    if (folder_name_parameterization == "final") {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" +
                   a_pop_s + "ctrysspe_relative_stability_semester" + a_semester + ".dat";
    } else {
        //=> NEW_VERSION: replaced by:
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" +
                   a_pop_s + "ctrysspe_relative_stability_semester" + a_semester + ".dat";
    }

    ifstream file_relative_stability;
    file_relative_stability.open(filename.c_str());
    if (file_relative_stability.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    map<string, double> relative_stability;
    fill_from_relative_stability(file_relative_stability, relative_stability);
    file_relative_stability.close();

#ifdef VERBOSE
    // check input
    map<string,double>::iterator pos;
    dout(cout << " relative_stability " << "\n");
    for (pos=relative_stability.begin(); pos != relative_stability.end(); pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << "\n");
#endif

    return (relative_stability);
}


map<int, double>
read_percent_tac_per_vessel_length_class(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename;
    if (folder_name_parameterization == "final") {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" +
            a_pop_s + "vesselslengthspe_percent_tac_per_vessel_length_class.dat";
    }
    else {
        //=> NEW_VERSION: replaced by:
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" +
            a_pop_s + "vesselslengthspe_percent_tac_per_vessel_length_class.dat";
    }

    ifstream file_percent_tac_per_vessel_length_class;
    file_percent_tac_per_vessel_length_class.open(filename.c_str());
    if (file_percent_tac_per_vessel_length_class.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        //exit(-1);
         // back compatibility: if not informed then fill out with 100% 
        map<int, double> percent_tac_per_vessel_length_class;
        percent_tac_per_vessel_length_class[0] = 100;
        percent_tac_per_vessel_length_class[1] = 100;
        percent_tac_per_vessel_length_class[2] = 100;
        percent_tac_per_vessel_length_class[3] = 100;
        percent_tac_per_vessel_length_class[4] = 100;
        return(percent_tac_per_vessel_length_class);

    }
    map<int, double> percent_tac_per_vessel_length_class;
    fill_from_percent_tac_per_vessel_length_class(file_percent_tac_per_vessel_length_class,
                                                   percent_tac_per_vessel_length_class,
                                                   folder_name_parameterization);
    file_percent_tac_per_vessel_length_class.close();

#ifdef VERBOSE
    // check input
    map<string, double>::iterator pos;
    dout(cout << " percent_tac_per_vessel_length_class " << "\n");
    for (pos = percent_tac_per_vessel_length_class.begin(); pos != percent_tac_per_vessel_length_class.end(); pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << "\n");
#endif

    return (percent_tac_per_vessel_length_class);
}



map<int, double>
read_percent_tac_cumul_over_months_keys(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename;
    if (folder_name_parameterization == "final") {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" +
            a_pop_s + "spe_percent_tac_cumul_over_months_key.dat";
    }
    else {
        //=> NEW_VERSION: replaced by:
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" +
            a_pop_s + "spe_percent_tac_cumul_over_months_key.dat";
    }

    ifstream file_percent_tac_cumul_over_months_key;
    file_percent_tac_cumul_over_months_key.open(filename.c_str());
    if (file_percent_tac_cumul_over_months_key.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        //exit(-1);
        // back compatibility: if not informed then fill out with 100% 
        map<int, double> percent_tac_cumul_over_months_key;
        percent_tac_cumul_over_months_key[1] = 100;
        percent_tac_cumul_over_months_key[2] = 100;
        percent_tac_cumul_over_months_key[3] = 100;
        percent_tac_cumul_over_months_key[4] = 100;
        percent_tac_cumul_over_months_key[5] = 100;
        percent_tac_cumul_over_months_key[6] = 100;
        percent_tac_cumul_over_months_key[7] = 100;
        percent_tac_cumul_over_months_key[8] = 100;
        percent_tac_cumul_over_months_key[9] = 100;
        percent_tac_cumul_over_months_key[10] = 100;
        percent_tac_cumul_over_months_key[11] = 100;
        percent_tac_cumul_over_months_key[12] = 100;
        return(percent_tac_cumul_over_months_key);

    }
    map<int, double> percent_tac_cumul_over_months_key;
    fill_from_percent_tac_cumul_over_months_key(file_percent_tac_cumul_over_months_key,
                                                   percent_tac_cumul_over_months_key, 
                                                   folder_name_parameterization);

    
    file_percent_tac_cumul_over_months_key.close();

#ifdef VERBOSE
    // check input
    map<string, double>::iterator pos;
    dout(cout << " percent_tac_cumul_over_months_key " << "\n");
    for (pos = percent_tac_cumul_over_months_key.begin(); pos != percent_tac_cumul_over_months_key.end(); pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << "\n");
#endif

    return (percent_tac_cumul_over_months_key);
}


/* []DEPRECATED
multimap<types::NodeId, double> read_avai_szgroup_nodes_with_pop(string a_semester,
                                                                 int a_pop, string folder_name_parameterization,
                                                                 string inputfolder,
                                                                 string str_rand_avai_file,
                                                                 vector<string> type_of_avai_field_to_read)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string a_type_of_avai_field_to_read = type_of_avai_field_to_read.at(a_pop);
    //cout << "a_type_of_avai_field_to_read is " << a_type_of_avai_field_to_read << "\n";

    string filename;
    if (str_rand_avai_file == "baseline") {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/static_avai/" +
                   a_pop_s + "spe_avai_szgroup_nodes_semester" + a_semester + a_type_of_avai_field_to_read + ".dat";
    } else {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/stochast_avai/" +
                   a_pop_s + "spe_avai_szgroup_nodes_semester" + a_semester + a_type_of_avai_field_to_read + ".dat";
    }

    ifstream file_avai_szgroup_nodes_with_pop;
    file_avai_szgroup_nodes_with_pop.open(filename.c_str());
    if (file_avai_szgroup_nodes_with_pop.fail()) {
        cout << "Unfortunately the availability vector is not informed for stock " << a_pop_s << "\n";
        cout << "You´ll have to stop the simu, correct input and re-run. " << "\n";
        cout
                << "This could happen if the stock is missing in the displace_input_for_data_merger_xx.dat along the avai_updater_on Option"
                << "\n";
        cout
                << "this might also result from an attempt to use an empty vmslikefpingsonly.dat file (empty if no ref. vessels defined)"
                << "\n";
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<types::NodeId, double> avai_szgroup_nodes_with_pop;
    fill_from_avai_szgroup_nodes_with_pop(file_avai_szgroup_nodes_with_pop, avai_szgroup_nodes_with_pop);
    file_avai_szgroup_nodes_with_pop.close();

#ifdef VERBOSE
    // check input
    //node 1600
    multimap<int,double>::iterator lower = avai_szgroup_nodes_with_pop.lower_bound(1600);
    multimap<int,double>::iterator upper = avai_szgroup_nodes_with_pop.upper_bound(1600);
    dout(cout << "avai on node 1600: ");
    for (multimap<int, double>::iterator pos=lower; pos != upper; pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << "\n");
#endif

    // TODO (fba#5#): check avai sum to 1 for a given szgroup

    return (avai_szgroup_nodes_with_pop);
}
*/


multimap<types::NodeId, double> read_full_avai_szgroup_nodes_with_pop(string a_semester, int a_pop,
                                                                      string folder_name_parameterization,
                                                                      string inputfolder,
                                                                      string str_rand_avai_file,
                                                                      vector<string> type_of_avai_field_to_read,
                                                                      string avai_folder_addon_name)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string a_type_of_avai_field_to_read = type_of_avai_field_to_read.at(a_pop);

    string filename;
    if (str_rand_avai_file == "baseline") {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/static_avai"+ avai_folder_addon_name +"/" +
                   a_pop_s + "spe_full_avai_szgroup_nodes_semester" + a_semester + a_type_of_avai_field_to_read +
                   ".dat";
    } else {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/stochast_avai/" +
                   a_pop_s + "spe_full_avai_szgroup_nodes_semester" + a_semester + "_" + str_rand_avai_file +
                   a_type_of_avai_field_to_read + ".dat";
    }

    ifstream file_avai_szgroup_nodes_with_pop;
    file_avai_szgroup_nodes_with_pop.open(filename.c_str());
    if (file_avai_szgroup_nodes_with_pop.fail()) {
    
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/static_avai"+ avai_folder_addon_name +"/" +
            a_pop_s + "spe_full_avai_szgroup_nodes_semester" + a_semester +
            ".dat"; // default naming
        file_avai_szgroup_nodes_with_pop.open(filename.c_str());

        if (file_avai_szgroup_nodes_with_pop.fail()) {
            string error_msg = "error opening file " + filename;
            cout << error_msg << "\n";

            exit(-1);
        }
    
    }

    dout(cout << "filename for pop  " << a_pop << " is " << filename << "\n";)
    cout << "filename for pop " << a_pop << " avai is " << filename << "\n";

    multimap<types::NodeId, double> full_avai_szgroup_nodes_with_pop;
    if (!fill_from_avai_szgroup_nodes_with_pop(file_avai_szgroup_nodes_with_pop, full_avai_szgroup_nodes_with_pop)) {
        throw std::runtime_error("Error while executing: fill_from_avai_szgroup_nodes_with_pop");
    }

    file_avai_szgroup_nodes_with_pop.close();

    // check input
    //cout << "avai on node 9510: ";
    //for(auto iter=full_avai_szgroup_nodes_with_pop.begin(); iter != full_avai_szgroup_nodes_with_pop.end();
    //    iter = full_avai_szgroup_nodes_with_pop.upper_bound( iter->first ) )
    //{
    //    cout << iter->first.toIndex() <<" : " << iter->second << " - ";
    //}
    //cout << "\n";
    // TODO (fba#5#): check avai sum to 1 for a given szgroup

    return (full_avai_szgroup_nodes_with_pop);
}


multimap<types::NodeId, double> read_field_of_coeff_diffusion_this_pop(string a_semester, int a_pop,
                                                                       string folder_name_parameterization,
                                                                       string inputfolder,
                                                                       string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename;
    filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/static_avai/" + a_pop_s +
               "spe_field_of_coeff_diffusion_this_pop_nodes_semester" + a_semester + "_biolsce" + biolsce + ".dat";

    ifstream file_field_of_coeff_diffusion_this_pop;
    file_field_of_coeff_diffusion_this_pop.open(filename.c_str());
    if (file_field_of_coeff_diffusion_this_pop.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<types::NodeId, double> field_of_coeff_diffusion_this_pop;
    if (!fill_field_of_coeff_diffusion_this_pop(file_field_of_coeff_diffusion_this_pop,
                                                field_of_coeff_diffusion_this_pop)) {
        throw std::runtime_error("Error while executing: fill_field_of_coeff_diffusion_this_pop");
    }

    file_field_of_coeff_diffusion_this_pop.close();


    return (field_of_coeff_diffusion_this_pop);
}


vector<vector<double> >
read_percent_szgroup_per_age_matrix(int a_pop, int nbszgroup, int nbage, string folder_name_parameterization,
                                    string inputfolder, string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" + a_pop_s +
                      "spe_percent_szgroup_per_age_biolsce" + biolsce + ".dat";

    ifstream file_percent_szgroup_per_age_matrix;
    file_percent_szgroup_per_age_matrix.open(filename.c_str());
    if (file_percent_szgroup_per_age_matrix.fail()) {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" + a_pop_s +
                   "spe_percent_szgroup_per_age_biolsce1.dat";
        file_percent_szgroup_per_age_matrix.open(filename.c_str());
        cout << "biolsce ignored for " << filename << "\n";
    }
    if (file_percent_szgroup_per_age_matrix.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    vector<vector<double> > percent_szgroup_per_age_matrix(nbszgroup, vector<double>(nbage));
    if (!fill_in_percent_szgroup_per_age_matrix(file_percent_szgroup_per_age_matrix, percent_szgroup_per_age_matrix)) {
        throw std::runtime_error("Error while executuing: fill_in_percent_szgroup_per_age_matrix");
    }

    file_percent_szgroup_per_age_matrix.close();

    return (percent_szgroup_per_age_matrix);
}


vector<vector<double> >
read_percent_age_per_szgroup_matrix(int a_pop, int nbszgroup, int nbage, string folder_name_parameterization,
                                    string inputfolder, string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" + a_pop_s +
                      "spe_percent_age_per_szgroup_biolsce" + biolsce + ".dat";

    ifstream file_percent_age_per_szgroup_matrix;
    file_percent_age_per_szgroup_matrix.open(filename.c_str());
    if (file_percent_age_per_szgroup_matrix.fail()) {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" + a_pop_s +
                   "spe_percent_age_per_szgroup_biolsce1.dat";
        file_percent_age_per_szgroup_matrix.open(filename.c_str());
        cout << "biolsce ignored for " << filename << "\n";
    }
    if (file_percent_age_per_szgroup_matrix.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    vector<vector<double> > percent_age_per_szgroup_matrix(nbszgroup, vector<double>(nbage));
    if (!fill_in_percent_age_per_szgroup_matrix(file_percent_age_per_szgroup_matrix, percent_age_per_szgroup_matrix)) {
        throw std::runtime_error("Error while executuing: fill_in_percent_age_per_szgroup_matrix");
    }

    file_percent_age_per_szgroup_matrix.close();

    return (percent_age_per_szgroup_matrix);
}


vector<vector<double> >
read_growth_transition_matrix(int a_pop, int nbszgroup, string folder_name_parameterization, string inputfolder,
                              string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" + a_pop_s +
                      "spe_size_transition_matrix_biolsce" + biolsce + ".dat";

    ifstream file_size_transition_matrix;
    file_size_transition_matrix.open(filename.c_str());
    if (file_size_transition_matrix.fail()) {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" + a_pop_s +
                   "spe_size_transition_matrix_biolsce1.dat";
        file_size_transition_matrix.open(filename.c_str());
        cout << "biolsce ignored for " << filename << "\n";
    }
    if (file_size_transition_matrix.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    vector<vector<double> > growth_transition_matrix(nbszgroup, vector<double>(nbszgroup));
    if (!fill_in_growth_transition(file_size_transition_matrix, growth_transition_matrix)) {
        throw std::runtime_error("Error while executuing: fill_in_growth_trransition_matrix");
    }

    file_size_transition_matrix.close();

    return (growth_transition_matrix);
}


vector<vector<double> >
read_preferences_for_species_matrix(int a_pop, int nbpops, int nbszgroup, string folder_name_parameterization,
                                    string inputfolder, string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" + a_pop_s +
                      "preferences_for_species_matrix_biolsce" + biolsce + ".dat";

    ifstream file_preferences_for_species;
    file_preferences_for_species.open(filename.c_str());
    if (file_preferences_for_species.fail()) {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" + a_pop_s +
                   "preferences_for_species_matrix_biolsce1.dat";
        file_preferences_for_species.open(filename.c_str());
        cout << "biolsce ignored for " << filename << "\n";
    }
    if (file_preferences_for_species.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    vector<vector<double> > preferences_for_species_matrix(nbpops, vector<double>(nbszgroup));
    if (!fill_in_preferences_for_species_matrix(file_preferences_for_species, preferences_for_species_matrix)) {
        throw std::runtime_error("Error while executing: fill_in_preferences_for_species_matrix");
    }

    file_preferences_for_species.close();

    return (preferences_for_species_matrix);
}


vector<vector<double> >
read_species_interactions_mortality_proportion_matrix(int nbpops, string folder_name_parameterization,
                                                      string inputfolder, string biolsce)
{
    // casting a_pop into a string
    string filename = inputfolder + "/popsspe_" + folder_name_parameterization +
                      "/species_interactions_mortality_proportion_matrix_biolsce" + biolsce + ".dat";

    ifstream file_species_interactions;
    file_species_interactions.open(filename.c_str());
    if (file_species_interactions.fail()) {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization +
                   "/species_interactions_mortality_proportion_matrix_biolsce1.dat";
        file_species_interactions.open(filename.c_str());
        cout << "biolsce ignored for " << filename << "\n";
    }
    if (file_species_interactions.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    vector<vector<double> > species_interactions_mortality_proportion_matrix(nbpops, vector<double>(nbpops));
    if (!fill_in_species_interactions_mortality_proportion_matrix(file_species_interactions,
                                                                  species_interactions_mortality_proportion_matrix)) {
        throw std::runtime_error("Error while executuing: fill_in_species_interactions_mortality_proportion_matrix");
    }

    file_species_interactions.close();

    return (species_interactions_mortality_proportion_matrix);
}

vector<double> read_param_sr(int a_pop, string folder_name_parameterization, string inputfolder, string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename =
            inputfolder + "/popsspe_" + folder_name_parameterization + "/" + a_pop_s + "spe_SSB_R_parameters_biolsce" +
            biolsce + ".dat";

    ifstream file_param_sr;
    file_param_sr.open(filename.c_str());
    if (file_param_sr.fail()) {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" + a_pop_s +
                   "spe_SSB_R_parameters_biolsce1.dat";
        file_param_sr.open(filename.c_str());
        cout << "biolsce ignored for " << filename << "\n";
    }
    if (file_param_sr.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    vector<double> param_sr(3);
    if (!fill_in_param_sr(file_param_sr, param_sr)) {
        throw std::runtime_error("Error while executuing: fill_in_param_sr");
    }

    file_param_sr.close();

    return (param_sr);
}

vector<double> read_initial_tac(int a_pop, string folder_name_parameterization, string inputfolder)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" + a_pop_s + "spe_initial_tac.dat";

    ifstream file_initial_tac;
    file_initial_tac.open(filename.c_str());
    if (file_initial_tac.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    vector<double> initial_tac(1);
    if (!fill_in_initial_tac(file_initial_tac, initial_tac)) {
        throw std::runtime_error("Error while executuing: fill_in_initial_tac");
    }
    file_initial_tac.close();

    return (initial_tac);
}


vector<double>
read_fbar_ages_min_max_and_ftarget(int a_pop, string folder_name_parameterization, string inputfolder, string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" + a_pop_s +
                      "spe_fbar_amin_amax_ftarget_Fpercent_TACpercent_biolsce" + biolsce + ".dat";

    ifstream file_fbar_ages_min_max;
    file_fbar_ages_min_max.open(filename.c_str());
    if (file_fbar_ages_min_max.fail()) {
        filename = inputfolder + "/popsspe_" + folder_name_parameterization + "/" + a_pop_s +
                   "spe_fbar_amin_amax_ftarget_Fpercent_TACpercent.dat";
        file_fbar_ages_min_max.open(filename.c_str());
        cout << "biolsce ignored for " << filename << "\n";
    }
    if (file_fbar_ages_min_max.fail()) {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    // i.e. 8 columns: F min age, F max age, LTMP ftarget, Fpercent, TACpercent, Btrigger, F-MSY value, F-MSY-low, F-MSY-up
    vector<double> fbar_ages_min_max(9);
    if (!fill_in_fbar_ages_min_max(file_fbar_ages_min_max, fbar_ages_min_max)) {
        throw std::runtime_error("bad file format while executing: fill_in_fbar_ages_min_max");
    }
    file_fbar_ages_min_max.close();


    return (fbar_ages_min_max);
}

bool TextfileModelLoader::loadPopulations(int a_quarter)
{
    map<int, string> pop_names;
    read_pop_names_in_string(pop_names, p->folder_name_parameterization, p->inputfolder);

    vector<string> type_of_avai_field_to_read(model().config().nbpops);

    for (int st = 0; st < model().config().nbpops; st++) {
        type_of_avai_field_to_read.at(st) = "";
    }


    // ...or if a biolsce informed on staticAvai
    if (model().scenario().dyn_pop_sce.option(Options::biolsceOnStaticAvai)) {
        for (int st = 0; st < model().config().nbpops; st++) {
            type_of_avai_field_to_read.at(st) = "_biolsce" + model().scenario().biolsce;
        }
    }


    string str_rand_avai_file = "baseline"; // deprecated?
    // by default, will use the initial avai input

    int year = model().year() +1;
    auto month = std::to_string(model().month());
    auto quarter = std::to_string(model().quarter());
    auto semester = std::to_string(model().semester());
    auto iparam1 = model().config().nbpops;
    auto iparam2 = NBAGE;
    auto iparam3 = NBSZGROUP;
    auto iparam4 = SEL_NBSZGROUP;
    auto vdparam1 = model().config().calib_cpue_multiplier;
    auto vdparam2 = model().config().calib_weight_at_szgroup;

    // read the pop-specific betas related to the availability
    // szgroup0
    multimap<int, double> avai0_betas = read_avai_betas(semester, "0", p->folder_name_parameterization, p->inputfolder);
    // szgroup2
    multimap<int, double> avai2_betas = read_avai_betas(semester, "2", p->folder_name_parameterization, p->inputfolder);
    // szgroup3
    multimap<int, double> avai3_betas = read_avai_betas(semester, "3", p->folder_name_parameterization, p->inputfolder);
    // szgroup5
    multimap<int, double> avai5_betas = read_avai_betas(semester, "5", p->folder_name_parameterization, p->inputfolder);
    // szgroup7
    multimap<int, double> avai7_betas = read_avai_betas(semester, "7", p->folder_name_parameterization, p->inputfolder);

    // read other stuffs...
    // CAUTION: DO NOT LEFT BLANK AT THE END OF THE FILES!!!!  // CAUTION: DO NOT LEFT BLANK AT THE END OF THE FILES!!!!
    cout << "Do the pop files init_pops_per_szgroup need a check?" << "\n";
    multimap<int, double> init_pops_per_szgroup = read_init_pops_per_szgroup(p->folder_name_parameterization,
                                                                             p->inputfolder,
                                                                             model().scenario().biolsce);
    cout << "Do the pop files init_prop_migrants_pops_per_szgroup need a check?" << "\n";
    multimap<int, double> init_prop_migrants_pops_per_szgroup = read_init_prop_migrants_pops_per_szgroup(
            p->folder_name_parameterization, p->inputfolder, model().scenario().biolsce);
    cout << "Do the pop files init_fecundity_per_szgroup need a check?" << "\n";
    multimap<int, double> init_fecundity_per_szgroup = read_init_fecundity_per_szgroup(p->folder_name_parameterization,
                                                                                       p->inputfolder,
                                                                                       model().scenario().biolsce);
    cout << "Do the pop files init_maturity_per_szgroup need a check?" << "\n";
    multimap<int, double> init_maturity_per_szgroup = read_init_maturity_per_szgroup(p->folder_name_parameterization,
                                                                                     p->inputfolder,
                                                                                     model().scenario().biolsce);
    cout << "Do the pop files init_weight_per_szgroupneed a check?" << "\n";
    multimap<int, double> init_weight_per_szgroup = read_init_weight_per_szgroup(p->folder_name_parameterization,
                                                                                 p->inputfolder,
                                                                                 model().scenario().biolsce);
    cout << "Do the pop files init_comcat_per_szgroup need a check?" << "\n";
    multimap<int, int> init_comcat_per_szgroup = read_init_comcat_per_szgroup(p->folder_name_parameterization,
                                                                              p->inputfolder);
    cout << "Do the pop files init_M_per_szgroup need a check?" << "\n";
    multimap<int, double> init_M_per_szgroup = read_init_M_per_szgroup(p->folder_name_parameterization, p->inputfolder,
                                                                       model().scenario().biolsce);
    cout << "Do the pop files init_proprecru_per_szgroup need a check?" << "\n";
    multimap<int, double> init_proprecru_per_szgroup = read_init_proprecru_per_szgroup(p->folder_name_parameterization,
                                                                                       p->inputfolder,
                                                                                       model().scenario().biolsce);
    //cout << "Do the pop files lst_idx_nodes_per_pop need a check?" << "\n";
    multimap<int, types::NodeId> lst_idx_nodes_per_pop = read_lst_idx_nodes_per_pop(semester,
                                                                                    p->folder_name_parameterization,
                                                                                    p->inputfolder, str_rand_avai_file);

   //cout << "Do the pop files init_M_per_szgroup need a check?" << "\n";
   // additional params for the size spectra modelling
    multimap<int, double> init_beta_ssm_per_szgroup = read_beta_ssm_szgroup(p->folder_name_parameterization, p->inputfolder,
        model().scenario().biolsce);

    multimap<int, double> init_background_mortality_per_szgroup = read_background_mortality_szgroup(p->folder_name_parameterization, p->inputfolder,
        model().scenario().biolsce);

    
    cout << "Do the pop files selected_szgroups need a check?" << "\n";
    multimap<int, int> selected_szgroups = read_selected_szgroups_per_pop(p->folder_name_parameterization,
                                                                          p->inputfolder);
    // CAUTION: DO NOT LEFT BLANK AT THE END OF THE FILES!!!!  // CAUTION: DO NOT LEFT BLANK AT THE END OF THE FILES!!!!


    // init
    // detect nb of selected szgroups arbitrary from pop0
    // and init the vector of vector avai object on node
    multimap<int, int>::iterator lselsz = selected_szgroups.lower_bound(0);
    multimap<int, int>::iterator uselsz = selected_szgroups.upper_bound(0);
    vector<int> selected_szgroups_pop0;
    for (multimap<int, int>::iterator pos = lselsz; pos != uselsz; pos++) {
        selected_szgroups_pop0.push_back(pos->second);
    }


    cout << "Does the pop file tac_percent_simulated need a check?" << "\n";
    map<int, int> tac_percent_simulated = read_tac_percent_simulated(p->folder_name_parameterization, p->inputfolder);
    cout << "Does the pop file hyperstability_param need a check?" << "\n";
    map<int, double> hyperstability_param = read_hyperstability_param(p->folder_name_parameterization, p->inputfolder);


    // input data, read proportion of natural mortality from other species when spatial co-occurences on node
    cout << "Do the species_interactions_mortality_proportion_matrix creation  need a check?" << "\n";
    vector<vector<double> > species_interactions_mortality_proportion_matrix = read_species_interactions_mortality_proportion_matrix(
            iparam1, p->folder_name_parameterization, p->inputfolder, model().scenario().biolsce);



    // get the name of the pops
    // copy only unique elements of init_pops_per_szgroup into name_pops
    // DEADLY BUG: MAKE SURE THAT NO BLANK IS LEFT IN THE VERY END OF THE .DAT FILE...
    cout << "Do the name_pops creation  need a check?" << "\n";
    vector<int> name_pops;
    for (multimap<int, double>::iterator iter = init_pops_per_szgroup.begin(); iter != init_pops_per_szgroup.end();
         iter = init_pops_per_szgroup.upper_bound(iter->first)) {
        name_pops.push_back(iter->first);
        cout << "pop " << iter->first << "\n";

    }
    cout << "nb pops: " << name_pops.size() << "\n";
    cout << "if you have a problem of overflow here then check if you forgot a blank at the end of N_at_szgroup.dat! "
         << "\n";





    // initiate for loading loop over sp
    vector<double> vect_of_avai0_beta_v(name_pops.size());
    vector<double> vect_of_avai2_beta_v(name_pops.size());
    vector<double> vect_of_avai3_beta_v(name_pops.size());
    vector<double> vect_of_avai5_beta_v(name_pops.size());
    vector<double> vect_of_avai7_beta_v(name_pops.size());
    vector<vector<int> > vect_of_init_selected_szgroups_vov(name_pops.size());
    vector<vector<double> > vect_of_init_tot_N_per_szgroup_vov(name_pops.size());
    vector<vector<double> > vect_of_init_prop_migrants_in_N_per_szgroup_vov(name_pops.size());
    vector<vector<double> > vect_of_init_fecundity_per_szgroup_vov(name_pops.size());
    vector<vector<double> > vect_of_init_maturity_per_szgroup_vov(name_pops.size());
    vector<vector<double> > vect_of_init_weight_per_szgroup_vov(name_pops.size());
    vector<vector<int> > vect_of_init_comcat_per_szgroup_vov(name_pops.size());
    vector<vector<double> > vect_of_init_M_per_szgroup_vov(name_pops.size());
    vector<vector<double> > vect_of_beta_ssm_per_szgroup_vov(name_pops.size());
    vector<vector<double> > vect_of_background_mortality_per_szgroup_vov(name_pops.size()); 
    vector<vector<double> > vect_of_init_proprecru_per_szgroup_vov(name_pops.size());

    //vector <multimap<types::NodeId, double> > vect_of_avai_szgroup_nodes_with_pop_mmap(name_pops.size());
    vector<multimap<types::NodeId, double> > vect_of_full_avai_szgroup_nodes_with_pop_mmap(name_pops.size());
    vector<multimap<types::NodeId, double> > vect_of_field_of_coeff_diffusion_this_pop_mmap(name_pops.size());
    vector<map<types::NodeId, double> > vect_of_oth_land_map(name_pops.size());
    vector<vector<double> > vect_of_selectivity_per_stock_ogives_for_oth_land(name_pops.size());
    vector<vector<map<types::NodeId, double> > > vect_of_vect_of_oth_land_map(name_pops.size());
    vector<map<string, double> > vect_of_relative_stability_key_map(name_pops.size());
    vector<map<int, double> > vect_of_percent_tac_per_vessel_length_class_map(name_pops.size());
    vector<map<int, double> > vect_of_percent_tac_cumul_over_months_key_map(name_pops.size());
    vector<vector<vector<double> > > vect_of_growth_transition_matrix_vov(name_pops.size());
    vector<vector<vector<double> > > vect_of_percent_szgroup_per_age_matrix_vov(name_pops.size());
    vector<vector<vector<double> > > vect_of_percent_age_per_szgroup_matrix_vov(name_pops.size());
    vector<vector<double> > vect_of_param_sr_v(name_pops.size());
    vector<vector<double> > vect_of_fbar_ages_min_max_and_ftarget_this_pop_v(name_pops.size());
    vector<vector<double> > vect_of_tac_this_pop(name_pops.size());
    vector<multimap<int, double> > vect_of_overall_migration_fluxes_mmap(name_pops.size());
    vector<double> landings_so_far(name_pops.size());

    vector<vector<types::NodeId> > vect_of_lst_idx_nodes_per_pop_vov(name_pops.size());

    vector<string> popnames(name_pops.size());

    for (unsigned int sp = 0; sp < iparam1; sp++) 
    {
        dout(cout << "\n");

        popnames.at(sp) = pop_names[sp];
        cout << "pop_name: " << sp << ": " << popnames.at(sp) << "\n";


        // avai0 beta for this particular pop
        multimap<int, double>::iterator lower_0 = avai0_betas.lower_bound(sp);
        multimap<int, double>::iterator upper_0 = avai0_betas.upper_bound(sp);
        for (multimap<int, double>::iterator pos = lower_0; pos != upper_0; pos++)
            vect_of_avai0_beta_v.at(sp) = pos->second;

        // avai2 beta for this particular pop
        multimap<int, double>::iterator lower_2 = avai2_betas.lower_bound(sp);
        multimap<int, double>::iterator upper_2 = avai2_betas.upper_bound(sp);
        for (multimap<int, double>::iterator pos = lower_2; pos != upper_2; pos++)
            vect_of_avai2_beta_v.at(sp) = pos->second;

        // avai3 beta for this particular pop
        multimap<int, double>::iterator lower_3 = avai3_betas.lower_bound(sp);
        multimap<int, double>::iterator upper_3 = avai3_betas.upper_bound(sp);
        for (multimap<int, double>::iterator pos = lower_3; pos != upper_3; pos++)
            vect_of_avai3_beta_v.at(sp) = pos->second;

        // avai5 beta for this particular pop
        multimap<int, double>::iterator lower_5 = avai5_betas.lower_bound(sp);
        multimap<int, double>::iterator upper_5 = avai5_betas.upper_bound(sp);
        for (multimap<int, double>::iterator pos = lower_5; pos != upper_5; pos++)
            vect_of_avai5_beta_v.at(sp) = pos->second;

        // avai7 beta for this particular pop
        multimap<int, double>::iterator lower_7 = avai7_betas.lower_bound(sp);
        multimap<int, double>::iterator upper_7 = avai7_betas.upper_bound(sp);
        for (multimap<int, double>::iterator pos = lower_7; pos != upper_7; pos++)
            vect_of_avai7_beta_v.at(sp) = pos->second;

        // initial selected szgroups
        if (year== 1 && model().month() == 1)
        {
            multimap<int, int>::iterator lower_init_selsz = selected_szgroups.lower_bound(sp);
            multimap<int, int>::iterator upper_init_selsz = selected_szgroups.upper_bound(sp);
            for (multimap<int, int>::iterator pos = lower_init_selsz; pos != upper_init_selsz; pos++)
                // convert in thousands
                vect_of_init_selected_szgroups_vov.at(sp).push_back(pos->second);
        }

        // initial N for this particular pop
        if (year == 1 && model().month() == 1)
        {
            multimap<int, double>::iterator lower_init = init_pops_per_szgroup.lower_bound(sp);
            multimap<int, double>::iterator upper_init = init_pops_per_szgroup.upper_bound(sp);
            for (multimap<int, double>::iterator pos = lower_init; pos != upper_init; pos++)
                // !!!! CONVERT FROM THOUSANDS TO ABSOLUTE NUMBERS N  !!!!
                vect_of_init_tot_N_per_szgroup_vov.at(sp).push_back(pos->second * 1000);
            // !!!! CONVERT FROM THOUSANDS TO ABSOLUTE NUMBERS N  !!!!
            cout << "Caution: we remind you that DISPLACE expects input initial N in THOUSANDS...Did you check? " << "\n";
        }

        // initial prop_migrants for this particular pop
        multimap<int, double>::iterator lower_init_migrants = init_prop_migrants_pops_per_szgroup.lower_bound(sp);
        multimap<int, double>::iterator upper_init_migrants = init_prop_migrants_pops_per_szgroup.upper_bound(sp);
        for (multimap<int, double>::iterator pos = lower_init_migrants; pos != upper_init_migrants; pos++)
            // convert in thousands
            vect_of_init_prop_migrants_in_N_per_szgroup_vov.at(sp).push_back(pos->second);

        // initial fecundity for this particular pop
        if (year == 1 && model().month() == 1)
        {
            multimap<int, double>::iterator lower_init_fec = init_fecundity_per_szgroup.lower_bound(sp);
            multimap<int, double>::iterator upper_init_fec = init_fecundity_per_szgroup.upper_bound(sp);
            for (multimap<int, double>::iterator pos = lower_init_fec; pos != upper_init_fec; pos++)
                vect_of_init_fecundity_per_szgroup_vov.at(sp).push_back(pos->second);
        }

        // initial maturity for particular this pop
        if (year == 1 && model().month() == 1)
        {
            multimap<int, double>::iterator lower_init_ma = init_maturity_per_szgroup.lower_bound(sp);
            multimap<int, double>::iterator upper_init_ma = init_maturity_per_szgroup.upper_bound(sp);
            for (multimap<int, double>::iterator pos = lower_init_ma; pos != upper_init_ma; pos++)
                vect_of_init_maturity_per_szgroup_vov.at(sp).push_back(pos->second);
        }

        // initial weight for this particular pop
        if (year == 1 && model().month() == 1)
        {
            multimap<int, double>::iterator lower_init_we = init_weight_per_szgroup.lower_bound(sp);
            multimap<int, double>::iterator upper_init_we = init_weight_per_szgroup.upper_bound(sp);
            for (multimap<int, double>::iterator pos = lower_init_we; pos != upper_init_we; pos++)
                vect_of_init_weight_per_szgroup_vov.at(sp).push_back(pos->second);
        }

        // initial comcat for this particular pop
        if (year == 1 && model().month() == 1)
        {
            multimap<int, int>::iterator lower_init_cc = init_comcat_per_szgroup.lower_bound(sp);
            multimap<int, int>::iterator upper_init_cc = init_comcat_per_szgroup.upper_bound(sp);
            for (multimap<int, int>::iterator pos = lower_init_cc; pos != upper_init_cc; pos++)
                vect_of_init_comcat_per_szgroup_vov.at(sp).push_back(pos->second);
        }

        // initial M for this particular pop
        if (year == 1 && model().month() == 1)
        {
            multimap<int, double>::iterator lower_init_M = init_M_per_szgroup.lower_bound(sp);
            multimap<int, double>::iterator upper_init_M = init_M_per_szgroup.upper_bound(sp);
            for (multimap<int, double>::iterator pos = lower_init_M; pos != upper_init_M; pos++)
                vect_of_init_M_per_szgroup_vov.at(sp).push_back(pos->second);
        }

        // initial beta_ssm for this particular pop
        if (year == 1 && model().month() == 1)
        {
            multimap<int, double>::iterator lower_beta_ssm = init_beta_ssm_per_szgroup.lower_bound(sp);
            multimap<int, double>::iterator upper_beta_ssm = init_beta_ssm_per_szgroup.upper_bound(sp);
            for (multimap<int, double>::iterator pos = lower_beta_ssm; pos != upper_beta_ssm; pos++)
                vect_of_beta_ssm_per_szgroup_vov.at(sp).push_back(pos->second);
        }
        
        // initial background_mortality for this particular pop
        if (year == 1 && model().month() == 1)
        {
            multimap<int, double>::iterator lower_background_mortality = init_background_mortality_per_szgroup.lower_bound(sp);
            multimap<int, double>::iterator upper_background_mortality = init_background_mortality_per_szgroup.upper_bound(sp);
            for (multimap<int, double>::iterator pos = lower_background_mortality; pos != upper_background_mortality; pos++)
                vect_of_background_mortality_per_szgroup_vov.at(sp).push_back(pos->second);
        }
        
        // initial proprecru for this particular pop
        if (year == 1 && model().month() == 1)
        {
            multimap<int, double>::iterator lower_init_proprecru = init_proprecru_per_szgroup.lower_bound(sp);
            multimap<int, double>::iterator upper_init_proprecru = init_proprecru_per_szgroup.upper_bound(sp);
            for (multimap<int, double>::iterator pos = lower_init_proprecru; pos != upper_init_proprecru; pos++)
                vect_of_init_proprecru_per_szgroup_vov.at(sp).push_back(pos->second);
        }

      

        // OPTIONS ALTERING THE SPATIAL AVAILABILITY OF STOCKS
        // default
        if (year == 1 && model().month() == 1)
        {
            type_of_avai_field_to_read.push_back("");
        }

        // ...or if a biolsce informed on staticAvai
        if (model().scenario().dyn_pop_sce.option(Options::biolsceOnStaticAvai)) 
        {
                type_of_avai_field_to_read.at(sp) = "_biolsce" + model().scenario().biolsce;
            }

        // ...or if a dynamic coupling done with LGNB
        if (model().scenario().dyn_pop_sce.option(Options::lgnbCoupling)) 
        {
            string a_command_for_R;

                if (binary_search(model().config().nbcp_coupling_pops.begin(),
                    model().config().nbcp_coupling_pops.end(), sp)) 
                {
                    type_of_avai_field_to_read.at(sp) = "_updated_" + model().nameSimu();

                    stringstream out;
                    out << sp;
                    string a_pop = out.str();

                    stringstream outtstep;
                    outtstep << model().timestep();
                    string atstep = outtstep.str();
#if defined(_WIN32)
                    cout << "if ERR here: Did you set the environmental variables with the Rscript path and restart the compiler env?" << "\n";
                    //a_command_for_R = "Rscript .\\interactiverscripts\\lgnb-displace_coupling_script2.r " + a_pop + " " + atstep + " " + p->folder_name_parameterization + " " + model().nameSimu() + " " + model().scenario().a_graph_name;
                    a_command_for_R = "Rscript " + p->inputfolder + "\\interactiverscripts\\lgnb-displace_coupling_script2.r " + a_pop + " " + atstep + " " + p->folder_name_parameterization + " " + model().nameSimu() + " " + model().scenario().a_graph_name;
                    cout << "executing " << a_command_for_R << "\n";
                    system(a_command_for_R.c_str());
#else
                    cout << "try the lgnb-displace_coupling..." << "\n";
                    // caution with HPC, annoying lower cases in file names and paths required! 
                    a_command_for_R =
                        "Rscript  " + p->inputfolder + "/interactiverscripts/lgnb-displace_coupling_script2.r " +
                        a_pop + " " + atstep + " " + p->folder_name_parameterization + " " + model().nameSimu() +
                        " " + model().scenario().a_graph_name;
                    cout << "lgnb-displace_coupling...done" << "\n";

		    // alternatively, if the module load for this R version fails on HPC (see in base_script_ui.txt bash file), use a hardcoding path to Rscript like /appl/R/bin/Rscript-3.6.3-mkl and recompile DISPLACE

                    system(a_command_for_R.c_str());
#endif
                }  // end nbcp coupling pops
           
        }


        if (model().scenario().dyn_pop_sce.option(Options::avai_shuffler_on) ||
            model().scenario().dyn_pop_sce.option(Options::avai_updater_on))
        {


            // alter the availability field, if required
            for (unsigned int pp = 0; pp < model().populations().size(); pp++)
            {
                if (!binary_search(model().config().implicit_pops.begin(), model().config().implicit_pops.end(),
                    pp)) 
                {
                    stringstream out;
                    out << pp;
                    string a_pop = out.str();

                    stringstream out2;
                    out2 << model().scenario().nrow_coord;
                    string a_nrow_coord = out2.str();

                    string a_command;
                    string a_command_for_R;
                    stringstream outtstep;
                    outtstep << model().timestep();
                    string atstep = outtstep.str();
                    // the system command line
#if defined(_WIN32)
                    if (model().scenario().dyn_pop_sce.option(Options::avai_updater_on) && model().timestep() > 744) {
                        // note that nothing is done before end of 1st month (745) to get enough catch data for an update
                        type_of_avai_field_to_read.at(pp) = "_updated_" + model().nameSimu();
                        //system("dir");
                        // caution with HPC, annoying lower cases in file names and paths required!
                        cout
                            << "if ERR here: Did you set the environmental variables with the Rscript path and restart the compiler env?"
                            << "\n";
                        a_command_for_R =
                            "Rscript .\\interactiverscripts\\input2avaiupdater.r " + a_pop + " " + atstep +
                            " " + p->folder_name_parameterization + " " + model().nameSimu() + " " + model().scenario().a_graph_name;
                        //a_command_for_R = "R CMD BATCH .\\interactiverscripts\\input2avaiupdater.r "+a_pop+" "+atstep;
                        cout << "look after " << a_command_for_R << "\n";
                        cout << "This supposes StockId " << a_pop
                            << " is informed in displace_input_for_data_merger.csv input file" << "\n";
                        system(a_command_for_R.c_str());
                        a_command = "avaifieldupdater.exe -tstep " + atstep + " -f " + p->folder_name_parameterization + " -a " +
                            p->inputfolder + " -s " + std::to_string(model().semester()) + " -graph " +
                            model().scenario().a_graph_name + " -nr " + a_nrow_coord + " -dist 15 -shepard_p 0.5";
                        cout << "look after " << a_command
                            << "\n"; // right now look into the data input folder, so need to have the exe here...TODO look into the displace.exe folder instead!!
                        system(a_command.c_str());
                    }
                    if (model().scenario().dyn_pop_sce.option(Options::avai_shuffler_on)) {
                        type_of_avai_field_to_read.at(pp) = "_shuffled";
                        a_command = "avaifieldshuffler.exe -f " + p->folder_name_parameterization + " -s " +
                            std::to_string(model().semester()) + " -p " + a_pop;
                        cout << "look after " << a_command
                            << "\n"; // right now look into the data input folder, so need to have the exe here...TODO look into the displace.exe folder instead!!
                        system(a_command.c_str());
                    }
#else
                    if (model().scenario().dyn_pop_sce.option(Options::avai_updater_on) && model().timestep() > 744) {
                        type_of_avai_field_to_read.at(pp) = "_updated";
                        // caution with HPC, annoying lower cases in file names and paths required!
                        a_command_for_R =
                            "Rscript " + p->inputfolder + "/interactiverscripts/input2avaiupdater.r " + a_pop +
                            " " + atstep + " " + p->folder_name_parameterization + " " + model().nameSimu() + " " +
                            model().scenario().a_graph_name;
                        system(a_command_for_R.c_str());
                        a_command =
                            p->inputfolder + "/avaifieldupdatertool -tstep " + atstep + " -f " +
                            p->folder_name_parameterization +
                            " -a " + p->inputfolder + " -s " + std::to_string(model().semester()) + " -graph " +
                            model().scenario().a_graph_name +
                            " -nr " + a_nrow_coord + " -dist 30 -shepard_p 0.5";
                        system(a_command.c_str());
                    }
                    if (model().scenario().dyn_pop_sce.option(Options::avai_shuffler_on)) {
                        a_command = p->inputfolder + "/avaifieldshufflertool -f " + p->folder_name_parameterization +
                            " -s " +
                            std::to_string(model().semester()) + " -p " + a_pop;
                        system(a_command.c_str());
                    }
                    cout << "avaifieldshuffler...done" << "\n";
#endif

                }
            }
        }



        string avai_folder_addon_name = "";
        if (model().scenario().dyn_pop_sce.option(Options::static_avai_2)) avai_folder_addon_name = "_2";
        if (model().scenario().dyn_pop_sce.option(Options::static_avai_3)) avai_folder_addon_name = "_3";
        if (model().scenario().dyn_pop_sce.option(Options::static_avai_4)) avai_folder_addon_name = "_4";

        vect_of_full_avai_szgroup_nodes_with_pop_mmap.at(sp) = read_full_avai_szgroup_nodes_with_pop(
                semester, sp,
                p->folder_name_parameterization,
                p->inputfolder, str_rand_avai_file,
                type_of_avai_field_to_read,
            avai_folder_addon_name);


        // get the vector of nodes of presence for this pop (an optimization to avoid looping over all nodes...)
        outc(cout << "first find the list of nodes with presence for this pop (this quarter)..." << "\n");
        auto lower_pop = lst_idx_nodes_per_pop.lower_bound(sp);
        auto upper_pop = lst_idx_nodes_per_pop.upper_bound(sp);
        for (multimap<int, types::NodeId>::iterator a_pos = lower_pop; a_pos != upper_pop; a_pos++) {
            vect_of_lst_idx_nodes_per_pop_vov.at(sp).push_back(a_pos->second);
        }


        // input data
        if (year == 1 && model().month() == 1)
        {
            if (model().scenario().dyn_pop_sce.option(Options::diffusePopN)) {
                cout << "read_field_of_coeff_diffusion_this_pop ..." << "\n";
                vect_of_field_of_coeff_diffusion_this_pop_mmap.at(sp) = read_field_of_coeff_diffusion_this_pop(semester, sp,
                    p->folder_name_parameterization,
                    p->inputfolder,
                    model().scenario().biolsce);
            }
        }

        // input data, read a other landings per node for this species
        if (year == 1 && model().month() == 1)
        {
            // oth_land on node per pop
            vect_of_oth_land_map.at(sp) = read_oth_land_nodes_with_pop(semester, month, sp,
                p->folder_name_parameterization, p->inputfolder,
                model().scenario().fleetsce);
        
            // ...or oth_land on node per pop per met
            if (model().scenario().dyn_alloc_sce.option(Options::otherLandPerMetPerPop)) {                   
                cout << "Looking for oth_land on node per pop per met dat files" << "\n";
                int met = -1, er= 0;
                do{
                    met += 1;
                    map<types::NodeId, double> a_map;
                    er = read_oth_land_map_per_met_on_nodes(a_map, semester, month, sp, met,
                        p->folder_name_parameterization, p->inputfolder,
                        model().scenario().fleetsce);
                       
                    if(er==0) vect_of_vect_of_oth_land_map.at(sp).push_back(a_map);
                } while(er!=-1);
                
            }
        
            // oth_land are not metier-specific by nature, but the reader is placed here for coherence...
            vect_of_selectivity_per_stock_ogives_for_oth_land = read_selectivity_per_stock_ogives_for_oth_land(
              model().config().nbpops, NBSZGROUP,
              p->folder_name_parameterization, p->inputfolder,
             model().scenario().fleetsce);



        }

        if (year == 1 && model().month() == 1)
        {
            vect_of_relative_stability_key_map.at(sp) = read_relative_stability_keys(semester, sp,
                p->folder_name_parameterization,
                p->inputfolder);
            vect_of_percent_tac_per_vessel_length_class_map.at(sp) = read_percent_tac_per_vessel_length_class(semester, sp,
                p->folder_name_parameterization,
                p->inputfolder);
            vect_of_percent_tac_cumul_over_months_key_map.at(sp) = read_percent_tac_cumul_over_months_keys(semester, sp,
                p->folder_name_parameterization,
                p->inputfolder);

            //cout << "check relative_stability_key when loading: " << "\n";
            //for (auto elem : vect_of_relative_stability_key_map.at(sp))
            //{
            //    std::cout << elem.first << " " << elem.second << "\n";
            //}

        
        }

        // input data, growth transition, percent_szgroup_per_age_matrix
        if (year == 1 && model().month() == 1)
        {
            vect_of_growth_transition_matrix_vov.at(sp) = read_growth_transition_matrix(sp, iparam3,
                p->folder_name_parameterization,
                p->inputfolder,
                model().scenario().biolsce);
        }
        
        if (year == 1 && model().month() == 1)
        {
            vect_of_percent_szgroup_per_age_matrix_vov.at(sp) = read_percent_szgroup_per_age_matrix(sp, iparam3,
                iparam2,
                p->folder_name_parameterization,
                p->inputfolder,
                model().scenario().biolsce);
        }

        if (year == 1 && model().month() == 1)
        {
            vect_of_percent_age_per_szgroup_matrix_vov.at(sp) = read_percent_age_per_szgroup_matrix(sp, iparam3,
                iparam2,
                p->folder_name_parameterization,
                p->inputfolder,
                model().scenario().biolsce);
        }

        // input data, parameter for stock-recruitment relationship
        if (year == 1 && model().month() == 1)
        {
            vect_of_param_sr_v.at(sp) = read_param_sr(sp, p->folder_name_parameterization, p->inputfolder,
                model().scenario().biolsce);
        }

        // input data, fbar ages
        if (year == 1 && model().month() == 1)
        {
            vect_of_fbar_ages_min_max_and_ftarget_this_pop_v.at(sp) = read_fbar_ages_min_max_and_ftarget(sp,
                p->folder_name_parameterization,
                p->inputfolder,
                model().scenario().biolsce);
        }

        // input data, initial tac
        if (year == 1 && model().month() == 1)
        {
            vect_of_tac_this_pop.at(sp) = read_initial_tac(sp, p->folder_name_parameterization, p->inputfolder);
            cout << "initial tac has been read correctly" << "\n";
        }

        if (year == 1 && model().month() == 1)
        {
            if (model().scenario().dyn_alloc_sce.option(Options::TACs) && vect_of_tac_this_pop.at(sp).at(0) == 0) {
                cout << "WARNING: TACs Option is active: Consider informing a initial TAC value for pop" << sp
                    << "and potentially other pops..." << "\n";
                cout << "a fake, non binding value is filled in for now" << "\n";
                vect_of_tac_this_pop.at(sp).at(0) = 100000; // tons
            }
        }


        // input data, read migration fluxes in proportion per size group (if any)
        dout(cout << "read overall migration..." << "\n");
        vect_of_overall_migration_fluxes_mmap.at(sp) = read_overall_migration_fluxes(semester, sp,
                                                                                     p->folder_name_parameterization,
                                                                                     p->inputfolder,
                                                                                     model().scenario().biolsce);
        dout(cout << "overall migration has been read correctly" << "\n");


        if (year == 1 && model().month() == 1)
        {
            landings_so_far.push_back(1.0);
        }

    } // end sp

    LoadedData loadedData;

    //  export 
    // used in the Population() creator
    loadedData.vectsparam1 = popnames;
    loadedData.vectdparam1 = vect_of_avai0_beta_v;
    loadedData.vectdparam2 = vect_of_avai2_beta_v;
    loadedData.vectdparam3 = vect_of_avai3_beta_v;
    loadedData.vectdparam4 = vect_of_avai5_beta_v;
    loadedData.vectdparam5 = vect_of_avai7_beta_v;
    loadedData.vovi1 = vect_of_init_selected_szgroups_vov;
    loadedData.vovd1 = vect_of_init_tot_N_per_szgroup_vov;
    loadedData.vovd2 = vect_of_init_prop_migrants_in_N_per_szgroup_vov;
    loadedData.vovd3 = vect_of_init_fecundity_per_szgroup_vov;
    loadedData.vovd4 = vect_of_init_weight_per_szgroup_vov;
    loadedData.vovi2 = vect_of_init_comcat_per_szgroup_vov;
    loadedData.vovd5 = vect_of_init_maturity_per_szgroup_vov;
    loadedData.vovd6 = vect_of_init_M_per_szgroup_vov;
    loadedData.vovd7 = vect_of_init_proprecru_per_szgroup_vov;
    loadedData.vovd8 = vect_of_param_sr_v;
    loadedData.vovd9 = vect_of_selectivity_per_stock_ogives_for_oth_land;
    loadedData.vovd12 = vect_of_beta_ssm_per_szgroup_vov;
    loadedData.vovd13 = vect_of_background_mortality_per_szgroup_vov;
    loadedData.vectmmapndparam1 = vect_of_full_avai_szgroup_nodes_with_pop_mmap;
    loadedData.vectmmapndparam2 = vect_of_field_of_coeff_diffusion_this_pop_mmap;
    loadedData.vectmapndparam1 = vect_of_oth_land_map;
    loadedData.vovomapndparam1 = vect_of_vect_of_oth_land_map;  
    loadedData.vectmmapidparam1 = vect_of_overall_migration_fluxes_mmap;
    loadedData.vectmapsdparam1 = vect_of_relative_stability_key_map;
    loadedData.vectmapsdparam2 = vect_of_percent_tac_per_vessel_length_class_map;
    loadedData.vectmapsdparam3 = vect_of_percent_tac_cumul_over_months_key_map;
    loadedData.vovovd2 = vect_of_percent_szgroup_per_age_matrix_vov;
    loadedData.vovovd3 = vect_of_percent_age_per_szgroup_matrix_vov;
    loadedData.vovovd1 = vect_of_growth_transition_matrix_vov;
    // nodes,
    loadedData.vovd10 = vect_of_fbar_ages_min_max_and_ftarget_this_pop_v;
    loadedData.vovd11 = vect_of_tac_this_pop;
    loadedData.mapiiparam1 = tac_percent_simulated;
    loadedData.mapidparam1 = hyperstability_param;
    loadedData.vectdparam6 = vdparam1; // calib_cpue_multiplier;
    loadedData.vectdparam7 = vdparam2;  // calib_weight_at_szgroup;

    // not used in the Population() creator
    loadedData.vectiparam1 = selected_szgroups_pop0;
    loadedData.vovn1 = vect_of_lst_idx_nodes_per_pop_vov; // nodes_with_presence

    // used in simulator main.cpp
    loadedData.mmapidparam_init_weight_per_szgroup = init_weight_per_szgroup;
    loadedData.vovd_species_interactions_mortality_proportion_matrix = species_interactions_mortality_proportion_matrix;

    loadedData.vectsparam2 = type_of_avai_field_to_read;


    // create Populations only once, i.e. at y1 m1
    if (year == 1 && model().month()==1)
    {

        //int nbmets = 100; // CAUTION: for now we don´t care as a large value will be resized. but best to ultimately include it to config()
        int nbmets = model().config().nbmets; //  from config()
        for (unsigned int i = 0; i < model().nodes().size(); i++) {
            model().nodes().at(i)->init_Ns_pops_at_szgroup(iparam1, iparam3, nbmets);
            model().nodes().at(i)->init_avai_pops_at_selected_szgroup(iparam1, iparam4);
        }



        auto populations = vector<Population*>(model().config().nbpops);
        vector<double> global_quotas_uptake;

        for (unsigned int sp = 0; sp < populations.size(); sp++) {
            dout(cout << "\n");


            cout << " create pop... " << "\n";
            populations[sp] = new Population(sp,
                loadedData.vectsparam1.at(sp),
                loadedData.vectdparam1.at(sp),
                loadedData.vectdparam2.at(sp),
                loadedData.vectdparam3.at(sp),
                loadedData.vectdparam4.at(sp),
                loadedData.vectdparam5.at(sp),
                loadedData.vovi1.at(sp),
                loadedData.vovd1.at(sp),
                loadedData.vovd2.at(sp),
                loadedData.vovd3.at(sp),
                loadedData.vovd4.at(sp),
                loadedData.vovi2.at(sp),
                loadedData.vovd5.at(sp),
                loadedData.vovd6.at(sp),
                loadedData.vovd7.at(sp),
                loadedData.vovd8.at(sp),
                loadedData.vovd9.at(sp),
                loadedData.vectmmapndparam1.at(sp),
                loadedData.vectmmapndparam2.at(sp),
                loadedData.vectmapndparam1.at(sp),
                loadedData.vovomapndparam1.at(sp), 
                loadedData.vectmmapidparam1.at(sp),
                loadedData.vectmapsdparam1.at(sp),
                loadedData.vectmapsdparam2.at(sp),
                loadedData.vectmapsdparam3.at(sp),
                loadedData.vovovd2.at(sp),
                loadedData.vovovd3.at(sp),
                loadedData.vovovd1.at(sp),
                model().nodes(),
                loadedData.vovd10.at(sp),
                loadedData.vovd11.at(sp),
                loadedData.mapiiparam1.at(sp),
                loadedData.mapidparam1.at(sp),
                loadedData.vectdparam6.at(sp),
                loadedData.vectdparam7.at(sp)
            );


            cout << " Population creator()...done " << "\n";

            global_quotas_uptake.push_back(0.0);

            if (model().scenario().dyn_pop_sce.option(Options::sizeSpectra)) {

                if (loadedData.vovd12.at(sp).size() != NBSZGROUP)
                {
                    cout << "check beta_ssm for pop" << sp << ": some missing szgroups...." << "\n";
                }
                if (loadedData.vovd13.at(sp).size() != NBSZGROUP)
                {
                    cout << "check background_mortality for pop" << sp << ": some missing szgroups...." << "\n";
                }
                populations.at(sp)->set_beta_ssm_at_szgroup(loadedData.vovd12.at(sp)); // beta_ssm
                populations.at(sp)->set_background_mortality_at_szgroup(loadedData.vovd13.at(sp));   // background_M

            }


            if (!binary_search(model().config().implicit_pops.begin(), model().config().implicit_pops.end(), sp)) {
                outc(cout << "inform avai on nodes " << "\n");

                outc(cout << "...then attach avai to each node for this pop (this quarter)" << "\n");
                // init avai on each node (we know the presence...) for this pop for selected szgroup
                vector<types::NodeId> nodes_with_presence = loadedData.vovn1.at(sp);
                multimap<types::NodeId, double> avai_szgroup_nodes_with_pop = loadedData.vectmmapndparam1.at(sp);

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
                        model().nodes().at(nodes_with_presence.at(n).toIndex())->set_avai_pops_at_selected_szgroup(sp,
                            spat_avai_per_selected_szgroup);
                    }
                    else {
                        // inconsistence between lst_idx_nodes and avai files if this happen...
                        outc(cout << nodes_with_presence.at(n));
                    }

                }

                // check
                /*
               outc(cout << "avai at selected szgroup for the pop " << sp << " on a given node xx:" << "\n"); // used in do_catch != the one used in distributeN()
                vector<double> avai_pops_at_selected_szgroup = nodes[792]->get_avai_pops_at_selected_szgroup(sp);
                vector<double>::iterator szgroup = avai_pops_at_selected_szgroup.begin();
                for( ; szgroup != avai_pops_at_selected_szgroup.end(); szgroup++)
                {
                   outc(cout << *szgroup << " " );
                }
               outc(cout << "\n");

                // check
               outc(cout << "tot N at szgroup for the pop " << sp << "on a given node xx:" << "\n");
                vector<double> tot_N_at_szgroup = populations[sp]->get_tot_N_at_szgroup();
                vector<double>::iterator szgroup2 = tot_N_at_szgroup.begin();
                for( ; szgroup2 != tot_N_at_szgroup.end(); szgroup2++)
                {
                   outc(cout << *szgroup << " " );
                }
               outc(cout << "\n");
                */

                outc(cout
                    << "if you have a problem of out of range here then check if you forgot a blank at the end of N_at_szgroup.dat! "
                    << "\n");
            }                         // end implicit pop
        }                             // end pop


        model().setPopulations(std::move(populations));
        model().setGlobalQuotasUptake(std::move(global_quotas_uptake));
        model().setInitWeightPerSzgroup(std::move(init_weight_per_szgroup));
        model().set_species_interactions_mortality_proportion_matrix(species_interactions_mortality_proportion_matrix);



    }
    else
    {


        cout << "redispatch the population over its spatial extent...." << "\n";

        // aggregate from nodes to set the tot_N_at_szgroup per pop
        for (unsigned int sp = 0; sp < model().populations().size(); sp++) {
            // aggregate from nodes (caution: do it before changing of list_nodes)
            if (!binary_search(model().config().implicit_pops.begin(), model().config().implicit_pops.end(),
                sp)) {

                /*
                if(sp==1){
                    vector <double> a_tot_N_at_szgroup_here = simModel->populations().at(sp)->get_tot_N_at_szgroup();
                    for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
                     cout << "BEFORE AGGREGATE IN MAIN: a_tot_N_at_szgroup[" << sz << "] is "<< a_tot_N_at_szgroup_here[sz]  << "\n";
                }
                */
                // get total N from summing up N over nodes
                model().populations().at(sp)->aggregate_N();

                /*
                if(sp==1){
                    vector <double> a_tot_N_at_szgroup_here = simModel->populations().at(sp)->get_tot_N_at_szgroup();
                    for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
                     cout << "AFTER AGGREGATE IN MAIN: a_tot_N_at_szgroup[" << sz << "] is "<< a_tot_N_at_szgroup_here[sz]  << "\n";
                }
                */

            }
        }
        cout << "aggregate_N over all pops....done" << "\n";

        

        // then, clean up all nodes before changing of spatial avai
        // (necessary to remove any fish in now wrong locations)
        cout << "clear pops on nodes" << "\n";
        for (unsigned int i = 0; i < model().nodes().size(); i++) {
            model().nodes().at(i)->clear_pop_names_on_node();
            model().nodes().at(i)->clear_Ns_pops_at_szgroup();
            model().nodes().at(i)->clear_avai_pops_at_selected_szgroup();
        }
        cout << "clear pops on nodes...done" << "\n";

        // RE-read for simModel->populations()
        for (unsigned int i = 0; i < model().populations().size(); i++) {
            stringstream out;
            out << i;

            cout << "RE-read for population " << model().populations().at(i)->get_name() << " from " <<
                p->folder_name_parameterization << " " << p->inputfolder << " " << type_of_avai_field_to_read.at(i)
                << "\n";

            auto full_avai_szgroup_nodes_with_pop = loadedData.vectmmapndparam1.at(i);
            model().populations().at(i)->set_full_spatial_availability(full_avai_szgroup_nodes_with_pop);


            // read a other landings per node for this species (DEPRECATED - DONE AT MONTH TSTEP INSTEAD)
            //map<int, double> oth_land= read_oth_land_nodes_with_pop(simModel->semester(), simModel->month(), i, folder_name_parameterization, inputfolder, scenario.fleetsce);
            //simModel->populations().at(i)->set_oth_land(oth_land);

            multimap<int, double> overall_migration_fluxes = loadedData.vectmmapidparam1.at(i);
            model().populations().at(i)->set_overall_migration_fluxes(overall_migration_fluxes);

            // apply the overall migration loss fluxes (i.e. on the overall N at szgroup)
            if (!model().scenario().dyn_pop_sce.option(Options::stop_mig_35065) || model().timestep() < 35065) {
                model().populations().at(i)->apply_overall_migration_fluxes(model().populations());
            }

            //then, re-set the list_nodes and the pop_names_on_node
            // from the new area distribution given by this new spatial avai
            vector<Node*> list_nodes;
            for (auto iter = full_avai_szgroup_nodes_with_pop.begin();
                iter != full_avai_szgroup_nodes_with_pop.end();
                iter = full_avai_szgroup_nodes_with_pop.upper_bound(iter->first)) {
                list_nodes.push_back(model().nodes()[iter->first.toIndex()]);
                model().nodes()[iter->first.toIndex()]->set_pop_names_on_node(i);
                //   check per node
                //   vector <int> pop_names = nodes[ iter->first ]->get_pop_names_on_node();
                //   cout << "Node " << iter->first << "\n";
                //   for(int p=0;p<pop_names.size();p++) cout<< pop_names.at(p) << " ";
                //   cout << "\n";
            }
            model().populations().at(i)->set_list_nodes(list_nodes);

            // add the current Ns to the vectors of vectors of the concerned nodes
            vector<double> tot_N_at_szgroup = model().populations().at(i)->get_tot_N_at_szgroup();

            /*if( simModel->populations().at(i)->get_name()==1){
                vector <double> a_tot_N_at_szgroup_here = simModel->populations().at(i)->get_tot_N_at_szgroup();
                for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
                   cout << "CHECK IN MAIN: a_tot_N_at_szgroup[" << sz << "] is "<< a_tot_N_at_szgroup_here[sz]  << "\n";
            */


            for (unsigned int n = 0; n < list_nodes.size(); n++) {
                list_nodes[n]->set_Ns_pops_at_szgroup(i, tot_N_at_szgroup);
                dout(cout << list_nodes[n]->get_idx_node().toIndex() << " ");
            }
            dout(cout << "\n");

            // distribute tot_N_at_szgroup on nodes knowing the avai spatial key
            // i.e. update the vectors of vectors Ns_pops_at_szgroup of the nodes as usual
            // divide on nodes according to avai
            if (!binary_search(model().config().implicit_pops.begin(), model().config().implicit_pops.end(),
                i)) {
                model().populations().at(i)->distribute_N();

                //if(simModel->populations().at(i)->get_name()==1){
                //    vector <double> a_tot_N_at_szgroup_here = simModel->populations().at(i)->get_tot_N_at_szgroup();
                //    for(int sz=0; sz < a_tot_N_at_szgroup_here.size(); sz++)
                //      cout << "CHECK IN MAIN2: a_tot_N_at_szgroup[" << sz << "] is "<< a_tot_N_at_szgroup_here[sz]  << "\n";
                //}

                //...and compute the Ns on nodes at the start of this month!
                for (unsigned int n = 0; n < model().nodes().size(); n++) {
                    model().nodes().at(n)->set_Ns_pops_at_szgroup_at_month_start(i, model().nodes().at(
                        n)->get_Ns_pops_at_szgroup(i));
                }
            }


            vector<types::NodeId> nodes_with_presence = loadedData.vovn1.at(i);
            multimap<types::NodeId, double> avai_szgroup_nodes_with_pop = loadedData.vectmmapndparam1.at(i);

            for (unsigned int n = 0; n < nodes_with_presence.size(); n++) {
                dout(cout << ".");
                auto spat_avai_this_pop_this_node = find_entries(avai_szgroup_nodes_with_pop,
                    nodes_with_presence.at(n));

                vector<double> spat_avai_per_selected_szgroup;
                vector<int> selected_szgroups = model().populations().at(i)->get_selected_szgroups();
                for (int sz = 0; sz < spat_avai_this_pop_this_node.size(); ++sz) {
                    auto it = find(selected_szgroups.begin(), selected_szgroups.end(), sz);
                    if (it != selected_szgroups.end()) {
                        spat_avai_per_selected_szgroup.push_back(spat_avai_this_pop_this_node.at(sz));
                    }
                }
                if (!spat_avai_per_selected_szgroup.empty()) {
                    model().nodes().at(
                        nodes_with_presence.at(n).toIndex())->set_avai_pops_at_selected_szgroup(i,
                            spat_avai_per_selected_szgroup);
                }
                else {
                    // inconsistence between lst_idx_nodes and avai files if this happen...
                    outc(cout << nodes_with_presence.at(n));
                }

            }


        }

        dout(cout << "re-read data for this period...OK" << "\n");





    }








    return true;
}

