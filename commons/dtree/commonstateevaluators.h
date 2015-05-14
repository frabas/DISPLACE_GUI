#ifndef COMMONSTATEEVALUATORS_H
#define COMMONSTATEEVALUATORS_H

#include <dtree/stateevaluator.h>

namespace dtree {

/** \brief A templated class to evaluate a static variable
 *
 * The evaluate() method simply returns the value of the referenced variable.
 * */
template <typename T>
class VariableReferenceStateEvaluator: public StateEvaluator {
private:
    T& mVariable;
public:

    VariableReferenceStateEvaluator(T &var)
        : StateEvaluator(), mVariable(var) {
    }

    double evaluate() const {
        return static_cast<double>(mVariable);
    }
};

/** \brief A templated class that evaluates constant-valued variable
 *
 * */
template <typename T>
class ConstStateEvaluator : public StateEvaluator {
private:
    const T mConst;
public:
    ConstStateEvaluator(const T &var)
        : StateEvaluator(), mConst(var) {
    }

    double evaluate() const {
        return static_cast<double>(mConst);
    }
};


/** \brief A decorator class that evaluates the relation between the values of two other evaluators
 *
 * This class takes an operator as argument in the constructor, and uses it to operate on the two evaluators.
 * A useful application is comparing two values.
 * The following code snippet illustrate how to compare a double variable with a constant using the std::less
 * functional class
 * \code
 *   #include <functional>
 *
 *   StateEvaluator *myEvaluator = new dtree::TwoArgumentsOperatorStateEvaluator<std::less<double> >(
 *               new dtree::VariableReferenceStateEvaluator<double>(myvalue),
 *               new dtree::ConstStateEvaluator<double>(1000.0),
 *               std::less<double>());
 * \endcode
 *
 * */
template <typename Operator>
class TwoArgumentsComparatorStateEvaluator : public StateEvaluator {
private:
    StateEvaluator *mOp1, *mOp2;
    Operator mOperator;

public:
    TwoArgumentsComparatorStateEvaluator (StateEvaluator *op1, StateEvaluator *op2, Operator oper)
        : StateEvaluator(), mOp1(op1), mOp2(op2), mOperator(oper) {
    }

    /** \brief Overridden method. Apply an operator (passed as template parameter) to two other evaluators.
     * \return 1.0 if operator returns true, 0.0 if false */
    double evaluate() const {
        if (mOperator(mOp1->evaluate(), mOp2->evaluate())) {
            return 1.0;
        } else {
            return 0.0;
        }
    }
};

} // ns

#endif // COMMONSTATEEVALUATORS_H

