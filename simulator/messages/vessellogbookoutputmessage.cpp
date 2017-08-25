#include "vessellogbookoutputmessage.h"

#include <Vessel.h>
#include <helpers.h>
#include <mutex>

#include <Node.h>
#include <Metier.h>

extern std::mutex glob_mutex;
extern bool use_gui;

VesselLogbookOutputMessage::VesselLogbookOutputMessage(unsigned int _tstep, Vessel *v, const std::vector<Population* >& populations, vector<int> &implicit_pops)
{
    // (caution: hardcoding to be removed)
    //vector <int> explicit_pops;
    //explicit_pops.push_back(10);
    //explicit_pops.push_back(11);


    logbook.tstep = _tstep;
    logbook.tstepdep = v->get_tstep_dep();
    logbook.rtbb = v->get_reason_to_go_back();
    logbook.cumstm = v->get_cumsteaming();
    logbook.node = v->get_loc()->get_idx_node().toIndex();
    logbook.idx = v->get_idx();
    name = v->get_name();
    logbook.timeatsea = v->get_timeatsea();
    logbook.cumfcons = v->get_cumfuelcons();
    logbook.travdist = v->get_traveled_dist_this_trip();

    logbook.metier = v->get_metier()->get_name();

    logbook.revenue_from_av_prices=v->getLastTripRevenues();
    logbook.revenue_explicit_from_av_prices=v->getLastTripExplicitRevenues();

    logbook.vpuf=0.0;
    if(logbook.cumfcons>1)
    {
        logbook.vpuf = logbook.revenue_from_av_prices/logbook.cumfcons;
    }

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
    logbook.sweptarea=v->get_sweptareathistrip()*1e6; // from km^2 to m^2
    logbook.revenuepersweptarea=0.0;
    if(logbook.sweptarea>10) // i.e. at least 10 sqr meters
    {
        logbook.revenuepersweptarea=logbook.revenue_from_av_prices /(logbook.sweptarea); // euro per m^2
    }



    logbook.GVA = v->get_GVA();
    logbook.GVAPerRevenue= v->get_GVAPerRevenue();
    logbook.LabourSurplus= v->get_LabourSurplus();
    logbook.GrossProfit= v->get_GrossProfit();
    logbook.NetProfit= v->get_NetProfit();
    logbook.NetProfitMargin= v->get_NetProfitMargin();
    logbook.GVAPerFTE= v->get_GVAPerFTE();
    logbook.RoFTA= v->get_RoFTA();
    logbook.BER= v->get_BER();
    logbook.CRBER= v->get_CRBER();
    logbook.NetPresentValue= v->get_NetPresentValue();



}

bool VesselLogbookOutputMessage::process()
{
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
    ss  << freq_metiers << " " << 0 << " " ;
    ss  << logbook.revenue_from_av_prices << " " ;
    ss  << logbook.fuelcost << " " ;
    ss  << logbook.vpuf << " " ;
    ss  << 0 << " " ;
    ss  << logbook.gav2 << " " ;
    ss  << logbook.GVA << " " ;
    ss  << logbook.GVAPerRevenue << " " ;
    ss  << logbook.LabourSurplus << " " ;
    ss  << logbook.GrossProfit << " " ;
    ss  << logbook.NetProfit << " " ;
    ss  << logbook.NetProfitMargin << " " ;
    ss  << logbook.GVAPerFTE << " " ;
    ss  << logbook.RoFTA << " " ;
    ss  << logbook.BER << " " ;
    ss  << logbook.CRBER << " " ;
    ss  << logbook.NetPresentValue << " " ;

    ss  << " " << std::endl;

    std::cout << "=v" << ss.str();

    return true;
}

size_t VesselLogbookOutputMessage::sendBinary(void *buffer, size_t maxlen)
{
    UNUSED(maxlen);

    logbook.popnum = cumul.size();
    size_t i = put(buffer, 0, logbook);
    for (size_t n = 0; n < logbook.popnum; ++n) {
        i = put(buffer, i, cumul[n]);
    }
    for (size_t n = 0; n < cumul_discards.size(); ++n) {
        i = put(buffer, i, cumul_discards[n]);
    }

    return i;
}
