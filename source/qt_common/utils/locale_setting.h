//==============================================================================
// Copyright (c) 2015-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief This class manages the locale settings.
//==============================================================================

#ifndef QTCOMMON_UTILS_LOCALE_SETTING_H_
#define QTCOMMON_UTILS_LOCALE_SETTING_H_

/// This class manages the locale settings
class LocaleSetting
{
public:
    /// Retrieve the list separator in the current locale
    /// \return a list separator
    static char GetListSeparator();
};

#endif  //QTCOMMON_UTILS_LOCALE_SETTING_H_
