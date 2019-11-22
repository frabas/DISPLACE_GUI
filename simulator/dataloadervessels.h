#ifndef DATALOADERVESSELS_H
#define DATALOADERVESSELS_H

#include<dataloader.h>

#include<dataloader.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <deque>
#include <map>
#include <list>
#include <set>

#include "Node.h"
#include "Benthos.h"

#include "readdata.h"
#include "myutils.h"
#include "helpers.h"
#include "options.h"

using namespace std;


class Dataloadervessels : public virtual Dataloader
{
public:
    Dataloadervessels();
    int gettype (void);
    void features(const string &folder_name_parameterization,
                  const string &inputfolder,
                  PopSceOptions &dyn_pop_sce);

};

#endif // DATALOADERVESSELS_H
