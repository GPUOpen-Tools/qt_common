//=============================================================================
// Copyright (c) 2024 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief  Implementation for the Driver Overrides Qt tree model.
///
//=============================================================================

#include "driver_overrides_model.h"

#include <QAbstractItemModel>
#include <QDataWidgetMapper>
#include <QFont>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QList>
#include <QModelIndex>
#include <QString>
#include <QVariant>
#include <QWidget>

namespace driver_overrides
{
    // The Driver Overrides model singleton instance.
    DriverOverridesModel* driver_overrides_model_instance_ = nullptr;

    // Driver Overrides JSON node names (for parsing imported JSON text).
    static constexpr const char* kJsonNodeNameIsDriverExperiments = "IsDriverExperiments";
    static constexpr const char* kJsonNodeNameComponents          = "Components";
    static constexpr const char* kJsonNodeNameStructures          = "Structures";
    static constexpr const char* kJsonNodeNameSettingName         = "SettingName";
    static constexpr const char* kJsonNodeNameSettingValue        = "Value";
    static constexpr const char* kJsonNodeNameSettingDescription  = "Description";
    static constexpr const char* kJsonNodeNameSettingValueType    = "ValueType";

    // Model section key names.
    static constexpr const char* kSubTreeNameAttributes    = "Attributes";
    static constexpr const char* kSubTreeNameOverridesTree = "OverridesTree";

    // Formatted text strings for model attributes that are dynamically generated.

    // The message that will be displayed in the notification banner.  The %1 argument will be replaced with the file type (e.g. "trace," "profile" or "scene"...).
    static const QString kModelAttributeValueTextFormatBannerMessage = "The %1 was captured with at least one %2 active.";

    // The title of the message box that will be displayed when the user disables a notification.  The %1 argument will be replaced with "Driver setting" or "Driver experiment."
    static const QString kModelAttributeValueTextFormatDontShowMessageBoxTitle = "Disable %1 notification";

    // The text that will be displayed in the message box when the user disables a notification.
    static const QString kModelAttributeValueTextFormatDontShowMessageBoxText =
        "Are you sure?\n\nThis notification can be restored in the General Settings pane.";

    // The title of the tree view that will be displayed in the Driver Overrides pane.  The %1 argument will be replaced with "Driver setting" or "Driver experiment" (Note: the %1s is intentional to pluralize the string).
    static const QString kModelAttributeValueTextFormatTreeViewTitle = "%1s";

    // The title of the notification setting.  The %1 argument will be replaced with the "Driver setting" or "Driver experiment" string.
    static const QString kModelAttributeValueTextFormatNotificationSettingTitle = "%1 notification";

    // The label of the notification setting.  The %1 argument will be replaced with the file type (e.g. "trace," "profile" or "scene"...),
    // and the %2 argument will be replaced with "Driver setting" or "Driver experiment."
    static const QString kModelAttributeValueTextFormatNotificationSettingLabel = "Show a notification when loading a %1 with at least one %2 enabled";

    // Static text strings.
    static constexpr const char* kDriverSettingsText    = "Driver setting";
    static constexpr const char* kDriverExperimentsText = "Driver experiment";
    static constexpr const char* kDefaultFileTypeText   = "file";

    // Boolean values as strings.
    static constexpr const char* kBoolValueTrue  = "True";
    static constexpr const char* kBoolValueFalse = "False";

    // Converts tooltip to rich text so that it properly wordwraps.
    static const QString kToolTipRichTextFormat = "<span>%1</span>";

    /// @brief Convert a JSON value to a string.
    ///
    /// @param [in] json_value  The JSON value to convert.
    /// @param [out] out_value  The converted value as a string.
    ///
    /// @return True if the conversion was successful, false otherwise.
    static bool JsonObjectToString(const QJsonValue& json_value, QString& out_value)
    {
        bool result = true;
        out_value.clear();

        if (json_value.isBool())
        {
            out_value = json_value.toBool() ? kBoolValueTrue : kBoolValueFalse;
        }
        else if (json_value.isDouble())
        {
            out_value = QString::number(json_value.toDouble());
        }
        else if (json_value.isString())
        {
            out_value = json_value.toString();
        }
        else
        {
            result = false;
        }

        return result;
    }

    // TreeItem implementation.
    TreeItem::TreeItem(const QString& key, const QVariant& value, TreeItem* parent)
        : key_(key)
        , value_(value)
        , parent_(parent)
        , is_bold_(false)
    {
    }

    TreeItem::~TreeItem()
    {
        // Delete all child items owned by this item.
        for (TreeItem* child : children_)
        {
            delete child;
        }
    }

    QString TreeItem::GetToolTip() const
    {
        return tool_tip_;
    }

    void TreeItem::SetToolTip(const QString& tool_tip)
    {
        tool_tip_ = tool_tip;
    }

    void TreeItem::SetIsBold(const bool is_bold)
    {
        is_bold_ = is_bold;
    }

    bool TreeItem::IsBold() const
    {
        return is_bold_;
    }

    void TreeItem::SetKey(const QString& key)
    {
        key_ = key;
    }

    QString TreeItem::GetKey() const
    {
        return key_;
    }

    void TreeItem::SetValue(const QVariant& value)
    {
        value_ = value;
    }

    QVariant TreeItem::GetValue() const
    {
        return value_;
    }

    void TreeItem::SetParent(TreeItem* parent)
    {
        parent_ = parent;
    }

    TreeItem* TreeItem::GetParent() const
    {
        return parent_;
    }

    void TreeItem::AddChild(TreeItem* child)
    {
        children_.append(child);
    }

    TreeItem* TreeItem::GetChild(const int index) const
    {
        if (index >= 0 && index < children_.size())
        {
            return children_.at(index);
        }
        return nullptr;
    }

    int TreeItem::GetChildCount() const
    {
        return children_.size();
    }

    int TreeItem::GetRow() const
    {
        int row = 0;
        if (parent_)
        {
            row = parent_->children_.indexOf(const_cast<TreeItem*>(this));
        }

        return row;
    }

    bool TreeItem::RemoveAllChildren(TreeItem* parent)
    {
        bool result = false;

        if (parent != nullptr)
        {
            for (TreeItem* child : parent->children_)
            {
                delete child;
            }
            parent->children_.clear();
            result = true;
        }

        return result;
    }

    DriverOverridesModel* DriverOverridesModel::GetInstance()
    {
        if (driver_overrides_model_instance_ == nullptr)
        {
            driver_overrides_model_instance_ = new DriverOverridesModel();
        }

        return driver_overrides_model_instance_;
    }

    void DriverOverridesModel::DestroyInstance()
    {
        delete driver_overrides_model_instance_;
        driver_overrides_model_instance_ = nullptr;
    }

    DriverOverridesModel::DriverOverridesModel()
    {
        // Create a new root item
        TreeItem* root_item = new TreeItem("Root", "", nullptr);
        SetRootItem(root_item);

        // Add the properties to the root branch of the tree model.
        InitializeDefaultModelAttributes();

        // Create a branch for the Driver Overrides sub-tree.
        // The Driver Overrides sub-tree will contain the imported JSON Driver settings/Driver experiments.
        AddOrUpdateChild(kSubTreeNameOverridesTree, "", RootIndex());
    }

    DriverOverridesModel::~DriverOverridesModel()
    {
        delete root_item_;
    }

    QModelIndex DriverOverridesModel::index(int row, int column, const QModelIndex& parent) const
    {
        if (!hasIndex(row, column, parent))
        {
            return QModelIndex();
        }

        TreeItem* parent_item = nullptr;

        if (!parent.isValid())
        {
            parent_item = root_item_;
        }
        else
        {
            parent_item = static_cast<TreeItem*>(parent.internalPointer());
        }

        TreeItem* child_item = parent_item->GetChild(row);
        if (child_item != nullptr)
        {
            return createIndex(row, column, child_item);
        }

        return QModelIndex();
    }

    QModelIndex DriverOverridesModel::parent(const QModelIndex& index) const
    {
        if (!index.isValid())
        {
            return QModelIndex();
        }

        TreeItem* child_item  = static_cast<TreeItem*>(index.internalPointer());
        TreeItem* parent_item = child_item->GetParent();

        if (parent_item == root_item_)
        {
            return QModelIndex();
        }

        return createIndex(parent_item->GetRow(), 0, parent_item);
    }

    int DriverOverridesModel::rowCount(const QModelIndex& parent) const
    {
        if (root_item_ == nullptr)
        {
            return 0;
        }

        TreeItem* parent_item = root_item_;

        if (parent.column() > 0)
        {
            return 0;
        }

        if (!parent.isValid())
        {
            parent_item = root_item_;
        }
        else
        {
            parent_item = static_cast<TreeItem*>(parent.internalPointer());
        }

        return parent_item->GetChildCount();
    }

    int DriverOverridesModel::columnCount(const QModelIndex& parent) const
    {
        Q_UNUSED(parent);

        // This model only has two columns: key and value.
        return kModelMaxColumnCount;
    }

    bool DriverOverridesModel::setData(const QModelIndex& index, const QVariant& value, int role)
    {
        if (index.isValid() && role == Qt::EditRole)
        {
            TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
            item->SetValue(value);
            emit dataChanged(index, index);
            return true;
        }

        return false;
    }

    QVariant DriverOverridesModel::data(const QModelIndex& index, int role) const
    {
        if (!index.isValid())
        {
            return QVariant();
        }

        const TreeItem* item = static_cast<TreeItem*>(index.internalPointer());

        if ((role == Qt::DisplayRole) || (role == Qt::EditRole))
        {
            if (index.column() == kModelKeyColumnNumber)
            {
                return item->GetKey();
            }
            else if (index.column() == kModelValueColumnNumber)
            {
                return item->GetValue();
            }
        }
        else if (role == Qt::FontRole)
        {
            if (item->IsBold())
            {
                QFont bold_font(default_item_font_);
                bold_font.setBold(true);
                return bold_font;
            }
            else
            {
                return default_item_font_;
            }
        }
        else if (role == DriverOverridesModel::custom_tooltip_role_)
        {
            // Note: A custom role is used to allow tooltips to only be displayed when the user hovers over the item text and not whitespace.
            // This is necessary because the default tooltip role behavior displays the tooltip when the user hovers over any part of the tree view item.
            // An event filter is needed to handle displaying the tooltip.  The tooltip is formatted as rich text to force word wrapping.
            return kToolTipRichTextFormat.arg(item->GetToolTip());
        }

        return QVariant();
    }

    void DriverOverridesModel::SetApplicationDetails(const QString& file_type_string, const bool is_driver_experiments)
    {
        SetModelAttributeValue(kModelAttributeNameApplicationFileTypeString, file_type_string);
        SetModelAttributeValue(kModelAttributeNameIsDriverExperiments, is_driver_experiments);
        UpdateModelAttributes();
    }

    void DriverOverridesModel::SetDefaultItemFont(const QFont& font)
    {
        default_item_font_ = font;
        default_item_font_.setBold(false);
    }

    void DriverOverridesModel::GetModelAttributes(DriverOverridesModelAttributes& out_attributes) const
    {
        out_attributes.show_notification            = GetModelAttributeValue(kModelAttributeShowNotification).toBool();
        out_attributes.enable_notifications         = GetModelAttributeValue(kModelAttributeNameEnableNotifications).toBool();
        out_attributes.driver_overrides_present     = GetModelAttributeValue(kModelAttributeNameDriverOverridesPresent).toBool();
        out_attributes.is_driver_experiments        = GetModelAttributeValue(kModelAttributeNameIsDriverExperiments).toBool();
        out_attributes.application_file_type_string = GetModelAttributeValue(kModelAttributeNameApplicationFileTypeString).toString();
        out_attributes.dont_show_message_box_text   = GetModelAttributeValue(kModelAttributeNameDontShowMessageBoxText).toString();
        out_attributes.dont_show_message_box_title  = GetModelAttributeValue(kModelAttributeNameDontShowMessageBoxTitle).toString();
        out_attributes.notification_message         = GetModelAttributeValue(kModelAttributeNameNotificationMessage).toString();
        out_attributes.notification_setting_label   = GetModelAttributeValue(kModelAttributeNameNotificationSettingLabel).toString();
        out_attributes.notification_setting_title   = GetModelAttributeValue(kModelAttributeNameNotificationSettingTitle).toString();
        out_attributes.overrides_name               = GetModelAttributeValue(kModelAttributeNameOverridesName).toString();
        out_attributes.tree_view_title              = GetModelAttributeValue(kModelAttributeNameTreeViewTitle).toString();
        out_attributes.enable_see_details_link      = GetModelAttributeValue(kModelAttributeEnableSeeDetailsLink).toBool();
    }

    QVariant DriverOverridesModel::GetModelAttributeValue(const QString& key) const
    {
        const QModelIndex& key_index = GetIndexForKey(key, GetModelAttributesSubTreeIndex());

        if (key_index.isValid())
        {
            // Use the key index to create a value index.
            const QModelIndex& value_index = index(key_index.row(), kModelValueColumnNumber, key_index.parent());
            return value_index.data();
        }

        return QVariant();
    }

    bool DriverOverridesModel::SetModelAttributeValue(const QString& key, const QVariant& value)
    {
        bool               result    = false;
        const QModelIndex& key_index = GetIndexForKey(key, GetModelAttributesSubTreeIndex());

        if (key_index.isValid())
        {
            if (key_index.isValid())
            {
                // Use the key index to create a value index.
                const QModelIndex& value_index = index(key_index.row(), kModelValueColumnNumber, key_index.parent());
                setData(value_index, value, Qt::EditRole);
                result = true;
            }
        }

        return result;
    }

    QModelIndex DriverOverridesModel::GetModelAttributesSubTreeIndex() const
    {
        return GetIndexForKey(kSubTreeNameAttributes, QModelIndex());
    }

    QModelIndex DriverOverridesModel::GetDriverOverridesSubTreeIndex() const
    {
        return GetIndexForKey(kSubTreeNameOverridesTree, QModelIndex());
    }

    bool DriverOverridesModel::BindWidgetToModelAttribute(const QString&     model_attribute_name,
                                                          QWidget*           widget,
                                                          const QString&     widget_property,
                                                          QDataWidgetMapper* mapper)
    {
        bool result = false;

        if ((widget != nullptr) && (mapper != nullptr))
        {
            const QModelIndex index = GetIndexForModelAttributeName(model_attribute_name);
            if (index.isValid())
            {
                mapper->setModel(this);
                mapper->setOrientation(Qt::Orientation::Vertical);
                mapper->setRootIndex(GetModelAttributesSubTreeIndex());
                mapper->addMapping(widget, index.row(), widget_property.toUtf8());
                mapper->setCurrentIndex(kModelValueColumnNumber);
                mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
                result = true;
            }
        }
        return result;
    }

    void DriverOverridesModel::Reset()
    {
        // Clear the flags that indicates Driver Overrides are present.
        SetModelAttributeValue(kModelAttributeNameDriverOverridesPresent, false);
        SetModelAttributeValue(kModelAttributeShowNotification, false);

        // Remove the Driver Overrides tree items.
        RemoveAllChildren(GetDriverOverridesSubTreeIndex());

        // Update the model attributes based on the current model property values.
        UpdateModelAttributes();
    }

    bool DriverOverridesModel::RemoveAllChildren(const QModelIndex& parent_index)
    {
        bool      result    = false;
        const int row_count = rowCount(parent_index);
        if (row_count > 0)
        {
            beginRemoveRows(parent_index, 0, row_count - 1);
            TreeItem* parent_tree_item = static_cast<TreeItem*>(parent_index.internalPointer());
            parent_tree_item->RemoveAllChildren(parent_tree_item);
            endRemoveRows();
            emit dataChanged(parent_index, parent_index);
            result = true;
        }

        return result;
    }

    bool DriverOverridesModel::ImportFromJsonText(const QString& json_text)
    {
        bool is_success = true;

        // Parse the JSON text.
        QJsonDocument json_doc = QJsonDocument::fromJson(json_text.toUtf8());
        QJsonObject   json_object;
        if (json_doc.isEmpty())
        {
            is_success = false;
        }
        else
        {
            json_object = json_doc.object();
            if (json_object.isEmpty())
            {
                is_success = false;
            }
        }

        const QModelIndex driver_overrides_tree_index = GetDriverOverridesSubTreeIndex();
        Q_ASSERT(driver_overrides_tree_index.isValid());

        // Remove the existing Driver Overrides tree items if they already exist.
        RemoveAllChildren(driver_overrides_tree_index);

        // Update the model with the parsed JSON data
        bool is_driver_experiments_flag = false;

        if (json_object.contains(kJsonNodeNameIsDriverExperiments))
        {
            QJsonValue is_driver_experiments = json_object.value(kJsonNodeNameIsDriverExperiments);
            if (is_driver_experiments.isBool())
            {
                is_driver_experiments_flag = is_driver_experiments.toBool();
            }
        }
        else
        {
            // If the JSON data does not contain the IsDriverExperiments flag, assume it is a Driver experiments.
            is_driver_experiments_flag = true;
        }

        SetModelAttributeValue(kModelAttributeNameIsDriverExperiments, is_driver_experiments_flag);

        // Check if Driver Experiments or Driver Settings are present in the JSON data.
        if (json_object.contains(kJsonNodeNameStructures) || json_object.contains(kJsonNodeNameComponents))
        {
            if (is_driver_experiments_flag)
            {
                is_success = ParseJsonStructureList(json_doc[kJsonNodeNameStructures].toObject(), driver_overrides_tree_index);
            }
            else
            {
                is_success = ParseJsonComponentList(json_doc[kJsonNodeNameComponents].toObject(), driver_overrides_tree_index);
            }

            SetModelAttributeValue(kModelAttributeNameDriverOverridesPresent, is_success);
        }
        else
        {
            // If the JSON data does not contain any Driver settings or Driver experiments, set the Driver Overrides present attribute to false.
            SetModelAttributeValue(kModelAttributeNameDriverOverridesPresent, false);
        }

        // Update the Model Attributes based on the JSON data parsed.
        UpdateModelAttributes();

        // Emit the data changed signal to update the entire model.
        const QModelIndex topLeft     = index(0, 0);
        const QModelIndex bottomRight = index(rowCount() - 1, columnCount() - 1);
        emit              dataChanged(topLeft, bottomRight);

        emit DriverOverridesImported();

        return is_success;
    }

    void DriverOverridesModel::InitializeDefaultModelAttributes()
    {
        // Add the attribute item root branch to the tree model.
        const QModelIndex attribute_index = AddOrUpdateChild(kSubTreeNameAttributes, "", RootIndex());

        // Add default parameters to the model.
        AddOrUpdateChild(kModelAttributeNameEnableNotifications, true, attribute_index);
        AddOrUpdateChild(kModelAttributeShowNotification, false, attribute_index);
        AddOrUpdateChild(kModelAttributeNameApplicationFileTypeString, kDefaultFileTypeText, attribute_index);
        AddOrUpdateChild(kModelAttributeNameDriverOverridesPresent, false, attribute_index);
        AddOrUpdateChild(kModelAttributeNameIsDriverExperiments, true, attribute_index);
        AddOrUpdateChild(kModelAttributeShowNotification, false, attribute_index);
        AddOrUpdateChild(kModelAttributeNameOverridesName, "", attribute_index);
        AddOrUpdateChild(kModelAttributeNameNotificationMessage, "", attribute_index);
        AddOrUpdateChild(kModelAttributeNameDontShowMessageBoxTitle, "", attribute_index);
        AddOrUpdateChild(kModelAttributeNameDontShowMessageBoxText, "", attribute_index);
        AddOrUpdateChild(kModelAttributeNameTreeViewTitle, "", attribute_index);
        AddOrUpdateChild(kModelAttributeNameNotificationSettingTitle, "", attribute_index);
        AddOrUpdateChild(kModelAttributeNameNotificationSettingLabel, "", attribute_index);
        AddOrUpdateChild(kModelAttributeEnableSeeDetailsLink, true, attribute_index);

        // Update the attributes that are dynamically generated based on other attribute values.
        UpdateModelAttributes();
    }

    bool DriverOverridesModel::ParseJsonComponentList(const QJsonObject& json_object, const QModelIndex& parent_index)
    {
        bool is_success = false;
        for (auto components_iterator = json_object.constBegin(); components_iterator != json_object.constEnd(); ++components_iterator)
        {
            QString     component_key = components_iterator.key();
            QModelIndex index         = AddOrUpdateChild(component_key, "", parent_index);
            SetItemBold(index, true);
            is_success = ParseJsonStructureList(components_iterator.value().toObject().value(kJsonNodeNameStructures).toObject(), index);

            if (!is_success)
            {
                break;
            }
        }

        return is_success;
    }

    bool DriverOverridesModel::ParseJsonStructureList(const QJsonObject& json_object, const QModelIndex& parent_index)
    {
        bool is_success = false;

        for (auto structures_iterator = json_object.constBegin(); structures_iterator != json_object.constEnd(); ++structures_iterator)
        {
            QString     structure_key = structures_iterator.key();
            QModelIndex index         = AddOrUpdateChild(structure_key, "", parent_index);
            SetItemBold(index, true);
            is_success = ParseJsonSettingList(structures_iterator.value().toArray(), index);

            if (!is_success)
            {
                break;
            }
        }

        return is_success;
    }

    bool DriverOverridesModel::ParseJsonSettingList(const QJsonArray& json_settings_array, const QModelIndex& parent_index)
    {
        bool is_success = false;

        for (auto settings_iterator = json_settings_array.constBegin(); settings_iterator != json_settings_array.constEnd(); ++settings_iterator)
        {
            is_success = ParseJsonSetting(settings_iterator->toObject(), parent_index);

            if (!is_success)
            {
                break;
            }
        }

        return is_success;
    }

    bool DriverOverridesModel::ParseJsonSetting(const QJsonObject& json_settings_object, const QModelIndex& parent_index)
    {
        bool is_success = false;
        if (!json_settings_object.isEmpty())
        {
            if (json_settings_object.contains(kJsonNodeNameSettingName) && json_settings_object.contains(kJsonNodeNameSettingValue))
            {
                QString setting_name = json_settings_object.value(kJsonNodeNameSettingName).toString();
                QString setting_value;
                is_success = JsonObjectToString(json_settings_object.value(kJsonNodeNameSettingValue), setting_value);

                if (is_success)
                {
                    // Add the setting name and value to the model.
                    QModelIndex index = AddOrUpdateChild(setting_name, setting_value, parent_index);

                    // Check for an optional setting description.
                    if (json_settings_object.contains(kJsonNodeNameSettingDescription))
                    {
                        // Use the description for the setting's tooltip.
                        QString setting_description = json_settings_object.value(kJsonNodeNameSettingDescription).toString();
                        SetItemToolTip(index, setting_description);
                    }
                }
            }
        }

        return is_success;
    }

    void DriverOverridesModel::UpdateModelAttributes()
    {
        // Update the dynamic model attributes based on the current model values.
        const bool     is_driver_experiments      = GetModelAttributeValue(kModelAttributeNameIsDriverExperiments).toBool();
        const bool     is_present                 = GetModelAttributeValue(kModelAttributeNameDriverOverridesPresent).toBool();
        const bool     is_enabled                 = GetModelAttributeValue(kModelAttributeNameEnableNotifications).toBool();
        const QString& driver_overrides_file_type = GetModelAttributeValue(kModelAttributeNameApplicationFileTypeString).toString();

        QString driver_overrides_name;
        driver_overrides_name = is_driver_experiments ? kDriverExperimentsText : kDriverSettingsText;

        SetModelAttributeValue(kModelAttributeShowNotification, is_enabled && is_present);
        SetModelAttributeValue(kModelAttributeNameOverridesName, driver_overrides_name);
        SetModelAttributeValue(kModelAttributeNameNotificationMessage,
                               kModelAttributeValueTextFormatBannerMessage.arg(driver_overrides_file_type).arg(driver_overrides_name));
        SetModelAttributeValue(kModelAttributeNameDontShowMessageBoxTitle, kModelAttributeValueTextFormatDontShowMessageBoxTitle.arg(driver_overrides_name));
        SetModelAttributeValue(kModelAttributeNameDontShowMessageBoxText, kModelAttributeValueTextFormatDontShowMessageBoxText);
        SetModelAttributeValue(kModelAttributeNameTreeViewTitle, kModelAttributeValueTextFormatTreeViewTitle.arg(driver_overrides_name));
        SetModelAttributeValue(kModelAttributeNameNotificationSettingTitle, kModelAttributeValueTextFormatNotificationSettingTitle.arg(driver_overrides_name));
        SetModelAttributeValue(kModelAttributeNameNotificationSettingLabel,
                               kModelAttributeValueTextFormatNotificationSettingLabel.arg(driver_overrides_file_type).arg(driver_overrides_name));
    }

    void DriverOverridesModel::SetItemToolTip(const QModelIndex& index, const QString& tool_tip)
    {
        if (index.isValid())
        {
            TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
            item->SetToolTip(tool_tip);
        }
    }

    void DriverOverridesModel::SetItemBold(const QModelIndex& index, const bool is_bold)
    {
        if (index.isValid())
        {
            TreeItem* item = static_cast<TreeItem*>(index.internalPointer());
            item->SetIsBold(is_bold);
        }
    }

    void DriverOverridesModel::SetRootItem(TreeItem* root_item)
    {
        root_item_ = root_item;
    }

    QModelIndex DriverOverridesModel::RootIndex() const
    {
        return createIndex(0, 0, root_item_);
    }

    QModelIndex DriverOverridesModel::GetIndexForKey(const QString& key, const QModelIndex& parent) const
    {
        QModelIndex new_index;
        TreeItem*   parent_item = nullptr;

        if (!parent.isValid())
        {
            parent_item = root_item_;
        }
        else
        {
            parent_item = static_cast<TreeItem*>(parent.internalPointer());
        }

        for (int i = 0; i < parent_item->GetChildCount(); i++)
        {
            TreeItem* child_item = parent_item->GetChild(i);
            if (child_item != nullptr && child_item->GetKey() == key)
            {
                new_index = createIndex(i, 0, child_item);
                break;
            }
        }

        return new_index;
    }

    QModelIndex DriverOverridesModel::GetIndexForModelAttributeName(const QString& attribute_name) const
    {
        return GetIndexForKey(attribute_name, GetModelAttributesSubTreeIndex());
    }

    QModelIndex DriverOverridesModel::AddOrUpdateChild(const QString& key, const QVariant& value, const QModelIndex& parent)
    {
        TreeItem* parent_item = static_cast<TreeItem*>(parent.internalPointer());
        TreeItem* item        = AddOrUpdateChildItem(key, value, parent_item);

        return createIndex(item->GetRow(), 0, item);
    }

    TreeItem* DriverOverridesModel::AddOrUpdateChildItem(const QString& key, const QVariant& value, TreeItem* parent)
    {
        TreeItem* item = nullptr;

        // Search for a child item with the same key name. If it exists, return the existing TreeItem pointer.
        for (int i = 0; i < parent->GetChildCount(); i++)
        {
            TreeItem* child_item = parent->GetChild(i);
            if (child_item != nullptr && child_item->GetKey() == key)
            {
                item = child_item;
                item->SetValue(value);
            }
        }

        // If the child item does not exist, create a new TreeItem and add it to the parent.
        if (item == nullptr)
        {
            item = new TreeItem(key, value, parent);
            parent->AddChild(item);
        }

        return item;
    }
}  // namespace driver_overrides
