//=============================================================================
/// Copyright (c) 2016-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  ModelViewMapper class implementation.
//=============================================================================

#include "model_view_mapper.h"

#include <cassert>

#include <QStandardItemModel>
#include <QDataWidgetMapper>
#include <QWidget>

/// A widget property name string. Used to retrieve the mapped widget Id if given a registered QWidget.
static const char* kObjectIdProperty = "WidgetObjectId";

ModelViewMapper::ModelViewMapper(uint32_t model_count)
    : control_model_array_(nullptr)
    , control_mapper_array_(nullptr)
    , model_count_(model_count)
{
    if (model_count > 0)
    {
        control_model_array_  = new QStandardItemModel*[model_count];
        control_mapper_array_ = new QDataWidgetMapper*[model_count];

        for (uint32_t loop = 0; loop < model_count_; loop++)
        {
            control_model_array_[loop] = nullptr;
            control_mapper_array_[loop] = nullptr;
        }
    }
}

ModelViewMapper::~ModelViewMapper()
{
    for (uint32_t loop = 0; loop < model_count_; loop++)
    {
        delete control_model_array_[loop];
        delete control_mapper_array_[loop];
    }

    if (model_count_ > 0)
    {
        delete[] control_model_array_;
        delete[] control_mapper_array_;
    }
}

void ModelViewMapper::SetModelData(int id, const QVariant& data)
{
    Q_ASSERT(control_model_array_ != nullptr);
    Q_ASSERT(id >= 0);
    Q_ASSERT((uint32_t)id < model_count_);

    if (control_model_array_ != nullptr && (id >= 0) && ((uint32_t)id < model_count_))
    {
        control_model_array_[id]->setData(control_model_array_[id]->index(0, 0), data);
    }
}

int ModelViewMapper::GetMappedWidgetId(const QWidget* widget) const
{
    Q_ASSERT(widget != nullptr);

    // Default to -1 since this Id is also used as an index.
    // If the widget does not have the supplied property for some unexpected
    // reason, the default QVariant will convert to the int 0, which might
    // appear correct, but result in some unexpected behavior. By changing the
    // default return to -1, hopefully the error will be more obvious.
    int object_id_int = -1;

    if (widget != nullptr)
    {
        const QVariant& object_id = widget->property(kObjectIdProperty);
        Q_ASSERT(object_id.isValid());
        if (!object_id.isValid())
        {
            bool is_ok   = false;
            int  tmpInt = object_id.toInt(&is_ok);
            Q_ASSERT(is_ok);
            if (is_ok)
            {
                object_id_int = tmpInt;
            }
        }
    }

    return object_id_int;
}

QWidget* ModelViewMapper::GetMappedWidget(int id)
{
    QWidget* widget = nullptr;

    if (control_mapper_array_ != nullptr && (id >= 0) && ((uint32_t)id < model_count_))
    {
        widget = control_mapper_array_[id]->mappedWidgetAt(0);
    }

    return widget;
}

uint32_t ModelViewMapper::GetModelCount() const
{
    return model_count_;
}

void ModelViewMapper::InitializeModel(QWidget* widget, uint32_t id, const QString& property_name)
{
    Q_ASSERT(widget != nullptr);
    Q_ASSERT(control_model_array_ != nullptr);
    Q_ASSERT(control_mapper_array_ != nullptr);

    if (widget != nullptr && control_model_array_ != nullptr && control_mapper_array_ != nullptr)
    {
        // Store the Id in the widget properties.
        widget->setProperty(kObjectIdProperty, id);

        control_model_array_[id] = new QStandardItemModel(1, 1);
        QString default_value = "{0}";
        if (property_name.compare("styleSheet") == 0)
        {
            default_value = "";
        }
        QStandardItem* item = new QStandardItem(default_value);
        control_model_array_[id]->setItem(0, 0, item);

        // Add a mapping between a QLabel and a section from the model by using QDataWidgetMapper.
        control_mapper_array_[id] = new QDataWidgetMapper();
        control_mapper_array_[id]->setModel(control_model_array_[id]);
        control_mapper_array_[id]->addMapping(widget, 0, property_name.toUtf8());
        control_mapper_array_[id]->toFirst();
    }
}
