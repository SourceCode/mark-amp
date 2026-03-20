# P01-T01: UI-to-Capability Integration Ledger

> **Phase 01 — Integration Inventory and Shared Contracts**
> **Status:** Complete
> **Scope:** Maps every visible control to its backing service, command, event, and handler.
> **Rollback:** Documentation only — no code changes.

---

## 1 · Surface Index

| # | Surface            | Source File(s)                        | Control Count | Section |
|---|-------------------|---------------------------------------|--------------|---------|
| 1 | Menu Bar           | `MainFrame.cpp:874–1313`              | 130+ items   | §2      |
| 2 | Toolbar            | `Toolbar.cpp`                         | 7 buttons    | §3      |
| 3 | Activity Bar       | `ActivityBar.cpp:196–282`             | 15 items     | §4      |
| 4 | Status Bar         | `StatusBarPanel.cpp:405–800`          | 20+ items    | §5      |
| 5 | Command Palette    | `CommandPalette.cpp` + `MainFrame.cpp:2649–2900` | Dynamic  | §6      |
| 6 | Keyboard Shortcuts | `ShortcutManager.h` + `MainFrame.cpp:2349–2647` | 50+ binds | §7      |
| 7 | Context Menus      | `LayoutManager.cpp`, `ActivityBar.cpp:746–791`   | See §8     | §8      |
| 8 | Tab Bar            | `LayoutManager.cpp` (TabBar events)   | See §9       | §9      |

---

## 2 · Menu Bar (MainFrame.cpp)

### 2.1 File Menu

| Menu ID                | Label                   | Shortcut        | Event / Handler                          | Status |
|------------------------|-------------------------|-----------------|------------------------------------------|--------|
| `wxID_NEW`             | New                     | Ctrl+N          | Inline λ → `showEditor()` + `OpenFileInTab()` | ✅ Wired |
| `wxID_OPEN`            | Open Folder...          | Ctrl+O          | `onOpenFolder()` → `scanDirectory()`    | ✅ Wired |
| `kMenuOpenFile`        | Open File...            | Ctrl+Shift+O    | Inline λ → `wxFileDialog` → `OpenFileInTab()` | ✅ Wired |
| `kMenuNewBoard`        | New Board...            | Ctrl+Shift+B    | `BoardOpenRequestEvent`                  | ✅ Wired |
| `wxID_SAVE`            | Save                    | Ctrl+S          | `onSave()` → `SaveActiveFile()`         | ✅ Wired |
| `kMenuSaveAs`          | Save As...              | Ctrl+Shift+S    | Inline λ → `SaveActiveFileAs()`         | ✅ Wired |
| `kMenuSaveAll`         | Save All                | Ctrl+Alt+S      | Inline λ → `SaveAllFiles()`             | ✅ Wired |
| `kMenuRevertFile`      | Revert File             | —               | Inline λ → `RevertActiveFile()`         | ✅ Wired |
| `kMenuCloseTab`        | Close Tab               | Ctrl+W          | `TabCloseRequestEvent`                  | ✅ Wired |
| `kMenuCloseAllTabs`    | Close All Tabs          | Ctrl+Shift+W    | Inline λ → `CloseAllTabs()`             | ✅ Wired |
| `kMenuCloseOtherTabs`  | Close Other Tabs        | —               | `CloseOtherTabsRequestEvent`            | ✅ Wired |
| `kMenuCloseSavedTabs`  | Close Saved Tabs        | —               | `CloseSavedTabsRequestEvent`            | ✅ Wired |
| `kMenuCloseTabsToLeft` | Close Tabs to Left      | —               | `CloseTabsToLeftRequestEvent`           | ✅ Wired |
| `kMenuCloseTabsToRight`| Close Tabs to Right     | —               | `CloseTabsToRightRequestEvent`          | ✅ Wired |
| `kMenuPinTab`          | Pin Current Tab         | —               | `PinTabRequestEvent`                    | ✅ Wired |
| `kMenuUnpinTab`        | Unpin Current Tab       | —               | `UnpinTabRequestEvent`                  | ✅ Wired |
| `kMenuCloseFolder`     | Close Folder            | —               | Inline λ → `showStartupScreen()`        | ✅ Wired |
| `kMenuPrint`           | Print...                | Ctrl+P          | `wxHtmlEasyPrinting`                    | ✅ Wired |
| `kMenuCopyFilePath`    | Copy File Path          | —               | `CopyFilePathRequestEvent`              | ✅ Wired |
| `kMenuRevealInFinder`  | Reveal in Finder        | —               | `RevealInFinderRequestEvent`            | ✅ Wired |
| `kMenuExportHTML`      | Export as HTML          | —               | ⚠️ **MISSING** — menu item exists, no handler bound |
| `kMenuExportPDF`       | Export as PDF           | —               | ⚠️ **MISSING** — menu item exists, no handler bound |
| `kMenuExportMarkdown`  | Export as Markdown      | —               | ⚠️ **MISSING** — menu item exists, no handler bound |
| `kMenuExportBatch`     | Batch Export            | —               | ⚠️ **MISSING** — menu item exists, no handler bound |
| `kMenuImportHTML`      | Import HTML             | —               | ⚠️ **MISSING** — menu item exists, no handler bound |
| `kMenuImportMarkdown`  | Import Markdown         | —               | ⚠️ **MISSING** — menu item exists, no handler bound |
| `kMenuImportPDFAnnotations` | Import PDF Annotations | —          | ⚠️ **MISSING** — menu item exists, no handler bound |
| `wxID_EXIT`            | Exit                    | Alt+F4          | Inline λ → `Close(true)`                | ✅ Wired |

### 2.2 Edit Menu

| Menu ID                     | Label                       | Shortcut              | Event / Handler                              | Status |
|-----------------------------|-----------------------------|-----------------------|----------------------------------------------|--------|
| `kMenuUndo`                 | Undo                        | Ctrl+Z                | ⚠️ **MISSING** — menu item, no explicit binding in `createMenuBar()` |
| `kMenuRedo`                 | Redo                        | Ctrl+Shift+Z          | ⚠️ **MISSING** — same as above          |
| `kMenuCut`                  | Cut                         | Ctrl+X                | ⚠️ **MISSING** — relies on wxWidgets default |
| `kMenuCopy`                 | Copy                        | Ctrl+C                | ⚠️ **MISSING** — relies on wxWidgets default |
| `kMenuPaste`                | Paste                       | Ctrl+V                | ⚠️ **MISSING** — relies on wxWidgets default |
| `kMenuSelectAll`            | Select All                  | Ctrl+A                | ⚠️ **MISSING** — relies on wxWidgets default |
| `kMenuFind`                 | Find...                     | Ctrl+F                | `FindRequestEvent`                           | ✅ Wired |
| `kMenuReplace`              | Find and Replace...         | Ctrl+H                | `ReplaceRequestEvent`                        | ✅ Wired |
| `kMenuSearchNextMatch`      | Search Next Match           | F4                    | `SearchNextMatchRequestEvent`                | ✅ Wired |
| `kMenuSearchPrevMatch`      | Search Previous Match       | Shift+F4              | `SearchPrevMatchRequestEvent`                | ✅ Wired |
| `kMenuDuplicateLine`        | Duplicate Line              | Ctrl+Shift+D          | `DuplicateLineRequestEvent`                  | ✅ Wired |
| `kMenuToggleComment`        | Toggle Comment              | Ctrl+/                | `ToggleCommentRequestEvent`                  | ✅ Wired |
| `kMenuDeleteLine`           | Delete Line                 | Ctrl+Shift+K          | `DeleteLineRequestEvent`                     | ✅ Wired |
| `kMenuMoveLineUp`           | Move Line Up                | Alt+Up                | `MoveLineUpRequestEvent`                     | ✅ Wired |
| `kMenuMoveLineDown`         | Move Line Down              | Alt+Down              | `MoveLineDownRequestEvent`                   | ✅ Wired |
| `kMenuJoinLines`            | Join Lines                  | Ctrl+J                | `JoinLinesRequestEvent`                      | ✅ Wired |
| `kMenuSelectAllOccurrences` | Select All Occurrences      | Ctrl+Shift+L          | `SelectAllOccurrencesRequestEvent`           | ✅ Wired |
| `kMenuExpandLineSelection`  | Expand Line Selection       | Ctrl+L                | `ExpandLineSelectionRequestEvent`            | ✅ Wired |
| `kMenuInsertLineAbove`      | Insert Line Above           | Ctrl+Shift+Return     | `InsertLineAboveRequestEvent`                | ✅ Wired |
| `kMenuInsertLineBelow`      | Insert Line Below           | Ctrl+Return           | `InsertLineBelowRequestEvent`                | ✅ Wired |
| `kMenuCopyLineUp`           | Copy Line Up                | Shift+Alt+Up          | `CopyLineUpRequestEvent`                     | ✅ Wired |
| `kMenuCopyLineDown`         | Copy Line Down              | Shift+Alt+Down        | `CopyLineDownRequestEvent`                   | ✅ Wired |
| `kMenuDeleteAllLeft`        | Delete All Left             | Ctrl+Backspace        | `DeleteAllLeftRequestEvent`                  | ✅ Wired |
| `kMenuDeleteAllRight`       | Delete All Right            | Ctrl+Delete           | `DeleteAllRightRequestEvent`                 | ✅ Wired |
| `kMenuTransposeChars`       | Transpose Characters        | Ctrl+T                | `TransposeCharsRequestEvent`                 | ✅ Wired |
| `kMenuIndentSelection`      | Indent Selection            | Ctrl+]                | `IndentSelectionRequestEvent`                | ✅ Wired |
| `kMenuOutdentSelection`     | Outdent Selection           | Ctrl+[                | `OutdentSelectionRequestEvent`               | ✅ Wired |
| `kMenuJumpToBracket`        | Jump to Matching Bracket    | Ctrl+Shift+\\         | `JumpToBracketRequestEvent`                  | ✅ Wired |
| `kMenuAddLineComment`       | Add Line Comment            | —                     | `AddLineCommentRequestEvent`                 | ✅ Wired |
| `kMenuRemoveLineComment`    | Remove Line Comment         | —                     | `RemoveLineCommentRequestEvent`              | ✅ Wired |
| `kMenuToggleReadOnly`       | Toggle Read-Only            | —                     | `ToggleReadOnlyRequestEvent`                 | ✅ Wired |
| `kMenuSelectWord`           | Select Word                 | Ctrl+D                | `SelectWordRequestEvent`                     | ✅ Wired |
| `kMenuSelectParagraph`      | Select Paragraph            | —                     | `SelectParagraphRequestEvent`                | ✅ Wired |
| `kMenuExpandSelection`      | Expand Selection            | Shift+Alt+Right       | `ExpandSelectionRequestEvent`                | ✅ Wired |
| `kMenuShrinkSelection`      | Shrink Selection            | Shift+Alt+Left        | `ShrinkSelectionRequestEvent`                | ✅ Wired |
| `kMenuTransformUpper`       | Uppercase                   | —                     | `TransformUpperRequestEvent`                 | ✅ Wired |
| `kMenuTransformLower`       | Lowercase                   | —                     | `TransformLowerRequestEvent`                 | ✅ Wired |
| `kMenuTransformTitle`       | Title Case                  | —                     | `TransformTitleRequestEvent`                 | ✅ Wired |
| `kMenuConvertIndentSpaces`  | Convert to Spaces           | —                     | `ConvertIndentSpacesRequestEvent`            | ✅ Wired |
| `kMenuConvertIndentTabs`    | Convert to Tabs             | —                     | `ConvertIndentTabsRequestEvent`              | ✅ Wired |
| `kMenuSortAsc`              | Sort Lines Ascending        | —                     | `SortLinesAscRequestEvent`                   | ✅ Wired |
| `kMenuSortDesc`             | Sort Lines Descending       | —                     | `SortLinesDescRequestEvent`                  | ✅ Wired |
| `kMenuReverseLines`         | Reverse Lines               | —                     | `ReverseLinesRequestEvent`                   | ✅ Wired |
| `kMenuDeleteDuplicateLines` | Delete Duplicate Lines      | —                     | `DeleteDuplicateLinesRequestEvent`           | ✅ Wired |
| `kMenuGoToLine`             | Go to Line...               | Ctrl+G                | `GoToLineRequestEvent`                       | ✅ Wired |
| `kMenuTrimTrailingWS`       | Trim Trailing Whitespace    | —                     | `TrimTrailingWSRequestEvent`                 | ✅ Wired |
| `kMenuCursorUndo`           | Cursor Undo                 | Ctrl+U                | `CursorUndoRequestEvent`                     | ✅ Wired |
| `kMenuCursorRedo`           | Cursor Redo                 | Ctrl+Shift+U          | `CursorRedoRequestEvent`                     | ✅ Wired |
| `kMenuMoveTextLeft`         | Move Text Left              | —                     | `MoveTextLeftRequestEvent`                   | ✅ Wired |
| `kMenuMoveTextRight`        | Move Text Right             | —                     | `MoveTextRightRequestEvent`                  | ✅ Wired |
| `kMenuSelectToBracket`      | Select to Matching Bracket  | —                     | `SelectToBracketRequestEvent`                | ✅ Wired |
| `kMenuToggleBlockComment`   | Toggle Block Comment        | Shift+Alt+A           | `ToggleBlockCommentRequestEvent`             | ✅ Wired |
| `kMenuInsertDateTime`       | Insert Date/Time            | —                     | `InsertDateTimeRequestEvent`                 | ✅ Wired |
| `kMenuAddCursorBelow`       | Add Cursor Below            | Alt+Cmd+Down          | `AddCursorBelowRequestEvent`                 | ✅ Wired |
| `kMenuAddCursorAbove`       | Add Cursor Above            | Alt+Cmd+Up            | `AddCursorAboveRequestEvent`                 | ✅ Wired |
| `kMenuAddCursorNextOccurrence` | Add Cursor at Next Match | —                    | `AddCursorNextOccurrenceRequestEvent`        | ✅ Wired |
| `kMenuRemoveSurroundingBrackets` | Remove Surrounding Brackets | —              | `RemoveSurroundingBracketsRequestEvent`      | ✅ Wired |
| `kMenuDuplicateSelectionOrLine`  | Duplicate Selection or Line | —              | `DuplicateSelectionOrLineRequestEvent`       | ✅ Wired |
| `kMenuEnsureFinalNewline`   | Ensure Final Newline        | —                     | `EnsureFinalNewlineRequestEvent`             | ✅ Wired |
| `kMenuDeleteCurrentLine`    | Delete Current Line         | —                     | `DeleteLineRequestEvent`                     | ✅ Wired (DUPLICATE of `kMenuDeleteLine`) |
| `kMenuCopyLineNoSel`        | Copy Line (No Selection)    | —                     | `CopyLineNoSelRequestEvent`                  | ✅ Wired |
| `kMenuAddSelNextMatch`      | Add Selection to Next Match | —                     | `AddSelNextMatchRequestEvent`                | ✅ Wired |
| `kMenuSmartBackspace`       | Smart Backspace             | —                     | `SmartBackspaceRequestEvent`                 | ✅ Wired |
| `kMenuConvertEolLf`         | LF (Unix/macOS)             | —                     | `ConvertEolLfRequestEvent`                   | ✅ Wired |
| `kMenuConvertEolCrlf`       | CRLF (Windows)              | —                     | `ConvertEolCrlfRequestEvent`                 | ✅ Wired |

### 2.3 Format Menu

| Menu ID                | Label                   | Shortcut        | Event / Handler                          | Status |
|------------------------|-------------------------|-----------------|------------------------------------------|--------|
| `kMenuBold`            | Bold                    | Ctrl+B          | `BoldRequestEvent`                       | ✅ Wired |
| `kMenuItalic`          | Italic                  | Ctrl+I          | `ItalicRequestEvent`                     | ✅ Wired |
| `kMenuInlineCode`      | Inline Code             | Ctrl+E          | `InlineCodeRequestEvent`                 | ✅ Wired |
| `kMenuBlockquote`      | Blockquote              | Ctrl+Shift+.    | `BlockquoteRequestEvent`                 | ✅ Wired |
| `kMenuCycleHeading`    | Cycle Heading           | Ctrl+Shift+H    | `CycleHeadingRequestEvent`               | ✅ Wired |
| `kMenuInsertTable`     | Insert Table            | —               | `InsertTableRequestEvent`                | ✅ Wired |
| `kMenuInsertLink`      | Insert Link             | Ctrl+K          | `InsertLinkRequestEvent`                 | ✅ Wired |
| `kMenuShowTableEditor` | Show Table Editor       | —               | `ShowTableEditorRequestEvent`            | ✅ Wired |
| `kMenuHideTableEditor` | Hide Table Editor       | —               | `HideTableEditorRequestEvent`            | ✅ Wired |
| `kMenuToggleSmartList` | Smart List Continuation | —               | `ToggleSmartListContinuationRequestEvent`| ✅ Wired |
| `kMenuInsertSnippet`   | Insert Snippet          | —               | `InsertSnippetRequestEvent`              | ✅ Wired |
| `kMenuAutoPairBold`    | Auto-Pair Bold (**)     | —               | `AutoPairBoldRequestEvent`               | ✅ Wired |
| `kMenuAutoPairItalic`  | Auto-Pair Italic (*)    | —               | `AutoPairItalicRequestEvent`             | ✅ Wired |
| `kMenuAutoPairCode`    | Auto-Pair Code (`)      | —               | `AutoPairCodeRequestEvent`               | ✅ Wired |

### 2.4 View Menu

| Menu ID                          | Label                        | Shortcut        | Event / Handler                                       | Status |
|----------------------------------|------------------------------|-----------------|-------------------------------------------------------|--------|
| `kMenuViewEditor`                | Editor Mode                  | Ctrl+1          | `ViewModeChangedEvent{Editor}`                        | ✅ Wired |
| `kMenuViewSplit`                 | Split Mode                   | Ctrl+2          | `ViewModeChangedEvent{Split}`                         | ✅ Wired |
| `kMenuViewPreview`               | Preview Mode                 | Ctrl+3          | `ViewModeChangedEvent{Preview}`                       | ✅ Wired |
| `kMenuToggleSidebar`             | Toggle Sidebar               | Ctrl+B          | `SidebarToggleEvent`                                  | ✅ Wired |
| `kMenuToggleZenMode`             | Toggle Zen Mode              | Ctrl+K          | `toggleZenMode()` inline                              | ✅ Wired |
| `kMenuWordWrap`                  | Word Wrap                    | Ctrl+Alt+W      | `WrapToggleRequestEvent`                              | ✅ Wired |
| `kMenuCanvasMode`                | Canvas Mode                  | Ctrl+4          | `ShowCanvasWorkspace()` / `ShowEditorWorkspace()`     | ✅ Wired |
| `kMenuFullscreen`                | Toggle Fullscreen            | F11             | `enter_fullscreen()` / `exit_fullscreen()`            | ✅ Wired |
| `kMenuZoomIn`                    | Zoom In                      | Ctrl+=          | `ZoomInRequestEvent`                                  | ✅ Wired |
| `kMenuZoomOut`                   | Zoom Out                     | Ctrl+-          | `ZoomOutRequestEvent`                                 | ✅ Wired |
| `kMenuZoomReset`                 | Reset Zoom                   | Ctrl+0          | `ZoomResetRequestEvent`                               | ✅ Wired |
| `kMenuToggleAutoIndent`          | Toggle Auto-Indent           | —               | `ToggleAutoIndentRequestEvent`                        | ✅ Wired |
| `kMenuToggleBracketMatching`     | Toggle Bracket Matching      | —               | `ToggleBracketMatchingRequestEvent`                   | ✅ Wired |
| `kMenuToggleCodeFolding`         | Toggle Code Folding          | —               | `ToggleCodeFoldingRequestEvent`                       | ✅ Wired |
| `kMenuToggleIndentGuides`        | Toggle Indent Guides         | —               | `ToggleIndentGuidesRequestEvent`                      | ✅ Wired |
| `kMenuToggleScrollBeyond`        | Scroll Beyond Last Line      | —               | `ToggleScrollBeyondLastLineRequestEvent`              | ✅ Wired |
| `kMenuToggleHighlightLine`       | Highlight Current Line       | —               | `ToggleHighlightCurrentLineRequestEvent`              | ✅ Wired |
| `kMenuToggleAutoCloseBrackets`   | Auto-Close Brackets          | —               | `ToggleAutoClosingBracketsRequestEvent`               | ✅ Wired |
| `kMenuToggleStickyScroll`        | Sticky Scroll                | —               | `ToggleStickyScrollRequestEvent`                      | ✅ Wired |
| `kMenuToggleFontLigatures`       | Font Ligatures               | —               | `ToggleFontLigaturesRequestEvent`                     | ✅ Wired |
| `kMenuToggleSmoothCaret`         | Smooth Caret                 | —               | `ToggleSmoothCaretRequestEvent`                       | ✅ Wired |
| `kMenuToggleColorPreview`        | Inline Color Preview         | —               | `ToggleInlineColorPreviewRequestEvent`                | ✅ Wired |
| `kMenuToggleEdgeRuler`           | Edge Column Ruler            | —               | `ToggleEdgeColumnRulerRequestEvent`                   | ✅ Wired |
| `kMenuToggleSmoothScrolling`     | Smooth Scrolling             | —               | `ToggleSmoothScrollingRequestEvent`                   | ✅ Wired |
| `kMenuToggleTrailingWSHighlight` | Trailing WS Highlight        | —               | `ToggleTrailingWSHighlightRequestEvent`               | ✅ Wired |
| `kMenuToggleAutoTrimWS`          | Auto-Trim Trailing WS        | —               | `ToggleAutoTrimWSRequestEvent`                        | ✅ Wired |
| `kMenuToggleGutterSeparator`     | Gutter Separator             | —               | `ToggleGutterSeparatorRequestEvent`                   | ✅ Wired |
| `kMenuToggleInsertFinalNewline`  | Insert Final Newline         | —               | `ToggleInsertFinalNewlineRequestEvent`                | ✅ Wired |
| `kMenuToggleWhitespaceBoundary`  | Whitespace Boundary          | —               | `ToggleWhitespaceBoundaryRequestEvent`                | ✅ Wired |
| `kMenuToggleLinkAutoComplete`    | Link Auto-Complete           | —               | `ToggleLinkAutoCompleteRequestEvent`                  | ✅ Wired |
| `kMenuToggleDragDrop`            | Drag & Drop                  | —               | `ToggleDragDropRequestEvent`                          | ✅ Wired |
| `kMenuToggleAutoSave`            | Auto-Save                    | —               | `ToggleAutoSaveRequestEvent`                          | ✅ Wired |
| `kMenuToggleEmptySelClipboard`   | Empty Selection Clipboard    | —               | `ToggleEmptySelClipboardRequestEvent`                 | ✅ Wired |
| `kMenuCycleRenderWhitespace`     | Cycle Render Whitespace      | —               | `CycleRenderWhitespaceRequestEvent`                   | ✅ Wired |
| `kMenuToggleMinimapR11`          | Toggle Minimap               | —               | `ToggleMinimapRequestEvent`                           | ✅ Wired |
| `kMenuToggleLineNumbers`         | Toggle Line Numbers          | —               | `ToggleLineNumbersRequestEvent`                       | ✅ Wired |
| `kMenuToggleWhitespace`          | Toggle Whitespace            | —               | `ToggleWhitespaceRequestEvent`                        | ✅ Wired |
| `kMenuFoldAll`                   | Fold All Regions             | —               | `FoldAllRequestEvent`                                 | ✅ Wired |
| `kMenuUnfoldAll`                 | Unfold All Regions           | —               | `UnfoldAllRequestEvent`                               | ✅ Wired |
| `kMenuFoldCurrent`               | Fold Current Region          | Ctrl+Alt+[      | `FoldCurrentRequestEvent`                             | ✅ Wired |
| `kMenuUnfoldCurrent`             | Unfold Current Region        | Ctrl+Alt+]      | `UnfoldCurrentRequestEvent`                           | ✅ Wired |
| `kMenuToggleMinimap`             | Toggle Minimap               | —               | `ToggleMinimapRequestEvent`                           | ✅ DUPLICATE of `kMenuToggleMinimapR11` |

### 2.5 Tools, Notebooks, Canvas, Git, Data, Sync Menus

| Menu ID                     | Label                    | Shortcut          | Event / Handler                | Status |
|-----------------------------|--------------------------|-------------------|--------------------------------|--------|
| `kMenuToolsAI`              | AI Assistant             | Ctrl+Shift+A      | ⚠️ **MISSING** handler        |
| `kMenuToolsFlashcardBrowser`| Flashcard Browser        | —                 | ⚠️ **MISSING** handler        |
| `kMenuToolsFlashcardReview` | Flashcard Review         | —                 | ⚠️ **MISSING** handler        |
| `kMenuToolsKnowledgeGraph`  | Knowledge Graph          | Ctrl+Shift+G      | ⚠️ **MISSING** handler        |
| `kMenuToolsPresentation`    | Presentation Mode        | Ctrl+Shift+P      | ⚠️ **MISSING** handler        |
| `kMenuNotebookNew`          | New Notebook             | Ctrl+Alt+N        | ⚠️ **MISSING** handler        |
| `kMenuNotebookRunCell`      | Run Cell                 | Ctrl+Return       | ⚠️ **MISSING** handler        |
| `kMenuNotebookRunAll`       | Run All Cells            | Ctrl+Shift+Return | ⚠️ **MISSING** handler        |
| `kMenuNotebookClearOutputs` | Clear Outputs            | —                 | ⚠️ **MISSING** handler        |
| `kMenuCanvasAddNote`        | Add Sticky Note          | —                 | ⚠️ **MISSING** handler        |
| `kMenuCanvasAddShape`       | Add Shape                | —                 | ⚠️ **MISSING** handler        |
| `kMenuCanvasAddConnector`   | Add Connector            | —                 | ⚠️ **MISSING** handler        |
| `kMenuCanvasToggleGrid`     | Toggle Grid/Snap         | —                 | ⚠️ **MISSING** handler        |
| `kMenuCanvasExportImage`    | Export Board as Image    | —                 | ⚠️ **MISSING** handler        |
| `kMenuGitStatus`            | Status                   | —                 | ⚠️ **MISSING** handler        |
| `kMenuGitStageAll`          | Stage All                | —                 | ⚠️ **MISSING** handler        |
| `kMenuGitCommit`            | Commit                   | Ctrl+K            | ⚠️ **MISSING** handler        |
| `kMenuGitPush`              | Push                     | —                 | ⚠️ **MISSING** handler        |
| `kMenuGitPull`              | Pull                     | —                 | ⚠️ **MISSING** handler        |
| `kMenuGitBlame`             | Blame                    | —                 | ⚠️ **MISSING** handler        |
| `kMenuGitDiff`              | Diff                     | —                 | ⚠️ **MISSING** handler        |
| `kMenuGitHistory`           | History                  | —                 | ⚠️ **MISSING** handler        |
| `kMenuDataNewDB`            | New Database             | —                 | ⚠️ **MISSING** handler        |
| `kMenuDataAddProperty`      | Add Property             | —                 | ⚠️ **MISSING** handler        |
| `kMenuDataViewTable`        | Table                    | —                 | ⚠️ **MISSING** handler        |
| `kMenuDataViewGallery`      | Gallery                  | —                 | ⚠️ **MISSING** handler        |
| `kMenuDataViewKanban`       | Kanban                   | —                 | ⚠️ **MISSING** handler        |
| `kMenuDataViewTimeline`     | Timeline                 | —                 | ⚠️ **MISSING** handler        |
| `kMenuDataImportCSV`        | Import CSV               | —                 | ⚠️ **MISSING** handler        |
| `kMenuDataExport`           | Export                   | —                 | ⚠️ **MISSING** handler        |
| `kMenuSyncConfigure`        | Configure Sync...        | —                 | ⚠️ **MISSING** handler        |
| `kMenuSyncNow`              | Sync Now                 | —                 | ⚠️ **MISSING** handler        |
| `kMenuSyncHistory`          | Sync History             | —                 | ⚠️ **MISSING** handler        |
| `kMenuSyncConflict`         | Conflict Resolution      | —                 | ⚠️ **MISSING** handler        |

### 2.6 Window & Help Menus

| Menu ID             | Label                | Shortcut   | Event / Handler                         | Status |
|---------------------|----------------------|------------|----------------------------------------|--------|
| `kMenuThemeGallery`  | Theme Gallery...     | Ctrl+T     | `SettingsOpenRequestEvent` (redirect)   | ✅ Wired |
| `kMenuPreferences`   | Preferences...       | Ctrl+,     | `SettingsOpenRequestEvent`              | ✅ Wired |
| `kMenuAbout`         | About MarkAmp        | —          | `wxAboutBox` inline                     | ✅ Wired |
| `kMenuWelcomeScreen` | Welcome Screen       | —          | `showStartupScreen()` inline            | ✅ Wired |

---

## 3 · Toolbar (Toolbar.cpp)

| Button Position | Icon ID             | Label           | Click Handler                              | Status |
|-----------------|---------------------|-----------------|--------------------------------------------|--------|
| Left 1          | `toolbar-source-view` | Source/Editor | `ViewModeChangedEvent{Editor}`             | ✅ Wired |
| Left 2          | `toolbar-split-view`  | Split         | `ViewModeChangedEvent{Split}`              | ✅ Wired |
| Left 3          | `toolbar-preview-view`| Preview       | `ViewModeChangedEvent{Preview}`            | ✅ Wired |
| Left 4          | `toolbar-focus`       | Focus/Zen     | `toggleZenMode()` via event                | ✅ Wired |
| Right 1         | `toolbar-save`        | Save          | `TabSaveRequestEvent`                      | ✅ Wired |
| Right 2         | `toolbar-themes`      | Themes        | `SettingsOpenRequestEvent` via event       | ✅ Wired |
| Right 3         | `activity-settings`   | Settings      | `SettingsOpenRequestEvent`                 | ✅ Wired |

---

## 4 · Activity Bar (ActivityBar.cpp)

| Item ID                    | Label            | Icon ID                  | Shortcut        | Event on Click                       | Status |
|----------------------------|------------------|--------------------------|-----------------|--------------------------------------|--------|
| `kFileExplorer`            | Explorer         | `activity-explorer`      | Cmd+Shift+E     | `ActivityBarSelectionEvent`          | ✅ Wired |
| `kSearch`                  | Search           | `activity-search`        | Cmd+Shift+F     | `ActivityBarSelectionEvent`          | ✅ Wired |
| `kNotebooks`               | Notebooks        | `activity-notebooks`     | —               | `ActivityBarSelectionEvent`          | ✅ Wired |
| `kCanvas`                  | Canvas           | `activity-canvas`        | —               | `ActivityBarSelectionEvent`          | ✅ Wired |
| `kGraph`                   | Knowledge Graph  | `activity-graph`         | —               | `ActivityBarSelectionEvent`          | ✅ Wired |
| `kAI`                      | AI Assistant     | `activity-ai`            | —               | `ActivityBarSelectionEvent`          | ✅ Wired |
| `kFlashcards`              | Flashcards       | `activity-flashcards`    | —               | `ActivityBarSelectionEvent`          | ✅ Wired |
| `kGit`                     | Git              | `activity-git`           | —               | `ActivityBarSelectionEvent`          | ✅ Wired |
| `kTasks`                   | Tasks            | `activity-tasks`         | —               | `ActivityBarSelectionEvent`          | ✅ Wired |
| `kDatabase`                | Database         | `activity-database`      | —               | `ActivityBarSelectionEvent`          | ✅ Wired |
| `kPresentation`            | Presentation     | `activity-presentation`  | —               | `ActivityBarSelectionEvent`          | ✅ Wired |
| `kExtensions`              | Extensions       | `activity-extensions`    | Cmd+Shift+X     | `ActivityBarSelectionEvent`          | ✅ Wired |
| `kSettings` (bottom)       | Settings         | `activity-settings`      | —               | `ActivityBarSelectionEvent`          | ✅ Wired |
| `kThemes` (bottom)         | Themes           | `toolbar-themes`         | —               | `ActivityBarSelectionEvent`          | ✅ Wired |
| `kAccount` (bottom)        | Accounts         | `activity-account`       | —               | `ActivityBarSelectionEvent`          | ✅ Wired |

---

## 5 · Status Bar (StatusBarPanel.cpp)

### 5.1 Left Zone

| Item                 | Data Source                            | Click Action                    | Icon     | Status |
|----------------------|----------------------------------------|---------------------------------|----------|--------|
| Ready/Modified state | `ready_state_`, `file_modified_`       | —                               | —        | ✅ |
| Cursor position      | `cursor_line_`, `cursor_col_`          | `GoToLineRequestEvent`          | —        | ✅ |
| Encoding             | `encoding_`                            | Cycles UTF-8/ASCII/ISO-8859-1   | —        | ✅ |
| EOL mode             | `eol_mode_`                            | Cycles LF/CRLF/CR              | —        | ✅ |
| Indent mode          | `indent_mode_`                         | Cycles Spaces:4/2/Tabs          | —        | ✅ |
| Zoom level           | `zoom_level_`                          | —                               | —        | ✅ |
| View mode            | `view_mode_`                           | —                               | —        | ✅ |
| Progress spinner     | `progress_active_`, `progress_label_`  | —                               | —        | ✅ |
| Git branch           | `git_branch_`                          | `CommandPaletteEvent`           | `status-git` | ✅ |
| Git sync status      | `git_ahead_`, `git_behind_`            | `CommandPaletteEvent`           | —        | ✅ |
| Panel notifications  | `panel_errors_`, `panel_warnings_`     | `ToggleBottomPanelRequestEvent` | —        | ✅ |
| Sidebar mode         | `sidebar_mode_name_`                   | `SidebarToggleEvent`            | —        | ✅ |
| Group indicator      | `active_group_`                        | —                               | —        | ✅ |
| AI status            | `ai_provider_`, `ai_connected_`        | `SidebarModeChangedEvent`       | —        | ✅ |
| Filename             | `filename_`, `file_modified_`          | —                               | FileType icon | ✅ |

### 5.2 Right Zone

| Item            | Data Source             | Click Action                     | Status |
|-----------------|--------------------------|----------------------------------|--------|
| Word count      | `word_count_`            | —                                | ✅ |
| Reading time    | `word_count_` / 200      | —                                | ✅ |
| Character count | `char_count_`            | —                                | ✅ |
| Selection length| `selection_len_`         | —                                | ✅ |
| Mermaid status  | `mermaid_status_`        | —                                | ✅ |
| Theme name      | `theme_name_`            | —                                | ✅ |
| Language        | `language_`              | Cycles Markdown/Plain Text/HTML  | ✅ |
| Line count      | `line_count_`            | —                                | ✅ |
| File size       | `file_size_bytes_`       | —                                | ✅ |

### 5.3 Extension Status Bar Items Gap

⚠️ `StatusBarItemService::create_item()` allows extensions to create status bar items, but `StatusBarPanel::RebuildItems()` does **not** query `StatusBarItemService` for extension-contributed items. Extension items are stored but never rendered.

---

## 6 · Command Palette (CommandPalette.cpp + MainFrame.cpp)

`MainFrame::RegisterPaletteCommands()` (lines 2649+) registers commands with `command_palette_->RegisterCommand(...)`. Only a subset of commands have palette entries:

| Category | Commands Registered | Gap |
|----------|--------------------|----|
| File     | Open Folder, Save  | ⚠️ Most File commands not registered |
| View     | Editor/Split/Preview Mode, Toggle Sidebar, Zen Mode, Toggle Fullscreen | ⚠️ ~40 View toggles not registered |
| Edit     | Most edit commands rely on menu shortcuts only | ⚠️ Not searchable via palette |
| Format   | —                  | ⚠️ No format commands registered |
| Tools    | —                  | ⚠️ No tools commands registered |

---

## 7 · Keyboard Shortcuts (ShortcutManager via MainFrame::RegisterDefaultShortcuts)

50+ shortcuts registered. Key shortcuts with **no attached callback** (action is `{}` empty):

| Shortcut ID          | Most shortcuts use empty callbacks, relying on menu accelerator table |
|----------------------|----------------------------------------------------------------------|

> **Gap:** Most `ShortcutManager` entries have `{}` (empty) action callbacks. The shortcuts display correctly in the shortcuts overlay and command palette hints, but don't fire via `ShortcutManager::process_key_event()`. Actual execution relies on the wxWidgets accelerator table attached to menu items. This is a dual-path issue.

---

## 8 · Context Menus

| Surface          | Source                            | Items | Status |
|-----------------|-----------------------------------|-------|--------|
| Activity Bar     | `ActivityBar::OnRightClick()`     | Dynamic: show/hide items per panel | ✅ Wired |
| Tab Bar          | `LayoutManager` (via right-click) | Close/Close Other/Pin/Unpin | ✅ Wired |
| File Tree        | `FileTreePanel::OnRightClick()`   | Open/Rename/Delete/Copy Path | ✅ Wired |

---

## 9 · Tab Bar Events

| Event                    | Publisher          | Subscriber             | Status |
|--------------------------|-------------------|------------------------|--------|
| `TabSwitchedEvent`       | TabBar → EventBus | MainFrame, LayoutManager | ✅ |
| `TabCloseRequestEvent`   | Menu / TabBar     | LayoutManager          | ✅ |
| `TabSaveRequestEvent`    | Toolbar / Menu    | LayoutManager          | ✅ |
| `ActiveFileChangedEvent` | LayoutManager     | MainFrame (title bar)  | ✅ |

---

## 10 · Findings Summary

### 10.1 Missing Handlers (menu items with no event binding)

| Count | Category            |
|-------|---------------------|
| 7     | Export/Import       |
| 5     | Tools               |
| 4     | Notebook            |
| 5     | Canvas              |
| 8     | Git                 |
| 8     | Data (AV)           |
| 4     | Sync                |
| **41**| **Total MISSING**   |

### 10.2 Orphaned Services (service emits events with no UI subscriber)

| Service                | Event Emitted                   | UI Subscriber | Status |
|------------------------|--------------------------------|----|--------|
| `QuickPickService`     | `ShowQuickPickRequestEvent`    | None | ⚠️ Orphaned |
| `InputBoxService`      | `ShowInputBoxRequestEvent`     | None | ⚠️ Orphaned |
| `StatusBarItemService` | Items stored in service        | Not read by `StatusBarPanel` | ⚠️ Orphaned |

### 10.3 Duplicate Bindings

| Binding                | Menu IDs                               |
|------------------------|----------------------------------------|
| Delete Current Line     | `kMenuDeleteLine` + `kMenuDeleteCurrentLine` both fire `DeleteLineRequestEvent` |
| Toggle Minimap          | `kMenuToggleMinimap` + `kMenuToggleMinimapR11` both fire `ToggleMinimapRequestEvent` |

### 10.4 Shortcut Conflicts

| Shortcut   | Conflict                              |
|-----------|---------------------------------------|
| Ctrl+K    | `kMenuGitCommit` vs `kMenuToggleZenMode` (both bind Ctrl+K) |

### 10.5 Command Palette Gap

Only ~8 of 130+ commands are registered in the palette. Phase 02+ should auto-register all commands via the canonical command contract (P01-T02).
