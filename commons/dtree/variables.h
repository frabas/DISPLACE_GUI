#ifndef DTREEVARIABLES_H
#define DTREEVARIABLES_H

namespace dtree {

enum Variable {
    VarUndefined = -1,
    VarWeatherIs = 0,
    VarPriceIs,

    // keep this last
    VarLast
};

class VariableNames {
public:
    static const char *variableName(Variable);

private:
    static const char *const names[];
};

}

#endif // DTREEVARIABLES_H
