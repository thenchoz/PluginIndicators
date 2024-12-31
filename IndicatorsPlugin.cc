#include "IndicatorsPlugin.hh"

#include "OpenFlipper/BasePlugin/PluginFunctions.hh"

#include "IndicatorsTriangles.hh"
#include "IndicatorsPolygons.hh"

using namespace indicatorsType;

void IndicatorsPlugin::initializePlugin()
{
  // Create the Toolbox Widget
  QWidget* toolBox = new QWidget();

  output_type_label_ = new QLabel(toolBox);
  output_min_value_label_ = new QLabel(toolBox);
  output_max_value_label_ = new QLabel(toolBox);
  output_avg_value_label_ = new QLabel(toolBox);

  QGridLayout* layout = new QGridLayout(toolBox);

  layout->addWidget(output_type_label_, 0, 1);
  layout->addWidget(output_min_value_label_, 1, 1);
  layout->addWidget(output_max_value_label_, 2, 1);
  layout->addWidget(output_avg_value_label_, 3, 1);

  QPushButton* warpingButton                = new QPushButton("&Warping", toolBox);
  QPushButton* aspectRatioButton            = new QPushButton("&Aspect ratio", toolBox);
  QPushButton* skewnessButton               = new QPushButton("&Skewness", toolBox);
  QPushButton* taperButton                  = new QPushButton("&Taper", toolBox);
  QPushButton* interpolationQuatityButton   = new QPushButton("&Interpolation quality", toolBox);
  QPushButton* meanRatioButton              = new QPushButton("&Mean ratio", toolBox);
  QPushButton* shapeRegularityButton        = new QPushButton("&Shape regularity", toolBox);

  layout->addWidget(warpingButton, 0, 0);
  layout->addWidget(aspectRatioButton, 1, 0);
  layout->addWidget(skewnessButton, 2, 0);
  layout->addWidget(taperButton, 3, 0);
  layout->addWidget(interpolationQuatityButton, 4, 0);
  layout->addWidget(meanRatioButton, 5, 0);
  layout->addWidget(shapeRegularityButton, 6, 0);

  connect(warpingButton, SIGNAL(clicked()), this, SLOT(slot_calculate_warping()));
  connect(aspectRatioButton, SIGNAL(clicked()), this, SLOT(slot_calculate_aspect_ratio()));
  connect(skewnessButton, SIGNAL(clicked()), this, SLOT(slot_calculate_skewness()));
  connect(taperButton, SIGNAL(clicked()), this, SLOT(slot_calculate_taper()));
  connect(interpolationQuatityButton, SIGNAL(clicked()), this, SLOT(slot_calculate_interpolation_quality()));
  connect(meanRatioButton, SIGNAL(clicked()), this, SLOT(slot_calculate_mean_ratio()));
  connect(shapeRegularityButton, SIGNAL(clicked()), this, SLOT(slot_calculate_shape_regularity()));

  emit addToolbox(tr("Quality indicators"), toolBox);
}

//====================================================================================================================//
void IndicatorsPlugin::slot_calculate_warping()
{
  slot_calculate(WARPING);
}

void IndicatorsPlugin::slot_calculate_aspect_ratio()
{
  slot_calculate(ASPECTRATIO);
}

void IndicatorsPlugin::slot_calculate_skewness()
{
  slot_calculate(SKEWNESS);
}

void IndicatorsPlugin::slot_calculate_taper()
{
  slot_calculate(TAPER);
}

void IndicatorsPlugin::slot_calculate_interpolation_quality()
{
  slot_calculate(INTERPOLATIONQUALITY);
}

void IndicatorsPlugin::slot_calculate_mean_ratio()
{
  slot_calculate(MEANRATIO);
}

void IndicatorsPlugin::slot_calculate_shape_regularity()
{
  slot_calculate(SHAPEREGULARITY);
}

//====================================================================================================================//
void IndicatorsPlugin::slot_calculate(indicators i)
{
  QString type = tr("Undefined");
  QString min_result = tr("Undefined");
  QString max_result = tr("Undefined");
  QString avg_result = tr("Undefined");
  Indicators *indicat = nullptr;

  for (PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS);
        o_it != PluginFunctions::objectsEnd(); ++o_it)
  {
    if (o_it->dataType(DATA_TRIANGLE_MESH)) 
    {
      TriMeshObject *tri_obj = PluginFunctions::triMeshObject(*o_it);
      TriMesh *mesh = tri_obj->mesh();

      if (mesh)
      {
        indicat = new IndicatorsTriangles(*mesh);
      }
      emit log(LOGERR, "Triangle mesh not supported for warping.");
    }
      else if (o_it->dataType(DATA_POLY_MESH))
    {
      PolyMesh *mesh = PluginFunctions::polyMesh(*o_it);

      if (mesh)
      {
        indicat = new IndicatorsPolygons(*mesh);
      }
    }
      else
    {
      emit log(LOGERR, "Data type not supported.");
    }
  }

  if (indicat != nullptr)
  {
    type = QString::fromStdString(as_s(i));
    Indicators::Result r;
    r.min = -1;

    switch (i)
    {
      case WARPING:
        r = indicat->warping();
        break;
      case ASPECTRATIO:
        r = indicat->aspect_ratio();
        break;
      case SKEWNESS:
        r = indicat->skewness();
        break;
      case TAPER:
        r = indicat->taper();
        break;
      case INTERPOLATIONQUALITY:
        r = indicat->interpolation_quality();
        break;
      case MEANRATIO:
        r = indicat->mean_ratio();
        break;
      case SHAPEREGULARITY:
        r = indicat->shape_regularity();
        break;
    }

    if (r.min >= 0)
    {
      min_result = tr("Min value: %1").arg(r.min);
      max_result = tr("Max value: %1").arg(r.max);
      avg_result = tr("Average: %1").arg(r.average);

      for (PluginFunctions::ObjectIterator o_it(PluginFunctions::TARGET_OBJECTS);
          o_it != PluginFunctions::objectsEnd(); ++o_it)
      {
        if (o_it->dataType(DATA_TRIANGLE_MESH)) 
        {
          TriMeshObject *tri_obj = PluginFunctions::triMeshObject(*o_it);
          TriMesh *mesh = tri_obj->mesh();

          if (mesh)
          {
            //set draw mode
            tri_obj->meshNode()->drawMode(ACG::SceneGraph::DrawModes::FACES
              | ACG::SceneGraph::DrawModes::SOLID_FACES_COLORED);

            emit updatedObject(tri_obj->id(), UPDATE_ALL);
          }
        }
          else if (o_it->dataType(DATA_POLY_MESH))
        {
          //set draw mode
          //o_it->meshNode()->drawMode(ACG::SceneGraph::DrawModes::FACES
          //    | ACG::SceneGraph::DrawModes::SOLID_FACES_COLORED);

          emit updatedObject(o_it->id(), UPDATE_ALL);
        }
      }
    }

    delete indicat;
  }

  output_type_label_->setText(type);
  output_min_value_label_->setText(min_result);
  output_max_value_label_->setText(max_result);
  output_avg_value_label_->setText(avg_result);
}