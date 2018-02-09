#include "outputexporter.h"

#include "storage/sqliteoutputstorage.h"
#include "storage/tables/vesselslogliketable.h"
#include "storage/tables/vesselvmsliketable.h"

#include <mutex>
#include <Vessel.h>
#include <Metier.h>
#include <Node.h>

extern std::mutex glob_mutex;
using namespace std;

std::shared_ptr<OutputExporter> OutputExporter::mInstance = nullptr;

OutputExporter::OutputExporter(const string &basepath, const string &namesimu)
{
    auto filename=basepath + "/vmslike_" + namesimu + ".dat";
    mVmsLike.open(filename.c_str());

    auto filename2=basepath + "/vmslikefpingsonly_" + namesimu + ".dat";
    mVmsLikeFPingsOnly.open(filename2.c_str());

    filename=basepath + "/loglike_" + namesimu + ".dat";
    mLogLike.open(filename.c_str());

    filename=basepath + "/tripcatchesperszgroup_" + namesimu + ".dat";
    mTripCatchesPerSzgroup.open(filename.c_str());

}

void OutputExporter::exportVmsLike(unsigned int tstep, Vessel *vessel)
{
    if (useSql)
        exportVmsLikeSQLite(tstep, vessel);
    if (usePlainText)
        exportVmsLikePlaintext(tstep, vessel);
}

void OutputExporter::exportVmsLikePlaintext(unsigned int tstep, Vessel *vessel)
{
    std::unique_lock<std::mutex> locker(glob_mutex);

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

void OutputExporter::exportVmsLikeSQLite(unsigned int tstep, Vessel *vessel)
{
    std::unique_lock<std::mutex> locker(glob_mutex);

    VesselVmsLikeTable::Log log;
    log.tstep = tstep;
    log.id = vessel->get_idx();
    log.tstep_dep = vessel->get_tstep_dep();
    log.p_long = vessel->get_x();
    log.p_lat = vessel->get_y();
    log.p_course = vessel->get_course();
    log.cum_fuel = vessel->get_cumfuelcons();
    log.state = vessel->get_state();

    mSqlDb->getVesselVmsLikeTable()->insertLog(log);
}

void OutputExporter::exportVmsLikeFPingsOnly(unsigned int tstep, Vessel *vessel,  const std::vector<Population *> &populations, vector<int> &implicit_pops)
{
    std::unique_lock<std::mutex> locker(glob_mutex);

    // tstep / vessel name /  start trip tstep / lon / lat/ course / state / pop / catches (i.e. landings+ discards, in weight) szgroup 0 /  szgroup 1 /... / 13
    // note that combining vessel_name and start_trip_tstep will give a trip id.

    std::ostringstream ss;

        int NBSZGROUP=14;
        vector< vector<double> > a_ping_catch_pop_at_szgroup(populations.size(), vector<double>(NBSZGROUP));

        for(int pop = 0; pop < a_ping_catch_pop_at_szgroup.size(); pop++)
            {

            if (!binary_search (implicit_pops.begin(), implicit_pops.end(),  pop  ))
               {

                ss << tstep << " "
                            //<< vessels[ index_v ]->get_idx() << " "
                         << vessel->get_name() << " "
                            // can be used as a trip identifier
                         << vessel->get_tstep_dep() << " "
                         << setprecision(3) << fixed << vessel->get_x() << " "
                         << setprecision(3) << fixed << vessel->get_y() << " "
                         << setprecision(0) << fixed << vessel->get_course() << " "
                            //<< vessels[ index_v ]->get_inharbour() << " "
                         << setprecision(0) << fixed << vessel->get_cumfuelcons() << " "
                         << vessel->get_state() << " " ;

                ss << pop << " ";


                a_ping_catch_pop_at_szgroup = vessel->get_ping_catch_pop_at_szgroup();
                for(int sz = 0; sz < a_ping_catch_pop_at_szgroup[pop].size(); sz++)
                        {
                            ss << setprecision(0) << fixed << a_ping_catch_pop_at_szgroup[pop][sz] << " ";
                        }

                ss  << " " << std::endl;



                }


            }
        mVmsLikeFPingsOnly << ss.str();


}

void OutputExporter::exportLogLike(unsigned int tstep, Vessel *v, const std::vector<Population *> &populations, vector<int> &implicit_pops)
{
    if (useSql)
        exportLogLikeSQLite(tstep, v, populations, implicit_pops);
    if (usePlainText)
        exportLogLikePlaintext(tstep, v, populations, implicit_pops);
}

void OutputExporter::exportLogLikeSQLite(unsigned int tstep, Vessel *v, const std::vector<Population *> &populations, vector<int> &implicit_pops)
{
    std::vector<double> cumul, cumul_discards;

    const auto &a_catch_pop_at_szgroup = v->get_catch_pop_at_szgroup();
    for(size_t pop = 0; pop < a_catch_pop_at_szgroup.size(); pop++)
    {
        cumul.push_back(0);
        for(size_t sz = 0; sz < a_catch_pop_at_szgroup[pop].size(); sz++)
        {
            cumul.at(pop) = cumul.at(pop)+ a_catch_pop_at_szgroup[pop][sz];
        }
    }

    const auto &a_discards_pop_at_szgroup = v->get_discards_pop_at_szgroup();
    int count =0;
    for(int pop = 0; pop < a_discards_pop_at_szgroup.size(); pop++)
    {
        cumul_discards.push_back(0);
        for(int sz = 0; sz < a_discards_pop_at_szgroup[pop].size(); sz++)
        {
            if(isfinite(a_discards_pop_at_szgroup[pop][sz])) cumul_discards.at(count) +=  a_discards_pop_at_szgroup[pop][sz];
        }
        count+=1;
    }

    mSqlDb->exportLogLike(v, cumul, cumul_discards, tstep);
}

void OutputExporter::exportLogLikePlaintext(unsigned int tstep, Vessel *v, const std::vector<Population *> &populations, vector<int> &implicit_pops)
{
    std::string name, freq_metiers= "M";
    int length_class;

    std::vector<double> cumul;
    std::vector<double> cumul_discards;


    struct VesselLogbookMessage {
        unsigned int tstep, tstepdep;
        int rtbb, node, idx;
        double cumstm, timeatsea,cumfcons,travdist, revenue_from_av_prices, revenue_explicit_from_av_prices,
                fuelcost, vpuf, gav2, sweptarea, revenuepersweptarea,
                GVA, GVAPerRevenue, LabourSurplus, GrossProfit, NetProfit, NetProfitMargin, GVAPerFTE, RoFTA, BER, CRBER, NetPresentValue, numTrips;
        size_t popnum;
        double pop[];
    } logbook;

    logbook.tstep = tstep;
    logbook.tstepdep = v->get_tstep_dep();
    logbook.rtbb = v->get_reason_to_go_back();
    logbook.cumstm = v->get_cumsteaming();
    logbook.node = v->get_loc()->get_idx_node().toIndex();
    logbook.idx = v->get_idx();
    name = v->get_name();
    logbook.timeatsea = v->get_timeatsea();
    logbook.cumfcons = v->get_cumfuelcons();
    logbook.travdist = v->get_traveled_dist_this_trip();

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

    std::ostringstream oss;
    oss << "M(" << v->get_metier()->get_name() << ")";
    freq_metiers= oss.str();

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
    
    logbook.sweptarea=v->get_sweptareathistrip()*1e6; // CAUTION: converted in m^2 for output file

    logbook.revenuepersweptarea=0.0;
    if(logbook.sweptarea>10) // i.e. at least 10 sqr meters
    {
          logbook.revenuepersweptarea=logbook.revenue_from_av_prices/(logbook.sweptarea); // euro per m^2
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
    logbook.numTrips= v->get_numTrips();


    std::ostringstream ss;

    // vessel / date dep / date arr / reason to return to port / cum steaming in hours/
    //    idx node harbour / idx vessel / name vessel / fuelcons /tot catch per pop
    ss << fixed << setprecision(0) << logbook.tstepdep << " "
       << setprecision(0) << logbook.tstep << " "
       << setprecision(0) << logbook.rtbb << " "
       << setprecision(0) << logbook.cumstm << " "
       << setprecision(0) << logbook.node << " "
       << setprecision(0) << logbook.idx << " "
       << setprecision(0) << name << " "
       << setprecision(0) << logbook.timeatsea << " "
       << setprecision(0) << logbook.cumfcons << " "
       << setprecision(0) << logbook.travdist << " ";
    for (std::vector<double>::iterator it = cumul.begin(); it != cumul.end(); ++it)
        ss  << setprecision(1) << *it << " " ;
    ss << setprecision(0)   << freq_metiers << " " << 0 << " " ;
    ss  << setprecision(0) << logbook.revenue_from_av_prices << " " ;
    ss  << setprecision(0) << logbook.revenue_explicit_from_av_prices << " " ;
    ss  << setprecision(0) << logbook.fuelcost << " " ;
    ss  << setprecision(0) << logbook.vpuf << " " ;
    ss  << 0 << " " ;
    ss  << setprecision(0) << logbook.gav2 << " " ;
    ss  << setprecision(0) << logbook.sweptarea << " " ;
    ss  << setprecision(0) << logbook.revenuepersweptarea << " " ;
    for (std::vector<double>::iterator it2 = cumul_discards.begin(); it2 != cumul_discards.end(); ++it2)
        ss  << setprecision(1) << *it2 << " " ;
    ss << setprecision(0) << logbook.GVA << " " ;
    ss << setprecision(3) << logbook.GVAPerRevenue << " " ;
    ss << setprecision(0) << logbook.LabourSurplus << " " ;
    ss << setprecision(0) << logbook.GrossProfit << " " ;
    ss << setprecision(0) << logbook.NetProfit << " " ;
    ss << setprecision(3) << logbook.NetProfitMargin << " " ;
    ss << setprecision(3) << logbook.GVAPerFTE << " " ;
    ss << setprecision(0) << logbook.RoFTA << " " ;
    ss << setprecision(0) << logbook.BER << " " ;
    ss << setprecision(3) << logbook.CRBER << " " ;
    ss << setprecision(0) << logbook.NetPresentValue << " " ;
    ss << setprecision(0) << logbook.numTrips << " " ;

    ss  << " " << std::endl;

    std::unique_lock<std::mutex> l(glob_mutex);

    mLogLike << ss.str();
}



void OutputExporter::exportTripCatchPopPerSzgroup(unsigned int tstep, Vessel *v, const std::vector<Population *> &populations, vector<int> &implicit_pops)
{

    // tstep / vessel name / start trip tstep / pop / TRIP catches (i.e. landings only!, in weight) szgroup 0 /  szgroup 1 /... / 13

    int NBSZGROUP=14;
    vector< vector<double> > a_catch_pop_at_szgroup(populations.size(), vector<double>(NBSZGROUP));

    for(int pop = 0; pop < a_catch_pop_at_szgroup.size(); pop++)
    {

        if (!binary_search (implicit_pops.begin(), implicit_pops.end(),  pop  ))
        {

         std::ostringstream ss;
         ss << tstep << " "
                //<< v[ index_v ]->get_idx() << " "
             << v->get_name() << " "
                // can be used as a trip identifier
             << v->get_tstep_dep() << " "

             << pop << " ";


                a_catch_pop_at_szgroup = v->get_catch_pop_at_szgroup();
                for(int sz = 0; sz < a_catch_pop_at_szgroup[pop].size(); sz++)
                        {
                            ss << setprecision(0) << fixed << a_catch_pop_at_szgroup[pop][sz] << " ";
                        }

          ss  << " " << std::endl;

          mTripCatchesPerSzgroup << ss.str();
          }


       }





}



void OutputExporter::close()
{
    mVmsLike.close();
    mTripCatchesPerSzgroup.close();
}

bool OutputExporter::instantiate(const string &basepath, const string &namesimu)
{
    mInstance = std::make_shared<OutputExporter>(basepath, namesimu);
    return true;
}
