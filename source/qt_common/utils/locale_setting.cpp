//==============================================================================
// Copyright (c) 2015-2025 Advanced Micro Devices, Inc. All rights reserved.
/// \author AMD Developer Tools Team
/// \file
/// \brief This class manages the locale settings.
//==============================================================================

#include "locale_setting.h"

#ifdef _WIN32
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif  //_WIN32

#ifdef __linux__
#include <cstring>
#endif  //__linux__
#include <locale>

#ifdef _WIN32

char LocaleSetting::GetListSeparator()
{
    // set the current locale to be based on the current OS settings
    // user can override this settings by using a LANG environment variable (see the setlocale's manual)
    setlocale(LC_ALL, "");

    char list_separator = ',';  // the default setting

    // get the list separator from the registry key
    HKEY key;

    if (RegOpenKeyExA(HKEY_CURRENT_USER, "Control Panel\\International", 0, KEY_QUERY_VALUE, &key) == ERROR_SUCCESS)
    {
        char  list_separator_string[256];
        DWORD size = sizeof(list_separator_string);

        if (RegQueryValueExA(key, "list_separator_string", NULL, NULL, (BYTE*)list_separator_string, &size) == ERROR_SUCCESS)
        {
            list_separator = list_separator_string[0];
        }

        RegCloseKey(key);
    }

    return list_separator;
}

#else

#include <langinfo.h>

// Use default separator for linux
char LocaleSetting::GetListSeparator()
{
    // Use comma for the list separator, unless comma is also used for the decimal character, then use a semi-colon.
    // There doesn't appear to be a "list separator" character on Linux, like there is on Windows, so we need to try
    // our best to use a character than doesn't conflict with another locale-specific character (like the comma decimal
    // character in some cases)
    char ret_val = ',';

#ifdef __linux__
    // For Mac, we will always just return ',' rather than querying the OS.  We currently don't need a locale-specific
    // delimiter for our Mac builds.

    char* current_locale;
    char* saved_locale;

    // get current locale
    current_locale   = setlocale(LC_ALL, NULL);
    const size_t len = strlen(current_locale);

    // save a copy so it doesn't get overwritten by next call to setlocale
    saved_locale = (char*)malloc(sizeof(char) * (len + 1));

    if (nullptr == saved_locale)
    {
        // malloc failed
        return ret_val;
    }
    std::memcpy(saved_locale, current_locale, len);
    saved_locale[len] = '\0';

    // set the current locale to be based on the current OS settings
    // user can override this settings by using a LANG environment variable (see the setlocale's manual)
    setlocale(LC_ALL, "");

    std::string decimal_char = nl_langinfo(RADIXCHAR);

    // restore original locale
    setlocale(LC_ALL, saved_locale);

    free(saved_locale);
    if (decimal_char == ",")
    {
        ret_val = ';';
    }
#endif  //__linux__

    return ret_val;
}

#endif  //_WIN32
