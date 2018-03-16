//=============================================================================
/// Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  Header for QtUtil which holds useful utility functions.
//=============================================================================

#ifndef _QT_UTIL_H_
#define _QT_UTIL_H_

#include <vector>

#include <QtGlobal>

class QFont;
class QTreeView;
class QWidget;
class ArrowIconWidget;
class ListWidget;

namespace QtCommon
{
    namespace QtUtil
    {
        void ApplyStandardTableStyle(QTreeView* pView);
        void AutoAdjustTableColumns(QTreeView* pTable, int maxRows = 32, int padding = 0, int maxWidth = 0);
        int ComputeMinimumTableWidth(QTreeView* pTable, int maxRows = 32, int padding = 0);
        void GetFilesizeAcronymFromByteCount(quint64 byteCount, QString& sizeString, bool useRound = false);
        int GetListWidgetHeight(ListWidget* pListWidget);
        int GetListWidgetWidth(ListWidget* pListWidget);
        int GetTextWidth(const QFont& font, const QString& str);
        bool VerifyOneCheckboxChecked(const std::vector<bool>& visibilityVector, int firstcolumn, int lasColumn);
    }
}

#endif // _QT_UTIL_H_
