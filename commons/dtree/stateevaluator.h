#ifndef STATEEVALUATOR_H
#define STATEEVALUATOR_H

class Vessel;
class Node;

namespace dtree {

/** \brief An abstract class to allow internal/External states to be evaluated
 */
class StateEvaluator
{
public:
    StateEvaluator() {}
    virtual ~StateEvaluator() {}

    /** \brief evaluate the state/variable, and returns the selected output, as double.
     * Client can decide what to do with the value, for example, it can round it and use it as an index
     * to select the proper output in a node.
     * It is a client responsibility, in particular, to handle special cases, for example, if the
     * index is outside the range of expected outputs.
     * A decorator class can eventually remap outputs.
     * \arg tstep The current Time Step of the simulation.
     * \arg vessel A pointer for the vessel being evaluated.
     * */
    virtual double evaluate(int tstep, Vessel *vessel) const = 0;
    virtual double evaluateAreaType(int tstep, Node *node) const = 0;
};

} // ns dtree

#endif // STATEEVALUATOR_H
