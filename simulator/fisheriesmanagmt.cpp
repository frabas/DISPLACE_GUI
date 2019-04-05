// --------------------------------------------------------------------------
// DISPLACE: DYNAMIC INDIVIDUAL VESSEL-BASED SPATIAL PLANNING
// AND EFFORT DISPLACEMENT
// Copyright (c) 2012-2019 Francois Bastardie <fba@aqua.dtu.dk>

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



#include <Population.h>
#include <Vessel.h>


using namespace std;

#include <thread>
#include <condition_variable>
#include <errno.h>
#include <helpers.h>

// for Windows
#ifdef _WIN32
#include <windows.h>
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif


extern std::mutex glob_mutex;

// todo: remove this, better use a unique_lock<> instead
static void lock()
{
    glob_mutex.lock();
}

// todo: remove this, better use a unique_lock<> instead
static void unlock()
{
    glob_mutex.unlock();
}



bool computeEffortMultiplier(vector<Population* >& populations,
                          vector<Vessel* >& vessels,
                          int nb_y_left_to_tgrt_year,
                          int HCR)
{
    double effort_multiplier=1.0;
    vector<double> effort_multipliers;

    cout << "EffortControl: HCR in use is HCR" << HCR << endl;

    for(int sp=0; sp<populations.size();++sp)
    {

        // first, retrieve fbar for this pop (computed in biomodule2)...
        double fbar_py= populations.at(sp)->get_fbar();

        if(fbar_py>0 && fbar_py<2)
        {
           vector<double> fbar_ages_min_max =populations.at(sp)-> get_fbar_ages_min_max();
           double FMSY = fbar_ages_min_max.at(6);
           cout << "EffortControl: fbar_py this pop "<< sp << " is " <<fbar_py << ", while FMSY is " << FMSY << endl;

           if(nb_y_left_to_tgrt_year!=0)
           {
            effort_multipliers.push_back((FMSY/fbar_py)/nb_y_left_to_tgrt_year);
           }
           else
           {
           effort_multipliers.push_back((FMSY/fbar_py));
           }
        }
    } // end sp

    if(!effort_multipliers.empty())
    {
       // mixed fisheries issue:
       // choose the min effort
       if(HCR==1) effort_multiplier= *min_element(effort_multipliers.begin(), effort_multipliers.end());
       // choose the max effort
       if(HCR==2) effort_multiplier= *max_element(effort_multipliers.begin(), effort_multipliers.end());
    }
    else
    {
     effort_multiplier=1.0;
    }

    // set same multiplier for all vessels (for now)....
    for (unsigned int i =0; i < vessels.size(); i++)
      {
         vessels[ i ]->set_effort_multiplier(effort_multiplier);
      }

    cout << "Effort control for next y: effort_multiplier set to " << effort_multiplier << endl;

return true;
}




bool computeTAC(vector<Population* >& populations, int sp, int tstep, double multiOnTACconstraint, int HCR)
{
    dout(cout<< "------------" << endl);
    dout(cout<< "COMPUTE TAC for HCR (based on F) for this coming year for pop " << populations.at(sp)->get_name() << endl);
    vector<double> fbar_ages_min_max =populations.at(sp)-> get_fbar_ages_min_max();
    int age_min = fbar_ages_min_max.at(0);
    dout(cout<< "age min:" << age_min << endl);
    int age_max = fbar_ages_min_max.at(1);
    dout(cout<< "age_max:" <<age_max << endl);
    double ftarget = fbar_ages_min_max.at(2);
    dout(cout<< "ftarget:" << ftarget << endl);
    double Fpercent = fbar_ages_min_max.at(3);
    dout(cout<< "Fpercent:" << Fpercent << endl);
    double TACpercent = fbar_ages_min_max.at(4);
    dout(cout<< "TACpercent:" << TACpercent << endl);
    double Btrigger = fbar_ages_min_max.at(5);
    dout(cout<< "TACpercent:" << Btrigger << endl);
    double FMSY = fbar_ages_min_max.at(6);
    dout(cout<< "FMSY:" << FMSY << endl);
    //double FMSYdown = fbar_ages_min_max.at(7);
    double FMSYdown=FMSY*0.9; // TO BE REMOVED WHEN INPUT DATA
    dout(cout<< "FMSYdown:" << FMSY << endl);
    //double FMSYup = fbar_ages_min_max.at(8);
    double FMSYup=FMSY*1.1; // TO BE REMOVED WHEN INPUT DATA
    dout(cout<< "FMSYup:" << FMSY << endl);

    int nbages=populations.at(sp)->get_nb_ages();
    int nbszgroups=populations.at(sp)->get_nb_szgroups();
    vector <double> tot_N_at_age_end_previous_y(nbages);
    vector <double> tot_N_at_sz_end_previous_y(nbszgroups);
    vector <double> tot_W_at_age_y_plus_1(nbages);
    vector <double> maturity_at_age_y_plus_1(nbages);
    vector <double> tot_F_at_age_end_previous_y(nbages);
    vector <double> tot_M_at_age_y(nbages);

    // 1. compute previous fbar
                                 // at the end of the last year, then for last year py...
    dout(cout<< "when computing TAC, first compute fbar for pop..." << populations.at(sp)->get_name() << endl);
    double fbar_py=1.0;
    if(tstep==0) // THIS CODE BLOCK IS NOT USED IN NORMAL MODE, THIS IS JUST FOR DEBUGGING (Options::DEBUG) TO AVOID WAITING THE FULL Y TO LOOK AT TAC NUMBERS...
    {
        fbar_py=FMSY; // in case the tac is computed at start (for debugging) then use FMSY, because by nature perceived_F does not exist yet
        cout << "fbar_py is " << fbar_py << endl;
        // but first apply size percent_szgroup_per_age_matrix to finds out N & co IN AGE!
        tot_N_at_sz_end_previous_y                 = populations.at(sp)->get_tot_N_at_szgroup_month_minus_1();
        vector <double> a_weight_at_szgroup        =populations.at(sp)->get_weight_at_szgroup();
        vector <double> a_maturity_at_szgroup      =populations.at(sp)->get_maturity_at_szgroup();
        vector <vector <double> > percent_szgroup_per_age_matrix= populations.at(sp)->get_percent_age_per_szgroup_matrix();
            for(int sz = 0; sz < nbszgroups; sz++)
            {
                for(int a = 0; a < nbages; a++)
                {
                    tot_F_at_age_end_previous_y[a] = fbar_py; // assumption
                    tot_M_at_age_y[a]              = 0.3; // assumption
                    tot_N_at_age_end_previous_y[a] +=  percent_szgroup_per_age_matrix[sz][a] * tot_N_at_sz_end_previous_y[sz] ; // assumption
                    tot_W_at_age_y_plus_1[a]       +=  percent_szgroup_per_age_matrix[sz][a] * a_weight_at_szgroup[sz] ; // assumption
                    maturity_at_age_y_plus_1[a]    +=  percent_szgroup_per_age_matrix[sz][a] * a_maturity_at_szgroup[sz] ; // assumption
                }

            }
        populations.at(sp)->set_perceived_tot_F_at_age(tot_F_at_age_end_previous_y); // assumption
        populations.at(sp)->set_tot_F_at_age(tot_F_at_age_end_previous_y); // assumption
        populations.at(sp)->set_tot_M_at_age(tot_M_at_age_y); // assumption
        populations.at(sp)->set_perceived_tot_N_at_age(tot_N_at_age_end_previous_y); // assumption
        populations.at(sp)->set_tot_W_at_age(tot_W_at_age_y_plus_1); // assumption
        populations.at(sp)->set_tot_Mat_at_age(maturity_at_age_y_plus_1);// assumption

    }
    else
    {   // by default...
        fbar_py= populations.at(sp)->compute_fbar();
        tot_F_at_age_end_previous_y = populations.at(sp)->get_tot_F_at_age_running_average();
        tot_N_at_sz_end_previous_y  = populations.at(sp)->get_tot_N_at_szgroup_month_minus_1(); // true N
        tot_N_at_age_end_previous_y = populations.at(sp)->get_perceived_tot_N_at_age(); // perceived
        tot_W_at_age_y_plus_1       = populations.at(sp)->get_tot_W_at_age();
        maturity_at_age_y_plus_1    = populations.at(sp)->get_tot_Mat_at_age();
    }
    populations.at(sp)->set_fbar(fbar_py);
    dout(cout << "the fbar at y-1 for this pop is " << fbar_py << endl);



    double tac_y_plus_1=0.0;
    double tac_y=0.0; // only used for YEAR 2
    if(fbar_py>0)
    {
       if(fbar_py>2)
       {
        fbar_py=2; // a safeguard to avoid non-sense computation of TAC change
        cout << "fbar_py set to 2 for pop " << populations.at(sp)->get_name() << endl;
       }



       if(HCR==1){ // long-term management plan

       // 2. compare with the target
       double fmultiplier=1;
       if(fbar_py > ftarget)
       {
        fmultiplier = 1.0 - (Fpercent/100);
       }
       else
       {
        fmultiplier = 1.0 + (Fpercent/100);
       }
    cout << "HCR type 1: the fmultiplier for this pop " << populations.at(sp)->get_name() <<" is then " << fmultiplier <<
        " given the target F " <<  ftarget << " in the plan..." << endl;

    // 3. perform a short-term forecast on N with F * fmultipier * fmultiplier

    //a. for year y from y-1
    dout(cout << "the  N by age at the end of  y is " << endl);
    tot_N_at_age_end_previous_y = populations.at(sp)->get_perceived_tot_N_at_age(); // perceived

    for (unsigned int i=0; i < tot_N_at_age_end_previous_y.size(); i++)
    {
        cout << "tot_N_at_age_end_previous_y age" << i << ": " << tot_N_at_age_end_previous_y.at(i) << endl;
    }


    dout(cout << "the forecast F by age for y (from y-1) is " << endl);
    vector <double> tot_F_at_age_end_previous_y = populations.at(sp)->get_tot_F_at_age_running_average();
    vector <double> tot_F_at_age_y(tot_F_at_age_end_previous_y.size());
    for (unsigned int i=0; i < tot_F_at_age_y.size(); i++)
    {
                                 // target for y
        tot_F_at_age_y.at(i) = tot_F_at_age_end_previous_y.at(i)*fmultiplier;
    }
    for (unsigned int i=0; i < tot_F_at_age_y.size(); i++)
    {
        cout << "tot_F_at_age_y age" << i << ": " << tot_F_at_age_y.at(i) << endl;
    }


    dout(cout << "and the forecast M by age for y+1 (from y) is " << endl);
    vector <double> tot_M_at_age_y = populations.at(sp)->get_tot_M_at_age();
    for (unsigned int i=0; i < tot_M_at_age_y.size(); i++)
    {
        cout << "tot_M_at_age_y age" << i << ": " << tot_M_at_age_y.at(i) << endl;
    }

    dout(cout << "the forecast N by age for y (from y-1) is " << endl);
                                 // init
    vector <double> forecast_tot_N_at_age_end_y(tot_N_at_age_end_previous_y.size());

    //  adding recruits before the F, assuming fixed recruits
    //double recruits = tot_N_at_age_end_previous_y.at(0)*0.9; // precautionary pessimistic recruits
    double recruits = populations.at(sp)->get_tot_N_at_age0();
    forecast_tot_N_at_age_end_y.at(0)=recruits;

    dout(cout << "then, the forecast N by age for y (from y-1) is " << endl);
    for (unsigned int i=1; i < forecast_tot_N_at_age_end_y.size(); i++)
    {
        forecast_tot_N_at_age_end_y.at(i) =
            tot_N_at_age_end_previous_y.at(i-1)*
            exp( -((tot_F_at_age_y.at(i-1)) + tot_M_at_age_y.at(i-1)) );
        ;						 // class change and apply mortality F+M
    }							 // TO DO: add the plusgroup (if necessary)


    for (unsigned int i=0; i < forecast_tot_N_at_age_end_y.size(); i++)
    {
        cout << "forecast_tot_N_at_age_end_y at age" << i << ": " << forecast_tot_N_at_age_end_y.at(i) << endl;
    }

    // CAUTION in YEAR 2 compute the TAC y (in tons) BECAUSE THIS INTERMEDIATE YEAR IS MISSING!
    // according to the forecast N (Baronovs equation)
    // TAC per age and then sum over ages....
    if(tstep==8761)
    {
       cout << "FOR YEAR y i.e. year 2" << endl;
        double contribution_this_age=0.0;
       for (unsigned int i=0; i < forecast_tot_N_at_age_end_y.size(); i++)
       {
        if (tot_W_at_age_y_plus_1.at(i)<1e-6) cout << "something wrong in the TAC computation: e.g. check spe_percent_per_szgroup_biolsceXX matrix for leaks here..." << endl;

        contribution_this_age=  (tot_F_at_age_y.at(i)/((tot_F_at_age_y.at(i))+tot_M_at_age_y.at(i))) *
            forecast_tot_N_at_age_end_y.at(i)* tot_W_at_age_y_plus_1.at(i)*
            (1- (exp(-(tot_F_at_age_y.at(i)+tot_M_at_age_y.at(i)))));

       if(!isfinite(contribution_this_age)) contribution_this_age=0.0;
       tac_y+= contribution_this_age;
       cout << "ADDING the contribution of age" << i << ", tac_y in kg is now   : " << tac_y << endl;
       }
       cout << "tac_y in kg is: " << tac_y << endl;
       tac_y= tac_y/1000;   // convert in tons;
    }


    //b. for year y+1 from y
    dout(cout << "the forecast F by age for y+1 (from y) is " << endl);
                                 //init
    vector <double> forecast_tot_N_at_age_end_y_plus_1 (forecast_tot_N_at_age_end_y.size());
                                 //init
    vector <double> tot_F_at_age_y_plus_1(tot_F_at_age_end_previous_y.size());
    for (unsigned int i=0; i < tot_F_at_age_end_previous_y.size(); i++)
    {
                                 // target for y+1
        tot_F_at_age_y_plus_1.at(i) = tot_F_at_age_y.at(i)*fmultiplier;
    }
    for (unsigned int i=0; i < tot_F_at_age_y_plus_1.size(); i++)
    {
        cout << "tot_F_at_age_y_plus_1 age" << i << ": " << tot_F_at_age_y_plus_1.at(i) << endl;
    }

    dout(cout << "and the forecast M by age for y+1 (from y) is " << endl);
    vector <double> tot_M_at_age_y_plus_1 = populations.at(sp)->get_tot_M_at_age();
    for (unsigned int i=0; i < tot_M_at_age_y_plus_1.size(); i++)
    {
        cout << "tot_M_at_age_y_plus_1 age" << i << ": " << tot_M_at_age_y_plus_1.at(i) << endl;
    }

    dout(cout << "and the forecast W by age for y+1 (from y) is " << endl);
    tot_W_at_age_y_plus_1 = populations.at(sp)->get_tot_W_at_age();
    for (unsigned int i=0; i < tot_W_at_age_y_plus_1.size(); i++)
    {
        cout << "tot_W_at_age_y_plus_1 age" << i << ": " << tot_W_at_age_y_plus_1.at(i) << endl;
    }


    //  Adding recruits before the F, assuming fixed recruits and distribute recruits among szgroup
    //recruits = forecast_tot_N_at_age_end_y.at(0)*0.9; // precautionary pessimistic recruits
    recruits = populations.at(sp)->get_tot_N_at_age0();
    forecast_tot_N_at_age_end_y_plus_1.at(0) = recruits;

    dout(cout << "then, the forecast N by age for y+1 (from y) is " << endl);
    for (unsigned int i=1; i < forecast_tot_N_at_age_end_y.size(); i++)
    {
        forecast_tot_N_at_age_end_y_plus_1.at(i) =
            forecast_tot_N_at_age_end_y.at(i-1)*
            exp( -((tot_F_at_age_y_plus_1.at(i-1)) + tot_M_at_age_y_plus_1.at(i-1)) );
        ;						 // class change and apply mortality F+M
    }							 // TO DO: add the plusgroup (if necessary)


    for (unsigned int i=0; i < forecast_tot_N_at_age_end_y_plus_1.size(); i++)
    {
        cout << "forecast_tot_N_at_age_end_y_plus_1 age" << i << ": " << forecast_tot_N_at_age_end_y_plus_1.at(i) << endl;
    }

    // 4. compute the TAC y+1 (in tons) according to the forecast N (Baronovs equation)
    // TAC per age and then sum over ages....
    cout << "compute TAC from contributions of age classes---" << endl;
    double contribution_this_age=0.0;
    for (unsigned int i=0; i < forecast_tot_N_at_age_end_y_plus_1.size(); i++)
    {
        if (tot_W_at_age_y_plus_1.at(i)<1e-6) cout << "something wrong in the TAC computation: e.g. check spe_percent_per_szgroup_biolsceXX matrix for leaks here..." << endl;

       contribution_this_age=  (tot_F_at_age_y_plus_1.at(i)/((tot_F_at_age_y_plus_1.at(i))+tot_M_at_age_y_plus_1.at(i))) *
            forecast_tot_N_at_age_end_y_plus_1.at(i)* tot_W_at_age_y_plus_1.at(i)*
            (1- (exp(-(tot_F_at_age_y_plus_1.at(i)+tot_M_at_age_y_plus_1.at(i)))));

       if(!isfinite(contribution_this_age)) contribution_this_age=0.0;
       tac_y_plus_1+= contribution_this_age;


       cout << "ADDING the contribution of age" << i << ", tac_y_plus_1 in kg is now   : " << tac_y_plus_1 << endl;
   }
    cout << "tac_y_plus_1 in kg is: " << tac_y_plus_1 << endl;
   tac_y_plus_1= tac_y_plus_1/1000;   // convert in tons;


    // 5. check if within the TAC range, otherwise force to keep the range.
                                 // i.e. at y


    // apply here a scenario if any (default multi is 1)
    TACpercent=TACpercent*multiOnTACconstraint;

    double tac_y= populations.at(sp)->get_tac()->get_current_tac();
    if((tac_y_plus_1/tac_y) > (1+(TACpercent/100)))
    {
        tac_y_plus_1=tac_y* (1+(TACpercent/100));
    }
    if((tac_y_plus_1/tac_y) < (1-(TACpercent/100)))
    {
        tac_y_plus_1=tac_y* (1-(TACpercent/100));
    }
    cout << "but actually the TAC for y+1 will be " << tac_y_plus_1 <<
        " to comply with the TAC limits..." << endl;



   } // end HCR type 1




   if(HCR==2){ //  F-MSY ICES approach (i.e. incl. a B-MSY trigger)

        // 1. perform a short-term forecast on N with F * fmultiplier

         //a. for year y from y-1
         dout(cout << "the  N by age at the end of  y is " << endl);
         tot_N_at_age_end_previous_y = populations.at(sp)->get_perceived_tot_N_at_age(); // perceived
         for (unsigned int i=0; i < tot_N_at_age_end_previous_y.size(); i++)
         {
             cout << "tot_N_at_age_end_previous_y age" << i << ": " << tot_N_at_age_end_previous_y.at(i) << endl;
         }

         for (unsigned int i=0; i < tot_F_at_age_end_previous_y.size(); i++)
         {
             cout << "pop" << populations.at(sp)->get_name() << ": tot_F_at_age_end_previous_y age" << i << ": " << tot_F_at_age_end_previous_y.at(i) << endl;
         }

         dout(cout << "the forecast F by age for y (from y-1) is " << endl);
         vector <double> tot_F_at_age_y(tot_F_at_age_end_previous_y.size());
         for (unsigned int i=0; i < tot_F_at_age_y.size(); i++)
         {
                                      // target for y
             tot_F_at_age_y.at(i) = tot_F_at_age_end_previous_y.at(i);
         }
         for (unsigned int i=0; i < tot_F_at_age_y.size(); i++)
         {
             cout << "pop" << populations.at(sp)->get_name() << ": tot_F_at_age_y age" << i << ": " << tot_F_at_age_y.at(i) << endl;
         }


         dout(cout << "and the forecast M by age for y+1 (from y) is " << endl);
         vector <double> tot_M_at_age_y = populations.at(sp)->get_tot_M_at_age();
         for (unsigned int i=0; i < tot_M_at_age_y.size(); i++)
         {
             cout << "pop" << populations.at(sp)->get_name() << ": tot_M_at_age_y age" << i << ": " << tot_M_at_age_y.at(i) << endl;
         }

         dout(cout << "the forecast N by age for y (from y-1) is " << endl);
                                      // init
         vector <double> forecast_tot_N_at_age_end_y(tot_N_at_age_end_previous_y.size());


         dout(cout << "then, the forecast N by age for y (from y-1) is " << endl);
         for (unsigned int i=1; i < forecast_tot_N_at_age_end_y.size(); i++)
         {
             forecast_tot_N_at_age_end_y.at(i) =
                 tot_N_at_age_end_previous_y.at(i-1)*
                 exp( -((tot_F_at_age_y.at(i-1)) + tot_M_at_age_y.at(i-1)) );
             ;						 // class change and apply mortality F+M
         }							 // TO DO: add the plusgroup (if necessary)

         //  adding recruits after the F, assuming fixed or SR recruits
         //double recruits = this->get_tot_N_at_age0()*0.5; // precautionary recruits assuming a timing for recruits different from start y estimation
         double recruits=0.0;
         vector <double> param_sr=populations.at(sp)->get_param_sr();
         if(param_sr[0]>2000)
         {
             recruits =param_sr[0];
         }
         else
         {
             double ssb_py_computed_from_ages =0.0;
             for (unsigned int a=0; a < tot_N_at_age_end_previous_y.size(); a++)
             {
                 ssb_py_computed_from_ages+= tot_N_at_age_end_previous_y.at(a)* tot_W_at_age_y_plus_1.at(a) * maturity_at_age_y_plus_1.at(a); // SSB
             }
             ssb_py_computed_from_ages= ssb_py_computed_from_ages/1000; // SSB in tons
             recruits =(param_sr[0]*(ssb_py_computed_from_ages)*exp(-param_sr[1]*(ssb_py_computed_from_ages))) * 1000;
         }
         forecast_tot_N_at_age_end_y.at(0)=recruits * exp( -((tot_F_at_age_y.at(0)) + tot_M_at_age_y.at(0)) );

         for (unsigned int i=0; i < forecast_tot_N_at_age_end_y.size(); i++)
         {
             cout << "pop" << populations.at(sp)->get_name() << ": forecast_tot_N_at_age_end_y at age" << i << ": " << forecast_tot_N_at_age_end_y.at(i) << endl;
         }

         // CAUTION in YEAR 2 compute the TAC y (in tons) BECAUSE THIS INTERMEDIATE YEAR IS MISSING!
         // according to the forecast N (Baronovs equation)
         // TAC per age and then sum over ages....
         if(tstep==8761)
         {
            cout << "FOR YEAR y i.e. year 2" << endl;
             double contribution_this_age=0.0;
            for (unsigned int i=0; i < forecast_tot_N_at_age_end_y.size(); i++)
            {
             if (tot_W_at_age_y_plus_1.at(i)<1e-6) cout << "something wrong in the TAC computation: e.g. check spe_percent_per_szgroup_biolsceXX matrix for leaks here..." << endl;

             contribution_this_age=  (tot_F_at_age_y.at(i)/((tot_F_at_age_y.at(i))+tot_M_at_age_y.at(i))) *
                 forecast_tot_N_at_age_end_y.at(i)* tot_W_at_age_y_plus_1.at(i)*
                 (1- (exp(-(tot_F_at_age_y.at(i)+tot_M_at_age_y.at(i)))));

            if(!isfinite(contribution_this_age)) contribution_this_age=0.0;
            tac_y+= contribution_this_age;
            cout << "ADDING the contribution of age" << i << ", tac_y in kg is now   : " << tac_y << endl;
            }
            cout << "tac_y in kg is: " << tac_y << endl;
            tac_y= tac_y/1000;   // convert in tons;
         }


         //b. for year y+1 from y

         cout<< "pop" << populations.at(sp)->get_name() << ": compute forecasted fbar y..." << endl;
         double fbar_y=0;
         vector<double>fbar_ages_min_max =populations.at(sp)->get_fbar_ages_min_max();
         int age_min =fbar_ages_min_max.at(0);
         int age_max =fbar_ages_min_max.at(1);
         if(age_max==0 || (age_max < age_min))
         {
             dout(cout << "age_max at 0 for this pop??" << endl);
             age_max=5;
         }
         for(int a = age_min; a < age_max; a++)
         {
                                      // sum...
             fbar_y+=tot_F_at_age_y[a];
         }
                                      // then do the average...
         fbar_y=fbar_y/((fbar_ages_min_max.at(1)-fbar_ages_min_max.at(0)) +1);
         cout<< "forecasted fbar y is..." << fbar_y  << endl;
         if(fbar_y>2)
         {
          fbar_y=2; // a safeguard to avoid non-sense computation of TAC change
          cout << "fbar_y set to 2 for pop " << populations.at(sp)->get_name() << endl;
         }

         // 1. check B trigger
         cout << "pop" << populations.at(sp)->get_name() << ": F-MSY approach applies with Btrigger at " << Btrigger << "  (when necessary)... " << endl;
         double ssb_y_computed_from_ages =0.0;
         for (unsigned int a=0; a < forecast_tot_N_at_age_end_y.size(); a++)
         {
             ssb_y_computed_from_ages+= forecast_tot_N_at_age_end_y.at(a)* tot_W_at_age_y_plus_1.at(a) * maturity_at_age_y_plus_1.at(a); // SSB
         }
         ssb_y_computed_from_ages= ssb_y_computed_from_ages/1000; // SSB in tons
         cout << "forecast SSB y pop"<< populations.at(sp)->get_name() <<" in tons is " << ssb_y_computed_from_ages << "  for this stock...(when computed from ages (perceived)) " << endl;


         double fmultiplier=1;
         // 2. compare with the target
             fmultiplier = 1.0 *FMSY /fbar_y;
           if(ssb_y_computed_from_ages<Btrigger)
           {
           fmultiplier = 1.0 *FMSY*(ssb_y_computed_from_ages/Btrigger) /fbar_y;
           cout << "so...Btrigger applies! " << endl;
           }

          cout << "HCR type 2: the fmultiplier for this pop " << populations.at(sp)->get_name() <<" is then " << fmultiplier <<
             " given the target F " <<  FMSY << " in the plan..." << " while fbar_y is " << fbar_y << endl;


         dout(cout << "the forecast F by age for y+1 (from y) is " << endl);
                                      //init
         vector <double> forecast_tot_N_at_age_end_y_plus_1 (forecast_tot_N_at_age_end_y.size());
                                      //init
         vector <double> tot_F_at_age_y_plus_1(tot_F_at_age_end_previous_y.size());
         for (unsigned int i=0; i < tot_F_at_age_end_previous_y.size(); i++)
         {
                                      // target for y+1
             tot_F_at_age_y_plus_1.at(i) = tot_F_at_age_y.at(i)*fmultiplier;
         }
         for (unsigned int i=0; i < tot_F_at_age_y_plus_1.size(); i++)
         {
             cout << "tot_F_at_age_y_plus_1 age" << i << ": " << tot_F_at_age_y_plus_1.at(i) << endl;
         }

         dout(cout << "and the forecast M by age for y+1 (from y) is " << endl);
         vector <double> tot_M_at_age_y_plus_1 = populations.at(sp)->get_tot_M_at_age();
         for (unsigned int i=0; i < tot_M_at_age_y_plus_1.size(); i++)
         {
             cout << "tot_M_at_age_y_plus_1 age" << i << ": " << tot_M_at_age_y_plus_1.at(i) << endl;
         }

         dout(cout << "and the forecast W by age for y+1 (from y) is " << endl);
         tot_W_at_age_y_plus_1 = populations.at(sp)->get_tot_W_at_age();
         for (unsigned int i=0; i < tot_W_at_age_y_plus_1.size(); i++)
         {
             cout << "tot_W_at_age_y_plus_1 age" << i << ": " << tot_W_at_age_y_plus_1.at(i) << endl;
         }



         dout(cout << "then, the forecast N by age for y+1 (from y) is " << endl);
         for (unsigned int i=1; i < forecast_tot_N_at_age_end_y.size(); i++)
         {
             forecast_tot_N_at_age_end_y_plus_1.at(i) =
                 forecast_tot_N_at_age_end_y.at(i-1)*
                 exp( -((tot_F_at_age_y_plus_1.at(i-1)) + tot_M_at_age_y_plus_1.at(i-1)) );
             ;						 // class change and apply mortality F+M
         }							 // TO DO: add the plusgroup (if necessary)

         //  Adding recruits After the F, assuming fixed or SR recruits
         //recruits = populations.at(sp)->get_tot_N_at_age0()*0.5; // precautionary recruits assuming a timing for recruits different from start y estimation
         if(param_sr.at(0)>2000)
         {
             recruits =param_sr.at(0);
         }
         else
         {
             recruits =(param_sr.at(0)*(ssb_y_computed_from_ages)*exp(-param_sr.at(1)*(ssb_y_computed_from_ages))) * 1000;
         }
         forecast_tot_N_at_age_end_y_plus_1.at(0) = recruits * exp( -((tot_F_at_age_y_plus_1.at(0)) + tot_M_at_age_y_plus_1.at(0)) );

         for (unsigned int i=0; i < forecast_tot_N_at_age_end_y_plus_1.size(); i++)
         {
             cout << "forecast_tot_N_at_age_end_y_plus_1 age" << i << ": " << forecast_tot_N_at_age_end_y_plus_1.at(i) << endl;
         }

         // 4. compute the TAC y+1 (in tons) according to the forecast N (Baronovs equation)
         // TAC per age and then sum over ages....
         cout << "compute TAC from contributions of age classes---" << endl;
         double contribution_this_age=0.0;
         for (unsigned int i=0; i < forecast_tot_N_at_age_end_y_plus_1.size(); i++)
         {
             if (tot_W_at_age_y_plus_1.at(i)<1e-6) cout << "something wrong in the TAC computation: e.g. check spe_percent_per_szgroup_biolsceXX matrix for leaks here..." << endl;

            contribution_this_age=  (tot_F_at_age_y_plus_1.at(i)/((tot_F_at_age_y_plus_1.at(i))+tot_M_at_age_y_plus_1.at(i))) *
                 forecast_tot_N_at_age_end_y_plus_1.at(i)* tot_W_at_age_y_plus_1.at(i)*
                 (1- (exp(-(tot_F_at_age_y_plus_1.at(i)+tot_M_at_age_y_plus_1.at(i)))));

            if(!isfinite(contribution_this_age)) contribution_this_age=0.0;
            tac_y_plus_1+= contribution_this_age;


            cout << "ADDING the contribution of age" << i << ", tac_y_plus_1 in kg is now   : " << tac_y_plus_1 << endl;
        }
         cout << "tac_y_plus_1 in kg is: " << tac_y_plus_1 << endl;
        tac_y_plus_1= tac_y_plus_1/1000;   // convert in tons;

   } // end HCR type 2


   if(HCR==3){ //  F-MSY ICES approach (i.e. incl. a B-MSY trigger) but with flexibility of FMSY range



         // 1. perform a short-term forecast on N with F * fmultiplier

         //a. for year y from y-1
         dout(cout << "the  N by age at the end of  y is " << endl);
         tot_N_at_age_end_previous_y = populations.at(sp)->get_perceived_tot_N_at_age(); // perceived
         for (unsigned int i=0; i < tot_N_at_age_end_previous_y.size(); i++)
         {
             cout << "tot_N_at_age_end_previous_y age" << i << ": " << tot_N_at_age_end_previous_y.at(i) << endl;
         }

         for (unsigned int i=0; i < tot_F_at_age_end_previous_y.size(); i++)
         {
             cout << "pop" << populations.at(sp)->get_name() << ": tot_F_at_age_end_previous_y age" << i << ": " << tot_F_at_age_end_previous_y.at(i) << endl;
         }

         dout(cout << "the forecast F by age for y (from y-1) is " << endl);
         vector <double> tot_F_at_age_y(tot_F_at_age_end_previous_y.size());
         for (unsigned int i=0; i < tot_F_at_age_y.size(); i++)
         {
                                      // target for y
             tot_F_at_age_y.at(i) = tot_F_at_age_end_previous_y.at(i);
         }
         for (unsigned int i=0; i < tot_F_at_age_y.size(); i++)
         {
             cout << "pop" << populations.at(sp)->get_name() << ": tot_F_at_age_y age" << i << ": " << tot_F_at_age_y.at(i) << endl;
         }


         dout(cout << "and the forecast M by age for y+1 (from y) is " << endl);
         vector <double> tot_M_at_age_y = populations.at(sp)->get_tot_M_at_age();
         for (unsigned int i=0; i < tot_M_at_age_y.size(); i++)
         {
             cout << "pop" << populations.at(sp)->get_name() << ": tot_M_at_age_y age" << i << ": " << tot_M_at_age_y.at(i) << endl;
         }

         dout(cout << "the forecast N by age for y (from y-1) is " << endl);
                                      // init
         vector <double> forecast_tot_N_at_age_end_y(tot_N_at_age_end_previous_y.size());


         dout(cout << "then, the forecast N by age for y (from y-1) is " << endl);
         for (unsigned int i=1; i < forecast_tot_N_at_age_end_y.size(); i++)
         {
             forecast_tot_N_at_age_end_y.at(i) =
                 tot_N_at_age_end_previous_y.at(i-1)*
                 exp( -((tot_F_at_age_y.at(i-1)) + tot_M_at_age_y.at(i-1)) );
             ;						 // class change and apply mortality F+M
         }							 // TO DO: add the plusgroup (if necessary)

         //  adding recruits after the F, assuming fixed or SR recruits
         //double recruits = populations.at(sp)->get_tot_N_at_age0()*0.5; // precautionary recruits assuming a timing for recruits different from start y estimation
         double recruits=0.0;
         vector <double> param_sr=populations.at(sp)->get_param_sr();
         if(param_sr[0]>2000)
         {
             recruits =param_sr[0];
         }
         else
         {
             double ssb_py_computed_from_ages =0.0;
             for (unsigned int a=0; a < tot_N_at_age_end_previous_y.size(); a++)
             {
                 ssb_py_computed_from_ages+= tot_N_at_age_end_previous_y.at(a)* tot_W_at_age_y_plus_1.at(a) * maturity_at_age_y_plus_1.at(a); // SSB
             }
             ssb_py_computed_from_ages= ssb_py_computed_from_ages/1000; // SSB in tons
             recruits =(param_sr[0]*(ssb_py_computed_from_ages)*exp(-param_sr[1]*(ssb_py_computed_from_ages))) * 1000;
         }
         forecast_tot_N_at_age_end_y.at(0)=recruits * exp( -((tot_F_at_age_y.at(0)) + tot_M_at_age_y.at(0)) );

         for (unsigned int i=0; i < forecast_tot_N_at_age_end_y.size(); i++)
         {
             cout << "pop" << populations.at(sp)->get_name() << ": forecast_tot_N_at_age_end_y at age" << i << ": " << forecast_tot_N_at_age_end_y.at(i) << endl;
         }

         // CAUTION in YEAR 2 compute the TAC y (in tons) BECAUSE THIS INTERMEDIATE YEAR IS MISSING!
         // according to the forecast N (Baronovs equation)
         // TAC per age and then sum over ages....
         if(tstep==8761)
         {
            cout << "FOR YEAR y i.e. year 2" << endl;
             double contribution_this_age=0.0;
            for (unsigned int i=0; i < forecast_tot_N_at_age_end_y.size(); i++)
            {
             if (tot_W_at_age_y_plus_1.at(i)<1e-6) cout << "something wrong in the TAC computation: e.g. check spe_percent_per_szgroup_biolsceXX matrix for leaks here..." << endl;

             contribution_this_age=  (tot_F_at_age_y.at(i)/((tot_F_at_age_y.at(i))+tot_M_at_age_y.at(i))) *
                 forecast_tot_N_at_age_end_y.at(i)* tot_W_at_age_y_plus_1.at(i)*
                 (1- (exp(-(tot_F_at_age_y.at(i)+tot_M_at_age_y.at(i)))));

            if(!isfinite(contribution_this_age)) contribution_this_age=0.0;
            tac_y+= contribution_this_age;
            cout << "ADDING the contribution of age" << i << ", tac_y in kg is now   : " << tac_y << endl;
            }
            cout << "tac_y in kg is: " << tac_y << endl;
            tac_y= tac_y/1000;   // convert in tons;
         }


         //b. for year y+1 from y

         cout<< "pop" << populations.at(sp)->get_name() << ": compute forecasted fbar y..." << endl;
         double fbar_y=0;
         vector<double>fbar_ages_min_max =populations.at(sp)->get_fbar_ages_min_max();
         int age_min =fbar_ages_min_max.at(0);
         int age_max =fbar_ages_min_max.at(1);
         if(age_max==0 || (age_max < age_min))
         {
             dout(cout << "age_max at 0 for this pop??" << endl);
             age_max=5;
         }
         for(int a = age_min; a < age_max; a++)
         {
                                      // sum...
             fbar_y+=tot_F_at_age_y[a];
         }
                                      // then do the average...
         fbar_y=fbar_y/((fbar_ages_min_max.at(1)-fbar_ages_min_max.at(0)) +1);
         cout<< "forecasted fbar y is..." << fbar_y  << endl;
         if(fbar_y>2)
         {
          fbar_y=2; // a safeguard to avoid non-sense computation of TAC change
          cout << "fbar_y set to 2 for pop " << populations.at(sp)->get_name() << endl;
         }

         // 1. check B trigger
         cout << "pop" << populations.at(sp)->get_name() << ": F-MSY approach applies with Btrigger at " << Btrigger << "  (when necessary)... " << endl;
         double ssb_y_computed_from_ages =0.0;
         for (unsigned int a=0; a < forecast_tot_N_at_age_end_y.size(); a++)
         {
             ssb_y_computed_from_ages+= forecast_tot_N_at_age_end_y.at(a)* tot_W_at_age_y_plus_1.at(a) * maturity_at_age_y_plus_1.at(a); // SSB
         }
         ssb_y_computed_from_ages= ssb_y_computed_from_ages/1000; // SSB in tons
         cout << "forecast SSB y pop"<< populations.at(sp)->get_name() <<" in tons is " << ssb_y_computed_from_ages << "  for this stock...(when computed from ages (perceived)) " << endl;


         double fmultiplier=1;
         // 2. compare with the target
             fmultiplier = 1.0 *FMSY /fbar_y;
             if(populations.at(sp)->get_is_choking_fisheries()) fmultiplier = 1.0 *FMSYup /fbar_y;
             if(!populations.at(sp)->get_is_choking_fisheries()) fmultiplier = 1.0 *FMSYdown /fbar_y;
           if(ssb_y_computed_from_ages<Btrigger)
           {
           fmultiplier = 1.0 *FMSY*(ssb_y_computed_from_ages/Btrigger) /fbar_py;
           cout << "so...Btrigger applies! " << endl;
           }

           cout << "HCR type 3: the fmultiplier for this pop " << populations.at(sp)->get_name() <<" is then " << fmultiplier <<
             " given the target F " <<  FMSY << " in the plan..." << " while fbar_y is " << fbar_y << endl;


         dout(cout << "the forecast F by age for y+1 (from y) is " << endl);
                                      //init
         vector <double> forecast_tot_N_at_age_end_y_plus_1 (forecast_tot_N_at_age_end_y.size());
                                      //init
         vector <double> tot_F_at_age_y_plus_1(tot_F_at_age_end_previous_y.size());
         for (unsigned int i=0; i < tot_F_at_age_end_previous_y.size(); i++)
         {
                                      // target for y+1
             tot_F_at_age_y_plus_1.at(i) = tot_F_at_age_y.at(i)*fmultiplier;
         }
         for (unsigned int i=0; i < tot_F_at_age_y_plus_1.size(); i++)
         {
             cout << "tot_F_at_age_y_plus_1 age" << i << ": " << tot_F_at_age_y_plus_1.at(i) << endl;
         }

         dout(cout << "and the forecast M by age for y+1 (from y) is " << endl);
         vector <double> tot_M_at_age_y_plus_1 = populations.at(sp)->get_tot_M_at_age();
         for (unsigned int i=0; i < tot_M_at_age_y_plus_1.size(); i++)
         {
             cout << "tot_M_at_age_y_plus_1 age" << i << ": " << tot_M_at_age_y_plus_1.at(i) << endl;
         }

         dout(cout << "and the forecast W by age for y+1 (from y) is " << endl);
         tot_W_at_age_y_plus_1 = populations.at(sp)->get_tot_W_at_age();
         for (unsigned int i=0; i < tot_W_at_age_y_plus_1.size(); i++)
         {
             cout << "tot_W_at_age_y_plus_1 age" << i << ": " << tot_W_at_age_y_plus_1.at(i) << endl;
         }



         dout(cout << "then, the forecast N by age for y+1 (from y) is " << endl);
         for (unsigned int i=1; i < forecast_tot_N_at_age_end_y.size(); i++)
         {
             forecast_tot_N_at_age_end_y_plus_1.at(i) =
                 forecast_tot_N_at_age_end_y.at(i-1)*
                 exp( -((tot_F_at_age_y_plus_1.at(i-1)) + tot_M_at_age_y_plus_1.at(i-1)) );
             ;						 // class change and apply mortality F+M
         }							 // TO DO: add the plusgroup (if necessary)

         //  Adding recruits After the F, assuming fixed or SR recruits
         //recruits = populations.at(sp)->get_tot_N_at_age0()*0.5; // precautionary recruits assuming a timing for recruits different from start y estimation
         if(param_sr.at(0)>2000)
         {
             recruits =param_sr.at(0);
         }
         else
         {
             recruits =(param_sr.at(0)*(ssb_y_computed_from_ages)*exp(-param_sr.at(1)*(ssb_y_computed_from_ages))) * 1000;
         }
         forecast_tot_N_at_age_end_y_plus_1.at(0) = recruits * exp( -((tot_F_at_age_y_plus_1.at(0)) + tot_M_at_age_y_plus_1.at(0)) );

         for (unsigned int i=0; i < forecast_tot_N_at_age_end_y_plus_1.size(); i++)
         {
             cout << "forecast_tot_N_at_age_end_y_plus_1 age" << i << ": " << forecast_tot_N_at_age_end_y_plus_1.at(i) << endl;
         }

         // 4. compute the TAC y+1 (in tons) according to the forecast N (Baronovs equation)
         // TAC per age and then sum over ages....
         cout << "compute TAC from contributions of age classes---" << endl;
         double contribution_this_age=0.0;
         for (unsigned int i=0; i < forecast_tot_N_at_age_end_y_plus_1.size(); i++)
         {
             if (tot_W_at_age_y_plus_1.at(i)<1e-6) cout << "something wrong in the TAC computation: e.g. check spe_percent_per_szgroup_biolsceXX matrix for leaks here..." << endl;

            contribution_this_age=  (tot_F_at_age_y_plus_1.at(i)/((tot_F_at_age_y_plus_1.at(i))+tot_M_at_age_y_plus_1.at(i))) *
                 forecast_tot_N_at_age_end_y_plus_1.at(i)* tot_W_at_age_y_plus_1.at(i)*
                 (1- (exp(-(tot_F_at_age_y_plus_1.at(i)+tot_M_at_age_y_plus_1.at(i)))));

            if(!isfinite(contribution_this_age)) contribution_this_age=0.0;
            tac_y_plus_1+= contribution_this_age;


            cout << "ADDING the contribution of age" << i << ", tac_y_plus_1 in kg is now   : " << tac_y_plus_1 << endl;
        }
         cout << "tac_y_plus_1 in kg is: " << tac_y_plus_1 << endl;
        tac_y_plus_1= tac_y_plus_1/1000;   // convert in tons;


   } // end HCR type 3


   if(HCR==4){ //  statuquo TAC

             cout << "Statuquo TAC definition for next year... " << endl;
             double fmultiplier=1.0;
             // 1. perform a short-term forecast on N with F * fmultiplier

              //a. for year y from y-1
              dout(cout << "the  N by age at the end of  y is " << endl);
              tot_N_at_age_end_previous_y = populations.at(sp)->get_perceived_tot_N_at_age(); // perceived
              for (unsigned int i=0; i < tot_N_at_age_end_previous_y.size(); i++)
              {
                  cout << "tot_N_at_age_end_previous_y age" << i << ": " << tot_N_at_age_end_previous_y.at(i) << endl;
              }

              for (unsigned int i=0; i < tot_F_at_age_end_previous_y.size(); i++)
              {
                  cout << "pop" << populations.at(sp)->get_name() << ": tot_F_at_age_end_previous_y age" << i << ": " << tot_F_at_age_end_previous_y.at(i) << endl;
              }

              dout(cout << "the forecast F by age for y (from y-1) is " << endl);
              vector <double> tot_F_at_age_y(tot_F_at_age_end_previous_y.size());
              for (unsigned int i=0; i < tot_F_at_age_y.size(); i++)
              {
                                           // target for y
                  tot_F_at_age_y.at(i) = tot_F_at_age_end_previous_y.at(i);
              }
              for (unsigned int i=0; i < tot_F_at_age_y.size(); i++)
              {
                  cout << "pop" << populations.at(sp)->get_name() << ": tot_F_at_age_y age" << i << ": " << tot_F_at_age_y.at(i) << endl;
              }


              dout(cout << "and the forecast M by age for y+1 (from y) is " << endl);
              vector <double> tot_M_at_age_y = populations.at(sp)->get_tot_M_at_age();
              for (unsigned int i=0; i < tot_M_at_age_y.size(); i++)
              {
                  cout << "pop" << populations.at(sp)->get_name() << ": tot_M_at_age_y age" << i << ": " << tot_M_at_age_y.at(i) << endl;
              }

              dout(cout << "the forecast N by age for y (from y-1) is " << endl);
                                           // init
              vector <double> forecast_tot_N_at_age_end_y(tot_N_at_age_end_previous_y.size());


              dout(cout << "then, the forecast N by age for y (from y-1) is " << endl);
              for (unsigned int i=1; i < forecast_tot_N_at_age_end_y.size(); i++)
              {
                  forecast_tot_N_at_age_end_y.at(i) =
                      tot_N_at_age_end_previous_y.at(i-1)*
                      exp( -((tot_F_at_age_y.at(i-1)) + tot_M_at_age_y.at(i-1)) );
                  ;						 // class change and apply mortality F+M
              }							 // TO DO: add the plusgroup (if necessary)

              //  adding recruits after the F, assuming fixed or SR recruits
              //double recruits = populations.at(sp)->get_tot_N_at_age0()*0.5; // precautionary recruits assuming a timing for recruits different from start y estimation
              double recruits=0.0;
              vector <double> param_sr=populations.at(sp)->get_param_sr();
              if(param_sr.at(0)>2000)
              {
                  recruits =param_sr.at(0);
              }
              else
              {
                  double ssb_py_computed_from_ages =0.0;
                  for (unsigned int a=0; a < tot_N_at_age_end_previous_y.size(); a++)
                  {
                      ssb_py_computed_from_ages+= tot_N_at_age_end_previous_y.at(a)* tot_W_at_age_y_plus_1.at(a) * maturity_at_age_y_plus_1.at(a); // SSB
                  }
                  ssb_py_computed_from_ages= ssb_py_computed_from_ages/1000; // SSB in tons
                  recruits =(param_sr.at(0)*(ssb_py_computed_from_ages)*exp(-param_sr.at(1)*(ssb_py_computed_from_ages))) * 1000;
              }
              forecast_tot_N_at_age_end_y.at(0)=recruits * exp( -((tot_F_at_age_y.at(0)) + tot_M_at_age_y.at(0)) );

              for (unsigned int i=0; i < forecast_tot_N_at_age_end_y.size(); i++)
              {
                  cout << "pop" << populations.at(sp)->get_name() << ": forecast_tot_N_at_age_end_y at age" << i << ": " << forecast_tot_N_at_age_end_y.at(i) << endl;
              }

              // CAUTION in YEAR 2 compute the TAC y (in tons) BECAUSE THIS INTERMEDIATE YEAR IS MISSING!
              // according to the forecast N (Baronovs equation)
              // TAC per age and then sum over ages....
              if(tstep==8761)
              {
                 cout << "FOR YEAR y i.e. year 2" << endl;
                  double contribution_this_age=0.0;
                 for (unsigned int i=0; i < forecast_tot_N_at_age_end_y.size(); i++)
                 {
                  if (tot_W_at_age_y_plus_1.at(i)<1e-6) cout << "something wrong in the TAC computation: e.g. check spe_percent_per_szgroup_biolsceXX matrix for leaks here..." << endl;

                  contribution_this_age=  (tot_F_at_age_y.at(i)/((tot_F_at_age_y.at(i))+tot_M_at_age_y.at(i))) *
                      forecast_tot_N_at_age_end_y.at(i)* tot_W_at_age_y_plus_1.at(i)*
                      (1- (exp(-(tot_F_at_age_y.at(i)+tot_M_at_age_y.at(i)))));

                 if(!isfinite(contribution_this_age)) contribution_this_age=0.0;
                 tac_y+= contribution_this_age;
                 cout << "ADDING the contribution of age" << i << ", tac_y in kg is now   : " << tac_y << endl;
                 }
                 cout << "tac_y in kg is: " << tac_y << endl;
                 tac_y= tac_y/1000;   // convert in tons;
              }


              //b. for year y+1 from y

              cout<< "pop" << populations.at(sp)->get_name() << ": compute forecasted fbar y..." << endl;
              double fbar_y=0;
              vector<double>fbar_ages_min_max =populations.at(sp)->get_fbar_ages_min_max();
              int age_min =fbar_ages_min_max.at(0);
              int age_max =fbar_ages_min_max.at(1);
              if(age_max==0 || (age_max < age_min))
              {
                  dout(cout << "age_max at 0 for this pop??" << endl);
                  age_max=5;
              }
              for(int a = age_min; a < age_max; a++)
              {
                                           // sum...
                  fbar_y+=tot_F_at_age_y[a];
              }
                                           // then do the average...
              fbar_y=fbar_y/((fbar_ages_min_max.at(1)-fbar_ages_min_max.at(0)) + 1);
              cout<< "forecasted fbar y is..." << fbar_y  << endl;
              if(fbar_y>2)
              {
               fbar_y=2; // a safeguard to avoid non-sense computation of TAC change
               cout << "fbar_y set to 2 for pop " << populations.at(sp)->get_name() << endl;
              }

              // 1. check B trigger
              cout << "pop" << populations.at(sp)->get_name() << ": F-MSY approach applies with Btrigger at " << Btrigger << "  (when necessary)... " << endl;
              double ssb_y_computed_from_ages =0.0;
              for (unsigned int a=0; a < forecast_tot_N_at_age_end_y.size(); a++)
              {
                  ssb_y_computed_from_ages+= forecast_tot_N_at_age_end_y.at(a)* tot_W_at_age_y_plus_1.at(a) * maturity_at_age_y_plus_1.at(a); // SSB
              }
              ssb_y_computed_from_ages= ssb_y_computed_from_ages/1000; // SSB in tons
              cout << "forecast SSB y pop"<< populations.at(sp)->get_name() <<" in tons is " << ssb_y_computed_from_ages << "  for this stock...(when computed from ages (perceived)) " << endl;


              fmultiplier=1.0; // STATU QUO

               cout << "HCR type 4: the fmultiplier for this pop " << populations.at(sp)->get_name() <<
                       " is then " << fmultiplier << " while fbar_y is " << fbar_y << endl;


              dout(cout << "the forecast F by age for y+1 (from y) is " << endl);
                                           //init
              vector <double> forecast_tot_N_at_age_end_y_plus_1 (forecast_tot_N_at_age_end_y.size());
                                           //init
              vector <double> tot_F_at_age_y_plus_1(tot_F_at_age_end_previous_y.size());
              for (unsigned int i=0; i < tot_F_at_age_end_previous_y.size(); i++)
              {
                                           // target for y+1
                  tot_F_at_age_y_plus_1.at(i) = tot_F_at_age_y.at(i)*fmultiplier;
              }
              for (unsigned int i=0; i < tot_F_at_age_y_plus_1.size(); i++)
              {
                  cout << "tot_F_at_age_y_plus_1 age" << i << ": " << tot_F_at_age_y_plus_1.at(i) << endl;
              }

              dout(cout << "and the forecast M by age for y+1 (from y) is " << endl);
              vector <double> tot_M_at_age_y_plus_1 = populations.at(sp)->get_tot_M_at_age();
              for (unsigned int i=0; i < tot_M_at_age_y_plus_1.size(); i++)
              {
                  cout << "tot_M_at_age_y_plus_1 age" << i << ": " << tot_M_at_age_y_plus_1.at(i) << endl;
              }

              dout(cout << "and the forecast W by age for y+1 (from y) is " << endl);
              tot_W_at_age_y_plus_1 = populations.at(sp)->get_tot_W_at_age();
              for (unsigned int i=0; i < tot_W_at_age_y_plus_1.size(); i++)
              {
                  cout << "tot_W_at_age_y_plus_1 age" << i << ": " << tot_W_at_age_y_plus_1.at(i) << endl;
              }



              dout(cout << "then, the forecast N by age for y+1 (from y) is " << endl);
              for (unsigned int i=1; i < forecast_tot_N_at_age_end_y.size(); i++)
              {
                  forecast_tot_N_at_age_end_y_plus_1.at(i) =
                      forecast_tot_N_at_age_end_y.at(i-1)*
                      exp( -((tot_F_at_age_y_plus_1.at(i-1)) + tot_M_at_age_y_plus_1.at(i-1)) );
                  ;						 // class change and apply mortality F+M
              }							 // TO DO: add the plusgroup (if necessary)

              //  Adding recruits After the F, assuming fixed or SR recruits
              //recruits = populations.at(sp)->get_tot_N_at_age0()*0.5; // precautionary recruits assuming a timing for recruits different from start y estimation
              param_sr=populations.at(sp)->get_param_sr();
              if(param_sr.at(0)>2000)
              {
                  recruits =param_sr.at(0);
              }
              else
              {
                  recruits =(param_sr.at(0)*(ssb_y_computed_from_ages)*exp(-param_sr.at(1)*(ssb_y_computed_from_ages))) * 1000;
              }
              forecast_tot_N_at_age_end_y_plus_1.at(0) = recruits * exp( -((tot_F_at_age_y_plus_1.at(0)) + tot_M_at_age_y_plus_1.at(0)) );

              for (unsigned int i=0; i < forecast_tot_N_at_age_end_y_plus_1.size(); i++)
              {
                  cout << "forecast_tot_N_at_age_end_y_plus_1 age" << i << ": " << forecast_tot_N_at_age_end_y_plus_1.at(i) << endl;
              }

              // 4. compute the TAC y+1 (in tons) according to the forecast N (Baronovs equation)
              // TAC per age and then sum over ages....
              cout << "compute TAC from contributions of age classes---" << endl;
              double contribution_this_age=0.0;
              for (unsigned int i=0; i < forecast_tot_N_at_age_end_y_plus_1.size(); i++)
              {
                  if (tot_W_at_age_y_plus_1.at(i)<1e-6) cout << "something wrong in the TAC computation: e.g. check spe_percent_per_szgroup_biolsceXX matrix for leaks here..." << endl;

                 contribution_this_age=  (tot_F_at_age_y_plus_1.at(i)/((tot_F_at_age_y_plus_1.at(i))+tot_M_at_age_y_plus_1.at(i))) *
                      forecast_tot_N_at_age_end_y_plus_1.at(i)* tot_W_at_age_y_plus_1.at(i)*
                      (1- (exp(-(tot_F_at_age_y_plus_1.at(i)+tot_M_at_age_y_plus_1.at(i)))));

                 if(!isfinite(contribution_this_age)) contribution_this_age=0.0;
                 tac_y_plus_1+= contribution_this_age;


                 cout << "ADDING the contribution of age" << i << ", tac_y_plus_1 in kg is now   : " << tac_y_plus_1 << endl;
             }
              cout << "tac_y_plus_1 in kg is: " << tac_y_plus_1 << endl;
             tac_y_plus_1= tac_y_plus_1/1000;   // convert in tons;


       } // end HCR type 4




     } // end fbar_py is 0


    if(tac_y==0 || !isfinite(tac_y))
    {
        cout << "WARNING: TACs Option is active but TAC y for this species found to go to 0.... Consider informing a initial TAC value for this pop..." << endl;
        cout << "a fake, non binding value is filled in for now" << endl;

        tac_y =100000; // in tons, and not limiting the fisheries
    }

     if(tac_y_plus_1==0 || !isfinite(tac_y_plus_1))
     {
         cout << "WARNING: TACs Option is active but TAC y+1 for this species found to go to 0.... Consider informing a initial TAC value for this pop..." << endl;
         cout << "a fake, non binding value is filled in for now" << endl;

         tac_y_plus_1 =100000; // in tons, and not limiting the fisheries
     }



    if(tstep==8761) {
        populations.at(sp)->get_tac()->add_tac_to_ts(tac_y, 1);
        populations.at(sp)->get_tac()->add_tac_to_ts(tac_y_plus_1, -1);
        cout << "so, the TAC (in tons) for y will be " << tac_y << endl;
        cout << "so, the TAC (in tons) for y+1 will be " << tac_y_plus_1 << endl;
    } else{
        cout << "so, the TAC (in tons) for y+1 will be " << tac_y_plus_1 << endl;
        populations.at(sp)->get_tac()->add_tac_to_ts(tac_y_plus_1, -1);
    }

    populations.at(sp)->set_is_choking_fisheries(0); // reinit
    populations.at(sp)->get_tac()->set_is_tac_exhausted(0); // reinit


return true;
}




