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

#include <../CustomWidgets/ArrowIconWidget.h>
#include <../CustomWidgets/ListWidget.h>
#include <../Util/QtUtil.h>

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
        if (!pHeader)
            return;

        pTable->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
        int columnCount = pTable->header()->count();
        QVector<int> columnWidth(columnCount);

        QAbstractItemModel* pModel = (QAbstractItemModel*)pTable->model();
        if (pModel)
        {
            int margin = 0;
            QStyle* pStyle = pHeader->style();
            if (pStyle)
            {
                margin = pHeader->style()->pixelMetric(QStyle::PM_HeaderMargin, 0, pHeader);
            }

            int rowCount = pModel->rowCount();
            int elideWidth = QtUtil::GetTextWidth(pTable->font(), QString(0x2026));
            QFontMetrics fm(pHeader->font());
            pHeader->setFixedHeight(fm.height() + (margin * 2));

            for (int column = 0; column < columnCount; column++)
            {
                QVariant headerData = pModel->headerData(column, Qt::Horizontal);
                QString textHeader = headerData.toString().trimmed();
                columnWidth[column] = QtUtil::GetTextWidth(pHeader->font(), textHeader) + padding + (margin * 2);
                for (int row = 0; (row < rowCount) && (row < maxRows); row++)
                {
                    QString textData = pModel->data(pModel->index(row, column)).toString().trimmed();
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
}