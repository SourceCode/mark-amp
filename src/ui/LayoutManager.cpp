#include "LayoutManager.h"

#include "BreadcrumbBar.h"
#include "EditorPanel.h"
#include "ExtensionsBrowserPanel.h"
#include "FileTreeCtrl.h"
#include "SplitView.h"
#include "SplitterBar.h"
#include "StatusBarPanel.h"
#include "TabBar.h"
#include "ThemeGallery.h"
#include "Toolbar.h"
#include "core/BuiltInPlugins.h"
#include "core/Config.h"
#include "core/Events.h"
#include "core/FeatureRegistry.h"
#include "core/Logger.h"
#include "core/SampleFiles.h"

#include <wx/button.h>
#include <wx/clipbrd.h>
#include <wx/dcbuffer.h>
#include <wx/filedlg.h>
#include <wx/msgdlg.h>
#include <wx/sizer.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <fstream>

namespace markamp::ui
{

LayoutManager::LayoutManager(wxWindow* parent,
                             core::ThemeEngine& theme_engine,
                             core::EventBus& event_bus,
                             core::Config* config,
                             core::FeatureRegistry* feature_registry,
                             core::IMermaidRenderer* mermaid_renderer,
                             core::IMathRenderer* math_renderer)
    : ThemeAwareWindow(
          parent, theme_engine, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
    , event_bus_(event_bus)
    , config_(config)
    , feature_registry_(feature_registry)
    , mermaid_renderer_(mermaid_renderer)
    , math_renderer_(math_renderer)
    , sidebar_anim_timer_(this)
{
    RestoreLayoutState();
    CreateLayout();

    // Subscribe to sidebar toggle events from CustomChrome
    sidebar_toggle_sub_ = event_bus_.subscribe<core::events::SidebarToggleEvent>(
        [this](const core::events::SidebarToggleEvent& /*evt*/) { toggle_sidebar(); });

    // Tab event subscriptions
    tab_switched_sub_ = event_bus_.subscribe<core::events::TabSwitchedEvent>(
        [this](const core::events::TabSwitchedEvent& evt) { SwitchToTab(evt.file_path); });

    tab_close_sub_ = event_bus_.subscribe<core::events::TabCloseRequestEvent>(
        [this](const core::events::TabCloseRequestEvent& evt) { CloseTab(evt.file_path); });

    tab_save_sub_ = event_bus_.subscribe<core::events::TabSaveRequestEvent>(
        [this](const core::events::TabSaveRequestEvent& /*evt*/) { SaveActiveFile(); });

    tab_save_as_sub_ = event_bus_.subscribe<core::events::TabSaveAsRequestEvent>(
        [this](const core::events::TabSaveAsRequestEvent& /*evt*/) { SaveActiveFileAs(); });

    content_changed_sub_ = event_bus_.subscribe<core::events::EditorContentChangedEvent>(
        [this](const core::events::EditorContentChangedEvent& evt)
        {
            if (!active_file_path_.empty())
            {
                auto buf_it = file_buffers_.find(active_file_path_);
                if (buf_it != file_buffers_.end())
                {
                    buf_it->second.content = evt.content;
                    buf_it->second.is_modified = true;
                    if (tab_bar_ != nullptr)
                    {
                        tab_bar_->SetTabModified(active_file_path_, true);
                    }
                }
            }
        });

    // Auto-save timer
    Bind(wxEVT_TIMER, &LayoutManager::OnAutoSaveTimer, this, auto_save_timer_.GetId());

    // Bind animation timer
    Bind(wxEVT_TIMER, &LayoutManager::OnSidebarAnimTimer, this, sidebar_anim_timer_.GetId());

    // Start auto-save
    StartAutoSave();

    // R6 event subscriptions
    find_sub_ = event_bus_.subscribe<core::events::FindRequestEvent>(
        [this](const core::events::FindRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ShowFindBar();
                }
            }
        });

    replace_sub_ = event_bus_.subscribe<core::events::ReplaceRequestEvent>(
        [this](const core::events::ReplaceRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ShowFindBar();
                }
            }
        });

    dup_line_sub_ = event_bus_.subscribe<core::events::DuplicateLineRequestEvent>(
        [this](const core::events::DuplicateLineRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->GetStyledTextCtrl()->LineDuplicate();
                }
            }
        });

    toggle_comment_sub_ = event_bus_.subscribe<core::events::ToggleCommentRequestEvent>(
        [this](const core::events::ToggleCommentRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    auto* stc = editor->GetStyledTextCtrl();
                    const wxString sel = stc->GetSelectedText();
                    if (sel.StartsWith("<!-- ") && sel.EndsWith(" -->"))
                    {
                        // Unwrap comment
                        wxString inner = sel.Mid(5, sel.Len() - 9);
                        stc->ReplaceSelection(inner);
                    }
                    else
                    {
                        // Wrap in comment
                        stc->ReplaceSelection("<!-- " + sel + " -->");
                    }
                }
            }
        });

    delete_line_sub_ = event_bus_.subscribe<core::events::DeleteLineRequestEvent>(
        [this](const core::events::DeleteLineRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->GetStyledTextCtrl()->LineDelete();
                }
            }
        });

    wrap_toggle_sub_ = event_bus_.subscribe<core::events::WrapToggleRequestEvent>(
        [this](const core::events::WrapToggleRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    const auto mode = editor->GetWordWrapMode();
                    editor->SetWordWrap(mode == core::events::WrapMode::None);
                }
            }
        });

    // R7 event subscriptions
    move_line_up_sub_ = event_bus_.subscribe<core::events::MoveLineUpRequestEvent>(
        [this](const core::events::MoveLineUpRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->MoveLineUp();
                }
            }
        });
    move_line_down_sub_ = event_bus_.subscribe<core::events::MoveLineDownRequestEvent>(
        [this](const core::events::MoveLineDownRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->MoveLineDown();
                }
            }
        });
    join_lines_sub_ = event_bus_.subscribe<core::events::JoinLinesRequestEvent>(
        [this](const core::events::JoinLinesRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->JoinLines();
                }
            }
        });
    sort_asc_sub_ = event_bus_.subscribe<core::events::SortLinesAscRequestEvent>(
        [this](const core::events::SortLinesAscRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->SortLinesAscending();
                }
            }
        });
    sort_desc_sub_ = event_bus_.subscribe<core::events::SortLinesDescRequestEvent>(
        [this](const core::events::SortLinesDescRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->SortLinesDescending();
                }
            }
        });
    transform_upper_sub_ = event_bus_.subscribe<core::events::TransformUpperRequestEvent>(
        [this](const core::events::TransformUpperRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->TransformToUppercase();
                }
            }
        });
    transform_lower_sub_ = event_bus_.subscribe<core::events::TransformLowerRequestEvent>(
        [this](const core::events::TransformLowerRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->TransformToLowercase();
                }
            }
        });
    transform_title_sub_ = event_bus_.subscribe<core::events::TransformTitleRequestEvent>(
        [this](const core::events::TransformTitleRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->TransformToTitleCase();
                }
            }
        });
    select_all_occ_sub_ = event_bus_.subscribe<core::events::SelectAllOccurrencesRequestEvent>(
        [this](const core::events::SelectAllOccurrencesRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->SelectAllOccurrences();
                }
            }
        });
    expand_line_sub_ = event_bus_.subscribe<core::events::ExpandLineSelectionRequestEvent>(
        [this](const core::events::ExpandLineSelectionRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ExpandLineSelection();
                }
            }
        });
    insert_line_above_sub_ = event_bus_.subscribe<core::events::InsertLineAboveRequestEvent>(
        [this](const core::events::InsertLineAboveRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->InsertLineAbove();
                }
            }
        });
    insert_line_below_sub_ = event_bus_.subscribe<core::events::InsertLineBelowRequestEvent>(
        [this](const core::events::InsertLineBelowRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->InsertLineBelow();
                }
            }
        });
    fold_all_sub_ = event_bus_.subscribe<core::events::FoldAllRequestEvent>(
        [this](const core::events::FoldAllRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->FoldAllRegions();
                }
            }
        });
    unfold_all_sub_ = event_bus_.subscribe<core::events::UnfoldAllRequestEvent>(
        [this](const core::events::UnfoldAllRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->UnfoldAllRegions();
                }
            }
        });
    toggle_line_numbers_sub_ = event_bus_.subscribe<core::events::ToggleLineNumbersRequestEvent>(
        [this](const core::events::ToggleLineNumbersRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ToggleLineNumbers();
                }
            }
        });
    toggle_whitespace_sub_ = event_bus_.subscribe<core::events::ToggleWhitespaceRequestEvent>(
        [this](const core::events::ToggleWhitespaceRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ToggleRenderWhitespace();
                }
            }
        });

    // R8 event subscriptions
    copy_line_up_sub_ = event_bus_.subscribe<core::events::CopyLineUpRequestEvent>(
        [this](const core::events::CopyLineUpRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->CopyLineUp();
                }
            }
        });
    copy_line_down_sub_ = event_bus_.subscribe<core::events::CopyLineDownRequestEvent>(
        [this](const core::events::CopyLineDownRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->CopyLineDown();
                }
            }
        });
    delete_all_left_sub_ = event_bus_.subscribe<core::events::DeleteAllLeftRequestEvent>(
        [this](const core::events::DeleteAllLeftRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->DeleteAllLeft();
                }
            }
        });
    delete_all_right_sub_ = event_bus_.subscribe<core::events::DeleteAllRightRequestEvent>(
        [this](const core::events::DeleteAllRightRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->DeleteAllRight();
                }
            }
        });
    reverse_lines_sub_ = event_bus_.subscribe<core::events::ReverseLinesRequestEvent>(
        [this](const core::events::ReverseLinesRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ReverseSelectedLines();
                }
            }
        });
    delete_dup_lines_sub_ = event_bus_.subscribe<core::events::DeleteDuplicateLinesRequestEvent>(
        [this](const core::events::DeleteDuplicateLinesRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->DeleteDuplicateLines();
                }
            }
        });
    transpose_chars_sub_ = event_bus_.subscribe<core::events::TransposeCharsRequestEvent>(
        [this](const core::events::TransposeCharsRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->TransposeCharacters();
                }
            }
        });
    indent_selection_sub_ = event_bus_.subscribe<core::events::IndentSelectionRequestEvent>(
        [this](const core::events::IndentSelectionRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->IndentSelection();
                }
            }
        });
    outdent_selection_sub_ = event_bus_.subscribe<core::events::OutdentSelectionRequestEvent>(
        [this](const core::events::OutdentSelectionRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->OutdentSelection();
                }
            }
        });
    select_word_sub_ = event_bus_.subscribe<core::events::SelectWordRequestEvent>(
        [this](const core::events::SelectWordRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->SelectWordAtCursor();
                }
            }
        });
    select_paragraph_sub_ = event_bus_.subscribe<core::events::SelectParagraphRequestEvent>(
        [this](const core::events::SelectParagraphRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->SelectCurrentParagraph();
                }
            }
        });
    toggle_read_only_sub_ = event_bus_.subscribe<core::events::ToggleReadOnlyRequestEvent>(
        [this](const core::events::ToggleReadOnlyRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ToggleReadOnly();
                }
            }
        });
    convert_indent_spaces_sub_ =
        event_bus_.subscribe<core::events::ConvertIndentSpacesRequestEvent>(
            [this](const core::events::ConvertIndentSpacesRequestEvent& /*evt*/)
            {
                if (split_view_ != nullptr)
                {
                    auto* editor = split_view_->GetEditorPanel();
                    if (editor != nullptr)
                    {
                        editor->ConvertIndentationToSpaces();
                    }
                }
            });
    convert_indent_tabs_sub_ = event_bus_.subscribe<core::events::ConvertIndentTabsRequestEvent>(
        [this](const core::events::ConvertIndentTabsRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ConvertIndentationToTabs();
                }
            }
        });
    jump_to_bracket_sub_ = event_bus_.subscribe<core::events::JumpToBracketRequestEvent>(
        [this](const core::events::JumpToBracketRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->JumpToMatchingBracket();
                }
            }
        });
    toggle_minimap_sub_ = event_bus_.subscribe<core::events::ToggleMinimapRequestEvent>(
        [this](const core::events::ToggleMinimapRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ToggleMinimapVisibility();
                }
            }
        });
    fold_current_sub_ = event_bus_.subscribe<core::events::FoldCurrentRequestEvent>(
        [this](const core::events::FoldCurrentRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->FoldCurrentRegion();
                }
            }
        });
    unfold_current_sub_ = event_bus_.subscribe<core::events::UnfoldCurrentRequestEvent>(
        [this](const core::events::UnfoldCurrentRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->UnfoldCurrentRegion();
                }
            }
        });
    add_line_comment_sub_ = event_bus_.subscribe<core::events::AddLineCommentRequestEvent>(
        [this](const core::events::AddLineCommentRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->AddLineComment();
                }
            }
        });
    remove_line_comment_sub_ = event_bus_.subscribe<core::events::RemoveLineCommentRequestEvent>(
        [this](const core::events::RemoveLineCommentRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->RemoveLineComment();
                }
            }
        });

    // ── R9 subscriptions ──
    trim_trailing_ws_sub_ = event_bus_.subscribe<core::events::TrimTrailingWSRequestEvent>(
        [this](const core::events::TrimTrailingWSRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->TrimTrailingWhitespaceNow();
                }
            }
        });

    expand_selection_sub_ = event_bus_.subscribe<core::events::ExpandSelectionRequestEvent>(
        [this](const core::events::ExpandSelectionRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ExpandSelection();
                }
            }
        });

    shrink_selection_sub_ = event_bus_.subscribe<core::events::ShrinkSelectionRequestEvent>(
        [this](const core::events::ShrinkSelectionRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ShrinkSelection();
                }
            }
        });

    cursor_undo_sub_ = event_bus_.subscribe<core::events::CursorUndoRequestEvent>(
        [this](const core::events::CursorUndoRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->CursorUndo();
                }
            }
        });

    cursor_redo_sub_ = event_bus_.subscribe<core::events::CursorRedoRequestEvent>(
        [this](const core::events::CursorRedoRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->CursorRedo();
                }
            }
        });

    move_text_left_sub_ = event_bus_.subscribe<core::events::MoveTextLeftRequestEvent>(
        [this](const core::events::MoveTextLeftRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->MoveSelectedTextLeft();
                }
            }
        });

    move_text_right_sub_ = event_bus_.subscribe<core::events::MoveTextRightRequestEvent>(
        [this](const core::events::MoveTextRightRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->MoveSelectedTextRight();
                }
            }
        });

    toggle_auto_indent_sub_ = event_bus_.subscribe<core::events::ToggleAutoIndentRequestEvent>(
        [this](const core::events::ToggleAutoIndentRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ToggleAutoIndent();
                }
            }
        });

    toggle_bracket_matching_sub_ =
        event_bus_.subscribe<core::events::ToggleBracketMatchingRequestEvent>(
            [this](const core::events::ToggleBracketMatchingRequestEvent& /*evt*/)
            {
                if (split_view_ != nullptr)
                {
                    auto* editor = split_view_->GetEditorPanel();
                    if (editor != nullptr)
                    {
                        editor->ToggleBracketMatching();
                    }
                }
            });

    toggle_code_folding_sub_ = event_bus_.subscribe<core::events::ToggleCodeFoldingRequestEvent>(
        [this](const core::events::ToggleCodeFoldingRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ToggleCodeFolding();
                }
            }
        });

    toggle_indent_guides_sub_ = event_bus_.subscribe<core::events::ToggleIndentGuidesRequestEvent>(
        [this](const core::events::ToggleIndentGuidesRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ToggleIndentationGuides();
                }
            }
        });

    select_to_bracket_sub_ = event_bus_.subscribe<core::events::SelectToBracketRequestEvent>(
        [this](const core::events::SelectToBracketRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->SelectToMatchingBracket();
                }
            }
        });

    toggle_block_comment_sub_ = event_bus_.subscribe<core::events::ToggleBlockCommentRequestEvent>(
        [this](const core::events::ToggleBlockCommentRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ToggleBlockComment();
                }
            }
        });

    insert_datetime_sub_ = event_bus_.subscribe<core::events::InsertDateTimeRequestEvent>(
        [this](const core::events::InsertDateTimeRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->InsertDateTime();
                }
            }
        });

    bold_sub_ = event_bus_.subscribe<core::events::BoldRequestEvent>(
        [this](const core::events::BoldRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ToggleBold();
                }
            }
        });

    italic_sub_ = event_bus_.subscribe<core::events::ItalicRequestEvent>(
        [this](const core::events::ItalicRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ToggleItalic();
                }
            }
        });

    inline_code_sub_ = event_bus_.subscribe<core::events::InlineCodeRequestEvent>(
        [this](const core::events::InlineCodeRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ToggleInlineCode();
                }
            }
        });

    blockquote_sub_ = event_bus_.subscribe<core::events::BlockquoteRequestEvent>(
        [this](const core::events::BlockquoteRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->InsertBlockquote();
                }
            }
        });

    cycle_heading_sub_ = event_bus_.subscribe<core::events::CycleHeadingRequestEvent>(
        [this](const core::events::CycleHeadingRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->CycleHeading();
                }
            }
        });

    insert_table_sub_ = event_bus_.subscribe<core::events::InsertTableRequestEvent>(
        [this](const core::events::InsertTableRequestEvent& /*evt*/)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->InsertTable();
                }
            }
        });

    // ── R10 subscriptions ──
    toggle_smart_list_sub_ =
        event_bus_.subscribe<core::events::ToggleSmartListContinuationRequestEvent>(
            [this](
                [[maybe_unused]] const core::events::ToggleSmartListContinuationRequestEvent& evt)
            {
                if (split_view_ != nullptr)
                {
                    auto* editor = split_view_->GetEditorPanel();
                    if (editor != nullptr)
                    {
                        editor->SetSmartListContinuation(!editor->GetSmartListContinuation());
                    }
                }
            });

    close_other_tabs_sub_ = event_bus_.subscribe<core::events::CloseOtherTabsRequestEvent>(
        [this]([[maybe_unused]] const core::events::CloseOtherTabsRequestEvent& evt)
        {
            if (tab_bar_ != nullptr && !active_file_path_.empty())
            {
                tab_bar_->CloseOtherTabs(active_file_path_);
            }
        });

    close_saved_tabs_sub_ = event_bus_.subscribe<core::events::CloseSavedTabsRequestEvent>(
        [this]([[maybe_unused]] const core::events::CloseSavedTabsRequestEvent& evt)
        {
            if (tab_bar_ != nullptr)
            {
                tab_bar_->CloseSavedTabs();
            }
        });

    insert_link_sub_ = event_bus_.subscribe<core::events::InsertLinkRequestEvent>(
        [this]([[maybe_unused]] const core::events::InsertLinkRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->InsertLink();
                }
            }
        });

    add_cursor_below_sub_ = event_bus_.subscribe<core::events::AddCursorBelowRequestEvent>(
        [this]([[maybe_unused]] const core::events::AddCursorBelowRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->AddCursorBelow();
                }
            }
        });

    add_cursor_above_sub_ = event_bus_.subscribe<core::events::AddCursorAboveRequestEvent>(
        [this]([[maybe_unused]] const core::events::AddCursorAboveRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->AddCursorAbove();
                }
            }
        });

    add_cursor_next_occurrence_sub_ =
        event_bus_.subscribe<core::events::AddCursorNextOccurrenceRequestEvent>(
            [this]([[maybe_unused]] const core::events::AddCursorNextOccurrenceRequestEvent& evt)
            {
                if (split_view_ != nullptr)
                {
                    auto* editor = split_view_->GetEditorPanel();
                    if (editor != nullptr)
                    {
                        editor->AddCursorAtNextOccurrence();
                    }
                }
            });

    remove_surrounding_brackets_sub_ =
        event_bus_.subscribe<core::events::RemoveSurroundingBracketsRequestEvent>(
            [this]([[maybe_unused]] const core::events::RemoveSurroundingBracketsRequestEvent& evt)
            {
                if (split_view_ != nullptr)
                {
                    auto* editor = split_view_->GetEditorPanel();
                    if (editor != nullptr)
                    {
                        editor->RemoveSurroundingBrackets();
                    }
                }
            });

    duplicate_selection_or_line_sub_ =
        event_bus_.subscribe<core::events::DuplicateSelectionOrLineRequestEvent>(
            [this]([[maybe_unused]] const core::events::DuplicateSelectionOrLineRequestEvent& evt)
            {
                if (split_view_ != nullptr)
                {
                    auto* editor = split_view_->GetEditorPanel();
                    if (editor != nullptr)
                    {
                        editor->DuplicateSelectionOrLine();
                    }
                }
            });

    show_table_editor_sub_ = event_bus_.subscribe<core::events::ShowTableEditorRequestEvent>(
        [this]([[maybe_unused]] const core::events::ShowTableEditorRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ShowTableEditor();
                }
            }
        });

    toggle_scroll_beyond_sub_ =
        event_bus_.subscribe<core::events::ToggleScrollBeyondLastLineRequestEvent>(
            [this]([[maybe_unused]] const core::events::ToggleScrollBeyondLastLineRequestEvent& evt)
            {
                if (split_view_ != nullptr)
                {
                    auto* editor = split_view_->GetEditorPanel();
                    if (editor != nullptr)
                    {
                        editor->SetScrollBeyondLastLine(!editor->GetScrollBeyondLastLine());
                    }
                }
            });

    toggle_highlight_line_sub_ =
        event_bus_.subscribe<core::events::ToggleHighlightCurrentLineRequestEvent>(
            [this]([[maybe_unused]] const core::events::ToggleHighlightCurrentLineRequestEvent& evt)
            {
                if (split_view_ != nullptr)
                {
                    auto* editor = split_view_->GetEditorPanel();
                    if (editor != nullptr)
                    {
                        editor->SetHighlightCurrentLine(!editor->GetHighlightCurrentLine());
                    }
                }
            });

    toggle_auto_close_brackets_sub_ =
        event_bus_.subscribe<core::events::ToggleAutoClosingBracketsRequestEvent>(
            [this]([[maybe_unused]] const core::events::ToggleAutoClosingBracketsRequestEvent& evt)
            {
                if (split_view_ != nullptr)
                {
                    auto* editor = split_view_->GetEditorPanel();
                    if (editor != nullptr)
                    {
                        editor->SetAutoClosingBrackets(!editor->GetAutoClosingBrackets());
                    }
                }
            });

    toggle_sticky_scroll_sub_ = event_bus_.subscribe<core::events::ToggleStickyScrollRequestEvent>(
        [this]([[maybe_unused]] const core::events::ToggleStickyScrollRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->SetStickyScrollEnabled(!editor->GetStickyScrollEnabled());
                }
            }
        });

    toggle_font_ligatures_sub_ =
        event_bus_.subscribe<core::events::ToggleFontLigaturesRequestEvent>(
            [this]([[maybe_unused]] const core::events::ToggleFontLigaturesRequestEvent& evt)
            {
                if (split_view_ != nullptr)
                {
                    auto* editor = split_view_->GetEditorPanel();
                    if (editor != nullptr)
                    {
                        editor->SetFontLigatures(!editor->GetFontLigatures());
                    }
                }
            });

    toggle_smooth_caret_sub_ = event_bus_.subscribe<core::events::ToggleSmoothCaretRequestEvent>(
        [this]([[maybe_unused]] const core::events::ToggleSmoothCaretRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->SetSmoothCaret(!editor->GetSmoothCaret());
                }
            }
        });

    toggle_color_preview_sub_ =
        event_bus_.subscribe<core::events::ToggleInlineColorPreviewRequestEvent>(
            [this]([[maybe_unused]] const core::events::ToggleInlineColorPreviewRequestEvent& evt)
            {
                if (split_view_ != nullptr)
                {
                    auto* editor = split_view_->GetEditorPanel();
                    if (editor != nullptr)
                    {
                        editor->SetInlineColorPreview(!editor->GetInlineColorPreview());
                    }
                }
            });

    toggle_edge_ruler_sub_ = event_bus_.subscribe<core::events::ToggleEdgeColumnRulerRequestEvent>(
        [this]([[maybe_unused]] const core::events::ToggleEdgeColumnRulerRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->SetShowEdgeColumnRuler(!editor->GetShowEdgeColumnRuler());
                }
            }
        });

    ensure_final_newline_sub_ = event_bus_.subscribe<core::events::EnsureFinalNewlineRequestEvent>(
        [this]([[maybe_unused]] const core::events::EnsureFinalNewlineRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->EnsureFinalNewline();
                }
            }
        });

    insert_snippet_sub_ = event_bus_.subscribe<core::events::InsertSnippetRequestEvent>(
        [this]([[maybe_unused]] const core::events::InsertSnippetRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    EditorPanel::Snippet default_snippet{"Snippet", "", "$0"};
                    editor->InsertSnippet(default_snippet);
                }
            }
        });

    // ── R11 subscriptions ──
    toggle_smooth_scrolling_sub_ =
        event_bus_.subscribe<core::events::ToggleSmoothScrollingRequestEvent>(
            [this]([[maybe_unused]] const core::events::ToggleSmoothScrollingRequestEvent& evt)
            {
                if (split_view_ != nullptr)
                {
                    auto* editor = split_view_->GetEditorPanel();
                    if (editor != nullptr)
                    {
                        editor->SetSmoothScrolling(!editor->GetSmoothScrolling());
                    }
                }
            });

    toggle_trailing_ws_sub_ =
        event_bus_.subscribe<core::events::ToggleTrailingWSHighlightRequestEvent>(
            [this]([[maybe_unused]] const core::events::ToggleTrailingWSHighlightRequestEvent& evt)
            {
                if (split_view_ != nullptr)
                {
                    auto* editor = split_view_->GetEditorPanel();
                    if (editor != nullptr)
                    {
                        editor->SetTrailingWhitespace(!editor->GetTrailingWhitespace());
                    }
                }
            });

    toggle_auto_trim_ws_sub_ = event_bus_.subscribe<core::events::ToggleAutoTrimWSRequestEvent>(
        [this]([[maybe_unused]] const core::events::ToggleAutoTrimWSRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->SetAutoTrimTrailingWhitespace(!editor->GetAutoTrimTrailingWhitespace());
                }
            }
        });

    toggle_gutter_separator_sub_ =
        event_bus_.subscribe<core::events::ToggleGutterSeparatorRequestEvent>(
            [this]([[maybe_unused]] const core::events::ToggleGutterSeparatorRequestEvent& evt)
            {
                if (split_view_ != nullptr)
                {
                    auto* editor = split_view_->GetEditorPanel();
                    if (editor != nullptr)
                    {
                        editor->SetGutterSeparator(!editor->GetGutterSeparator());
                    }
                }
            });

    toggle_insert_final_newline_sub_ =
        event_bus_.subscribe<core::events::ToggleInsertFinalNewlineRequestEvent>(
            [this]([[maybe_unused]] const core::events::ToggleInsertFinalNewlineRequestEvent& evt)
            {
                if (split_view_ != nullptr)
                {
                    auto* editor = split_view_->GetEditorPanel();
                    if (editor != nullptr)
                    {
                        editor->SetInsertFinalNewline(!editor->GetInsertFinalNewline());
                    }
                }
            });

    toggle_whitespace_boundary_sub_ =
        event_bus_.subscribe<core::events::ToggleWhitespaceBoundaryRequestEvent>(
            [this]([[maybe_unused]] const core::events::ToggleWhitespaceBoundaryRequestEvent& evt)
            {
                if (split_view_ != nullptr)
                {
                    auto* editor = split_view_->GetEditorPanel();
                    if (editor != nullptr)
                    {
                        editor->SetWhitespaceBoundary(!editor->GetWhitespaceBoundary());
                    }
                }
            });

    toggle_link_auto_complete_sub_ =
        event_bus_.subscribe<core::events::ToggleLinkAutoCompleteRequestEvent>(
            [this]([[maybe_unused]] const core::events::ToggleLinkAutoCompleteRequestEvent& evt)
            {
                if (split_view_ != nullptr)
                {
                    auto* editor = split_view_->GetEditorPanel();
                    if (editor != nullptr)
                    {
                        editor->SetLinkAutoComplete(!editor->GetLinkAutoComplete());
                    }
                }
            });

    toggle_drag_drop_sub_ = event_bus_.subscribe<core::events::ToggleDragDropRequestEvent>(
        [this]([[maybe_unused]] const core::events::ToggleDragDropRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->SetDragDropEnabled(!editor->GetDragDropEnabled());
                }
            }
        });

    toggle_auto_save_sub_ = event_bus_.subscribe<core::events::ToggleAutoSaveRequestEvent>(
        [this]([[maybe_unused]] const core::events::ToggleAutoSaveRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->SetAutoSave(!editor->GetAutoSave());
                }
            }
        });

    toggle_empty_sel_clipboard_sub_ =
        event_bus_.subscribe<core::events::ToggleEmptySelClipboardRequestEvent>(
            [this]([[maybe_unused]] const core::events::ToggleEmptySelClipboardRequestEvent& evt)
            {
                if (split_view_ != nullptr)
                {
                    auto* editor = split_view_->GetEditorPanel();
                    if (editor != nullptr)
                    {
                        editor->SetEmptySelectionClipboard(!editor->GetEmptySelectionClipboard());
                    }
                }
            });

    cycle_render_whitespace_sub_ =
        event_bus_.subscribe<core::events::CycleRenderWhitespaceRequestEvent>(
            [this]([[maybe_unused]] const core::events::CycleRenderWhitespaceRequestEvent& evt)
            {
                if (split_view_ != nullptr)
                {
                    auto* editor = split_view_->GetEditorPanel();
                    if (editor != nullptr)
                    {
                        editor->ToggleRenderWhitespace();
                    }
                }
            });

    delete_current_line_sub_ = event_bus_.subscribe<core::events::DeleteLineRequestEvent>(
        [this]([[maybe_unused]] const core::events::DeleteLineRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->DeleteCurrentLine();
                }
            }
        });

    copy_line_no_sel_sub_ = event_bus_.subscribe<core::events::CopyLineNoSelRequestEvent>(
        [this]([[maybe_unused]] const core::events::CopyLineNoSelRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->CopyLineIfNoSelection();
                }
            }
        });

    add_sel_next_match_sub_ = event_bus_.subscribe<core::events::AddSelNextMatchRequestEvent>(
        [this]([[maybe_unused]] const core::events::AddSelNextMatchRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->AddSelectionToNextFindMatch();
                }
            }
        });

    smart_backspace_sub_ = event_bus_.subscribe<core::events::SmartBackspaceRequestEvent>(
        [this]([[maybe_unused]] const core::events::SmartBackspaceRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->SmartBackspace();
                }
            }
        });

    hide_table_editor_sub_ = event_bus_.subscribe<core::events::HideTableEditorRequestEvent>(
        [this]([[maybe_unused]] const core::events::HideTableEditorRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->HideTableEditor();
                }
            }
        });

    auto_pair_bold_sub_ = event_bus_.subscribe<core::events::AutoPairBoldRequestEvent>(
        [this]([[maybe_unused]] const core::events::AutoPairBoldRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->AutoPairEmphasis('*');
                }
            }
        });

    auto_pair_italic_sub_ = event_bus_.subscribe<core::events::AutoPairItalicRequestEvent>(
        [this]([[maybe_unused]] const core::events::AutoPairItalicRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->AutoPairEmphasis('_');
                }
            }
        });

    auto_pair_code_sub_ = event_bus_.subscribe<core::events::AutoPairCodeRequestEvent>(
        [this]([[maybe_unused]] const core::events::AutoPairCodeRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->AutoPairEmphasis('`');
                }
            }
        });

    toggle_minimap_r11_sub_ = event_bus_.subscribe<core::events::ToggleMinimapRequestEvent>(
        [this]([[maybe_unused]] const core::events::ToggleMinimapRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ToggleMinimap();
                }
            }
        });

    // ── R12 subscriptions ──
    close_tabs_to_left_sub_ = event_bus_.subscribe<core::events::CloseTabsToLeftRequestEvent>(
        [this]([[maybe_unused]] const core::events::CloseTabsToLeftRequestEvent& evt)
        {
            if (tab_bar_ != nullptr)
            {
                tab_bar_->CloseTabsToLeft(active_file_path_);
            }
        });

    close_tabs_to_right_sub_ = event_bus_.subscribe<core::events::CloseTabsToRightRequestEvent>(
        [this]([[maybe_unused]] const core::events::CloseTabsToRightRequestEvent& evt)
        {
            if (tab_bar_ != nullptr)
            {
                tab_bar_->CloseTabsToRight(active_file_path_);
            }
        });

    pin_tab_sub_ = event_bus_.subscribe<core::events::PinTabRequestEvent>(
        [this]([[maybe_unused]] const core::events::PinTabRequestEvent& evt)
        {
            if (tab_bar_ != nullptr)
            {
                tab_bar_->PinTab(active_file_path_);
            }
        });

    unpin_tab_sub_ = event_bus_.subscribe<core::events::UnpinTabRequestEvent>(
        [this]([[maybe_unused]] const core::events::UnpinTabRequestEvent& evt)
        {
            if (tab_bar_ != nullptr)
            {
                tab_bar_->UnpinTab(active_file_path_);
            }
        });

    // ── R13 subscriptions ──
    copy_file_path_sub_ = event_bus_.subscribe<core::events::CopyFilePathRequestEvent>(
        [this]([[maybe_unused]] const core::events::CopyFilePathRequestEvent& evt)
        {
            if (!active_file_path_.empty() && wxTheClipboard->Open())
            {
                wxTheClipboard->SetData(new wxTextDataObject(active_file_path_));
                wxTheClipboard->Close();
            }
        });

    reveal_in_finder_sub_ = event_bus_.subscribe<core::events::RevealInFinderRequestEvent>(
        [this]([[maybe_unused]] const core::events::RevealInFinderRequestEvent& evt)
        {
            if (!active_file_path_.empty())
            {
#ifdef __APPLE__
                wxExecute(wxString::Format("open -R \"%s\"", active_file_path_));
#elif defined(__linux__)
                wxExecute(wxString::Format(
                    "xdg-open \"%s\"",
                    std::filesystem::path(active_file_path_).parent_path().string()));
#endif
            }
        });

    zoom_in_sub_ = event_bus_.subscribe<core::events::ZoomInRequestEvent>(
        [this]([[maybe_unused]] const core::events::ZoomInRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ZoomIn();
                }
            }
        });

    zoom_out_sub_ = event_bus_.subscribe<core::events::ZoomOutRequestEvent>(
        [this]([[maybe_unused]] const core::events::ZoomOutRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ZoomOut();
                }
            }
        });

    zoom_reset_sub_ = event_bus_.subscribe<core::events::ZoomResetRequestEvent>(
        [this]([[maybe_unused]] const core::events::ZoomResetRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ZoomReset();
                }
            }
        });

    convert_eol_lf_sub_ = event_bus_.subscribe<core::events::ConvertEolLfRequestEvent>(
        [this]([[maybe_unused]] const core::events::ConvertEolLfRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ConvertEolToLf();
                    if (statusbar_panel_ != nullptr)
                    {
                        statusbar_panel_->set_eol_mode("LF");
                    }
                }
            }
        });

    convert_eol_crlf_sub_ = event_bus_.subscribe<core::events::ConvertEolCrlfRequestEvent>(
        [this]([[maybe_unused]] const core::events::ConvertEolCrlfRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ConvertEolToCrlf();
                    if (statusbar_panel_ != nullptr)
                    {
                        statusbar_panel_->set_eol_mode("CRLF");
                    }
                }
            }
        });

    // ── R14 subscriptions ──
    fold_current_sub_ = event_bus_.subscribe<core::events::FoldCurrentRequestEvent>(
        [this]([[maybe_unused]] const core::events::FoldCurrentRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->FoldCurrentRegion();
                }
            }
        });

    unfold_current_sub_ = event_bus_.subscribe<core::events::UnfoldCurrentRequestEvent>(
        [this]([[maybe_unused]] const core::events::UnfoldCurrentRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->UnfoldCurrentRegion();
                }
            }
        });

    jump_to_bracket_sub_ = event_bus_.subscribe<core::events::JumpToBracketRequestEvent>(
        [this]([[maybe_unused]] const core::events::JumpToBracketRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->JumpToMatchingBracket();
                }
            }
        });

    transpose_chars_sub_ = event_bus_.subscribe<core::events::TransposeCharsRequestEvent>(
        [this]([[maybe_unused]] const core::events::TransposeCharsRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->TransposeCharacters();
                }
            }
        });

    reverse_lines_sub_ = event_bus_.subscribe<core::events::ReverseLinesRequestEvent>(
        [this]([[maybe_unused]] const core::events::ReverseLinesRequestEvent& evt)
        {
            if (split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    editor->ReverseSelectedLines();
                }
            }
        });

    file_reload_sub_ = event_bus_.subscribe<core::events::FileReloadRequestEvent>(
        [this]([[maybe_unused]] const core::events::FileReloadRequestEvent& evt)
        {
            if (!active_file_path_.empty() && split_view_ != nullptr)
            {
                auto* editor = split_view_->GetEditorPanel();
                if (editor != nullptr)
                {
                    std::ifstream file_stream(active_file_path_);
                    if (file_stream.is_open())
                    {
                        std::stringstream buf;
                        buf << file_stream.rdbuf();
                        editor->SetContent(buf.str());
                    }
                }
            }
        });

    MARKAMP_LOG_INFO(
        "LayoutManager created (sidebar={}px, visible={})", sidebar_width_, sidebar_visible_);

    // Phase 8: Subscribe to sidebar mode switching events
    show_extensions_sub_ = event_bus_.subscribe<core::events::ShowExtensionsBrowserRequestEvent>(
        [this]([[maybe_unused]] const core::events::ShowExtensionsBrowserRequestEvent& evt)
        {
            SetSidebarMode(SidebarMode::kExtensions);
            if (!sidebar_visible_)
            {
                set_sidebar_visible(true);
            }
        });

    show_explorer_sub_ = event_bus_.subscribe<core::events::ShowExplorerRequestEvent>(
        [this]([[maybe_unused]] const core::events::ShowExplorerRequestEvent& evt)
        {
            SetSidebarMode(SidebarMode::kExplorer);
            if (!sidebar_visible_)
            {
                set_sidebar_visible(true);
            }
        });

    // Phase 9: Subscribe to feature toggle events for dynamic show/hide
    feature_toggled_sub_ = event_bus_.subscribe<core::events::FeatureToggledEvent>(
        [this](const core::events::FeatureToggledEvent& evt)
        {
            if (evt.feature_id == core::builtin_features::kBreadcrumb)
            {
                if (breadcrumb_bar_ != nullptr)
                {
                    breadcrumb_bar_->Show(evt.enabled);
                    content_panel_->Layout();
                }
            }
            // Phase 4: Mermaid toggle — forward to SplitView/PreviewPanel
            else if (evt.feature_id == core::builtin_features::kMermaid)
            {
                if (split_view_ != nullptr)
                {
                    split_view_->set_mermaid_enabled(evt.enabled);
                }
            }
            // Phase 4: ThemeGallery toggle is handled at click-time (no widget to hide)
            MARKAMP_LOG_INFO(
                "Feature toggled: {} = {}", evt.feature_id, evt.enabled ? "on" : "off");
        });
}

void LayoutManager::SaveFile(const std::string& path)
{
    if (split_view_)
    {
        split_view_->SaveFile(path);
    }
}

void LayoutManager::CreateLayout()
{
    // --- Sidebar panel ---
    sidebar_panel_ = new wxPanel(this, wxID_ANY);
    sidebar_panel_->SetBackgroundStyle(wxBG_STYLE_PAINT);
    sidebar_panel_->Bind(wxEVT_PAINT, &LayoutManager::OnSidebarPaint, this);

    // Sidebar internal layout: header + content + footer
    auto* sidebar_sizer = new wxBoxSizer(wxVERTICAL);

    // Header: "EXPLORER"
    auto* header_panel = new wxPanel(sidebar_panel_, wxID_ANY, wxDefaultPosition, wxSize(-1, 40));
    header_panel->SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgHeader));

    // Fix 11: Render "EXPLORER" label in header
    auto* header_sizer = new wxBoxSizer(wxHORIZONTAL);
    header_label_ = new wxStaticText(header_panel, wxID_ANY, "EXPLORER");
    header_label_->SetFont(
        theme_engine().font(core::ThemeFontToken::MonoRegular).Bold().Scaled(0.85f));
    header_label_->SetForegroundColour(theme_engine().color(core::ThemeColorToken::TextMuted));
    header_sizer->AddSpacer(12);
    header_sizer->Add(header_label_, 0, wxALIGN_CENTER_VERTICAL);
    header_sizer->AddStretchSpacer();

    // R4 Fix 15: Collapse All button in sidebar header
    auto* collapse_btn = new wxButton(
        header_panel, wxID_ANY, "\xE2\x96\xBE", wxDefaultPosition, wxSize(28, 28), wxBORDER_NONE);
    collapse_btn->SetToolTip("Collapse All");
    collapse_btn->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(0.85f));
    collapse_btn->SetForegroundColour(theme_engine().color(core::ThemeColorToken::TextMuted));
    collapse_btn->SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgHeader));
    collapse_btn->Bind(wxEVT_BUTTON,
                       [this](wxCommandEvent& /*evt*/)
                       {
                           if (file_tree_ != nullptr)
                           {
                               file_tree_->CollapseAllNodes();
                           }
                       });
    header_sizer->Add(collapse_btn, 0, wxALIGN_CENTER_VERTICAL);
    header_sizer->AddSpacer(4);

    header_panel->SetSizer(header_sizer);

    sidebar_sizer->Add(header_panel, 0, wxEXPAND);

    // R5 Fix 7: Use wxSearchCtrl for built-in clear/cancel button
    // Phase 8: Wrap explorer widgets in a container panel for sidebar switching
    explorer_panel_ = new wxPanel(sidebar_panel_, wxID_ANY);
    auto* explorer_sizer = new wxBoxSizer(wxVERTICAL);

    auto* search_sizer = new wxBoxSizer(wxHORIZONTAL);
    search_field_ = new wxSearchCtrl(
        explorer_panel_, wxID_ANY, wxEmptyString, wxDefaultPosition, wxSize(-1, 28));
    search_field_->SetDescriptiveText("Filter files\u2026");
    search_field_->ShowCancelButton(true);
    search_field_->SetBackgroundColour(
        theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(110));
    search_field_->SetForegroundColour(theme_engine().color(core::ThemeColorToken::TextMain));
    search_field_->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular));

    search_sizer->AddSpacer(8);
    search_sizer->Add(search_field_, 1, wxALIGN_CENTER_VERTICAL);
    search_sizer->AddSpacer(8);
    explorer_sizer->Add(search_sizer, 0, wxEXPAND | wxTOP | wxBOTTOM, 8); // 8E: was 4

    // Spacer content area -> FileTreeCtrl
    file_tree_ = new FileTreeCtrl(explorer_panel_, theme_engine(), event_bus_);
    explorer_sizer->Add(file_tree_, 1, wxEXPAND);

    // Bind search field text changes to filter
    search_field_->Bind(wxEVT_TEXT,
                        [this](wxCommandEvent& /*evt*/)
                        {
                            if (file_tree_ != nullptr)
                            {
                                file_tree_->ApplyFilter(search_field_->GetValue().ToStdString());
                            }
                        });

    // R5 Fix 7: Bind cancel button to clear the filter
    search_field_->Bind(wxEVT_SEARCHCTRL_CANCEL_BTN,
                        [this](wxCommandEvent& /*evt*/)
                        {
                            search_field_->Clear();
                            if (file_tree_ != nullptr)
                            {
                                file_tree_->ApplyFilter("");
                            }
                        });

    // Wire file open callback — double-click or context menu "Open" (Fix 1)
    file_tree_->SetOnFileOpen(
        [this](const core::FileNode& node)
        {
            if (node.is_file())
            {
                OpenFileInTab(node.id);
            }
        });

    // Wire file select callback — single-click opens file (Fix 2)
    file_tree_->SetOnFileSelect(
        [this](const core::FileNode& node)
        {
            if (node.is_file())
            {
                OpenFileInTab(node.id);
            }
        });

    // Load sample file tree only when not restoring a workspace (Fix 14)
    if (config_ == nullptr || config_->get_string("workspace.last_path", "").empty())
    {
        auto sample_root = core::get_sample_file_tree();
        file_tree_->SetFileTree(sample_root.children);

        // Auto-select first file
        if (!sample_root.children.empty())
        {
            for (const auto& child : sample_root.children)
            {
                if (child.is_file())
                {
                    file_tree_->SetActiveFileId(child.id);
                    break;
                }
            }
        }
    } // end: Fix 14 conditional sample tree

    // Footer — Fix 13: show file count
    auto* footer_panel = new wxPanel(explorer_panel_, wxID_ANY, wxDefaultPosition, wxSize(-1, 28));
    footer_panel->SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgApp));
    auto* footer_sizer = new wxBoxSizer(wxHORIZONTAL);
    file_count_label_ = new wxStaticText(footer_panel, wxID_ANY, "");
    file_count_label_->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(0.8f));
    file_count_label_->SetForegroundColour(theme_engine().color(core::ThemeColorToken::TextMuted));
    footer_sizer->AddSpacer(12);
    footer_sizer->Add(file_count_label_, 1, wxALIGN_CENTER_VERTICAL);
    footer_panel->SetSizer(footer_sizer);
    explorer_sizer->Add(footer_panel, 0, wxEXPAND);

    explorer_panel_->SetSizer(explorer_sizer);
    sidebar_sizer->Add(explorer_panel_, 1, wxEXPAND);

    sidebar_panel_->SetSizer(sidebar_sizer);

    // --- Splitter ---
    splitter_ = new SplitterBar(this, theme_engine(), this);

    // --- Content panel ---
    content_panel_ = new wxPanel(this, wxID_ANY);
    content_panel_->SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgApp));

    // Content internal: toolbar + split view
    auto* content_sizer = new wxBoxSizer(wxVERTICAL);

    toolbar_ = new Toolbar(content_panel_, theme_engine(), event_bus_);
    toolbar_->SetOnThemeGalleryClick(
        [this]()
        {
            // Phase 4: Guard ThemeGallery behind feature toggle
            if (feature_registry_ != nullptr &&
                !feature_registry_->is_enabled(core::builtin_features::kThemeGallery))
            {
                return;
            }
            ThemeGallery gallery(this, theme_engine(), theme_engine().registry());
            gallery.ShowGallery();
        });
    content_sizer->Add(toolbar_, 0, wxEXPAND);

    // Tab bar (QoL feature 1)
    tab_bar_ = new TabBar(content_panel_, theme_engine(), event_bus_);
    content_sizer->Add(tab_bar_, 0, wxEXPAND);

    // R3 Fix 14: BreadcrumbBar between tab bar and split view
    breadcrumb_bar_ = new BreadcrumbBar(content_panel_, theme_engine(), event_bus_);
    content_sizer->Add(breadcrumb_bar_, 0, wxEXPAND);

    // Phase 9: Respect initial feature toggle state for breadcrumb
    if (feature_registry_ != nullptr &&
        !feature_registry_->is_enabled(core::builtin_features::kBreadcrumb))
    {
        breadcrumb_bar_->Hide();
    }

    split_view_ = new SplitView(
        content_panel_, theme_engine(), event_bus_, config_, mermaid_renderer_, math_renderer_);

    // Phase 4: Wire FeatureRegistry to SplitView (forwards to EditorPanel)
    if (feature_registry_ != nullptr)
    {
        split_view_->set_feature_registry(feature_registry_);

        // Set initial Mermaid rendering state from feature registry
        split_view_->set_mermaid_enabled(
            feature_registry_->is_enabled(core::builtin_features::kMermaid));
    }

    content_sizer->Add(split_view_, 1, wxEXPAND);

    content_panel_->SetSizer(content_sizer);

    // --- Status bar ---
    statusbar_panel_ = new StatusBarPanel(this, theme_engine(), event_bus_);

    // --- Main layout ---
    main_sizer_ = new wxBoxSizer(wxVERTICAL);

    // Body: sidebar + splitter + content (horizontal)
    body_sizer_ = new wxBoxSizer(wxHORIZONTAL);

    sidebar_current_width_ = sidebar_visible_ ? sidebar_width_ : 0;

    body_sizer_->Add(sidebar_panel_, 0, wxEXPAND);
    body_sizer_->Add(splitter_, 0, wxEXPAND);
    body_sizer_->Add(content_panel_, 1, wxEXPAND);

    main_sizer_->Add(body_sizer_, 1, wxEXPAND);
    main_sizer_->Add(statusbar_panel_, 0, wxEXPAND);

    SetSizer(main_sizer_);

    // Apply initial sidebar width
    UpdateSidebarSize(sidebar_current_width_);
}

// --- Zone access ---

auto LayoutManager::sidebar_container() -> wxPanel*
{
    return sidebar_panel_;
}

auto LayoutManager::content_container() -> wxPanel*
{
    return content_panel_;
}

auto LayoutManager::statusbar_container() -> StatusBarPanel*
{
    return statusbar_panel_;
}

void LayoutManager::setFileTree(const std::vector<core::FileNode>& roots)
{
    if (file_tree_ != nullptr)
    {
        file_tree_->SetFileTree(roots);
    }

    // Fix 13: Update sidebar footer file count
    if (file_count_label_ != nullptr)
    {
        size_t total_files = 0;
        size_t total_folders = 0;
        for (const auto& root : roots)
        {
            total_files += root.file_count();
            total_folders += root.folder_count();
            if (root.is_folder())
            {
                ++total_folders;
            }
            else
            {
                ++total_files;
            }
        }
        file_count_label_->SetLabel(
            wxString::Format("%zu files, %zu folders", total_files, total_folders));
    }
}

// Fix 15: Forward workspace root to file tree for relative path computation
void LayoutManager::SetWorkspaceRoot(const std::string& root_path)
{
    if (file_tree_ != nullptr)
    {
        file_tree_->SetWorkspaceRoot(root_path);
    }
    // R3 Fix 19: Update sidebar header with workspace folder name
    if (header_label_ != nullptr)
    {
        std::string folder_name = std::filesystem::path(root_path).filename().string();
        std::transform(folder_name.begin(), folder_name.end(), folder_name.begin(), ::toupper);
        header_label_->SetLabel(folder_name);
    }
}

// --- Sidebar control ---

void LayoutManager::toggle_sidebar()
{
    set_sidebar_visible(!sidebar_visible_);
}

void LayoutManager::set_sidebar_visible(bool visible)
{
    if (visible == sidebar_visible_ && !sidebar_anim_timer_.IsRunning())
    {
        return;
    }

    sidebar_visible_ = visible;
    sidebar_anim_showing_ = visible;
    sidebar_anim_start_width_ = sidebar_current_width_;
    sidebar_anim_target_width_ = visible ? sidebar_width_ : 0;
    sidebar_anim_progress_ = 0.0;

    sidebar_anim_timer_.Start(kAnimFrameMs);
    MARKAMP_LOG_DEBUG("Sidebar animation started: {} -> {}",
                      sidebar_anim_start_width_,
                      sidebar_anim_target_width_);
}

auto LayoutManager::is_sidebar_visible() const -> bool
{
    return sidebar_visible_;
}

void LayoutManager::set_sidebar_width(int width)
{
    sidebar_width_ = std::clamp(width, kMinSidebarWidth, kMaxSidebarWidth);
    if (sidebar_visible_ && !sidebar_anim_timer_.IsRunning())
    {
        UpdateSidebarSize(sidebar_width_);
    }
    SaveLayoutState();
}

auto LayoutManager::sidebar_width() const -> int
{
    return sidebar_width_;
}

// --- Animation ---

void LayoutManager::OnSidebarAnimTimer(wxTimerEvent& /*event*/)
{
    double duration = sidebar_anim_showing_ ? kShowDurationMs : kHideDurationMs;
    sidebar_anim_progress_ += static_cast<double>(kAnimFrameMs) / duration;

    if (sidebar_anim_progress_ >= 1.0)
    {
        sidebar_anim_progress_ = 1.0;
        sidebar_anim_timer_.Stop();
        SaveLayoutState();
    }

    // Easing: ease-out for show (1 - (1-t)^3), ease-in for hide (t^3)
    double t = sidebar_anim_progress_;
    double eased = sidebar_anim_showing_ ? 1.0 - std::pow(1.0 - t, 3.0) // ease-out
                                         : std::pow(t, 3.0);            // ease-in

    int new_width = sidebar_anim_start_width_ +
                    static_cast<int>(eased * static_cast<double>(sidebar_anim_target_width_ -
                                                                 sidebar_anim_start_width_));

    UpdateSidebarSize(new_width);
}

void LayoutManager::UpdateSidebarSize(int width)
{
    sidebar_current_width_ = width;

    // Show/hide sidebar and splitter based on width
    bool show = (width > 0);
    sidebar_panel_->Show(show);
    splitter_->Show(show);

    if (show)
    {
        sidebar_panel_->SetMinSize(wxSize(width, -1));
        sidebar_panel_->SetMaxSize(wxSize(width, -1));
    }

    body_sizer_->Layout();
    main_sizer_->Layout();
}

// --- Theme ---

void LayoutManager::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);

    content_panel_->SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgApp));
    content_panel_->Refresh();

    if (search_field_ != nullptr)
    {
        search_field_->SetBackgroundColour(
            theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(110));
        search_field_->SetForegroundColour(theme_engine().color(core::ThemeColorToken::TextMain));
        search_field_->Refresh();
    }

    sidebar_panel_->Refresh();
}

void LayoutManager::OnSidebarPaint(wxPaintEvent& /*event*/)
{
    wxAutoBufferedPaintDC paint_dc(sidebar_panel_);
    auto client_sz = sidebar_panel_->GetClientSize();
    const int panel_width = client_sz.GetWidth();
    const int panel_height = client_sz.GetHeight();

    // 8D: Subtle top-to-bottom gradient (BgPanel → 3% darker)
    {
        auto base_col = theme_engine().color(core::ThemeColorToken::BgPanel);
        auto darker = base_col.ChangeLightness(97);
        for (int row = 0; row < panel_height; ++row)
        {
            const double frac =
                static_cast<double>(row) / static_cast<double>(std::max(panel_height - 1, 1));
            auto lerp = [](int from, int to, double ratio) -> unsigned char
            {
                return static_cast<unsigned char>(
                    std::clamp(static_cast<int>(from + ratio * (to - from)), 0, 255));
            };
            paint_dc.SetPen(wxPen(wxColour(lerp(base_col.Red(), darker.Red(), frac),
                                           lerp(base_col.Green(), darker.Green(), frac),
                                           lerp(base_col.Blue(), darker.Blue(), frac)),
                                  1));
            paint_dc.DrawLine(0, row, panel_width, row);
        }
    }

    // 8A: Drop shadow on right edge (4 graduated bands: 8%→4%→2%→1% black)
    {
        constexpr int kShadowBands = 4;
        constexpr std::array<unsigned char, kShadowBands> kShadowAlphas = {20, 10, 5, 3};
        for (int band = 0; band < kShadowBands; ++band)
        {
            paint_dc.SetPen(
                wxPen(wxColour(0, 0, 0, kShadowAlphas.at(static_cast<size_t>(band))), 1));
            paint_dc.DrawLine(panel_width - 1 - band, 0, panel_width - 1 - band, panel_height);
        }
    }

    // 8B: Soft left highlight — 1px BgPanel lighter
    {
        auto highlight = theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(108);
        paint_dc.SetPen(wxPen(highlight, 1));
        paint_dc.DrawLine(0, 0, 0, panel_height);
    }
}

// --- Persistence ---

void LayoutManager::SaveLayoutState()
{
    if (config_ == nullptr)
    {
        return;
    }
    config_->set("layout.sidebar_visible", sidebar_visible_);
    config_->set("layout.sidebar_width", sidebar_width_);
    // Fix 15: Persist active file path for restore on next launch
    config_->set("workspace.last_active_file", active_file_path_);
}

void LayoutManager::RestoreLayoutState()
{
    if (config_ == nullptr)
    {
        return;
    }
    sidebar_visible_ = config_->get_bool("layout.sidebar_visible", true);
    sidebar_width_ = config_->get_int("layout.sidebar_width", kDefaultSidebarWidth);
    sidebar_width_ = std::clamp(sidebar_width_, kMinSidebarWidth, kMaxSidebarWidth);
    sidebar_current_width_ = sidebar_visible_ ? sidebar_width_ : 0;
}

void LayoutManager::ToggleEditorMinimap()
{
    if (split_view_ != nullptr)
    {
        auto* editor = split_view_->GetEditorPanel();
        if (editor != nullptr)
        {
            editor->ToggleMinimap();
        }
    }
}

// --- Phase 8: Sidebar panel switching ---

void LayoutManager::SetSidebarMode(SidebarMode mode)
{
    if (mode == sidebar_mode_)
    {
        return;
    }

    sidebar_mode_ = mode;

    switch (mode)
    {
        case SidebarMode::kExplorer:
        {
            if (extensions_panel_ != nullptr)
            {
                extensions_panel_->Hide();
            }
            if (explorer_panel_ != nullptr)
            {
                explorer_panel_->Show();
            }
            if (header_label_ != nullptr)
            {
                header_label_->SetLabel("EXPLORER");
            }
            break;
        }
        case SidebarMode::kExtensions:
        {
            if (explorer_panel_ != nullptr)
            {
                explorer_panel_->Hide();
            }

            // Lazily create the extensions panel when first shown
            if (extensions_panel_ == nullptr && ext_mgmt_service_ != nullptr &&
                ext_gallery_service_ != nullptr)
            {
                extensions_panel_ = new ExtensionsBrowserPanel(sidebar_panel_,
                                                               theme_engine(),
                                                               event_bus_,
                                                               *ext_mgmt_service_,
                                                               *ext_gallery_service_);

                auto* sidebar_sizer = sidebar_panel_->GetSizer();
                if (sidebar_sizer != nullptr)
                {
                    sidebar_sizer->Add(extensions_panel_, 1, wxEXPAND);
                }
            }

            if (extensions_panel_ != nullptr)
            {
                extensions_panel_->Show();
                extensions_panel_->ShowInstalledExtensions();
            }

            if (header_label_ != nullptr)
            {
                header_label_->SetLabel("EXTENSIONS");
            }
            break;
        }
    }

    sidebar_panel_->Layout();
    sidebar_panel_->Refresh();
}

auto LayoutManager::GetSidebarMode() const -> SidebarMode
{
    return sidebar_mode_;
}

void LayoutManager::SetExtensionServices(core::IExtensionManagementService* mgmt_service,
                                         core::IExtensionGalleryService* gallery_service)
{
    ext_mgmt_service_ = mgmt_service;
    ext_gallery_service_ = gallery_service;
}

// --- Multi-file tab management ---

void LayoutManager::OpenFileInTab(const std::string& path)
{
    // If file is already open, just switch to it
    if (tab_bar_ != nullptr && tab_bar_->HasTab(path))
    {
        SwitchToTab(path);
        return;
    }

    // Save current editor state before switching
    if (!active_file_path_.empty() && split_view_ != nullptr)
    {
        auto buf_it = file_buffers_.find(active_file_path_);
        if (buf_it != file_buffers_.end())
        {
            auto* editor = split_view_->GetEditorPanel();
            if (editor != nullptr)
            {
                buf_it->second.content = editor->GetContent();
                auto session = editor->GetSessionState();
                buf_it->second.cursor_position = session.cursor_position;
                buf_it->second.first_visible_line = session.first_visible_line;
            }
        }
    }

    // Read file content
    std::string content;
    try
    {
        std::ifstream file_stream(path);
        if (file_stream.is_open())
        {
            content.assign(std::istreambuf_iterator<char>(file_stream),
                           std::istreambuf_iterator<char>());
        }
        else
        {
            MARKAMP_LOG_ERROR("Failed to open file: {}", path);
            return;
        }
    }
    catch (const std::exception& ex)
    {
        MARKAMP_LOG_ERROR("Error reading file {}: {}", path, ex.what());
        return;
    }

    // Store in buffer
    FileBuffer buffer;
    buffer.content = content;
    buffer.is_modified = false;
    buffer.cursor_position = 0;
    buffer.first_visible_line = 0;
    try
    {
        buffer.last_write_time = std::filesystem::last_write_time(path);
    }
    catch (const std::filesystem::filesystem_error& ex)
    {
        MARKAMP_LOG_WARN("Could not get last write time for {}: {}", path, ex.what());
    }
    file_buffers_[path] = std::move(buffer);

    // Extract display name from path
    const std::string display_name = std::filesystem::path(path).filename().string();

    // Add tab
    if (tab_bar_ != nullptr)
    {
        tab_bar_->AddTab(path, display_name);
    }

    // Load content into editor
    active_file_path_ = path;
    if (split_view_ != nullptr)
    {
        auto* editor = split_view_->GetEditorPanel();
        if (editor != nullptr)
        {
            editor->SetContent(content);
            editor->ClearModified();
            // R3 Fix 9: Deferred focus so Select All works immediately
            CallAfter([editor]() { editor->SetFocus(); });
        }
    }

    // R2 Fix 13: Update status bar filename
    if (statusbar_panel_ != nullptr)
    {
        statusbar_panel_->set_filename(display_name);
        // R2 Fix 14: Language from extension
        const std::string ext = std::filesystem::path(path).extension().string();
        std::string language;
        if (ext == ".md" || ext == ".markdown" || ext == ".mdx")
            language = "Markdown";
        else if (ext == ".json")
            language = "JSON";
        else if (ext == ".cpp" || ext == ".cc" || ext == ".cxx")
            language = "C++";
        else if (ext == ".h" || ext == ".hpp" || ext == ".hxx")
            language = "C++ Header";
        else if (ext == ".c")
            language = "C";
        else if (ext == ".py")
            language = "Python";
        else if (ext == ".js")
            language = "JavaScript";
        else if (ext == ".ts")
            language = "TypeScript";
        else if (ext == ".html" || ext == ".htm")
            language = "HTML";
        else if (ext == ".css")
            language = "CSS";
        else if (ext == ".yaml" || ext == ".yml")
            language = "YAML";
        else if (ext == ".xml")
            language = "XML";
        else if (ext == ".txt")
            language = "Plain Text";
        else if (ext == ".sh" || ext == ".zsh" || ext == ".bash")
            language = "Shell";
        else if (ext == ".cmake")
            language = "CMake";
        else
            language = ext.empty() ? "Plain Text" : ext.substr(1);
        statusbar_panel_->set_language(language);

        // R2 Fix 19: File size
        try
        {
            const auto file_size = std::filesystem::file_size(path);
            statusbar_panel_->set_file_size(file_size);
        }
        catch (const std::filesystem::filesystem_error& /*err*/)
        {
            statusbar_panel_->set_file_size(0);
        }

        // R4 Fix 9: Detect EOL mode from file content
        if (content.find("\r\n") != std::string::npos)
        {
            statusbar_panel_->set_eol_mode("CRLF");
        }
        else
        {
            statusbar_panel_->set_eol_mode("LF");
        }
    }

    MARKAMP_LOG_INFO("Opened file in tab: {}", path);
}

void LayoutManager::CloseTab(const std::string& path)
{
    const auto buf_it = file_buffers_.find(path);
    if (buf_it == file_buffers_.end())
    {
        // Not in our buffers, just remove the tab
        if (tab_bar_ != nullptr)
        {
            tab_bar_->RemoveTab(path);
        }
        return;
    }

    // Check if modified — prompt user
    if (buf_it->second.is_modified)
    {
        const std::string display_name = std::filesystem::path(path).filename().string();
        const int result = wxMessageBox(
            wxString::Format("'%s' has unsaved changes. Save before closing?", display_name),
            "Unsaved Changes",
            wxYES_NO | wxCANCEL | wxICON_QUESTION,
            this);

        if (result == wxCANCEL)
        {
            return;
        }
        if (result == wxYES)
        {
            SaveFile(path);
        }
    }

    // Remove from buffer
    file_buffers_.erase(buf_it);

    // Remove tab (TabBar handles activating adjacent tab)
    if (tab_bar_ != nullptr)
    {
        tab_bar_->RemoveTab(path);
    }

    // Update active path
    if (active_file_path_ == path)
    {
        if (tab_bar_ != nullptr)
        {
            active_file_path_ = tab_bar_->GetActiveTabPath();
        }
        else
        {
            active_file_path_.clear();
        }

        // Load the new active file if any
        if (!active_file_path_.empty())
        {
            SwitchToTab(active_file_path_);
        }
        else if (split_view_ != nullptr)
        {
            // Fix 12: Show empty-state placeholder when last tab closes
            auto* editor = split_view_->GetEditorPanel();
            if (editor != nullptr)
            {
                editor->SetContent("");
                editor->ClearModified();
            }

            // R2 Fix 12: Return to startup screen when all tabs close
            core::events::ShowStartupRequestEvent startup_evt;
            event_bus_.publish(startup_evt);
        }
    }

    MARKAMP_LOG_INFO("Closed tab: {}", path);
}

void LayoutManager::SwitchToTab(const std::string& path)
{
    if (path == active_file_path_)
    {
        return;
    }

    // Save current editor state
    if (!active_file_path_.empty() && split_view_ != nullptr)
    {
        auto buf_it = file_buffers_.find(active_file_path_);
        if (buf_it != file_buffers_.end())
        {
            auto* editor = split_view_->GetEditorPanel();
            if (editor != nullptr)
            {
                buf_it->second.content = editor->GetContent();
                auto session = editor->GetSessionState();
                buf_it->second.cursor_position = session.cursor_position;
                buf_it->second.first_visible_line = session.first_visible_line;
            }
        }
    }

    // Load target file from buffer
    const auto buf_it = file_buffers_.find(path);
    if (buf_it == file_buffers_.end())
    {
        MARKAMP_LOG_WARN("SwitchToTab: file not in buffer: {}", path);
        return;
    }

    active_file_path_ = path;

    // Update tab bar
    if (tab_bar_ != nullptr)
    {
        tab_bar_->SetActiveTab(path);
    }

    // Load content
    if (split_view_ != nullptr)
    {
        auto* editor = split_view_->GetEditorPanel();
        if (editor != nullptr)
        {
            editor->SetContent(buf_it->second.content);
            EditorPanel::SessionState restore_state;
            restore_state.cursor_position = buf_it->second.cursor_position;
            restore_state.first_visible_line = buf_it->second.first_visible_line;
            editor->RestoreSessionState(restore_state);

            if (!buf_it->second.is_modified)
            {
                editor->ClearModified();
            }

            // R3 Fix 9: Deferred focus on tab switch
            CallAfter([editor]() { editor->SetFocus(); });
        }
    }

    // Fix 7: Sync file tree selection with active tab
    // R4 Fix 12: Auto-reveal file in sidebar
    if (file_tree_ != nullptr)
    {
        file_tree_->SetActiveFileId(path);
        file_tree_->EnsureNodeVisible(path);
    }

    // R3 Fix 14: Update breadcrumb bar with file path segments
    // R4 Fix 19: Handle Untitled files in breadcrumb
    if (breadcrumb_bar_ != nullptr)
    {
        namespace fs = std::filesystem;
        std::vector<std::string> segments;

        // Check if this is an untitled file (path starts with "untitled:")
        if (path.rfind("untitled:", 0) == 0)
        {
            // Extract "Untitled-N.md" from the path
            const auto untitled_name = path.substr(9);
            segments.push_back(untitled_name.empty() ? "Untitled.md" : untitled_name);
        }
        else
        {
            fs::path file_path(path);
            for (const auto& part : file_path)
            {
                std::string part_str = part.string();
                if (!part_str.empty() && part_str != "/")
                {
                    segments.push_back(part_str);
                }
            }
        }
        breadcrumb_bar_->SetFilePath(segments);
    }

    // Fix 13: Publish content changed event to refresh preview panel
    core::events::EditorContentChangedEvent content_evt;
    content_evt.content = buf_it->second.content;
    event_bus_.publish(content_evt);

    // Fix 16: Update status bar cursor position on tab switch
    if (statusbar_panel_ != nullptr)
    {
        statusbar_panel_->set_cursor_position(buf_it->second.first_visible_line + 1, 1);

        // R2 Fix 13: Update filename in status bar
        const std::string display_name = std::filesystem::path(path).filename().string();
        statusbar_panel_->set_filename(display_name);

        // R2 Fix 14: Update language in status bar
        const std::string ext = std::filesystem::path(path).extension().string();
        std::string language;
        if (ext == ".md" || ext == ".markdown" || ext == ".mdx")
            language = "Markdown";
        else if (ext == ".json")
            language = "JSON";
        else if (ext == ".cpp" || ext == ".cc" || ext == ".cxx")
            language = "C++";
        else if (ext == ".h" || ext == ".hpp" || ext == ".hxx")
            language = "C++ Header";
        else if (ext == ".c")
            language = "C";
        else if (ext == ".py")
            language = "Python";
        else if (ext == ".js")
            language = "JavaScript";
        else if (ext == ".ts")
            language = "TypeScript";
        else if (ext == ".html" || ext == ".htm")
            language = "HTML";
        else if (ext == ".css")
            language = "CSS";
        else if (ext == ".yaml" || ext == ".yml")
            language = "YAML";
        else if (ext == ".xml")
            language = "XML";
        else if (ext == ".txt")
            language = "Plain Text";
        else if (ext == ".sh" || ext == ".zsh" || ext == ".bash")
            language = "Shell";
        else if (ext == ".cmake")
            language = "CMake";
        else
            language = ext.empty() ? "Plain Text" : ext.substr(1);
        statusbar_panel_->set_language(language);

        // R2 Fix 19: Update file size in status bar
        try
        {
            const auto file_size = std::filesystem::file_size(path);
            statusbar_panel_->set_file_size(file_size);
        }
        catch (const std::filesystem::filesystem_error& /*err*/)
        {
            statusbar_panel_->set_file_size(0);
        }
    }

    MARKAMP_LOG_DEBUG("Switched to tab: {}", path);
}

void LayoutManager::SaveActiveFile()
{
    if (!active_file_path_.empty())
    {
        SaveFile(active_file_path_);

        // Mark as not modified
        auto buf_it = file_buffers_.find(active_file_path_);
        if (buf_it != file_buffers_.end())
        {
            buf_it->second.is_modified = false;
            try
            {
                buf_it->second.last_write_time =
                    std::filesystem::last_write_time(active_file_path_);
            }
            catch (const std::filesystem::filesystem_error& /*ex*/)
            {
            }
        }
        if (tab_bar_ != nullptr)
        {
            tab_bar_->SetTabModified(active_file_path_, false);
        }
    }
}

void LayoutManager::SaveActiveFileAs()
{
    wxFileDialog dialog(this,
                        "Save As",
                        wxEmptyString,
                        wxEmptyString,
                        "Markdown files (*.md)|*.md|All files (*.*)|*.*",
                        wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

    if (dialog.ShowModal() == wxID_CANCEL)
    {
        return;
    }

    const std::string new_path = dialog.GetPath().ToStdString();

    // Save content to new path
    if (split_view_ != nullptr)
    {
        split_view_->SaveFile(new_path);
    }

    // Update buffer
    if (!active_file_path_.empty())
    {
        auto buf_it = file_buffers_.find(active_file_path_);
        if (buf_it != file_buffers_.end())
        {
            FileBuffer new_buf = std::move(buf_it->second);
            new_buf.is_modified = false;
            file_buffers_.erase(buf_it);
            file_buffers_[new_path] = std::move(new_buf);
        }

        // Update tab
        if (tab_bar_ != nullptr)
        {
            const std::string display_name = std::filesystem::path(new_path).filename().string();
            tab_bar_->RenameTab(active_file_path_, new_path, display_name);
            tab_bar_->SetTabModified(new_path, false);
        }

        active_file_path_ = new_path;
    }
}

auto LayoutManager::GetActiveFilePath() const -> std::string
{
    return active_file_path_;
}

auto LayoutManager::GetActiveFileContent() const -> std::string
{
    if (split_view_ != nullptr)
    {
        auto* editor = split_view_->GetEditorPanel();
        if (editor != nullptr)
        {
            return editor->GetContent();
        }
    }
    return {};
}

auto LayoutManager::GetTabBar() -> TabBar*
{
    return tab_bar_;
}

// --- Auto-save ---

void LayoutManager::StartAutoSave()
{
    auto_save_timer_.SetOwner(this);
    auto_save_timer_.Start(kAutoSaveIntervalMs);
    MARKAMP_LOG_INFO("Auto-save started (interval={}ms)", kAutoSaveIntervalMs);
}

void LayoutManager::StopAutoSave()
{
    auto_save_timer_.Stop();
    MARKAMP_LOG_INFO("Auto-save stopped");
}

void LayoutManager::OnAutoSaveTimer(wxTimerEvent& /*event*/)
{
    for (auto& [path, buffer] : file_buffers_)
    {
        if (buffer.is_modified)
        {
            const std::string draft_path = path + ".markamp-draft";
            try
            {
                std::ofstream draft(draft_path);
                if (draft.is_open())
                {
                    draft << buffer.content;
                    MARKAMP_LOG_DEBUG("Auto-saved draft: {}", draft_path);
                }
            }
            catch (const std::exception& ex)
            {
                MARKAMP_LOG_WARN("Auto-save failed for {}: {}", path, ex.what());
            }
        }
    }
}

// --- External file change detection ---

void LayoutManager::CheckExternalFileChanges()
{
    if (active_file_path_.empty())
    {
        return;
    }

    auto buf_it = file_buffers_.find(active_file_path_);
    if (buf_it == file_buffers_.end())
    {
        return;
    }

    try
    {
        const auto current_write_time = std::filesystem::last_write_time(active_file_path_);
        if (current_write_time > buf_it->second.last_write_time)
        {
            const std::string display_name =
                std::filesystem::path(active_file_path_).filename().string();
            const int result = wxMessageBox(
                wxString::Format("'%s' has been modified externally. Reload?", display_name),
                "File Changed",
                wxYES_NO | wxICON_QUESTION,
                this);

            if (result == wxYES)
            {
                // Re-read file
                std::ifstream file_stream(active_file_path_);
                if (file_stream.is_open())
                {
                    std::string content((std::istreambuf_iterator<char>(file_stream)),
                                        std::istreambuf_iterator<char>());

                    buf_it->second.content = content;
                    buf_it->second.is_modified = false;
                    buf_it->second.last_write_time = current_write_time;

                    if (split_view_ != nullptr)
                    {
                        auto* editor = split_view_->GetEditorPanel();
                        if (editor != nullptr)
                        {
                            editor->SetContent(content);
                            editor->ClearModified();
                        }
                    }

                    if (tab_bar_ != nullptr)
                    {
                        tab_bar_->SetTabModified(active_file_path_, false);
                    }

                    MARKAMP_LOG_INFO("Reloaded file from disk: {}", active_file_path_);
                }
            }
            else
            {
                // User declined — update timestamp to avoid re-prompting
                buf_it->second.last_write_time = current_write_time;
            }
        }
    }
    catch (const std::filesystem::filesystem_error& ex)
    {
        MARKAMP_LOG_WARN("Error checking file changes: {}", ex.what());
    }
}

auto LayoutManager::HasUnsavedFiles() const -> bool
{
    for (const auto& [path, buffer] : file_buffers_)
    {
        if (buffer.is_modified)
        {
            return true;
        }
    }
    return false;
}

auto LayoutManager::GetOpenFileCount() const -> size_t
{
    return file_buffers_.size();
}

// R2 Fix 15: Save all modified files
void LayoutManager::SaveAllFiles()
{
    for (auto& [path, buffer] : file_buffers_)
    {
        if (buffer.is_modified)
        {
            SaveFile(path);
        }
    }
}

// R2 Fix 16: Revert active file to on-disk content
void LayoutManager::RevertActiveFile()
{
    if (active_file_path_.empty())
    {
        return;
    }

    auto buf_it = file_buffers_.find(active_file_path_);
    if (buf_it == file_buffers_.end())
    {
        return;
    }

    // Re-read from disk
    try
    {
        std::ifstream file_stream(active_file_path_);
        if (!file_stream.is_open())
        {
            return;
        }
        std::string content;
        content.assign(std::istreambuf_iterator<char>(file_stream),
                       std::istreambuf_iterator<char>());

        buf_it->second.content = content;
        buf_it->second.is_modified = false;

        // Reload into editor
        if (split_view_ != nullptr)
        {
            auto* editor = split_view_->GetEditorPanel();
            if (editor != nullptr)
            {
                editor->SetContent(content);
                editor->ClearModified();
            }
        }

        // Update tab modified state
        if (tab_bar_ != nullptr)
        {
            tab_bar_->SetTabModified(active_file_path_, false);
        }
    }
    catch (const std::exception& ex)
    {
        MARKAMP_LOG_ERROR("Error reverting file {}: {}", active_file_path_, ex.what());
    }
}

// R2 Fix 17: Close all open tabs
void LayoutManager::CloseAllTabs()
{
    if (tab_bar_ == nullptr)
    {
        return;
    }

    // Copy paths because CloseTab mutates the container
    const auto all_paths = tab_bar_->GetAllTabPaths();
    for (const auto& path : all_paths)
    {
        CloseTab(path);
    }
}

} // namespace markamp::ui
