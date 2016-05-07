#include "outputexporter.h"

#include <mutexlocker.h>
#include <Vessel.h>

extern pthread_mutex_t glob_mutex;
using namespace std;

std::shared_ptr<OutputExporter> OutputExporter::mInstance = nullptr;

OutputExporter::OutputExporter(const string &basepath, const string &namesimu)
{
    auto filename=basepath + "/vmslike_" + namesimu + ".dat";
    mVmsLike.open(filename.c_str());
}

void OutputExporter::exportVmsLike(unsigned int tstep, Vessel *vessel)
{
    MutexLocker l(&glob_mutex);

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

void OutputExporter::close()
{
    mVmsLike.close();
}

bool OutputExporter::instantiate(const string &basepath, const string &namesimu)
{
    mInstance = std::make_shared<OutputExporter>(basepath, namesimu);
    return true;
}
