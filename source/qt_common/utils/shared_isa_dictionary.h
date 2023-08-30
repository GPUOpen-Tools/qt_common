//=============================================================================
// Copyright (c) 2022 Advanced Micro Devices, Inc. All rights reserved.
/// @author AMD Developer Tools Team
/// @file
/// @brief Declaration for an isa dictionary.
//=============================================================================

#ifndef QTCOMMON_UTILS_SHARED_ISA_DICTIONARY_H_
#define QTCOMMON_UTILS_SHARED_ISA_DICTIONARY_H_

#include <memory>
#include <string>
#include <unordered_map>

#include <QColor>
#include <QtGlobal>

#include "common_definitions.h"

namespace QtCommon
{
    namespace QtUtils
    {
        // Light Theme Colors.
        static const QColor kIsaLightThemeColorLightOrange    = QColor(255, 128, 0);         ///< Light orange.
        static const QColor kIsaLightThemeColorPink           = QColor(255, 0, 128);         ///< Pink.
        static const QColor kIsaLightThemeColorLightPink      = QColor(255, 142, 255, 200);  ///< Light pink.
        static const QColor kIsaLightThemeColorLightLightPink = QColor(255, 200, 255, 200);  ///< Light light pink.
        static const QColor kIsaLightThemeColorRed            = QColor(255, 0, 0);           ///< Red.
        static const QColor kIsaLightThemeColorBlue           = QColor(0, 0, 255);           ///< Blue.
        static const QColor kIsaLightThemeColorLightBlue      = QColor(0, 128, 213);         ///< Light blue.
        static const QColor kIsaLightThemeColorLightGreen     = QColor(0, 128, 0);           ///< Light green.
        static const QColor kIsaLightThemeColorPurple         = QColor(128, 0, 255);         ///< Purple.
        static const QColor kIsaLightThemeColorDarkMagenta    = Qt::darkMagenta;             ///< Dark magenta.

        // Dark theme colors.
        static const QColor kIsaDarkThemeColorLightOrange    = QColor(255, 128, 0);         ///< Light orange.
        static const QColor kIsaDarkThemeColorPink           = QColor(240, 64, 128);        ///< Pink.
        static const QColor kIsaDarkThemeColorDarkDarkPurple = QColor(60, 0, 60, 200);      ///< Dark dark purple.
        static const QColor kIsaDarkThemeColorLightLightPink = QColor(240, 200, 240, 200);  ///< Light light pink.
        static const QColor kIsaDarkThemeColorRed            = QColor(240, 64, 64);         ///< Red.
        static const QColor kIsaDarkThemeColorBlue           = QColor(64, 96, 220);         ///< Blue.
        static const QColor kIsaDarkThemeColorLightBlue      = QColor(64, 164, 212);        ///< Light blue.
        static const QColor kIsaDarkThemeColorLightGreen     = QColor(32, 142, 32);         ///< Light green.
        static const QColor kIsaDarkThemeColorPurple         = QColor(164, 64, 240);        ///< Purple.
        static const QColor kIsaDarkThemeColorDarkMagenta    = QColor(142, 64, 142);        ///< Dark magenta.

        /// @brief Class defining a prefix tree node.
        template <typename UserType>
        class PrefixTreeNode
        {
        public:
            /// @brief Constructor.
            ///
            /// @param [in] data The character to store in this node.
            inline PrefixTreeNode(const char data)
                : data_(data)
                , is_leaf_node_(false)
            {
            }

            char                                                      data_;          ///< The character this node represents.
            bool                                                      is_leaf_node_;  ///< If current node is a leaf node.
            std::unordered_map<char, std::shared_ptr<PrefixTreeNode>> child_nodes_;   ///< Map to the child nodes.
            UserType                                                  user_data_;     ///< User defined data to record status of each prefix string.
        };

        /// @brief Class defining a prefix tree.
        template <typename UserType>
        class PrefixTree
        {
        public:
            /// @brief Constructor.
            inline PrefixTree()
            {
                root_ = std::make_shared<PrefixTreeNode<UserType>>('\0');
            }

            /// @brief Insert a string with user data into the tree.
            ///
            /// @param [in] str       String to insert.
            /// @param [in] user_data User defined data indicating the status of prefix string.
            inline void Insert(const std::string& str, const UserType& user_data)
            {
                if (!str.empty())
                {
                    std::shared_ptr<PrefixTreeNode<UserType>> current_node = root_;

                    Q_ASSERT_X(current_node != nullptr, "PrefixTree Insert", "tree node nullptr");

                    if (current_node == nullptr)
                    {
                        return;
                    }

                    for (const auto& c : str)
                    {
                        if (current_node->child_nodes_.find(c) == current_node->child_nodes_.end())
                        {
                            current_node->child_nodes_[c] = std::make_shared<PrefixTreeNode<UserType>>(c);
                            current_node                  = current_node->child_nodes_[c];
                        }
                        else
                        {
                            current_node = current_node->child_nodes_[c];
                        }

                        Q_ASSERT_X(current_node != nullptr, "PrefixTree Insert", "tree node nullptr");

                        if (current_node == nullptr)
                        {
                            return;
                        }
                    }

                    current_node->is_leaf_node_ = true;
                    current_node->user_data_    = user_data;
                }
            }

            /// @brief Search a string in the tree and return its user-defined data type.
            ///
            /// The search is trying find an exact match of the input string in the tree.
            ///
            /// @param [in] str       Target string to search.
            /// @param [in] user_data Output user data that matches the user-defined string type.
            ///
            /// @return true if an exact match was found.
            inline bool Search(const std::string& str, UserType& user_data)
            {
                if (!str.empty())
                {
                    std::shared_ptr<PrefixTreeNode<UserType>> current_node = root_;

                    Q_ASSERT_X(current_node != nullptr, "PrefixTree Search", "tree node nullptr");

                    if (current_node == nullptr)
                    {
                        return false;
                    }

                    for (const auto& c : str)
                    {
                        if (current_node->child_nodes_.find(c) == current_node->child_nodes_.end())
                        {
                            return false;
                        }
                        else
                        {
                            current_node = current_node->child_nodes_[c];
                        }

                        Q_ASSERT_X(current_node != nullptr, "PrefixTree Search", "tree node nullptr");

                        if (current_node == nullptr)
                        {
                            return false;
                        }
                    }

                    if (current_node != nullptr && current_node->is_leaf_node_)
                    {
                        user_data = current_node->user_data_;
                        return true;
                    }
                }

                return false;
            }

            /// @brief Search the tree to find out if there is a string in the tree that can be the prefix of the given string.
            ///
            /// The search will return when the longest prefix was found and return its user-defined data.
            ///
            /// @param [in]  str       Target string to search.
            /// @param [in]  user_data Output user data that matches the user-defined string type.
            ///
            /// @return true if a prefix was found, if there are multiple matching prefix, it returns the longest one.
            inline bool PrefixFoundInTree(const std::string& str, UserType& user_data) const
            {
                if (!str.empty())
                {
                    std::shared_ptr<PrefixTreeNode<UserType>> current_node   = root_;
                    bool                                      seen_last_leaf = false;
                    UserType                                  last_user_data;

                    Q_ASSERT_X(current_node != nullptr, "PrefixTree PrefixFoundInTree", "tree node nullptr");

                    if (current_node == nullptr)
                    {
                        return false;
                    }

                    for (const auto& c : str)
                    {
                        if (current_node->child_nodes_.find(c) == current_node->child_nodes_.end())
                        {
                            // If we could not find a match of the char in child nodes,
                            // we should check if we have seen a leaf before, if a leaf is already seen,
                            // it means we have found a matching prefix, just record its user data and return.
                            if (seen_last_leaf)
                            {
                                user_data = last_user_data;
                                return true;
                            }
                            else
                            {
                                break;
                            }
                        }
                        else
                        {
                            current_node = current_node->child_nodes_[c];

                            Q_ASSERT_X(current_node != nullptr, "PrefixTree PrefixFoundInTree", "tree node nullptr");

                            if (current_node == nullptr)
                            {
                                return false;
                            }

                            if (current_node->is_leaf_node_)
                            {
                                seen_last_leaf = true;
                                last_user_data = current_node->user_data_;
                            }
                        }
                    }

                    if (current_node != nullptr && current_node->is_leaf_node_)
                    {
                        user_data = current_node->user_data_;
                        return true;
                    }
                }

                return false;
            }

        private:
            std::shared_ptr<PrefixTreeNode<UserType>> root_ = nullptr;  ///< The root node of the tree.
        };

        /// @brief ISA syntax highlighter keyword dictionary.
        class IsaColorCodingDictionaryInstance
        {
        public:
            /// @brief Get current instance.
            ///
            /// @return The current instance.
            static IsaColorCodingDictionaryInstance& GetInstance();

            /// @brief Should the input string be highlighted, if so, record its color info.
            ///
            /// @param [in] str   The input string.
            /// @param [in] color The color info that the string should be highlighted with.
            ///
            /// @return true if the string should be highlighted.
            bool ShouldHighlight(const std::string& str, QColor& color) const;

        private:
            /// @brief Constructor.
            IsaColorCodingDictionaryInstance();

            /// @brief Destructor.
            ~IsaColorCodingDictionaryInstance() = default;

            /// @brief Disable copy constructor.
            IsaColorCodingDictionaryInstance(const IsaColorCodingDictionaryInstance&) = delete;

            /// @brief Disable assignment operator.
            IsaColorCodingDictionaryInstance& operator=(const IsaColorCodingDictionaryInstance&) = delete;

            PrefixTree<QColor> prefix_tree_[kColorThemeTypeCount];  ///< The prefix tree that is used as a dictionary to decide the coloring profile.
        };
    }  // namespace QtUtils
}  // namespace QtCommon

#endif  // QTCOMMON_UTILS_SHARED_ISA_DICTIONARY_H_
