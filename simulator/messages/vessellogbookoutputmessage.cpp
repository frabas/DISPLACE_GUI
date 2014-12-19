#include "vessellogbookoutputmessage.h"

#include <Vessel.h>

#include <mutexlocker.h>

extern pthread_mutex_t glob_mutex;
extern bool use_gui;

VesselLogbookOutputMessage::VesselLogbookOutputMessage(std::ostream &strm, unsigned int _tstep, Vessel *v, const std::vector<Population* >& populations)
    : loglike(strm)
{
    tstep = _tstep;
    tstepdep = v->get_tstep_dep();
    rtbb = v->get_reason_to_go_back();
    cumstm = v->get_cumsteaming();
    node = v->get_loc()->get_idx_node();
    idx = v->get_idx();
    name = v->get_name();
    timeatsea = v->get_timeatsea();
    cumfcons = v->get_cumfuelcons();
    travdist = v->get_traveled_dist_this_trip();

    vector< vector<double> > a_catch_pop_at_szgroup = v->get_catch_pop_at_szgroup();
    cumul.resize(a_catch_pop_at_szgroup.size(), 0);
    revenue=0.0;
    freq_metiers="";
    for(unsigned int pop = 0; pop < a_catch_pop_at_szgroup.size(); pop++)
    {
        cumul[pop]=0;
        for(unsigned int sz = 0; sz < a_catch_pop_at_szgroup[pop].size(); sz++)
        {
            cumul[pop] = cumul[pop]+ a_catch_pop_at_szgroup[pop][sz];
        }

        // add revenue from this stock to cumulated revenue from landings
        if(cumul[pop]!=0)			 // if some landings for this pop
        {
            freq_metiers.clear();
            vector<int> vec = v->get_idx_used_metiers_this_trip();
            typedef map<int, unsigned> counts_t;

            // Create the histogram
            counts_t histogram;
            for (vector<int>::const_iterator i = vec.begin(); i != vec.end(); i++)
            {
                ++histogram[*i];
            }
            // ... and display it: loop over metier
            for (counts_t::const_iterator i = histogram.begin(); i != histogram.end(); i++)
            {
                int a_met = i->first;
                //cout << " and this metier " << a_met << endl;
                stringstream out;
                out << a_met;

                double freq_this_met = static_cast<double>(i->second) / vec.size();
                double price_this_stock =0;

                                 // cumulated to revenue
                revenue+= cumul[pop]*price_this_stock*freq_this_met;
                out << ":" << freq_this_met;
                freq_metiers+="_"+out.str();
            }
        }
    }							 // end pop

    revenue_from_av_prices=0.0;
    for(unsigned int pop = 0; pop < a_catch_pop_at_szgroup.size(); pop++)
    {
        vector<int> comcat_at_szgroup =   populations[pop]->get_comcat_at_szgroup();

        for(unsigned int sz = 0; sz < a_catch_pop_at_szgroup[pop].size(); sz++)
        {
            int comcat_this_size =comcat_at_szgroup.at(sz);
            revenue_from_av_prices += a_catch_pop_at_szgroup[pop][sz] * v->get_loc()->get_prices_per_cat(pop, comcat_this_size);
        }
    }

    length_class =v->get_length_class();
    fuelcost = v->get_cumfuelcons() * v->get_loc()->get_fuelprices(length_class);
    gav=revenue-fuelcost;
    gav2=revenue_from_av_prices-fuelcost;
}

bool VesselLogbookOutputMessage::send()
{
    std::ostringstream ss;

    ss << setprecision(0) << fixed;
    // vessel / date dep / date arr / reason to return to port / cum steaming in hours/
    //    idx node harbour / idx vessel / name vessel / fuelcons /tot catch per pop
    ss << tstepdep << " " << tstep << " "
        << rtbb << " "
        << cumstm << " "
        << node << " "
        << idx << " "
        << name << " "
        << timeatsea << " "
        << cumfcons << " "
        << travdist << " ";
    for (std::vector<double>::iterator it = cumul.begin(); it != cumul.end(); ++it)
        ss  << *it << " " ;
    ss  << freq_metiers << " " << revenue << " " ;
    ss  << revenue_from_av_prices << " " ;
    ss  << fuelcost << " " ;
    ss  << gav << " " ;
    ss  << gav2 << " " ;
    ss  << " " << std::endl;

    MutexLocker l(&glob_mutex);

    loglike << ss.str();
    if (use_gui) {
        std::cout << "=v" << ss.str();
    }

    return true;
}
