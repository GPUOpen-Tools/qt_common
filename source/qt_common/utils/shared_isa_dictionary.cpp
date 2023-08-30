//=============================================================================
// Copyright (c) 2022 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief Implementation for an isa dictionary.
//=============================================================================

#include "shared_isa_dictionary.h"

#include "common_definitions.h"
#include "qt_util.h"

namespace QtCommon
{
    namespace QtUtils
    {
        IsaColorCodingDictionaryInstance& IsaColorCodingDictionaryInstance::GetInstance()
        {
            static IsaColorCodingDictionaryInstance instance;
            return instance;
        }

        bool IsaColorCodingDictionaryInstance::ShouldHighlight(const std::string& str, QColor& color) const
        {
            color = QtCommon::QtUtils::ColorTheme::Get().GetCurrentThemeColors().graphics_scene_text_color;

            QColor tree_colors;

            bool should_highlight = prefix_tree_[QtCommon::QtUtils::ColorTheme::Get().GetColorTheme()].PrefixFoundInTree(str, tree_colors);

            if (tree_colors != nullptr)
            {
                color = tree_colors;
            }

            return should_highlight;
        }

        IsaColorCodingDictionaryInstance::IsaColorCodingDictionaryInstance()
        {
            prefix_tree_[kColorThemeTypeLight].Insert("s_buffer", kIsaLightThemeColorLightOrange);
            prefix_tree_[kColorThemeTypeLight].Insert("s_load", kIsaLightThemeColorLightOrange);
            prefix_tree_[kColorThemeTypeLight].Insert("s_waitcnt", kIsaLightThemeColorPink);
            prefix_tree_[kColorThemeTypeLight].Insert("expcnt", kIsaLightThemeColorPink);
            prefix_tree_[kColorThemeTypeLight].Insert("vmcnt", kIsaLightThemeColorPink);
            prefix_tree_[kColorThemeTypeLight].Insert("lgkmcnt", kIsaLightThemeColorPink);
            prefix_tree_[kColorThemeTypeLight].Insert("s_swap", kIsaLightThemeColorRed);
            prefix_tree_[kColorThemeTypeLight].Insert("s_branch", kIsaLightThemeColorRed);
            prefix_tree_[kColorThemeTypeLight].Insert("s_cbranch", kIsaLightThemeColorRed);
            prefix_tree_[kColorThemeTypeLight].Insert("s_setpc", kIsaLightThemeColorRed);
            prefix_tree_[kColorThemeTypeLight].Insert("ds_", kIsaLightThemeColorBlue);
            prefix_tree_[kColorThemeTypeLight].Insert("buffer_", kIsaLightThemeColorPurple);
            prefix_tree_[kColorThemeTypeLight].Insert("tbuffer_", kIsaLightThemeColorPurple);
            prefix_tree_[kColorThemeTypeLight].Insert("image_", kIsaLightThemeColorPurple);
            prefix_tree_[kColorThemeTypeLight].Insert("global_load", kIsaLightThemeColorPurple);
            prefix_tree_[kColorThemeTypeLight].Insert("idxen", kIsaLightThemeColorPurple);
            prefix_tree_[kColorThemeTypeLight].Insert("s_", kIsaLightThemeColorBlue);
            prefix_tree_[kColorThemeTypeLight].Insert("s[", kIsaLightThemeColorBlue);  // Scalar register.
            prefix_tree_[kColorThemeTypeLight].Insert("[s", kIsaLightThemeColorBlue);  // Scalar register range.
            prefix_tree_[kColorThemeTypeLight].Insert("|s", kIsaLightThemeColorBlue);  // Scalar register absolute value.
            prefix_tree_[kColorThemeTypeLight].Insert("-s", kIsaLightThemeColorBlue);  // Scalar register negative value.
            prefix_tree_[kColorThemeTypeLight].Insert("v_", kIsaLightThemeColorLightGreen);
            prefix_tree_[kColorThemeTypeLight].Insert("v[", kIsaLightThemeColorLightGreen);  // Vector register.
            prefix_tree_[kColorThemeTypeLight].Insert("[v", kIsaLightThemeColorLightGreen);  // Vector register range.
            prefix_tree_[kColorThemeTypeLight].Insert("|v", kIsaLightThemeColorLightGreen);  // Vector register absolute value.
            prefix_tree_[kColorThemeTypeLight].Insert("-v", kIsaLightThemeColorLightGreen);  // Vector register negative value.
            prefix_tree_[kColorThemeTypeLight].Insert("//", kIsaLightThemeColorLightBlue);   // Comments.

            for (size_t i = 0; i <= 9; ++i)
            {
                prefix_tree_[kColorThemeTypeLight].Insert("s" + std::to_string(i), kIsaLightThemeColorBlue);
                prefix_tree_[kColorThemeTypeLight].Insert("v" + std::to_string(i), kIsaLightThemeColorLightGreen);
            }

            prefix_tree_[kColorThemeTypeDark].Insert("s_buffer", kIsaDarkThemeColorLightOrange);
            prefix_tree_[kColorThemeTypeDark].Insert("s_load", kIsaDarkThemeColorLightOrange);
            prefix_tree_[kColorThemeTypeDark].Insert("s_waitcnt", kIsaDarkThemeColorPink);
            prefix_tree_[kColorThemeTypeDark].Insert("expcnt", kIsaDarkThemeColorPink);
            prefix_tree_[kColorThemeTypeDark].Insert("vmcnt", kIsaDarkThemeColorPink);
            prefix_tree_[kColorThemeTypeDark].Insert("lgkmcnt", kIsaDarkThemeColorPink);
            prefix_tree_[kColorThemeTypeDark].Insert("s_swap", kIsaDarkThemeColorRed);
            prefix_tree_[kColorThemeTypeDark].Insert("s_branch", kIsaDarkThemeColorRed);
            prefix_tree_[kColorThemeTypeDark].Insert("s_cbranch", kIsaDarkThemeColorRed);
            prefix_tree_[kColorThemeTypeDark].Insert("s_setpc", kIsaDarkThemeColorRed);
            prefix_tree_[kColorThemeTypeDark].Insert("ds_", kIsaDarkThemeColorBlue);
            prefix_tree_[kColorThemeTypeDark].Insert("buffer_", kIsaDarkThemeColorPurple);
            prefix_tree_[kColorThemeTypeDark].Insert("tbuffer_", kIsaDarkThemeColorPurple);
            prefix_tree_[kColorThemeTypeDark].Insert("image_", kIsaDarkThemeColorPurple);
            prefix_tree_[kColorThemeTypeDark].Insert("global_load", kIsaDarkThemeColorPurple);
            prefix_tree_[kColorThemeTypeDark].Insert("idxen", kIsaDarkThemeColorPurple);
            prefix_tree_[kColorThemeTypeDark].Insert("s_", kIsaDarkThemeColorBlue);
            prefix_tree_[kColorThemeTypeDark].Insert("s[", kIsaDarkThemeColorBlue);  // Scalar register.
            prefix_tree_[kColorThemeTypeDark].Insert("[s", kIsaDarkThemeColorBlue);  // Scalar register range.
            prefix_tree_[kColorThemeTypeDark].Insert("|s", kIsaDarkThemeColorBlue);  // Scalar register absolute value.
            prefix_tree_[kColorThemeTypeDark].Insert("-s", kIsaDarkThemeColorBlue);  // Scalar register negative value.
            prefix_tree_[kColorThemeTypeDark].Insert("v_", kIsaDarkThemeColorLightGreen);
            prefix_tree_[kColorThemeTypeDark].Insert("v[", kIsaDarkThemeColorLightGreen);  // Vector register.
            prefix_tree_[kColorThemeTypeDark].Insert("[v", kIsaDarkThemeColorLightGreen);  // Vector register range.
            prefix_tree_[kColorThemeTypeDark].Insert("|v", kIsaDarkThemeColorLightGreen);  // Vector register absolute value.
            prefix_tree_[kColorThemeTypeDark].Insert("-v", kIsaDarkThemeColorLightGreen);  // Vector register negative value.
            prefix_tree_[kColorThemeTypeDark].Insert("//", kIsaDarkThemeColorLightBlue);   // Comments.

            for (size_t i = 0; i <= 9; ++i)
            {
                prefix_tree_[kColorThemeTypeDark].Insert("s" + std::to_string(i), kIsaDarkThemeColorBlue);
                prefix_tree_[kColorThemeTypeDark].Insert("v" + std::to_string(i), kIsaDarkThemeColorLightGreen);
            }
        }
    }  // namespace QtUtils
}  // namespace QtCommon
