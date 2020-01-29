#include "dataloaderpops.h"
#include<iostream>

using namespace std;



Dataloaderpops::Dataloaderpops()
{

}

int Dataloaderpops::gettype()
{
    return(1);
}


void read_pop_names_in_string(map<int, string>& pop_names,
                                string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/pop_names_"+folder_name_parameterization+".txt";
    ifstream file_pop_names;
    file_pop_names.open(filename.c_str());
    if(file_pop_names.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
     }
    else
    {
        fill_map_from_specifications_i_s(file_pop_names,  pop_names,  inputfolder);
        file_pop_names.close();

    }
}



// FOR POPULATION
multimap<int, double> read_avai_betas(string a_semester, string szgroup, string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/avai"+szgroup+"_betas_semester"+a_semester+".dat";

    //input data, pop characteristics: e.g. avai0_betas i.e. betas per pop for szgroup 0
    ifstream avai_betas_file;
    avai_betas_file.open(filename.c_str());
    if(avai_betas_file.fail())
    {
        cout << "Unfortunately the /avaiXX_betas_semesterXX.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    // it is actually only a map...but was simpler to reuse the multimap routines
    multimap<int, double> avai_betas;
    fill_multimap_from_specifications_i_d(avai_betas_file,  avai_betas);
    avai_betas_file.close();

    return(avai_betas);
}


multimap<int, double> read_init_pops_per_szgroup(string folder_name_parameterization,  string inputfolder, string biolsce)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_pops_per_szgroup_biolsce"+biolsce+".dat";

    //input data, pop characteristics: initial N
    ifstream file_init_pops_per_szgroup;
    file_init_pops_per_szgroup.open(filename.c_str());
    if(file_init_pops_per_szgroup.fail())
    {
        filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_pops_per_szgroup_biolsce1.dat";
        file_init_pops_per_szgroup.open(filename.c_str());
    }
    if(file_init_pops_per_szgroup.fail())
    {
        cout << "Unfortunately the init_pops_per_szgroup_biolsceXX.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> init_pops_per_szgroup;
    fill_multimap_from_specifications_i_d(file_init_pops_per_szgroup,  init_pops_per_szgroup);
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
    dout(cout << endl);
#endif

    return(init_pops_per_szgroup);
}



multimap<int, double> read_adults_diet_preference_per_stock_allstks(string folder_name_parameterization,  string inputfolder, string biolsce)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/_adults_diet_preference_per_stock_allstks_biolsce"+biolsce+".dat";

    //input data
    ifstream file_adults_diet_preference_per_stock_allstks;
    file_adults_diet_preference_per_stock_allstks.open(filename.c_str());
    if(file_adults_diet_preference_per_stock_allstks.fail())
    {
        filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/adults_diet_preference_per_stock_allstks_biolsce1.dat";
        file_adults_diet_preference_per_stock_allstks.open(filename.c_str());
    }
    if(file_adults_diet_preference_per_stock_allstks.fail())
    {
        cout << "Unfortunately the adults_diet_preference_per_stock_allstks_biolsceXX.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> adults_diet_preference_per_stock_allstks;
    fill_multimap_from_specifications_i_d(file_adults_diet_preference_per_stock_allstks,  adults_diet_preference_per_stock_allstks);
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
    dout(cout << endl);
#endif

    return(adults_diet_preference_per_stock_allstks);
}


multimap<int, double> read_juveniles_diet_preference_per_stock_allstks(string folder_name_parameterization,  string inputfolder, string biolsce)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/_juveniles_diet_preference_per_stock_allstks_biolsce"+biolsce+".dat";

    //input data
    ifstream file_juveniles_diet_preference_per_stock_allstks;
    file_juveniles_diet_preference_per_stock_allstks.open(filename.c_str());
    if(file_juveniles_diet_preference_per_stock_allstks.fail())
    {
        filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/juveniles_diet_preference_per_stock_allstks_biolsce1.dat";
        file_juveniles_diet_preference_per_stock_allstks.open(filename.c_str());
    }
    if(file_juveniles_diet_preference_per_stock_allstks.fail())
    {
        cout << "Unfortunately the juveniles_diet_preference_per_stock_allstks_biolsceXX.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> juveniles_diet_preference_per_stock_allstks;
    fill_multimap_from_specifications_i_d(file_juveniles_diet_preference_per_stock_allstks,  juveniles_diet_preference_per_stock_allstks);
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
    dout(cout << endl);
#endif

    return(juveniles_diet_preference_per_stock_allstks);
}



multimap<int, double> read_init_prop_migrants_pops_per_szgroup(string folder_name_parameterization,  string inputfolder, string biolsce)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_prop_migrants_pops_per_szgroup_biolsce"+biolsce+".dat";

    //input data, pop characteristics: initial prop migrants
    ifstream file_init_prop_migrants_pops_per_szgroup;
    file_init_prop_migrants_pops_per_szgroup.open(filename.c_str());
    if(file_init_prop_migrants_pops_per_szgroup.fail())
    {
        filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_prop_migrants_pops_per_szgroup_biolsce1.dat";
        file_init_prop_migrants_pops_per_szgroup.open(filename.c_str());
    }
    if(file_init_prop_migrants_pops_per_szgroup.fail())
    {
        cout << "Unfortunately the init_prop_migrants_pops_per_szgroup_biolsceXX.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> init_prop_migrants_pops_per_szgroup;
    fill_multimap_from_specifications_i_d(file_init_prop_migrants_pops_per_szgroup,  init_prop_migrants_pops_per_szgroup);
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
    dout(cout << endl);
#endif

    return(init_prop_migrants_pops_per_szgroup);
}







multimap<int, double> read_init_maturity_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_maturity_per_szgroup_biolsce"+biolsce+".dat";

    //input data, pop characteristics: maturity_at_szgroup
    ifstream file_init_maturity_per_szgroup;
    file_init_maturity_per_szgroup.open(filename.c_str());
    if(file_init_maturity_per_szgroup.fail())
    {
        filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_maturity_per_szgroup_biolsce1.dat";
        file_init_maturity_per_szgroup.open(filename.c_str());
    }
    if(file_init_maturity_per_szgroup.fail())
    {
        cout << "Unfortunately the init_maturity_per_szgroup_biolsceXX.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> init_maturity_per_szgroup;
    fill_multimap_from_specifications_i_d(file_init_maturity_per_szgroup,  init_maturity_per_szgroup);
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
    dout(cout << endl);
#endif

    return(init_maturity_per_szgroup);
}


multimap<int, double> read_init_fecundity_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_fecundity_per_szgroup_biolsce"+biolsce+".dat";

    //input data, pop characteristics: maturity_at_szgroup
    ifstream file_init_fecundity_per_szgroup;
    file_init_fecundity_per_szgroup.open(filename.c_str());
    if(file_init_fecundity_per_szgroup.fail())
    {
        filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_fecundity_per_szgroup_biolsce1.dat";
        file_init_fecundity_per_szgroup.open(filename.c_str());
    }
    if(file_init_fecundity_per_szgroup.fail())
    {
        cout << "Unfortunately the init_fecundity_per_szgroup_biolsceXX.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> init_fecundity_per_szgroup;
    fill_multimap_from_specifications_i_d(file_init_fecundity_per_szgroup,  init_fecundity_per_szgroup);
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
    dout(cout << endl);
#endif

    return(init_fecundity_per_szgroup);
}


multimap<int, double> read_init_weight_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_weight_per_szgroup_biolsce"+biolsce+".dat";

    //input data, pop characteristics: weight_at_szgroup
    ifstream file_init_weight_per_szgroup;
    file_init_weight_per_szgroup.open(filename.c_str());
    if(file_init_weight_per_szgroup.fail())
    {
        filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_weight_per_szgroup_biolsce1.dat";
        file_init_weight_per_szgroup.open(filename.c_str());
    }
    if(file_init_weight_per_szgroup.fail())
    {
        cout << "Unfortunately the init_weight_per_szgroup_biolsceXX.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> init_weight_per_szgroup;
    fill_multimap_from_specifications_i_d(file_init_weight_per_szgroup,  init_weight_per_szgroup);
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
    dout(cout << endl);
#endif

    return(init_weight_per_szgroup);
}


multimap<int, int> read_init_comcat_per_szgroup(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/comcat_per_szgroup_done_by_hand.dat";

    //input data, pop characteristics: weight_at_szgroup
    ifstream file_comcat_per_szgroup;
    file_comcat_per_szgroup.open(filename.c_str());
    if(file_comcat_per_szgroup.fail())
    {
        cout << "Unfortunately the comcat_per_szgroup_done_by_hand.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, int> comcat_per_szgroup;
    fill_multimap_from_specifications_i_i(file_comcat_per_szgroup,  comcat_per_szgroup);
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
    cout << endl;
    */

    return(comcat_per_szgroup);
}



multimap<int, double> read_init_M_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_M_per_szgroup_biolsce"+biolsce+".dat";

    //input data, pop characteristics: weight_at_szgroup
    ifstream file_init_M_per_szgroup;
    file_init_M_per_szgroup.open(filename.c_str());
    if(file_init_M_per_szgroup.fail())
    {
        filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_M_per_szgroup_biolsce1.dat";
        file_init_M_per_szgroup.open(filename.c_str());
    }
    if(file_init_M_per_szgroup.fail())
    {
        cout << "Unfortunately the init_M_per_szgroup_biolsceXX.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> init_M_per_szgroup;
    fill_multimap_from_specifications_i_d(file_init_M_per_szgroup,  init_M_per_szgroup);
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
    dout(cout << endl);
#endif

    return(init_M_per_szgroup);
}


multimap<int, double> read_init_proprecru_per_szgroup(string folder_name_parameterization, string inputfolder, string biolsce)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_proprecru_per_szgroup_biolsce"+biolsce+".dat";

    //input data, pop characteristics: weight_at_szgroup
    ifstream file_init_proprecru_per_szgroup;
    file_init_proprecru_per_szgroup.open(filename.c_str());
    if(file_init_proprecru_per_szgroup.fail())
    {
        filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/init_proprecru_per_szgroup_biolsce1.dat";
        file_init_proprecru_per_szgroup.open(filename.c_str());
    }
    if(file_init_proprecru_per_szgroup.fail())
    {
        cout << "Unfortunately the init_proprecru_per_szgroup_biolsceXX.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
     }
    multimap<int, double> init_proprecru_per_szgroup;
    fill_multimap_from_specifications_i_d(file_init_proprecru_per_szgroup,  init_proprecru_per_szgroup);
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
    dout(cout << endl);
#endif

    return(init_proprecru_per_szgroup);
}


multimap<int, types::NodeId> read_lst_idx_nodes_per_pop(string a_semester, string folder_name_parameterization, string inputfolder, string str_rand_avai_file)
{

    string filename;
    if(str_rand_avai_file=="baseline")
    {
        filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/static_avai/"+"lst_idx_nodes_per_pop_semester"+a_semester+".dat";
    }
    else
    {
        filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/static_avai/"+"lst_idx_nodes_per_pop_semester"+a_semester+".dat";
        // filename=  "popsspe_"+folder_name_parameterization+"/stochast_avai/"+"lst_idx_nodes_per_pop_semester"+a_semester+"_"+str_rand_avai_file+".dat";
    }

    //input data, pop characteristics: lst idx nodes with presence
    ifstream file_lst_idx_nodes_per_pop;
    file_lst_idx_nodes_per_pop.open(filename.c_str());
    if(file_lst_idx_nodes_per_pop.fail())
    {
        cout << "Unfortunately the lst_idx_nodes_per_pop_.dat vector is not informed  "<< endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, int> lst_idx_nodes_per_pop;
    fill_multimap_from_specifications_i_i(file_lst_idx_nodes_per_pop,  lst_idx_nodes_per_pop);
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
    dout(cout << endl);
#endif

    // TODO (fba#5#): remove possible replicates in the list of nodes per pop

    return(types::helpers::toValueIdMultimap<types::NodeId>(lst_idx_nodes_per_pop));
}



multimap<int, int> read_selected_szgroups_per_pop(string folder_name_parameterization, string inputfolder)
{

    string filename;
    filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/"+"the_selected_szgroups.dat";

    //input data, pop characteristics: lst idx nodes with presence
    ifstream file_the_selected_szgroups;
    file_the_selected_szgroups.open(filename.c_str());
    if(file_the_selected_szgroups.fail())
    {
        cout << "Unfortunately the the_selected_szgroups.dat vector is not informed " << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
     }
    multimap<int, int> the_selected_szgroups;
    fill_multimap_from_specifications_i_i(file_the_selected_szgroups,  the_selected_szgroups);
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
    cout << endl;*/

    return(the_selected_szgroups);
}

map<int, int> read_tac_percent_simulated(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/percent_landings_from_simulated_vessels.dat";

    //input data, pop characteristics: percent_landings
    ifstream file_tac_percent_simulated;
    file_tac_percent_simulated.open(filename.c_str());
    if(file_tac_percent_simulated.fail())
    {
        cout << "Unfortunately the percent_landings_from_simulated_vessels.dat vector is not informed" << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    map<int, int> tac_percent_simulated;
    fill_map_from_specifications_i_i(file_tac_percent_simulated,  tac_percent_simulated);
    file_tac_percent_simulated.close();

#ifdef VERBOSE
    // check input
    for ( map<int,int>::iterator it=tac_percent_simulated.begin() ; it != tac_percent_simulated.end(); it++ )
        dout(cout << (*it).first << " => " << (*it).second << endl);
#endif

    return(tac_percent_simulated);
}


map<int, double> read_hyperstability_param(string folder_name_parameterization, string inputfolder)
{

    string filename=  inputfolder+"/popsspe_"+folder_name_parameterization+"/hyperstability_param.dat";

    //input data, pop characteristics: hyperstability
    ifstream file_hyperstability_param;
    file_hyperstability_param.open(filename.c_str());
    if(file_hyperstability_param.fail())
    {
        cout << "Unfortunately the hyperstability_param.dat vector is not informed "<< endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    map<int, double> hyperstability_param;
    fill_map_from_specifications_i_d(file_hyperstability_param,  hyperstability_param, folder_name_parameterization);
    file_hyperstability_param.close();

#ifdef VERBOSE
    // check input
    for ( map<int,double>::iterator it=hyperstability_param.begin() ; it != hyperstability_param.end(); it++ )
        dout(cout << (*it).first << " => " << (*it).second << endl);
#endif

    return(hyperstability_param);
}



map<types::NodeId, double> read_oth_land_nodes_with_pop(string a_semester, string a_month, int a_pop, string folder_name_parameterization, string inputfolder, string fleetsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename;
    if(fleetsce=="")
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_stecf_oth_land_per_month_per_node_semester"+a_semester+".dat";
    }
    else
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_stecf_oth_land_per_month_per_node_month"+a_month+"_fleetsce"+fleetsce+".dat";
    }

    ifstream file_oth_land;
    file_oth_land.open(filename.c_str());
    if(file_oth_land.fail())
    {
        cout << "Unfortunately the spe_stecf_oth_land_per_month_per_node_ vector is not informed for stock "<< a_pop_s << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    map<types::NodeId, double> oth_land;
    fill_from_oth_land (file_oth_land, oth_land);
    file_oth_land.close();

#ifdef VERBOSE
    // check input
    map<int,double>::iterator pos;
    dout(cout << " oth_land " << endl);
    for (pos=oth_land.begin(); pos != oth_land.end(); pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif

    return(oth_land);
}





multimap<int, double> read_overall_migration_fluxes(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder, string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/"+a_pop_s+"overall_migration_fluxes_semester"+a_semester+"_biolsce"+biolsce+".dat";

    ifstream file_overall_migration_fluxes;
    file_overall_migration_fluxes.open(filename.c_str());
    if(file_overall_migration_fluxes.fail())
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/"+a_pop_s+"overall_migration_fluxes_semester"+a_semester+"_biolsce1.dat";
        file_overall_migration_fluxes.open(filename.c_str());
        cout << "biolsce ignored for " << filename  << endl;
    }
    if(file_overall_migration_fluxes.fail())
    {
        cout << "Unfortunately the overall_migration_fluxes_ vector is not informed for stock "<< a_pop_s << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<int, double> overall_migration_fluxes;
    fill_from_overall_migration_fluxes (file_overall_migration_fluxes, overall_migration_fluxes);
    file_overall_migration_fluxes.close();

#ifdef VERBOSE
    // check input
    multimap<int,double>::iterator pos;
    dout(cout << " overall_migration_fluxes " << endl);
    for (pos=overall_migration_fluxes.begin(); pos != overall_migration_fluxes.end(); pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif

    return(overall_migration_fluxes);
}





map<string, double> read_relative_stability_keys(string a_semester, int a_pop, string folder_name_parameterization, string inputfolder)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename;
    if(folder_name_parameterization=="final")
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" +
                a_pop_s + "ctrysspe_relative_stability_semester"+a_semester+".dat";
    }
    else
    {
        //=> NEW_VERSION: replaced by:
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" +
                a_pop_s + "ctrysspe_relative_stability_semester"+a_semester+".dat";
    }

    ifstream file_relative_stability;
    file_relative_stability.open(filename.c_str());
    if(file_relative_stability.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    map<string, double> relative_stability;
    fill_from_relative_stability (file_relative_stability, relative_stability);
    file_relative_stability.close();

#ifdef VERBOSE
    // check input
    map<string,double>::iterator pos;
    dout(cout << " relative_stability " << endl);
    for (pos=relative_stability.begin(); pos != relative_stability.end(); pos++)
    {
        dout(cout << pos->second << " ");
    }
    dout(cout << endl);
#endif

    return(relative_stability);
}


multimap<types::NodeId, double> read_avai_szgroup_nodes_with_pop(string a_semester,
                                                                 int a_pop, string folder_name_parameterization, string inputfolder,
                                                                 string str_rand_avai_file,
                                                                 vector<string> type_of_avai_field_to_read)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string a_type_of_avai_field_to_read = type_of_avai_field_to_read.at(a_pop);
    //cout << "a_type_of_avai_field_to_read is " << a_type_of_avai_field_to_read << endl;

    string filename;
    if(str_rand_avai_file=="baseline")
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/static_avai/" +
                       a_pop_s + "spe_avai_szgroup_nodes_semester"+a_semester+a_type_of_avai_field_to_read+".dat";
    }
    else
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/stochast_avai/" +
                       a_pop_s + "spe_avai_szgroup_nodes_semester"+a_semester+a_type_of_avai_field_to_read+".dat";
    }

    ifstream file_avai_szgroup_nodes_with_pop;
    file_avai_szgroup_nodes_with_pop.open(filename.c_str());
    if(file_avai_szgroup_nodes_with_pop.fail())
    {
        cout << "Unfortunately the availability vector is not informed for stock "<< a_pop_s << endl;
        cout << "You´ll have to stop the simu, correct input and re-run. " << endl;
        cout << "This could happen if the stock is missing in the displace_input_for_data_merger_xx.dat along the avai_updater_on Option" << endl;
        cout << "this might also result from an attempt to use an empty vmslikefpingsonly.dat file (empty if no ref. vessels defined)" << endl;
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<types::NodeId, double> avai_szgroup_nodes_with_pop;
    fill_from_avai_szgroup_nodes_with_pop (file_avai_szgroup_nodes_with_pop, avai_szgroup_nodes_with_pop);
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
    dout(cout << endl);
#endif

    // TODO (fba#5#): check avai sum to 1 for a given szgroup

    return(avai_szgroup_nodes_with_pop);
}


multimap<types::NodeId, double> read_full_avai_szgroup_nodes_with_pop(string a_semester, int a_pop,
                                                                      string folder_name_parameterization, string inputfolder,
                                                                      string str_rand_avai_file,
                                                                      vector<string> type_of_avai_field_to_read)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string a_type_of_avai_field_to_read = type_of_avai_field_to_read.at(a_pop);

    string filename;
    if(str_rand_avai_file=="baseline")
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/static_avai/" +
                a_pop_s + "spe_full_avai_szgroup_nodes_semester"+a_semester+a_type_of_avai_field_to_read+".dat";
    }
    else
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/stochast_avai/" +
                a_pop_s + "spe_full_avai_szgroup_nodes_semester"+a_semester+"_"+str_rand_avai_file+a_type_of_avai_field_to_read+".dat";
    }

    ifstream file_avai_szgroup_nodes_with_pop;
    file_avai_szgroup_nodes_with_pop.open(filename.c_str());
    if(file_avai_szgroup_nodes_with_pop.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    dout(cout << "filename for pop  " << a_pop << " is " << filename << endl;)

    multimap<types::NodeId, double> full_avai_szgroup_nodes_with_pop;
    if (!fill_from_avai_szgroup_nodes_with_pop (file_avai_szgroup_nodes_with_pop, full_avai_szgroup_nodes_with_pop))
        throw std::runtime_error("Error while executing: fill_from_avai_szgroup_nodes_with_pop");

    file_avai_szgroup_nodes_with_pop.close();

    // check input
    //cout << "avai on node 9510: ";
    //for(auto iter=full_avai_szgroup_nodes_with_pop.begin(); iter != full_avai_szgroup_nodes_with_pop.end();
    //    iter = full_avai_szgroup_nodes_with_pop.upper_bound( iter->first ) )
    //{
    //    cout << iter->first.toIndex() <<" : " << iter->second << " - ";
    //}
    //cout << endl;
    // TODO (fba#5#): check avai sum to 1 for a given szgroup

    return(full_avai_szgroup_nodes_with_pop);
}



multimap<types::NodeId, double> read_field_of_coeff_diffusion_this_pop(string a_semester, int a_pop,
                                                                       string folder_name_parameterization, string inputfolder,
                                                                       string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename;
    filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/static_avai/" + a_pop_s + "spe_field_of_coeff_diffusion_this_pop_nodes_semester"+a_semester+"_biolsce"+biolsce+".dat";

    ifstream file_field_of_coeff_diffusion_this_pop;
    file_field_of_coeff_diffusion_this_pop.open(filename.c_str());
    if(file_field_of_coeff_diffusion_this_pop.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    multimap<types::NodeId, double> field_of_coeff_diffusion_this_pop;
    if (!fill_field_of_coeff_diffusion_this_pop (file_field_of_coeff_diffusion_this_pop, field_of_coeff_diffusion_this_pop))
        throw std::runtime_error("Error while executing: fill_field_of_coeff_diffusion_this_pop");

    file_field_of_coeff_diffusion_this_pop.close();


    return(field_of_coeff_diffusion_this_pop);
}




vector< vector<double> > read_percent_szgroup_per_age_matrix(int a_pop, int nbszgroup,int nbage, string folder_name_parameterization, string inputfolder, string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_percent_szgroup_per_age_biolsce"+biolsce+".dat";

    ifstream file_percent_szgroup_per_age_matrix;
    file_percent_szgroup_per_age_matrix.open(filename.c_str());
    if(file_percent_szgroup_per_age_matrix.fail())
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_percent_szgroup_per_age_biolsce1.dat";
        file_percent_szgroup_per_age_matrix.open(filename.c_str());
        cout << "biolsce ignored for " << filename  << endl;
    }
    if(file_percent_szgroup_per_age_matrix.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
     }
    vector< vector<double> > percent_szgroup_per_age_matrix(nbszgroup, vector<double>(nbage));
    if (!fill_in_percent_szgroup_per_age_matrix(file_percent_szgroup_per_age_matrix, percent_szgroup_per_age_matrix))
        throw std::runtime_error("Error while executuing: fill_in_percent_szgroup_per_age_matrix");

    file_percent_szgroup_per_age_matrix.close();

    return(percent_szgroup_per_age_matrix);
}


vector< vector<double> > read_percent_age_per_szgroup_matrix(int a_pop, int nbszgroup,int nbage, string folder_name_parameterization, string inputfolder, string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_percent_age_per_szgroup_biolsce"+biolsce+".dat";

    ifstream file_percent_age_per_szgroup_matrix;
    file_percent_age_per_szgroup_matrix.open(filename.c_str());
    if(file_percent_age_per_szgroup_matrix.fail())
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_percent_age_per_szgroup_biolsce1.dat";
        file_percent_age_per_szgroup_matrix.open(filename.c_str());
        cout << "biolsce ignored for " << filename  << endl;
    }
    if(file_percent_age_per_szgroup_matrix.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    vector< vector<double> > percent_age_per_szgroup_matrix(nbszgroup, vector<double>(nbage));
    if (!fill_in_percent_age_per_szgroup_matrix(file_percent_age_per_szgroup_matrix, percent_age_per_szgroup_matrix))
        throw std::runtime_error("Error while executuing: fill_in_percent_age_per_szgroup_matrix");

    file_percent_age_per_szgroup_matrix.close();

    return(percent_age_per_szgroup_matrix);
}


vector< vector<double> > read_growth_transition_matrix(int a_pop, int nbszgroup, string folder_name_parameterization, string inputfolder, string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_size_transition_matrix_biolsce"+biolsce+".dat";

    ifstream file_size_transition_matrix;
    file_size_transition_matrix.open(filename.c_str());
    if(file_size_transition_matrix.fail())
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_size_transition_matrix_biolsce1.dat";
        file_size_transition_matrix.open(filename.c_str());
        cout << "biolsce ignored for " << filename  << endl;
    }
    if(file_size_transition_matrix.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
     }
    vector< vector<double> > growth_transition_matrix(nbszgroup, vector<double>(nbszgroup));
    if (!fill_in_growth_transition(file_size_transition_matrix, growth_transition_matrix))
        throw std::runtime_error("Error while executuing: fill_in_growth_trransition_matrix");

    file_size_transition_matrix.close();

    return(growth_transition_matrix);
}


vector< vector<double> > read_preferences_for_species_matrix(int a_pop, int nbpops, int nbszgroup, string folder_name_parameterization, string inputfolder, string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "preferences_for_species_matrix_biolsce"+biolsce+".dat";

    ifstream file_preferences_for_species;
    file_preferences_for_species.open(filename.c_str());
    if(file_preferences_for_species.fail())
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "preferences_for_species_matrix_biolsce1.dat";
        file_preferences_for_species.open(filename.c_str());
        cout << "biolsce ignored for " << filename  << endl;
    }
    if(file_preferences_for_species.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    vector< vector<double> > preferences_for_species_matrix(nbpops, vector<double>(nbszgroup));
    if (!fill_in_preferences_for_species_matrix(file_preferences_for_species, preferences_for_species_matrix))
        throw std::runtime_error("Error while executing: fill_in_preferences_for_species_matrix");

    file_preferences_for_species.close();

    return(preferences_for_species_matrix);
}



vector< vector<double> > read_species_interactions_mortality_proportion_matrix(int nbpops, string folder_name_parameterization, string inputfolder, string biolsce)
{
    // casting a_pop into a string
    string filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/species_interactions_mortality_proportion_matrix_biolsce"+biolsce+".dat";

    ifstream file_species_interactions;
    file_species_interactions.open(filename.c_str());
    if(file_species_interactions.fail())
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/species_interactions_mortality_proportion_matrix_biolsce1.dat";
        file_species_interactions.open(filename.c_str());
        cout << "biolsce ignored for " << filename  << endl;
    }
    if(file_species_interactions.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    vector< vector<double> > species_interactions_mortality_proportion_matrix(nbpops, vector<double>(nbpops));
    if (!fill_in_species_interactions_mortality_proportion_matrix(file_species_interactions, species_interactions_mortality_proportion_matrix))
        throw std::runtime_error("Error while executuing: fill_in_species_interactions_mortality_proportion_matrix");

    file_species_interactions.close();

    return(species_interactions_mortality_proportion_matrix);
}

vector<double>  read_param_sr(int a_pop,  string folder_name_parameterization, string inputfolder, string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_SSB_R_parameters_biolsce"+biolsce+".dat";

    ifstream file_param_sr;
    file_param_sr.open(filename.c_str());
    if(file_param_sr.fail())
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_SSB_R_parameters_biolsce1.dat";
        file_param_sr.open(filename.c_str());
        cout << "biolsce ignored for " << filename  << endl;
    }
    if(file_param_sr.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    vector<double> param_sr(3);
    if (!fill_in_param_sr(file_param_sr, param_sr))
        throw std::runtime_error("Error while executuing: fill_in_param_sr");

    file_param_sr.close();

    return(param_sr);
}

vector<double>  read_initial_tac(int a_pop,  string folder_name_parameterization,  string inputfolder)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_initial_tac.dat";

    ifstream file_initial_tac;
    file_initial_tac.open(filename.c_str());
    if(file_initial_tac.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
    }
    vector<double> initial_tac(1);
    if (!fill_in_initial_tac(file_initial_tac, initial_tac))
        throw std::runtime_error("Error while executuing: fill_in_initial_tac");
    file_initial_tac.close();

    return(initial_tac);
}


vector<double>  read_fbar_ages_min_max_and_ftarget(int a_pop,  string folder_name_parameterization, string inputfolder, string biolsce)
{
    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_fbar_amin_amax_ftarget_Fpercent_TACpercent_biolsce"+biolsce+".dat";

    ifstream file_fbar_ages_min_max;
    file_fbar_ages_min_max.open(filename.c_str());
    if(file_fbar_ages_min_max.fail())
    {
        filename = inputfolder+"/popsspe_"+folder_name_parameterization+"/" + a_pop_s + "spe_fbar_amin_amax_ftarget_Fpercent_TACpercent.dat";
        file_fbar_ages_min_max.open(filename.c_str());
        cout << "biolsce ignored for " << filename  << endl;
    }
    if(file_fbar_ages_min_max.fail())
    {
        string error_msg = "error opening file " + filename;
        cout << error_msg << "\n";

        exit(-1);
     }
    // i.e. 8 columns: F min age, F max age, LTMP ftarget, Fpercent, TACpercent, Btrigger, F-MSY value, F-MSY-low, F-MSY-up
    vector<double> fbar_ages_min_max(9);
    if (!fill_in_fbar_ages_min_max(file_fbar_ages_min_max, fbar_ages_min_max))
        throw std::runtime_error("bad file format while executing: fill_in_fbar_ages_min_max");
    file_fbar_ages_min_max.close();


    return(fbar_ages_min_max);
}







int Dataloaderpops::features(std::shared_ptr<sql::Storage> indb,
                                 const string& folder_name_parameterization,
                                 const string& inputfolder,
                                 PopSceOptions &dyn_pop_sce,
                                 DynAllocOptions &dyn_alloc_sce,
                                 string &biolsce,
                                 string &fleetsce,
                                 ParamsForLoad &paramsForLoad,
                                 LoadedData& loadedData)
{



    this->Dataloader::features(indb,
                             folder_name_parameterization,
                             inputfolder,
                             dyn_pop_sce,
                             dyn_alloc_sce,
                             biolsce,
                             fleetsce,
                             paramsForLoad,
                             loadedData);
  cout << "Loading pops features" << endl;


  // paramsForLoad.sparam1; // a_month
  // paramsForLoad.sparam2;// a_quarter
  // paramsForLoad.sparam3; //a_semester
  // paramsForLoad.iparam1; //nbpops
  // paramsForLoad.iparam2; //NBAGE
  // paramsForLoad.iparam3; //NBSZGROUP




  map<int, string> pop_names;
  read_pop_names_in_string(pop_names, folder_name_parameterization, inputfolder);

  vector<string > type_of_avai_field_to_read(paramsForLoad.iparam1);

  for (int st=0; st < paramsForLoad.iparam1; st++)
  {
     type_of_avai_field_to_read.at(st) ="";
  }
  string str_rand_avai_file="baseline"; // deprecated?
  // by default, will use the initial avai input

  // read the pop-specific betas related to the availability
  // szgroup0
  multimap<int, double> avai0_betas = read_avai_betas(paramsForLoad.sparam3, "0", folder_name_parameterization, inputfolder);
  // szgroup2
  multimap<int, double> avai2_betas = read_avai_betas(paramsForLoad.sparam3, "2", folder_name_parameterization, inputfolder);
  // szgroup3
  multimap<int, double> avai3_betas = read_avai_betas(paramsForLoad.sparam3, "3", folder_name_parameterization, inputfolder);
  // szgroup5
  multimap<int, double> avai5_betas = read_avai_betas(paramsForLoad.sparam3, "5", folder_name_parameterization, inputfolder);
  // szgroup7
  multimap<int, double> avai7_betas = read_avai_betas(paramsForLoad.sparam3, "7", folder_name_parameterization, inputfolder);

  // read other stuffs...
  // CAUTION: DO NOT LEFT BLANK AT THE END OF THE FILES!!!!  // CAUTION: DO NOT LEFT BLANK AT THE END OF THE FILES!!!!
  cout << "Do the pop files init_pops_per_szgroup need a check?" << endl;
  multimap<int, double> init_pops_per_szgroup = read_init_pops_per_szgroup(folder_name_parameterization, inputfolder,
                                                                           biolsce);
  cout << "Do the pop files init_prop_migrants_pops_per_szgroup need a check?" << endl;
  multimap<int, double> init_prop_migrants_pops_per_szgroup = read_init_prop_migrants_pops_per_szgroup(
          folder_name_parameterization, inputfolder, biolsce);
  cout << "Do the pop files init_fecundity_per_szgroup need a check?" << endl;
  multimap<int, double> init_fecundity_per_szgroup = read_init_fecundity_per_szgroup(folder_name_parameterization,
                                                                                     inputfolder, biolsce);
  cout << "Do the pop files init_maturity_per_szgroup need a check?" << endl;
  multimap<int, double> init_maturity_per_szgroup = read_init_maturity_per_szgroup(folder_name_parameterization,
                                                                                   inputfolder, biolsce);
  cout << "Do the pop files init_weight_per_szgroupneed a check?" << endl;
  multimap<int, double> init_weight_per_szgroup = read_init_weight_per_szgroup(folder_name_parameterization,
                                                                               inputfolder, biolsce);
  cout << "Do the pop files init_comcat_per_szgroup need a check?" << endl;
  multimap<int, int> init_comcat_per_szgroup = read_init_comcat_per_szgroup(folder_name_parameterization,
                                                                            inputfolder);
  cout << "Do the pop files init_M_per_szgroup need a check?" << endl;
  multimap<int, double> init_M_per_szgroup = read_init_M_per_szgroup(folder_name_parameterization, inputfolder,
                                                                     biolsce);
  cout << "Do the pop files init_proprecru_per_szgroup need a check?" << endl;
  multimap<int, double> init_proprecru_per_szgroup = read_init_proprecru_per_szgroup(folder_name_parameterization,
                                                                                     inputfolder, biolsce);
  //cout << "Do the pop files lst_idx_nodes_per_pop need a check?" << endl;
  multimap<int, types::NodeId> lst_idx_nodes_per_pop = read_lst_idx_nodes_per_pop(paramsForLoad.sparam3,
                                                                                  folder_name_parameterization,
                                                                                  inputfolder, str_rand_avai_file);

  cout << "Do the pop files selected_szgroups need a check?" << endl;
  multimap<int, int> selected_szgroups = read_selected_szgroups_per_pop(folder_name_parameterization, inputfolder);
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


  cout << "Does the pop file tac_percent_simulated need a check?" << endl;
  map<int, int> tac_percent_simulated = read_tac_percent_simulated(folder_name_parameterization, inputfolder);
  cout << "Does the pop file hyperstability_param need a check?" << endl;
  map<int, double> hyperstability_param = read_hyperstability_param(folder_name_parameterization, inputfolder);


  // input data, read proportion of natural mortality from other species when spatial co-occurences on node
  cout << "Do the species_interactions_mortality_proportion_matrix creation  need a check?" << endl;
  vector<vector<double> > species_interactions_mortality_proportion_matrix = read_species_interactions_mortality_proportion_matrix(
          paramsForLoad.iparam1, folder_name_parameterization, inputfolder, biolsce);



  // get the name of the pops
  // copy only unique elements of init_pops_per_szgroup into name_pops
  // DEADLY BUG: MAKE SURE THAT NO BLANK IS LEFT IN THE VERY END OF THE .DAT FILE...
  cout << "Do the name_pops creation  need a check?" << endl;
  vector<int> name_pops;
  for (multimap<int, double>::iterator iter = init_pops_per_szgroup.begin(); iter != init_pops_per_szgroup.end();
       iter = init_pops_per_szgroup.upper_bound(iter->first)) {
      name_pops.push_back(iter->first);
      cout << "pop " << iter->first << endl;

  }
  cout << "nb pops: " << name_pops.size() << endl;
  cout << "if you have a problem of overflow here then check if you forgot a blank at the end of N_at_szgroup.dat! "
       << endl;





  // initiate for loading loop over sp
  vector <double > vect_of_avai0_beta_v(name_pops.size());
  vector <double > vect_of_avai2_beta_v(name_pops.size());
  vector <double > vect_of_avai3_beta_v(name_pops.size());
  vector <double > vect_of_avai5_beta_v(name_pops.size());
  vector <double > vect_of_avai7_beta_v(name_pops.size());
  vector <vector<int> > vect_of_init_selected_szgroups_vov(name_pops.size());
  vector <vector<double> > vect_of_init_tot_N_per_szgroup_vov(name_pops.size());
  vector <vector<double> > vect_of_init_prop_migrants_in_N_per_szgroup_vov(name_pops.size());
  vector <vector<double> > vect_of_init_fecundity_per_szgroup_vov(name_pops.size());
  vector <vector<double> > vect_of_init_maturity_per_szgroup_vov(name_pops.size());
  vector <vector<double> > vect_of_init_weight_per_szgroup_vov(name_pops.size());
  vector <vector<int> > vect_of_init_comcat_per_szgroup_vov(name_pops.size());
  vector <vector<double> > vect_of_init_M_per_szgroup_vov(name_pops.size());
  vector <vector<double> > vect_of_init_proprecru_per_szgroup_vov(name_pops.size());

  //vector <multimap<types::NodeId, double> > vect_of_avai_szgroup_nodes_with_pop_mmap(name_pops.size());
  vector <multimap<types::NodeId, double> > vect_of_full_avai_szgroup_nodes_with_pop_mmap(name_pops.size());
  vector <multimap<types::NodeId, double> > vect_of_field_of_coeff_diffusion_this_pop_mmap(name_pops.size());
  vector <map<types::NodeId, double> > vect_of_oth_land_map(name_pops.size());
  vector <map<string, double> > vect_of_relative_stability_key_map(name_pops.size());
  vector <vector<vector<double> > > vect_of_growth_transition_matrix_vov(name_pops.size());
  vector <vector<vector<double> > > vect_of_percent_szgroup_per_age_matrix_vov(name_pops.size());
  vector <vector<vector<double> > > vect_of_percent_age_per_szgroup_matrix_vov(name_pops.size());
  vector <vector<double> > vect_of_param_sr_v(name_pops.size());
  vector <vector<double> > vect_of_fbar_ages_min_max_and_ftarget_this_pop_v(name_pops.size());
  vector <vector<double> > vect_of_tac_this_pop(name_pops.size());
  vector <multimap<int, double> > vect_of_overall_migration_fluxes_mmap(name_pops.size());
  vector<double> landings_so_far(name_pops.size());

  vector <vector<types::NodeId> > vect_of_lst_idx_nodes_per_pop_vov(name_pops.size());

  vector <string> popnames (name_pops.size());

  for (unsigned int sp = 0; sp < paramsForLoad.iparam1; sp++) {
      dout(cout << endl);

      popnames.at(sp) = pop_names[sp];
      cout << "pop_name: " << sp << ": " <<  popnames.at(sp) << endl;


      // avai0 beta for this particular pop
      multimap<int, double>::iterator lower_0 = avai0_betas.lower_bound(sp);
      multimap<int, double>::iterator upper_0 = avai0_betas.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_0; pos != upper_0; pos++)
          vect_of_avai0_beta_v.at(sp) = pos->second;

      // avai2 beta for this particular pop
      multimap<int, double>::iterator lower_2 = avai2_betas.lower_bound(sp);
      multimap<int, double>::iterator upper_2 = avai2_betas.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_2; pos != upper_2; pos++)
          vect_of_avai2_beta_v.at(sp)= pos->second;

      // avai3 beta for this particular pop
      multimap<int, double>::iterator lower_3 = avai3_betas.lower_bound(sp);
      multimap<int, double>::iterator upper_3 = avai3_betas.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_3; pos != upper_3; pos++)
          vect_of_avai3_beta_v.at(sp)= pos->second;

      // avai5 beta for this particular pop
      multimap<int, double>::iterator lower_5 = avai5_betas.lower_bound(sp);
      multimap<int, double>::iterator upper_5 = avai5_betas.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_5; pos != upper_5; pos++)
          vect_of_avai5_beta_v.at(sp)= pos->second;

      // avai7 beta for this particular pop
      multimap<int, double>::iterator lower_7 = avai7_betas.lower_bound(sp);
      multimap<int, double>::iterator upper_7 = avai7_betas.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_7; pos != upper_7; pos++)
          vect_of_avai7_beta_v.at(sp) = pos->second;

      // initial selected szgroups
      multimap<int, int>::iterator lower_init_selsz = selected_szgroups.lower_bound(sp);
      multimap<int, int>::iterator upper_init_selsz = selected_szgroups.upper_bound(sp);
      for (multimap<int, int>::iterator pos = lower_init_selsz; pos != upper_init_selsz; pos++)
          // convert in thousands
          vect_of_init_selected_szgroups_vov.at(sp).push_back(pos->second);

      // initial N for this particular pop
      multimap<int, double>::iterator lower_init = init_pops_per_szgroup.lower_bound(sp);
      multimap<int, double>::iterator upper_init = init_pops_per_szgroup.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_init; pos != upper_init; pos++)
          // !!!! CONVERT FROM THOUSANDS TO ABSOLUTE NUMBERS N  !!!!
          vect_of_init_tot_N_per_szgroup_vov.at(sp).push_back(pos->second * 1000);
      // !!!! CONVERT FROM THOUSANDS TO ABSOLUTE NUMBERS N  !!!!
      cout << "Caution: we remind you that DISPLACE expects input initial N in THOUSANDS...Did you check? " << endl;

      // initial prop_migrants for this particular pop
      multimap<int, double>::iterator lower_init_migrants = init_prop_migrants_pops_per_szgroup.lower_bound(sp);
      multimap<int, double>::iterator upper_init_migrants = init_prop_migrants_pops_per_szgroup.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_init_migrants; pos != upper_init_migrants; pos++)
          // convert in thousands
          vect_of_init_prop_migrants_in_N_per_szgroup_vov.at(sp).push_back(pos->second);

      // initial fecundity for this particular pop
      multimap<int, double>::iterator lower_init_fec = init_fecundity_per_szgroup.lower_bound(sp);
      multimap<int, double>::iterator upper_init_fec = init_fecundity_per_szgroup.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_init_fec; pos != upper_init_fec; pos++)
          vect_of_init_fecundity_per_szgroup_vov.at(sp).push_back(pos->second);

      // initial maturity for particular this pop
      multimap<int, double>::iterator lower_init_ma = init_maturity_per_szgroup.lower_bound(sp);
      multimap<int, double>::iterator upper_init_ma = init_maturity_per_szgroup.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_init_ma; pos != upper_init_ma; pos++)
          vect_of_init_maturity_per_szgroup_vov.at(sp).push_back(pos->second);

      // initial weight for this particular pop
      multimap<int, double>::iterator lower_init_we = init_weight_per_szgroup.lower_bound(sp);
      multimap<int, double>::iterator upper_init_we = init_weight_per_szgroup.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_init_we; pos != upper_init_we; pos++)
          vect_of_init_weight_per_szgroup_vov.at(sp).push_back(pos->second);

      // initial comcat for this particular pop
      multimap<int, int>::iterator lower_init_cc = init_comcat_per_szgroup.lower_bound(sp);
      multimap<int, int>::iterator upper_init_cc = init_comcat_per_szgroup.upper_bound(sp);
      for (multimap<int, int>::iterator pos = lower_init_cc; pos != upper_init_cc; pos++)
          vect_of_init_comcat_per_szgroup_vov.at(sp).push_back(pos->second);

      // initial M for this particular pop
      multimap<int, double>::iterator lower_init_M = init_M_per_szgroup.lower_bound(sp);
      multimap<int, double>::iterator upper_init_M = init_M_per_szgroup.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_init_M; pos != upper_init_M; pos++)
          vect_of_init_M_per_szgroup_vov.at(sp).push_back(pos->second);

      // initial proprecru for this particular pop
      multimap<int, double>::iterator lower_init_proprecru = init_proprecru_per_szgroup.lower_bound(sp);
      multimap<int, double>::iterator upper_init_proprecru = init_proprecru_per_szgroup.upper_bound(sp);
      for (multimap<int, double>::iterator pos = lower_init_proprecru; pos != upper_init_proprecru; pos++)
          vect_of_init_proprecru_per_szgroup_vov.at(sp).push_back(pos->second);

      // input data, avai per szgroup on nodes and presence of the pop
      //vect_of_avai_szgroup_nodes_with_pop_mmap.at(sp) = read_avai_szgroup_nodes_with_pop(a_semester, sp,
      //                                                                                               folder_name_parameterization,
      //                                                                                               inputfolder,
      //                                                                                               str_rand_avai_file,
      //                                                                                               type_of_avai_field_to_read);
      vect_of_full_avai_szgroup_nodes_with_pop_mmap.at(sp) = read_full_avai_szgroup_nodes_with_pop(
                                                                                         paramsForLoad.sparam3, sp,
                                                                                                     folder_name_parameterization,
                                                                                                     inputfolder, str_rand_avai_file,
                                                                                                     type_of_avai_field_to_read);


      // get the vector of nodes of presence for this pop (an optimization to avoid looping over all nodes...)
      outc(cout << "first find the list of nodes with presence for this pop (this quarter)..." << endl);
      auto lower_pop = lst_idx_nodes_per_pop.lower_bound(sp);
      auto upper_pop = lst_idx_nodes_per_pop.upper_bound(sp);
      for (multimap<int, types::NodeId>::iterator a_pos = lower_pop; a_pos != upper_pop; a_pos++) {
          vect_of_lst_idx_nodes_per_pop_vov.at(sp).push_back(a_pos->second);
      }


      // input data
      if (dyn_pop_sce.option(Options::diffusePopN)) {
          cout << "read_field_of_coeff_diffusion_this_pop ..." << endl;
          vect_of_field_of_coeff_diffusion_this_pop_mmap.at(sp)  = read_field_of_coeff_diffusion_this_pop(paramsForLoad.sparam3, sp,
                                                                                     folder_name_parameterization,
                                                                                     inputfolder, biolsce);
      }

      // input data, read a other landings per node for this species
      vect_of_oth_land_map.at(sp)  = read_oth_land_nodes_with_pop(paramsForLoad.sparam3, paramsForLoad.sparam1, sp,
                                                                         folder_name_parameterization, inputfolder,
                                                                         fleetsce);

      vect_of_relative_stability_key_map.at(sp) = read_relative_stability_keys(paramsForLoad.sparam3, sp,
                                                                                folder_name_parameterization,
                                                                                inputfolder);

      // input data, growth transition, percent_szgroup_per_age_matrix
      vect_of_growth_transition_matrix_vov.at(sp) = read_growth_transition_matrix(sp, paramsForLoad.iparam3,
                                                                                       folder_name_parameterization,
                                                                                       inputfolder, biolsce);

      vect_of_percent_szgroup_per_age_matrix_vov.at(sp) = read_percent_szgroup_per_age_matrix(sp, paramsForLoad.iparam3,
                                                                                                   paramsForLoad.iparam2,
                                                                                                   folder_name_parameterization,
                                                                                                   inputfolder,
                                                                                                   biolsce);

      vect_of_percent_age_per_szgroup_matrix_vov.at(sp) = read_percent_age_per_szgroup_matrix(sp, paramsForLoad.iparam3,
                                                                                                   paramsForLoad.iparam2,
                                                                                                   folder_name_parameterization,
                                                                                                   inputfolder,
                                                                                                   biolsce);

      // input data, parameter for stock-recruitment relationship
      vect_of_param_sr_v.at(sp) = read_param_sr(sp, folder_name_parameterization, inputfolder, biolsce);

      // input data, fbar ages
      vect_of_fbar_ages_min_max_and_ftarget_this_pop_v.at(sp) = read_fbar_ages_min_max_and_ftarget(sp,
                                                                                                 folder_name_parameterization,
                                                                                                 inputfolder,
                                                                                                 biolsce);

      // input data, initial tac
      vect_of_tac_this_pop.at(sp) = read_initial_tac(sp, folder_name_parameterization, inputfolder);
      cout << "initial tac has been read correctly" << endl;


      if (dyn_alloc_sce.option(Options::TACs) && vect_of_tac_this_pop.at(sp).at(0) == 0) {
          cout << "WARNING: TACs Option is active: Consider informing a initial TAC value for pop" << sp
               << "and potentially other pops..." << endl;
          cout << "a fake, non binding value is filled in for now" << endl;
          vect_of_tac_this_pop.at(sp).at(0) = 100000; // tons
      }


      // input data, read migration fluxes in proportion per size group (if any)
      dout(cout << "read overall migration..." << endl);
      vect_of_overall_migration_fluxes_mmap.at(sp) = read_overall_migration_fluxes(paramsForLoad.sparam3, sp,
                                                                                     folder_name_parameterization,
                                                                                     inputfolder, biolsce);
      dout(cout << "overall migration has been read correctly" << endl);


      landings_so_far.push_back(1.0);



}





  //  export 
  // used in the Population() creator
  loadedData.vectsparam1=popnames;
  loadedData.vectdparam1=vect_of_avai0_beta_v;
  loadedData.vectdparam2=vect_of_avai2_beta_v;
  loadedData.vectdparam3=vect_of_avai3_beta_v;
  loadedData.vectdparam4=vect_of_avai5_beta_v;
  loadedData.vectdparam5=vect_of_avai7_beta_v;
  loadedData.vovi1= vect_of_init_selected_szgroups_vov;
  loadedData.vovd1= vect_of_init_tot_N_per_szgroup_vov;
  loadedData.vovd2= vect_of_init_prop_migrants_in_N_per_szgroup_vov;
  loadedData.vovd3= vect_of_init_fecundity_per_szgroup_vov;
  loadedData.vovd4= vect_of_init_weight_per_szgroup_vov;
  loadedData.vovi2= vect_of_init_comcat_per_szgroup_vov;
  loadedData.vovd5= vect_of_init_maturity_per_szgroup_vov;
  loadedData.vovd6= vect_of_init_M_per_szgroup_vov;
  loadedData.vovd7= vect_of_init_proprecru_per_szgroup_vov;
  loadedData.vovd8= vect_of_param_sr_v;
  loadedData.vectmmapndparam1=vect_of_full_avai_szgroup_nodes_with_pop_mmap;
  loadedData.vectmmapndparam2=vect_of_field_of_coeff_diffusion_this_pop_mmap;
  loadedData.vectmapndparam1=vect_of_oth_land_map;
  loadedData.vectmmapidparam1=vect_of_overall_migration_fluxes_mmap;
  loadedData.vectmapsdparam1=vect_of_relative_stability_key_map;
  loadedData.vovovd2=vect_of_percent_szgroup_per_age_matrix_vov;
  loadedData.vovovd3=vect_of_percent_age_per_szgroup_matrix_vov;
  loadedData.vovovd1=vect_of_growth_transition_matrix_vov;
  // nodes,
  loadedData.vovd9= vect_of_fbar_ages_min_max_and_ftarget_this_pop_v;
  loadedData.vovd10= vect_of_tac_this_pop;
  loadedData.mapiiparam1=tac_percent_simulated;
  loadedData.mapidparam1=hyperstability_param;
  loadedData.vectdparam6=paramsForLoad.vdparam1; // calib_cpue_multiplier;
  loadedData.vectdparam7=paramsForLoad.vdparam2;  // calib_weight_at_szgroup;
 
  // not used in the Population() creator
  loadedData.vectiparam1=selected_szgroups_pop0;
  loadedData.vovn1=vect_of_lst_idx_nodes_per_pop_vov; // nodes_with_presence

  // used in simulator main.cpp
  loadedData.mmapidparam_init_weight_per_szgroup=init_weight_per_szgroup;
  loadedData.vovd_species_interactions_mortality_proportion_matrix=species_interactions_mortality_proportion_matrix;

  loadedData.vectsparam2=type_of_avai_field_to_read;

return 0;
}

