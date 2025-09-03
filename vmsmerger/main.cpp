#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <map>
#include <string>
#include <vector>

using namespace std;

void open_file_error(string filename)
{
    string error_msg = "error opening file " + filename;
    cout << error_msg << "\n";

    exit(-1);
}



/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/
/*****************************************************************/

int main(int argc, char* argv[])
    {
    string folder_name_parameterization="DanishFleet";
    string a_simu="simu2";
    string namefolderoutput="baseline";
    string outputfolder="C:/DISPLACE_outputs/"+folder_name_parameterization;
    int tstart =0;
    int tend=100;
    int nbpops=3;

    int foo=0;

    //typical usage:
    //    a_command = "vmsmerger.exe -f " +namefolderinput+ " -o " +outputfolder+ " -simu " +a_simu+ " -start " + tstart + " -tend " endt + " -nbpops " + nbpops;


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
            else if (sw == "-f2") {
                namefolderoutput = argv[++optind];
            }
            else if (sw == "-o") {
                outputfolder = argv[++optind];
            }
            else if (sw=="-simu")
            {
                optind++;
                a_simu = argv[optind];
            }
            else if (sw=="-startt")
            {
                optind++;
                foo = atoi(argv[optind]);
            }
            else if (sw=="-endt")
            {
                optind++;
                foo = atoi(argv[optind]);
            }
            else if (sw=="-nbpops")
            {
                optind++;
                nbpops = atoi(argv[optind]);
            }
            else if (sw=="-foo")
            {
                optind++;
                foo = atoi(argv[optind]);
            }
            optind++;
        }


    // open vmslike file
    string filename_vmslike;
    filename_vmslike = outputfolder+"/"+namefolderoutput+"/vmslikefpingsonly_"+a_simu+".dat";

    ifstream in_vms;
    in_vms.open(filename_vmslike.c_str());
    if(in_vms.fail())
    {
        open_file_error(filename_vmslike.c_str());
        //return 1;
        cout << "error when opening the vmslike file..." << "\n";
    int aa;
    cin >> aa;
    }


    // open logbooks or catch file file
    string filename_catches;
    filename_catches = outputfolder+"/"+namefolderoutput+"/tripcatchesperszgroup_"+a_simu+".dat";

    fstream in_catches;
    in_catches.open(filename_catches.c_str());
    if(in_catches.fail())
    {
        open_file_error(filename_catches.c_str());
        //return 1;
        cout << "error when opening the in_catch file..." << "\n";
    int aa;
    cin >> aa;
    }

    string filename_catches_temp1;
    filename_catches_temp1 = outputfolder+"/"+namefolderoutput+"/tripcatchesperszgroup_"+a_simu+"_temp1.dat";
    std::fstream in_catches_temp1( filename_catches_temp1, std::ios::binary ) ;
    in_catches_temp1 << in_catches.rdbuf() ;
    in_catches_temp1.close();

    string filename_catches_temp2;
    filename_catches_temp2 = outputfolder+"/"+namefolderoutput+"/tripcatchesperszgroup_"+a_simu+"_temp2.dat";
    std::fstream in_catches_temp2( filename_catches_temp2, std::ios::binary ) ;
    in_catches_temp2 << in_catches.rdbuf() ;
    in_catches_temp2.close();



    cout << "Calling VMS merger..." << "\n";


    //  open the output file
    ofstream mergedvms;
    string filename_mergedvms_out = outputfolder+"/"+namefolderoutput+"/mergedvms.dat";
    mergedvms.open(filename_mergedvms_out.c_str());


    int tstep_file1=0, tstep_file2;
    string vname_file1="", vname_file2="";
    int tstep_dep_file1=0, tstep_dep_file2=0; // give a primary key when combined with vname
    double lon=0, lat=0;
    double course=0, cumfuelcons=0;
    int state=2;
    int popid=0;
    double catch_szgroup0=0, catch_szgroup1=0, catch_szgroup2=0, catch_szgroup3=0, catch_szgroup4=0, catch_szgroup5=0, catch_szgroup6=0;
    double catch_szgroup7=0, catch_szgroup8=0, catch_szgroup9=0, catch_szgroup10=0, catch_szgroup11=0, catch_szgroup12=0, catch_szgroup13=0;

   // std::string dummystring;
   // getline (in_vms, dummystring); // eat the heading
   // getline (in_catches, dummystring); // eat the heading
    mergedvms << "tstep" << " " << "vname" << " " << "tstep_dep" << " " <<
                  "lon" << " " << "lat" << " " << "course" << " " << "cumfuelcons" <<  " " << "state" << " " << "popid" << " "
                    "catch_szgroup.0" << " " << "catch_szgroup.1" << " " << "catch_szgroup.2" << " " <<
                    "catch_szgroup.3" << " " << "catch_szgroup.4" << " " << "catch_szgroup.5" << " " <<
                    "catch_szgroup.6" << " " << "catch_szgroup.7" << " " << "catch_szgroup.8" << " " <<
                     "catch_szgroup.9" << " " << "catch_szgroup.10" << " " << "catch_szgroup.11" << " " <<
                     "catch_szgroup.12" << " " << "catch_szgroup.13" << "\n";

     int linenum = 0;
     int a_countpings = 1;
     map <string, int> count_pings;
     string key_file1="";
     string last_key_file1="";
     string key_file2="";



     // a first screening to get count pings
     cout << "Getting nb of pings per trip between tstart "<< tstart << " and tend " << tend << "..." << "\n";
     std::string line_in_vms;
     while (!std::getline(in_vms, line_in_vms).eof()) { // process line
       //  cout << "Processing VMS line " << linenum << "\n";

            std::istringstream iss(line_in_vms);
            if (!(iss  >> tstep_file1 >> vname_file1 >> tstep_dep_file1 >> lon >> lat >> course >> cumfuelcons >> state)) {
                cout << "error at line " << linenum << ": fix the input VMS file for missing or mixed fields!!" << "\n";
                in_vms.clear();
                in_vms.seekg(0, ios::beg);
                break; } // error

         if(tstep_file1<tstart || tstep_file1>tend) break;

         if(state==1){
             stringstream out;
             out << tstep_dep_file1;
             string tstep_dep_file1_s = out.str();

             key_file1=vname_file1+"_"+tstep_dep_file1_s;

             count_pings[key_file1]+=1;
             //cout << "key_file1 is " << key_file1 << " then count_pings[key_file1] is "  << count_pings[key_file1] <<  "\n";
            }
        ++linenum;
     }
     in_vms.clear();
     in_vms.seekg(0, ios::beg);


     cout << "Getting nb of pings per trip...OK" << "\n";



     //...then a second screening
    linenum=0;
    std::vector<int> already_found (400000, 0);
     cout << "Processing VMS line..." << "\n";
     while (!std::getline(in_vms, line_in_vms).eof()) { // process line

         cout << "Processing VMS line " << linenum << "\n";

            std::istringstream iss(line_in_vms);
            if (!(iss  >> tstep_file1 >> vname_file1 >> tstep_dep_file1 >> lon >> lat >> course >> cumfuelcons >> state)) {
                cout << "error at line " << linenum << ": fix the input VMS file for missing or mixed fields!!" << "\n";
                in_vms.clear();
                in_vms.seekg(0, ios::beg);
                break; } // error

            if(tstep_file1<tstart || tstep_file1>tend) break;

            stringstream out;
            out << tstep_dep_file1;
            string tstep_dep_file1_s = out.str();

            if(linenum!=0) last_key_file1= key_file1;
            key_file1=vname_file1+"_"+tstep_dep_file1_s;


            int found=0;
            int linenum2=0;
            std::string line_in_catches;

           // in_catches_temp1.open(filename_catches_temp1.c_str());
           // in_catches_temp2.open(filename_catches_temp2.c_str());
           // in_catches_temp1 << in_catches_temp2.rdbuf();
           // in_catches_temp2.close();
           // in_catches_temp2.open(filename_catches_temp2.c_str(), std::ofstream::out | std::ofstream::trunc);


            if(state==1) while (!std::getline(in_catches, line_in_catches).eof()  && found<nbpops) { // search for merging only if VMS point corresponds to a fishing position (i.e. state =1)

           //    cout << "Processing catch file line " << linenum2 << " ...found so far: " << found << "\n";
            //   if(!already_found[linenum2]){ // do not skip

               std::istringstream iss(line_in_catches);
               if (!(iss   >> tstep_file2 >> vname_file2 >> tstep_dep_file2 >> popid >> catch_szgroup0 >> catch_szgroup1 >> catch_szgroup2 >>
                        catch_szgroup3 >> catch_szgroup4 >> catch_szgroup5 >> catch_szgroup6 >> catch_szgroup7 >> catch_szgroup8 >>
                          catch_szgroup9 >> catch_szgroup10 >> catch_szgroup11 >> catch_szgroup12 >> catch_szgroup13)) {
                                       cout << "error at line " << linenum2 << ": fix the input catch file for missing or mixed fields!!" << "\n";
                                       cout << "tstep_file2 is " << tstep_file2 << ", vname_file2 is " << vname_file2 << "\n";
                                       in_catches.clear();
                                       in_catches.seekg(0, ios::beg);
                                         break;
               } // error


                stringstream out;
                out << tstep_dep_file2;
                string tstep_dep_file2_s = out.str();

                key_file2=vname_file2+"_"+tstep_dep_file2_s;
                //cout << "key_file2 is " << key_file1  << " key_file2 is " << key_file2 << "\n";

                // populate the merged file hereafter:
               if(key_file1==key_file2){
                   ++found;
                   already_found[linenum2]=1;

                   a_countpings = count_pings[key_file1];
                   mergedvms << tstep_file1 << " " << vname_file1 << " " << key_file1 << " " <<
                          lon << " " << lat << " " << course << " " << cumfuelcons <<  " " << state << " " << popid << " " <<
                            catch_szgroup0/a_countpings << " " << catch_szgroup1/a_countpings << " " << catch_szgroup2/a_countpings << " " <<
                            catch_szgroup3/a_countpings << " " << catch_szgroup4/a_countpings << " " << catch_szgroup5/a_countpings << " " <<
                            catch_szgroup6/a_countpings << " " << catch_szgroup7/a_countpings << " " << catch_szgroup8/a_countpings << " " <<
                            catch_szgroup9/a_countpings << " " << catch_szgroup10/a_countpings << " " << catch_szgroup11/a_countpings << " " <<
                            catch_szgroup12/a_countpings << " " << catch_szgroup13/a_countpings  << "\n";
             /* } else{
                    in_catches_temp2  << tstep_file2 << " " << vname_file2 << " " << tstep_dep_file2 << " " << popid << " " <<
                                         catch_szgroup0 << " " << catch_szgroup1 << " " << catch_szgroup2 <<" " <<
                                           catch_szgroup3 << " " << catch_szgroup4 << " " <<  catch_szgroup5 << " " << catch_szgroup6 << " " <<
                                            catch_szgroup7 << " " << catch_szgroup8 << " " <<
                                             catch_szgroup9 << " " << catch_szgroup10 << " " << catch_szgroup11 << " " << catch_szgroup12 << " " << catch_szgroup13  << "\n";
             */
               }
           // }

         ++linenum2;
        }
        in_catches.clear();
        in_catches.seekg(0, ios::beg);
        in_catches_temp2.close();

      ++linenum;
     }
    mergedvms.close();
    cout  << "Process VMS lines..ok " << "\n";







    return 0;
}
