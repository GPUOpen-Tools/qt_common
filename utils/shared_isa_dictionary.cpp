//=============================================================================
// Copyright (c) 2022 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief Implementation for an isa dictionary.
//=============================================================================

#include "shared_isa_dictionary.h"

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
            return prefix_tree_.PrefixFoundInTree(str, color);
        }

        IsaColorCodingDictionaryInstance::IsaColorCodingDictionaryInstance()
        {
            prefix_tree_.Insert("s_buffer", kIsaColorLightOrange);
            prefix_tree_.Insert("s_load", kIsaColorLightOrange);
            prefix_tree_.Insert("s_waitcnt", kIsaColorPink);
            prefix_tree_.Insert("expcnt", kIsaColorPink);
            prefix_tree_.Insert("vmcnt", kIsaColorPink);
            prefix_tree_.Insert("lgkmcnt", kIsaColorPink);
            prefix_tree_.Insert("s_swap", kIsaColorRed);
            prefix_tree_.Insert("s_branch", kIsaColorRed);
            prefix_tree_.Insert("s_cbranch", kIsaColorRed);
            prefix_tree_.Insert("s_setpc", kIsaColorRed);
            prefix_tree_.Insert("ds_", kIsaColorBlue);
            prefix_tree_.Insert("buffer_", kIsaColorPurple);
            prefix_tree_.Insert("tbuffer_", kIsaColorPurple);
            prefix_tree_.Insert("image_", kIsaColorPurple);
            prefix_tree_.Insert("global_load", kIsaColorPurple);
            prefix_tree_.Insert("idxen", kIsaColorPurple);
            prefix_tree_.Insert("s_", kIsaColorBlue);
            prefix_tree_.Insert("s[", kIsaColorBlue);  // Scalar register.
            prefix_tree_.Insert("[s", kIsaColorBlue);  // Scalar register range.
            prefix_tree_.Insert("|s", kIsaColorBlue);  // Scalar register absolute value.
            prefix_tree_.Insert("-s", kIsaColorBlue);  // Scalar register negative value.
            prefix_tree_.Insert("v_", kIsaColorLightGreen);
            prefix_tree_.Insert("v[", kIsaColorLightGreen);  // Vector register.
            prefix_tree_.Insert("[v", kIsaColorLightGreen);  // Vector register range.
            prefix_tree_.Insert("|v", kIsaColorLightGreen);  // Vector register absolute value.
            prefix_tree_.Insert("-v", kIsaColorLightGreen);  // Vector register negative value.
            prefix_tree_.Insert("//", kIsaColorLightBlue);   // Comments.

            for (size_t i = 0; i <= 9; ++i)
            {
                prefix_tree_.Insert("s" + std::to_string(i), kIsaColorBlue);
                prefix_tree_.Insert("v" + std::to_string(i), kIsaColorLightGreen);
            }
        }
    }  // namespace QtUtils
}  // namespace QtCommon
