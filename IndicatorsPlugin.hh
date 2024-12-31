#ifndef INDICATORSPLUGIN_HH
#define INDICATORSPLUGIN_HH

#include <OpenFlipper/BasePlugin/BaseInterface.hh>
#include <OpenFlipper/BasePlugin/ToolboxInterface.hh>
#include <OpenFlipper/BasePlugin/LoggingInterface.hh>
#include <OpenFlipper/BasePlugin/LoadSaveInterface.hh>
#include <OpenFlipper/common/Types.hh>

#include <QPushButton>
#include <QLabel>
#include <QGridLayout>
#include <QSpinBox>

#include <ACG/Utils/HaltonColors.hh>
#include <ACG/Scenegraph/LineNode.hh>

#include "IndicatorsType.hh"

class IndicatorsPlugin : public QObject, BaseInterface, ToolboxInterface, LoggingInterface, LoadSaveInterface
{
  Q_OBJECT
  Q_INTERFACES(BaseInterface)
  Q_INTERFACES(ToolboxInterface)
  Q_INTERFACES(LoggingInterface)
  Q_INTERFACES(LoadSaveInterface)
  Q_PLUGIN_METADATA(IID "org.OpenFlipper.Plugins.Plugin-Indicators")

  signals:
    //BaseInterface
    void updateView();

    //LoggingInterface
    void log(Logtype _type, QString _message);
    void log(QString _message);

    // ToolboxInterface
    void addToolbox(QString _name, QWidget* _widget);

    // LoadSaveInterface
    void addEmptyObject(DataType _type, int& _id);
    void updatedObject(int _id, const UpdateType& _type);

  public:
    IndicatorsPlugin():
    output_type_label_(0), output_min_value_label_(0), output_max_value_label_(0), output_avg_value_label_(0)
    {}
    ~IndicatorsPlugin() {}

    // BaseInterface
    QString name() { return (QString("IndicatorsPlugin")); };
    QString description( ) { return (QString("Digital Geometry Processing seminar plugin")); };

   private:
    QLabel* output_type_label_;
    QLabel* output_min_value_label_;
    QLabel* output_max_value_label_;
    QLabel* output_avg_value_label_;

   private slots:
    // BaseInterface
    void initializePlugin();

    void slot_calculate(indicatorsType::indicators);

   public slots:
    void slot_calculate_warping();
    
    void slot_calculate_aspect_ratio();
    
    void slot_calculate_skewness();
    
    void slot_calculate_taper();
    
    void slot_calculate_interpolation_quality();
    
    void slot_calculate_mean_ratio();
    
    void slot_calculate_shape_regularity();

    QString version() { return QString("1.0"); };
};

#endif //INDICATORSPLUGIN_HH