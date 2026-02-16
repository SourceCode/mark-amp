add_executable(markamp
    main.cpp
    app/MarkAmpApp.cpp
    ui/MainFrame.cpp
    ui/CustomChrome.cpp
    ui/BevelPanel.cpp
    ui/ThemeAwareWindow.cpp
    ui/ThemedScrollbar.cpp
    ui/LayoutManager.cpp
    ui/ToolWindowHost.cpp
    # V8 Phase 12: Surface link router + transition coordinator
    core/SurfaceLink.cpp
    ui/SurfaceTransitionCoordinator.cpp
    # V8 Phase 12: VSCode theme infrastructure
    core/VsCodeThemeAdapter.cpp
    core/ThemeScopeMapper.cpp
    ui/SplitterBar.cpp
    ui/StatusBarPanel.cpp
    ui/FileTreeCtrl.cpp
    ui/EditorPanel.cpp
    ui/PreviewPanel.cpp
    ui/SplitView.cpp
    ui/Toolbar.cpp
    ui/ThemeGallery.cpp
    ui/ThemeTokenEditor.cpp
    ui/ThemePreviewCard.cpp
    ui/ShortcutOverlay.cpp
    ui/CommandPalette.cpp
    ui/StartupPanel.cpp
    ui/BreadcrumbBar.cpp
    ui/FloatingFormatBar.cpp
    ui/LinkPreviewPopover.cpp
    ui/ImagePreviewPopover.cpp
    ui/TableEditorOverlay.cpp
    ui/TabBar.cpp
    ui/SettingsPanel.cpp
    ui/SettingsDialog.cpp
    ui/NotificationManager.cpp
    ui/ActivityBar.cpp
    ui/ExtensionsBrowserPanel.cpp
    ui/ExtensionCard.cpp
    ui/ExtensionDetailPanel.cpp
    ui/OutputPanel.cpp
    ui/ProblemsPanel.cpp
    ui/TreeViewHost.cpp
    ui/WebviewHostPanel.cpp
    ui/WalkthroughPanel.cpp
    core/Logger.cpp
    core/PluginManager.cpp
    core/FeatureRegistry.cpp
    core/BuiltInPlugins.cpp
    core/ExtensionManifest.cpp
    core/ExtensionScanner.cpp
    core/ExtensionStorage.cpp
    core/ExtensionEnablement.cpp
    core/VsixService.cpp
    core/HttpClient.cpp
    core/GalleryService.cpp
    core/ExtensionManagement.cpp
    core/ContextKeyService.cpp
    core/WhenClause.cpp
    core/OutputChannelService.cpp
    core/DiagnosticsService.cpp
    core/TreeDataProviderRegistry.cpp
    core/WebviewService.cpp
    core/DecorationService.cpp
    core/FileSystemProviderRegistry.cpp
    core/LanguageProviderRegistry.cpp
    core/NotificationService.cpp
    core/StatusBarItemService.cpp
    core/InputBoxService.cpp
    core/QuickPickService.cpp
    core/SnippetEngine.cpp
    core/WorkspaceService.cpp
    core/TextEditorService.cpp
    core/ProgressService.cpp
    core/ExtensionEvents.cpp
    core/EnvironmentService.cpp
    core/GrammarEngine.cpp
    core/TerminalService.cpp
    core/TaskRunnerService.cpp
    core/ExtensionHostRecovery.cpp
    core/ExtensionRecommendations.cpp
    core/ExtensionTelemetry.cpp
    core/ExtensionSandbox.cpp
    core/EventBus.cpp
    core/AppState.cpp
    core/Command.cpp
    core/Config.cpp
    core/SettingsCatalog.cpp
    core/Color.cpp
    core/Theme.cpp
    core/BuiltInThemes.cpp
    core/ThemeRegistry.cpp
    core/ThemeValidator.cpp
    core/ThemeEngine.cpp
    core/FileNode.cpp
    core/SampleFiles.cpp
    core/FileSystem.cpp
    core/EncodingDetector.cpp
    core/RecentFiles.cpp
    core/MarkdownDocument.cpp
    core/Md4cWrapper.cpp
    core/MarkdownParser.cpp
    core/RecentWorkspaces.cpp
    core/SyntaxHighlighter.cpp
    core/ShortcutManager.cpp
    core/AccessibilityManager.cpp
    core/Profiler.cpp
    core/StartupTimer.cpp
    core/MermaidRenderer.cpp
    core/MathRenderer.cpp
    core/HtmlSanitizer.cpp
    core/PieceTable.cpp
    core/LineIndex.cpp
    core/AsyncHighlighter.cpp
    core/AsyncFileLoader.cpp
    core/IncrementalSearcher.cpp
    core/loader/ThemeLoader.cpp
    rendering/HtmlRenderer.cpp
    rendering/CodeBlockRenderer.cpp
    rendering/MermaidBlockRenderer.cpp
    # V8 Phase 13: FX Engine
    rendering/FxEngine.cpp
    rendering/FxMotionPreset.cpp
    rendering/TextFxRenderer.cpp
    rendering/FxPresetRegistry.cpp
    rendering/FxSafetyController.cpp
    # V3 Phases 1–3: Block Data Model, ID Generation, Notebook
    core/Block.cpp
    core/BlockID.cpp
    core/Notebook.cpp
    # V3 Phases 4–6: SQLite Storage, Document FS, Block Tree Index
    core/BlockDatabase.cpp
    core/DocumentFileSystem.cpp
    core/BlockTreeIndex.cpp
    # V3 Phases 7–10: Core CRUD, IAL, References, Backlinks
    core/IALParser.cpp
    core/BlockService.cpp
    core/AttributeService.cpp
    core/BlockRef.cpp
    core/RefParser.cpp
    core/RefResolver.cpp
    core/Backlink.cpp
    core/BacklinkService.cpp
    # V3 Phases 11–16: Navigation & Query
    core/Outline.cpp
    core/OutlineService.cpp
    core/Tag.cpp
    core/Bookmark.cpp
    core/BookmarkService.cpp
    core/Search.cpp
    core/SearchService.cpp
    core/SqlQuery.cpp
    core/QueryService.cpp
    ui/SearchPanel.cpp

    # ── Attribute Views (Phases 17–23) ──
    core/av/AVTypes.cpp
    core/av/AVValue.cpp
    core/av/AttributeView.cpp
    core/av/AttributeViewStore.cpp
    core/av/AVColumnType.cpp
    core/av/AVFormatters.cpp
    core/av/AVValidators.cpp
    core/av/AVFilter.cpp
    core/av/AVSort.cpp
    core/av/AVQueryEngine.cpp
    core/av/AVRelationService.cpp
    core/av/AVRollupEngine.cpp
    ui/av/AVTablePanel.cpp
    ui/av/AVCellRenderer.cpp
    ui/av/AVCellEditor.cpp
    ui/av/AVHeaderRenderer.cpp
    ui/av/AVFilterBar.cpp
    ui/av/AVGalleryPanel.cpp
    ui/av/AVGalleryCard.cpp
    ui/av/AVGalleryConfig.cpp
    ui/av/AVKanbanPanel.cpp
    ui/av/AVKanbanLane.cpp
    ui/av/AVKanbanCard.cpp
    ui/av/AVRelationEditor.cpp
    ui/av/AVRollupConfigDialog.cpp
    # Phase 24: FSRS Scheduling Algorithm
    core/fsrs/FSRSTypes.cpp
    core/fsrs/FSRSEngine.cpp
    core/fsrs/FlashcardStore.cpp
    core/fsrs/FSRSReviewSession.cpp
    # Phase 25: Flashcard Deck Management
    core/DeckStore.cpp
    core/DeckManager.cpp
    # Phase 26: Review Session & UI
    core/ReviewSession.cpp
    core/CardFactory.cpp
    ui/FlashcardReviewPanel.cpp
    ui/FlashcardBrowserPanel.cpp
    # Phase 27: Knowledge Graph Data
    core/ReferenceScanner.cpp
    core/GraphService.cpp
    # Phase 28: Knowledge Graph Visualization
    core/ForceDirectedLayout.cpp
    ui/GraphPanel.cpp
    ui/GraphMiniMap.cpp
    ui/GraphFilterBar.cpp
    # V4 Phase 01: Block Model and Document Architecture
    core/WikiLink.cpp
    core/WikiLinkParser.cpp
    core/DocumentModel.cpp
    # V4 Phase 02: Vault Management System
    core/VaultService.cpp
    # V4 Phase 03: Bidirectional Link Index
    core/BacklinkIndex.cpp
    # V4 Phase 04: Tag System
    core/TagService.cpp
    # V4 Phase 05: Full-Text Search Engine
    core/SearchEngine.cpp
    # V4 Phase 07: Graph Engine
    core/GraphEngine.cpp
    # V4 Phase 06: Backlinks Panel
    ui/BacklinksPanel.cpp
    # V4 Phase 08: Graph View Panel
    ui/GraphViewPanel.cpp
    # V4 Phase 09: Daily Notes Service
    core/DailyNoteService.cpp
    # V4 Phase 10: Embed Resolver
    core/EmbedResolver.cpp
    # V4 Phase 11: Link Suggestion Service
    core/LinkSuggestionService.cpp
    # V4 Phase 12: Outline Panel Controller
    core/OutlinePanelController.cpp
    # V4 Phase 13: Live Preview Renderer
    ui/LivePreviewRenderer.cpp
    # V4 Phase 14: Note Template Engine
    core/NoteTemplateEngine.cpp
    # V4 Phase 15: Search Query Parser
    core/SearchQueryParser.cpp
    # V4 Phase 16: Find/Replace
    core/FindReplaceService.cpp
    # V4 Phase 17: Clipboard
    core/ClipboardService.cpp
    # V4 Phase 18: LaTeX Engine
    core/LatexEngine.cpp
    # V4 Phase 19: Pane Manager
    ui/PaneManager.cpp
    # V4 Phase 20: Navigation Service
    ui/NavigationService.cpp
    # V4 Phase 21: Workspace Layout
    ui/WorkspaceLayout.cpp
    # V4 Phase 22: Vault Style Service
    core/VaultStyleService.cpp
    # V4 Phase 23: Theme Marketplace
    core/ThemeMarketplace.cpp
    # V4 Phase 24: Task Service
    core/TaskService.cpp
    # V4 Phase 25: Kanban Engine
    ui/KanbanEngine.cpp
    # V4 Phase 26: Calendar Engine
    ui/CalendarEngine.cpp
    # V4 Phase 27: Presentation Engine
    ui/PresentationEngine.cpp
    # V4 Phase 28: Encryption Service
    core/EncryptionService.cpp
    # V4 Phase 29: Kernel Manager
    core/KernelManager.cpp
    # V4 Phase 30: Kernel Adapters
    core/KernelAdapters.cpp
    # V4 Phase 31: Cell Output Renderer
    ui/CellOutputRenderer.cpp
    # V4 Phase 32: DataFrame Renderer
    ui/DataFrameRenderer.cpp
    # V4 Phase 33: Widget Manager
    core/WidgetManager.cpp
    # V4 Phase 34: Notebook Cell Manager
    core/NotebookCellManager.cpp
    # V4 Phase 35: Git Service
    core/GitService.cpp
    # V4 Phase 36: Local Graph Engine
    core/LocalGraphEngine.cpp
    # V4 Phase 37: Notebook Export Engine
    core/NotebookExportEngine.cpp
    # V4 Phase 38: Magic Command Engine
    core/MagicCommandEngine.cpp
    # V4 Phase 39: File Snapshot Service
    core/FileSnapshotService.cpp
    # V4 Phase 40: Notebook Diff Engine
    core/NotebookDiffEngine.cpp
    # V5 Phase 01: Canvas Data Model & Coordinate System
    canvas/CanvasTypes.cpp
    canvas/CanvasObject.cpp
    canvas/Quadtree.cpp
    canvas/ViewportTransform.cpp
    # V5 Phase 02: Canvas Rendering Engine
    canvas/CanvasRenderer.cpp
    canvas/TileCache.cpp
    ui/CanvasPanel.cpp
    # V8 Phase 6: Canvas Workbench Shell
    ui/CanvasWorkspacePanel.cpp
    # V8 Phase 7: Realtime Collaboration
    core/CanvasCollabService.cpp
    canvas/CommentObject.cpp
    ui/CanvasCommentsPanel.cpp
    ui/CanvasFacilitationPanel.cpp
    # V8 Phase 8: Canvas Apps Marketplace
    canvas/AppWidgetObject.cpp
    core/CanvasIntegrationService.cpp
    # V5 Phase 03: Canvas Input System
    canvas/CanvasInputManager.cpp
    canvas/SelectTool.cpp
    canvas/PanTool.cpp
    # V5 Phase 04: Object Selection & Transform
    canvas/SelectionManager.cpp
    canvas/SelectionRenderer.cpp
    # V5 Phase 05: Canvas Serialization & Board Model
    canvas/Board.cpp
    canvas/BoardSerializer.cpp
    canvas/UndoRedoStack.cpp
    canvas/CanvasCommands.cpp
    # V5 Phase 06: Grid, Snap & Alignment
    canvas/SnapEngine.cpp
    canvas/AlignmentGuides.cpp
    # V5 Phase 07: Sticky Notes
    canvas/StickyNote.cpp
    canvas/StickyNoteRenderer.cpp
    # V5 Phase 08: Text Boxes
    canvas/TextBox.cpp
    canvas/TextBoxRenderer.cpp
    # V5 Phase 09: Shapes Library
    canvas/ShapeData.cpp
    canvas/ShapeRenderer.cpp
    # V5 Phase 10: Connectors & Lines
    canvas/ConnectorData.cpp
    canvas/ConnectorRenderer.cpp
    # V5 Phase 11: Freehand Drawing Tool
    canvas/FreehandPath.cpp
    canvas/FreehandPathRenderer.cpp
    canvas/DrawTool.cpp
    # V5 Phase 12: Smart Drawing Recognition
    canvas/ShapeRecognizer.cpp
    # V5 Phase 13: Image & File Objects
    canvas/ImageObject.cpp
    canvas/ImageObjectRenderer.cpp
    # V5 Phase 14: Frames
    canvas/FrameObject.cpp
    canvas/FrameRenderer.cpp
    # V5 Phase 15: Board Sections
    canvas/SectionObject.cpp
    canvas/SectionRenderer.cpp
    # V5 Phase 16: Object Grouping & Nesting
    canvas/GroupObject.cpp
    canvas/GroupingService.cpp
    # V5 Phase 17: Object Layering / Z-Index
    canvas/LayeringService.cpp
    # V5 Phase 18: Object Locking
    canvas/LockingService.cpp
    # V5 Phase 19: Table Creation & Editing
    canvas/TableObject.cpp
    canvas/TableRenderer.cpp
    # V5 Phase 20: Table Sorting & Filtering
    canvas/TableSortFilter.cpp
    # V5 Phase 21: Object Metadata System
    canvas/MetadataPanel.cpp
    # V5 Phase 22: Tags System
    canvas/TagManager.cpp
    canvas/TagPanel.cpp
    # V5 Phase 23: Minimap / Board Map
    canvas/MinimapPanel.cpp
    # V5 Phase 24: Canvas Search
    canvas/CanvasSearch.cpp
    canvas/SearchBar.cpp
    # V5 Phase 25: Outline Panel
    canvas/OutlinePanel.cpp
    # V5 Phase 26: Cross-Board Linking
    canvas/CrossBoardLink.cpp
    canvas/BoardNavigator.cpp
    # V5 Phase 27: PDF Import & Page Rendering
    canvas/PDFPageObject.cpp
    canvas/PDFImporter.cpp
    # V5 Phase 28: Embedded Video
    canvas/VideoEmbedObject.cpp
    # V5 Phase 29: Bookmark Cards
    canvas/BookmarkCardObject.cpp
    canvas/MetadataScraper.cpp
    # V5 Phase 30: Custom Icons & Emoji Packs
    canvas/IconObject.cpp
    canvas/IconLibrary.cpp
    # V5 Phase 31: Smart Layout Engine
    canvas/LayoutEngine.cpp
    canvas/StickyAggregator.cpp
    # V5 Phase 32: Mind Map Tool
    canvas/MindMapNode.cpp
    canvas/MindMapController.cpp
    # V5 Phase 33: Kanban Board
    canvas/KanbanObjects.cpp
    canvas/KanbanController.cpp
    # V5 Phase 34: Diagram Libraries (UML/BPMN)
    canvas/DiagramShapeObject.cpp
    canvas/DiagramLibraryPanel.cpp
    # V5 Phase 35: Board Templates System
    canvas/BoardTemplate.cpp
)

# Platform-specific sources
if(APPLE)
    target_sources(markamp PRIVATE platform/MacPlatform.mm)
    # Enable Objective-C++ for .mm files
    set_source_files_properties(platform/MacPlatform.mm PROPERTIES
        COMPILE_FLAGS "-x objective-c++"
    )
elseif(WIN32)
    target_sources(markamp PRIVATE platform/WinPlatform.cpp)
elseif(UNIX)
    target_sources(markamp PRIVATE platform/LinuxPlatform.cpp)
endif()

# Include paths -- src/ is the root for includes like "app/MarkAmpApp.h"
target_include_directories(markamp PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Source groups for IDEs
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Src" FILES
    main.cpp
    # App
    app/MarkAmpApp.h
    app/MarkAmpApp.cpp
    # UI
    ui/MainFrame.h
    ui/MainFrame.cpp
    ui/CustomChrome.h
    ui/CustomChrome.cpp
    ui/BevelPanel.h
    ui/BevelPanel.cpp
    ui/ThemeAwareWindow.h
    ui/ThemeAwareWindow.cpp
    ui/ThemedScrollbar.h
    ui/ThemedScrollbar.cpp
    ui/LayoutManager.h
    ui/LayoutManager.cpp
    ui/ToolWindowHost.h
    ui/ToolWindowHost.cpp
    # V8 Phase 12: Surface link router + transition coordinator
    core/SurfaceLink.h
    core/SurfaceLink.cpp
    ui/SurfaceTransitionCoordinator.h
    ui/SurfaceTransitionCoordinator.cpp
    # V8 Phase 12: VSCode theme infrastructure
    core/VsCodeThemeAdapter.h
    core/VsCodeThemeAdapter.cpp
    core/ThemeScopeMapper.h
    core/ThemeScopeMapper.cpp
    ui/SplitterBar.h
    ui/SplitterBar.cpp
    ui/StatusBarPanel.h
    ui/StatusBarPanel.cpp
    ui/FileTreeCtrl.h
    ui/FileTreeCtrl.cpp
    ui/EditorPanel.h
    ui/EditorPanel.cpp
    ui/PreviewPanel.h
    ui/PreviewPanel.cpp
    ui/SplitView.h
    ui/SplitView.cpp
    ui/Toolbar.h
    ui/Toolbar.cpp
    ui/ThemeGallery.h
    ui/ThemeGallery.cpp
    ui/ThemePreviewCard.h
    ui/ThemePreviewCard.cpp
    ui/ThemeTokenEditor.h
    ui/ThemeTokenEditor.cpp
    ui/ShortcutOverlay.h
    ui/ShortcutOverlay.cpp
    ui/CommandPalette.h
    ui/CommandPalette.cpp
    ui/StartupPanel.h
    ui/StartupPanel.cpp
    ui/BreadcrumbBar.h
    ui/BreadcrumbBar.cpp
    ui/FloatingFormatBar.h
    ui/FloatingFormatBar.cpp
    ui/LinkPreviewPopover.h
    ui/LinkPreviewPopover.cpp
    ui/ImagePreviewPopover.h
    ui/ImagePreviewPopover.cpp
    ui/TableEditorOverlay.h
    ui/TableEditorOverlay.cpp
    ui/TabBar.h
    ui/TabBar.cpp
    ui/SettingsPanel.h
    ui/SettingsPanel.cpp
    ui/SettingsDialog.h
    ui/SettingsDialog.cpp
    ui/NotificationManager.h
    ui/NotificationManager.cpp
    ui/ActivityBar.h
    ui/ActivityBar.cpp
    ui/ExtensionsBrowserPanel.h
    ui/ExtensionsBrowserPanel.cpp
    ui/ExtensionCard.h
    ui/ExtensionCard.cpp
    ui/ExtensionDetailPanel.h
    ui/ExtensionDetailPanel.cpp
    ui/OutputPanel.h
    ui/OutputPanel.cpp
    ui/ProblemsPanel.h
    ui/ProblemsPanel.cpp
    ui/TreeViewHost.h
    ui/TreeViewHost.cpp
    ui/WebviewHostPanel.h
    ui/WebviewHostPanel.cpp
    ui/WalkthroughPanel.h
    ui/WalkthroughPanel.cpp
    # Core
    core/Logger.h
    core/Logger.cpp
    core/EventBus.h
    core/EventBus.cpp
    core/Events.h
    core/Types.h
    core/SPSCQueue.h
    core/IThemeEngine.h
    core/IFileSystem.h
    core/IMarkdownParser.h
    core/IMermaidRenderer.h
    core/IPlugin.h
    core/PluginContext.h
    core/AppState.h
    core/AppState.cpp
    core/Command.h
    core/Command.cpp
    core/Config.h
    core/Config.cpp
    core/SettingsCatalog.h
    core/SettingsCatalog.cpp
    core/Color.h
    core/Color.cpp
    core/Theme.h
    core/Theme.cpp
    core/BuiltInThemes.h
    core/BuiltInThemes.cpp
    core/ThemeRegistry.h
    core/ThemeRegistry.cpp
    core/ThemeValidator.h
    core/ThemeValidator.cpp
    core/ThemeEngine.h
    core/ThemeEngine.cpp
    core/PluginManager.h
    core/PluginManager.cpp
    core/BuiltInPlugins.h
    core/BuiltInPlugins.cpp
    core/FeatureRegistry.h
    core/FeatureRegistry.cpp
    core/ExtensionManifest.h
    core/ExtensionManifest.cpp
    core/ExtensionScanner.h
    core/ExtensionScanner.cpp
    core/ExtensionStorage.h
    core/ExtensionStorage.cpp
    core/ExtensionEnablement.h
    core/ExtensionEnablement.cpp
    core/VsixService.h
    core/VsixService.cpp
    core/HttpClient.h
    core/HttpClient.cpp
    core/GalleryService.h
    core/GalleryService.cpp
    core/ExtensionManagement.h
    core/ExtensionManagement.cpp
    core/ExtensionHostRecovery.h
    core/ExtensionHostRecovery.cpp
    core/ExtensionRecommendations.h
    core/ExtensionRecommendations.cpp
    core/ExtensionTelemetry.h
    core/ExtensionTelemetry.cpp
    core/ExtensionSandbox.h
    core/ExtensionSandbox.cpp
    core/ContextKeyService.h
    core/ContextKeyService.cpp
    core/WhenClause.h
    core/WhenClause.cpp
    core/OutputChannelService.h
    core/OutputChannelService.cpp
    core/DiagnosticsService.h
    core/DiagnosticsService.cpp
    core/TreeDataProviderRegistry.h
    core/TreeDataProviderRegistry.cpp
    core/WebviewService.h
    core/WebviewService.cpp
    core/DecorationService.h
    core/DecorationService.cpp
    core/FileSystemProviderRegistry.h
    core/FileSystemProviderRegistry.cpp
    core/LanguageProviderRegistry.h
    core/LanguageProviderRegistry.cpp
    core/NotificationService.h
    core/NotificationService.cpp
    core/StatusBarItemService.h
    core/StatusBarItemService.cpp
    core/InputBoxService.h
    core/InputBoxService.cpp
    core/QuickPickService.h
    core/QuickPickService.cpp
    core/SnippetEngine.h
    core/SnippetEngine.cpp
    core/WorkspaceService.h
    core/WorkspaceService.cpp
    core/TextEditorService.h
    core/TextEditorService.cpp
    core/ProgressService.h
    core/ProgressService.cpp
    core/ExtensionEvents.h
    core/ExtensionEvents.cpp
    core/EnvironmentService.h
    core/EnvironmentService.cpp
    core/GrammarEngine.h
    core/GrammarEngine.cpp
    core/TerminalService.h
    core/TerminalService.cpp
    core/TaskRunnerService.h
    core/TaskRunnerService.cpp
    core/FileNode.h
    core/FileNode.cpp
    core/SampleFiles.h
    core/SampleFiles.cpp
    core/FileSystem.h
    core/FileSystem.cpp
    core/EncodingDetector.h
    core/EncodingDetector.cpp
    core/RecentFiles.h
    core/RecentFiles.cpp
    core/RecentWorkspaces.h
    core/RecentWorkspaces.cpp
    core/MarkdownParser.h
    core/MarkdownParser.cpp
    core/MarkdownDocument.cpp
    core/Md4cWrapper.h
    core/Md4cWrapper.cpp
    core/SyntaxHighlighter.h
    core/SyntaxHighlighter.cpp
    core/ShortcutManager.h
    core/ShortcutManager.cpp
    core/AccessibilityManager.h
    core/AccessibilityManager.cpp
    core/Profiler.h
    core/Profiler.cpp
    core/StartupTimer.h
    core/StartupTimer.cpp
    core/MermaidRenderer.h
    core/MermaidRenderer.cpp
    core/IMathRenderer.h
    core/MathRenderer.h
    core/MathRenderer.cpp
    core/HtmlSanitizer.h
    core/HtmlSanitizer.cpp
    core/PieceTable.h
    core/PieceTable.cpp
    core/LineIndex.h
    core/LineIndex.cpp
    core/AsyncHighlighter.h
    core/AsyncHighlighter.cpp
    core/AsyncFileLoader.h
    core/AsyncFileLoader.cpp
    core/IncrementalSearcher.h
    core/IncrementalSearcher.cpp
    # Core header-only utilities
    core/AdaptiveThrottle.h
    core/AsyncPipeline.h
    core/ChunkedStorage.h
    core/CoalescingTask.h
    core/CompilerHints.h
    core/DocumentSnapshot.h
    core/FrameArena.h
    core/FrameBudgetToken.h
    core/FrameScheduler.h
    core/GenerationCounter.h
    core/GraphemeBoundaryCache.h
    core/IMECompositionOverlay.h
    core/InputPriorityDispatcher.h
    core/StableLineId.h
    core/StringUtils.h
    core/StyleRunStore.h
    core/TextSpan.h
    # Core loader
    core/loader/ThemeLoader.h
    core/loader/ThemeLoader.cpp
    # Rendering
    rendering/HtmlRenderer.h
    rendering/HtmlRenderer.cpp
    rendering/CodeBlockRenderer.h
    rendering/CodeBlockRenderer.cpp
    rendering/MermaidBlockRenderer.h
    rendering/MermaidBlockRenderer.cpp
    rendering/CaretOverlay.h
    rendering/DirtyRegion.h
    rendering/DoubleBufferedPaint.h
    rendering/GlyphAdvanceCache.h
    rendering/HitTestAccelerator.h
    rendering/IncrementalLineWrap.h
    rendering/PrefetchManager.h
    rendering/ScrollBlit.h
    rendering/SelectionPainter.h
    rendering/ViewportCache.h
    # V8 Phase 13: FX Engine
    rendering/FxPass.h
    rendering/FxEngine.h
    rendering/FxEngine.cpp
    rendering/FxMotionPreset.h
    rendering/FxMotionPreset.cpp
    rendering/TextFxRenderer.h
    rendering/TextFxRenderer.cpp
    rendering/FxPresetRegistry.h
    rendering/FxPresetRegistry.cpp
    rendering/FxSafetyController.h
    rendering/FxSafetyController.cpp
    # Platform
    platform/PlatformAbstraction.h
    platform/MacPlatform.h
    platform/WinPlatform.h
    platform/LinuxPlatform.h
)

# Apply quality tooling per-target
markamp_set_warnings(markamp)
markamp_configure_sanitizers(markamp)
markamp_configure_lto(markamp)
markamp_configure_tracy(markamp)

# Link all third-party dependencies
target_link_libraries(markamp PRIVATE markamp_dependencies)

if(APPLE)
    target_link_libraries(markamp PRIVATE "-framework Cocoa")
endif()

# Windows resource file for application icon
if(WIN32)
    target_sources(markamp PRIVATE markamp.rc)
endif()

# ── Config defaults externalization ──
# Pass CMAKE_SOURCE_DIR so Config::defaults_file_path() can locate the JSON fallback in dev builds.
target_compile_definitions(markamp PRIVATE CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

# Copy config_defaults.json to build tree for dev/test usage
set(CONFIG_DEFAULTS_SRC "${CMAKE_SOURCE_DIR}/resources/config_defaults.json")
set(CONFIG_DEFAULTS_DST "${CMAKE_BINARY_DIR}/resources/config_defaults.json")
add_custom_command(
    TARGET markamp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${CMAKE_BINARY_DIR}/resources"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CONFIG_DEFAULTS_SRC}" "${CONFIG_DEFAULTS_DST}"
    COMMENT "Copying config_defaults.json to build tree"
)

# ── Application icon support ──

# Copy icon resources to the build tree so the executable can find them at runtime.
# The icon is resolved relative to the executable path in MainFrame::findIconPath().
set(ICON_SRC "${CMAKE_SOURCE_DIR}/resources/icons/markamp.png")
set(ICON_DST_DIR "${CMAKE_BINARY_DIR}/resources/icons")

add_custom_command(
    TARGET markamp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E make_directory "${ICON_DST_DIR}"
    COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ICON_SRC}" "${ICON_DST_DIR}/markamp.png"
    COMMENT "Copying application icon to build tree"
)

# macOS: Generate .icns and set bundle properties
if(APPLE)
    set(ICNS_OUTPUT "${CMAKE_BINARY_DIR}/markamp.icns")

    # Generate .icns from PNG using sips + iconutil (macOS built-in tools).
    add_custom_command(
        OUTPUT "${ICNS_OUTPUT}"
        COMMAND bash "${CMAKE_SOURCE_DIR}/cmake/generate_icns.sh" "${ICON_SRC}" "${ICNS_OUTPUT}"
        DEPENDS "${ICON_SRC}" "${CMAKE_SOURCE_DIR}/cmake/generate_icns.sh"
        COMMENT "Generating macOS .icns icon from PNG"
    )

    # Ensure the .icns is built when the target is built
    add_custom_target(markamp_icns DEPENDS "${ICNS_OUTPUT}")
    add_dependencies(markamp markamp_icns)

    # Set macOS bundle properties so the icon appears in Finder/Dock
    set_target_properties(markamp PROPERTIES
        MACOSX_BUNDLE TRUE
        MACOSX_BUNDLE_BUNDLE_NAME "MarkAmp"
        MACOSX_BUNDLE_GUI_IDENTIFIER "com.markamp.editor"
        MACOSX_BUNDLE_BUNDLE_VERSION "${PROJECT_VERSION}"
        MACOSX_BUNDLE_SHORT_VERSION_STRING "${PROJECT_VERSION}"
        MACOSX_BUNDLE_ICON_FILE "markamp.icns"
        MACOSX_BUNDLE_INFO_PLIST "${CMAKE_SOURCE_DIR}/packaging/macos/Info.plist"
    )

    # Copy .icns into the .app bundle Resources directory
    set_source_files_properties("${ICNS_OUTPUT}" PROPERTIES
        MACOSX_PACKAGE_LOCATION "Resources"
    )
    target_sources(markamp PRIVATE "${ICNS_OUTPUT}")

    # Also copy the PNG icon into the bundle Resources/icons/ for wxIcon loading
    add_custom_command(
        TARGET markamp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "$<TARGET_BUNDLE_DIR:markamp>/Contents/Resources/icons"
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ICON_SRC}" "$<TARGET_BUNDLE_DIR:markamp>/Contents/Resources/icons/markamp.png"
        COMMENT "Copying PNG icon into macOS .app bundle"
    )

    # Copy config_defaults.json into the macOS bundle Resources
    add_custom_command(
        TARGET markamp POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${CONFIG_DEFAULTS_SRC}" "$<TARGET_BUNDLE_DIR:markamp>/Contents/Resources/config_defaults.json"
        COMMENT "Copying config_defaults.json into macOS .app bundle"
    )
endif()
