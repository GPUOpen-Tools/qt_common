//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header for QtUtil which holds useful utility functions.
//=============================================================================

#include <cmath>

#include <QApplication>
#include <QFont>
#include <QTreeView>
#include <QHeaderView>
#include <QPainter>
#include <QTextStream>
#include <QTableView>

#include <../CustomWidgets/ArrowIconWidget.h>
#include <../CustomWidgets/ListWidget.h>
#include <../Util/QtUtil.h>
#include <../Util/CommonDefinitions.h>

#ifdef _WIN32
    #pragma warning(disable: 4714)  // prevent warning caused by QString::trimmed()
#endif // _WIN32

namespace QtCommon
{
    //-----------------------------------------------------------------------------
    /// Apply our standard look and feel to a table view
    /// \param pView The table view
    //-----------------------------------------------------------------------------
    void QtUtil::ApplyStandardTableStyle(QTreeView* pView)
    {
        if (pView != nullptr)
        {
            QFont font = pView->font();
            font.setBold(true);
            pView->header()->setFont(font);

            font.setBold(false);
            pView->setFont(font);

            pView->setRootIsDecorated(false);
            pView->setItemsExpandable(false);
            pView->setExpandsOnDoubleClick(false);

            pView->header()->setDefaultAlignment(Qt::AlignLeft);
            pView->header()->setSectionsClickable(false);
            pView->header()->setStretchLastSection(true);
            pView->setFrameStyle(QFrame::NoFrame);
            pView->setSelectionMode(QAbstractItemView::SingleSelection);
            pView->setEditTriggers(QAbstractItemView::NoEditTriggers);
            pView->setSelectionBehavior(QAbstractItemView::SelectRows);
            pView->setWordWrap(false);
            pView->setAlternatingRowColors(true);

            pView->setStyleSheet(pView->styleSheet() +
                "QTreeView{paint-alternating-row-colors-for-empty-area: 1; }"
            );
        }
    }

    //-----------------------------------------------------------------------------
    /// Adjust the width of table columns to display longest header label and cell data.
    /// \param pTable Pointer to the table view object.
    /// \param maxRows Maximum number of rows to sample when calculating column width.
    /// \param padding Additional pixels added to the calculated width of the longest string.
    /// The specified Padding value is adjusted for DPI scale setting (i.e. multiplied by scaling factor).
    /// \param maxWidth Maximum width (in pixels) allowed for column.  Ignored if set to zero.
    //-----------------------------------------------------------------------------
    void QtUtil::AutoAdjustTableColumns(QTreeView* pTable, int maxRows, int padding, int maxWidth)
    {
        Q_ASSERT(pTable);
        QHeaderView* pHeader = pTable->header();
        if (pHeader != nullptr)
        {
            pTable->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
            const int columnCount = pTable->header()->count();
            QVector<int> columnWidth(columnCount);

            QAbstractItemModel* pModel = (QAbstractItemModel*)pTable->model();
            if (pModel != nullptr)
            {
                int margin = 0;
                QStyle* pStyle = pHeader->style();
                if (pStyle != nullptr)
                {
                    margin = pHeader->style()->pixelMetric(QStyle::PM_HeaderMargin, 0, pHeader);
                }

                const int rowCount = pModel->rowCount();
                const int elideWidth = QtUtil::GetTextWidth(pTable->font(), QString(0x2026));
                QFontMetrics fm(pHeader->font());
                pHeader->setFixedHeight(fm.height() + (margin * 2));

                for (int column = 0; column < columnCount; column++)
                {
                    const QVariant headerData = pModel->headerData(column, Qt::Horizontal);
                    const QString textHeader = headerData.toString().trimmed();
                    columnWidth[column] = QtUtil::GetTextWidth(pHeader->font(), textHeader) + padding + (margin * 2);
                    for (int row = 0; (row < rowCount) && (row < maxRows); row++)
                    {
                        const QString textData = pModel->data(pModel->index(row, column)).toString().trimmed();
                        int width = QtUtil::GetTextWidth(pTable->font(), textData) + elideWidth;
                        if (width > columnWidth[column])
                        {
                            columnWidth[column] = width;
                        }
                    }
                }

                pHeader->setSectionResizeMode(QHeaderView::Interactive);
                for (int column = 0; column < columnCount; column++)
                {
                    if ((maxWidth == 0) || ((columnWidth[column] + padding) < maxWidth))
                    {
                        pTable->setColumnWidth(column, columnWidth[column] + padding);
                    }
                    else
                    {
                        pTable->setColumnWidth(column, maxWidth);
                    }
                }
            }
        }
    }

    //-----------------------------------------------------------------------------
    /// Adjust the width of table columns to display longest header label and cell data.
    /// \param pTable Pointer to the table view object.
    /// \param maxRows Maximum number of rows to sample when calculating column width.
    /// \param padding Additional pixels added to the calculated width of the longest string.
    /// The specified Padding value is adjusted for DPI scale setting (i.e. multiplied by scaling factor).
    /// \param maxWidth Maximum width (in pixels) allowed for column.  Ignored if set to zero.
    //-----------------------------------------------------------------------------
    void QtUtil::AutoAdjustTableColumns(QTableView* pTable, int maxRows, int padding, int maxWidth)
    {
        Q_ASSERT(pTable);
        QHeaderView* pHeader = pTable->horizontalHeader();
        if (pHeader != nullptr)
        {
            pTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
            const int columnCount = pTable->horizontalHeader()->count();
            QVector<int> columnWidth(columnCount);

            QAbstractItemModel* pModel = (QAbstractItemModel*)pTable->model();
            if (pModel != nullptr)
            {
                int margin = 0;
                QStyle* pStyle = pHeader->style();
                if (pStyle != nullptr)
                {
                    margin = pHeader->style()->pixelMetric(QStyle::PM_HeaderMargin, 0, pHeader);
                }

                const int rowCount = pModel->rowCount();
                const int elideWidth = QtUtil::GetTextWidth(pTable->font(), QString(0x2026));
                QFontMetrics fm(pHeader->font());
                pHeader->setFixedHeight(fm.height() + (margin * 2));

                for (int column = 0; column < columnCount; column++)
                {
                    const QVariant headerData = pModel->headerData(column, Qt::Horizontal);
                    const QString textHeader = headerData.toString().trimmed();
                    columnWidth[column] = QtUtil::GetTextWidth(pHeader->font(), textHeader) + padding + (margin * 2);
                    for (int row = 0; (row < rowCount) && (row < maxRows); row++)
                    {
                        const QString textData = pModel->data(pModel->index(row, column)).toString().trimmed();
                        int width = QtUtil::GetTextWidth(pTable->font(), textData) + elideWidth;
                        if (width > columnWidth[column])
                        {
                            columnWidth[column] = width;
                        }
                    }
                }

                pHeader->setSectionResizeMode(QHeaderView::Interactive);
                for (int column = 0; column < columnCount; column++)
                {
                    if ((maxWidth == 0) || ((columnWidth[column] + padding) < maxWidth))
                    {
                        pTable->setColumnWidth(column, columnWidth[column] + padding);
                    }
                    else
                    {
                        pTable->setColumnWidth(column, maxWidth);
                    }
                }
            }
        }
    }

    //-----------------------------------------------------------------------------
    /// Compute the minimum width required to display all columns of the given QTreeView.
    /// \param pTreeView Pointer to the tree view object.
    /// \param maxRows Maximum number of rows to sample when calculating column width.
    /// \param padding Additional pixels added to the calculated width of the longest string.
    /// \returns The minimum width required to display all columns of the given QTreeView.
    //-----------------------------------------------------------------------------
    int QtUtil::ComputeMinimumTableWidth(QTreeView* pTreeView, int maxRows, int padding)
    {
        int minimumWidth = 0;

        Q_ASSERT(pTreeView);
        QHeaderView* pHeader = pTreeView->header();
        if (pHeader != nullptr)
        {
            QHeaderView* pHeaderView = pTreeView->header();
            if (pHeaderView != nullptr)
            {
                const int columnCount = pTreeView->header()->count();
                QVector<int> columnWidths(columnCount);

                QAbstractItemModel* pModel = (QAbstractItemModel*)pTreeView->model();
                if (pModel != nullptr)
                {
                    int margin = 0;
                    QStyle* pStyle = pHeader->style();
                    if (pStyle != nullptr)
                    {
                        margin = pHeader->style()->pixelMetric(QStyle::PM_HeaderMargin, 0, pHeader);
                    }

                    const int rowCount = pModel->rowCount();
                    const int elideWidth = QtCommon::QtUtil::GetTextWidth(pTreeView->font(), QString(0x2026));

                    for (int column = 0; column < columnCount; column++)
                    {
                        QVariant headerData = pModel->headerData(column, Qt::Horizontal);
                        QString textHeader = headerData.toString().trimmed();
                        columnWidths[column] = QtCommon::QtUtil::GetTextWidth(pHeader->font(), textHeader) + padding + (margin * 2);
                        for (int row = 0; (row < rowCount) && (row < maxRows); row++)
                        {
                            const QString textData = pModel->data(pModel->index(row, column)).toString().trimmed();
                            const int width = QtCommon::QtUtil::GetTextWidth(pTreeView->font(), textData) + elideWidth;
                            if (width > columnWidths[column])
                            {
                                columnWidths[column] = width;
                            }
                        }
                    }

                    // Sum the widths of all visible columns.
                    for (int columnWidth : columnWidths)
                    {
                        minimumWidth += columnWidth;
                    }
                }
            }
        }
        else
        {
            minimumWidth = QWIDGETSIZE_MAX;
        }

        return minimumWidth;
    }

    //-----------------------------------------------------------------------------
    /// Compute the file size acronym for the given byte count.
    /// \param byteCount The number of bytes to compute the size acronym for.
    /// \param sizeString A string with the file size acronym.
    /// \param useRound If true, round the value to the nearest integer.
    //-----------------------------------------------------------------------------
    void QtUtil::GetFilesizeAcronymFromByteCount(quint64 byteCount, QString& sizeString, bool useRound)
    {
        static const float kNumBytesInMegabyte = 1024.0f;

        double scaledSize = byteCount;

        int acroynmIndex = 0;
        while (scaledSize >= kNumBytesInMegabyte)
        {
            scaledSize /= kNumBytesInMegabyte;
            acroynmIndex++;
        }

        if (useRound == true)
        {
            scaledSize = round(scaledSize);
        }

        // Catch gigantic filesizes.
        static const QString sizeAcronym[] = { " B", " KB", " MB", " GB", " TB", " PB" };
        Q_ASSERT(acroynmIndex < 6);

        // Return the abbreviated file size and the unit acronym.
        sizeString = QString::number(scaledSize, 'g', 4) + sizeAcronym[acroynmIndex];
    }

    //-----------------------------------------------------------------------------
    /// Calculate the height of a list widget
    /// \param pListWidget Pointer to the list widget whose height is needed
    /// \return The height of the list widget
    //-----------------------------------------------------------------------------
    int QtUtil::GetListWidgetHeight(ListWidget* pListWidget)
    {
        QFontMetrics fm(pListWidget->font());
        int count = pListWidget->count();
        int height = 0;
        for (int loop = 0; loop < count; loop++)
        {
            height += pListWidget->sizeHintForRow(loop);
        }
        height += pListWidget->frameWidth() * 2;

        return height;
    }

    //-----------------------------------------------------------------------------
    /// Calculate the width of a list widget
    /// \param pListWidget Pointer to the list widget whose width is needed
    /// \return The width of the list widget
    //-----------------------------------------------------------------------------
    int QtUtil::GetListWidgetWidth(ListWidget* pListWidget)
    {
        QFontMetrics fm(pListWidget->font());
        int width = 0;
        for (int loop = 0; loop < pListWidget->count(); loop++)
        {
            if (width < pListWidget->sizeHintForColumn(loop))
            {
                width = pListWidget->sizeHintForColumn(loop);
            }
        }

        return width;
    }

    //-----------------------------------------------------------------------------
    /// Calculate the pixel width of a string
    /// \param font The font to measure
    /// \param str The string to measure
    /// \return The string width
    //-----------------------------------------------------------------------------
    int QtUtil::GetTextWidth(const QFont& font, const QString& str)
    {
        QFontMetrics fm(font);
        return fm.width(str);
    }

    //-----------------------------------------------------------------------------
    /// Calculate the pixel width of a string
    /// \param pPainter ptr to painter
    /// \param str The string to measure
    /// \return the string width
    //-----------------------------------------------------------------------------
    int QtUtil::GetPainterTextWidth(QPainter* pPainter, const QString& str)
    {
        int width = 0;

        if (pPainter != nullptr)
        {
            QRect boundingRect = pPainter->boundingRect(QRect(0, 0, 0, 0), Qt::AlignLeft, str);
            width = boundingRect.width();
        }

        return width;
    }

    //-----------------------------------------------------------------------------
    /// Verify that at least two check boxes are checked, not counting the
    /// first one ("All").
    /// \param visibilityVector The vetor to look through
    /// \param firstcolumn The first column index
    /// \param lastColumn The last column index
    /// \return The boolean to indicate whether to allow unchecking of this check box
    //-----------------------------------------------------------------------------
    bool QtUtil::VerifyOneCheckboxChecked(const std::vector<bool>& visibilityVector, int firstColumn, int lastColumn)
    {
        bool result = false;
        int counter = 0;

        // Step through the vector and verify at least two items are checked.
        for (int columnIndex = firstColumn; columnIndex < lastColumn; ++columnIndex)
        {
            if (visibilityVector[columnIndex])
            {
                counter++;
                if (counter >= 2)
                {
                    // Set result to true if counter is greater than or equal to two.
                    result = true;

                    break;
                }
            }
        }

        // Return the result.
        return result;
    }

    //-----------------------------------------------------------------------------
    /// Utility function to convert a clock to a time unit and output as string.
    /// \param clk input clock to convert
    /// \param unitType unit type (clk, ns, us, ms)
    /// \return a string representing a clock value
    //-----------------------------------------------------------------------------
    QString QtUtil::ClockToTimeUnit(double clk, int unitType)
    {
        double time = clk;

        QString str = "";
        QTextStream out(&str);
        out.setRealNumberNotation(QTextStream::FixedNotation);
        out.setLocale(QLocale::English);

        switch (unitType)
        {
        case TIME_UNIT_TYPE_NS:
            out.setRealNumberPrecision(0);
            out << time << " ns";
            break;

        case TIME_UNIT_TYPE_US:
            time /= 1000.0;
            out.setRealNumberPrecision(3);
            out << time << QString(u8" μs");
            break;

        case TIME_UNIT_TYPE_MS:
            out.setRealNumberPrecision(3);
            time /= 1000000.0;
            out << time << " ms";
            break;

        case TIME_UNIT_TYPE_CLK:
        default:
            out << (quint64)clk << " clk";
            break;
        }

        return str;
    }

    //-----------------------------------------------------------------------------
    /// Utility function to convert a uint64 value to capitalized and aligned str
    /// \param val 64-bit value
    /// \return a string representation of the value
    //-----------------------------------------------------------------------------
    QString QtUtil::HashToStr(quint64 val)
    {
        char buffer[19] = {};
        snprintf(buffer, 19, "%016llX", val);
        return buffer;
    }
}