#include <helpers.h>

#include <map>
#include <string>
#include <vector>
#include <queue>

#include <ostream>
#include <iostream>

#include <values.h>
#include <Vessel.h>
#include <Node.h>

#include <outputqueuemanager.h>
#include <messages/movevesseloutputmessage.h>
#include <messages/exportvmslikeoutputmessage.h>
#include <messages/vessellogbookoutputmessage.h>

using namespace std;

#include <pthread.h>
#include <semaphore.h>
#include <errno.h>


struct thread_data_t {
    pthread_t thread;
    int thread_idx;
    int thread_id;
};

static unsigned int numthreads;
static std::queue<int> works;
static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t work_cond = PTHREAD_COND_INITIALIZER;
static pthread_mutex_t work_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t completion_cond = PTHREAD_COND_INITIALIZER;
static unsigned int uncompleted_works;

static bool exit_flag;
static thread_data_t *thread_data;

extern OutputQueueManager mOutQueue;

extern pthread_mutex_t glob_mutex;
extern bool use_gui;
extern bool use_dtrees;
extern bool use_gnuplot;
extern bool gui_move_vessels;
extern vector<int> ve;
extern vector <Vessel*> vessels;
extern vector <Population* > populations;
extern int tstep;
extern int nbpops;
extern int export_vmslike;
extern ofstream vmslike;
extern double graph_res;
extern int is_individual_vessel_quotas;
extern vector <int> implicit_pops;
extern ofstream loglike;
extern DynAllocOptions dyn_alloc_sce;
extern PopSceOptions dyn_pop_sce;
extern string biolsce;
extern int create_a_path_shop;
extern deque<map<vertex_t, vertex_t> > path_shop;
extern deque<map<vertex_t, weight_t> >  min_distance_shop;
extern vector <int> idx_path_shop;
extern adjacency_map_t adjacency_map;
extern vector<string> vertex_names;
//extern map<vertex_t, vertex_t> previous;
extern vector<int> relevant_nodes;
extern multimap<int, int> nodes_in_polygons;
extern multimap<int, int> possible_metiers;
extern multimap<int, double> freq_possible_metiers;
extern multimap<int, double> gshape_cpue_per_stk_on_nodes;
extern multimap<int, double> gscale_cpue_per_stk_on_nodes;
extern vector<int> spe_fgrounds;
extern vector<int> spe_harbours;
extern vector<double> spe_freq_fgrounds;
extern vector<double> spe_freq_harbours;
extern vector<double> spe_vessel_betas_per_pop;
extern vector<double> spe_percent_tac_per_pop;
extern vector <Node* > nodes;
extern multimap<int, string> harbour_names;
extern vector<int> name_metiers;
extern ofstream freq_cpue;
extern ofstream freq_profit;
extern ofstream freq_distance;
extern ofstream vmslike2;
extern ofstream vmslike3;
extern vector <Metier*> metiers;
extern ofstream export_individual_tacs;
//extern vector <double> dist_to_ports;

extern void guiSendVesselLogbook(const std::string &line);

static void manage_vessel(thread_data_t *dt, int idx_v)
{
    UNUSED(dt);

    map<vertex_t, weight_t> min_distance;
    map<vertex_t, vertex_t> previous;
    vector <double> dist_to_ports;

    dout(cout  << "----------" << endl);
    pthread_mutex_lock (&glob_mutex);
    int index_v =  ve[idx_v];
    //dout(cout  <<  ve[idx_v] << " idx of the vessel " << vessels[ ve[idx_v] ]->get_name() << " " << endl);
    outc(cout  <<  ve[idx_v] << " idx of the vessel " << vessels[ ve[idx_v] ]->get_name() << " " << endl);
    pthread_mutex_unlock (&glob_mutex);

    // check roadmap
    vessels[index_v]->lock();
    bool roadmap_empty = vessels[ index_v ]->get_roadmap().empty();
    int possible_metiers_size = vessels[ index_v ]->get_possible_metiers().size();
    bool inharbour = vessels[ index_v ]->get_inharbour();
    bool inactive = vessels[ index_v ]-> get_inactive();
    vessels[index_v]->unlock();

    if(roadmap_empty)
    {
        // check if the vessel is actually active this quarter
        // (when at least one possible metier within this quarter)
        if(possible_metiers_size > 1)
        {
            outc(cout  << "ROADMAP EMPTY" << endl);
            if(inharbour)
            {
                outc(cout  << "IN HARB" << endl);

                // LAND the catches when arriving in port and DECLARE IN LOGBOOK
                // i.e. write this trip down in the logbook output file
                // i.e. just arrived!
                if(!inactive)
                {
                    vessels[index_v]->updateTripsStatistics(populations);
                    mOutQueue.enqueue(boost::shared_ptr<OutputMessage>(new VesselLogbookOutputMessage(loglike, tstep, vessels[index_v], populations)));
#if 0
                    std::ostringstream ss;
                    vessels[ index_v ]->export_loglike (ss, populations, tstep, nbpops);
                    loglike << ss.str();

                    guiSendVesselLogbook(ss.str());
#endif
                    //vessels[ index_v ]->export_loglike_prop_met (loglike_prop_met, tstep, nbpops);
                    vessels[index_v]->lock();
                    vessels[ index_v ]->reinit_after_a_trip();
                    vessels[index_v]->unlock();
                }
                // ***************make a probable decision*************************
                vessels[ index_v ]->which_metier_should_i_go_for(metiers);

                // the metier should help informing to which fish price time series the vessel will be look at
                // if a decision on the fish price is used in the GoFishing dtree...
                // (interesting stocks for this vessel are given in Vessel::get_metier_target_stocks() )
                // TO DO....

                // ***************make a dtree decision****************************
                map<string,int> external_states_relevant_for_going_fishing;
                external_states_relevant_for_going_fishing.insert(make_pair(" fish_price_is ",1));
                external_states_relevant_for_going_fishing.insert(make_pair(" weather_is ",0));
                //if(tstep % 24 ==7){ // frequency of the decision e.g. every 7 a.m
                int go_fishing= vessels[ index_v ]->should_i_go_fishing(
                    external_states_relevant_for_going_fishing, use_dtrees);
                //}
                // ***************implement a decision*****************************
                if(go_fishing)
                {

                    //go fishing
                    outc(cout  << "GO FISHING" << endl);
                    vessels[ index_v ]->choose_a_ground_and_go_fishing(
                        tstep,
                        dyn_alloc_sce, create_a_path_shop,
                        idx_path_shop, path_shop, min_distance_shop,
                        adjacency_map, min_distance, previous, relevant_nodes, nodes_in_polygons,
                        vertex_names,
                        nodes,
                        metiers,
                        freq_cpue, freq_profit, freq_distance
                        );

                }
                else
                {
                    //have some rest in the harbour
                    outc(cout  << "STAY IN HARBOUR" << endl);
                    // and decrease the rest time...
                    vessels[ index_v ]-> set_timeforrest( vessels[ index_v ]-> get_timeforrest() - PING_RATE );
                    vessels[ index_v ]-> set_next_xy( vessels[index_v ]->get_x(), vessels[ index_v ]->get_y() );
                    dout(cout  << "...for the next " << vessels[ index_v ]-> get_timeforrest() << " steps" << endl);

                }

            }
            else
            {
                outc(cout  << "NOT IN HARB...SO ON A FISHING GROUND!" << endl);

                // ***************make a decision************************************
                map<string,int> external_states_relevant_for_stopping_fishing;
                external_states_relevant_for_stopping_fishing.insert(make_pair(" none ",0));
                int stop_fishing = vessels[ index_v ]->should_i_stop_fishing(
                    external_states_relevant_for_stopping_fishing,
                    false,
                    tstep,
                    dyn_alloc_sce, create_a_path_shop,
                    idx_path_shop, path_shop, min_distance_shop,
                    adjacency_map, min_distance, previous, relevant_nodes,
                    vertex_names,
                    nodes,
                    metiers,
                    freq_cpue, freq_distance,
                    dist_to_ports);

                //....unless we got a message (e.g. at the end of a year-quarter)
                dout(cout  << "message: " << vessels[ index_v ]->read_message() << endl);
                bool force_another_ground=false;
                // check my mailbox: Am I forced to change of ground?...
                if(vessels[ index_v ]->read_message()==1)
                {
                    if(vessels[ index_v ]->get_fgrounds().size()<3)
                    {
                         //in this case, forced to go back to port instead!
                        stop_fishing=true;
                    }
                         //in this case, forced to change!
                    force_another_ground=true;
                    // reset my mail box
                    vessels[ index_v ]->reset_message();
                }

                // ***************implement a decision************************************
                // go on fishing...
                if(!stop_fishing)
                {
                    // ***************make a decision************************************
                    map<string,int> external_states_relevant_for_change_ground;
                    external_states_relevant_for_change_ground.insert(make_pair(" none ",0));
                    int another_ground = vessels[ index_v ]->should_i_change_ground(
                        external_states_relevant_for_change_ground, false);

                    // ***************implement the decision************************************
                         // ...but not on this ground!
                    if(another_ground || force_another_ground )
                    {
                        outc(cout  << "CHANGE OF GROUND, GUYS! "  << endl);
                        vessels[ index_v ]->choose_another_ground_and_go_fishing(
                            tstep,
                            dyn_alloc_sce, create_a_path_shop,
                            idx_path_shop, path_shop, min_distance_shop,
                            adjacency_map, min_distance, previous, relevant_nodes, nodes_in_polygons,
                            vertex_names,
                            nodes,
                            metiers,
                            freq_cpue, freq_distance
                            );
                        outc(cout  << "GOOD JOB, GUYS! "  << endl);

                    }
                    // ***************implement a decision************************************
                    else // Yes, keep go on catching on this ground...
                    {
                        outc(cout  << "hey, I am fishing on " << vessels[ index_v ]->get_loc()->get_idx_node() << endl);
                        //#pragma omp critical(docatch)
                        {
                            dout(cout  << "please, check you mail! :" << vessels[ index_v ]->read_message() << endl);
                            vessels[ index_v ]->do_catch(export_individual_tacs, populations, nodes, implicit_pops, tstep, graph_res, is_individual_vessel_quotas);

                            // check
                            //if(vessels[ index_v ]->get_loc()->get_idx_node()==430)
                            //{
                            //    vector <double> N_at_szgroup= vessels[ index_v ]->get_loc()->get_Ns_pops_at_szgroup(3);
                            //
                            //    for(int sz=0; sz<N_at_szgroup.size(); sz++)
                            //    {
                            //        cout << "HERE RIGHT AFTER do_catch sz " <<  N_at_szgroup.at(sz) << endl;
                            //    }
                            //}

                        }
                        // update
                        vessels[index_v]->lock();
                        vessels[ index_v ]->set_timeatsea(vessels[ index_v ]->get_timeatsea()+ PING_RATE);
                        // note: no traveled_dist_this_trip cumulated here...might be changed.
                        vessels[ index_v ]-> set_state(1);
                        double cumfuelcons;
                        if(vessels[ index_v ]->get_metier()->get_metier_type()==1)
                        {
                            //trawling (type 1)
                            cumfuelcons = vessels[ index_v ]->get_cumfuelcons()+ vessels[ index_v ]->get_fuelcons()*PING_RATE*vessels[ index_v ]->get_mult_fuelcons_when_fishing();
                            outc(cout  << "fuel cons for trawlers (metier " << vessels[ index_v ]->get_metier()->get_name() << ")" << endl);
                        }
                        else
                        {
                            // gillnetting, seining (type 2)
                            cumfuelcons = vessels[ index_v ]->get_cumfuelcons()+ vessels[ index_v ]->get_fuelcons()*PING_RATE*vessels[ index_v ]->get_mult_fuelcons_when_inactive();
                            outc(cout  << "fuel cons for gillnetters or seiners (metier " << vessels[ index_v ]->get_metier()->get_name() << ")" << endl);
                        }
                        vessels[ index_v ]->set_cumfuelcons(cumfuelcons);

                        // add for cum. effort on this node
                        vessels[ index_v ]->get_loc()->add_to_cumftime(PING_RATE);
                        vessels[index_v]->unlock();

                        outc(cout  << "my catches so far is " << vessels[ index_v ]->get_cumcatches() << endl);
                        outc(cout  << "my comsumed fuel so far is " << cumfuelcons << endl);
                        outc(cout  << "my time at sea so far is " << vessels[ index_v ]->get_timeatsea() << endl);

                    }
                }
                // ***************implement a decision************************************
                else
                {
                    outc(cout  << "RETURN TO PORT, NOW! "  << endl);
                    pthread_mutex_lock(&glob_mutex);
                    vessels[ index_v ]->choose_a_port_and_then_return(
                        tstep,
                        dyn_alloc_sce, create_a_path_shop,
                        idx_path_shop, path_shop, min_distance_shop,
                        adjacency_map, min_distance, previous, relevant_nodes,
                        vertex_names,
                        nodes,
                        metiers,
                        freq_cpue, freq_distance,
                        dist_to_ports
                        );

                    pthread_mutex_unlock(&glob_mutex);
                }

            }

        }

    }
    else
    {
        outc(cout  << "roadmap is not empty... ");
        // display the road map
        //list<int>::iterator pos;
        //dout(cout  << "roadmap (in): ");
        //for(pos=lst.begin(); pos!=lst.end(); pos++)
        //{
        //    dout(cout  << *pos << " ");
        //}
        //dout(cout  << endl);

        // find.next.pt.on.the.graph()
        vessels[ index_v ]->find_next_point_on_the_graph(nodes);

        outc(cout  << "CURRENT LAST POS " << vessels[ index_v ]->get_loc()->get_idx_node() << endl);

        //dout(cout  << "roadmap (out): ");
        //lst = vessels[ index_v ]->get_roadmap();
        //for(pos=lst.begin(); pos!=lst.end(); pos++)
        //{
        //    dout(cout  << *pos << " ");
        //}

    }
    outc(cout  << endl);

    // write this movement in the output  file (hourly data if PING=1)
    // (setprecision is 6 in c++ by default)
    // for VMS, export the first year only because the file is growing too big otherwise....
    vessels[index_v]->lock();

    if(export_vmslike /*&& tstep<8641*/) {
        if( vessels[ index_v ]->get_state()!=3) {
            mOutQueue.enqueue(boost::shared_ptr<OutputMessage>(new ExportVmslikeOutputMessage(vmslike, tstep, vessels[index_v])));
        }
    }

    if (use_gui && gui_move_vessels) {
        mOutQueue.enqueue(boost::shared_ptr<OutputMessage>(new MoveVesselOutputMessage(tstep, vessels[index_v])));
    }

    // realtime gnuplot
    if(use_gnuplot)
    {
        pthread_mutex_lock(&mutex);
        vmslike2   << vessels[ index_v ]->get_x() << " "
            << vessels[ index_v ]->get_y() <<  endl;
        pthread_mutex_unlock(&mutex);
    }
    vessels[index_v]->unlock();

}


static void *thread(void *args)
{
    thread_data_t *data = (thread_data_t *)args;

    while (!exit_flag) {
        pthread_mutex_lock(&work_mutex);
        while (works.size() == 0) {
            pthread_cond_wait(&work_cond, &work_mutex);
        }

        int nextidx = works.front();
        works.pop();
//        cout << "Thr " << data->thread_idx << " work " << nextidx << endl;
        pthread_mutex_unlock(&work_mutex);

        manage_vessel(data, nextidx);

        pthread_mutex_lock(&work_mutex);
        --uncompleted_works;
//        cout << "Thr " << data->thread_idx << " Completed, " << uncompleted_works << " rem\n";
        pthread_cond_signal(&completion_cond);
        pthread_mutex_unlock(&work_mutex);
    }

    return 0;
}

/* Public functions */

void thread_vessel_init (int n)
{
    exit_flag = false;

    numthreads = n;
    thread_data = new thread_data_t[numthreads];
    for (unsigned int i = 0; i < numthreads; ++i) {
        thread_data[i].thread_idx = i;
        thread_data[i].thread_id = pthread_create(&thread_data[i].thread, 0, thread, (void *)&thread_data[i]);

        struct sched_param tparam;
        int policy;

        int ret = pthread_getschedparam(thread_data[i].thread, &policy, &tparam);
        if (ret < 0) {
            perror ("getschedparam");
            exit (1);
        }

#ifdef __unix__
        tparam.__sched_priority -=2;
        policy = SCHED_RR;
#else
        tparam.sched_priority = -2;
        policy = 0;
#endif

        pthread_setschedparam(thread_data[i].thread, policy, &tparam);
    }
}

void thread_vessel_prepare()
{
    pthread_mutex_lock(&work_mutex);
    uncompleted_works = 0;
    pthread_mutex_unlock(&work_mutex);
}

void thread_vessel_insert_job(int idx)
{
    pthread_mutex_lock(&work_mutex);
    works.push(idx);
    ++uncompleted_works;
    pthread_cond_signal(&work_cond);
    pthread_mutex_unlock(&work_mutex);
}

void thread_vessel_wait_completed()
{
    pthread_mutex_lock(&work_mutex);

    while (uncompleted_works > 0) {
        pthread_cond_wait(&completion_cond, &work_mutex);
    }

//    cout << " MAIN: completed " << endl;
    pthread_mutex_unlock(&work_mutex);
}


void thread_vessel_deinit()
{
    pthread_mutex_lock(&work_mutex);
    works.empty();
    exit_flag = true;
    pthread_mutex_unlock (&work_mutex);
}
