//=============================================================================
/// Copyright (c) 2024-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief Definition for QLabel derived object that contains a hyperlink.
//=============================================================================

#ifndef QTCOMMON_CUSTOM_WIDGETS_SCALED_LINK_LABEL_H_
#define QTCOMMON_CUSTOM_WIDGETS_SCALED_LINK_LABEL_H_

#include "scaled_label.h"

/// Support for a QLabel that contains a hyperlink.
class ScaledLinkLabel : public ScaledLabel
{
    Q_OBJECT
    Q_PROPERTY(QColor linkColor READ GetLinkColor WRITE SetLinkColor)
    Q_PROPERTY(QColor disabledLinkColor READ GetDisabledLinkColor WRITE SetDisabledLinkColor)

public:
    /// @brief ScaledLinkLabel constructor.
    ///
    /// @param [in] parent                          The parent widget.
    explicit ScaledLinkLabel(QWidget* parent = nullptr);

    /// @brief Virtual destructor.
    virtual ~ScaledLinkLabel();

    /// @brief Get the text caption for the link.
    ///
    /// @return The link text.
    const QString& GetLinkText() const;

    /// @brief Get the URL used for link.
    ///
    /// @return The link URL.
    const QString& GetLinkUrl() const;

    /// @brief Set the link text and URL.
    ///
    /// @param [in] text                           The text to display.
    /// @param [in] url                            The URL to navigate to.
    void SetLink(const QString& text, const QString& url);

    /// @brief Get the text color for the link.
    ///
    /// @return The link text color.
    const QColor& GetLinkColor() const;

    /// @brief Set the text color for the link.
    ///
    /// @param [in] color                          The color to set.
    void SetLinkColor(const QColor& color);

    /// @brief Get the text color for the link when disabled.
    ///
    /// @return The disabled link text color.
    const QColor& GetDisabledLinkColor() const;

    /// @brief Set the text color for the link when disabled.
    ///
    /// @param [in] color                          The color to set.
    void SetDisabledLinkColor(const QColor& color);

protected:
    /// @brief Update color for the link the HTML text string.
    void UpdateLinkColor();

    /// @brief Event filter for the link label.
    ///
    /// Handles updating the link color when a label is enabled/disabled or the color style changes.
    ///
    /// @param [in] object                         The object that the event is for.
    /// @param [in] event                          The event that occurred.
    ///
    /// @return True if the event was handled, false otherwise.
    bool eventFilter(QObject* object, QEvent* event);

private:
    QString link_url_;             ///< The URL to navigate to.
    QString link_text_;            ///< The text to display.
    QColor  link_color_;           ///< The color of the link.
    QColor  disabled_link_color_;  ///< The color of the link when disabled.
};
#endif  // QTCOMMON_CUSTOM_WIDGETS_SCALED_LINK_LABEL_H_
