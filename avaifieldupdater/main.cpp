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

int main(int argc, char* argv[])
    {
    int a_pop=0;
    string folder_name_parameterization="myfish";
    string a_semester="semester1";

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
            else if (sw=="-p")
            {
                optind++;
                a_pop = atoi(argv[optind]);
            }
            optind++;
        }

    // TODO make it platform independent....
    string inputfolder="C:/Users/fbas/Documents/GitHub/DISPLACE_input_" +folder_name_parameterization;


    // full
    string filename_displace_input_for_data_merger;
    filename_displace_input_for_data_merger = inputfolder+"/popsspe_"+folder_name_parameterization+"/static_avai/file_displace_input_for_data_merger.dat";

    ifstream file_displace_input_for_data_merger;
    file_displace_input_for_data_merger.open(filename_displace_input_for_data_merger.c_str());
    if(file_displace_input_for_data_merger.fail())
    {
        open_file_error(filename_displace_input_for_data_merger);
        //return 1;
    }


    cout << "Calling avai field updater..." << endl;


    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();


    // read in and populate (full) adding an error on the fly
    multimap<int, double> full_avai_szgroup_nodes_with_pop;
    string line;
    while(!getline(file_displace_input_for_data_merger, line).eof())
    {



        // TO DO





    }
    //cout  << "read the availability at szgroup...ok " << endl;
    file_displace_input_for_data_merger.close();




    // export back
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



    cout  << "export back the availability at szgroup field...ok " << endl;

    return 0;
}
