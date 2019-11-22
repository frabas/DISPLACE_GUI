#include "dataloaderbenthos.h"
#include<iostream>

using namespace std;

Dataloaderbenthos::Dataloaderbenthos()
{

}

int Dataloaderbenthos::gettype()
{
    return(2);
}

void Dataloaderbenthos::features()
{
  this->Dataloader::features();
  cout << "Loading benthos features" << endl;
}
