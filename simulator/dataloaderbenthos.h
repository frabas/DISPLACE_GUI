#ifndef DATALOADERBENTHOS_H
#define DATALOADERBENTHOS_H

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



class Dataloaderbenthos : public virtual Dataloader
{
public:
    Dataloaderbenthos();
    int gettype (void);
    void features(const string& folder_name_parameterization,
                  const string& inputfolder,
                  PopSceOptions& dyn_pop_sce);


};

#endif // DATALOADERBENTHOS_H
