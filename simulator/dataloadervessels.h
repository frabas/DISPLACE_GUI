#ifndef DATALOADERVESSELS_H
#define DATALOADERVESSELS_H

#include<dataloader.h>

class Dataloadervessels : public virtual Dataloader
{
public:
    Dataloadervessels();
    int gettype (void);
    void features();

};

#endif // DATALOADERVESSELS_H
