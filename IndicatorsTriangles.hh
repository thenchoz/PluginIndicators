#ifndef INDICATORS_TRIANGLE_HH 
#define INDICATORS_TRIANGLE_HH 

#include "Indicators.hh"

#include <ObjectTypes/TriangleMesh/TriangleMesh.hh>

class IndicatorsTriangles : public Indicators
{
public:
    IndicatorsTriangles(TriMesh& _mesh):
    Indicators(), mesh_(_mesh)
    {
        mesh_.add_property(face_aspect_ratio_, "Aspect ratio");
        mesh_.add_property(face_skewness_, "Skewness");
        mesh_.add_property(face_interpolation_quality_, "Interpolation quality");
        mesh_.add_property(face_mean_ratio_, "Mean ratio");
        mesh_.add_property(face_shape_regularity_, "Shape Regularity");
    }

    virtual ~IndicatorsTriangles()
    {
        mesh_.remove_property(face_aspect_ratio_);
        mesh_.remove_property(face_skewness_);
        mesh_.remove_property(face_interpolation_quality_);
        mesh_.remove_property(face_mean_ratio_);
        mesh_.remove_property(face_shape_regularity_);
    }

private:
    struct Triangle
    {
        Point v0;
        Point v1;
        Point v2;
    };

public:
    virtual Result warping() override;

    virtual Result aspect_ratio() override;

    virtual Result skewness() override;

    virtual Result taper() override;

    virtual Result interpolation_quality() override;

    virtual Result mean_ratio() override;

    virtual Result shape_regularity() override;

private:
    Triangle get_triangle(const TriMesh::FaceHandle& _fh) const;

    virtual void color_coding(const OpenMesh::FPropHandleT<double>&, const double, const double) override;

private:
    TriMesh& mesh_;
};

#endif // INDICATORS_TRIANGLE_HH