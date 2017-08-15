#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>

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
    string inputfolder="C:/Users/fbas/Documents/GitHub/DISPLACE_input_" +folder_name_parameterization;


    cout << "AvaiFieldShuffle..." << endl;


    // casting a_pop into a string
    stringstream out;
    out << a_pop;
    string a_pop_s = out.str();

    string filename_avai_file;
    filename_avai_file = inputfolder+"/popsspe_"+folder_name_parameterization+"/static_avai/" + a_pop_s + "spe_avai_szgroup_nodes_"+a_semester+".dat";

    ifstream file_avai_szgroup_nodes_with_pop;
    file_avai_szgroup_nodes_with_pop.open(filename_avai_file.c_str());
    if(file_avai_szgroup_nodes_with_pop.fail())
    {
        open_file_error(filename_avai_file);
        //return 1;
    }

    // read in and populate
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



    // here, alter.
    // TODO




    // export back
    ofstream avaiField;
    string filename_avai_file_out = inputfolder+"/popsspe_"+folder_name_parameterization+"/static_avai/" + a_pop_s + "spe_avai_szgroup_nodes_"+a_semester+"_avaiField.dat";
    avaiField.open(filename_avai_file_out.c_str());

    avaiField << "idx_node" << " " << "avai" << endl;
    for (multimap<int, double>::iterator pos=avai_szgroup_nodes_with_pop.begin(); pos != avai_szgroup_nodes_with_pop.end(); pos++)
    {
        avaiField << setprecision(6) << fixed;
            avaiField << pos->first << " " << pos->second << " ";
            avaiField << " " <<  endl;
    }
    cout  << "export back the availability at szgroup...ok " << endl;

    return 0;
}
