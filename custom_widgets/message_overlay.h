//=============================================================================
// Copyright (c) 2021 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief MessageOverlay class definition
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_MESSAGE_OVERLAY_H_
#define QTCOMMON_CUSTOM_WIDGETS_MESSAGE_OVERLAY_H_

#include <memory>

#include <QDialog>
#include <QDialogButtonBox>

namespace Ui
{
    class MessageOverlay;
}

class MessageOverlay : public QDialog
{
    Q_OBJECT
public:
    /// @brief Three levels of severity are provided for messages.
    enum class Type
    {
        Question,
        Warning,
        Info,
        Error
    };

    /// @brief Constructor
    /// @param parent The parent widget
    explicit MessageOverlay(QWidget* parent = nullptr);

    /// @brief Destructor
    virtual ~MessageOverlay();

    /// @brief Sets the message overlay title.
    /// @param title The title text
    void SetTitle(const QString& title);

    /// @brief Sets the message overlay text.
    /// @param text The message text
    void SetText(const QString& text);

    /// @brief Sets the standard buttons to display.
    /// @param buttons The standard buttons
    void SetButtons(QDialogButtonBox::StandardButtons buttons);

    /// @brief Sets the default button.
    /// @param button The default button
    void SetDefaultButton(QDialogButtonBox::StandardButton button);

    /// @brief Sets the message overlay type.
    /// @param type The message overlay type
    void SetType(Type type);

    /// @brief Gets the result value.
    /// @return user chosen result value
    QDialogButtonBox::StandardButton GetResult() const;

    /// @brief Displays a critical message.
    /// @param title The message title
    /// @param text The message text
    /// @param buttons The buttons to display
    /// @param default_button The default button
    /// @return user chosen result of message overlay
    static QDialogButtonBox::StandardButton Critical(const QString&                    title,
                                                     const QString&                    text,
                                                     QDialogButtonBox::StandardButtons buttons        = QDialogButtonBox::Ok,
                                                     QDialogButtonBox::StandardButton  default_button = QDialogButtonBox::NoButton);

    /// @brief Displays a warning message.
    /// @param title The message title
    /// @param text The message text
    /// @param buttons The buttons to display
    /// @param default_button The default button
    /// @return user chosen result of message overlay
    static QDialogButtonBox::StandardButton Warning(const QString&                    title,
                                                    const QString&                    text,
                                                    QDialogButtonBox::StandardButtons buttons        = QDialogButtonBox::Ok,
                                                    QDialogButtonBox::StandardButton  default_button = QDialogButtonBox::NoButton);

    /// @brief Displays an informational message.
    /// @param title The message title
    /// @param text The message text
    /// @param buttons The buttons to display
    /// @param default_button The default button
    /// @return user chosen result of message overlay
    static QDialogButtonBox::StandardButton Info(const QString&                    title,
                                                 const QString&                    text,
                                                 QDialogButtonBox::StandardButtons buttons        = QDialogButtonBox::Ok,
                                                 QDialogButtonBox::StandardButton  default_button = QDialogButtonBox::NoButton);

    /// @brief Displays a question.
    /// @param title The message title
    /// @param text The message text
    /// @param buttons The buttons to display
    /// @param default_button The default button
    /// @return user chosen result of message overlay
    static QDialogButtonBox::StandardButton Question(const QString&                    title,
                                                     const QString&                    text,
                                                     QDialogButtonBox::StandardButtons buttons = QDialogButtonBox::StandardButtons(QDialogButtonBox::Yes |
                                                                                                                                   QDialogButtonBox::No),
                                                     QDialogButtonBox::StandardButton  default_button = QDialogButtonBox::NoButton);

private slots:
    /// @brief Handle response to dialog button clicked.
    void OnButtonClicked(QAbstractButton* button);

private:
    std::unique_ptr<Ui::MessageOverlay> ui_;      ///< Qt ui
    QDialogButtonBox::StandardButton    result_;  ///< Result
};

#endif
