#include "LayoutManager.h"

#include "ActivityBar.h"
#include "BreadcrumbBar.h"
#include "BuildPanel.h"
#include "CanvasWorkspacePanel.h"
#include "DebugConsolePanel.h"
#include "EditorPanel.h"
#include "OutputPanel.h"
#include "ProblemsPanel.h"
#include "SplitView.h"
#include "StatusBarPanel.h"
#include "TabBar.h"
#include "TaskListPanel.h"
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
#include "ui/PanelAreaModel.h"
#include "ui/PanelContainer.h"
#include "ui/SearchSidebarPanel.h"
#include "ui/SecondarySidebarTabStrip.h"
#include "ui/SidebarHeader.h"
#include "ui/SourceControlPanel.h"
#include "ui/TerminalPanel.h"

#include <nlohmann/json.hpp>
#include <wx/button.h>
#include <wx/checkbox.h>
#include <wx/clipbrd.h>
#include <wx/dcbuffer.h>
#include <wx/filedlg.h>
#include <wx/listbox.h>
#include <wx/menu.h>
#include <wx/msgdlg.h>
#include <wx/scrolwin.h>
#include <wx/sizer.h>
#include <wx/treectrl.h>

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
                             core::IMathRenderer* math_renderer,
                             PanelAreaModel* panel_area_model)
    : ThemeAwareWindow(
          parent, theme_engine, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL)
    , event_bus_(event_bus)
    , config_(config)
    , feature_registry_(feature_registry)
    , mermaid_renderer_(mermaid_renderer)
    , math_renderer_(math_renderer)
    , panel_area_model_(panel_area_model)
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

    CreateLayout();
    RestoreLayoutState();

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
                    if (GetActiveTabBar() != nullptr)
                    {
                        GetActiveTabBar()->SetTabModified(active_file_path_, true);
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
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->ShowFindBar();
                }
            }
        });

    replace_sub_ = event_bus_.subscribe<core::events::ReplaceRequestEvent>(
        [this](const core::events::ReplaceRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->ShowFindBar();
                }
            }
        });

    dup_line_sub_ = event_bus_.subscribe<core::events::DuplicateLineRequestEvent>(
        [this](const core::events::DuplicateLineRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->GetStyledTextCtrl()->LineDuplicate();
                }
            }
        });

    toggle_comment_sub_ = event_bus_.subscribe<core::events::ToggleCommentRequestEvent>(
        [this](const core::events::ToggleCommentRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
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
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->GetStyledTextCtrl()->LineDelete();
                }
            }
        });

    wrap_toggle_sub_ = event_bus_.subscribe<core::events::WrapToggleRequestEvent>(
        [this](const core::events::WrapToggleRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
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
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->MoveLineUp();
                }
            }
        });
    move_line_down_sub_ = event_bus_.subscribe<core::events::MoveLineDownRequestEvent>(
        [this](const core::events::MoveLineDownRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->MoveLineDown();
                }
            }
        });
    join_lines_sub_ = event_bus_.subscribe<core::events::JoinLinesRequestEvent>(
        [this](const core::events::JoinLinesRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->JoinLines();
                }
            }
        });
    sort_asc_sub_ = event_bus_.subscribe<core::events::SortLinesAscRequestEvent>(
        [this](const core::events::SortLinesAscRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->SortLinesAscending();
                }
            }
        });
    sort_desc_sub_ = event_bus_.subscribe<core::events::SortLinesDescRequestEvent>(
        [this](const core::events::SortLinesDescRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->SortLinesDescending();
                }
            }
        });
    transform_upper_sub_ = event_bus_.subscribe<core::events::TransformUpperRequestEvent>(
        [this](const core::events::TransformUpperRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->TransformToUppercase();
                }
            }
        });
    transform_lower_sub_ = event_bus_.subscribe<core::events::TransformLowerRequestEvent>(
        [this](const core::events::TransformLowerRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->TransformToLowercase();
                }
            }
        });
    transform_title_sub_ = event_bus_.subscribe<core::events::TransformTitleRequestEvent>(
        [this](const core::events::TransformTitleRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->TransformToTitleCase();
                }
            }
        });
    select_all_occ_sub_ = event_bus_.subscribe<core::events::SelectAllOccurrencesRequestEvent>(
        [this](const core::events::SelectAllOccurrencesRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->SelectAllOccurrences();
                }
            }
        });
    expand_line_sub_ = event_bus_.subscribe<core::events::ExpandLineSelectionRequestEvent>(
        [this](const core::events::ExpandLineSelectionRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->ExpandLineSelection();
                }
            }
        });
    insert_line_above_sub_ = event_bus_.subscribe<core::events::InsertLineAboveRequestEvent>(
        [this](const core::events::InsertLineAboveRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->InsertLineAbove();
                }
            }
        });
    insert_line_below_sub_ = event_bus_.subscribe<core::events::InsertLineBelowRequestEvent>(
        [this](const core::events::InsertLineBelowRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->InsertLineBelow();
                }
            }
        });
    fold_all_sub_ = event_bus_.subscribe<core::events::FoldAllRequestEvent>(
        [this](const core::events::FoldAllRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->FoldAllRegions();
                }
            }
        });
    unfold_all_sub_ = event_bus_.subscribe<core::events::UnfoldAllRequestEvent>(
        [this](const core::events::UnfoldAllRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->UnfoldAllRegions();
                }
            }
        });
    toggle_line_numbers_sub_ = event_bus_.subscribe<core::events::ToggleLineNumbersRequestEvent>(
        [this](const core::events::ToggleLineNumbersRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->ToggleLineNumbers();
                }
            }
        });
    toggle_whitespace_sub_ = event_bus_.subscribe<core::events::ToggleWhitespaceRequestEvent>(
        [this](const core::events::ToggleWhitespaceRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
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
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->CopyLineUp();
                }
            }
        });
    copy_line_down_sub_ = event_bus_.subscribe<core::events::CopyLineDownRequestEvent>(
        [this](const core::events::CopyLineDownRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->CopyLineDown();
                }
            }
        });
    delete_all_left_sub_ = event_bus_.subscribe<core::events::DeleteAllLeftRequestEvent>(
        [this](const core::events::DeleteAllLeftRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->DeleteAllLeft();
                }
            }
        });
    delete_all_right_sub_ = event_bus_.subscribe<core::events::DeleteAllRightRequestEvent>(
        [this](const core::events::DeleteAllRightRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->DeleteAllRight();
                }
            }
        });
    reverse_lines_sub_ = event_bus_.subscribe<core::events::ReverseLinesRequestEvent>(
        [this](const core::events::ReverseLinesRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->ReverseSelectedLines();
                }
            }
        });
    delete_dup_lines_sub_ = event_bus_.subscribe<core::events::DeleteDuplicateLinesRequestEvent>(
        [this](const core::events::DeleteDuplicateLinesRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->DeleteDuplicateLines();
                }
            }
        });
    transpose_chars_sub_ = event_bus_.subscribe<core::events::TransposeCharsRequestEvent>(
        [this](const core::events::TransposeCharsRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->TransposeCharacters();
                }
            }
        });
    indent_selection_sub_ = event_bus_.subscribe<core::events::IndentSelectionRequestEvent>(
        [this](const core::events::IndentSelectionRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->IndentSelection();
                }
            }
        });
    outdent_selection_sub_ = event_bus_.subscribe<core::events::OutdentSelectionRequestEvent>(
        [this](const core::events::OutdentSelectionRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->OutdentSelection();
                }
            }
        });
    select_word_sub_ = event_bus_.subscribe<core::events::SelectWordRequestEvent>(
        [this](const core::events::SelectWordRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->SelectWordAtCursor();
                }
            }
        });
    select_paragraph_sub_ = event_bus_.subscribe<core::events::SelectParagraphRequestEvent>(
        [this](const core::events::SelectParagraphRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->SelectCurrentParagraph();
                }
            }
        });
    toggle_read_only_sub_ = event_bus_.subscribe<core::events::ToggleReadOnlyRequestEvent>(
        [this](const core::events::ToggleReadOnlyRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
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
                if (GetActiveEditor() != nullptr)
                {
                    auto* editor = GetActiveEditor();
                    if (editor != nullptr)
                    {
                        editor->ConvertIndentationToSpaces();
                    }
                }
            });
    convert_indent_tabs_sub_ = event_bus_.subscribe<core::events::ConvertIndentTabsRequestEvent>(
        [this](const core::events::ConvertIndentTabsRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->ConvertIndentationToTabs();
                }
            }
        });
    jump_to_bracket_sub_ = event_bus_.subscribe<core::events::JumpToBracketRequestEvent>(
        [this](const core::events::JumpToBracketRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->JumpToMatchingBracket();
                }
            }
        });
    toggle_minimap_sub_ = event_bus_.subscribe<core::events::ToggleMinimapRequestEvent>(
        [this](const core::events::ToggleMinimapRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->ToggleMinimapVisibility();
                }
            }
        });
    fold_current_sub_ = event_bus_.subscribe<core::events::FoldCurrentRequestEvent>(
        [this](const core::events::FoldCurrentRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->FoldCurrentRegion();
                }
            }
        });
    unfold_current_sub_ = event_bus_.subscribe<core::events::UnfoldCurrentRequestEvent>(
        [this](const core::events::UnfoldCurrentRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->UnfoldCurrentRegion();
                }
            }
        });
    add_line_comment_sub_ = event_bus_.subscribe<core::events::AddLineCommentRequestEvent>(
        [this](const core::events::AddLineCommentRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->AddLineComment();
                }
            }
        });
    remove_line_comment_sub_ = event_bus_.subscribe<core::events::RemoveLineCommentRequestEvent>(
        [this](const core::events::RemoveLineCommentRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
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
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->TrimTrailingWhitespaceNow();
                }
            }
        });

    expand_selection_sub_ = event_bus_.subscribe<core::events::ExpandSelectionRequestEvent>(
        [this](const core::events::ExpandSelectionRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->ExpandSelection();
                }
            }
        });

    shrink_selection_sub_ = event_bus_.subscribe<core::events::ShrinkSelectionRequestEvent>(
        [this](const core::events::ShrinkSelectionRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->ShrinkSelection();
                }
            }
        });

    cursor_undo_sub_ = event_bus_.subscribe<core::events::CursorUndoRequestEvent>(
        [this](const core::events::CursorUndoRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->CursorUndo();
                }
            }
        });

    cursor_redo_sub_ = event_bus_.subscribe<core::events::CursorRedoRequestEvent>(
        [this](const core::events::CursorRedoRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->CursorRedo();
                }
            }
        });

    move_text_left_sub_ = event_bus_.subscribe<core::events::MoveTextLeftRequestEvent>(
        [this](const core::events::MoveTextLeftRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->MoveSelectedTextLeft();
                }
            }
        });

    move_text_right_sub_ = event_bus_.subscribe<core::events::MoveTextRightRequestEvent>(
        [this](const core::events::MoveTextRightRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->MoveSelectedTextRight();
                }
            }
        });

    toggle_auto_indent_sub_ = event_bus_.subscribe<core::events::ToggleAutoIndentRequestEvent>(
        [this](const core::events::ToggleAutoIndentRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
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
                if (GetActiveEditor() != nullptr)
                {
                    auto* editor = GetActiveEditor();
                    if (editor != nullptr)
                    {
                        editor->ToggleBracketMatching();
                    }
                }
            });

    toggle_code_folding_sub_ = event_bus_.subscribe<core::events::ToggleCodeFoldingRequestEvent>(
        [this](const core::events::ToggleCodeFoldingRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->ToggleCodeFolding();
                }
            }
        });

    toggle_indent_guides_sub_ = event_bus_.subscribe<core::events::ToggleIndentGuidesRequestEvent>(
        [this](const core::events::ToggleIndentGuidesRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->ToggleIndentationGuides();
                }
            }
        });

    select_to_bracket_sub_ = event_bus_.subscribe<core::events::SelectToBracketRequestEvent>(
        [this](const core::events::SelectToBracketRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->SelectToMatchingBracket();
                }
            }
        });

    toggle_block_comment_sub_ = event_bus_.subscribe<core::events::ToggleBlockCommentRequestEvent>(
        [this](const core::events::ToggleBlockCommentRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->ToggleBlockComment();
                }
            }
        });

    insert_datetime_sub_ = event_bus_.subscribe<core::events::InsertDateTimeRequestEvent>(
        [this](const core::events::InsertDateTimeRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->InsertDateTime();
                }
            }
        });

    bold_sub_ = event_bus_.subscribe<core::events::BoldRequestEvent>(
        [this](const core::events::BoldRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->ToggleBold();
                }
            }
        });

    italic_sub_ = event_bus_.subscribe<core::events::ItalicRequestEvent>(
        [this](const core::events::ItalicRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->ToggleItalic();
                }
            }
        });

    inline_code_sub_ = event_bus_.subscribe<core::events::InlineCodeRequestEvent>(
        [this](const core::events::InlineCodeRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->ToggleInlineCode();
                }
            }
        });

    blockquote_sub_ = event_bus_.subscribe<core::events::BlockquoteRequestEvent>(
        [this](const core::events::BlockquoteRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->InsertBlockquote();
                }
            }
        });

    cycle_heading_sub_ = event_bus_.subscribe<core::events::CycleHeadingRequestEvent>(
        [this](const core::events::CycleHeadingRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->CycleHeading();
                }
            }
        });

    insert_table_sub_ = event_bus_.subscribe<core::events::InsertTableRequestEvent>(
        [this](const core::events::InsertTableRequestEvent& /*evt*/)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
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
                if (GetActiveEditor() != nullptr)
                {
                    auto* editor = GetActiveEditor();
                    if (editor != nullptr)
                    {
                        editor->SetSmartListContinuation(!editor->GetSmartListContinuation());
                    }
                }
            });

    close_other_tabs_sub_ = event_bus_.subscribe<core::events::CloseOtherTabsRequestEvent>(
        [this]([[maybe_unused]] const core::events::CloseOtherTabsRequestEvent& evt)
        {
            if (GetActiveTabBar() != nullptr && !active_file_path_.empty())
            {
                GetActiveTabBar()->CloseOtherTabs(active_file_path_);
            }
        });

    close_saved_tabs_sub_ = event_bus_.subscribe<core::events::CloseSavedTabsRequestEvent>(
        [this]([[maybe_unused]] const core::events::CloseSavedTabsRequestEvent& evt)
        {
            if (GetActiveTabBar() != nullptr)
            {
                GetActiveTabBar()->CloseSavedTabs();
            }
        });

    insert_link_sub_ = event_bus_.subscribe<core::events::InsertLinkRequestEvent>(
        [this]([[maybe_unused]] const core::events::InsertLinkRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->InsertLink();
                }
            }
        });

    add_cursor_below_sub_ = event_bus_.subscribe<core::events::AddCursorBelowRequestEvent>(
        [this]([[maybe_unused]] const core::events::AddCursorBelowRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->AddCursorBelow();
                }
            }
        });

    add_cursor_above_sub_ = event_bus_.subscribe<core::events::AddCursorAboveRequestEvent>(
        [this]([[maybe_unused]] const core::events::AddCursorAboveRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
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
                if (GetActiveEditor() != nullptr)
                {
                    auto* editor = GetActiveEditor();
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
                if (GetActiveEditor() != nullptr)
                {
                    auto* editor = GetActiveEditor();
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
                if (GetActiveEditor() != nullptr)
                {
                    auto* editor = GetActiveEditor();
                    if (editor != nullptr)
                    {
                        editor->DuplicateSelectionOrLine();
                    }
                }
            });

    show_table_editor_sub_ = event_bus_.subscribe<core::events::ShowTableEditorRequestEvent>(
        [this]([[maybe_unused]] const core::events::ShowTableEditorRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
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
                if (GetActiveEditor() != nullptr)
                {
                    auto* editor = GetActiveEditor();
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
                if (GetActiveEditor() != nullptr)
                {
                    auto* editor = GetActiveEditor();
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
                if (GetActiveEditor() != nullptr)
                {
                    auto* editor = GetActiveEditor();
                    if (editor != nullptr)
                    {
                        editor->SetAutoClosingBrackets(!editor->GetAutoClosingBrackets());
                    }
                }
            });

    toggle_sticky_scroll_sub_ = event_bus_.subscribe<core::events::ToggleStickyScrollRequestEvent>(
        [this]([[maybe_unused]] const core::events::ToggleStickyScrollRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
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
                if (GetActiveEditor() != nullptr)
                {
                    auto* editor = GetActiveEditor();
                    if (editor != nullptr)
                    {
                        editor->SetFontLigatures(!editor->GetFontLigatures());
                    }
                }
            });

    toggle_smooth_caret_sub_ = event_bus_.subscribe<core::events::ToggleSmoothCaretRequestEvent>(
        [this]([[maybe_unused]] const core::events::ToggleSmoothCaretRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
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
                if (GetActiveEditor() != nullptr)
                {
                    auto* editor = GetActiveEditor();
                    if (editor != nullptr)
                    {
                        editor->SetInlineColorPreview(!editor->GetInlineColorPreview());
                    }
                }
            });

    toggle_edge_ruler_sub_ = event_bus_.subscribe<core::events::ToggleEdgeColumnRulerRequestEvent>(
        [this]([[maybe_unused]] const core::events::ToggleEdgeColumnRulerRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->SetShowEdgeColumnRuler(!editor->GetShowEdgeColumnRuler());
                }
            }
        });

    ensure_final_newline_sub_ = event_bus_.subscribe<core::events::EnsureFinalNewlineRequestEvent>(
        [this]([[maybe_unused]] const core::events::EnsureFinalNewlineRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->EnsureFinalNewline();
                }
            }
        });

    insert_snippet_sub_ = event_bus_.subscribe<core::events::InsertSnippetRequestEvent>(
        [this]([[maybe_unused]] const core::events::InsertSnippetRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
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
                if (GetActiveEditor() != nullptr)
                {
                    auto* editor = GetActiveEditor();
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
                if (GetActiveEditor() != nullptr)
                {
                    auto* editor = GetActiveEditor();
                    if (editor != nullptr)
                    {
                        editor->SetTrailingWhitespace(!editor->GetTrailingWhitespace());
                    }
                }
            });

    toggle_auto_trim_ws_sub_ = event_bus_.subscribe<core::events::ToggleAutoTrimWSRequestEvent>(
        [this]([[maybe_unused]] const core::events::ToggleAutoTrimWSRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
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
                if (GetActiveEditor() != nullptr)
                {
                    auto* editor = GetActiveEditor();
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
                if (GetActiveEditor() != nullptr)
                {
                    auto* editor = GetActiveEditor();
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
                if (GetActiveEditor() != nullptr)
                {
                    auto* editor = GetActiveEditor();
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
                if (GetActiveEditor() != nullptr)
                {
                    auto* editor = GetActiveEditor();
                    if (editor != nullptr)
                    {
                        editor->SetLinkAutoComplete(!editor->GetLinkAutoComplete());
                    }
                }
            });

    toggle_drag_drop_sub_ = event_bus_.subscribe<core::events::ToggleDragDropRequestEvent>(
        [this]([[maybe_unused]] const core::events::ToggleDragDropRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->SetDragDropEnabled(!editor->GetDragDropEnabled());
                }
            }
        });

    toggle_auto_save_sub_ = event_bus_.subscribe<core::events::ToggleAutoSaveRequestEvent>(
        [this]([[maybe_unused]] const core::events::ToggleAutoSaveRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
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
                if (GetActiveEditor() != nullptr)
                {
                    auto* editor = GetActiveEditor();
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
                if (GetActiveEditor() != nullptr)
                {
                    auto* editor = GetActiveEditor();
                    if (editor != nullptr)
                    {
                        editor->ToggleRenderWhitespace();
                    }
                }
            });

    delete_current_line_sub_ = event_bus_.subscribe<core::events::DeleteLineRequestEvent>(
        [this]([[maybe_unused]] const core::events::DeleteLineRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->DeleteCurrentLine();
                }
            }
        });

    copy_line_no_sel_sub_ = event_bus_.subscribe<core::events::CopyLineNoSelRequestEvent>(
        [this]([[maybe_unused]] const core::events::CopyLineNoSelRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->CopyLineIfNoSelection();
                }
            }
        });

    add_sel_next_match_sub_ = event_bus_.subscribe<core::events::AddSelNextMatchRequestEvent>(
        [this]([[maybe_unused]] const core::events::AddSelNextMatchRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->AddSelectionToNextFindMatch();
                }
            }
        });

    smart_backspace_sub_ = event_bus_.subscribe<core::events::SmartBackspaceRequestEvent>(
        [this]([[maybe_unused]] const core::events::SmartBackspaceRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->SmartBackspace();
                }
            }
        });

    hide_table_editor_sub_ = event_bus_.subscribe<core::events::HideTableEditorRequestEvent>(
        [this]([[maybe_unused]] const core::events::HideTableEditorRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->HideTableEditor();
                }
            }
        });

    auto_pair_bold_sub_ = event_bus_.subscribe<core::events::AutoPairBoldRequestEvent>(
        [this]([[maybe_unused]] const core::events::AutoPairBoldRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->AutoPairEmphasis('*');
                }
            }
        });

    auto_pair_italic_sub_ = event_bus_.subscribe<core::events::AutoPairItalicRequestEvent>(
        [this]([[maybe_unused]] const core::events::AutoPairItalicRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->AutoPairEmphasis('_');
                }
            }
        });

    auto_pair_code_sub_ = event_bus_.subscribe<core::events::AutoPairCodeRequestEvent>(
        [this]([[maybe_unused]] const core::events::AutoPairCodeRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->AutoPairEmphasis('`');
                }
            }
        });

    toggle_minimap_r11_sub_ = event_bus_.subscribe<core::events::ToggleMinimapRequestEvent>(
        [this]([[maybe_unused]] const core::events::ToggleMinimapRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
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
            if (GetActiveTabBar() != nullptr)
            {
                GetActiveTabBar()->CloseTabsToLeft(active_file_path_);
            }
        });

    close_tabs_to_right_sub_ = event_bus_.subscribe<core::events::CloseTabsToRightRequestEvent>(
        [this]([[maybe_unused]] const core::events::CloseTabsToRightRequestEvent& evt)
        {
            if (GetActiveTabBar() != nullptr)
            {
                GetActiveTabBar()->CloseTabsToRight(active_file_path_);
            }
        });

    pin_tab_sub_ = event_bus_.subscribe<core::events::PinTabRequestEvent>(
        [this]([[maybe_unused]] const core::events::PinTabRequestEvent& evt)
        {
            if (GetActiveTabBar() != nullptr)
            {
                GetActiveTabBar()->PinTab(active_file_path_);
            }
        });

    unpin_tab_sub_ = event_bus_.subscribe<core::events::UnpinTabRequestEvent>(
        [this]([[maybe_unused]] const core::events::UnpinTabRequestEvent& evt)
        {
            if (GetActiveTabBar() != nullptr)
            {
                GetActiveTabBar()->UnpinTab(active_file_path_);
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
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->ZoomIn();
                }
            }
        });

    zoom_out_sub_ = event_bus_.subscribe<core::events::ZoomOutRequestEvent>(
        [this]([[maybe_unused]] const core::events::ZoomOutRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->ZoomOut();
                }
            }
        });

    zoom_reset_sub_ = event_bus_.subscribe<core::events::ZoomResetRequestEvent>(
        [this]([[maybe_unused]] const core::events::ZoomResetRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->ZoomReset();
                }
            }
        });

    convert_eol_lf_sub_ = event_bus_.subscribe<core::events::ConvertEolLfRequestEvent>(
        [this]([[maybe_unused]] const core::events::ConvertEolLfRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
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
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
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
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->FoldCurrentRegion();
                }
            }
        });

    unfold_current_sub_ = event_bus_.subscribe<core::events::UnfoldCurrentRequestEvent>(
        [this]([[maybe_unused]] const core::events::UnfoldCurrentRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->UnfoldCurrentRegion();
                }
            }
        });

    jump_to_bracket_sub_ = event_bus_.subscribe<core::events::JumpToBracketRequestEvent>(
        [this]([[maybe_unused]] const core::events::JumpToBracketRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->JumpToMatchingBracket();
                }
            }
        });

    transpose_chars_sub_ = event_bus_.subscribe<core::events::TransposeCharsRequestEvent>(
        [this]([[maybe_unused]] const core::events::TransposeCharsRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->TransposeCharacters();
                }
            }
        });

    reverse_lines_sub_ = event_bus_.subscribe<core::events::ReverseLinesRequestEvent>(
        [this]([[maybe_unused]] const core::events::ReverseLinesRequestEvent& evt)
        {
            if (GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
                if (editor != nullptr)
                {
                    editor->ReverseSelectedLines();
                }
            }
        });

    file_reload_sub_ = event_bus_.subscribe<core::events::FileReloadRequestEvent>(
        [this]([[maybe_unused]] const core::events::FileReloadRequestEvent& evt)
        {
            if (!active_file_path_.empty() && GetActiveEditor() != nullptr)
            {
                auto* editor = GetActiveEditor();
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
    RegisterSecondarySidebarPanels();

    // Phase 09 Task 2: Subscribe to Secondary Sidebar Tab Strip selections
    secondary_sidebar_selection_sub_ =
        event_bus_.subscribe<core::events::SecondarySidebarSelectionEvent>(
            [this](const core::events::SecondarySidebarSelectionEvent& evt)
            {
                // Phase 09 Task 16: Panel Duplication Detection
                if (evt.item != "" && evt.item == GetSidebarMode())
                {
                    MARKAMP_LOG_WARN(
                        "Phase 09 Task 16: Panel '{}' is now duplicated in both sidebars.",
                        evt.item);
                }

                SetSecondarySidebarMode(evt.item);
                if (activity_bar_ != nullptr)
                {
                    activity_bar_->SetSecondaryActiveItem(evt.item);
                }
            });

    // Phase 09 Task 22: Auto-open Outline in Secondary Sidebar
    file_opened_sub_ = event_bus_.subscribe<core::events::FileOpenedEvent>(
        [this]([[maybe_unused]] const core::events::FileOpenedEvent& evt)
        {
            if (GetSidebarMode() == kSidebarModeExplorer)
            {
                if (config_ &&
                    config_->get_bool("workbench.secondarySidebar.autoOpenOutline", true))
                {
                    if (secondary_sidebar_mode_ != kSidebarModeOutline)
                    {
                        if (!is_secondary_sidebar_visible())
                            ToggleSecondarySidebar();
                        SetSecondarySidebarMode(kSidebarModeOutline);
                        if (secondary_tab_strip_ &&
                            !secondary_tab_strip_->HasTab(kSidebarModeOutline))
                        {
                            auto icon = secondary_panel_registry_.GetIconChar(kSidebarModeOutline);
                            auto label = secondary_panel_registry_.GetLabel(kSidebarModeOutline);
                            secondary_tab_strip_->AddTab(kSidebarModeOutline, icon, label);
                        }
                    }
                }
            }
        });

    // Phase 09 Task 6: Panel drag and drop
    sidebar_panel_moved_sub_ = event_bus_.subscribe<core::events::SidebarPanelMovedEvent>(
        [this](const core::events::SidebarPanelMovedEvent& evt)
        {
            if (activity_bar_ == nullptr || secondary_tab_strip_ == nullptr)
                return;

            if (evt.target_sidebar == "primary")
            {
                secondary_tab_strip_->RemoveTab(evt.panel_id);
                activity_bar_->SetItemVisible(evt.panel_id, true);

                SetSidebarMode(evt.panel_id);
                activity_bar_->SetActiveItem(evt.panel_id);
                if (!is_sidebar_visible())
                {
                    set_sidebar_visible(true);
                }
            }
            else if (evt.target_sidebar == "secondary")
            {
                activity_bar_->SetItemVisible(evt.panel_id, false);

                std::string label = secondary_panel_registry_.GetLabel(evt.panel_id);
                if (label.empty())
                {
                    label = panel_registry_.GetLabel(evt.panel_id);
                }

                std::string icon = secondary_panel_registry_.GetIconChar(evt.panel_id);
                if (icon.empty())
                {
                    icon = panel_registry_.GetIconChar(evt.panel_id);
                }

                secondary_tab_strip_->AddTab(evt.panel_id, icon, label);

                SetSecondarySidebarMode(evt.panel_id);
                activity_bar_->SetSecondaryActiveItem(evt.panel_id);
                if (!is_secondary_sidebar_visible())
                {
                    set_secondary_sidebar_visible(true);
                }
            }
        });

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
                if (GetActiveBreadcrumbBar() != nullptr)
                {
                    GetActiveBreadcrumbBar()->Show(evt.enabled);
                    if (auto* c = content_container())
                        c->Layout();
                }
            }
            // Phase 4: Mermaid toggle — forward to SplitView/PreviewPanel
            else if (evt.feature_id == core::builtin_features::kMermaid)
            {
                if (GetActiveEditor() != nullptr)
                {
                    // Improvement 95: Broadcast mermaid toggle to all previews
                    core::events::CommandExecutedEvent mermaid_cmd;
                    mermaid_cmd.command_id = "mermaid.toggleRendering";
                    mermaid_cmd.source = "feature_toggle";
                    event_bus_.publish(mermaid_cmd);
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

    // Phase 10: Panel notifications to Status Bar
    panel_tabs_sub_ = event_bus_.subscribe<core::events::PanelAreaTabsChangedEvent>(
        [this](const auto&) { UpdatePanelNotifications(); });
    panel_badge_sub_ = event_bus_.subscribe<core::events::PanelAreaBadgeChangedEvent>(
        [this](const auto&) { UpdatePanelNotifications(); });

    toggle_bottom_panel_sub_ = event_bus_.subscribe<core::events::ToggleBottomPanelRequestEvent>(
        [this](const auto&) { ShowBottomPanel(!is_bottom_panel_visible()); });

    // Phase 23: Update status bar with diagnostic counts
    diagnostics_changed_sub_ = event_bus_.subscribe<core::events::DiagnosticsChangedEvent>(
        [this](const core::events::DiagnosticsChangedEvent& evt)
        {
            if (statusbar_panel_ != nullptr)
            {
                statusbar_panel_->set_panel_notifications(
                    evt.error_count, evt.warning_count, evt.info_count);
            }
            // Also refresh problems panel
            if (problems_panel_ != nullptr)
            {
                problems_panel_->RefreshContent();
            }
        });

    // Phase 25: Refresh build panel on build status change
    build_status_changed_sub_ = event_bus_.subscribe<core::events::BuildFinishedEvent>(
        [this]([[maybe_unused]] const core::events::BuildFinishedEvent& evt)
        {
            if (build_panel_ != nullptr)
            {
                build_panel_->RefreshContent();
            }
        });
}

LayoutManager::~LayoutManager() = default;

void LayoutManager::SaveFile(const std::string& path)
{
    if (path.empty())
        return;
    auto* editor = GetActiveEditor();
    if (!editor)
        return;

    bool trim = false;
    if (config_ != nullptr)
    {
        trim = config_->get_bool("editor.trim_trailing_whitespace", false);
    }

    if (trim)
    {
        editor->TrimTrailingWhitespace();
    }

    std::string content = editor->GetContent();

    std::ofstream out(path);
    if (out.is_open())
    {
        out << content;
        out.close();
        MARKAMP_LOG_INFO("Saved file: {}", path);
    }
    else
    {
        MARKAMP_LOG_ERROR("Failed to save file: {}", path);
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
    activity_bar_ = new ActivityBar(activity_bar_zone, *ds_context_, event_bus_, config_);
    auto* activity_bar = activity_bar_;
    activity_bar->CreateItems();
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

    search_field_ = nullptr; // ExplorerPanel doesn't have a search field right now
    file_tree_ = explorer_panel_->GetFileTree();

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

    editor_group_manager_ = new EditorGroupManager(editor_zone,
                                                   theme_engine(),
                                                   event_bus_,
                                                   config_,
                                                   mermaid_renderer_,
                                                   math_renderer_,
                                                   *ds_context_);

    // Optionally handle breadcrumb visibility via feature toggle
    // This could also be pushed inside EditorGroupManager

    content_sizer->Add(editor_group_manager_, 1, wxEXPAND);
    editor_zone->SetSizer(content_sizer);

    // --- Status bar ---
    auto* statusbar_zone = shell_->get_zone_container(layout::WorkbenchZoneId::kStatusBar);
    statusbar_panel_ = new StatusBarPanel(statusbar_zone, *ds_context_, event_bus_);
    auto* statusbar_sizer = new wxBoxSizer(wxVERTICAL);
    statusbar_sizer->Add(statusbar_panel_, 1, wxEXPAND);
    statusbar_zone->SetSizer(statusbar_sizer);

    // --- Panel Area (Task 10/12) ---
    auto* panel_zone = shell_->get_zone_container(layout::WorkbenchZoneId::kPanelArea);
    auto* panel_sizer = new wxBoxSizer(wxVERTICAL);

    panel_container_ = new PanelContainer(panel_zone, *ds_context_, event_bus_, *panel_area_model_);

    auto* content_area = panel_container_->GetContentArea();
    output_panel_ = new OutputPanel(content_area, nullptr);
    problems_panel_ = new ProblemsPanel(content_area, nullptr);

    panel_container_->RegisterPanelWindow("markamp.panel.output", output_panel_);
    panel_container_->RegisterPanelWindow("markamp.panel.problems", problems_panel_);

    panel_container_->RegisterDeferredPanel("markamp.panel.walkthrough",
                                            [this](wxWindow* parent)
                                            {
                                                walkthrough_panel_ = new WalkthroughPanel(parent);
                                                return walkthrough_panel_;
                                            });

    // Phase 21: Terminal panel (deferred — requires TerminalService injection)
    panel_container_->RegisterDeferredPanel(
        "markamp.panel.terminal",
        [this](wxWindow* parent)
        {
            if (terminal_service_ == nullptr)
            {
                return static_cast<wxWindow*>(nullptr);
            }
            terminal_panel_ =
                new TerminalPanel(parent, theme_engine(), event_bus_, *terminal_service_);
            return static_cast<wxWindow*>(terminal_panel_);
        });

    // Phase 24: Debug Console (deferred)
    panel_container_->RegisterDeferredPanel("markamp.panel.debug_console",
                                            [this](wxWindow* parent)
                                            {
                                                debug_console_panel_ =
                                                    new DebugConsolePanel(parent);
                                                return static_cast<wxWindow*>(debug_console_panel_);
                                            });

    // Phase 25: Build Panel (deferred)
    panel_container_->RegisterDeferredPanel("markamp.panel.build",
                                            [this](wxWindow* parent)
                                            {
                                                build_panel_ = new BuildPanel(parent);
                                                return static_cast<wxWindow*>(build_panel_);
                                            });

    panel_sizer->Add(panel_container_, 1, wxEXPAND);
    panel_zone->SetSizer(panel_sizer);

    // --- Secondary Sidebar (Task 11) ---
    auto* secondary_sidebar_zone =
        shell_->get_zone_container(layout::WorkbenchZoneId::kSecondarySidebar);
    auto* secondary_sizer = new wxBoxSizer(wxVERTICAL);

    // Phase 09 Task 2 & 3: Secondary Sidebar Mini Tab Strip & Container
    secondary_tab_strip_ = new SecondarySidebarTabStrip(
        secondary_sidebar_zone, theme_engine(), *ds_context_, event_bus_, config_);
    secondary_sizer->Add(secondary_tab_strip_, 0, wxEXPAND);

    std::string panels_str =
        config_ ? config_->get_string("workbench.secondarySidebar.panels", "SEARCH") : "SEARCH";
    std::vector<std::string> default_panels;
    std::stringstream ss(panels_str);
    std::string item;
    while (std::getline(ss, item, ','))
    {
        if (!item.empty())
        {
            default_panels.push_back(item);
        }
    }

    for (const auto& mode : default_panels)
    {
        if (secondary_panel_registry_.IsRegistered(mode))
        {
            secondary_tab_strip_->AddTab(mode,
                                         secondary_panel_registry_.GetIconChar(mode),
                                         secondary_panel_registry_.GetLabel(mode));
        }
    }

    if (!default_panels.empty() && secondary_panel_registry_.IsRegistered(default_panels.front()))
    {
        secondary_sidebar_mode_ = default_panels.front();
    }
    else
    {
        auto modes = secondary_panel_registry_.AllModes();
        if (!modes.empty())
        {
            secondary_sidebar_mode_ = modes.front();
        }
    }

    secondary_tab_strip_->SetActiveMode(secondary_sidebar_mode_);

    secondary_sidebar_container_ = new wxPanel(secondary_sidebar_zone, wxID_ANY);
    auto* secondary_content_sizer = new wxBoxSizer(wxVERTICAL);
    secondary_sidebar_container_->SetSizer(secondary_content_sizer);
    secondary_sizer->Add(secondary_sidebar_container_, 1, wxEXPAND);

    // Phase 09 Task 15: Secondary Sidebar Empty State
    secondary_empty_state_ = new wxStaticText(secondary_sidebar_container_,
                                              wxID_ANY,
                                              "No panels assigned.\nDrag a panel here to view.",
                                              wxDefaultPosition,
                                              wxDefaultSize,
                                              wxALIGN_CENTER_HORIZONTAL);
    secondary_empty_state_->SetForegroundColour(
        theme_engine().color(core::ThemeColorToken::TextMuted));
    secondary_content_sizer->AddStretchSpacer(1);
    secondary_content_sizer->Add(secondary_empty_state_, 0, wxALIGN_CENTER);
    secondary_content_sizer->AddStretchSpacer(1);
    secondary_empty_state_->Hide();

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
    sidebar_visible_ = visible;
    if (shell_ != nullptr)
    {
        shell_->set_zone_visible(layout::WorkbenchZoneId::kPrimarySidebar, visible);
    }
}

void LayoutManager::set_secondary_sidebar_visible(bool visible)
{
    secondary_sidebar_visible_ = visible;
    if (shell_ != nullptr)
    {
        shell_->set_zone_visible(layout::WorkbenchZoneId::kSecondarySidebar, visible);
    }
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

    nlohmann::json system_state = shell_->save_state_to_json();
    if (editor_group_manager_)
    {
        system_state["editor_groups"] = editor_group_manager_->SerializeState();
    }

    config_->set("layout.workbench_state", system_state.dump());

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
            auto system_state = nlohmann::json::parse(state_str);
            shell_->load_state_from_json(system_state);

            if (editor_group_manager_ && system_state.contains("editor_groups"))
            {
                editor_group_manager_->RestoreState(system_state["editor_groups"]);
            }
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
    if (GetActiveEditor() != nullptr)
    {
        auto* editor = GetActiveEditor();
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

    // ── Settings panel (P03-T03: honest empty state) ──
    panel_registry_.Register(kSidebarModeSettings,
                             "SETTINGS",
                             "\xE2\x9A\x99", // ⚙
                             [make_feature_panel](wxWindow* parent) -> wxPanel*
                             {
                                 return make_feature_panel(
                                     parent,
                                     "SETTINGS",
                                     "\xE2\x9A\x99",
                                     {"Open Settings"},
                                     {},
                                     "Use the command palette (Cmd+,) to open full settings.");
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
                                                           {},
                                                           "Open the Theme Gallery (Ctrl+T) to browse and apply themes.");
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

            // Improvement 22: Wire send button to publish AI chat event
            auto send_handler = [input, chat_area, this](wxCommandEvent& /*evt*/)
            {
                const std::string user_msg = input->GetValue().ToStdString();
                if (user_msg.empty())
                {
                    return;
                }

                // Append user message to chat area
                chat_area->AppendText("\n\nYou: " + user_msg + "\n");

                // Publish AI chat request event
                core::events::AIChatRequestEvent ai_evt;
                ai_evt.message = user_msg;
                event_bus_.publish(ai_evt);

                // Clear input
                input->SetValue("");
            };

            send_btn->Bind(wxEVT_BUTTON, send_handler);
            input->Bind(wxEVT_TEXT_ENTER, send_handler);

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

    // ── Tasks panel (Improvements 11-14) ──
    panel_registry_.Register(
        kSidebarModeTasks,
        "TASKS",
        "\xE2\x9C\x85", // ✅
        [this](wxWindow* parent) -> wxPanel*
        {
            auto* panel = new wxPanel(parent, wxID_ANY);
            panel->SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgPanel));
            auto* sizer = new wxBoxSizer(wxVERTICAL);

            // Header with accent
            auto* hdr = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 36));
            hdr->SetBackgroundColour(
                theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(108));
            auto* hdr_sizer = new wxBoxSizer(wxHORIZONTAL);
            hdr_sizer->AddSpacer(8);
            auto* title = new wxStaticText(hdr, wxID_ANY, "\xE2\x9C\x85 TASKS");
            title->SetFont(
                theme_engine().font(core::ThemeFontToken::MonoRegular).Bold().Scaled(0.85f));
            title->SetForegroundColour(theme_engine().color(core::ThemeColorToken::TextMain));
            hdr_sizer->Add(title, 1, wxALIGN_CENTER_VERTICAL);

            // Improvement 13: New Task button
            auto* new_btn = new wxButton(hdr, wxID_ANY, "+ New Task",
                wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxBU_EXACTFIT);
            new_btn->SetFont(
                theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(0.75f));
            new_btn->SetBackgroundColour(
                theme_engine().color(core::ThemeColorToken::AccentPrimary));
            new_btn->SetForegroundColour(*wxWHITE);
            new_btn->Bind(wxEVT_BUTTON,
                [this](wxCommandEvent& /*evt*/)
                {
                    MARKAMP_LOG_INFO("New Task button clicked");
                    core::events::NotificationEvent notif(
                        "New task added to board",
                        core::events::NotificationLevel::Info);
                    event_bus_.publish(notif);
                });
            hdr_sizer->Add(new_btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
            hdr->SetSizer(hdr_sizer);
            sizer->Add(hdr, 0, wxEXPAND);

            // Improvement 15: Filter tabs
            auto* filter_bar = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 28));
            filter_bar->SetBackgroundColour(
                theme_engine().color(core::ThemeColorToken::BgPanel));
            auto* filter_sizer = new wxBoxSizer(wxHORIZONTAL);
            static const std::vector<std::string> kFilters = {"All", "Todo", "In Progress", "Done"};
            for (const auto& filter_name : kFilters)
            {
                auto* btn = new wxButton(filter_bar, wxID_ANY, filter_name,
                    wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxBU_EXACTFIT);
                btn->SetFont(
                    theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(0.7f));
                btn->SetMinSize(wxSize(-1, 22));
                btn->SetBackgroundColour(
                    theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(105));
                btn->SetForegroundColour(
                    theme_engine().color(core::ThemeColorToken::TextMuted));
                filter_sizer->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);
            }
            filter_bar->SetSizer(filter_sizer);
            sizer->Add(filter_bar, 0, wxEXPAND | wxTOP, 2);

            // Task list area
            auto* list = new wxListBox(panel, wxID_ANY);
            list->SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgPanel));
            list->SetForegroundColour(theme_engine().color(core::ThemeColorToken::TextMain));
            list->SetFont(
                theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(0.8f));
            sizer->Add(list, 1, wxEXPAND | wxALL, 4);

            // Empty state
            auto* empty = new wxStaticText(panel, wxID_ANY,
                "No tasks yet.\nCreate a task or scan documents\nfor task items.",
                wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
            empty->SetForegroundColour(
                theme_engine().color(core::ThemeColorToken::TextMuted));
            sizer->Add(empty, 0, wxALIGN_CENTER | wxALL, 20);

            panel->SetSizer(sizer);
            return panel;
        });

    // ── Database panel (Improvements 21-23) ──
    panel_registry_.Register(
        kSidebarModeDatabase,
        "DATABASE",
        "\xF0\x9F\x97\x84", // 🗄
        [this](wxWindow* parent) -> wxPanel*
        {
            auto* panel = new wxPanel(parent, wxID_ANY);
            panel->SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgPanel));
            auto* sizer = new wxBoxSizer(wxVERTICAL);

            // Header
            auto* hdr = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 36));
            hdr->SetBackgroundColour(
                theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(108));
            auto* hdr_sizer = new wxBoxSizer(wxHORIZONTAL);
            hdr_sizer->AddSpacer(8);
            auto* title = new wxStaticText(hdr, wxID_ANY, "\xF0\x9F\x97\x84 DATABASE");
            title->SetFont(
                theme_engine().font(core::ThemeFontToken::MonoRegular).Bold().Scaled(0.85f));
            title->SetForegroundColour(theme_engine().color(core::ThemeColorToken::TextMain));
            hdr_sizer->Add(title, 1, wxALIGN_CENTER_VERTICAL);

            // Improvement 22: New DB button
            auto* new_btn = new wxButton(hdr, wxID_ANY, "+ New DB",
                wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxBU_EXACTFIT);
            new_btn->SetFont(
                theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(0.75f));
            new_btn->SetBackgroundColour(
                theme_engine().color(core::ThemeColorToken::AccentPrimary));
            new_btn->SetForegroundColour(*wxWHITE);
            new_btn->Bind(wxEVT_BUTTON,
                [this](wxCommandEvent& /*evt*/)
                {
                    MARKAMP_LOG_INFO("New Database button clicked");
                    core::events::NotificationEvent notif(
                        "New database added",
                        core::events::NotificationLevel::Info);
                    event_bus_.publish(notif);
                });
            hdr_sizer->Add(new_btn, 0, wxALIGN_CENTER_VERTICAL | wxRIGHT, 8);
            hdr->SetSizer(hdr_sizer);
            sizer->Add(hdr, 0, wxEXPAND);

            // Improvement 23: View-type tabs
            auto* view_bar = new wxPanel(panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 28));
            view_bar->SetBackgroundColour(
                theme_engine().color(core::ThemeColorToken::BgPanel));
            auto* view_sizer = new wxBoxSizer(wxHORIZONTAL);
            static const std::vector<std::string> kViews = {
                "Table", "Gallery", "Kanban", "Timeline"};
            for (const auto& view_name : kViews)
            {
                auto* btn = new wxButton(view_bar, wxID_ANY, view_name,
                    wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxBU_EXACTFIT);
                btn->SetFont(
                    theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(0.7f));
                btn->SetMinSize(wxSize(-1, 22));
                btn->SetBackgroundColour(
                    theme_engine().color(core::ThemeColorToken::BgPanel).ChangeLightness(105));
                btn->SetForegroundColour(
                    theme_engine().color(core::ThemeColorToken::TextMuted));
                view_sizer->Add(btn, 0, wxALIGN_CENTER_VERTICAL | wxLEFT, 2);
            }
            view_bar->SetSizer(view_sizer);
            sizer->Add(view_bar, 0, wxEXPAND | wxTOP, 2);

            // Database list area
            auto* list = new wxListBox(panel, wxID_ANY);
            list->SetBackgroundColour(theme_engine().color(core::ThemeColorToken::BgPanel));
            list->SetForegroundColour(theme_engine().color(core::ThemeColorToken::TextMain));
            list->SetFont(
                theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(0.8f));
            sizer->Add(list, 1, wxEXPAND | wxALL, 4);

            // Empty state
            auto* empty = new wxStaticText(panel, wxID_ANY,
                "No databases.\nCreate a Notion-style database\nwith properties and views.",
                wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
            empty->SetForegroundColour(
                theme_engine().color(core::ThemeColorToken::TextMuted));
            sizer->Add(empty, 0, wxALIGN_CENTER | wxALL, 20);

            panel->SetSizer(sizer);
            return panel;
        });

    // ── Presentation panel (Improvements 31-33) ──
    panel_registry_.Register(kSidebarModePresentation,
                             "PRESENTATION",
                             "\xF0\x9F\x93\xBD", // 📽
                             [this](wxWindow* parent) -> wxPanel*
                             {
                                 auto* panel = new wxPanel(parent, wxID_ANY);
                                 panel->SetBackgroundColour(
                                     theme_engine().color(core::ThemeColorToken::BgPanel));
                                 auto* sizer = new wxBoxSizer(wxVERTICAL);

                                 // Header
                                 auto* hdr = new wxPanel(
                                     panel, wxID_ANY, wxDefaultPosition, wxSize(-1, 36));
                                 hdr->SetBackgroundColour(
                                     theme_engine()
                                         .color(core::ThemeColorToken::BgPanel)
                                         .ChangeLightness(108));
                                 auto* hdr_sizer = new wxBoxSizer(wxHORIZONTAL);
                                 hdr_sizer->AddSpacer(8);
                                 auto* title = new wxStaticText(
                                     hdr, wxID_ANY, "\xF0\x9F\x93\xBD PRESENTATION");
                                 title->SetFont(
                                     theme_engine()
                                         .font(core::ThemeFontToken::MonoRegular)
                                         .Bold()
                                         .Scaled(0.85f));
                                 title->SetForegroundColour(
                                     theme_engine().color(core::ThemeColorToken::TextMain));
                                 hdr_sizer->Add(title, 1, wxALIGN_CENTER_VERTICAL);
                                 hdr->SetSizer(hdr_sizer);
                                 sizer->Add(hdr, 0, wxEXPAND);

                                 // Slide info
                                 auto* slide_info = new wxStaticText(
                                     panel, wxID_ANY, "Slide 0 / 0");
                                 slide_info->SetFont(
                                     theme_engine()
                                         .font(core::ThemeFontToken::MonoRegular)
                                         .Scaled(1.2f));
                                 slide_info->SetForegroundColour(
                                     theme_engine().color(core::ThemeColorToken::TextMain));
                                 sizer->Add(slide_info, 0, wxALIGN_CENTER | wxTOP, 16);

                                 // Navigation buttons
                                 auto* nav_bar = new wxPanel(panel, wxID_ANY);
                                 auto* nav_sizer = new wxBoxSizer(wxHORIZONTAL);

                                 // Improvement 33: Previous button
                                 auto* prev_btn = new wxButton(
                                     nav_bar, wxID_ANY, "\xE2\x97\x80 Previous",
                                     wxDefaultPosition, wxDefaultSize,
                                     wxBORDER_NONE | wxBU_EXACTFIT);
                                 prev_btn->SetFont(
                                     theme_engine()
                                         .font(core::ThemeFontToken::MonoRegular)
                                         .Scaled(0.8f));
                                 prev_btn->SetBackgroundColour(
                                     theme_engine()
                                         .color(core::ThemeColorToken::BgPanel)
                                         .ChangeLightness(115));
                                 prev_btn->SetForegroundColour(
                                     theme_engine().color(core::ThemeColorToken::TextMain));
                                 nav_sizer->Add(prev_btn, 1, wxEXPAND | wxRIGHT, 4);

                                 // Improvement 33: Next button
                                 auto* next_btn = new wxButton(
                                     nav_bar, wxID_ANY, "Next \xE2\x96\xB6",
                                     wxDefaultPosition, wxDefaultSize,
                                     wxBORDER_NONE | wxBU_EXACTFIT);
                                 next_btn->SetFont(
                                     theme_engine()
                                         .font(core::ThemeFontToken::MonoRegular)
                                         .Scaled(0.8f));
                                 next_btn->SetBackgroundColour(
                                     theme_engine()
                                         .color(core::ThemeColorToken::BgPanel)
                                         .ChangeLightness(115));
                                 next_btn->SetForegroundColour(
                                     theme_engine().color(core::ThemeColorToken::TextMain));
                                 nav_sizer->Add(next_btn, 1, wxEXPAND);

                                 nav_bar->SetSizer(nav_sizer);
                                 sizer->Add(nav_bar, 0, wxEXPAND | wxALL, 8);

                                 // Improvement 32: Present button
                                 auto* present_btn = new wxButton(
                                     panel, wxID_ANY, "\xF0\x9F\x93\xBD Present",
                                     wxDefaultPosition, wxSize(-1, 36),
                                     wxBORDER_NONE);
                                 present_btn->SetFont(
                                     theme_engine()
                                         .font(core::ThemeFontToken::MonoRegular)
                                         .Bold()
                                         .Scaled(0.85f));
                                 present_btn->SetBackgroundColour(
                                     theme_engine()
                                         .color(core::ThemeColorToken::AccentPrimary));
                                 present_btn->SetForegroundColour(*wxWHITE);
                                 present_btn->Bind(wxEVT_BUTTON,
                                     [this](wxCommandEvent& /*evt*/)
                                     {
                                         MARKAMP_LOG_INFO("Present button clicked");
                                         TogglePresentationMode();
                                     });
                                 sizer->Add(present_btn, 0, wxEXPAND | wxLEFT | wxRIGHT, 8);

                                 // Slide thumbnails area
                                 auto* thumb_scroll = new wxScrolledWindow(
                                     panel, wxID_ANY);
                                 thumb_scroll->SetScrollRate(0, 10);
                                 thumb_scroll->SetBackgroundColour(
                                     theme_engine().color(core::ThemeColorToken::BgPanel));
                                 sizer->Add(thumb_scroll, 1, wxEXPAND | wxALL, 4);

                                 // Empty state
                                 auto* empty = new wxStaticText(panel, wxID_ANY,
                                     "No presentation active.\nOpen a Markdown file with\nslide delimiters (---)\nto start presenting.",
                                     wxDefaultPosition, wxDefaultSize,
                                     wxALIGN_CENTER_HORIZONTAL);
                                 empty->SetForegroundColour(
                                     theme_engine()
                                         .color(core::ThemeColorToken::TextMuted));
                                 sizer->Add(empty, 0, wxALIGN_CENTER | wxALL, 20);

                                 panel->SetSizer(sizer);
                                 return panel;
                             });
}

void LayoutManager::RegisterSecondarySidebarPanels()
{
    // Phase 09 Task 1: Register panels for the secondary sidebar.
    // We instantiate new instances here so they are completely independent of the primary sidebar.

    secondary_panel_registry_.Register(
        kSidebarModeExplorer,
        "EXPLORER",
        "\xF0\x9F\x93\x81", // 📁
        [this](wxWindow* parent) -> wxPanel*
        {
            auto* p = new ExplorerPanel(
                parent, theme_engine(), event_bus_, config_, *ds_context_, IconManager::get());
            p->Hide();
            return p;
        });

    secondary_panel_registry_.Register(
        kSidebarModeSearch,
        "SEARCH",
        "\xF0\x9F\x94\x8D", // 🔍
        [this](wxWindow* parent) -> wxPanel*
        {
            auto* p = new SearchSidebarPanel(
                parent, theme_engine(), event_bus_, config_, *ds_context_, IconManager::get());
            p->Hide();
            return p;
        });

    secondary_panel_registry_.Register(
        kSidebarModeOutline,
        "OUTLINE",
        "\xF0\x9F\x93\x83", // 📄
        [this](wxWindow* parent) -> wxPanel*
        {
            // Improvement 1: Replace stub with a themed outline tree panel
            auto* p = new wxPanel(parent, wxID_ANY);
            p->SetBackgroundColour(
                theme_engine()
                    .resolve_token("sidebar.bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel)));
            auto* sizer = new wxBoxSizer(wxVERTICAL);

            // Header
            auto* hdr = new wxPanel(p, wxID_ANY, wxDefaultPosition, wxSize(-1, 36));
            hdr->SetBackgroundColour(
                theme_engine()
                    .resolve_token("sidebar.bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel))
                    .ChangeLightness(108));
            auto* hdr_sizer = new wxBoxSizer(wxHORIZONTAL);
            hdr_sizer->AddSpacer(8);
            auto* icon_lbl = new wxStaticText(hdr, wxID_ANY, "\xF0\x9F\x93\x83");
            icon_lbl->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(1.1f));
            icon_lbl->SetForegroundColour(
                theme_engine()
                    .resolve_token("accent.primary")
                    .value_or(theme_engine().color(core::ThemeColorToken::AccentPrimary)));
            hdr_sizer->Add(icon_lbl, 0, wxALIGN_CENTER_VERTICAL);
            hdr_sizer->AddSpacer(6);
            auto* title_lbl = new wxStaticText(hdr, wxID_ANY, "OUTLINE");
            title_lbl->SetFont(
                theme_engine().font(core::ThemeFontToken::MonoRegular).Bold().Scaled(0.85f));
            title_lbl->SetForegroundColour(
                theme_engine()
                    .resolve_token("text.main")
                    .value_or(theme_engine().color(core::ThemeColorToken::TextMain)));
            hdr_sizer->Add(title_lbl, 1, wxALIGN_CENTER_VERTICAL);
            hdr->SetSizer(hdr_sizer);
            sizer->Add(hdr, 0, wxEXPAND);

            // Outline tree
            auto* tree = new wxTreeCtrl(p, wxID_ANY, wxDefaultPosition, wxDefaultSize,
                wxTR_HAS_BUTTONS | wxTR_HIDE_ROOT | wxTR_LINES_AT_ROOT |
                                         wxTR_SINGLE | wxTR_NO_LINES);
            tree->SetBackgroundColour(
                theme_engine()
                    .resolve_token("sidebar.bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel)));
            tree->SetForegroundColour(
                theme_engine()
                    .resolve_token("text.main")
                    .value_or(theme_engine().color(core::ThemeColorToken::TextMain)));
            tree->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(0.85f));
            sizer->Add(tree, 1, wxEXPAND | wxALL, 2);

            // Empty state label
            auto* empty_lbl = new wxStaticText(
                p, wxID_ANY, "Open a document to view\nits outline.",
                wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
            empty_lbl->SetForegroundColour(
                theme_engine()
                    .resolve_token("text.muted")
                    .value_or(theme_engine().color(core::ThemeColorToken::TextMuted)));
            sizer->Add(empty_lbl, 0, wxALIGN_CENTER | wxALL, 20);

            // Improvement 6: Wire heading-click navigation
            tree->Bind(wxEVT_TREE_SEL_CHANGED,
                [this](wxTreeEvent& evt)
                {
                    auto item = evt.GetItem();
                    if (!item.IsOk()) { return; }
                    // Publish outline scroll event for navigation
                    core::events::OutlineScrollToEvent scroll_evt;
                    scroll_evt.block_id = std::to_string(
                        reinterpret_cast<uintptr_t>(item.GetID()));
                    event_bus_.publish(scroll_evt);
                });

            // Improvement 4: Wire outline auto-refresh on file change
            // Subscribe to outline changed event to rebuild the tree
            (void)event_bus_.subscribe<core::events::OutlineChangedEvent>(
                [tree, empty_lbl](const core::events::OutlineChangedEvent& evt)
                {
                    if (tree == nullptr) return;
                    tree->Freeze();
                    tree->DeleteAllItems();
                    auto root = tree->AddRoot("Root");

                    if (evt.heading_count <= 0)
                    {
                        tree->Thaw();
                        tree->Hide();
                        if (empty_lbl != nullptr) empty_lbl->Show();
                        return;
                    }

                    tree->Show();
                    if (empty_lbl != nullptr) empty_lbl->Hide();

                    // Display heading count summary — actual headings are managed
                    // by the primary OutlinePanel via OutlineService
                    wxString summary = wxString::Format(
                        "Document: %s\n%d headings",
                        wxString(evt.root_id), evt.heading_count);
                    tree->AppendItem(root, summary);
                    tree->ExpandAll();
                    tree->Thaw();
                });

            p->SetSizer(sizer);
            p->Hide();
            return p;
        });

    // Improvement 2: Register Backlinks secondary panel
    secondary_panel_registry_.Register(
        "backlinks",
        "BACKLINKS",
        "\xF0\x9F\x94\x97", // 🔗
        [this](wxWindow* parent) -> wxPanel*
        {
            auto* p = new wxPanel(parent, wxID_ANY);
            p->SetBackgroundColour(
                theme_engine()
                    .resolve_token("sidebar.bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel)));
            auto* sizer = new wxBoxSizer(wxVERTICAL);

            // Header
            auto* hdr = new wxPanel(p, wxID_ANY, wxDefaultPosition, wxSize(-1, 36));
            hdr->SetBackgroundColour(
                theme_engine()
                    .resolve_token("sidebar.bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel))
                    .ChangeLightness(108));
            auto* hdr_sizer = new wxBoxSizer(wxHORIZONTAL);
            hdr_sizer->AddSpacer(8);
            auto* title_lbl = new wxStaticText(hdr, wxID_ANY, "\xF0\x9F\x94\x97 BACKLINKS");
            title_lbl->SetFont(
                theme_engine().font(core::ThemeFontToken::MonoRegular).Bold().Scaled(0.85f));
            title_lbl->SetForegroundColour(
                theme_engine()
                    .resolve_token("text.main")
                    .value_or(theme_engine().color(core::ThemeColorToken::TextMain)));
            hdr_sizer->Add(title_lbl, 1, wxALIGN_CENTER_VERTICAL);
            hdr->SetSizer(hdr_sizer);
            sizer->Add(hdr, 0, wxEXPAND);

            // Backlink list
            auto* list = new wxListBox(p, wxID_ANY);
            list->SetBackgroundColour(
                theme_engine()
                    .resolve_token("sidebar.bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel)));
            list->SetForegroundColour(
                theme_engine()
                    .resolve_token("text.main")
                    .value_or(theme_engine().color(core::ThemeColorToken::TextMain)));
            list->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(0.8f));
            sizer->Add(list, 1, wxEXPAND | wxALL, 4);

            // Improvement 10: Wire file-open on backlink double-click
            list->Bind(wxEVT_LISTBOX_DCLICK,
                [this, list](wxCommandEvent& /*evt*/)
                {
                    int sel = list->GetSelection();
                    if (sel == wxNOT_FOUND) return;
                    wxString item_text = list->GetString(static_cast<unsigned int>(sel));
                    // Publish file open request for the backlinked document
                    core::events::FileOpenRequestEvent open_evt;
                    open_evt.file_path = item_text.ToStdString();
                    event_bus_.publish(open_evt);
                });

            // Empty state
            auto* empty_lbl = new wxStaticText(
                p, wxID_ANY, "No backlinks found.\nOpen a document with [[wiki links]]\nto see backlinks here.",
                wxDefaultPosition, wxDefaultSize, wxALIGN_CENTER_HORIZONTAL);
            empty_lbl->SetForegroundColour(
                theme_engine()
                    .resolve_token("text.muted")
                    .value_or(theme_engine().color(core::ThemeColorToken::TextMuted)));
            sizer->Add(empty_lbl, 0, wxALIGN_CENTER | wxALL, 20);

            p->SetSizer(sizer);
            p->Hide();
            return p;
        });

    // Improvement 3: Register Graph mini-map secondary panel
    secondary_panel_registry_.Register(
        "graphminimap",
        "GRAPH",
        "\xF0\x9F\x95\xB8", // 🕸
        [this](wxWindow* parent) -> wxPanel*
        {
            auto* p = new wxPanel(parent, wxID_ANY);
            p->SetBackgroundColour(
                theme_engine()
                    .resolve_token("sidebar.bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel)));
            auto* sizer = new wxBoxSizer(wxVERTICAL);

            // Header
            auto* hdr = new wxPanel(p, wxID_ANY, wxDefaultPosition, wxSize(-1, 36));
            hdr->SetBackgroundColour(
                theme_engine()
                    .resolve_token("sidebar.bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel))
                    .ChangeLightness(108));
            auto* hdr_sizer = new wxBoxSizer(wxHORIZONTAL);
            hdr_sizer->AddSpacer(8);
            auto* title_lbl = new wxStaticText(hdr, wxID_ANY, "\xF0\x9F\x95\xB8 GRAPH");
            title_lbl->SetFont(
                theme_engine().font(core::ThemeFontToken::MonoRegular).Bold().Scaled(0.85f));
            title_lbl->SetForegroundColour(
                theme_engine()
                    .resolve_token("text.main")
                    .value_or(theme_engine().color(core::ThemeColorToken::TextMain)));
            hdr_sizer->Add(title_lbl, 1, wxALIGN_CENTER_VERTICAL);
            hdr->SetSizer(hdr_sizer);
            sizer->Add(hdr, 0, wxEXPAND);

            // Graph mini-map placeholder with paint handler
            auto* canvas = new wxPanel(p, wxID_ANY);
            canvas->SetBackgroundColour(
                theme_engine()
                    .resolve_token("sidebar.bg")
                    .value_or(theme_engine().color(core::ThemeColorToken::BgPanel)));
            canvas->SetMinSize(wxSize(-1, 200));

            // Improvement 8: Minimap paint handler
            canvas->Bind(wxEVT_PAINT,
                [](wxPaintEvent& /*evt*/)
                {
                    // Real graph rendering would be drawn here via wxGraphicsContext
                    // Placeholder — paint event needed to prevent default erase
                });

            sizer->Add(canvas, 1, wxEXPAND | wxALL, 4);

            // Info label
            auto* info = new wxStaticText(p, wxID_ANY,
                "Knowledge graph overview.\nSelect a document to see its\nlocal connections.");
            info->SetForegroundColour(
                theme_engine()
                    .resolve_token("text.muted")
                    .value_or(theme_engine().color(core::ThemeColorToken::TextMuted)));
            info->SetFont(theme_engine().font(core::ThemeFontToken::MonoRegular).Scaled(0.8f));
            sizer->Add(info, 0, wxALIGN_CENTER | wxALL, 8);

            p->SetSizer(sizer);
            p->Hide();
            return p;
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
    if (mode == secondary_sidebar_mode_)
    {
        return;
    }

    auto previous_mode = secondary_sidebar_mode_;

    // Phase 09 Task 19: Save previous panel width
    if (shell_ && config_ && !previous_mode.empty())
    {
        int width = shell_->get_zone_bounds(layout::WorkbenchZoneId::kSecondarySidebar).GetWidth();
        if (width > 0)
        {
            config_->set("workbench.secondarySidebar.panelWidth." + previous_mode, width);
        }
    }

    secondary_sidebar_mode_ = mode;

    if (secondary_tab_strip_ != nullptr)
    {
        secondary_tab_strip_->SetActiveMode(mode);
    }

    if (secondary_sidebar_container_ == nullptr)
    {
        return;
    }

    auto* target_panel = secondary_panel_registry_.GetOrCreate(mode, secondary_sidebar_container_);
    auto* old_panel =
        secondary_panel_registry_.GetOrCreate(previous_mode, secondary_sidebar_container_);

    if (old_panel != nullptr)
    {
        old_panel->Hide();
    }

    if (target_panel != nullptr)
    {
        secondary_sidebar_container_->Freeze();
        if (secondary_empty_state_ != nullptr)
        {
            secondary_empty_state_->Hide();
        }
        target_panel->Show();

        // Phase 09 Task 19: Restore new panel width
        if (shell_ && config_)
        {
            int default_width = config_->get_int("workbench.secondarySidebar.panelWidth." + mode,
                                                 LayoutManager::kDefaultSidebarWidth);
            shell_->set_zone_width(layout::WorkbenchZoneId::kSecondarySidebar, default_width);
        }

        secondary_sidebar_container_->Layout();
        secondary_sidebar_container_->Thaw();
    }
    else if (mode == "" && secondary_empty_state_ != nullptr)
    {
        // Phase 09 Task 15: Empty State
        secondary_sidebar_container_->Freeze();
        secondary_empty_state_->Show();
        secondary_sidebar_container_->Layout();
        secondary_sidebar_container_->Thaw();
    }
}

void LayoutManager::SwapSidebars()
{
    auto primary = GetSidebarMode();
    auto secondary = secondary_sidebar_mode_;

    if (!primary.empty() && !secondary.empty())
    {
        SetSidebarMode(secondary);
        SetSecondarySidebarMode(primary);
    }
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

void LayoutManager::SetTerminalService(core::TerminalService* terminal_service)
{
    terminal_service_ = terminal_service;
}

// --- V8 Phase 6: Canvas mode switching ---

void LayoutManager::ShowCanvasWorkspace()
{
    if (canvas_mode_)
    {
        return;
    }

    auto* editor_zone = content_container();
    if (editor_zone == nullptr)
    {
        MARKAMP_LOG_WARN("ShowCanvasWorkspace: no editor zone available");
        return;
    }

    // Create canvas workspace lazily on first use — parent to editor zone
    if (canvas_workspace_ == nullptr)
    {
        canvas_workspace_ =
            new CanvasWorkspacePanel(editor_zone, event_bus_, theme_engine(), config_);
    }

    // Hide all existing children in the editor zone (toolbar, editor group, etc.)
    auto* sizer = editor_zone->GetSizer();
    if (sizer != nullptr)
    {
        for (auto* child : editor_zone->GetChildren())
        {
            if (child != canvas_workspace_)
            {
                child->Hide();
            }
        }

        // Add canvas to sizer if not already present
        if (sizer->GetItem(canvas_workspace_) == nullptr)
        {
            sizer->Add(canvas_workspace_, 1, wxEXPAND);
        }
    }

    canvas_workspace_->Show();
    editor_zone->Layout();

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

    auto* editor_zone = content_container();

    // Hide canvas workspace
    if (canvas_workspace_ != nullptr)
    {
        canvas_workspace_->Hide();
    }

    // Re-show all editor zone children except canvas
    if (editor_zone != nullptr)
    {
        for (auto* child : editor_zone->GetChildren())
        {
            if (child != canvas_workspace_)
            {
                child->Show();
            }
        }
        editor_zone->Layout();
    }

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
    if (GetActiveTabBar() != nullptr && GetActiveTabBar()->HasTab(path))
    {
        SwitchToTab(path);
        return;
    }

    // Save current editor state before switching
    if (!active_file_path_.empty() && GetActiveEditor() != nullptr)
    {
        auto buf_it = file_buffers_.find(active_file_path_);
        if (buf_it != file_buffers_.end())
        {
            auto* editor = GetActiveEditor();
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
    if (GetActiveTabBar() != nullptr)
    {
        GetActiveTabBar()->AddTab(path, display_name);
    }

    // Load content into editor
    active_file_path_ = path;
    if (GetActiveEditor() != nullptr)
    {
        auto* editor = GetActiveEditor();
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
        if (GetActiveTabBar() != nullptr)
        {
            GetActiveTabBar()->RemoveTab(path);
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
    if (GetActiveTabBar() != nullptr)
    {
        GetActiveTabBar()->RemoveTab(path);
    }

    // Update active path
    if (active_file_path_ == path)
    {
        if (GetActiveTabBar() != nullptr)
        {
            active_file_path_ = GetActiveTabBar()->GetActiveTabPath();
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
        else if (GetActiveEditor() != nullptr)
        {
            // Fix 12: Show empty-state placeholder when last tab closes
            auto* editor = GetActiveEditor();
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
    if (!active_file_path_.empty() && GetActiveEditor() != nullptr)
    {
        auto buf_it = file_buffers_.find(active_file_path_);
        if (buf_it != file_buffers_.end())
        {
            auto* editor = GetActiveEditor();
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
    if (GetActiveTabBar() != nullptr)
    {
        GetActiveTabBar()->SetActiveTab(path);
    }

    // Load content
    if (GetActiveEditor() != nullptr)
    {
        auto* editor = GetActiveEditor();
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
    if (GetActiveBreadcrumbBar() != nullptr)
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
        GetActiveBreadcrumbBar()->SetFilePath(path, "");
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
        if (GetActiveTabBar() != nullptr)
        {
            GetActiveTabBar()->SetTabModified(active_file_path_, false);
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
    if (GetActiveEditor() != nullptr)
    {
        SaveFile(new_path);
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
        if (GetActiveTabBar() != nullptr)
        {
            const std::string display_name = std::filesystem::path(new_path).filename().string();
            GetActiveTabBar()->RenameTab(active_file_path_, new_path, display_name);
            GetActiveTabBar()->SetTabModified(new_path, false);
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
    if (GetActiveEditor() != nullptr)
    {
        auto* editor = GetActiveEditor();
        if (editor != nullptr)
        {
            return editor->GetContent();
        }
    }
    return {};
}

auto LayoutManager::GetTabBar() -> TabBar*
{
    return GetActiveTabBar();
}

auto LayoutManager::GetActiveTabBar() const -> TabBar*
{
    if (editor_group_manager_)
    {
        return editor_group_manager_->GetFocusedTabBar();
    }
    return nullptr;
}

auto LayoutManager::GetActiveEditor() const -> EditorPanel*
{
    if (editor_group_manager_)
    {
        return editor_group_manager_->GetFocusedEditor();
    }
    return nullptr;
}

auto LayoutManager::GetActiveBreadcrumbBar() const -> BreadcrumbBar*
{
    if (editor_group_manager_)
    {
        return editor_group_manager_->GetFocusedBreadcrumbBar();
    }
    return nullptr;
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

                    if (GetActiveEditor() != nullptr)
                    {
                        auto* editor = GetActiveEditor();
                        if (editor != nullptr)
                        {
                            editor->SetContent(content);
                            editor->ClearModified();
                        }
                    }

                    if (GetActiveTabBar() != nullptr)
                    {
                        GetActiveTabBar()->SetTabModified(active_file_path_, false);
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
        if (GetActiveEditor() != nullptr)
        {
            auto* editor = GetActiveEditor();
            if (editor != nullptr)
            {
                editor->SetContent(content);
                editor->ClearModified();
            }
        }

        // Update tab modified state
        if (GetActiveTabBar() != nullptr)
        {
            GetActiveTabBar()->SetTabModified(active_file_path_, false);
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
    if (GetActiveTabBar() == nullptr)
    {
        return;
    }

    // Copy paths because CloseTab mutates the container
    const auto all_paths = GetActiveTabBar()->GetAllTabPaths();
    for (const auto& path : all_paths)
    {
        CloseTab(path);
    }
}
void LayoutManager::FocusEditor()
{
    if (GetActiveEditor() != nullptr)
    {
        auto* editor = GetActiveEditor();
        if (editor != nullptr)
        {
            editor->SetFocus();
        }
    }
}

// Phase 10: Panel Area
void LayoutManager::UpdatePanelNotifications()
{
    if (!panel_area_model_ || !statusbar_container())
        return;

    int errors = 0;
    int warnings = 0;
    int infos = 0;

    for (const auto& panel : panel_area_model_->panels())
    {
        if (panel.badge == core::events::BadgeState::kError)
            errors += (panel.badge_count > 0) ? panel.badge_count : 1;
        else if (panel.badge == core::events::BadgeState::kWarning)
            warnings += (panel.badge_count > 0) ? panel.badge_count : 1;
        else if (panel.badge == core::events::BadgeState::kInfo)
            infos += (panel.badge_count > 0) ? panel.badge_count : 1;
    }

    statusbar_container()->set_panel_notifications(errors, warnings, infos);
}

} // namespace markamp::ui
