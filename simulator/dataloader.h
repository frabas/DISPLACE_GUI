#ifndef DATALOADER_H
#define DATALOADER_H

#include<iostream>
using namespace std;

// abstract class
class Dataloader
{
public:
    virtual int gettype(void)=0;
    virtual void features(){ cout << "Loading features" << endl;}
};


class Loader
{
public:
    void loadfeatures(Dataloader *dl)
    {
    dl->features();
    }
};


/* example of use to put in simulator main.cpp:
 *
 * Loader *l = new Loader;
 *
 * Dataloadervessels vl;
 * l->loadFeatures(&vl)
 *
 * Dataloaderbenthos bl;
 * l->loadFeatures(&bl)
 *
 */

#endif // DATALOADER_H

