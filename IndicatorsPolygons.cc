#include "IndicatorsPolygons.hh"

#include <ACG/Geometry/Algorithms.hh>

bool IndicatorsPolygons::inside(const Sphere& _s, const Indicators::Point& _p)
{
    return (_s.center - _p).norm() <= _s.radius;
}

IndicatorsPolygons::Sphere IndicatorsPolygons::from_boundary(const std::vector<Indicators::Point>& _b)
{
    Sphere s;

    switch(_b.size())
    {
        case 0:
            s.radius = 0.0;
            break; 
        case 1:
            s.radius = 0.0;
            s.center = _b[0];
            break; 
        case 2:
            s.radius = (_b[0] - _b[1]).norm() / 2.0;
            s.center = (_b[0] + _b[1]) / 2.0;
            break; 
        case 3:
            s.radius = ACG::Geometry::circumRadius(_b[0], _b[1], _b[2]);
            ACG::Geometry::circumCenter(_b[0], _b[1], _b[2], s.center);
            break;
        case 4:
            s.radius = ACG::Geometry::circumRadius(_b[0], _b[1], _b[2], _b[3]);
            ACG::Geometry::circumCenter(_b[0], _b[1], _b[2], _b[3], s.center);
            break;
    }

    return s;
}

IndicatorsPolygons::Sphere IndicatorsPolygons::radius(
    std::vector<Indicators::Point> points,
    std::vector<Indicators::Point> boundary,
    bool circum
)
{
    // base on Welzl algorythm

    if (points.empty() || boundary.size() == 4)
        return from_boundary(boundary);

    // choose p
    std::uniform_int_distribution<std::mt19937::result_type> uniform(0, points.size()-1);
    size_t p = uniform(mt_);
    Point point = points[p];

    points.erase(points.begin() + p);
    Sphere s = radius(points, boundary, circum);

    if (inside(s, point) == circum)
        return s;

    boundary.push_back(point);
    return radius(points, boundary, circum);
}

double IndicatorsPolygons::radius(const PolyMesh::FaceHandle& _fh, bool circum)
{
    std::vector<Point> vertices;
    std::vector<Point> boundary;

    for (auto vh_iter = mesh_.fv_iter(_fh); vh_iter.is_valid(); ++vh_iter)
    {
        vertices.push_back(mesh_.point(*vh_iter));
    }

    Sphere cirumcircle = radius(vertices, boundary, circum);

    return cirumcircle.radius;
}

//====================================================================================================================//
Indicators::Result IndicatorsPolygons::warping()
{
    // based on equ. 16 from paper
    Indicators::Result _warping;
    _warping.min = std::numeric_limits<double>::max();
    _warping.max = 0;
    _warping.average = 0;

    size_t nb(0);

    for (auto fh: mesh_.faces())
    {
        std::vector<Point> edges;
        for (auto eh_it = mesh_.fh_iter(fh); eh_it.is_valid(); ++eh_it)
        {
            auto next = mesh_.to_vertex_handle(*eh_it);
            auto prev = mesh_.from_vertex_handle(*eh_it);

            edges.push_back(mesh_.point(next) - mesh_.point(prev));
        }

        size_t vec_size(edges.size());
        std::vector<Point> n_versor(vec_size);

        for (size_t i(0); i < vec_size; ++i)
        {
            n_versor[i] = (-edges[i] % edges[(i+1) % vec_size]).normalize();
        }

        std::vector<double> curvature;
        for (size_t i(0); i < vec_size; ++i)
        {
            for (size_t j(i+2); j < vec_size; ++j)
            {
                if ((j+1) % vec_size != i)
                {
                    auto product = n_versor[i] | n_versor[j];
                    curvature.push_back(product * product * product);
                }
            }
        }


        double w = 1.0 - (*std::min_element(std::begin(curvature), std::end(curvature)));

        mesh_.property(face_warping_, fh) = w;
        _warping.average += w;
        nb++;

        if (w > _warping.max)
            _warping.max = w;
        if (w < _warping.min)
            _warping.min = w;
    }

    _warping.average /= nb;

    color_coding(face_warping_, _warping.min, _warping.max);

    return _warping;
}

//====================================================================================================================//
Indicators::Result IndicatorsPolygons::aspect_ratio()
{
    // based on equ. 6 from paper
    Indicators::Result _aspect_ratio;
    _aspect_ratio.min = std::numeric_limits<double>::max();
    _aspect_ratio.max = 0;
    _aspect_ratio.average = 0;

    size_t nb(0);

    for (auto fh: mesh_.faces())
    {
        double ccradius = radius(fh);
        // approximate inradius
        double inradius = radius(fh, false);

        double ar(0.0);
        if(ccradius > std::numeric_limits<double>::min())
        {
            ar = inradius/ ccradius;
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
Indicators::Result IndicatorsPolygons::skewness()
{
    // based on equ. 7 from paper
    Indicators::Result _skewness;
    _skewness.min = std::numeric_limits<double>::max();
    _skewness.max = 0;
    _skewness.average = 0;

    size_t nb(0);

    for (auto fh: mesh_.faces())
    {
        double min_angle(M_PI);
        double max_angle(0);

        auto vh_iter = mesh_.fv_iter(fh);
        Point first_pt = mesh_.point(*vh_iter);
        ++vh_iter;
        Point second_pt = mesh_.point(*vh_iter);
        Point e1(first_pt), node(second_pt), e2;

        // loop around the vertices
        for (++vh_iter; vh_iter.is_valid(); ++vh_iter)
        {
            e2 = mesh_.point(*vh_iter);

            double a = angle(e1 - node, e2 - node);
            if (a < min_angle)
                min_angle = a;
            if (a > max_angle)
                max_angle = a;

            e1 = node;
            node = e2;
        }

        // close the loop
        double a = angle(e1 - node, first_pt - node);
        if (a < min_angle)
            min_angle = a;
        if (a > max_angle)
            max_angle = a;
        a = angle(node - first_pt, second_pt - first_pt);
        if (a < min_angle)
            min_angle = a;
        if (a > max_angle)
            max_angle = a;
        
        double sinMin(std::sin(min_angle));
        double sinMax(std::sin(max_angle));
        double s(sinMin / sinMax);

        mesh_.property(face_skewness_, fh) = s;
        _skewness.average += s;
        nb++;

        if (s > _skewness.max)
            _skewness.max = s;
        if (s < _skewness.min)
            _skewness.min = s;
    }

    _skewness.average /= nb;

    color_coding(face_skewness_, _skewness.min, _skewness.max);

    return _skewness;
}

//====================================================================================================================//
Indicators::Result IndicatorsPolygons::taper()
{
    Indicators::Result _taper;
    _taper.min = -1;

    return _taper;
}

//====================================================================================================================//
Indicators::Result IndicatorsPolygons::interpolation_quality()
{
    // based on equ. 9 from paper
    Indicators::Result _interpolation_quality;
    _interpolation_quality.min = -1;
    _interpolation_quality.max = 0;
    _interpolation_quality.average = 0;

/*
    size_t nb(0);

    for (auto fh: mesh_.faces())
    {
        double p1(0);
        double p2(0);
        double p3(0);
        double p4(0);

        double iq(sqrt((p1*p2 + p1*p3 + p1*p4) / 3.0));

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
*/

    return _interpolation_quality;
}

//====================================================================================================================//
Indicators::Result IndicatorsPolygons::mean_ratio()
{
    // based on equ. 11 from paper
    Indicators::Result _mean_ratio;
    _mean_ratio.min = -1;

    return _mean_ratio;
}

//====================================================================================================================//
Indicators::Result IndicatorsPolygons::shape_regularity()
{
    // based on equ. 14 from paper
    Indicators::Result _shape_regularity;
    _shape_regularity.min = -1;

    return _shape_regularity;
}

//====================================================================================================================//
void IndicatorsPolygons::color_coding(const OpenMesh::FPropHandleT<double>& _fprop, const double _min_value, const double _max_value)
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