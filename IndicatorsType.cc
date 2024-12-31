#include "IndicatorsType.hh"

std::string indicatorsType::as_s(const indicators& i)
{
    switch(i)
    {
        case WARPING:           return "Warping"; break;
        case ASPECTRATIO:       return "Aspect ratio"; break;
        case SKEWNESS:          return "Skewness"; break;
        case TAPER:             return "Taper"; break;
        case INTERPOLATIONQUALITY: return "Interpolation quality"; break;
        case MEANRATIO:         return "Mean ratio"; break;
        case SHAPEREGULARITY:   return "Shape regularity"; break;
    }
    return "";
}