#include "Indicators.hh"

double Indicators::angle(const ACG::Vec3d& d0, const ACG::Vec3d& d1) const
{
    auto d0_norm = d0.norm();
    auto d1_norm = d1.norm();
    auto denorm = d0_norm*d1_norm;

    if(denorm > std::numeric_limits<double>::min())
    {
        double cos = std::min(1.0, std::max(-1.0, (d0 | d1) / denorm));
        return std::acos(cos);
    }

    return 0;
}