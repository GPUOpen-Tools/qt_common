//=============================================================================
/// Copyright (c) 2016-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  ModelViewMapper class declaration.
//=============================================================================

#ifndef QTCOMMON_UTILS_MODEL_VIEW_MAPPER_H_
#define QTCOMMON_UTILS_MODEL_VIEW_MAPPER_H_

#include <memory>
#include <QObject>

class QStandardItemModel;
class QDataWidgetMapper;

class ModelViewMapper : public QObject
{
    Q_OBJECT

public:
    /// Explicit constructor
    /// \param model_count the number of models required. Each model is referenced
    ///        by an ID
    explicit ModelViewMapper(uint32_t model_count);

    /// Destructor
    virtual ~ModelViewMapper();

    /// Retrieve the widget id mapped to the given widget.
    /// \param widget The widget to retrieve the model id for.
    /// \returns The model Id registered to the given widget.
    int GetMappedWidgetId(const QWidget* widget) const;

    /// Get the number of values mapped to this model.
    /// \returns The number of values mapped to this model.
    uint32_t GetModelCount() const;

    /// Initialize a model corresponding to a IO control property. Allows model/view
    /// to work on any UI component
    /// \param widget the UI widget to use.
    /// \param index the ID of the model so it can be referenced internally.
    /// \param property_name the name of the widget property that the model will
    ///        be mapped to.
    void InitializeModel(QWidget* widget, uint32_t id, const QString& property_name);

protected:
    /// Set the data for the given model.
    /// \param id The model id to set data for.
    /// \param data The data to associate with the id.
    void SetModelData(int id, const QVariant& data);

    /// Set the data for the given model, including a tooltip.
    /// \param id The model id to set data for.
    /// \param data The data to associate with the id.
    /// \param tool_tip The tool tip string to associate with the id.
    void SetModelData(int id, const QVariant& data, const QString& tool_tip);

    /// Retrieve the widget mapped to the given widget id.
    /// \param id The widget id to retrieve the widget for.
    /// \returns A pointer to the given widget id.
    QWidget* GetMappedWidget(int id);

    QStandardItemModel** control_model_array_;   ///< Model associated with Index count control
    QDataWidgetMapper**  control_mapper_array_;  ///< The mapper to map a control to a model
    uint32_t             model_count_;           ///< The number of models needed, 1 per widget property
};

#endif  // QTCOMMON_UTILS_MODEL_VIEW_MAPPER_H_
