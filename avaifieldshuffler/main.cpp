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


    cout << "Calling avai field shuffler..." << endl;


    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    // full
    string filename_full_avai_file;
    filename_full_avai_file = inputfolder+"/popsspe_"+folder_name_parameterization+"/static_avai/" + a_pop_s + "spe_full_avai_szgroup_nodes_"+a_semester+".dat";

    ifstream file_full_avai_szgroup_nodes_with_pop;
    file_full_avai_szgroup_nodes_with_pop.open(filename_full_avai_file.c_str());
    if(file_full_avai_szgroup_nodes_with_pop.fail())
    {
        open_file_error(filename_full_avai_file);
        //return 1;
    }

    /*
    // selected
    string filename_avai_file;
    filename_avai_file = inputfolder+"/popsspe_"+folder_name_parameterization+"/static_avai/" + a_pop_s + "spe_avai_szgroup_nodes_"+a_semester+".dat";

    ifstream file_avai_szgroup_nodes_with_pop;
    file_avai_szgroup_nodes_with_pop.open(filename_avai_file.c_str());
    if(file_avai_szgroup_nodes_with_pop.fail())
    {
        open_file_error(filename_avai_file);
        //return 1;
    }
    */

    // read in and populate (full) adding an error on the fly
    multimap<int, double> full_avai_szgroup_nodes_with_pop;
    string line;
    double normalisation_sum_before=0;
    double normalisation_sum_after=0;
    while(!getline(file_full_avai_szgroup_nodes_with_pop, line).eof())
    {
        int key;
        file_full_avai_szgroup_nodes_with_pop >> key;

        double val;
        file_full_avai_szgroup_nodes_with_pop >> val;

        // add a lognormal error on the fly
        double sd=0.6;
        double logn_error=0;
        logn_error= exp( 0 + sd*norm_rand() ) / exp((sd*sd)/2.0);;

        normalisation_sum_before+=val;

        val*=logn_error;

        normalisation_sum_after+=val;

        full_avai_szgroup_nodes_with_pop.insert(make_pair(key,val));
    }
    //cout  << "read the availability at szgroup...ok " << endl;
    file_full_avai_szgroup_nodes_with_pop.close();

    /*do
     *
     *
    // read in and populate (selected)
    multimap<int, double> avai_szgroup_nodes_with_pop;
    string line;
    while(!getline(file_avai_szgroup_nodes_with_pop, line).eof())
    {
        int key;
        file_avai_szgroup_nodes_with_pop >> key;
        double val;
        file_avai_szgroup_nodes_with_pop >> val;
        avai_szgroup_nodes_with_pop.insert(make_pair(key,val));
    }
    cout  << "read the availability at szgroup...ok " << endl;
    file_avai_szgroup_nodes_with_pop.close();
    */


    //cout << "normalisation_sum " << normalisation_sum_before/11 << endl;
    // here, normalize after the altered reading.
    for (multimap<int, double>::iterator pos=full_avai_szgroup_nodes_with_pop.begin(); pos != full_avai_szgroup_nodes_with_pop.end(); pos++)
    {

        //cout <<  " pos->second "  <<  pos->second << endl;
        (pos->second) = (pos->second) /(normalisation_sum_after/normalisation_sum_before);
        //cout <<  " pos->second after"  <<  pos->second << endl;
    }



    // export back
    ofstream avaiField;
    string filename_full_avai_file_out = inputfolder+"/popsspe_"+folder_name_parameterization+"/static_avai/" + a_pop_s +
            "spe_full_avai_szgroup_nodes_"+a_semester+"_shuffled.dat";
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
