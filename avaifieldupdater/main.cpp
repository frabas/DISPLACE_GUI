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
    string folder_name_parameterization="DanishFleet";
    string a_semester="semester1";
    int nrow=35309; // 10140 for myfish
    int dist_km = 50;
    int graph=40; // 56 for myfish

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
        cout << "failing opening the graph file" << endl;
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
    int aa;
    cin >> aa;
    }


    cout << "Calling avai field updater..." << endl;




    // read in
    multimap<int, double> full_avai_szgroup_nodes_with_pop;

    vector <string> Surveys;
    vector <int> Years;
    vector <double> ShootLons;
    vector <double> ShootLats;
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
    double ShootLon=0, ShootLat=0;
    int Year=0, StockId=0;
    double nb_indiv0=0, nb_indiv1=0, nb_indiv2=0, nb_indiv3=0, nb_indiv4=0, nb_indiv5=0,
            nb_indiv6=0, nb_indiv7=0, nb_indiv8=0, nb_indiv9=0, nb_indiv10=0, nb_indiv11=0, nb_indiv12=0, nb_indiv13=0;

    std::string dummystring;
    getline (in, dummystring); // eat the heading

     int linenum = 0;
     while (!in.eof()) {


            std::string line;
            std::getline(in, line);
           // if (in.eof())
           //     break;
//cout << "new line read..." << endl;
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
 //cout <<  Survey << " " << Year << " " << ShootLon  << " " <<   ShootLat << " " <<   Stock << " " <<   StockId << " " <<   nb_indiv0  << " " <<   nb_indiv1 << " " <<   nb_indiv13 << endl;
    ++linenum;
    }
    cout  << "read the input file...ok " << endl;
    in.close();


    //remove dups in StockIds
    vector<int> listStockIds (StockIds);
    sort( listStockIds.begin(), listStockIds.end() ) ;
    listStockIds.erase( unique( listStockIds.begin(), listStockIds.end() ), listStockIds.end() ) ;
    int max_idx_stock = *max_element(std::begin(listStockIds), std::end(listStockIds));
    max_idx_stock=max_idx_stock+1; // because vector idx starts with 0

    vector <vector<double> > idw0 (graph_coord_x.size(), vector<double>(max_idx_stock));
    vector <vector<double> > idw1 (graph_coord_x.size(), vector<double>(max_idx_stock));
    vector <vector<double> > idw2 (graph_coord_x.size(), vector<double>(max_idx_stock));
    vector <vector<double> > idw3 (graph_coord_x.size(), vector<double>(max_idx_stock));
    vector <vector<double> > idw4 (graph_coord_x.size(), vector<double>(max_idx_stock));
    vector <vector<double> > idw5 (graph_coord_x.size(), vector<double>(max_idx_stock));
    vector <vector<double> > idw6 (graph_coord_x.size(), vector<double>(max_idx_stock));
    vector <vector<double> > idw7 (graph_coord_x.size(), vector<double>(max_idx_stock));
    vector <vector<double> > idw8 (graph_coord_x.size(), vector<double>(max_idx_stock));
    vector <vector<double> > idw9 (graph_coord_x.size(), vector<double>(max_idx_stock));
    vector <vector<double> > idw10 (graph_coord_x.size(), vector<double>(max_idx_stock));
    vector <vector<double> > idw11 (graph_coord_x.size(), vector<double>(max_idx_stock));
    vector <vector<double> > idw12 (graph_coord_x.size(), vector<double>(max_idx_stock));
    vector <vector<double> > idw13 (graph_coord_x.size(), vector<double>(max_idx_stock));



    double a_dist;
    vector<vector<int> > idx_n_in_range(max_idx_stock);
    vector< vector<double> > weights(graph_coord_x.size(), vector<double>(ShootLons.size()));
    vector <double> sum_weights (graph_coord_x.size());
    for (int n=0; n<graph_coord_x.size(); n++) {
       for (int pt=0; pt<ShootLons.size(); pt++) {
            sum_weights.at(n)=0;
            weights.at(n).at(pt)=0.0;
       }
       for (int st=0; st<max_idx_stock; st++) {
               idw0.at(n).at(st)=0;
               idw1.at(n).at(st)=0;
               idw2.at(n).at(st)=0;
               idw3.at(n).at(st)=0;
               idw4.at(n).at(st)=0;
               idw5.at(n).at(st)=0;
               idw6.at(n).at(st)=0;
               idw7.at(n).at(st)=0;
               idw8.at(n).at(st)=0;
               idw9.at(n).at(st)=0;
               idw10.at(n).at(st)=0;
               idw11.at(n).at(st)=0;
               idw12.at(n).at(st)=0;
               idw13.at(n).at(st)=0;
      }
    }
    cout  << "init for idw...ok " << endl;


    for (int pt=0; pt<ShootLons.size(); pt++) {
        // process line by line
       for (int n=0; n<graph_coord_x.size(); n++) {
        // search for each graph node

           a_dist = greatcircledistance(graph_coord_x.at(n), graph_coord_y.at(n), ShootLons.at(pt), ShootLats.at(pt));
           //cout  << "a_dist... " << a_dist << endl;


        if (a_dist < dist_km && a_dist > 1e-5) {
            sum_weights.at(n)    += (1.0/a_dist);
            weights.at(n).at(pt)  = (1.0/a_dist);
            int st=StockIds.at(pt);
            idx_n_in_range.at(st).push_back(n);
        } else{
            weights.at(n).at(pt)= 0;
        }

       }
     }
 cout  << "weights in idw computed...ok " << endl;



for(int i=0; i<listStockIds.size(); i++){
    // remove dups indx
    int st=listStockIds.at(i);

    sort( idx_n_in_range.at(st).begin(), idx_n_in_range.at(st).end() ) ;
    idx_n_in_range.at(st).erase( unique( idx_n_in_range.at(st).begin(), idx_n_in_range.at(st).end() ), idx_n_in_range.at(st).end() ) ;

    cout  << "nb nodes this stock " << st << " within the km range is... " <<  idx_n_in_range.at(st).size() << endl;

    }

    vector<double> totidw0 (max_idx_stock, 0);
     vector<double> totidw1 (max_idx_stock, 0);
     vector<double> totidw2 (max_idx_stock, 0);
     vector<double> totidw3 (max_idx_stock, 0);
     vector<double> totidw4 (max_idx_stock, 0);
     vector<double> totidw5 (max_idx_stock, 0);
     vector<double> totidw6 (max_idx_stock, 0);
     vector<double> totidw7 (max_idx_stock, 0);
     vector<double> totidw8 (max_idx_stock, 0);
     vector<double> totidw9 (max_idx_stock, 0);
     vector<double> totidw10 (max_idx_stock, 0);
     vector<double> totidw11 (max_idx_stock, 0);
     vector<double> totidw12 (max_idx_stock, 0);
     vector<double> totidw13 (max_idx_stock, 0);
       for (int pt=0; pt<ShootLons.size(); pt++) {
         //cout <<" process line "<< pt << endl;
           int st=StockIds.at(pt);
          for (int n=0; n<idx_n_in_range.at(st).size(); n++) {
               int n2=idx_n_in_range.at(st).at(n);
               //cout  << "for line " << pt << "sum_weights  this node " << n2 << " is... " <<  sum_weights.at(n2) << endl;
                  if(sum_weights.at(n2)>1e-5){
                      //cout  << "st is " << st << "given size of idw0.at(n2) " << idw0.at(n2).size() << endl;
                      idw0.at(n2).at(st) += nb_indiv0s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      totidw0.at(st) += idw0.at(n2).at(st);

 /*
if(st==11){
         cout << " for pop11 totidw0.at(st) is " <<  totidw0.at(st)  << " given " << idw0.at(n2).at(st) << endl;
         cout << " because  nb_indiv0s.at(pt) is "  << nb_indiv0s.at(pt) << endl;

       if(n>5){
          int aa;
         cin >> aa;
        }
  }
*/

                      idw1.at(n2).at(st) += nb_indiv1s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      totidw1.at(st) += idw1.at(n2).at(st);
                      idw2.at(n2).at(st) += nb_indiv2s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      totidw2.at(st) += idw2.at(n2).at(st);
                      idw3.at(n2).at(st) += nb_indiv3s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      totidw3.at(st) += idw3.at(n2).at(st);
                      idw4.at(n2).at(st) += nb_indiv4s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      totidw4.at(st) += idw4.at(n2).at(st);
                      idw5.at(n2).at(st) += nb_indiv5s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      totidw5.at(st) += idw5.at(n2).at(st);
                      idw6.at(n2).at(st) += nb_indiv6s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      totidw6.at(st) += idw6.at(n2).at(st);
                      idw7.at(n2).at(st) += nb_indiv7s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      totidw7.at(st) += idw7.at(n2).at(st);
                      idw8.at(n2).at(st) += nb_indiv8s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      totidw8.at(st) += idw8.at(n2).at(st);
                      idw9.at(n2).at(st) += nb_indiv9s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      totidw9.at(st) += idw9.at(n2).at(st);
                      idw10.at(n2).at(st) += nb_indiv10s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      totidw10.at(st) += idw10.at(n2).at(st);
                      idw11.at(n2).at(st) += nb_indiv11s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      totidw11.at(st) += idw11.at(n2).at(st);
                      idw12.at(n2).at(st) += nb_indiv12s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      totidw12.at(st) += idw12.at(n2).at(st);
                      idw13.at(n2).at(st) += nb_indiv13s.at(pt) * ( weights.at(n2).at(pt) / sum_weights.at(n2) ); // inverse-distance weighting average
                      totidw13.at(st) += idw13.at(n2).at(st);
           }
         }
       }


     cout  << "idw computed...ok " << endl;



    // export back
     for (int i=0; i<listStockIds.size(); i++)
     {
     int a_pop = listStockIds.at(i);
     cout  << "export for a_pop... " << a_pop << endl;

     stringstream out;
     out << a_pop;
     string a_pop_s = out.str();

     ofstream avaiField;
    string filename_full_avai_file_out = inputfolder+"/popsspe_"+folder_name_parameterization+"/static_avai/" + a_pop_s +
            "spe_full_avai_szgroup_nodes_"+a_semester+"_updated.dat";
    avaiField.open(filename_full_avai_file_out.c_str());

    avaiField << "idx_node" << " " << "avai" << endl;
    for (int n=0; n<idx_n_in_range.at(a_pop).size(); n++)
     {
         int n2=idx_n_in_range.at(a_pop).at(n);


/*
 * if(n2==50 && a_pop==11){
      cout << n2 << " 0: " << idw0.at(n2).at(a_pop)/totidw0.at(a_pop) << endl;
      cout << "because " << idw0.at(n2).at(a_pop) << "   "<< totidw0.at(a_pop) << endl;
      cout << n2 << " 1: " << idw1.at(n2).at(a_pop)/totidw1.at(a_pop) << endl;
      cout << "because " << idw1.at(n2).at(a_pop) << "   "<< totidw1.at(a_pop) << endl;
      cout << n2 << " 2: " << idw2.at(n2).at(a_pop)/totidw2.at(a_pop) << endl;
      cout << "because " << idw2.at(n2).at(a_pop) << "   "<< totidw2.at(a_pop) << endl;
      cout << n2 << " 3: " << idw3.at(n2).at(a_pop)/totidw3.at(a_pop) << endl;
      cout << "because " << idw3.at(n2).at(a_pop) << "   "<< totidw3.at(a_pop) << endl;
      cout << n2 << " 4: " << idw4.at(n2).at(a_pop)/totidw4.at(a_pop) << endl;
      cout << "because " << idw4.at(n2).at(a_pop) << "   "<< totidw4.at(a_pop) << endl;
      cout << n2 << " 5: " << idw5.at(n2).at(a_pop)/totidw5.at(a_pop) << endl;
      cout << "because " << idw5.at(n2).at(a_pop) << "   "<< totidw5.at(a_pop) << endl;
      cout << n2 << " 6: " << idw6.at(n2).at(a_pop)/totidw6.at(a_pop) << endl;
      cout << "because " << idw6.at(n2).at(a_pop) << "   "<< totidw6.at(a_pop) << endl;
      int aa;
      cin>>aa;

}
*/

         avaiField << setprecision(9) << fixed;

         if(totidw0.at(a_pop)>1e-5){
            avaiField << setprecision(9) << fixed << n2  << " " << idw0.at(n2).at(a_pop)/totidw0.at(a_pop);
         } else {
            avaiField << setprecision(0) << fixed <<  n2 << " " << 0;
         }
         avaiField << " " <<  endl;

         if(totidw1.at(a_pop)>1e-5){
               avaiField << setprecision(9) << fixed << n2  << " " << idw1.at(n2).at(a_pop)/totidw1.at(a_pop);
         } else {
               avaiField << setprecision(0) << fixed <<  n2 << " " << 0;
         }
         avaiField << " " <<  endl;

         if(totidw2.at(a_pop)>1e-5){
               avaiField << setprecision(9) << fixed << n2  << " " << idw2.at(n2).at(a_pop)/totidw2.at(a_pop);
            } else {
            avaiField << setprecision(0) << fixed <<  n2 << " " << 0;
            }
            avaiField << " " <<  endl;

            if(totidw3.at(a_pop)>1e-5){
                  avaiField << setprecision(9) << fixed << n2  << " " << idw3.at(n2).at(a_pop)/totidw3.at(a_pop);
            } else {
              avaiField << setprecision(0) << fixed <<  n2 << " " << 0;
            }
            avaiField << " " <<  endl;

            if(totidw4.at(a_pop)>1e-5){
                  avaiField << setprecision(9) << fixed << n2  << " " << idw4.at(n2).at(a_pop)/totidw4.at(a_pop);
            } else {
              avaiField << setprecision(0) << fixed <<  n2 << " " << 0;
            }
            avaiField << " " <<  endl;

            if(totidw5.at(a_pop)>1e-5){
                  avaiField << setprecision(9) << fixed << n2  << " " << idw5.at(n2).at(a_pop)/totidw5.at(a_pop);
            } else {
              avaiField << setprecision(0) << fixed <<  n2 << " " << 0;
            }
            avaiField << " " <<  endl;

            if(totidw6.at(a_pop)>1e-5){
                  avaiField << setprecision(9) << fixed << n2  << " " << idw6.at(n2).at(a_pop)/totidw6.at(a_pop);
            } else {
              avaiField << setprecision(0) << fixed <<  n2 << " " << 0;
            }
            avaiField << " " <<  endl;

            if(totidw7.at(a_pop)>1e-5){
                  avaiField << setprecision(9) << fixed << n2  << " " << idw7.at(n2).at(a_pop)/totidw7.at(a_pop);
            } else {
               avaiField << setprecision(0) << fixed <<  n2 << " " << 0;
            }
            avaiField << " " <<  endl;

            if(totidw8.at(a_pop)>1e-5){
                  avaiField << setprecision(9) << fixed << n2  << " " << idw8.at(n2).at(a_pop)/totidw8.at(a_pop);
            } else {
               avaiField << setprecision(0) << fixed <<  n2 << " " << 0;
            }
            avaiField << " " <<  endl;

            if(totidw9.at(a_pop)>1e-5){
                  avaiField << setprecision(9) << fixed << n2  << " " << idw9.at(n2).at(a_pop)/totidw9.at(a_pop);
            } else {
            avaiField << setprecision(0) << fixed <<  n2 << " " << 0;
            }
            avaiField << " " <<  endl;

            if(totidw10.at(a_pop)>1e-5){
                  avaiField << setprecision(9) << fixed << n2  << " " << idw10.at(n2).at(a_pop)/totidw10.at(a_pop);
            } else {
              avaiField << setprecision(0) << fixed <<  n2 << " " << 0;
            }
            avaiField << " " <<  endl;

            if(totidw11.at(a_pop)>1e-5){
                  avaiField << setprecision(9) << fixed << n2  << " " << idw11.at(n2).at(a_pop)/totidw11.at(a_pop);
            } else {
              avaiField << setprecision(0) << fixed <<  n2 << " " << 0;
            }
            avaiField << " " <<  endl;

            if(totidw12.at(a_pop)>1e-5){
                  avaiField << setprecision(9) << fixed << n2  << " " << idw12.at(n2).at(a_pop)/totidw12.at(a_pop);
            } else {
               avaiField << setprecision(0) << fixed <<  n2 << " " << 0;
            }
            avaiField << " " <<  endl;

            if(totidw13.at(a_pop)>1e-5){
                  avaiField << setprecision(9) << fixed << n2  << " " << idw13.at(n2).at(a_pop)/totidw13.at(a_pop);
            } else {
                avaiField << setprecision(0) << fixed <<  n2 << " " << 0;
            }
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
