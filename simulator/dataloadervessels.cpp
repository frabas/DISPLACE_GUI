#include "dataloadervessels.h"
#include<iostream>

using namespace std;

Dataloadervessels::Dataloadervessels()
{

}

int Dataloadervessels::gettype()
{
    return(1);
}

void Dataloadervessels::features()
{
  this->Dataloader::features();
  cout << "Loading vessels features" << endl;
}

