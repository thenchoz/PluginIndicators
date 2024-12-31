#ifndef INDICATORSTYPE_HH
#define INDICATORSTYPE_HH

#include <string>

namespace indicatorsType
{
    enum indicators {WARPING, ASPECTRATIO, SKEWNESS, TAPER, INTERPOLATIONQUALITY, MEANRATIO, SHAPEREGULARITY};

    std::string as_s(const indicators& i);
}

#endif //INDICATORSTYPE_HH