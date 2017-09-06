#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>
#include <string>

#include <myRutils.h>

using namespace std;

void open_file_error(string filename)
{
    string error_msg = "error opening file " + filename;
    cout << error_msg << "\n";

    exit(-1);
}


bool fill_from_coord (istream& in, vector<double>& graph_coord_x,
                      vector<double> & graph_coord_y,
                      vector<int>& graph_coord_harbour, int nrow)
{
    double val;
    int val2;

    for (int i = 0; i < nrow; i++)
    {
        in >> val;
        graph_coord_x.push_back(val);
    }
    for (int i2=nrow; i2 < (nrow+nrow); i2++)
    {
        in >> val;
        graph_coord_y.push_back(val);
    }
    for (int i3=(nrow+nrow); i3 < (nrow+nrow+nrow); i3++)
    {
        in >> val2;
        graph_coord_harbour.push_back(val2);
    }

    cout << "read coord with "
         << graph_coord_x.size() << " nodes" << endl << flush;
return true;
}

// distance between two points in long lat
double greatcircledistance (double x1, double y1, double x2, double y2)
{
    double p = 180/M_PI;
    double Rearth = 6371.0;
    double dist = Rearth * acos(sin(y1/p)*sin(y2/p) + (cos(y1/p) * cos(y2/p)*cos(x1/p - x2/p)));
return (dist);
}



/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

int main(int argc, char* argv[])
    {
    string folder_name_parameterization="myfish";
    string a_semester="semester1";
    int nrow=1000;
    int dist_km = 50;
    int graph=56;

    // e.g.  -f "myfish" -s "semester1" -p 0

    int optind=1;
    // decode arguments
    while ((optind < argc) && (argv[optind][0]=='-'))
        {
            string sw = argv[optind];
            if (sw=="-f")
            {
                optind++;
                folder_name_parameterization = argv[optind];
            }
            else if (sw=="-s")
            {
                optind++;
                a_semester = argv[optind];
            }
            else if (sw=="-graph")
            {
                optind++;
                a_semester = argv[optind];
            }
            else if (sw=="-nr")
            {
                optind++;
                nrow = atoi(argv[optind]);
            }
            else if (sw=="-dist")
            {
                optind++;
                dist_km = atoi(argv[optind]);
            }
            optind++;
        }

    // TODO make it platform independent....
    string inputfolder="C:/Users/fbas/Documents/GitHub/DISPLACE_input_" +folder_name_parameterization;



    stringstream out;
    out << graph;
    string a_graph_s = out.str();

    ifstream coord_graph;
    string filename_graph=inputfolder+"/graphsspe/coord"+a_graph_s+".dat";
    coord_graph.open(filename_graph.c_str());
    if(coord_graph.fail())
    {
        open_file_error(filename_graph.c_str());
        return 1;
    }
    vector<double> graph_coord_x;
    vector<double> graph_coord_y;
    vector<int> graph_coord_harbour;
    if (!fill_from_coord(coord_graph, graph_coord_x, graph_coord_y, graph_coord_harbour, nrow)) {
        std::cerr << "Cannot parse " << filename_graph << " Bad format\n";
        return 1;
    }
    coord_graph.close();



    // full
    string filename_displace_input_for_data_merger;
    filename_displace_input_for_data_merger = inputfolder+"/popsspe_"+folder_name_parameterization+"/static_avai/displace_input_for_data_merger.dat";

    ifstream in;
    in.open(filename_displace_input_for_data_merger.c_str());
    if(in.fail())
    {
        open_file_error(filename_displace_input_for_data_merger.c_str());
        //return 1;
    }


    cout << "Calling avai field updater..." << endl;




    // read in
    multimap<int, double> full_avai_szgroup_nodes_with_pop;

    vector <string> Surveys;
    vector <int> Years;
    vector <int> ShootLons;
    vector <int> ShootLats;
    vector <string> Stocks;
    vector <int> StockIds;
    vector <double> nb_indiv0s;
    vector <double> nb_indiv1s;
    vector <double> nb_indiv2s;
    vector <double> nb_indiv3s;
    vector <double> nb_indiv4s;
    vector <double> nb_indiv5s;
    vector <double> nb_indiv6s;
    vector <double> nb_indiv7s;
    vector <double> nb_indiv8s;
    vector <double> nb_indiv9s;
    vector <double> nb_indiv10s;
    vector <double> nb_indiv11s;
    vector <double> nb_indiv12s;
    vector <double> nb_indiv13s;

    string Survey="";
    string Stock="";
    double Year=0, ShootLon=0, ShootLat=0, StockId=0;
    double nb_indiv0=0, nb_indiv1=0, nb_indiv2=0, nb_indiv3=0, nb_indiv4=0, nb_indiv5=0,
            nb_indiv6=0, nb_indiv7=0, nb_indiv8=0, nb_indiv9=0, nb_indiv10=0, nb_indiv11=0, nb_indiv12=0, nb_indiv13=0;

    std::string dummystring;
    getline (in, dummystring); // eat the heading

    while (!in.eof()) {


            std::string line;
            std::getline(in, line);
            if (in.eof())
                break;

            //  Survey / Year/ ShootLon / ShootLat / Stock / StockId / nb_indiv0 / nb_indiv1 /...
            in >> Survey;
            Surveys.push_back(Survey);
            in >> Year;
            Years.push_back(Year);
            in >> ShootLon;
            ShootLons.push_back(ShootLon);
            in >> ShootLat;
            ShootLats.push_back(ShootLat);
            in >> Stock;
            Stocks.push_back(Stock);
            in >> StockId;
            StockIds.push_back(StockId);
            in >> nb_indiv0;
            nb_indiv0s.push_back(nb_indiv0);
            in >> nb_indiv1;
            nb_indiv1s.push_back(nb_indiv1);
            in >> nb_indiv2;
            nb_indiv2s.push_back(nb_indiv2);
            in >> nb_indiv3;
            nb_indiv3s.push_back(nb_indiv3);
            in >> nb_indiv4;
            nb_indiv4s.push_back(nb_indiv4);
            in >> nb_indiv5;
            nb_indiv5s.push_back(nb_indiv5);
            in >> nb_indiv6;
            nb_indiv6s.push_back(nb_indiv6);
            in >> nb_indiv7;
            nb_indiv7s.push_back(nb_indiv7);
            in >> nb_indiv8;
            nb_indiv8s.push_back(nb_indiv8);
            in >> nb_indiv9;
            nb_indiv9s.push_back(nb_indiv9);
            in >> nb_indiv10;
            nb_indiv10s.push_back(nb_indiv10);
            in >> nb_indiv11;
            nb_indiv11s.push_back(nb_indiv11);
            in >> nb_indiv12;
            nb_indiv12s.push_back(nb_indiv12);
            in >> nb_indiv13;
            nb_indiv13s.push_back(nb_indiv13);
    }
    //cout  << "read the input file...ok " << endl;
    in.close();




    vector <double> idw0 (graph_coord_x.size());
    vector <double> idw1 (graph_coord_x.size());
    vector <double> idw2 (graph_coord_x.size());
    vector <double> idw3 (graph_coord_x.size());
    vector <double> idw4 (graph_coord_x.size());
    vector <double> idw5 (graph_coord_x.size());
    vector <double> idw6 (graph_coord_x.size());
    vector <double> idw7 (graph_coord_x.size());
    vector <double> idw8 (graph_coord_x.size());
    vector <double> idw9 (graph_coord_x.size());
    vector <double> idw10 (graph_coord_x.size());
    vector <double> idw11 (graph_coord_x.size());
    vector <double> idw12 (graph_coord_x.size());
    vector <double> idw13 (graph_coord_x.size());



    double a_dist;
    vector<int> idx_n_in_range;
    vector <vector <double> > weights;
    vector<double> sum_weights;
    for (int n=0; n<graph_coord_x.size(); n++) {
       for (int pt=0; pt<ShootLons.size(); pt++) {
            sum_weights.at(n)=0;
            weights.at(n).at(pt)=0.0;
            idw0.at(n)=0;
            idw1.at(n)=0;
            idw2.at(n)=0;
            idw3.at(n)=0;
            idw4.at(n)=0;
            idw5.at(n)=0;
            idw6.at(n)=0;
            idw7.at(n)=0;
            idw8.at(n)=0;
            idw9.at(n)=0;
            idw10.at(n)=0;
            idw11.at(n)=0;
            idw12.at(n)=0;
            idw13.at(n)=0;

    }}
    for (int pt=0; pt<ShootLons.size(); pt++) {
        // process line by line
       for (int n=0; n<graph_coord_x.size(); n++) {
        // search for each graph node

           a_dist = greatcircledistance(graph_coord_y.at(n), graph_coord_y.at(n), ShootLats.at(pt), ShootLons.at(pt));


        if (a_dist < dist_km && a_dist > 1e-5) {
            sum_weights.at(n)    += (1.0/a_dist);
            weights.at(n).at(pt)  = (1.0/a_dist);
            idx_n_in_range.push_back(n);
        } else{
            weights.at(n).at(pt)= 0;
        }

       }
     }

     for (int n=0; n<idx_n_in_range.size(); n++) {
           for (int pt=0; pt<ShootLons.size(); pt++) {
               int n2=idx_n_in_range.at(n);
                  if(sum_weights.at(n2)>1e-5){
                      idw0.at(n2) += nb_indiv0s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      idw1.at(n2) += nb_indiv1s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      idw2.at(n2) += nb_indiv2s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      idw3.at(n2) += nb_indiv3s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      idw4.at(n2) += nb_indiv4s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      idw5.at(n2) += nb_indiv5s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      idw6.at(n2) += nb_indiv6s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      idw7.at(n2) += nb_indiv7s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      idw8.at(n2) += nb_indiv8s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      idw9.at(n2) += nb_indiv9s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      idw10.at(n2) += nb_indiv10s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      idw11.at(n2) += nb_indiv11s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      idw12.at(n2) += nb_indiv12s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      idw13.at(n2) += nb_indiv13s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
           }
         }
       }







    // TODO: compute the avai










    // export back
     for (int pop=0; pop<StockIds.size(); pop++)
     {
     int a_pop = StockIds.at(pop);

     stringstream out;
     out << a_pop;
     string a_pop_s = out.str();

     ofstream avaiField;
    string filename_full_avai_file_out = inputfolder+"/popsspe_"+folder_name_parameterization+"/static_avai/" + a_pop_s +
            "spe_full_avai_szgroup_nodes_"+a_semester+"_updated.dat";
    avaiField.open(filename_full_avai_file_out.c_str());

    avaiField << "idx_node" << " " << "avai" << endl;
    for (multimap<int, double>::iterator pos=full_avai_szgroup_nodes_with_pop.begin(); pos != full_avai_szgroup_nodes_with_pop.end(); pos++)
    {
        avaiField << setprecision(6) << fixed;
            avaiField << pos->first << " " << pos->second << " ";
            avaiField << " " <<  endl;
    }
    avaiField.close();

    /*
     * ofstream avaiFieldSelected;
    string filename_avai_file_out = inputfolder+"/popsspe_"+folder_name_parameterization+"/static_avai/" + a_pop_s +
            "spe_avai_szgroup_nodes_"+a_semester+"_shuffled.dat";
    avaiFieldSelected.open(filename_avai_file_out.c_str());

    avaiFieldSelected << "idx_node" << " " << "avai" << endl;
    for (multimap<int, double>::iterator pos=avai_szgroup_nodes_with_pop.begin(); pos != avai_szgroup_nodes_with_pop.end(); pos++)
    {
        avaiFieldSelected << setprecision(6) << fixed;
            avaiFieldSelected << pos->first << " " << pos->second << " ";
            avaiFieldSelected << " " <<  endl;
    }
    */
    // avaiFieldSelected.close();

    } // end pop

    cout  << "export back the availability at szgroup field...ok " << endl;

    return 0;
}
