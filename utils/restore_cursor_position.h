//=============================================================================
/// Copyright (c) 2017-2020 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief  A utility class used to restore a QLineEdit's cursor position
///         after the text has been edited.
//=============================================================================

#ifndef QTCOMMON_UTILS_RESTORE_CURSOR_POSITION_H_
#define QTCOMMON_UTILS_RESTORE_CURSOR_POSITION_H_

#include <QLineEdit>

namespace QtCommon
{
    namespace QtUtils
    {
        /// Implementation of the RestoreCursorPosition class.
        class RestoreCursorPosition
        {
        public:
            /// Constructor for RestoreCursorPosition.
            /// \param control The QLineEdit whose cursor position will be stored.
            explicit RestoreCursorPosition(QLineEdit* control)
                : line_edit_(control)
            {
                if (line_edit_ != nullptr)
                {
                    cursor_position_ = line_edit_->cursorPosition();
                }
            }

            /// Destructor for RestoreCursorPosition.
            ~RestoreCursorPosition()
            {
                if (line_edit_ != nullptr)
                {
                    line_edit_->setCursorPosition(cursor_position_);
                }
            }

        private:
            QLineEdit* line_edit_;          ///< The QLineEdit whose cursor position should be maintained.
            int        cursor_position_;    ///< The cursor position that should be restored.
        };
    }  // namespace QtUtils
}  // namespace QtCommon

#endif  // QTCOMMON_UTILS_RESTORE_CURSOR_POSITION_H_
