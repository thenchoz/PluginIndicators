#include "IndicatorsTriangles.hh"

#include <ACG/Geometry/Algorithms.hh>

IndicatorsTriangles::Triangle IndicatorsTriangles::get_triangle(const TriMesh::FaceHandle& _fh) const
{
    auto vh_iter = mesh_.fv_iter(_fh);

    Triangle tr;
    tr.v0 = mesh_.point(*vh_iter);

    ++vh_iter;
    tr.v1 = mesh_.point(*vh_iter);

    ++vh_iter;
    tr.v2 = mesh_.point(*vh_iter);

    return tr;
}

//====================================================================================================================//
Indicators::Result IndicatorsTriangles::warping()
{
    Indicators::Result _warping;
    _warping.min = -1;

    return _warping;
}

//====================================================================================================================//
Indicators::Result IndicatorsTriangles::aspect_ratio()
{
    // based on equ. 6 from paper
    Indicators::Result _aspect_ratio;
    _aspect_ratio.min = std::numeric_limits<double>::max();
    _aspect_ratio.max = 0;
    _aspect_ratio.average = 0;

    size_t nb(0);

    for (auto fh: mesh_.faces())
    {
        Triangle tr = get_triangle(fh);
        double e1 = (tr.v0 - tr.v1).norm();
        double e2 = (tr.v1 - tr.v2).norm();
        double e3 = (tr.v2 - tr.v0).norm();
        double area = ACG::Geometry::triangleArea(tr.v0, tr.v1, tr.v2);

        double semi_perimeter = (e1 + e2 + e3) / 2.0;
        double inradius = area / semi_perimeter;
        //double circumradius = e1*e2*e3 / 4 / area;
        double circumradius = ACG::Geometry::circumRadius(tr.v0, tr.v1, tr.v2);

        double ar(0.0);
        if(circumradius > std::numeric_limits<double>::min())
        {
            ar = inradius/ circumradius;
        }

        mesh_.property(face_aspect_ratio_, fh) = ar;
        _aspect_ratio.average += ar;
        nb++;

        if (ar > _aspect_ratio.max)
            _aspect_ratio.max = ar;
        if (ar < _aspect_ratio.min)
            _aspect_ratio.min = ar;
    }

    _aspect_ratio.average /= nb;

    color_coding(face_aspect_ratio_, _aspect_ratio.min, _aspect_ratio.max);

    return _aspect_ratio;
}

//====================================================================================================================//
Indicators::Result IndicatorsTriangles::skewness()
{
    // based on equ. 7 from paper
    Indicators::Result _skew;
    _skew.min = std::numeric_limits<double>::max();
    _skew.max = 0;
    _skew.average = 0;

    size_t nb(0);

    for (auto fh: mesh_.faces())
    {
        Triangle tr = get_triangle(fh);
        double a0 = angle(tr.v1 - tr.v0, tr.v2 - tr.v0);
        double a1 = angle(tr.v0 - tr.v1, tr.v2 - tr.v1);
        double a2 = angle(tr.v0 - tr.v2, tr.v1 - tr.v2);

        double sinMin(std::sin(std::min(a0, std::min(a1, a2))));
        double sinMax(std::sin(std::max(a0, std::max(a1, a2))));
        double s(sinMin / sinMax);

        mesh_.property(face_skewness_, fh) = s;
        _skew.average += s;
        nb++;

        if (s > _skew.max)
            _skew.max = s;
        if (s < _skew.min)
            _skew.min = s;
    }

    _skew.average /= nb;

    color_coding(face_skewness_, _skew.min, _skew.max);

    return _skew;
}

//====================================================================================================================//
Indicators::Result IndicatorsTriangles::taper()
{
    Indicators::Result _taper;
    _taper.min = -1;

    return _taper;
}

//====================================================================================================================//
Indicators::Result IndicatorsTriangles::interpolation_quality()
{
    // based on equ. 9 from paper
    Indicators::Result _interpolation_quality;
    _interpolation_quality.min = std::numeric_limits<double>::max();
    _interpolation_quality.max = 0;
    _interpolation_quality.average = 0;

    size_t nb(0);

    for (auto fh: mesh_.faces())
    {
        Triangle tr = get_triangle(fh);
        double e1 = (tr.v0 - tr.v1).norm();
        double e2 = (tr.v1 - tr.v2).norm();
        double e3 = (tr.v2 - tr.v0).norm();
        double area = ACG::Geometry::triangleArea(tr.v0, tr.v1, tr.v2);

        double iq(area / pow(e1 * e2 * e3, 2. / 3.));

        mesh_.property(face_interpolation_quality_, fh) = iq;
        _interpolation_quality.average += iq;
        nb++;

        if (iq > _interpolation_quality.max)
            _interpolation_quality.max = iq;
        if (iq < _interpolation_quality.min)
            _interpolation_quality.min = iq;
    }

    _interpolation_quality.average /= nb;

    color_coding(face_interpolation_quality_, _interpolation_quality.min, _interpolation_quality.max);

    return _interpolation_quality;
}

//====================================================================================================================//
Indicators::Result IndicatorsTriangles::mean_ratio()
{
    // based on equ. 11 from paper
    Indicators::Result _mean_ratio;
    _mean_ratio.min = std::numeric_limits<double>::max();
    _mean_ratio.max = 0;
    _mean_ratio.average = 0;

    size_t nb(0);

    for (auto fh: mesh_.faces())
    {
        Triangle tr = get_triangle(fh);
        double e1 = (tr.v0 - tr.v1).sqrnorm();
        double e2 = (tr.v1 - tr.v2).sqrnorm();
        double e3 = (tr.v2 - tr.v0).sqrnorm();

        double mr(3 * std::cbrt(e1*e2*e3) / (e1 + e2 + e3));

        mesh_.property(face_mean_ratio_, fh) = mr;
        _mean_ratio.average += mr;
        nb++;

        if (mr > _mean_ratio.max)
            _mean_ratio.max = mr;
        if (mr < _mean_ratio.min)
            _mean_ratio.min = mr;
    }

    _mean_ratio.average /= nb;

    color_coding(face_mean_ratio_, _mean_ratio.min, _mean_ratio.max);

    return _mean_ratio;
}

//====================================================================================================================//
Indicators::Result IndicatorsTriangles::shape_regularity()
{
    // based on equ. 14 from paper
    Indicators::Result _shape_regularity;
    _shape_regularity.min = std::numeric_limits<double>::max();
    _shape_regularity.max = 0;
    _shape_regularity.average = 0;

    size_t nb(0);

    for (auto fh: mesh_.faces())
    {
        Triangle tr = get_triangle(fh);
        double e1 = (tr.v0 - tr.v1).sqrnorm();
        double e2 = (tr.v1 - tr.v2).sqrnorm();
        double e3 = (tr.v2 - tr.v0).sqrnorm();
        double area = ACG::Geometry::triangleArea(tr.v0, tr.v1, tr.v2);

        double s(3 * area / (e1 + e2 + e3));

        mesh_.property(face_shape_regularity_, fh) = s;
        _shape_regularity.average += s;
        nb++;

        if (s > _shape_regularity.max)
            _shape_regularity.max = s;
        if (s < _shape_regularity.min)
            _shape_regularity.min = s;
    }

    _shape_regularity.average /= nb;

    color_coding(face_shape_regularity_, _shape_regularity.min, _shape_regularity.max);

    return _shape_regularity;
}

//====================================================================================================================//
void IndicatorsTriangles::color_coding(const OpenMesh::FPropHandleT<double>& _fprop, const double _min_value, const double _max_value)
{
    auto min_value = _min_value;
    auto max_value = _max_value;

    const auto range = max_value - min_value;
    color_.set_range(0, 1.0, false);

    for(auto fh : mesh_.faces()) {
        auto t = (mesh_.property(_fprop, fh) - min_value)/range;
        mesh_.set_color(fh, color_.color_float4(t));
    }
}