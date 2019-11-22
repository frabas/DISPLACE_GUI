#ifndef DATALOADERBENTHOS_H
#define DATALOADERBENTHOS_H

#include<dataloader.h>

class Dataloaderbenthos : public virtual Dataloader
{
public:
    Dataloaderbenthos();
    int gettype (void);
    void features();


};

#endif // DATALOADERBENTHOS_H
