//=============================================================================
// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief  Header file for the Driver Overrides Qt tree model.
///
/// A Qt tree model that is used to display JSON data from the Driver Overrides RDF chunck.
/// The model is divided into two main sections: Model Attributes and a Driver Overrides Tree.
/// The Driver Overrides Tree section contains the JSON data from the Driver Overrides RDF chunck.
/// The Attributes section contains the Driver Override properties.  Some of which are updated based
/// on the JSON data.  The model attributes are mapped to the UI widgets using a QDataWidgetMapper and
/// determine the behavior of the UI (e.g., whether to show a notification when Driver Overrides are
/// present and what message to display).  The data in the model can be accessed using the standard
/// QAbstractItemModel interface. In addition, attributes can be set and retrieved using the
/// SetModelAttributeValue() and GetModelAttributeValue() methods.  The model can be updated with JSON
/// data from the RDF DriverOverrides chunk using the ImportFromJsonText() method.
///
/// Note: When displaying the Driver Overrides tree with a QTreeView based widget,
/// The view root node should be set to the index returned by GetDriverOverridesSubTreeIndex().
/// Otherwise, the tree will display the model attributes as well as the Driver Overrides tree.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_DRIVER_OVERRIDES_MODEL_H_
#define QTCOMMON_CUSTOM_WIDGETS_DRIVER_OVERRIDES_MODEL_H_

#include <QAbstractItemModel>
#include <QDataWidgetMapper>
#include <QFont>
#include <QJsonObject>
#include <QList>
#include <QModelIndex>
#include <QString>
#include <QVariant>
#include <QWidget>

namespace driver_overrides
{
    // The column numbers for the model.
    static constexpr const int kModelKeyColumnNumber   = 0;
    static constexpr const int kModelValueColumnNumber = 1;
    static constexpr const int kModelMaxColumnCount    = 2;

    // Model attribute names.  Used to retrieve or set the value of a specific attribute in the model.
    static constexpr const char* kModelAttributeNameApplicationFileTypeString = "ApplicationFileTypeString";
    static constexpr const char* kModelAttributeNameOverridesName             = "OverridesName";
    static constexpr const char* kModelAttributeNameNotificationMessage       = "NotificationMessage";
    static constexpr const char* kModelAttributeNameDontShowMessageBoxTitle   = "DontShowMessageBoxTitle";
    static constexpr const char* kModelAttributeNameDontShowMessageBoxText    = "DontShowMessageBoxText";
    static constexpr const char* kModelAttributeNameTreeViewTitle             = "TreeViewTitle";
    static constexpr const char* kModelAttributeNameNotificationSettingTitle  = "NotificationSettingTitle";
    static constexpr const char* kModelAttributeNameNotificationSettingLabel  = "NotificationSettingLabel";
    static constexpr const char* kModelAttributeShowNotification              = "ShowNotification";
    static constexpr const char* kModelAttributeNameEnableNotifications       = "EnableNotifications";
    static constexpr const char* kModelAttributeNameDriverOverridesPresent    = "DriverOverridesPresent";
    static constexpr const char* kModelAttributeNameIsDriverExperiments       = "IsDriverExperiments";
    static constexpr const char* kModelAttributeEnableSeeDetailsLink          = "EnableSeeDetailsLink";

    typedef struct DriverOverridesModelAttributes
    {
        QString application_file_type_string;  ///< The name of the file type that the application loads.
        QString overrides_name;                ///< The name of the driver overrides (either "driver setting" or "driver experiment").
        QString notification_message;          ///< The message to display in the notification if driver overrides are present in the loaded file.
        QString dont_show_message_box_title;   ///< The title of the message box that asks the user if they want to show the notification again.
        QString dont_show_message_box_text;    ///< The text of the message box that asks the user if they want to show the notification again.
        QString tree_view_title;               ///< The title of the tree view ("Driver settings" or "Driver experiments").
        QString notification_setting_title;    ///< The title above the show notification setting checkbox.
        QString notification_setting_label;    ///< The label for the show notification setting checkbox.
        bool    show_notification;             ///< True if the driver override notification should be shown, false otherwise when a file is loaded.
        bool    enable_notifications;          ///< True if the driver override notification is enabled, false otherwise.
        bool    driver_overrides_present;      ///< True if driver overrides are present in the loaded file, false otherwise.
        bool    is_driver_experiments;         ///< True if the loaded file contains driver experiments, false otherwise.
        bool    enable_see_details_link;       ///< True if the "See Details" link should be enabled, false otherwise.
    } DriverOverridesModelAttributes;

    /// @brief A class that represent a tree item in the model.
    class TreeItem
    {
    public:
        /// @brief Constructor.
        ///
        /// @param [in] key                             The key name for the item.
        /// @param [in] value                           The value for the item.
        /// @param [in] parent                          The parent of this item.
        TreeItem(const QString& key, const QVariant& value, TreeItem* parent = nullptr);

        /// @brief Destructor.
        ~TreeItem();

        /// @brief Set the tool tip for the item.
        ///
        /// @param [in] tool_tip                        The tool tip string.
        void SetToolTip(const QString& tool_tip);

        /// @brief Get tool tip for the item.
        ///
        /// @return The tool tip string.
        QString GetToolTip() const;

        /// @brief Set the bold flag of the font used for this item.
        ///
        /// @param [in] is_bold                         True if the font should be bold, false otherwise.
        void SetIsBold(const bool is_bold);

        /// @brief Check if the item is bold.
        ///
        /// @return True if the item is bold, false otherwise.
        bool IsBold() const;

        /// @brief Get the key name for the item.
        ///
        /// @return The key name.
        QString GetKey() const;

        /// @brief Get the value for the item.
        ///
        /// @return The value.
        QVariant GetValue() const;

        /// @brief Get the parent of the item.
        ///
        /// @return The parent.
        TreeItem* GetParent() const;

        /// @brief Get the child at the specified index.
        ///
        /// @param [in] index                           The row index of the child.
        ///
        /// @return The child item.
        TreeItem* GetChild(const int index) const;

        /// @brief Get the number of children for this item.
        ///
        /// @return The number of children.
        int GetChildCount() const;

        /// @brief Get the row number of this item.
        ///
        /// @return The row number.
        int GetRow() const;

        /// @brief Set the key name for the item.
        ///
        /// @param [in] key                             The key name.
        void SetKey(const QString& key);

        /// @brief Set the value for the item.
        ///
        /// @param [in] value                           The value.
        void SetValue(const QVariant& value);

        /// @brief Set the parent of the item.
        ///
        /// @param [in] parent                          The parent item.
        void SetParent(TreeItem* parent);

        /// @brief Add a child to this item.
        ///
        /// @param [in] child                           The child item.
        void AddChild(TreeItem* child);

        /// @brief Remove all children from this item.
        ///
        /// @param [in] parent                           The parent of the item.
        ///
        /// @return True if children were removed, false otherwise.
        bool RemoveAllChildren(TreeItem* parent);

    private:
        QString          key_;       ///< The key name for this item.
        QVariant         value_;     ///< The value for this item.
        QString          tool_tip_;  ///< The tool tip for this item.
        TreeItem*        parent_;    ///< The parent of this item.
        QList<TreeItem*> children_;  ///< A list of the children for this item.
        bool             is_bold_;   ///< Flag to indicate if the item should be bold.
    };

    /// @brief A model that translates Driver Overrides JSON text into data viewable by a QTreeView.
    class DriverOverridesModel : public QAbstractItemModel
    {
        Q_OBJECT

    public:
        /// @brief Retrieve the singleton instance of the model.
        ///
        /// @return A pointer to the singleton instance of the model.
        static DriverOverridesModel* GetInstance();

        /// @brief Destroy the singleton instance of the model.
        static void DestroyInstance();

        /// @brief Remove the Driver Overrides sub-tree and resets model attributes.
        void Reset();

        /// @brief Imports the Driver Overrides JSON data into the model.
        ///
        /// @param [in] json_text                  The JSON data to import into the model.
        ///
        /// @return True if the JSON data was successfully imported, false otherwise.
        bool ImportFromJsonText(const QString& json_text);

        /// @brief Retrieves the Model Attributes and copies them to a structure.
        ///
        /// @param [out] out_attributes             The structure to copy the model attributes to.
        void GetModelAttributes(DriverOverridesModelAttributes& out_attributes) const;

        /// @brief Set the default font for the items in the model (retrieved from the Data() method with Font role).
        ///
        /// @param [in] font                            The font to use.
        void SetDefaultItemFont(const QFont& font);

        /// @brief Retrieves the index of the Model Attributes sub-tree.
        ///
        /// @return The index of the Model Attributes sub-tree.
        QModelIndex GetModelAttributesSubTreeIndex() const;

        /// @brief Retrieves the index of the Driver Overrides sub-tree.
        ///
        /// @return The index of the Driver Overrides sub tree.
        QModelIndex GetDriverOverridesSubTreeIndex() const;

        /// @brief Maps a widget to an attribute in the model.
        ///
        /// @param [in] key                         The name of the attribute in the model.
        /// @param [in] widget                      The widget to bind to the attribute.
        /// @param [in] widget_property             The property of the widget to bind to.
        /// @param [in] mapper                      The data widget mapper to use.
        ///
        /// @return True if the widget was successfully bound to a Model attribute , false otherwise.
        bool BindWidgetToModelAttribute(const QString& key, QWidget* widget, const QString& widget_property, QDataWidgetMapper* mapper);

        /// @brief Sets the value of the Model Attribute with the given name.
        ///
        /// @param [in] key                        The name of the Model Attribute to set the value for.
        /// @param [in] value                      The value to set the attribute to.
        ///
        /// @return True if the attribute was successfully set, false otherwise.
        bool SetModelAttributeValue(const QString& key, const QVariant& value);

        /// @brief Retrieves the value of a Model Attribute with the given name.
        ///
        /// @param [in] key                         The name of the attribute to retrieve the value for.
        ///
        /// @return The value of the attribute with the given name.  The return value is marked invalid if the attribute name doesn't exist.
        QVariant GetModelAttributeValue(const QString& key) const;

        /// @brief Sets details about the host application.
        ///
        /// The application file type string is the name of the file type that the application loads.
        /// It is used to customize Model Attributes which contain the application file type string.
        ///
        /// @param [in] file_type_string            The name of the file type that the application loads.
        /// @param [in] is_driver_experiments       True if the application defaults to using Driver Experiments, false otherwise.
        void SetApplicationDetails(const QString& file_type_string, const bool is_driver_experiments = true);

        /// @brief Get an index from the model.
        ///
        /// @param [in] row                             The row of the parent in the model.
        /// @param [in] column                          The column of the parent in the model.
        /// @param [in] parent                          The parent index.
        ///
        /// @return The model index.
        QModelIndex index(int row, int column, const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

        /// @brief Get the parent index for the model.
        ///
        /// @param [in] index                           The model index to retrieve the parent for.
        ///
        /// @return The parent index.
        QModelIndex parent(const QModelIndex& index) const Q_DECL_OVERRIDE;

        /// @brief Get the row count for the model.
        ///
        /// @param [in] parent                          The parent index of the row count to retrieve.
        ///
        /// @return The row count.
        int rowCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

        /// @brief Return the column count for the model.
        ///
        /// @param [in] parent                          The parent index.
        ///
        /// @return The column count.
        int columnCount(const QModelIndex& parent = QModelIndex()) const Q_DECL_OVERRIDE;

        /// @brief Get data from the model.
        ///
        /// @param [in] index                           The model index.
        /// @param [in] role                            The role.
        ///
        /// @return The data.
        QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const Q_DECL_OVERRIDE;

        /// @brief Set the data for the model.
        ///
        /// @param [in] index                           The model index.
        /// @param [in] value                           The value.
        /// @param [in] role                            The role.
        ///
        /// @return True if the data was set, false otherwise.
        bool setData(const QModelIndex& index, const QVariant& value, int role) Q_DECL_OVERRIDE;

        static const int custom_tooltip_role_ = Qt::UserRole + 1;  ///< Custom role for the tool tip.

    signals:
        /// @brief Signal emitted when the driver overrides have been imported.
        void DriverOverridesImported();

    private:
        /// @brief Constructor (made private since this is a singleton).
        DriverOverridesModel();

        /// @brief Move Constructor (deleted since this is a singleton).
        DriverOverridesModel(const DriverOverridesModel&&) = delete;

        /// @brief Destructor (made private since this is a singleton).
        ~DriverOverridesModel();

        /// @brief Copy constructor (deleted since this is a singleton).
        DriverOverridesModel(const DriverOverridesModel&) = delete;

        /// @brief Assignment operator (deleted since this is a singleton).
        ///
        /// @return A reference to the assigned object.
        DriverOverridesModel& operator=(const DriverOverridesModel&) = delete;

        /// @brief Move Assignment operator (deleted since this is a singleton).
        ///
        /// @return A reference to the assigned object.
        DriverOverridesModel& operator=(const DriverOverridesModel&&) = delete;

        /// @brief Initializes the model with default attributes.
        void InitializeDefaultModelAttributes();

        /// @brief Set the root item for the model.
        ///
        /// @param [in] root_item                       The root item.
        void SetRootItem(TreeItem* root_item);

        /// @brief Retrieve the root index of the model.
        ///
        /// @return The root index.
        QModelIndex RootIndex() const;

        /// @brief Updates Model Attributes based on the Driver Overrides JSON data.
        void UpdateModelAttributes();

        /// @brief Retrieves the index of the Model Attribute with the given name.
        ///
        /// @param [in] attribute_name               The name of the attribute to retrieve the index for.
        ///
        /// @return The index of the attribute with the given name or invalid index if the attribute doesn't exist.
        QModelIndex GetIndexForModelAttributeName(const QString& attribute_name) const;

        /// @brief Get the model index for the specified key.
        ///
        /// @param [in] key                             The key name.
        /// @param [in] parent                          The parent index.
        ///
        /// @return The model index.
        QModelIndex GetIndexForKey(const QString& key, const QModelIndex& parent) const;

        /// @brief Add a child to the model.
        ///
        /// If a child item with the same key already exists, the value of the existing item will be updated.
        ///
        /// @param [in] key                             The key name of the item.
        /// @param [in] value                           The value of the item.
        /// @param [in] parent                          The parent index that the child item is being added to.
        ///
        /// @return The model index of the child.
        QModelIndex AddOrUpdateChild(const QString& key, const QVariant& value, const QModelIndex& parent);

        /// @brief Add a child item to the model.
        ///
        /// If a child item with the same key already exists, the value of the existing item will be updated.
        ///
        /// @param [in] name                            The key name of the item.
        /// @param [in] value                           The value of the item.
        /// @param [in] parent                          The parent item.
        ///
        /// @return The new child item.
        TreeItem* AddOrUpdateChildItem(const QString& name, const QVariant& value, TreeItem* parent);

        /// @brief Set tool tip for an item in the model.
        ///
        /// @param [in] index                           The model index.
        /// @param [in] tool_tip                        The tool tip string.
        void SetItemToolTip(const QModelIndex& index, const QString& tool_tip);

        /// @brief Set the bold flag of the font used for an item in the model.
        ///
        /// @param [in] index                           The model index.
        /// @param [in] is_bold                         True if the font should be bold, false otherwise.
        void SetItemBold(const QModelIndex& index, const bool is_bold);

        /// @brief Remove all children of the parent from the model.
        ///
        /// @param [in] parent                          The parent index.
        ///
        /// @return True if children were removed, false otherwise.
        bool RemoveAllChildren(const QModelIndex& parent);

        /// @brief Parse the JSON Component list and add it to the model.
        ///
        /// @param [in] json_object                     The JSON object to parse.
        /// @param [in] parent_index                    The parent index.
        ///
        /// @return True if the JSON Component list was successfully parsed and added to the model, false otherwise.
        bool ParseJsonComponentList(const QJsonObject& json_object, const QModelIndex& parent_index);

        /// @brief Parse the JSON Structure list and add it to the model.
        ///
        /// @param [in] json_object                     The JSON object to parse.
        /// @param [in] parent_index                    The parent index.
        ///
        /// @return True if the JSON Structure list was successfully parsed and added to the model, false otherwise.
        bool ParseJsonStructureList(const QJsonObject& json_object, const QModelIndex& parent_index);

        /// @brief Parse the JSON Setting list and add it to the model.
        ///
        /// @param [in] json_settings_array             The JSON array of settings to parse.
        /// @param [in] parent_index                    The parent index.
        ///
        /// @return True if the JSON Setting list was successfully parsed and added to the model, false otherwise.
        bool ParseJsonSettingList(const QJsonArray& json_settings_array, const QModelIndex& parent_index);

        /// @brief Parse the JSON Setting and add it to the model.
        ///
        /// @param [in] json_settings_object            The JSON object of the setting to parse.
        /// @param [in] parent_index                    The parent index.
        ///
        /// @return True if the JSON Setting was successfully parsed and added to the model, false otherwise.
        bool ParseJsonSetting(const QJsonObject& json_settings_object, const QModelIndex& parent_index);

    private:
        TreeItem* root_item_;          ///< The root item of the model.
        QFont     default_item_font_;  ///< The default font for the items in the model.
    };
}  // namespace driver_overrides
#endif  // QTCOMMON_CUSTOM_WIDGETS_DRIVER_OVERRIDES_MODEL_H_
