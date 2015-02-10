#include "fileformathelper.h"

#include <cmath>

const double FileFormatHelper::WeightRoundingFactor = 1000;

double FileFormatHelper::roundWeight(double w)
{
    return std::floor(w * WeightRoundingFactor + 0.5) / WeightRoundingFactor;
}

