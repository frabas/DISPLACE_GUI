#ifndef STATEEVALUATOR_H
#define STATEEVALUATOR_H

namespace dtree {

/** \brief An abstract class to allow internal/External states to be evaluated
 */
class StateEvaluator
{
public:
    StateEvaluator();
    virtual ~StateEvaluator();

    virtual double operator() = 0;
};

} // ns dtree

#endif // STATEEVALUATOR_H
