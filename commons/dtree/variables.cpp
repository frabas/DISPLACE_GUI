#include <dtree/variables.h>

using namespace dtree ;

const char *const VariableNames::names[] = {
    "weather_is",
    "price_is",

    // keep this last
    0
};


const char *VariableNames::variableName(Variable id)
{
    if ( id < 0 || id >= VarLast)
        return 0;

    return names[id];
}

Variable VariableNames::variableCode(const std::string &name)
{
    int i = 0;
    while (names[i] != 0) {
        if (names[i] == name)
            return static_cast<Variable>(i);
        ++i;
    }

    return VarUndefined;
}
