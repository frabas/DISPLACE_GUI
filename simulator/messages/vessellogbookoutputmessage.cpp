#include "vessellogbookoutputmessage.h"

#include <Vessel.h>
#include <helpers.h>
#include <mutexlocker.h>

extern pthread_mutex_t glob_mutex;
extern bool use_gui;

VesselLogbookOutputMessage::VesselLogbookOutputMessage(std::ostream &strm, unsigned int _tstep, Vessel *v, const std::vector<Population* >& populations)
    : loglike(strm)
{
    logbook.tstep = _tstep;
    logbook.tstepdep = v->get_tstep_dep();
    logbook.rtbb = v->get_reason_to_go_back();
    logbook.cumstm = v->get_cumsteaming();
    logbook.node = v->get_loc()->get_idx_node();
    logbook.idx = v->get_idx();
    name = v->get_name();
    logbook.timeatsea = v->get_timeatsea();
    logbook.cumfcons = v->get_cumfuelcons();
    logbook.travdist = v->get_traveled_dist_this_trip();

    vector< vector<double> > a_catch_pop_at_szgroup = v->get_catch_pop_at_szgroup();
    cumul.resize(a_catch_pop_at_szgroup.size(), 0);
    logbook.revenue=0.0;
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
                logbook.revenue+= cumul[pop]*price_this_stock*freq_this_met;
                out << ":" << freq_this_met;
                freq_metiers+="_"+out.str();
            }
        }
    }							 // end pop

    logbook.revenue_from_av_prices=0.0;
    for(unsigned int pop = 0; pop < a_catch_pop_at_szgroup.size(); pop++)
    {
        vector<int> comcat_at_szgroup =   populations[pop]->get_comcat_at_szgroup();

        for(unsigned int sz = 0; sz < a_catch_pop_at_szgroup[pop].size(); sz++)
        {
            int comcat_this_size =comcat_at_szgroup.at(sz);
            logbook.revenue_from_av_prices += a_catch_pop_at_szgroup[pop][sz] * v->get_loc()->get_prices_per_cat(pop, comcat_this_size);
        }
    }

    length_class =v->get_length_class();
    logbook.fuelcost = v->get_cumfuelcons() * v->get_loc()->get_fuelprices(length_class);
    logbook.gav=logbook.revenue-logbook.fuelcost;
    logbook.gav2=logbook.revenue_from_av_prices-logbook.fuelcost;
}

bool VesselLogbookOutputMessage::process()
{
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
    ss  << freq_metiers << " " << logbook.revenue << " " ;
    ss  << logbook.revenue_from_av_prices << " " ;
    ss  << logbook.fuelcost << " " ;
    ss  << logbook.gav << " " ;
    ss  << logbook.gav2 << " " ;
    ss  << " " << std::endl;

    MutexLocker l(&glob_mutex);

    loglike << ss.str();
    return true;
}

bool VesselLogbookOutputMessage::send(std::ostream &)
{
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
    ss  << freq_metiers << " " << logbook.revenue << " " ;
    ss  << logbook.revenue_from_av_prices << " " ;
    ss  << logbook.fuelcost << " " ;
    ss  << logbook.gav << " " ;
    ss  << logbook.gav2 << " " ;
    ss  << " " << std::endl;

    std::cout << "=v" << ss.str();

    return true;
}

size_t VesselLogbookOutputMessage::sendBinary(void *buffer, size_t maxlen)
{
    UNUSED(maxlen);
    size_t i = put(buffer, 0, logbook);
    return i;
}
