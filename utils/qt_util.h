//=============================================================================
/// Copyright (c) 2017-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header for QtUtil which holds useful utility functions.
//=============================================================================

#ifndef QTCOMMON_UTILS_QT_UTIL_H_
#define QTCOMMON_UTILS_QT_UTIL_H_

#include <vector>

#include <QtGlobal>

#include <QFont>
#include <QGraphicsScene>
#include <QMessageBox>
#include <QPainter>
#include <QTableView>
#include <QTreeView>
#include <QWidget>

namespace QtCommon
{
    namespace QtUtils
    {
        /// Apply our standard look and feel to a table view
        /// \param view The table view
        void ApplyStandardTableStyle(QTreeView* view);

        /// Adjust the width of table columns to display longest header label and cell data.
        /// \param tree_view Pointer to the table view object.
        /// \param max_rows Maximum number of rows to sample when calculating column width.
        /// \param padding Additional pixels added to the calculated width of the longest string.
        /// The specified Padding value is adjusted for DPI scale setting (i.e. multiplied by scaling factor).
        /// \param max_width Maximum width (in pixels) allowed for column.  Ignored if set to zero.
        void AutoAdjustTableColumns(QTreeView* tree_view, int max_rows = 32, int padding = 0, int max_width = 0);

        /// Adjust the width of table columns to display longest header label and cell data.
        /// \param table Pointer to the table view object.
        /// \param max_rows Maximum number of rows to sample when calculating column width.
        /// \param padding Additional pixels added to the calculated width of the longest string.
        /// The specified Padding value is adjusted for DPI scale setting (i.e. multiplied by scaling factor).
        /// \param max_width Maximum width (in pixels) allowed for column.  Ignored if set to zero.
        void AutoAdjustTableColumns(QTableView* table, int max_rows = 32, int padding = 0, int max_width = 0);

        /// Fit tree columns to their contents
        /// \param tree_view The tree view
        void FitColumnsToContents(QTreeView* tree_view);

        /// Compute the minimum width required to display all columns of the given QTreeView.
        /// \param tree_view Pointer to the tree view object.
        /// \param max_rows Maximum number of rows to sample when calculating column width.
        /// \param padding Additional pixels added to the calculated width of the longest string.
        /// \returns The minimum width required to display all columns of the given QTreeView.
        int ComputeMinimumTableWidth(QTreeView* tree_view, int max_rows = 32, int padding = 0);

        /// Compute the file size acronym for the given byte count.
        /// \param byte_count The number of bytes to compute the size acronym for.
        /// \param size_string A string with the file size acronym.
        /// \param use_round If true, round the value to the nearest integer.
        void GetFilesizeAcronymFromByteCount(quint64 byte_count, QString& size_string, bool use_round = false);

        /// Calculate the pixel width of a string
        /// \param font The font to measure
        /// \param str The string to measure
        /// \return The string width
        int GetTextWidth(const QFont& font, const QString& str);

        /// Calculate the pixel width of a string
        /// \param painter ptr to painter
        /// \param str The string to measure
        /// \return the string width
        int GetPainterTextWidth(QPainter* painter, const QString& str);

        /// Invalidate font metrics by toggling the font size.
        /// This helps to resolve DPI scaling issues while also being able to rely
        /// on Qt's default functionality.
        /// \param widget The widget that needs invalidating
        void InvalidateFontMetrics(QWidget* widget);

        /// Invalidate font metrics by toggling the font size.
        /// This helps to resolve DPI scaling issues while also being able to rely
        /// on Qt's default functionality.
        /// \param scene The scene that needs invalidating
        void InvalidateFontMetrics(QGraphicsScene* scene);

        /// Verify that at least two check boxes are checked, not counting the
        /// first one ("All").
        /// \param visibility_vector The vetor to look through
        /// \param first_column The first column index
        /// \param last_column The last column index
        /// \return The boolean to indicate whether to allow unchecking of this check box
        bool VerifyOneCheckboxChecked(const std::vector<bool>& visibility_vector, int first_column, int last_column);

        /// Utility function to convert a clock to a time unit and output as string.
        /// \param clk input clock to convert
        /// \param unit_type unit type (clk, ns, us, ms)
        /// \return a string representing a clock value
        QString ClockToTimeUnit(double clk, int unit_type);

        /// Utility function to convert a uint64 value to capitalized and aligned str
        /// \param value 64-bit value
        /// \return a string representation of the value
        QString HashToStr(quint64 value);

        /// \brief Open up file explorer and select a file
        /// \param path file path to select
        /// \return If the path is valid, true is returned.
        /// otherwise false is returned.
        bool BrowseToFile(const QString& path);

        /// \brief Helper function to show a message box
        /// \param parent message box parent
        /// \param buttons message box button flags
        /// \param icon message box icon type
        /// \param title message box title text
        /// \param message message box content
        /// \return message box exec result
        int ShowMessageBox(QWidget* parent, QMessageBox::StandardButtons buttons, QMessageBox::Icon icon, const QString& title, const QString& message);

    }  // namespace QtUtils
}  // namespace QtCommon

#endif  // QTCOMMON_UTILS_QT_UTIL_H_
