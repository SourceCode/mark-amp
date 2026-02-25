#include "LayoutManager.h"

#include "ActivityBar.h"
#include "BreadcrumbBar.h"
#include "CanvasWorkspacePanel.h"
#include "EditorPanel.h"
#include "OutputPanel.h"
#include "ProblemsPanel.h"
#include "SplitView.h"
#include "StatusBarPanel.h"
#include "TabBar.h"
#include "ThemeGallery.h"
#include "Toolbar.h"
#include "WalkthroughPanel.h"
#include "core/BuiltInPlugins.h"
#include "core/Config.h"
#include "core/Events.h"
#include "core/FeatureRegistry.h"
#include "core/Logger.h"
#include "core/SampleFiles.h"
#include "ui/ExplorerPanel.h"
#include "ui/ExtensionsBrowserPanel.h"
#include "ui/FileTreeCtrl.h"
#include "ui/GraphSidebarPanel.h"
#include "ui/IconManager.h"
#include "ui/SearchSidebarPanel.h"
#include "ui/SidebarHeader.h"

#include <nlohmann/json.hpp>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/clipbrd.h>
#include <wx/dcbuffer.h>
#include <wx/filedlg.h>
#include <wx/listbox.h>
#include <wx/menu.h>
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
{
    design_registry_ = std::make_unique<DesignTokenRegistry>(theme_engine, event_bus_);
    typography_scale_ = std::make_unique<TypographyScale>();
    spacing_grid_ = std::make_unique<SpacingGrid>();
    elevation_system_ = std::make_unique<ElevationSystem>();

    ds_context_.reset(new DesignSystemContext{theme_engine,
                                              *design_registry_,
                                              *typography_scale_,
                                              *spacing_grid_,
                                              ComponentSizeResolver::get(),
                                              *elevation_system_,
                                              LayoutMetrics::get()});

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

    // Start auto-save
    StartAutoSave();

    panel_context_menu_sub_ = event_bus_.subscribe<core::events::PanelContextMenuEvent>(
        [this](const core::events::PanelContextMenuEvent& evt)
        {
            wxMenu menu;
            menu.Append(wxID_ANY, "Panel: " + evt.panel_title)->Enable(false);
            menu.AppendSeparator();

            auto* hide_item = menu.Append(wxID_ANY, "Hide Sidebar");
            menu.Bind(
                wxEVT_MENU,
                [this](wxCommandEvent&) { this->set_sidebar_visible(false); },
                hide_item->GetId());

            PopupMenu(&menu, ScreenToClient(wxPoint(evt.screen_x, evt.screen_y)));
        });

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

    MARKAMP_LOG_INFO("LayoutManager created", sidebar_width(), is_sidebar_visible());

    // Phase 06: Register sidebar panels in the registry
    RegisterSidebarPanels();

    // Phase 06: Subscribe to ActivityBar selection events to switch sidebar mode
    activity_bar_selection_sub_ = event_bus_.subscribe<core::events::ActivityBarSelectionEvent>(
        [this](const core::events::ActivityBarSelectionEvent& evt)
        {
            // Map ActivityBarItem -> SidebarMode. Since they are both strings
            // and use the exact same ID set, we pass it right through.
            SetSidebarMode(evt.item);
            if (!is_sidebar_visible())
            {
                set_sidebar_visible(true);
            }

            // Focus the sidebar when switched via activity bar
            auto* sidebar = sidebar_container();
            if (sidebar != nullptr)
            {
                sidebar->SetFocus();
                if (sidebar_mode_ == kSidebarModeExplorer && explorer_panel_ != nullptr)
                {
                    if (auto* tree = explorer_panel_->GetFileTree())
                    {
                        tree->SetFocus();
                    }
                }
            }
        });

    // Phase 8: Subscribe to sidebar mode switching events
    show_extensions_sub_ = event_bus_.subscribe<core::events::ShowExtensionsBrowserRequestEvent>(
        [this]([[maybe_unused]] const core::events::ShowExtensionsBrowserRequestEvent& evt)
        {
            SetSidebarMode(kSidebarModeExtensions);
            if (!is_sidebar_visible())
            {
                set_sidebar_visible(true);
            }
        });

    show_explorer_sub_ = event_bus_.subscribe<core::events::ShowExplorerRequestEvent>(
        [this]([[maybe_unused]] const core::events::ShowExplorerRequestEvent& evt)
        {
            SetSidebarMode(kSidebarModeExplorer);
            if (!is_sidebar_visible())
            {
                set_sidebar_visible(true);
            }
        });

    sidebar_focus_sub_ = event_bus_.subscribe<core::events::SidebarFocusRequestEvent>(
        [this](const core::events::SidebarFocusRequestEvent&)
        {
            if (!is_sidebar_visible())
            {
                set_sidebar_visible(true);
            }

            auto* sidebar = sidebar_container();
            if (sidebar != nullptr)
            {
                sidebar->SetFocus();

                // If it's the explorer panel, try to focus the tree
                if (sidebar_mode_ == kSidebarModeExplorer && explorer_panel_ != nullptr)
                {
                    if (auto* tree = explorer_panel_->GetFileTree())
                    {
                        tree->SetFocus();
                    }
                }
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
                    if (auto* c = content_container())
                        c->Layout();
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

    // V8 Phase 6: Subscribe to board open requests
    board_open_sub_ = event_bus_.subscribe<core::events::BoardOpenRequestEvent>(
        [this](const core::events::BoardOpenRequestEvent& evt)
        {
            ShowCanvasWorkspace();
            if (canvas_workspace_ != nullptr)
            {
                if (evt.board_id.empty())
                {
                    canvas_workspace_->NewBoard();
                }
                else
                {
                    canvas_workspace_->LoadBoard(evt.board_id);
                }
            }
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
    // --- Workbench Shell ---
    shell_ = new layout::WorkbenchShell(this, *ds_context_);
    shell_->set_zone_resized_callback(
        [this](layout::WorkbenchZoneId zone_id)
        {
            if (zone_id == layout::WorkbenchZoneId::kPrimarySidebar)
            {
                auto* container = shell_->get_zone_container(zone_id);
                if (container != nullptr)
                {
                    int width = container->GetSize().GetWidth();
                    config_->set("layout.sidebar.width." + sidebar_mode_, width);
                    [[maybe_unused]] auto res = config_->save();
                    MARKAMP_LOG_DEBUG("Saved sidebar width {} for mode {}", width, sidebar_mode_);
                }
            }
        });

    // --- Activity Bar (Task 8) ---
    auto* activity_bar_zone = shell_->get_zone_container(layout::WorkbenchZoneId::kActivityBar);
    auto* activity_bar = new ActivityBar(activity_bar_zone, *ds_context_, event_bus_, config_);
    auto* activity_bar_sizer = new wxBoxSizer(wxVERTICAL);
    activity_bar_sizer->Add(activity_bar, 1, wxEXPAND);
    activity_bar_zone->SetSizer(activity_bar_sizer);

    // --- Primary Sidebar (Task 9) ---
    auto* primary_sidebar_zone =
        shell_->get_zone_container(layout::WorkbenchZoneId::kPrimarySidebar);
    auto* explorer_sizer = new wxBoxSizer(wxVERTICAL);

    explorer_panel_ = new ExplorerPanel(primary_sidebar_zone,
                                        theme_engine(),
                                        event_bus_,
                                        config_,
                                        *ds_context_,
                                        IconManager::get());
    explorer_sizer->Add(explorer_panel_, 1, wxEXPAND | wxALL, 0);

    search_field_ = explorer_panel_->GetSearchField();
    file_tree_ = explorer_panel_->GetFileTree();

    search_field_->Bind(wxEVT_TEXT,
                        [this](wxCommandEvent& /*evt*/)
                        {
                            if (file_tree_ != nullptr)
                            {
                                file_tree_->ApplyFilter(search_field_->GetValue().ToStdString());
                            }
                        });

    search_field_->Bind(wxEVT_SEARCHCTRL_CANCEL_BTN,
                        [this](wxCommandEvent& /*evt*/)
                        {
                            search_field_->Clear();
                            if (file_tree_ != nullptr)
                            {
                                file_tree_->ApplyFilter("");
                            }
                        });

    file_tree_->SetOnFileOpen(
        [this](const core::FileNode& node)
        {
            if (node.is_file())
            {
                OpenFileInTab(node.id);
            }
        });

    file_tree_->SetOnFileSelect(
        [this](const core::FileNode& node)
        {
            if (node.is_file())
            {
                OpenFileInTab(node.id);
            }
        });

    if (config_ == nullptr || config_->get_string("workspace.last_path", "").empty())
    {
        auto sample_root = core::get_sample_file_tree();
        file_tree_->SetFileTree(sample_root.children);

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
    }

    primary_sidebar_zone->SetSizer(explorer_sizer);

    // --- Content Area (Task 10) ---
    auto* editor_zone = shell_->get_zone_container(layout::WorkbenchZoneId::kEditorArea);
    auto* content_sizer = new wxBoxSizer(wxVERTICAL);

    toolbar_ = new Toolbar(editor_zone, *ds_context_, event_bus_);
    toolbar_->SetOnThemeGalleryClick(
        [this]()
        {
            if (feature_registry_ != nullptr &&
                !feature_registry_->is_enabled(core::builtin_features::kThemeGallery))
            {
                return;
            }
            ThemeGallery gallery(this, theme_engine(), theme_engine().registry());
            gallery.ShowGallery();
        });
    content_sizer->Add(toolbar_, 0, wxEXPAND);

    tab_bar_ = new TabBar(editor_zone, *ds_context_, event_bus_);
    content_sizer->Add(tab_bar_, 0, wxEXPAND);

    breadcrumb_bar_ = new BreadcrumbBar(editor_zone, *ds_context_);
    content_sizer->Add(breadcrumb_bar_, 0, wxEXPAND);

    if (feature_registry_ != nullptr &&
        !feature_registry_->is_enabled(core::builtin_features::kBreadcrumb))
    {
        breadcrumb_bar_->Hide();
    }

    split_view_ = new SplitView(
        editor_zone, *ds_context_, event_bus_, config_, mermaid_renderer_, math_renderer_);

    if (feature_registry_ != nullptr)
    {
        split_view_->set_feature_registry(feature_registry_);
        split_view_->set_mermaid_enabled(
            feature_registry_->is_enabled(core::builtin_features::kMermaid));
    }

    content_sizer->Add(split_view_, 1, wxEXPAND);
    editor_zone->SetSizer(content_sizer);

    // --- Status bar ---
    auto* statusbar_zone = shell_->get_zone_container(layout::WorkbenchZoneId::kStatusBar);
    statusbar_panel_ = new StatusBarPanel(statusbar_zone, *ds_context_, event_bus_);
    auto* statusbar_sizer = new wxBoxSizer(wxVERTICAL);
    statusbar_sizer->Add(statusbar_panel_, 1, wxEXPAND);
    statusbar_zone->SetSizer(statusbar_sizer);

    // --- Panel Area (Task 12) ---
    auto* panel_zone = shell_->get_zone_container(layout::WorkbenchZoneId::kPanelArea);
    auto* panel_sizer = new wxBoxSizer(wxVERTICAL);
    bottom_panel_notebook_ =
        new wxNotebook(panel_zone, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxNB_TOP);

    output_panel_ = new OutputPanel(bottom_panel_notebook_, nullptr);
    problems_panel_ = new ProblemsPanel(bottom_panel_notebook_, nullptr);
    walkthrough_panel_ = new WalkthroughPanel(bottom_panel_notebook_);

    bottom_panel_notebook_->AddPage(output_panel_, "Output");
    bottom_panel_notebook_->AddPage(problems_panel_, "Problems");
    bottom_panel_notebook_->AddPage(walkthrough_panel_, "Walkthrough");

    panel_sizer->Add(bottom_panel_notebook_, 1, wxEXPAND);
    panel_zone->SetSizer(panel_sizer);

    // --- Secondary Sidebar (Task 11) ---
    auto* secondary_sidebar_zone =
        shell_->get_zone_container(layout::WorkbenchZoneId::kSecondarySidebar);
    auto* secondary_sizer = new wxBoxSizer(wxVERTICAL);
    // Left empty for ad-hoc or future plugin usage
    secondary_sidebar_zone->SetSizer(secondary_sizer);

    // --- Main layout ---
    auto* main_sizer = new wxBoxSizer(wxVERTICAL);
    main_sizer->Add(shell_, 1, wxEXPAND);
    SetSizer(main_sizer);

    // Final layout refresh
    shell_->trigger_layout();
}

// --- Mode Switching (Zen / Presentation) ---

void LayoutManager::ToggleZenMode()
{
    SetZenMode(!zen_mode_);
}

void LayoutManager::SetZenMode(bool enable)
{
    if (zen_mode_ == enable || shell_ == nullptr)
        return;

    if (enable)
    {
        pre_zen_state_ = shell_->save_state_to_json();
        shell_->set_zone_visible(layout::WorkbenchZoneId::kActivityBar, false);
        shell_->set_zone_visible(layout::WorkbenchZoneId::kPrimarySidebar, false);
        shell_->set_zone_visible(layout::WorkbenchZoneId::kSecondarySidebar, false);
        shell_->set_zone_visible(layout::WorkbenchZoneId::kPanelArea, false);
        shell_->set_zone_visible(layout::WorkbenchZoneId::kStatusBar, false);
    }
    else
    {
        shell_->load_state_from_json(pre_zen_state_);
    }

    zen_mode_ = enable;

    // Broadcast event
    core::events::ZenModeChangedEvent evt;
    evt.enabled = enable;
    event_bus_.publish(evt);
}

auto LayoutManager::is_zen_mode() const -> bool
{
    return zen_mode_;
}

void LayoutManager::TogglePresentationMode()
{
    SetPresentationMode(!presentation_mode_);
}

void LayoutManager::SetPresentationMode(bool enable)
{
    if (presentation_mode_ == enable || shell_ == nullptr)
        return;

    if (enable)
    {
        pre_presentation_state_ = shell_->save_state_to_json();
        // Presentation mode hides side panels to maximize space but keeps activity bar or
        // status if needed. Usually, just hide sidebars and panels.
        shell_->set_zone_visible(layout::WorkbenchZoneId::kPrimarySidebar, false);
        shell_->set_zone_visible(layout::WorkbenchZoneId::kSecondarySidebar, false);
        shell_->set_zone_visible(layout::WorkbenchZoneId::kPanelArea, false);
    }
    else
    {
        shell_->load_state_from_json(pre_presentation_state_);
    }

    presentation_mode_ = enable;

    // Broadcast event
    core::events::PresentationModeChangedEvent evt;
    evt.enabled = enable;
    event_bus_.publish(evt);
}

auto LayoutManager::is_presentation_mode() const -> bool
{
    return presentation_mode_;
}

// --- Zone access ---

auto LayoutManager::sidebar_container() -> wxWindow*
{
    if (!shell_)
        return nullptr;
    return shell_->get_zone_container(layout::WorkbenchZoneId::kPrimarySidebar);
}

auto LayoutManager::content_container() -> wxWindow*
{
    if (!shell_)
        return nullptr;
    return shell_->get_zone_container(layout::WorkbenchZoneId::kEditorArea);
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
}

// Fix 15: Forward workspace root to file tree for relative path computation
void LayoutManager::SetWorkspaceRoot(const std::string& root_path)
{
    if (file_tree_ != nullptr)
    {
        file_tree_->SetWorkspaceRoot(root_path);
    }
}

// --- Sidebar control ---

void LayoutManager::toggle_sidebar()
{
    set_sidebar_visible(!is_sidebar_visible());
}

void LayoutManager::set_sidebar_visible(bool visible)
{
    if (shell_)
        shell_->set_zone_visible(layout::WorkbenchZoneId::kPrimarySidebar, visible);
    SaveLayoutState();
}

auto LayoutManager::is_sidebar_visible() const -> bool
{
    if (shell_)
        return shell_->is_zone_visible(layout::WorkbenchZoneId::kPrimarySidebar);
    return true;
}

void LayoutManager::set_sidebar_width(int /*width*/)
{
    // Width managed by resize handle
}

auto LayoutManager::sidebar_width() const -> int
{
    // Getting width from shell is not natively hooked yet, return default
    return kDefaultSidebarWidth;
}

void LayoutManager::ShowBottomPanel(bool show)
{
    if (shell_)
        shell_->set_zone_visible(layout::WorkbenchZoneId::kPanelArea, show);
}

auto LayoutManager::is_bottom_panel_visible() const -> bool
{
    if (shell_)
        return shell_->is_zone_visible(layout::WorkbenchZoneId::kPanelArea);
    return false;
}

// --- Theme ---

void LayoutManager::OnThemeChanged(const core::Theme& new_theme)
{
    ThemeAwareWindow::OnThemeChanged(new_theme);

    if (search_field_ != nullptr)
    {
        search_field_->SetBackgroundColour(
            theme_engine()
                .resolve_token("sidebar.bg")
                .value_or(theme_engine().color(core::ThemeColorToken::BgPanel))
                .ChangeLightness(110));
        search_field_->SetForegroundColour(
            theme_engine()
                .resolve_token("text.main")
                .value_or(theme_engine().color(core::ThemeColorToken::TextMain)));
        search_field_->Refresh();
    }
}

// --- Persistence ---

void LayoutManager::SaveLayoutState()
{
    if (config_ == nullptr || shell_ == nullptr)
    {
        return;
    }
    config_->set("layout.workbench_state", shell_->save_state_to_json().dump());

    // Phase 06 Task 9: Persist active sidebar mode
    config_->set("layout.sidebar_mode", sidebar_mode_);
    // Fix 15: Persist active file path for restore on next launch
    config_->set("workspace.last_active_file", active_file_path_);
}

void LayoutManager::RestoreLayoutState()
{
    if (config_ == nullptr || shell_ == nullptr)
    {
        return;
    }

    auto state_str = config_->get_string("layout.workbench_state", "");
    if (!state_str.empty())
    {
        try
        {
            shell_->load_state_from_json(nlohmann::json::parse(state_str));
        }
        catch (...)
        {
        }
    }

    // Phase 06 Task 9: Restore active sidebar mode
    std::string saved_mode = config_->get_string("layout.sidebar_mode", kSidebarModeExplorer);
    sidebar_mode_ = saved_mode;
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

// --- Phase 8 / Phase 06: Sidebar panel switching ---

// Phase 06 Task 2/4: Register built-in sidebar panels with factories
void LayoutManager::RegisterSidebarPanels()
{
    // Explorer panel is created eagerly (already exists in CreateLayout)
    panel_registry_.Register(kSidebarModeExplorer,
                             "EXPLORER",
                             "\xF0\x9F\x93\x81", // 📁
                             [this](wxWindow* /*parent*/) -> wxPanel* { return explorer_panel_; });

    // Extensions panel is created lazily
    panel_registry_.Register(
        kSidebarModeExtensions,
        "EXTENSIONS",
        "\xF0\x9F\xA7\xA9", // 🧩
        [this](wxWindow* parent) -> wxPanel*
        {
            if (ext_mgmt_service_ == nullptr || ext_gallery_service_ == nullptr)
            {
                return nullptr;
            }
            extensions_panel_ = new ExtensionsBrowserPanel(parent,
                                                           theme_engine(),
                                                           event_bus_,
                                                           *ext_mgmt_service_,
                                                           *ext_gallery_service_,
                                                           *ds_context_,
                                                           IconManager::get());
            auto* sidebar_sizer = sidebar_container()->GetSizer();
            if (sidebar_sizer != nullptr)
            {
                sidebar_sizer->Add(extensions_panel_, 1, wxEXPAND);
            }
            return extensions_panel_;
        });

    // ── Helper: Build a functional sidebar panel with header, toolbar and content ──
    auto make_feature_panel = [this](wxWindow* parent,
                                     const wxString& title,
                                     const wxString& icon,
                                     const std::vector<wxString>& toolbar_labels,
                                     const std::vector<wxString>& list_items,
                                     const wxString& empty_msg) -> wxPanel*
    {
        auto* panel = new wxPanel(parent, wxID_ANY);
        panel->SetBackgroundColour(
            theme_engine()
                .resolve_token("sidebar.bg")
                .value_or(theme_engine().color(core::ThemeColorToken::BgPanel)));
        auto* sizer = new wxBoxSizer(wxVERTICAL);

        // ── Header ──
        auto* hdr = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 36));
        hdr->SetBackgroundColour(theme_engine()
                                     .resolve_token("sidebar.bg")
                                     .value_or(theme_engine().color(core::ThemeColorToken::BgPanel))
                                     .ChangeLightness(108));
        auto* hdr_sizer = new wxBoxSizer(wxHORIZONTAL);
        hdr_sizer->AddSpacer(8);
        auto* icon_lbl = new wxStaticText(hdr, wxID_ANY, icon);
        icon_lbl->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(1.1f));
        icon_lbl->SetForegroundColour(
            theme_engine()
                .resolve_token("accent.primary")
                .value_or(theme_engine().color(core::ThemeColorToken::AccentPrimary)));
        hdr_sizer->Add(icon_lbl, 0, wxALIGN_CENTER_VERTICAL);
        hdr_sizer->AddSpacer(6);
        auto* title_lbl = new wxStaticText(hdr, wxID_ANY, title);
        title_lbl->SetFont(
            theme_engine().font(core::ThemeFontToken::MonoRegular).Bold().Scaled(0.85f));
        title_lbl->SetForegroundColour(
            theme_engine()
                .resolve_token("text.main")
                .value_or(theme_engine().color(core::ThemeColorToken::TextMain)));
        hdr_sizer->Add(title_lbl, 1, wxALIGN_CENTER_VERTICAL);
        hdr->SetSizer(hdr_sizer);
        sizer->Add(hdr, 0, wxEXPAND);

        // ── Toolbar buttons ──
        if (!toolbar_labels.empty())
        {
            auto* tb_panel = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 32));
            tb_panel->SetBackgroundColour(
                theme_engine()
                    .resolve_token("sidebar.bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel)));
            auto* tb_sizer = new wxBoxSizer(wxHORIZONTAL);
            tb_sizer->AddSpacer(8);
            for (const auto& btn_label : toolbar_labels)
            {
                auto* btn = new wxButton(tb_panel,
                                         wxID_ANY,
                                         btn_label,
                                         wxDefaultPosition,
                                         wxSize(-1, 26),
                                         wxBORDER_NONE);
                btn->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(0.75f));
                btn->SetForegroundColour(
                    theme_engine()
                        .resolve_token("accent.primary")
                        .value_or(theme_engine().color(core::ThemeColorToken::AccentPrimary)));
                btn->SetBackgroundColour(
                    theme_engine()
                        .resolve_token("sidebar.bg")
                        .value_or(theme_engine().color(core::ThemeColorToken::BgPanel)));
                tb_sizer->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
            }
            tb_panel->SetSizer(tb_sizer);
            sizer->Add(tb_panel, 0, wxEXPAND);
        }

        // ── Content: list items or empty state ──
        if (!list_items.empty())
        {
            auto* list = new wxListBox(panel, wxID_ANY);
            list->SetBackgroundColour(
                theme_engine()
                    .resolve_token("sidebar.bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel)));
            list->SetForegroundColour(
                theme_engine()
                    .resolve_token("text.main")
                    .value_or(theme_engine().color(core::ThemeColorToken::TextMain)));
            list->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(0.85f));
            for (const auto& item : list_items)
            {
                list->Append(item);
            }
            sizer->Add(list, 1, wxEXPAND | wxALL, 4);
        }
        else
        {
            auto* empty_lbl = new wxStaticText(panel, wxID_ANY, empty_msg);
            empty_lbl->SetForegroundColour(
                theme_engine()
                    .resolve_token("text.muted")
                    .value_or(theme_engine().color(core::ThemeColorToken::TextMuted)));
            empty_lbl->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(0.8f));
            sizer->AddStretchSpacer();
            sizer->Add(empty_lbl, 0, wxALIGN_CENTER | wxALL, 16);
            sizer->AddStretchSpacer();
        }

        panel->SetSizer(sizer);
        auto* sidebar_sizer = sidebar_container()->GetSizer();
        if (sidebar_sizer != nullptr)
        {
            sidebar_sizer->Add(panel, 1, wxEXPAND);
        }
        return panel;
    };

    // ── Search panel ──
    panel_registry_.Register(
        kSidebarModeSearch,
        "SEARCH",
        "\xF0\x9F\x94\x8D", // 🔍
        [this](wxWindow* parent) -> wxPanel*
        {
            auto* search_panel = new SearchSidebarPanel(
                parent, theme_engine(), event_bus_, config_, *ds_context_, IconManager::get());

            auto* sidebar_sizer = sidebar_container()->GetSizer();
            if (sidebar_sizer != nullptr)
            {
                sidebar_sizer->Add(search_panel, 1, wxEXPAND);
            }
            return search_panel;
        });

    // ── Settings panel ──
    panel_registry_.Register(kSidebarModeSettings,
                             "SETTINGS",
                             "\xE2\x9A\x99", // ⚙
                             [make_feature_panel](wxWindow* parent) -> wxPanel*
                             {
                                 return make_feature_panel(
                                     parent,
                                     "SETTINGS",
                                     "\xE2\x9A\x99",
                                     {"General", "Editor", "Theme", "Keys", "Extensions"},
                                     {"General",
                                      "Editor",
                                      "Appearance",
                                      "Keyboard Shortcuts",
                                      "Extensions",
                                      "Files",
                                      "Markdown",
                                      "Preview",
                                      "Canvas",
                                      "Notebooks",
                                      "AI Assistant",
                                      "Git",
                                      "Cloud Sync",
                                      "Privacy & Security",
                                      "Advanced"},
                                     "");
                             });

    // ── Themes panel ──
    panel_registry_.Register(kSidebarModeThemes,
                             "THEMES",
                             "\xF0\x9F\x8E\xA8", // 🎨
                             [make_feature_panel](wxWindow* parent) -> wxPanel*
                             {
                                 return make_feature_panel(parent,
                                                           "THEMES",
                                                           "\xF0\x9F\x8E\xA8",
                                                           {"Import", "Export", "Create"},
                                                           {"Midnight Neon",
                                                            "Cyber Night",
                                                            "Solarized Dark",
                                                            "Classic Mono",
                                                            "High Contrast Blue",
                                                            "Matrix Core",
                                                            "Classic Amp",
                                                            "Vapor Wave"},
                                                           "");
                             });

    // ── Notebooks panel ──
    panel_registry_.Register(
        kSidebarModeNotebooks,
        "NOTEBOOKS",
        "\xF0\x9F\x93\x93", // 📓
        [make_feature_panel](wxWindow* parent) -> wxPanel*
        {
            return make_feature_panel(
                parent,
                "NOTEBOOKS",
                "\xF0\x9F\x93\x93",
                {"New", "Run Cell", "Run All", "Clear", "Export"},
                {},
                "No notebooks open.\nCreate a new notebook or open an existing one.");
        });

    // ── Canvas panel ──
    panel_registry_.Register(kSidebarModeCanvas,
                             "CANVAS",
                             "\xF0\x9F\x96\xBC", // 🖼
                             [make_feature_panel](wxWindow* parent) -> wxPanel*
                             {
                                 return make_feature_panel(
                                     parent,
                                     "CANVAS",
                                     "\xF0\x9F\x96\xBC",
                                     {"New Board", "Templates", "Export"},
                                     {},
                                     "No boards open.\nCreate a new board to start designing.");
                             });

    // ── Graph panel ──
    panel_registry_.Register(kSidebarModeGraph,
                             "KNOWLEDGE GRAPH",
                             "\xF0\x9F\x95\xB8", // 🕸
                             [make_feature_panel](wxWindow* parent) -> wxPanel*
                             {
                                 return make_feature_panel(
                                     parent,
                                     "KNOWLEDGE GRAPH",
                                     "\xF0\x9F\x95\xB8",
                                     {"Global", "Local", "Backlinks", "Search", "Export"},
                                     {},
                                     "Select a document to visualize its\nknowledge graph "
                                     "connections and\nlocal network.");
                             });

    // ── AI Assistant panel ──
    panel_registry_.Register(
        kSidebarModeAI,
        "AI ASSISTANT",
        "\xF0\x9F\xA4\x96", // 🤖
        [this](wxWindow* parent) -> wxPanel*
        {
            auto* panel = new wxPanel(parent, wxID_ANY);
            panel->SetBackgroundColour(
                theme_engine()
                    .resolve_token("sidebar.bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel)));
            auto* sizer = new wxBoxSizer(wxVERTICAL);

            // Header
            auto* hdr = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 36));
            hdr->SetBackgroundColour(
                theme_engine()
                    .resolve_token("sidebar.bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel))
                    .ChangeLightness(108));
            auto* hdr_sizer = new wxBoxSizer(wxHORIZONTAL);
            hdr_sizer->AddSpacer(8);
            auto* icon_lbl = new wxStaticText(hdr, wxID_ANY, "\xF0\x9F\xA4\x96");
            icon_lbl->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(1.1f));
            icon_lbl->SetForegroundColour(
                theme_engine()
                    .resolve_token("accent.primary")
                    .value_or(theme_engine().color(core::ThemeColorToken::AccentPrimary)));
            hdr_sizer->Add(icon_lbl, 0, wxALIGN_CENTER_VERTICAL);
            hdr_sizer->AddSpacer(6);
            auto* title_lbl = new wxStaticText(hdr, wxID_ANY, "AI ASSISTANT");
            title_lbl->SetFont(
                theme_engine().font(core::ThemeFontToken::MonoRegular).Bold().Scaled(0.85f));
            title_lbl->SetForegroundColour(
                theme_engine()
                    .resolve_token("text.main")
                    .value_or(theme_engine().color(core::ThemeColorToken::TextMain)));
            hdr_sizer->Add(title_lbl, 1, wxALIGN_CENTER_VERTICAL);
            hdr->SetSizer(hdr_sizer);
            sizer->Add(hdr, 0, wxEXPAND);

            // Action buttons
            auto* actions = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 34));
            actions->SetBackgroundColour(
                theme_engine()
                    .resolve_token("sidebar.bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel)));
            auto* act_sizer = new wxBoxSizer(wxHORIZONTAL);
            act_sizer->AddSpacer(8);
            for (const auto& lbl_text : {"Summarize", "Translate", "Expand", "Simplify", "Grammar"})
            {
                auto* btn = new wxButton(
                    actions, wxID_ANY, lbl_text, wxDefaultPosition, wxSize(-1, 26), wxBORDER_NONE);
                btn->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(0.7f));
                btn->SetForegroundColour(
                    theme_engine()
                        .resolve_token("accent.primary")
                        .value_or(theme_engine().color(core::ThemeColorToken::AccentPrimary)));
                btn->SetBackgroundColour(
                    theme_engine()
                        .resolve_token("sidebar.bg")
                        .value_or(theme_engine().color(core::ThemeColorToken::BgPanel)));
                act_sizer->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 2);
            }
            actions->SetSizer(act_sizer);
            sizer->Add(actions, 0, wxEXPAND);

            // Chat area (scrollable)
            auto* chat_area = new wxTextCtrl(panel,
                                             wxID_ANY,
                                             "",
                                             wxDefaultPosition,
                                             wxDefaultSize,
                                             wxTE_MULTILINE | wxTE_READONLY | wxBORDER_NONE);
            chat_area->SetBackgroundColour(
                theme_engine()
                    .resolve_token("sidebar.bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel)));
            chat_area->SetForegroundColour(
                theme_engine()
                    .resolve_token("text.main")
                    .value_or(theme_engine().color(core::ThemeColorToken::TextMain)));
            chat_area->SetFont(
                theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(0.85f));
            chat_area->SetValue("Welcome to MarkAmp AI Assistant.\n\n"
                                "Select text and use an action, or type\n"
                                "a message below to start a conversation.");
            sizer->Add(chat_area, 1, wxEXPAND | wxALL, 4);

            // Input area
            auto* input_sizer = new wxBoxSizer(wxHORIZONTAL);
            auto* input = new wxTextCtrl(panel, wxID_ANY, "", wxDefaultPosition, wxSize(-1, 28));
            input->SetHint("Ask AI\u2026");
            input->SetBackgroundColour(
                theme_engine()
                    .resolve_token("sidebar.bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel))
                    .ChangeLightness(115));
            input->SetForegroundColour(
                theme_engine()
                    .resolve_token("text.main")
                    .value_or(theme_engine().color(core::ThemeColorToken::TextMain)));
            input->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular));
            input_sizer->Add(input, 1, wxALIGN_CENTER_VERTICAL | wxLEFT, 4);
            auto* send_btn = new wxButton(
                panel, wxID_ANY, "\xE2\x96\xB6", wxDefaultPosition, wxSize(32, 28), wxBORDER_NONE);
            send_btn->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular));
            send_btn->SetForegroundColour(
                theme_engine()
                    .resolve_token("accent.primary")
                    .value_or(theme_engine().color(core::ThemeColorToken::AccentPrimary)));
            send_btn->SetBackgroundColour(
                theme_engine()
                    .resolve_token("sidebar.bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel))
                    .ChangeLightness(115));
            input_sizer->Add(send_btn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 4);
            sizer->Add(input_sizer, 0, wxEXPAND | wxBOTTOM, 4);

            panel->SetSizer(sizer);
            auto* sidebar_sizer = sidebar_container()->GetSizer();
            if (sidebar_sizer != nullptr)
            {
                sidebar_sizer->Add(panel, 1, wxEXPAND);
            }
            return panel;
        });

    // ── Flashcards panel ──
    panel_registry_.Register(kSidebarModeFlashcards,
                             "FLASHCARDS",
                             "\xF0\x9F\x83\x8F", // 🃏
                             [make_feature_panel](wxWindow* parent) -> wxPanel*
                             {
                                 return make_feature_panel(
                                     parent,
                                     "FLASHCARDS",
                                     "\xF0\x9F\x83\x8F",
                                     {"Review", "Browse", "Create Deck", "Import", "Export"},
                                     {},
                                     "No flashcard decks.\nCreate a deck or extract "
                                     "flashcards\nfrom the current document.");
                             });

    // ── Git panel ──
    panel_registry_.Register(
        kSidebarModeGit,
        "SOURCE CONTROL",
        "\xF0\x9F\x94\x80", // 🔀
        [this](wxWindow* parent) -> wxPanel*
        {
            auto* panel = new wxPanel(parent, wxID_ANY);
            panel->SetBackgroundColour(
                theme_engine()
                    .resolve_token("sidebar.bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel)));
            auto* sizer = new wxBoxSizer(wxVERTICAL);

            // Header
            auto* hdr = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 36));
            hdr->SetBackgroundColour(
                theme_engine()
                    .resolve_token("sidebar.bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel))
                    .ChangeLightness(108));
            auto* hdr_sizer = new wxBoxSizer(wxHORIZONTAL);
            hdr_sizer->AddSpacer(8);
            auto* title_lbl = new wxStaticText(hdr, wxID_ANY, "SOURCE CONTROL");
            title_lbl->SetFont(
                theme_engine().font(core::ThemeFontToken::MonoRegular).Bold().Scaled(0.85f));
            title_lbl->SetForegroundColour(
                theme_engine()
                    .resolve_token("text.main")
                    .value_or(theme_engine().color(core::ThemeColorToken::TextMain)));
            hdr_sizer->Add(title_lbl, 1, wxALIGN_CENTER_VERTICAL);
            hdr->SetSizer(hdr_sizer);
            sizer->Add(hdr, 0, wxEXPAND);

            // Commit message input
            auto* commit_input = new wxTextCtrl(
                panel, wxID_ANY, "", wxDefaultPosition, wxSize(-1, 60), wxTE_MULTILINE);
            commit_input->SetHint("Commit message\u2026");
            commit_input->SetBackgroundColour(
                theme_engine()
                    .resolve_token("sidebar.bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel))
                    .ChangeLightness(115));
            commit_input->SetForegroundColour(
                theme_engine()
                    .resolve_token("text.main")
                    .value_or(theme_engine().color(core::ThemeColorToken::TextMain)));
            commit_input->SetFont(
                theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(0.85f));
            sizer->Add(commit_input, 0, wxEXPAND | wxALL, 8);

            // Buttons
            auto* btn_sizer = new wxBoxSizer(wxHORIZONTAL);
            btn_sizer->AddSpacer(8);
            for (const auto& label : {"Commit", "Push", "Pull", "Stash"})
            {
                auto* btn = new wxButton(
                    panel, wxID_ANY, label, wxDefaultPosition, wxSize(-1, 26), wxBORDER_NONE);
                btn->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(0.75f));
                btn->SetForegroundColour(
                    theme_engine()
                        .resolve_token("accent.primary")
                        .value_or(theme_engine().color(core::ThemeColorToken::AccentPrimary)));
                btn->SetBackgroundColour(
                    theme_engine()
                        .resolve_token("sidebar.bg")
                        .value_or(theme_engine().color(core::ThemeColorToken::BgPanel)));
                btn_sizer->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 4);
            }
            sizer->Add(btn_sizer, 0, wxEXPAND);

            // Sections: Staged, Changes
            auto add_section_label = [&](const wxString& text)
            {
                auto* lbl = new wxStaticText(panel, wxID_ANY, text);
                lbl->SetFont(
                    theme_engine().font(core::ThemeFontToken::MonoRegular).Bold().Scaled(0.8f));
                lbl->SetForegroundColour(
                    theme_engine()
                        .resolve_token("text.muted")
                        .value_or(theme_engine().color(core::ThemeColorToken::TextMuted)));
                sizer->Add(lbl, 0, wxLEFT | wxTOP, 8);
            };

            add_section_label("STAGED CHANGES");
            auto* staged_list = new wxListBox(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 60));
            staged_list->SetBackgroundColour(
                theme_engine()
                    .resolve_token("sidebar.bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel)));
            staged_list->SetForegroundColour(
                theme_engine()
                    .resolve_token("text.main")
                    .value_or(theme_engine().color(core::ThemeColorToken::TextMain)));
            staged_list->SetFont(
                theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(0.8f));
            sizer->Add(staged_list, 0, wxEXPAND | wxALL, 4);

            add_section_label("CHANGES");
            auto* changes_list = new wxListBox(panel, wxID_ANY);
            changes_list->SetBackgroundColour(
                theme_engine()
                    .resolve_token("sidebar.bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel)));
            changes_list->SetForegroundColour(
                theme_engine()
                    .resolve_token("text.main")
                    .value_or(theme_engine().color(core::ThemeColorToken::TextMain)));
            changes_list->SetFont(
                theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(0.8f));
            sizer->Add(changes_list, 1, wxEXPAND | wxALL, 4);

            panel->SetSizer(sizer);
            auto* sidebar_sizer = sidebar_container()->GetSizer();
            if (sidebar_sizer != nullptr)
            {
                sidebar_sizer->Add(panel, 1, wxEXPAND);
            }
            return panel;
        });

    // ── Tasks panel ──
    panel_registry_.Register(
        kSidebarModeTasks,
        "TASKS",
        "\xE2\x9C\x85", // ✅
        [make_feature_panel](wxWindow* parent) -> wxPanel*
        {
            return make_feature_panel(
                parent,
                "TASKS",
                "\xE2\x9C\x85",
                {"New Task", "Board", "Calendar", "Gantt"},
                {},
                "No tasks yet.\nCreate a task or scan documents\nfor task items.");
        });

    // ── Database panel ──
    panel_registry_.Register(
        kSidebarModeDatabase,
        "DATABASE",
        "\xF0\x9F\x97\x84", // 🗄
        [make_feature_panel](wxWindow* parent) -> wxPanel*
        {
            return make_feature_panel(
                parent,
                "DATABASE",
                "\xF0\x9F\x97\x84",
                {"New DB", "Table", "Gallery", "Kanban", "Timeline", "Import"},
                {},
                "No databases.\nCreate a Notion-style database\nwith properties and views.");
        });

    // ── Presentation panel ──
    panel_registry_.Register(kSidebarModePresentation,
                             "PRESENTATION",
                             "\xF0\x9F\x93\xBD", // 📽
                             [make_feature_panel](wxWindow* parent) -> wxPanel*
                             {
                                 return make_feature_panel(
                                     parent,
                                     "PRESENTATION",
                                     "\xF0\x9F\x93\xBD",
                                     {"Present", "Next", "Previous", "Export PDF"},
                                     {},
                                     "No presentation active.\nOpen a Markdown file with "
                                     "slide\ndelimiters to start presenting.");
                             });
}

void LayoutManager::SetSidebarMode(SidebarMode mode)
{
    if (mode == sidebar_mode_)
    {
        return;
    }

    auto previous_mode = sidebar_mode_;
    sidebar_mode_ = mode;

    // Task 18: Load preferred width from config
    int saved_width = config_->get_int("layout.sidebar.width." + mode, -1);
    if (saved_width > 0)
    {
        // Actually, LayoutManager doesn't expose layout_model_ directly, but we can resize via
        // shell_ if there is a way. Or we can just use set_sidebar_width
        set_sidebar_width(saved_width);
    }

    if (sidebar_transition_mgr_ == nullptr && sidebar_container() != nullptr)
    {
        sidebar_transition_mgr_ =
            std::make_unique<animation::TransitionManager>(sidebar_container());
    }

    auto* target_panel = panel_registry_.GetOrCreate(mode, sidebar_container());
    auto* old_panel = panel_registry_.GetOrCreate(previous_mode, sidebar_container());

    if (sidebar_transition_mgr_ && target_panel && old_panel && target_panel != old_panel)
    {
        // Simple crossfade: old panel fades out, then hide it and show new panel fading in
        sidebar_transition_mgr_->stop_all();

        // Freeze layout
        sidebar_container()->Freeze();

        old_panel->Hide();
        target_panel->SetTransparent(0);
        target_panel->Show();

        if (mode == kSidebarModeExtensions && extensions_panel_ != nullptr)
        {
            extensions_panel_->ShowInstalledExtensions();
        }

        sidebar_container()->Layout();
        sidebar_container()->Thaw();

        // Fade in new panel
        sidebar_transition_mgr_->start<int>("sidebar_fade",
                                            0,
                                            255,
                                            [target_panel](const int& val)
                                            {
                                                if (target_panel)
                                                    target_panel->SetTransparent(
                                                        static_cast<wxByte>(val));
                                            });
    }
    else
    {
        // Fallback or initialization
        for (const auto& registered_mode : panel_registry_.AllModes())
        {
            auto* p = panel_registry_.GetOrCreate(registered_mode, sidebar_container());
            if (p != nullptr && p != target_panel)
            {
                p->Hide();
            }
        }

        if (target_panel != nullptr)
        {
            target_panel->SetTransparent(255);
            target_panel->Show();

            if (mode == kSidebarModeExtensions && extensions_panel_ != nullptr)
            {
                extensions_panel_->ShowInstalledExtensions();
            }
        }

        sidebar_container()->Layout();
        sidebar_container()->Refresh();
    }

    // Phase 06 Task 17: Sidebar transition animations should be handled via CSS or layout rules

    // Phase 06 Task 8: Broadcast SidebarModeChangedEvent
    core::events::SidebarModeChangedEvent changed_evt;
    changed_evt.previous_mode = previous_mode;
    changed_evt.new_mode = mode;
    event_bus_.publish(changed_evt);
}

auto LayoutManager::GetSidebarMode() const -> SidebarMode
{
    return sidebar_mode_;
}

// Phase 06 Task 11: Toggle secondary sidebar visibility
void LayoutManager::ToggleSecondarySidebar()
{
    if (!shell_)
        return;
    bool visible = shell_->is_zone_visible(layout::WorkbenchZoneId::kSecondarySidebar);
    shell_->set_zone_visible(layout::WorkbenchZoneId::kSecondarySidebar, !visible);
}

void LayoutManager::SetSecondarySidebarMode(SidebarMode mode)
{
    secondary_sidebar_mode_ = mode;
    // Future: swap secondary panel content based on mode
}

auto LayoutManager::is_secondary_sidebar_visible() const -> bool
{
    if (!shell_)
        return false;
    return shell_->is_zone_visible(layout::WorkbenchZoneId::kSecondarySidebar);
}

auto LayoutManager::sidebar_panel_registry() -> SidebarPanelRegistry&
{
    return panel_registry_;
}

void LayoutManager::SetExtensionServices(core::IExtensionManagementService* mgmt_service,
                                         core::IExtensionGalleryService* gallery_service)
{
    ext_mgmt_service_ = mgmt_service;
    ext_gallery_service_ = gallery_service;
}

// --- V8 Phase 6: Canvas mode switching ---

void LayoutManager::ShowCanvasWorkspace()
{
    if (canvas_mode_)
    {
        return;
    }

    // Create canvas workspace lazily on first use
    if (canvas_workspace_ == nullptr)
    {
        canvas_workspace_ = new CanvasWorkspacePanel(this, event_bus_, theme_engine(), config_);
    }

    // Hide editor content, show canvas workspace
    if (auto* c = content_container())
    {
        c->Hide();
    }
    canvas_workspace_->Show();

    // Replace content in body sizer
    Layout();

    canvas_mode_ = true;

    core::events::CanvasModeActivatedEvent evt;
    event_bus_.publish(evt);

    MARKAMP_LOG_INFO("Switched to canvas mode");
}

void LayoutManager::ShowEditorWorkspace()
{
    if (!canvas_mode_)
    {
        return;
    }

    // Hide canvas workspace, show editor content
    if (canvas_workspace_ != nullptr)
    {
        canvas_workspace_->Hide();
        // body_sizer_ removed, rely on main Layout()
    }
    if (auto* c = content_container())
    {
        c->Show();
    }
    Layout();

    canvas_mode_ = false;

    core::events::CanvasModeDeactivatedEvent evt;
    event_bus_.publish(evt);

    MARKAMP_LOG_INFO("Switched to editor mode");
}

auto LayoutManager::is_canvas_mode() const -> bool
{
    return canvas_mode_;
}

auto LayoutManager::canvas_workspace() -> CanvasWorkspacePanel*
{
    return canvas_workspace_;
}

// ═══════════════════════════════════════════════════════
// V8 Phase 11: Unified workbench mode
// ═══════════════════════════════════════════════════════

void LayoutManager::SetWorkbenchMode(core::events::WorkbenchMode mode)
{
    if (mode == workbench_mode_)
    {
        return;
    }

    auto previous = workbench_mode_;
    workbench_mode_ = mode;

    // Map workbench modes to canvas/editor switching
    if (mode == core::events::WorkbenchMode::kCanvas)
    {
        ShowCanvasWorkspace();
    }
    else if (previous == core::events::WorkbenchMode::kCanvas)
    {
        ShowEditorWorkspace();
    }

    core::events::WorkbenchModeChangedEvent evt;
    evt.previous_mode = previous;
    evt.new_mode = mode;
    event_bus_.publish(evt);
}

auto LayoutManager::GetWorkbenchMode() const -> core::events::WorkbenchMode
{
    return workbench_mode_;
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
void LayoutManager::FocusEditor()
{
    if (split_view_ != nullptr)
    {
        auto* editor = split_view_->GetEditorPanel();
        if (editor != nullptr)
        {
            editor->SetFocus();
        }
    }
}

} // namespace markamp::ui
