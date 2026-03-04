#pragma once

/// @file CodeActionTypes.h
/// @brief V13 Phase 29 — Enhanced code action grouping and preview types.
///
/// Builds on the base CodeActionKind / CodeActionInfo types defined in
/// CodeIntelligenceTypes.h by adding a grouped CodeActionSet for categorized
/// display in the code action menu.

#include "CodeIntelligenceTypes.h"

#include <algorithm>
#include <string>
#include <vector>

namespace markamp::core
{

/// A grouped set of code actions organized by category for menu display.
///
/// Usage:
/// ```cpp
/// CodeActionSet set;
/// set.add(action);
/// auto all = set.all_actions(); // preferred first
/// ```
struct CodeActionSet
{
    std::vector<CodeActionInfo> quick_fixes; ///< Diagnostic fix actions (kQuickFix)
    std::vector<CodeActionInfo>
        refactors; ///< Refactoring actions (kRefactor, kRefactorExtract, kRefactorInline)
    std::vector<CodeActionInfo> source_actions; ///< Source-level actions (kSource, kSourceFixAll)

    /// Add an action to the appropriate category.
    void add(CodeActionInfo action)
    {
        switch (action.kind)
        {
            case CodeActionKind::kQuickFix:
            {
                quick_fixes.push_back(std::move(action));
                break;
            }
            case CodeActionKind::kRefactor:
            case CodeActionKind::kRefactorExtract:
            case CodeActionKind::kRefactorInline:
            {
                refactors.push_back(std::move(action));
                break;
            }
            case CodeActionKind::kSource:
            case CodeActionKind::kSourceFixAll:
            {
                source_actions.push_back(std::move(action));
                break;
            }
        }
    }

    /// Total number of actions across all categories.
    [[nodiscard]] auto total_count() const -> int
    {
        return static_cast<int>(quick_fixes.size() + refactors.size() + source_actions.size());
    }

    /// Whether any action in the set is marked as preferred.
    [[nodiscard]] auto has_preferred() const -> bool
    {
        return preferred_action() != nullptr;
    }

    /// Get the preferred action, if any. Returns nullptr if none is preferred.
    [[nodiscard]] auto preferred_action() const -> const CodeActionInfo*
    {
        for (const auto& action : quick_fixes)
        {
            if (action.is_preferred)
            {
                return &action;
            }
        }
        for (const auto& action : refactors)
        {
            if (action.is_preferred)
            {
                return &action;
            }
        }
        for (const auto& action : source_actions)
        {
            if (action.is_preferred)
            {
                return &action;
            }
        }
        return nullptr;
    }

    /// Get all actions as a flat list, ordered: preferred first, then
    /// quick fixes, refactors, source actions.
    [[nodiscard]] auto all_actions() const -> std::vector<const CodeActionInfo*>
    {
        std::vector<const CodeActionInfo*> result;
        result.reserve(static_cast<size_t>(total_count()));

        // Preferred first
        const auto* pref = preferred_action();
        if (pref != nullptr)
        {
            result.push_back(pref);
        }

        auto add_if_not_preferred = [&](const CodeActionInfo& action)
        {
            if (&action != pref)
            {
                result.push_back(&action);
            }
        };

        for (const auto& action : quick_fixes)
        {
            add_if_not_preferred(action);
        }
        for (const auto& action : refactors)
        {
            add_if_not_preferred(action);
        }
        for (const auto& action : source_actions)
        {
            add_if_not_preferred(action);
        }

        return result;
    }

    /// Generate a preview showing the diff between original content and the
    /// result of applying an action's edits.
    [[nodiscard]] static auto preview_text(const CodeActionInfo& action,
                                           const std::string& original) -> std::string
    {
        if (action.edits.empty())
            return {};

        std::string preview;
        auto lines = split_lines(original);

        for (const auto& edit : action.edits)
        {
            if (edit.start_line >= 0 && edit.start_line < static_cast<int>(lines.size()))
            {
                preview += "- " + lines[static_cast<size_t>(edit.start_line)] + "\n";
            }
            if (!edit.new_text.empty())
            {
                auto replacement_lines = split_lines(edit.new_text);
                for (const auto& rep_line : replacement_lines)
                {
                    preview += "+ " + rep_line + "\n";
                }
            }
        }
        return preview;
    }

private:
    /// Split a string into lines.
    [[nodiscard]] static auto split_lines(const std::string& text) -> std::vector<std::string>
    {
        std::vector<std::string> result;
        std::string::size_type start = 0;
        while (start < text.size())
        {
            auto pos = text.find('\n', start);
            if (pos == std::string::npos)
            {
                result.push_back(text.substr(start));
                break;
            }
            result.push_back(text.substr(start, pos - start));
            start = pos + 1;
        }
        if (result.empty())
            result.emplace_back();
        return result;
    }
};

} // namespace markamp::core
