//=============================================================================
/// Copyright (c) 2017-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header for QtUtil which holds useful utility functions.
//=============================================================================

#include "qt_util.h"

#include <cmath>

#include <QApplication>
#include <QDesktopServices>
#include <QDir>
#include <QFileInfo>
#include <QHeaderView>
#include <QProcess>
#include <QTextStream>
#include <QUrl>

#include "common_definitions.h"
#include "scaling_manager.h"

#ifdef _WIN32
#pragma warning(disable : 4714)  // prevent warning caused by QString::trimmed()
#endif                           // _WIN32

namespace QtCommon
{
    void QtUtils::ApplyStandardTableStyle(QTreeView* view)
    {
        if (view != nullptr)
        {
            QFont font = view->font();
            font.setBold(true);
            view->header()->setFont(font);

            font.setBold(false);
            view->setFont(font);

            view->setRootIsDecorated(false);
            view->setItemsExpandable(false);
            view->setExpandsOnDoubleClick(false);

            view->header()->setDefaultAlignment(Qt::AlignLeft);
            view->header()->setSectionsClickable(false);
            view->header()->setStretchLastSection(true);
            view->setFrameStyle(QFrame::NoFrame);
            view->setSelectionMode(QAbstractItemView::SingleSelection);
            view->setEditTriggers(QAbstractItemView::NoEditTriggers);
            view->setSelectionBehavior(QAbstractItemView::SelectRows);
            view->setWordWrap(false);
            view->setAlternatingRowColors(true);

            view->setStyleSheet(view->styleSheet() + "QTreeView{paint-alternating-row-colors-for-empty-area: 1; }");
        }
    }

    void QtUtils::FitColumnsToContents(QTreeView* tree_view)
    {
        QHeaderView* header = tree_view->header();
        if (header != nullptr)
        {
            tree_view->header()->setSectionResizeMode(QHeaderView::ResizeToContents);

            const int           column_count = tree_view->header()->count();
            QAbstractItemModel* model        = static_cast<QAbstractItemModel*>(tree_view->model());
            if (model != nullptr)
            {
                header->setSectionResizeMode(QHeaderView::Interactive);
                for (int column = 0; column < column_count; column++)
                {
                    tree_view->resizeColumnToContents(column);
                }
            }
        }
    }

    void QtUtils::AutoAdjustTableColumns(QTreeView* tree_view, int max_rows, int padding, int max_width)
    {
        Q_ASSERT(tree_view);
        QHeaderView* header = tree_view->header();
        if (header != nullptr)
        {
            tree_view->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
            const int    column_count = tree_view->header()->count();
            QVector<int> column_widths(column_count);

            QAbstractItemModel* model = (QAbstractItemModel*)tree_view->model();
            if (model != nullptr)
            {
                int     margin = 0;
                QStyle* style  = header->style();
                if (style != nullptr)
                {
                    margin = header->style()->pixelMetric(QStyle::PM_HeaderMargin, 0, header);
                }

                const int    row_count   = model->rowCount();
                const int    elide_width = QtUtils::GetTextWidth(tree_view->font(), QString(0x2026));
                QFontMetrics fm(header->font());
                header->setFixedHeight(fm.height() + (margin * 2));

                for (int column = 0; column < column_count; column++)
                {
                    const QVariant header_data = model->headerData(column, Qt::Horizontal);
                    const QString  text_header = header_data.toString().trimmed();
                    column_widths[column]      = QtUtils::GetTextWidth(header->font(), text_header) + padding + (margin * 2);
                    for (int row = 0; (row < row_count) && (row < max_rows); row++)
                    {
                        const QString textData = model->data(model->index(row, column)).toString().trimmed();
                        int           width    = QtUtils::GetTextWidth(tree_view->font(), textData) + elide_width;
                        if (width > column_widths[column])
                        {
                            column_widths[column] = width;
                        }
                    }
                }

                header->setSectionResizeMode(QHeaderView::Interactive);
                for (int column = 0; column < column_count; column++)
                {
                    if ((max_width == 0) || ((column_widths[column] + padding) < max_width))
                    {
                        tree_view->setColumnWidth(column, column_widths[column] + padding);
                    }
                    else
                    {
                        tree_view->setColumnWidth(column, max_width);
                    }
                }
            }
        }
    }

    void QtUtils::AutoAdjustTableColumns(QTableView* table, int max_rows, int padding, int max_width)
    {
        Q_ASSERT(table);
        QHeaderView* header = table->horizontalHeader();
        if (header != nullptr)
        {
            table->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
            const int    column_count = table->horizontalHeader()->count();
            QVector<int> column_widths(column_count);

            QAbstractItemModel* model = (QAbstractItemModel*)table->model();
            if (model != nullptr)
            {
                int     margin = 0;
                QStyle* style  = header->style();
                if (style != nullptr)
                {
                    margin = header->style()->pixelMetric(QStyle::PM_HeaderMargin, 0, header);
                }

                const int    row_count   = model->rowCount();
                const int    elide_width = QtUtils::GetTextWidth(table->font(), QString(0x2026));
                QFontMetrics fm(header->font());
                header->setFixedHeight(fm.height() + (margin * 2));

                for (int column = 0; column < column_count; column++)
                {
                    const QVariant header_data = model->headerData(column, Qt::Horizontal);
                    const QString  text_header = header_data.toString().trimmed();
                    column_widths[column]      = QtUtils::GetTextWidth(header->font(), text_header) + padding + (margin * 2);
                    for (int row = 0; (row < row_count) && (row < max_rows); row++)
                    {
                        const QString text_data = model->data(model->index(row, column)).toString().trimmed();
                        int           width     = QtUtils::GetTextWidth(table->font(), text_data) + elide_width;
                        if (width > column_widths[column])
                        {
                            column_widths[column] = width;
                        }
                    }
                }

                header->setSectionResizeMode(QHeaderView::Interactive);
                for (int column = 0; column < column_count; column++)
                {
                    if ((max_width == 0) || ((column_widths[column] + padding) < max_width))
                    {
                        table->setColumnWidth(column, column_widths[column] + padding);
                    }
                    else
                    {
                        table->setColumnWidth(column, max_width);
                    }
                }
            }
        }
    }

    int QtUtils::ComputeMinimumTableWidth(QTreeView* tree_view, int maxRows, int padding)
    {
        int minimum_width = 0;

        Q_ASSERT(tree_view);
        QHeaderView* header = tree_view->header();
        if (header != nullptr)
        {
            QHeaderView* header_view = tree_view->header();
            if (header_view != nullptr)
            {
                const int    column_count = tree_view->header()->count();
                QVector<int> column_widths(column_count);

                QAbstractItemModel* model = (QAbstractItemModel*)tree_view->model();
                if (model != nullptr)
                {
                    int     margin = 0;
                    QStyle* style  = header->style();
                    if (style != nullptr)
                    {
                        margin = header->style()->pixelMetric(QStyle::PM_HeaderMargin, 0, header);
                    }

                    const int row_count   = model->rowCount();
                    const int elide_width = QtCommon::QtUtils::GetTextWidth(tree_view->font(), QString(0x2026));

                    for (int column = 0; column < column_count; column++)
                    {
                        QVariant header_data  = model->headerData(column, Qt::Horizontal);
                        QString  text_header  = header_data.toString().trimmed();
                        column_widths[column] = QtCommon::QtUtils::GetTextWidth(header->font(), text_header) + padding + (margin * 2);
                        for (int row = 0; (row < row_count) && (row < maxRows); row++)
                        {
                            const QString text_data = model->data(model->index(row, column)).toString().trimmed();
                            const int     width     = QtCommon::QtUtils::GetTextWidth(tree_view->font(), text_data) + elide_width;
                            if (width > column_widths[column])
                            {
                                column_widths[column] = width;
                            }
                        }
                    }

                    // Sum the widths of all visible columns.
                    for (int column_width : column_widths)
                    {
                        minimum_width += column_width;
                    }
                }
            }
        }
        else
        {
            minimum_width = QWIDGETSIZE_MAX;
        }

        return minimum_width;
    }

    void QtUtils::GetFilesizeAcronymFromByteCount(quint64 byte_count, QString& size_string, bool use_round)
    {
        const float kNumBytesInMegabyte = 1024.0f;

        double scaled_size = byte_count;

        int acronym_index = 0;
        while (scaled_size >= kNumBytesInMegabyte)
        {
            scaled_size /= kNumBytesInMegabyte;
            acronym_index++;
        }

        if (use_round == true)
        {
            scaled_size = round(scaled_size);
        }

        // Catch gigantic filesizes.
        const QString kSizeAcronym[] = {" B", " KB", " MB", " GB", " TB", " PB"};
        Q_ASSERT(acronym_index < 6);

        // Return the abbreviated file size and the unit acronym.
        size_string = QString::number(scaled_size, 'g', 4) + kSizeAcronym[acronym_index];
    }

    int QtUtils::GetTextWidth(const QFont& font, const QString& str)
    {
        return ScalingManager::Get().ScaledFontMetrics(font).boundingRect(str).width();
    }

    int QtUtils::GetPainterTextWidth(QPainter* pPainter, const QString& str)
    {
        int width = 0;

        if (pPainter != nullptr)
        {
            QRect boundingRect = pPainter->boundingRect(QRect(0, 0, 0, 0), Qt::AlignLeft, str);
            width              = boundingRect.width();
        }

        return width;
    }

    void QtUtils::InvalidateFontMetrics(QWidget* widget)
    {
        if (widget != nullptr)
        {
            // Invalidate font and fontmetrics by toggling the font size.
            QFont tmp_font            = widget->font();
            qreal original_point_size = tmp_font.pointSizeF();
            tmp_font.setPointSizeF(original_point_size + 1);
            widget->setFont(tmp_font);
            tmp_font.setPointSizeF(original_point_size);
            widget->setFont(tmp_font);
        }
    }

    void QtUtils::InvalidateFontMetrics(QGraphicsScene* scene)
    {
        if (scene != nullptr)
        {
            // Invalidate font and fontmetrics by toggling the font size.
            QFont tmp_font            = scene->font();
            qreal original_point_size = tmp_font.pointSizeF();
            tmp_font.setPointSizeF(original_point_size + 1);
            scene->setFont(tmp_font);
            tmp_font.setPointSizeF(original_point_size);
            scene->setFont(tmp_font);
        }
    }

    bool QtUtils::VerifyOneCheckboxChecked(const std::vector<bool>& visibility_vector, int first_column, int last_column)
    {
        bool result  = false;
        int  counter = 0;

        // Step through the vector and verify at least two items are checked.
        for (int column_index = first_column; column_index < last_column; ++column_index)
        {
            if (visibility_vector[column_index])
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

    QString QtUtils::ClockToTimeUnit(double clk, int unit_type)
    {
        double time = clk;

        QString     str = "";
        QTextStream out(&str);
        out.setRealNumberNotation(QTextStream::FixedNotation);
        out.setLocale(QLocale::English);

        switch (unit_type)
        {
        case kTimeUnitTypeNanosecond:
            out.setRealNumberPrecision(0);
            out << time << " ns";
            break;

        case kTimeUnitTypeMicrosecond:
            time /= 1000.0;
            out.setRealNumberPrecision(3);
            out << time << QString(u8" μs");
            break;

        case kTimeUnitTypeMillisecond:
            out.setRealNumberPrecision(3);
            time /= 1000000.0;
            out << time << " ms";
            break;

        case kTimeUnitTypeSecond: {
            out.setRealNumberPrecision(0);
            uint64_t secs = (uint64_t)(time / 1000000000.0);
            uint64_t ms   = (uint64_t)fmod((time / 10000000.0), 100);
            out << QString::number(secs).rightJustified(2, '0') << "." << QString::number(ms).rightJustified(2, '0') << "s";
        }
        break;

        case kTimeUnitTypeMinute: {
            out.setRealNumberPrecision(0);
            uint64_t mins = (uint64_t)(time / 60000000000.0);
            uint64_t secs = (uint64_t)fmod((time / 1000000000.0), 60);
            uint64_t ms   = (uint64_t)fmod((time / 10000000.0), 100);
            out << mins << "m " << QString::number(secs).rightJustified(2, '0') << "." << QString::number(ms).rightJustified(2, '0') << "s";
        }
        break;

        case kTimeUnitTypeClk:
        default:
            out << (quint64)clk << " clk";
            break;
        }

        return str;
    }

    QString QtUtils::HashToStr(quint64 value)
    {
        char buffer[19] = {};
        snprintf(buffer, 19, "%016llX", value);
        return buffer;
    }

    bool QtUtils::BrowseToFile(const QString& path)
    {
        QFileInfo file_info(path);
        if ((file_info.exists() == false) || (file_info.isFile() == false))
        {
            return false;
        }

#if _WIN32
        QStringList explorer_params;
        if (QFileInfo(path).isDir() == false)
        {
            explorer_params << QLatin1String("/select,");
        }

        explorer_params << QDir::toNativeSeparators(path);

        QProcess::startDetached("explorer", explorer_params);
#else
        QString dir_path = file_info.absoluteDir().absolutePath();

        QDesktopServices::openUrl(QUrl::fromLocalFile(dir_path));
#endif

        return true;
    }

    int QtUtils::ShowMessageBox(QWidget* parent, QMessageBox::StandardButtons buttons, QMessageBox::Icon icon, const QString& title, const QString& message)
    {
        QMessageBox message_box(parent);
        message_box.setWindowTitle(title);
        message_box.setText(message);
        message_box.setStandardButtons(buttons);
        message_box.setIcon(icon);

        // Display the message box
        return message_box.exec();
    }

}  // namespace QtCommon
