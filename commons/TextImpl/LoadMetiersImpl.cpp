//
// Created by happycactus on 4/15/20.
//

#include "TextfileModelLoader.h"
#include "TextImpl/Impl.h"
#include "Metier.h"
#include "SimModel.h"
#include "Calendar.h"
#include "../simulator/values.h"
#include "readdata.h"

#include <vector>
#include <string>

using namespace std;

// TODO: remove this, replace with a more portable definition
#define NBSZGROUP 14
#define NBAGE 11                 // nb age classes max
#define SEL_NBSZGROUP 5             // DEPRECATED - according to the R glm on cpue (see R code)

namespace {


static void
test(vector<vector<double> > const &selectivity_per_stock, vector<double> const &met_betas, double gear_width_a)
{
    // check selectivity per metier per stock
    for (unsigned int i = 0; i < selectivity_per_stock.size(); i++) {
        for (unsigned int j = 0; j < selectivity_per_stock[i].size(); j++) {

            cout << "pop is " << i << endl;
            cout << "szgroup is " << j << endl;
            cout << "selectivity_per_stock[i,j] is " << selectivity_per_stock[i][j] << endl;
        }
    }

    // check metier betas
    cout << "met_betas of the metier 0" << endl;
    for (int i = 0; i < met_betas.size(); i++) {
        cout << " " << met_betas[i] << " ";
    }
    cout << endl;

    // check gear_width_a
    cout << "gear_width_a of the metier 0" << endl;
    cout << " " << gear_width_a << " ";
    cout << endl;
}

}


struct [[deprecated]] LoadedData {
    std::vector<string> vectsparam1;
    std::vector<double> vectdparam1;
    std::vector<double> vectdparam2;
    std::vector<double> vectdparam3;
    std::vector<double> vectdparam4;
    std::vector<double> vectdparam5;
    std::vector<double> vectdparam6;
    std::vector<double> vectdparam7;
    std::vector<int> vectiparam1;
    std::vector<int> vectiparam2;
    std::vector<map<string, double> > vectmapsdparam1;
    std::map<int, int> mapiiparam1;
    std::map<int, double> mapidparam1;
    std::vector<map<types::NodeId, double> > vectmapndparam1;
    std::vector<multimap<types::NodeId, double> > vectmmapndparam1;
    std::vector<multimap<types::NodeId, double> > vectmmapndparam2;
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
    std::vector<vector<types::NodeId> > vovn1;
    std::vector<vector<int> > vovi1;
    std::vector<vector<int> > vovi2;
    std::vector<vector<int> > vovi3;
    std::vector<vector<int> > vovi4;
    std::vector<vector<vector<double> > > vovovd1;
    std::vector<vector<vector<double> > > vovovd2;
    std::vector<vector<vector<double> > > vovovd3;
};

struct [[deprecated]] ParamsForLoad {
    string sparam1;
    string sparam2;
    string sparam3;
    string sparam4;
    int iparam1;
    int iparam2;
    int iparam3;
    int iparam4;
    double dparam1;
    double dparam2;
    double dparam3;
    vector<double> vdparam1;
    vector<double> vdparam2;
};


void TextfileModelLoader::loadMetiers()
{
    ParamsForLoad paramsForLoad;

    paramsForLoad.sparam1 = std::to_string(model().month());
    paramsForLoad.sparam2 = std::to_string(model().quarter());
    paramsForLoad.sparam3 = std::to_string(model().semester());
    paramsForLoad.iparam1 = model().config().nbpops;
    paramsForLoad.iparam2 = NBAGE;
    paramsForLoad.iparam3 = NBSZGROUP;


    LoadedData loadedData;

    //input data, metier characteristics: selectivty ogives, beta per pop
    cout << "check whether all metiers informed in the following parameters files... " << endl;
    cout << "read metier betas parameters....ok? " << endl;
    multimap<int, double> metiers_betas = read_metiers_betas(paramsForLoad.sparam3, p->folder_name_parameterization,
                                                             p->inputfolder);
    cout << "read discardratio_limits parameters....ok? " << endl;
    multimap<int, double> discards_rate_limits = read_discardratio_limits(paramsForLoad.sparam3,
                                                                          p->folder_name_parameterization,
                                                                          p->inputfolder);
    cout << "read is_avoided_stocks parameters....ok? " << endl;
    multimap<int, int> is_avoided_stockss = read_is_avoided_stocks(paramsForLoad.sparam3,
                                                                   p->folder_name_parameterization,
                                                                   p->inputfolder);
    cout << "read mls cat parameters....ok? " << endl;
    multimap<int, int> metiers_mls_cat = read_metiers_mls_cat(paramsForLoad.sparam3, p->folder_name_parameterization,
                                                              p->inputfolder);
    cout << "read metiers types parameters....ok? " << endl;
    map<int, int> metiers_types = read_metiers_types(p->folder_name_parameterization, p->inputfolder);
    cout << "read revenue completeness parameters....ok? " << endl;
    map<int, double> percent_revenue_completenesses = read_percent_revenue_completenesses(
            p->folder_name_parameterization,
            p->inputfolder);
    cout << "read fspeed parameters....ok? " << endl;
    map<int, double> metiers_fspeed = read_metiers_fspeed(p->folder_name_parameterization, p->inputfolder);
    cout << "read metier gear width a parameters....ok? " << endl;
    map<int, double> metiers_gear_widths_param_a = read_gear_widths_param_a(p->folder_name_parameterization,
                                                                            p->inputfolder);
    cout << "read metier gear width b parameters....ok? " << endl;
    map<int, double> metiers_gear_widths_param_b = read_gear_widths_param_b(p->folder_name_parameterization,
                                                                            p->inputfolder);
    cout << "read metier gear width model type parameters....ok? " << endl;
    map<int, string> metiers_gear_widths_model_type = read_gear_widths_model_type(p->folder_name_parameterization,
                                                                                  p->inputfolder);

    // oth_land are not metier-specific by nature, but the reader is placed here for coherence...
    vector<vector<double> > selectivity_per_stock_ogives_for_oth_land = read_selectivity_per_stock_ogives_for_oth_land(
            paramsForLoad.iparam1, paramsForLoad.iparam3, p->folder_name_parameterization, p->inputfolder,
            model().scenario().fleetsce);

    // get the name of the metiers
    // copy only unique elements into name_metiers
    cout << "retrieve the metier names.... " << endl;
    vector<int> name_metiers;
    for (multimap<int, double>::iterator iter = metiers_gear_widths_param_a.begin();
         iter != metiers_gear_widths_param_a.end();
         iter = metiers_gear_widths_param_a.upper_bound(iter->first)) {
        name_metiers.push_back(iter->first);
        outc(cout << "metier " << iter->first << endl);
    }
    cout << "nb metiers: " << name_metiers.size() << endl;
    cout
            << "!!CAUTION!! nb metiers retrieved from the metier_gear_widths_param_a.dat file...do not forget the headers in this file! "
            << endl;






    // loop over metiers and fill in vectors
    vector<int> met_types(name_metiers.size());
    vector<double> met_speeds(name_metiers.size());
    vector<double> met_gear_widths_param_a(name_metiers.size());
    vector<double> met_gear_widths_param_b(name_metiers.size());
    vector<string> met_gear_widths_model_type(name_metiers.size());
    vector<double> met_percent_revenue_completenesses(name_metiers.size());
    vector<vector<double> > vect_of_metier_betas_vovd(name_metiers.size(), vector<double>(paramsForLoad.iparam1));
    vector<vector<double> > vect_of_discardratio_limits_vovd(name_metiers.size(),
                                                             vector<double>(paramsForLoad.iparam1));
    vector<vector<int> > vect_of_is_avoided_stocks_vovi(name_metiers.size(), vector<int>(paramsForLoad.iparam1));
    vector<vector<int> > vect_of_metier_mls_cat_vovi(name_metiers.size(), vector<int>(paramsForLoad.iparam1));
    vector<multimap<int, double> > vect_of_loss_after_1_passage_mmapid(name_metiers.size());
    vector<vector<int> > vect_of_the_metier_target_stocks_vovi(name_metiers.size());
    vector<vector<int> > vect_of_the_metier_suitable_seabottomtypes_vovi(name_metiers.size());
    vector<vector<vector<double> > > vect_of_selectivity_per_stock_ogives_vovovd(name_metiers.size());


    for (unsigned int i = 0; i < name_metiers.size(); i++) {

        int metier_name = i;
        outc(cout << "creating metier " << i << endl);

        met_types.at(i) = metiers_types[i];
        met_speeds.at(i) = metiers_fspeed[i];
        met_gear_widths_param_a.at(i) = metiers_gear_widths_param_a[i];
        met_gear_widths_param_b.at(i) = metiers_gear_widths_param_b[i];
        met_gear_widths_model_type.at(i) = metiers_gear_widths_model_type[i];
        met_percent_revenue_completenesses.at(i) = percent_revenue_completenesses[i];

        vect_of_metier_betas_vovd.at(i) = find_entries_i_d(metiers_betas, metier_name);
        cout << "Read metier_betas this met " << i << endl;
        for (int ii = 0; ii < vect_of_metier_betas_vovd.at(i).size(); ++ii)
            cout << vect_of_metier_betas_vovd.at(i).at(ii) << " ";
        cout << endl;

        vect_of_discardratio_limits_vovd.at(i) = find_entries_i_d(discards_rate_limits, metier_name);

        vect_of_is_avoided_stocks_vovi.at(i) = find_entries_i_i(is_avoided_stockss, metier_name);

        vect_of_metier_mls_cat_vovi.at(i) = find_entries_i_i(metiers_mls_cat, metier_name);
        cout << "Read metier_mls this met " << i << endl;
        for (int ii = 0; ii < vect_of_metier_mls_cat_vovi.at(i).size(); ++ii)
            cout << vect_of_metier_mls_cat_vovi.at(i).at(ii) << " ";
        cout << endl;

        vect_of_loss_after_1_passage_mmapid.at(i) = read_loss_after_1_passage_per_landscape_per_func_group(metier_name,
                                                                                                           p->folder_name_parameterization,
                                                                                                           p->inputfolder);


        vect_of_selectivity_per_stock_ogives_vovovd.at(i) = read_selectivity_per_stock_ogives(i, paramsForLoad.iparam1,
                                                                                              paramsForLoad.iparam3,
                                                                                              p->folder_name_parameterization,
                                                                                              p->inputfolder,
                                                                                              model().scenario().fleetsce);

        multimap<int, int> metier_target_stocks = read_metier_target_stocks(metier_name,
                                                                            p->folder_name_parameterization,
                                                                            p->inputfolder);
        multimap<int, int> metier_suitable_seabottomtypes = read_metier_suitable_seabottomtypes(metier_name,
                                                                                                p->folder_name_parameterization,
                                                                                                p->inputfolder);

        // metier_target_stocks for this particular metier
        multimap<int, int>::iterator lower_metier_target_stocks = metier_target_stocks.lower_bound(i);
        multimap<int, int>::iterator upper_metier_target_stocks = metier_target_stocks.upper_bound(i);
        for (multimap<int, int>::iterator pos = lower_metier_target_stocks; pos != upper_metier_target_stocks; pos++)
            vect_of_the_metier_target_stocks_vovi.at(i).push_back(pos->second);

        // metier_suitable_seabottomtypes for this particular metier
        multimap<int, int>::iterator lower_metier_suitable_seabottomtypes = metier_suitable_seabottomtypes.lower_bound(
                i);
        multimap<int, int>::iterator upper_metier_suitable_seabottomtypes = metier_suitable_seabottomtypes.upper_bound(
                i);
        for (multimap<int, int>::iterator pos = lower_metier_suitable_seabottomtypes;
             pos != upper_metier_suitable_seabottomtypes; pos++)
            vect_of_the_metier_suitable_seabottomtypes_vovi.at(i).push_back(pos->second);

    }


    cout << "export metier loaded data back to simulator.cpp " << endl;

    //  export
    // for Metier() creator
    loadedData.vectiparam1 = name_metiers;
    loadedData.vectiparam2 = met_types;
    loadedData.vectdparam1 = met_percent_revenue_completenesses;
    loadedData.vovovd1 = vect_of_selectivity_per_stock_ogives_vovovd;
    loadedData.vovd1 = vect_of_metier_betas_vovd;
    loadedData.vovd2 = vect_of_discardratio_limits_vovd;
    loadedData.vovi1 = vect_of_is_avoided_stocks_vovi;
    loadedData.vovi2 = vect_of_metier_mls_cat_vovi;
    loadedData.vectdparam2 = met_speeds;
    loadedData.vectdparam3 = met_gear_widths_param_a;
    loadedData.vectdparam4 = met_gear_widths_param_b;
    loadedData.vectsparam1 = met_gear_widths_model_type;
    loadedData.vectmmapidparam1 = vect_of_loss_after_1_passage_mmapid;
    loadedData.vovi3 = vect_of_the_metier_target_stocks_vovi;
    loadedData.vovi4 = vect_of_the_metier_suitable_seabottomtypes_vovi;

    // NOT for Metier() creator
    loadedData.vovd5 = selectivity_per_stock_ogives_for_oth_land;

    cout << "export metier loaded data back to simulator.cpp....done " << endl;






    // creation of a vector of metier from input data...
    name_metiers = loadedData.vectiparam1;


    if (model().metiers().size() == 0) {
        auto metiers = vector<Metier *>(name_metiers.size());

        for (unsigned int i = 0; i < name_metiers.size(); i++) {

            cout << "Create metier " << i << endl;


            metiers[i] = new Metier(loadedData.vectiparam1.at(i),
                                    loadedData.vectiparam2.at(i),
                                    loadedData.vectdparam1.at(i),
                                    loadedData.vovovd1.at(i),
                                    loadedData.vovd1.at(i),
                                    loadedData.vovd2.at(i),
                                    loadedData.vovi1.at(i),
                                    loadedData.vovi2.at(i),
                                    loadedData.vectdparam2.at(i),
                                    loadedData.vectdparam3.at(i),
                                    loadedData.vectdparam4.at(i),
                                    loadedData.vectsparam1.at(i),
                                    loadedData.vectmmapidparam1.at(i),
                                    loadedData.vovi3.at(i),
                                    loadedData.vovi4.at(i));

            cout << "Create metier " << i << "...done" << endl;

            selectivity_per_stock_ogives_for_oth_land = loadedData.vovd5;
        }

        model().setMetiers(std::move(metiers));
    } else {
        auto &metiers = model().metiers();
        for (unsigned int m = 0; m < metiers.size(); m++) {
            // casting m into a string
            //stringstream out;
            //out << m;
            //string a_met = "met" + out.str();
            metiers[m]->set_betas_per_pop(loadedData.vovd1.at(m));
            metiers[m]->set_discardratio_limits(loadedData.vovd2.at(m));
            metiers[m]->set_is_avoided_stocks(loadedData.vovi1.at(m));

        }
    }

    test(model().metiers()[0]->get_selectivity_per_stock_ogives(),
         model().metiers()[0]->get_betas_per_pop(),
         model().metiers()[0]->get_gear_width_a());
}