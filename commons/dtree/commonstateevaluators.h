#ifndef COMMONSTATEEVALUATORS_H
#define COMMONSTATEEVALUATORS_H

#include <dtree/stateevaluator.h>

namespace dtree {

template <typename T>
class VariableReferenceStateEvaluator: public StateEvaluator {
private:
    T& mVariable;
public:

    VariableReferenceStateEvaluator(T &var)
        : StateEvaluator(), mVariable(var) {
    }

    T evaluate() {
        return mVariable;
    }
};

template <typename T>
class ConstStateEvaluator : public StateEvaluator {
private:
    const T mConst;
public:
    ConstStateEvaluator(const T &var)
        : StateEvaluator(), mConst(var) {
    }

    double evaluate() {
        return mConst;
    }
};

template <typename Operator>
class TwoArgumentsComparatorStateEvaluator : public StateEvaluator {
private:
    StateEvaluator *mOp1, *mOp2;
    Operator mOperator;

public:
    TwoArgumentsComparatorStateEvaluator (StateEvaluator *op1, StateEvaluator *op2, Operator oper)
        : StateEvaluator(), mOp1(op1), mOp2(op2), mOperator(oper) {
    }

    double evaluate() {
        if (mOperator(mOp1->evaluate(), mOp2->evaluate())) {
            return 1.0;
        } else {
            return 0.0;
        }
    }
};

} // ns

#endif // COMMONSTATEEVALUATORS_H

