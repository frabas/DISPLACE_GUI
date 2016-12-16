#include "graphbuilderworker.h"

void GraphBuilderWorker::execute()
{
    builder->setFeedback(this);
    result = builder->buildGraph();
    mMain->graphCreated(result);
}

void GraphBuilderWorker::setMax (int m)
{
    emit totalChanged(m);
}

void GraphBuilderWorker::setStep(int step)
{
    emit progress(step);
}
