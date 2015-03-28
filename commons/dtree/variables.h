#ifndef DTREEVARIABLES_H
#define DTREEVARIABLES_H

#include <string>

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
    static Variable variableCode (const std::string &name);

private:
    static const char *const names[];
};

}

#endif // DTREEVARIABLES_H
