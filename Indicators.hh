#ifndef INDICATORS_HH 
#define INDICATORS_HH 

#include <ACG/Utils/ColorCoder.hh>

#include <ObjectTypes/PolyMesh/PolyMesh.hh>

class Indicators
{
public:
    using Point = ACG::Vec3d;

    struct Result 
    {
        double min;
        double max;
        double average;
    };

    Indicators() {}

    virtual ~Indicators() {}

public:
    virtual Result warping() = 0;

    virtual Result aspect_ratio() = 0;

    virtual Result skewness() = 0;

    virtual Result taper() = 0;

    virtual Result interpolation_quality() = 0;

    virtual Result mean_ratio() = 0;

    virtual Result shape_regularity() = 0;

protected:
    double angle(const ACG::Vec3d&, const ACG::Vec3d&) const;

    virtual void color_coding(const OpenMesh::FPropHandleT<double>&, const double, const double) = 0;

protected:
    OpenMesh::FPropHandleT<double> face_warping_;
    OpenMesh::FPropHandleT<double> face_aspect_ratio_;
    OpenMesh::FPropHandleT<double> face_skewness_;
    OpenMesh::FPropHandleT<double> face_taper_;
    OpenMesh::FPropHandleT<double> face_interpolation_quality_;
    OpenMesh::FPropHandleT<double> face_mean_ratio_;
    OpenMesh::FPropHandleT<double> face_shape_regularity_;

protected:
    ACG::ColorCoder color_;
};

#endif // INDICATORS_HH