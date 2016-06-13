#include "outputexporter.h"

#include <mutexlocker.h>
#include <Vessel.h>

#include <Node.h>

extern pthread_mutex_t glob_mutex;
using namespace std;

std::shared_ptr<OutputExporter> OutputExporter::mInstance = nullptr;

OutputExporter::OutputExporter(const string &basepath, const string &namesimu)
{
    auto filename=basepath + "/vmslike_" + namesimu + ".dat";
    mVmsLike.open(filename.c_str());

    filename=basepath + "/loglike_" + namesimu + ".dat";
    mLogLike.open(filename.c_str());
}

void OutputExporter::exportVmsLike(unsigned int tstep, Vessel *vessel)
{
    MutexLocker l(&glob_mutex);

    mVmsLike << tstep << " "
                //<< vessels[ index_v ]->get_idx() << " "
             << vessel->get_name() << " "
                // can be used as a trip identifier
             << vessel->get_tstep_dep() << " "
             << setprecision(3) << fixed << vessel->get_x() << " "
             << setprecision(3) << fixed << vessel->get_y() << " "
             << setprecision(0) << fixed << vessel->get_course() << " "
                //<< vessels[ index_v ]->get_inharbour() << " "
             << setprecision(0) << fixed << vessel->get_cumfuelcons() << " "
             << vessel->get_state() << " " <<  endl;
}

void OutputExporter::exportLogLike(unsigned int tstep, Vessel *v, const std::vector<Population *> &populations, vector<int> &implicit_pops)
{
    std::string name, freq_metiers= "M";
    int length_class;

    std::vector<double> cumul;
    std::vector<double> cumul_discards;

    struct VesselLogbookMessage {
        unsigned int tstep, tstepdep;
        int rtbb, node, idx;
        double cumstm, timeatsea,cumfcons,travdist, revenue_from_av_prices, revenue_explicit_from_av_prices, fuelcost, gav2;
        size_t popnum;
        double pop[];
    } logbook;

    logbook.tstep = tstep;
    logbook.tstepdep = v->get_tstep_dep();
    logbook.rtbb = v->get_reason_to_go_back();
    logbook.cumstm = v->get_cumsteaming();
    logbook.node = v->get_loc()->get_idx_node();
    logbook.idx = v->get_idx();
    name = v->get_name();
    logbook.timeatsea = v->get_timeatsea();
    logbook.cumfcons = v->get_cumfuelcons();
    logbook.travdist = v->get_traveled_dist_this_trip();

    logbook.revenue_from_av_prices=v->getLastTripRevenues();
    logbook.revenue_explicit_from_av_prices=v->getLastTripExplicitRevenues();

    // fill in for catches
    int NBSZGROUP=14;
    vector< vector<double> > a_catch_pop_at_szgroup(populations.size(), vector<double>(NBSZGROUP));
    a_catch_pop_at_szgroup = v->get_catch_pop_at_szgroup();
    for(size_t pop = 0; pop < a_catch_pop_at_szgroup.size(); pop++)
    {
        cumul.push_back(0);
        for(size_t sz = 0; sz < a_catch_pop_at_szgroup[pop].size(); sz++)
        {
            cumul.at(pop) = cumul.at(pop)+ a_catch_pop_at_szgroup[pop][sz];
        }
    }


    // inform frequencies of metiers by counting nb of time a metier has been used during the trip
    vector<int> vec = v->get_idx_used_metiers_this_trip();
    // Create an histogram
    map<int, unsigned> histogram;
    for (vector<int>::const_iterator i = vec.begin(); i != vec.end(); i++)
    {
        ++histogram[*i];
    }
    // ... and display it.
    for (map<int, unsigned>::const_iterator i = histogram.begin(); i != histogram.end(); i++)
    {
        freq_metiers= freq_metiers + "_" ;
        double freq = static_cast<double>(i->second) / vec.size();
        freq = floor(freq * 100.0) / 100.0; // 2 digits after decimal only
        stringstream ss, ss2;
        ss <<  i->first;
        ss2 <<  freq;
        string str = ss.str();
        string str2 = ss2.str();
        freq_metiers  = freq_metiers + str + ":" + str2 ;

    }




    vector< vector<double> > a_discards_pop_at_szgroup(populations.size(), vector<double>(NBSZGROUP));
    a_discards_pop_at_szgroup = v->get_discards_pop_at_szgroup();
    int count =0;
    for(int pop = 0; pop < a_discards_pop_at_szgroup.size(); pop++)
    {

        if (!binary_search (implicit_pops.begin(), implicit_pops.end(),  pop  ))
        {
            cumul_discards.push_back(0);
            for(int sz = 0; sz < a_discards_pop_at_szgroup[pop].size(); sz++)
            {
                if(isfinite(a_discards_pop_at_szgroup[pop][sz])) cumul_discards.at(count) +=  a_discards_pop_at_szgroup[pop][sz];
            }
            count+=1;
        }

    }


    length_class =v->get_length_class();
    logbook.fuelcost = v->get_cumfuelcons() * v->get_loc()->get_fuelprices(length_class);
    logbook.gav2=logbook.revenue_from_av_prices-logbook.fuelcost;

    std::ostringstream ss;

    ss << setprecision(0) << fixed;
    // vessel / date dep / date arr / reason to return to port / cum steaming in hours/
    //    idx node harbour / idx vessel / name vessel / fuelcons /tot catch per pop
    ss << logbook.tstepdep << " " << logbook.tstep << " "
       << logbook.rtbb << " "
       << logbook.cumstm << " "
       << logbook.node << " "
       << logbook.idx << " "
       << name << " "
       << logbook.timeatsea << " "
       << logbook.cumfcons << " "
       << logbook.travdist << " ";
    for (std::vector<double>::iterator it = cumul.begin(); it != cumul.end(); ++it)
        ss  << *it << " " ;
    ss  << freq_metiers << " " << 0 << " " ;
    ss  << logbook.revenue_from_av_prices << " " ;
    ss  << logbook.revenue_explicit_from_av_prices << " " ;
    ss  << logbook.fuelcost << " " ;
    ss  << 0 << " " ;
    ss  << logbook.gav2 << " " ;
    for (std::vector<double>::iterator it2 = cumul_discards.begin(); it2 != cumul_discards.end(); ++it2)
        ss  << *it2 << " " ;
    ss  << " " << std::endl;

    MutexLocker l(&glob_mutex);

    mLogLike << ss.str();
}

void OutputExporter::close()
{
    mVmsLike.close();
}

bool OutputExporter::instantiate(const string &basepath, const string &namesimu)
{
    mInstance = std::make_shared<OutputExporter>(basepath, namesimu);
    return true;
}
