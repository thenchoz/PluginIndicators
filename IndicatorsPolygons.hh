#ifndef INDICATORS_POLYGONS_HH 
#define INDICATORS_POLYGONS_HH 

#include "Indicators.hh"

#include <vector>
#include <random>

class IndicatorsPolygons : public Indicators
{
public:
    IndicatorsPolygons(PolyMesh& _mesh):
    Indicators(), mesh_(_mesh), rd_(), mt_(rd_())
    {
        mesh_.add_property(face_warping_, "Warping");
        mesh_.add_property(face_aspect_ratio_, "Aspect ratio");
        mesh_.add_property(face_skewness_, "Skewness");
        mesh_.add_property(face_taper_, "Taper");
        mesh_.add_property(face_interpolation_quality_, "Interpolation quality");
        mesh_.add_property(face_mean_ratio_, "Mean ratio");
        mesh_.add_property(face_shape_regularity_, "Shape Regularity");
    }

    virtual ~IndicatorsPolygons()
    {
        mesh_.remove_property(face_warping_);
        mesh_.remove_property(face_aspect_ratio_);
        mesh_.remove_property(face_skewness_);
        mesh_.remove_property(face_taper_);
        mesh_.remove_property(face_interpolation_quality_);
        mesh_.remove_property(face_mean_ratio_);
        mesh_.remove_property(face_shape_regularity_);
    }

public:
    virtual Result warping() override;

    virtual Result aspect_ratio() override;

    virtual Result skewness() override;

    virtual Result taper() override;

    virtual Result interpolation_quality() override;

    virtual Result mean_ratio() override;

    virtual Result shape_regularity() override;

private:
    struct Sphere
    {
        Point center;
        double radius;
    };

    bool inside(const Sphere&, const Point&);

    Sphere from_boundary(const std::vector<Point>&);

    Sphere radius(std::vector<Point>, std::vector<Point>, bool = true);

    double radius(const PolyMesh::FaceHandle&, bool = true);

    virtual void color_coding(const OpenMesh::FPropHandleT<double>&, const double, const double) override;

private:
    PolyMesh& mesh_;

    std::random_device rd_;
    std::mt19937 mt_;
};

#endif // INDICATORS_POLYGONS_HH