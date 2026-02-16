# Test sources live under tests/unit/
# Each test links Catch2 and, when needed, the markamp core sources.

# --- Core source library (shared by tests that test core components) ---
add_library(markamp_core STATIC
    # Core infrastructure
    ${CMAKE_SOURCE_DIR}/src/core/EventBus.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AppState.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Command.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Config.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Logger.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Color.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Theme.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BuiltInThemes.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ThemeRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ThemeValidator.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ThemeEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FileNode.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SampleFiles.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FileSystem.cpp
    ${CMAKE_SOURCE_DIR}/src/core/EncodingDetector.cpp
    ${CMAKE_SOURCE_DIR}/src/core/RecentFiles.cpp
    ${CMAKE_SOURCE_DIR}/src/core/MarkdownDocument.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Md4cWrapper.cpp
    ${CMAKE_SOURCE_DIR}/src/core/MarkdownParser.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SyntaxHighlighter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ShortcutManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AccessibilityManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Profiler.cpp
    ${CMAKE_SOURCE_DIR}/src/core/StartupTimer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/MermaidRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/HtmlSanitizer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PieceTable.cpp
    ${CMAKE_SOURCE_DIR}/src/core/LineIndex.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AsyncHighlighter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AsyncFileLoader.cpp
    ${CMAKE_SOURCE_DIR}/src/core/IncrementalSearcher.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FeatureRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/HtmlRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/CodeBlockRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/loader/ThemeLoader.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/MermaidBlockRenderer.cpp
    # Plugin & Extension infrastructure
    ${CMAKE_SOURCE_DIR}/src/core/PluginManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BuiltInPlugins.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionManifest.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionScanner.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionStorage.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionEnablement.cpp
    ${CMAKE_SOURCE_DIR}/src/core/VsixService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/HttpClient.cpp
    ${CMAKE_SOURCE_DIR}/src/core/GalleryService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionManagement.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionHostRecovery.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionRecommendations.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionTelemetry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionSandbox.cpp
    # Extension API services (P1–P4)
    ${CMAKE_SOURCE_DIR}/src/core/ContextKeyService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WhenClause.cpp
    ${CMAKE_SOURCE_DIR}/src/core/OutputChannelService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/DiagnosticsService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TreeDataProviderRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WebviewService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/DecorationService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FileSystemProviderRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/LanguageProviderRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/NotificationService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/StatusBarItemService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/InputBoxService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/QuickPickService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SnippetEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WorkspaceService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TextEditorService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ProgressService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionEvents.cpp
    ${CMAKE_SOURCE_DIR}/src/core/EnvironmentService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/GrammarEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TerminalService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TaskRunnerService.cpp
    # UI panels needed by tests
    ${CMAKE_SOURCE_DIR}/src/ui/OutputPanel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ProblemsPanel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/TreeViewHost.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/WebviewHostPanel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/WalkthroughPanel.cpp
    # V3 Phases 1–3: Block Data Model, ID Generation, Notebook
    ${CMAKE_SOURCE_DIR}/src/core/Block.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BlockID.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Notebook.cpp
    # V3 Phases 4–6: SQLite Storage, Document FS, Block Tree Index
    ${CMAKE_SOURCE_DIR}/src/core/BlockDatabase.cpp
    ${CMAKE_SOURCE_DIR}/src/core/DocumentFileSystem.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BlockTreeIndex.cpp
    # V4 Phase 01: Block Model and Document Architecture
    ${CMAKE_SOURCE_DIR}/src/core/WikiLink.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WikiLinkParser.cpp
    ${CMAKE_SOURCE_DIR}/src/core/DocumentModel.cpp
    # V4 Phase 02: Vault Management System
    ${CMAKE_SOURCE_DIR}/src/core/VaultService.cpp
    # V4 Phase 03: Bidirectional Link Index
    ${CMAKE_SOURCE_DIR}/src/core/BacklinkIndex.cpp
    # V4 Phase 04: Tag System
    ${CMAKE_SOURCE_DIR}/src/core/TagService.cpp
    # V4 Phase 05: Full-Text Search Engine
    ${CMAKE_SOURCE_DIR}/src/core/SearchEngine.cpp
    # V4 Phase 07: Graph Engine
    ${CMAKE_SOURCE_DIR}/src/core/GraphEngine.cpp
    # V4 Phase 06: Backlinks Panel
    ${CMAKE_SOURCE_DIR}/src/ui/BacklinksPanel.cpp
    # V4 Phase 08: Graph View Panel
    ${CMAKE_SOURCE_DIR}/src/ui/GraphViewPanel.cpp
    # V4 Phase 09: Daily Notes Service
    ${CMAKE_SOURCE_DIR}/src/core/DailyNoteService.cpp
    # V4 Phase 10: Embed Resolver
    ${CMAKE_SOURCE_DIR}/src/core/EmbedResolver.cpp
    # V4 Phase 11: Link Suggestion Service
    ${CMAKE_SOURCE_DIR}/src/core/LinkSuggestionService.cpp
    # V4 Phase 12: Outline Panel Controller
    ${CMAKE_SOURCE_DIR}/src/core/OutlinePanelController.cpp
    # V4 Phase 13: Live Preview Renderer
    ${CMAKE_SOURCE_DIR}/src/ui/LivePreviewRenderer.cpp
    # V4 Phase 14: Note Template Engine
    ${CMAKE_SOURCE_DIR}/src/core/NoteTemplateEngine.cpp
    # V4 Phase 15: Search Query Parser
    ${CMAKE_SOURCE_DIR}/src/core/SearchQueryParser.cpp
    # V4 Phase 16: Find/Replace
    ${CMAKE_SOURCE_DIR}/src/core/FindReplaceService.cpp
    # V4 Phase 17: Clipboard
    ${CMAKE_SOURCE_DIR}/src/core/ClipboardService.cpp
    # V4 Phase 18: LaTeX Engine
    ${CMAKE_SOURCE_DIR}/src/core/LatexEngine.cpp
    # V4 Phase 19: Pane Manager
    ${CMAKE_SOURCE_DIR}/src/ui/PaneManager.cpp
    # V4 Phase 20: Navigation Service
    ${CMAKE_SOURCE_DIR}/src/ui/NavigationService.cpp
    # V8 Phase 11: Tool Window Host
    ${CMAKE_SOURCE_DIR}/src/ui/ToolWindowHost.cpp
    # V8 Phase 12: Surface link router + transition coordinator
    ${CMAKE_SOURCE_DIR}/src/core/SurfaceLink.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SurfaceTransitionCoordinator.cpp
    # V8 Phase 12: VSCode theme infrastructure
    ${CMAKE_SOURCE_DIR}/src/core/VsCodeThemeAdapter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ThemeScopeMapper.cpp
    # V8 Phase 13: FX Engine
    ${CMAKE_SOURCE_DIR}/src/rendering/FxEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/FxMotionPreset.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/TextFxRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/FxPresetRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/FxSafetyController.cpp
    # V4 Phase 21: Workspace Layout
    ${CMAKE_SOURCE_DIR}/src/ui/WorkspaceLayout.cpp
    # V4 Phase 22: Vault Style Service
    ${CMAKE_SOURCE_DIR}/src/core/VaultStyleService.cpp
    # V4 Phase 23: Theme Marketplace
    ${CMAKE_SOURCE_DIR}/src/core/ThemeMarketplace.cpp
    # V4 Phase 24: Task Service
    ${CMAKE_SOURCE_DIR}/src/core/TaskService.cpp
    # V4 Phase 25: Kanban Engine
    ${CMAKE_SOURCE_DIR}/src/ui/KanbanEngine.cpp
    # V4 Phase 26: Calendar Engine
    ${CMAKE_SOURCE_DIR}/src/ui/CalendarEngine.cpp
    # V4 Phase 27: Presentation Engine
    ${CMAKE_SOURCE_DIR}/src/ui/PresentationEngine.cpp
    # V4 Phase 28: Encryption Service
    ${CMAKE_SOURCE_DIR}/src/core/EncryptionService.cpp
    # V4 Phase 29: Kernel Manager
    ${CMAKE_SOURCE_DIR}/src/core/KernelManager.cpp
    # V4 Phase 30: Kernel Adapters
    ${CMAKE_SOURCE_DIR}/src/core/KernelAdapters.cpp
    # V4 Phase 31: Cell Output Renderer
    ${CMAKE_SOURCE_DIR}/src/ui/CellOutputRenderer.cpp
    # V4 Phase 32: DataFrame Renderer
    ${CMAKE_SOURCE_DIR}/src/ui/DataFrameRenderer.cpp
    # V4 Phase 33: Widget Manager
    ${CMAKE_SOURCE_DIR}/src/core/WidgetManager.cpp
    # V4 Phase 34: Notebook Cell Manager
    ${CMAKE_SOURCE_DIR}/src/core/NotebookCellManager.cpp
    # V4 Phase 35: Git Service
    ${CMAKE_SOURCE_DIR}/src/core/GitService.cpp
    # V4 Phase 36: Local Graph Engine
    ${CMAKE_SOURCE_DIR}/src/core/LocalGraphEngine.cpp
    # V4 Phase 37: Notebook Export Engine
    ${CMAKE_SOURCE_DIR}/src/core/NotebookExportEngine.cpp
    # V4 Phase 38: Magic Command Engine
    ${CMAKE_SOURCE_DIR}/src/core/MagicCommandEngine.cpp
    # V4 Phase 39: File Snapshot Service
    ${CMAKE_SOURCE_DIR}/src/core/FileSnapshotService.cpp
    # V4 Phase 40: Notebook Diff Engine
    ${CMAKE_SOURCE_DIR}/src/core/NotebookDiffEngine.cpp
    # V5 Phase 01: Canvas Data Model & Coordinate System
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasTypes.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasObject.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/Quadtree.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/ViewportTransform.cpp
    # V5 Phase 02: Canvas Rendering Engine
    ${CMAKE_SOURCE_DIR}/src/canvas/TileCache.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasRenderer.cpp
    # V5 Phase 03: Canvas Input System
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasInputManager.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/SelectTool.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/PanTool.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/CanvasPanel.cpp
    # V5 Phase 04: Object Selection & Transform
    ${CMAKE_SOURCE_DIR}/src/canvas/SelectionManager.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/SelectionRenderer.cpp
    # V5 Phase 05: Canvas Serialization & Board Model
    ${CMAKE_SOURCE_DIR}/src/canvas/Board.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/BoardSerializer.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/UndoRedoStack.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasCommands.cpp
    # V5 Phase 06: Grid, Snap & Alignment
    ${CMAKE_SOURCE_DIR}/src/canvas/SnapEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/AlignmentGuides.cpp
    # V5 Phase 07: Sticky Notes
    ${CMAKE_SOURCE_DIR}/src/canvas/StickyNote.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/StickyNoteRenderer.cpp
    # V5 Phase 08: Text Boxes
    ${CMAKE_SOURCE_DIR}/src/canvas/TextBox.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/TextBoxRenderer.cpp
    # V5 Phase 09: Shapes Library
    ${CMAKE_SOURCE_DIR}/src/canvas/ShapeData.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/ShapeRenderer.cpp
    # V5 Phase 10: Connectors & Lines
    ${CMAKE_SOURCE_DIR}/src/canvas/ConnectorData.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/ConnectorRenderer.cpp
    # V5 Phase 11: Freehand Drawing Tool
    ${CMAKE_SOURCE_DIR}/src/canvas/FreehandPath.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/FreehandPathRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/DrawTool.cpp
    # V5 Phase 12: Smart Drawing Recognition
    ${CMAKE_SOURCE_DIR}/src/canvas/ShapeRecognizer.cpp
    # V5 Phase 13: Image & File Objects
    ${CMAKE_SOURCE_DIR}/src/canvas/ImageObject.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/ImageObjectRenderer.cpp
    # V5 Phase 14: Frames
    ${CMAKE_SOURCE_DIR}/src/canvas/FrameObject.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/FrameRenderer.cpp
    # V5 Phase 15: Board Sections
    ${CMAKE_SOURCE_DIR}/src/canvas/SectionObject.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/SectionRenderer.cpp
    # V5 Phase 16: Object Grouping & Nesting
    ${CMAKE_SOURCE_DIR}/src/canvas/GroupObject.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/GroupingService.cpp
    # V5 Phase 17: Object Layering / Z-Index
    ${CMAKE_SOURCE_DIR}/src/canvas/LayeringService.cpp
    # V5 Phase 18: Object Locking
    ${CMAKE_SOURCE_DIR}/src/canvas/LockingService.cpp
    # V5 Phase 19: Table Creation & Editing
    ${CMAKE_SOURCE_DIR}/src/canvas/TableObject.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/TableRenderer.cpp
    # V5 Phase 20: Table Sorting & Filtering
    ${CMAKE_SOURCE_DIR}/src/canvas/TableSortFilter.cpp
    # V5 Phase 21: Object Metadata System
    ${CMAKE_SOURCE_DIR}/src/canvas/MetadataPanel.cpp
    # V5 Phase 22: Tags System
    ${CMAKE_SOURCE_DIR}/src/canvas/TagManager.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/TagPanel.cpp
    # V5 Phase 23: Minimap / Board Map
    ${CMAKE_SOURCE_DIR}/src/canvas/MinimapPanel.cpp
    # V5 Phase 24: Canvas Search
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasSearch.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/SearchBar.cpp
    # V5 Phase 25: Outline Panel
    ${CMAKE_SOURCE_DIR}/src/canvas/OutlinePanel.cpp
    # V5 Phase 26: Cross-Board Linking
    ${CMAKE_SOURCE_DIR}/src/canvas/CrossBoardLink.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/BoardNavigator.cpp
    # V5 Phase 27: PDF Import & Page Rendering
    ${CMAKE_SOURCE_DIR}/src/canvas/PDFPageObject.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/PDFImporter.cpp
    # V5 Phase 28: Embedded Video
    ${CMAKE_SOURCE_DIR}/src/canvas/VideoEmbedObject.cpp
    # V5 Phase 29: Bookmark Cards
    ${CMAKE_SOURCE_DIR}/src/canvas/BookmarkCardObject.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/MetadataScraper.cpp
    # V5 Phase 30: Custom Icons & Emoji Packs
    ${CMAKE_SOURCE_DIR}/src/canvas/IconObject.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/IconLibrary.cpp
    # V5 Phase 31: Smart Layout Engine
    ${CMAKE_SOURCE_DIR}/src/canvas/LayoutEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/StickyAggregator.cpp
    # V5 Phase 32: Mind Map Tool
    ${CMAKE_SOURCE_DIR}/src/canvas/MindMapNode.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/MindMapController.cpp
    # V5 Phase 33: Kanban Board
    ${CMAKE_SOURCE_DIR}/src/canvas/KanbanObjects.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/KanbanController.cpp
    # V5 Phase 34: Diagram Libraries (UML/BPMN)
    ${CMAKE_SOURCE_DIR}/src/canvas/DiagramShapeObject.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/DiagramLibraryPanel.cpp
    # V5 Phase 35: Board Templates System
    ${CMAKE_SOURCE_DIR}/src/canvas/BoardTemplate.cpp
    # V8 Phase 5: Settings Catalog
    ${CMAKE_SOURCE_DIR}/src/core/SettingsCatalog.cpp
    # V8 Phase 7: Realtime Collaboration
    ${CMAKE_SOURCE_DIR}/src/core/CanvasCollabService.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CommentObject.cpp
    # V8 Phase 8: Canvas Apps Marketplace
    ${CMAKE_SOURCE_DIR}/src/canvas/AppWidgetObject.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CanvasIntegrationService.cpp
)
target_include_directories(markamp_core PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(markamp_core PUBLIC markamp_dependencies)
target_compile_definitions(markamp_core PUBLIC CMAKE_SOURCE_DIR="${CMAKE_SOURCE_DIR}")

# ... (omitted existing tests) ...

# --- Theme Loader test ---
add_executable(test_theme_loader
    unit/test_theme_loader.cpp
)
target_include_directories(test_theme_loader PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_theme_loader PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_theme_loader COMMAND test_theme_loader)

# --- Editor QoL test ---
add_executable(test_editor_qol
    unit/test_editor_qol.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/EditorPanel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ThemeAwareWindow.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/FloatingFormatBar.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/LinkPreviewPopover.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/TableEditorOverlay.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ImagePreviewPopover.cpp
)
target_include_directories(test_editor_qol PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_editor_qol PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_editor_qol COMMAND test_editor_qol)

# --- Performance Infrastructure test ---
add_executable(test_performance_infra
    unit/test_performance_infra.cpp
)
target_include_directories(test_performance_infra PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_performance_infra PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_performance_infra COMMAND test_performance_infra)

# --- Performance Patterns #21-#40 test ---
add_executable(test_perf_patterns_21_40
    unit/test_perf_patterns_21_40.cpp
)
target_include_directories(test_perf_patterns_21_40 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_perf_patterns_21_40 PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_perf_patterns_21_40 COMMAND test_perf_patterns_21_40)

# --- Split View Advanced test ---
add_executable(test_split_view_advanced
    unit/test_split_view_advanced.cpp
)
target_include_directories(test_split_view_advanced PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_split_view_advanced PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_split_view_advanced COMMAND test_split_view_advanced)

# --- Sidebar Filter test ---
add_executable(test_sidebar_filter
    unit/test_sidebar_filter.cpp
)
target_include_directories(test_sidebar_filter PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_sidebar_filter PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_sidebar_filter COMMAND test_sidebar_filter)

# --- Mermaid Phase 3 test ---
add_executable(test_mermaid_phase3
    unit/test_mermaid_phase3.cpp
)
target_include_directories(test_mermaid_phase3 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_mermaid_phase3 PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_mermaid_phase3 COMMAND test_mermaid_phase3)

# Phase 4: Tokenized Theme Design Engine
add_executable(test_theme_phase4
    unit/test_theme_phase4.cpp
)
target_include_directories(test_theme_phase4 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_theme_phase4 PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_theme_phase4 COMMAND test_theme_phase4)

# Phase 8: Visual Polish & Spatial Design
add_executable(test_visual_polish
    unit/test_visual_polish.cpp
)
target_include_directories(test_visual_polish PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_visual_polish PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_visual_polish COMMAND test_visual_polish)

# --- Crash regression tests ---
add_executable(test_crash_regressions
    unit/test_crash_regressions.cpp
)
target_include_directories(test_crash_regressions PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_crash_regressions PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_crash_regressions COMMAND test_crash_regressions)

# --- Feature Registry test ---
add_executable(test_feature_registry
    unit/test_feature_registry.cpp
)
target_include_directories(test_feature_registry PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_feature_registry PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_feature_registry COMMAND test_feature_registry)

# --- Extension Manifest test ---
add_executable(test_extension_manifest
    unit/test_extension_manifest.cpp
)
target_include_directories(test_extension_manifest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_extension_manifest PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_extension_manifest COMMAND test_extension_manifest)

# --- Extension Scanner / Storage / Enablement test ---
add_executable(test_extension_scanner
    unit/test_extension_scanner.cpp
)
target_include_directories(test_extension_scanner PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_extension_scanner PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_extension_scanner COMMAND test_extension_scanner)

# --- Plugin Manager V2 test ---
add_executable(test_plugin_manager_v2
    unit/test_plugin_manager_v2.cpp
)
target_include_directories(test_plugin_manager_v2 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_plugin_manager_v2 PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_plugin_manager_v2 COMMAND test_plugin_manager_v2)

# --- VSIX Service test ---
add_executable(test_vsix_service
    unit/test_vsix_service.cpp
)
target_include_directories(test_vsix_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_vsix_service PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_vsix_service COMMAND test_vsix_service)

# --- Gallery Service test ---
add_executable(test_gallery_service
    unit/test_gallery_service.cpp
)
target_include_directories(test_gallery_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_gallery_service PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_gallery_service COMMAND test_gallery_service)

# --- Extension Management test ---
add_executable(test_extension_management
    unit/test_extension_management.cpp
)
target_include_directories(test_extension_management PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_extension_management PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_extension_management COMMAND test_extension_management)

# --- Built-in Plugins test (Phase 9/10) ---
add_executable(test_builtin_plugins
    unit/test_builtin_plugins.cpp
)
target_include_directories(test_builtin_plugins PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_builtin_plugins PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_builtin_plugins COMMAND test_builtin_plugins)

# --- Context Keys & When-Clause test ---
add_executable(test_context_keys
    unit/test_context_keys.cpp
)
target_include_directories(test_context_keys PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_context_keys PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_context_keys COMMAND test_context_keys)

# --- Extension Services test (Batch 4-5) ---
add_executable(test_extension_services
    unit/test_extension_services.cpp
)
target_include_directories(test_extension_services PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_extension_services PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_extension_services COMMAND test_extension_services)

# --- Extension Integration test (Batch 8) ---
add_executable(test_extension_integration
    unit/test_extension_integration.cpp
)
target_include_directories(test_extension_integration PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_extension_integration PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_extension_integration COMMAND test_extension_integration)

# --- P1-P4 Extension Services test ---
add_executable(test_p1_p4_services
    unit/test_p1_p4_services.cpp
)
target_include_directories(test_p1_p4_services PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_p1_p4_services PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_p1_p4_services COMMAND test_p1_p4_services)

# --- Phase 13: Service Wiring integration test ---
add_executable(test_service_wiring
    unit/test_service_wiring.cpp
)
target_include_directories(test_service_wiring PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_service_wiring PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_service_wiring COMMAND test_service_wiring)

# --- Phase 13: EventBus Queuing test ---
add_executable(test_eventbus_queuing
    unit/test_eventbus_queuing.cpp
)
target_include_directories(test_eventbus_queuing PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_eventbus_queuing PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_eventbus_queuing COMMAND test_eventbus_queuing)

# --- Phase 13: Built-in Plugin Behavior test ---
add_executable(test_builtin_plugin_behavior
    unit/test_builtin_plugin_behavior.cpp
)
target_include_directories(test_builtin_plugin_behavior PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_builtin_plugin_behavior PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_builtin_plugin_behavior COMMAND test_builtin_plugin_behavior)

# --- Phase 13: Feature Guard Integration test ---
add_executable(test_feature_guard_integration
    unit/test_feature_guard_integration.cpp
)
target_include_directories(test_feature_guard_integration PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_feature_guard_integration PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_feature_guard_integration COMMAND test_feature_guard_integration)

# --- Phase 13: Panel Data Flow test ---
add_executable(test_panel_data_flow
    unit/test_panel_data_flow.cpp
)
target_include_directories(test_panel_data_flow PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_panel_data_flow PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_panel_data_flow COMMAND test_panel_data_flow)

# --- Phase 20: Performance Optimization test ---
add_executable(test_phase20_perf
    unit/test_phase20_perf.cpp
)
target_include_directories(test_phase20_perf PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase20_perf PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase20_perf COMMAND test_phase20_perf)

# ============================================================================
# V3 Phase 01: Block Data Model test
# ============================================================================
add_executable(test_block_model
    unit/test_block_model.cpp
)
target_include_directories(test_block_model PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_block_model PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_block_model COMMAND test_block_model)

# ============================================================================
# V3 Phase 02: Block ID Generation test
# ============================================================================
add_executable(test_block_id
    unit/test_block_id.cpp
)
target_include_directories(test_block_id PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_block_id PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_block_id COMMAND test_block_id)

# ============================================================================
# V3 Phase 03: Notebook Data Model test
# ============================================================================
add_executable(test_notebook
    unit/test_notebook.cpp
)
target_include_directories(test_notebook PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_notebook PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_notebook COMMAND test_notebook)

# ============================================================================
# V3 Phases 4–6: BlockDatabase, DocumentFileSystem, BlockTreeIndex test
# ============================================================================
add_executable(test_phases_4_6
    unit/test_phases_4_6.cpp
)
target_include_directories(test_phases_4_6 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phases_4_6 PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phases_4_6 COMMAND test_phases_4_6)

# ============================================================================
# V4 Phase 01: Document Model test
# ============================================================================
add_executable(test_document_model
    unit/test_document_model.cpp
)
target_include_directories(test_document_model PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_document_model PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_document_model COMMAND test_document_model)

# ============================================================================
# V4 Phase 02: Vault Service test
# ============================================================================
add_executable(test_vault_service
    unit/test_vault_service.cpp
)
target_include_directories(test_vault_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_vault_service PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_vault_service COMMAND test_vault_service)

# ============================================================================
# V4 Phase 03: Backlink Index test
# ============================================================================
add_executable(test_backlink_index
    unit/test_backlink_index.cpp
)
target_include_directories(test_backlink_index PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_backlink_index PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_backlink_index COMMAND test_backlink_index)

# ============================================================================
# V4 Phase 04: Tag Service test
# ============================================================================
add_executable(test_tag_service
    unit/test_tag_service.cpp
)
target_include_directories(test_tag_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_tag_service PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_tag_service COMMAND test_tag_service)

# ============================================================================
# V4 Phase 05: Search Engine test
# ============================================================================
add_executable(test_search_engine
    unit/test_search_engine.cpp
)
target_include_directories(test_search_engine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_search_engine PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_search_engine COMMAND test_search_engine)

# ============================================================================
# V4 Phase 07: Graph Engine test
# ============================================================================
add_executable(test_graph_engine
    unit/test_graph_engine.cpp
)
target_include_directories(test_graph_engine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_graph_engine PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_graph_engine COMMAND test_graph_engine)

# ============================================================================
# V4 Phase 06: Backlinks Panel test
# ============================================================================
add_executable(test_backlinks_panel
    unit/test_backlinks_panel.cpp
)
target_include_directories(test_backlinks_panel PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_backlinks_panel PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_backlinks_panel COMMAND test_backlinks_panel)

# ============================================================================
# V4 Phase 08: Graph View Panel test
# ============================================================================
add_executable(test_graph_view_panel
    unit/test_graph_view_panel.cpp
)
target_include_directories(test_graph_view_panel PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_graph_view_panel PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_graph_view_panel COMMAND test_graph_view_panel)

# ============================================================================
# V4 Phase 09: Daily Notes test
# ============================================================================
add_executable(test_daily_notes
    unit/test_daily_notes.cpp
)
target_include_directories(test_daily_notes PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_daily_notes PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_daily_notes COMMAND test_daily_notes)

# ============================================================================
# V4 Phase 10: Embed Resolver test
# ============================================================================
add_executable(test_embed_resolver
    unit/test_embed_resolver.cpp
)
target_include_directories(test_embed_resolver PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_embed_resolver PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_embed_resolver COMMAND test_embed_resolver)

# ============================================================================
# V4 Phase 11: Link Suggestions test
# ============================================================================
add_executable(test_link_suggestions
    unit/test_link_suggestions.cpp
)
target_include_directories(test_link_suggestions PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_link_suggestions PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_link_suggestions COMMAND test_link_suggestions)

# ============================================================================
# V4 Phase 12: Outline Panel test
# ============================================================================
add_executable(test_outline_panel
    unit/test_outline_panel.cpp
)
target_include_directories(test_outline_panel PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_outline_panel PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_outline_panel COMMAND test_outline_panel)

# ============================================================================
# V4 Phase 13: Live Preview test
# ============================================================================
add_executable(test_live_preview
    unit/test_live_preview.cpp
)
target_include_directories(test_live_preview PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_live_preview PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_live_preview COMMAND test_live_preview)

# ============================================================================
# V4 Phase 14: Note Template Engine test
# ============================================================================
add_executable(test_note_template_engine
    unit/test_note_template_engine.cpp
)
target_include_directories(test_note_template_engine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_note_template_engine PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_note_template_engine COMMAND test_note_template_engine)

# ============================================================================
# V4 Phase 15: Search Query Parser test
# ============================================================================
add_executable(test_search_query_parser
    unit/test_search_query_parser.cpp
)
target_include_directories(test_search_query_parser PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_search_query_parser PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_search_query_parser COMMAND test_search_query_parser)

# ============================================================================
# V4 Phase 16: Find/Replace test
# ============================================================================
add_executable(test_find_replace
    unit/test_find_replace.cpp
)
target_include_directories(test_find_replace PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_find_replace PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_find_replace COMMAND test_find_replace)

# ============================================================================
# V4 Phase 17: Clipboard Service test
# ============================================================================
add_executable(test_clipboard_service
    unit/test_clipboard_service.cpp
)
target_include_directories(test_clipboard_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_clipboard_service PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_clipboard_service COMMAND test_clipboard_service)

# ============================================================================
# V4 Phase 18: LaTeX Engine test
# ============================================================================
add_executable(test_latex_engine
    unit/test_latex_engine.cpp
)
target_include_directories(test_latex_engine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_latex_engine PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_latex_engine COMMAND test_latex_engine)

# ============================================================================
# V4 Phase 19: Pane Manager test
# ============================================================================
add_executable(test_pane_manager
    unit/test_pane_manager.cpp
)
target_include_directories(test_pane_manager PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_pane_manager PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_pane_manager COMMAND test_pane_manager)

# ============================================================================
# V4 Phase 20: Navigation Service test
# ============================================================================
add_executable(test_navigation_service
    unit/test_navigation_service.cpp
)
target_include_directories(test_navigation_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_navigation_service PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_navigation_service COMMAND test_navigation_service)

# ============================================================================
# V4 Phase 21: Workspace Layout test
# ============================================================================
add_executable(test_workspace_layout
    unit/test_workspace_layout.cpp
)
target_include_directories(test_workspace_layout PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_workspace_layout PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_workspace_layout COMMAND test_workspace_layout)

# ============================================================================
# V4 Phase 22: Vault Style Service test
# ============================================================================
add_executable(test_vault_style
    unit/test_vault_style.cpp
)
target_include_directories(test_vault_style PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_vault_style PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_vault_style COMMAND test_vault_style)

# ============================================================================
# V4 Phase 23: Theme Marketplace test
# ============================================================================
add_executable(test_theme_marketplace
    unit/test_theme_marketplace.cpp
)
target_include_directories(test_theme_marketplace PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_theme_marketplace PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_theme_marketplace COMMAND test_theme_marketplace)

# ============================================================================
# V4 Phase 24: Task Service test
# ============================================================================
add_executable(test_task_service
    unit/test_task_service.cpp
)
target_include_directories(test_task_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_task_service PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_task_service COMMAND test_task_service)

# ============================================================================
# V4 Phase 25: Kanban Engine test
# ============================================================================
add_executable(test_kanban_engine
    unit/test_kanban_engine.cpp
)
target_include_directories(test_kanban_engine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_kanban_engine PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_kanban_engine COMMAND test_kanban_engine)

# ============================================================================
# V4 Phase 26: Calendar Engine test
# ============================================================================
add_executable(test_calendar
    unit/test_calendar.cpp
)
target_include_directories(test_calendar PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_calendar PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_calendar COMMAND test_calendar)

# ============================================================================
# V4 Phase 27: Presentation Engine test
# ============================================================================
add_executable(test_presentation
    unit/test_presentation.cpp
)
target_include_directories(test_presentation PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_presentation PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_presentation COMMAND test_presentation)

# ============================================================================
# V4 Phase 28: Encryption Service test
# ============================================================================
add_executable(test_encryption_service
    unit/test_encryption_service.cpp
)
target_include_directories(test_encryption_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_encryption_service PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_encryption_service COMMAND test_encryption_service)

# ============================================================================
# V4 Phase 29: Kernel Manager test
# ============================================================================
add_executable(test_kernel_manager
    unit/test_kernel_manager.cpp
)
target_include_directories(test_kernel_manager PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_kernel_manager PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_kernel_manager COMMAND test_kernel_manager)

# ============================================================================
# V4 Phase 30: Kernel Adapters test
# ============================================================================
add_executable(test_kernel_adapters
    unit/test_kernel_adapters.cpp
)
target_include_directories(test_kernel_adapters PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_kernel_adapters PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_kernel_adapters COMMAND test_kernel_adapters)

# ============================================================================
# V4 Phase 31: Cell Output Renderer test
# ============================================================================
add_executable(test_cell_output
    unit/test_cell_output.cpp
)
target_include_directories(test_cell_output PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_cell_output PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_cell_output COMMAND test_cell_output)

# ============================================================================
# V4 Phase 32: DataFrame Renderer test
# ============================================================================
add_executable(test_dataframe_renderer
    unit/test_dataframe_renderer.cpp
)
target_include_directories(test_dataframe_renderer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_dataframe_renderer PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_dataframe_renderer COMMAND test_dataframe_renderer)

# ============================================================================
# V4 Phase 33: Widget Manager test
# ============================================================================
add_executable(test_widget_manager
    unit/test_widget_manager.cpp
)
target_include_directories(test_widget_manager PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_widget_manager PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_widget_manager COMMAND test_widget_manager)

# ============================================================================
# V4 Phase 34: Notebook Cells test
# ============================================================================
add_executable(test_notebook_cells
    unit/test_notebook_cells.cpp
)
target_include_directories(test_notebook_cells PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_notebook_cells PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_notebook_cells COMMAND test_notebook_cells)

# ============================================================================
# V4 Phase 35: Git Service test
# ============================================================================
add_executable(test_git_service
    unit/test_git_service.cpp
)
target_include_directories(test_git_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_git_service PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_git_service COMMAND test_git_service)

# ============================================================================
# V4 Phase 36: Local Graph Engine test
# ============================================================================
add_executable(test_local_graph
    unit/test_local_graph.cpp
)
target_include_directories(test_local_graph PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_local_graph PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_local_graph COMMAND test_local_graph)

# ============================================================================
# V4 Phase 37: Notebook Export Engine test
# ============================================================================
add_executable(test_notebook_export
    unit/test_notebook_export.cpp
)
target_include_directories(test_notebook_export PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_notebook_export PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_notebook_export COMMAND test_notebook_export)

# ============================================================================
# V4 Phase 38: Magic Commands test
# ============================================================================
add_executable(test_magic_commands
    unit/test_magic_commands.cpp
)
target_include_directories(test_magic_commands PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_magic_commands PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_magic_commands COMMAND test_magic_commands)

# ============================================================================
# V4 Phase 39: File Snapshot Service test
# ============================================================================
add_executable(test_file_snapshots
    unit/test_file_snapshots.cpp
)
target_include_directories(test_file_snapshots PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_file_snapshots PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_file_snapshots COMMAND test_file_snapshots)

# ============================================================================
# V4 Phase 40: Notebook Diff Engine test
# ============================================================================
add_executable(test_notebook_diff
    unit/test_notebook_diff.cpp
)
target_include_directories(test_notebook_diff PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_notebook_diff PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_notebook_diff COMMAND test_notebook_diff)

# ============================================================================
# V5 Phase 01: Canvas Types test
# ============================================================================
add_executable(test_canvas_types
    unit/test_canvas_types.cpp
)
target_include_directories(test_canvas_types PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_types PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_types COMMAND test_canvas_types)

# ============================================================================
# V5 Phase 01: Quadtree test
# ============================================================================
add_executable(test_quadtree
    unit/test_quadtree.cpp
)
target_include_directories(test_quadtree PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_quadtree PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_quadtree COMMAND test_quadtree)

# ============================================================================
# V5 Phase 01: Viewport Transform test
# ============================================================================
add_executable(test_viewport_transform
    unit/test_viewport_transform.cpp
)
target_include_directories(test_viewport_transform PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_viewport_transform PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_viewport_transform COMMAND test_viewport_transform)

# ============================================================================
# V5 Phase 02: Canvas Renderer/TileCache test
# ============================================================================
add_executable(test_canvas_renderer
    unit/test_canvas_renderer.cpp
)
target_include_directories(test_canvas_renderer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_renderer PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_renderer COMMAND test_canvas_renderer)

# ============================================================================
# V5 Phase 03: Canvas Input System test
# ============================================================================
add_executable(test_canvas_input
    unit/test_canvas_input.cpp
)
target_include_directories(test_canvas_input PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_input PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_input COMMAND test_canvas_input)

# ============================================================================
# V5 Phase 04: Object Selection & Transform test
# ============================================================================
add_executable(test_selection_manager
    unit/test_selection_manager.cpp
)
target_include_directories(test_selection_manager PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_selection_manager PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_selection_manager COMMAND test_selection_manager)

# ============================================================================
# V5 Phase 05: Board Model test
# ============================================================================
add_executable(test_board_model
    unit/test_board_model.cpp
)
target_include_directories(test_board_model PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_board_model PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_board_model COMMAND test_board_model)

# ============================================================================
# V5 Phase 05: Board Serializer test
# ============================================================================
add_executable(test_board_serializer
    unit/test_board_serializer.cpp
)
target_include_directories(test_board_serializer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_board_serializer PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_board_serializer COMMAND test_board_serializer)

# ============================================================================
# V5 Phase 05: Undo/Redo test
# ============================================================================
add_executable(test_undo_redo
    unit/test_undo_redo.cpp
)
target_include_directories(test_undo_redo PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_undo_redo PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_undo_redo COMMAND test_undo_redo)

# ============================================================================
# V5 Phase 06: Snap Engine test
# ============================================================================
add_executable(test_snap_engine
    unit/test_snap_engine.cpp
)
target_include_directories(test_snap_engine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_snap_engine PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_snap_engine COMMAND test_snap_engine)

# ============================================================================
# V5 Phase 07: Sticky Note test
# ============================================================================
add_executable(test_sticky_note
    unit/test_sticky_note.cpp
)
target_include_directories(test_sticky_note PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_sticky_note PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_sticky_note COMMAND test_sticky_note)

# ============================================================================
# V5 Phase 08: Text Box test
# ============================================================================
add_executable(test_text_box
    unit/test_text_box.cpp
)
target_include_directories(test_text_box PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_text_box PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_text_box COMMAND test_text_box)

# ============================================================================
# V5 Phase 09: Shapes test
# ============================================================================
add_executable(test_shapes
    unit/test_shapes.cpp
)
target_include_directories(test_shapes PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_shapes PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_shapes COMMAND test_shapes)

# ============================================================================
# V5 Phase 10: Connectors test
# ============================================================================
add_executable(test_connectors
    unit/test_connectors.cpp
)
target_include_directories(test_connectors PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_connectors PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_connectors COMMAND test_connectors)

# ============================================================================
# V5 Phase 11: Freehand Drawing test
# ============================================================================
add_executable(test_freehand_path
    unit/test_freehand_path.cpp
)
target_include_directories(test_freehand_path PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_freehand_path PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_freehand_path COMMAND test_freehand_path)

# ============================================================================
# V5 Phase 12: Shape Recognizer test
# ============================================================================
add_executable(test_shape_recognizer
    unit/test_shape_recognizer.cpp
)
target_include_directories(test_shape_recognizer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_shape_recognizer PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_shape_recognizer COMMAND test_shape_recognizer)

# ============================================================================
# V5 Phase 13: Image Object test
# ============================================================================
add_executable(test_image_object
    unit/test_image_object.cpp
)
target_include_directories(test_image_object PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_image_object PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_image_object COMMAND test_image_object)

# ============================================================================
# V5 Phase 14: Frame Object test
# ============================================================================
add_executable(test_frame_object
    unit/test_frame_object.cpp
)
target_include_directories(test_frame_object PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_frame_object PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_frame_object COMMAND test_frame_object)

# ============================================================================
# V5 Phase 15: Section Object test
# ============================================================================
add_executable(test_section_object
    unit/test_section_object.cpp
)
target_include_directories(test_section_object PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_section_object PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_section_object COMMAND test_section_object)

# --- V5 Phase 16: Object Grouping & Nesting ---
add_executable(test_group_object
    unit/test_group_object.cpp
)
target_include_directories(test_group_object PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_group_object PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_group_object COMMAND test_group_object)

# --- V5 Phase 17: Object Layering / Z-Index ---
add_executable(test_layering_service
    unit/test_layering_service.cpp
)
target_include_directories(test_layering_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_layering_service PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_layering_service COMMAND test_layering_service)

# --- V5 Phase 18: Object Locking ---
add_executable(test_locking_service
    unit/test_locking_service.cpp
)
target_include_directories(test_locking_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_locking_service PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_locking_service COMMAND test_locking_service)

# --- V5 Phase 19: Table Creation & Editing ---
add_executable(test_table_object
    unit/test_table_object.cpp
)
target_include_directories(test_table_object PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_table_object PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_table_object COMMAND test_table_object)

# --- V5 Phase 20: Table Sorting & Filtering ---
add_executable(test_table_sort_filter
    unit/test_table_sort_filter.cpp
)
target_include_directories(test_table_sort_filter PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_table_sort_filter PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_table_sort_filter COMMAND test_table_sort_filter)

# --- V5 Phase 21: Metadata Panel test ---
add_executable(test_metadata_panel
    unit/test_metadata_panel.cpp
)
target_include_directories(test_metadata_panel PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_metadata_panel PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_metadata_panel COMMAND test_metadata_panel)

# --- V5 Phase 22: Tag Manager test ---
add_executable(test_tag_manager
    unit/test_tag_manager.cpp
)
target_include_directories(test_tag_manager PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_tag_manager PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_tag_manager COMMAND test_tag_manager)

# --- V5 Phase 23: Minimap test ---
add_executable(test_minimap
    unit/test_minimap.cpp
)
target_include_directories(test_minimap PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_minimap PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_minimap COMMAND test_minimap)

# --- V5 Phase 24: Canvas Search test ---
add_executable(test_canvas_search
    unit/test_canvas_search.cpp
)
target_include_directories(test_canvas_search PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_search PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_search COMMAND test_canvas_search)

# V5 Phase 26: Cross-Board Linking tests
add_executable(test_cross_board_link unit/test_cross_board_link.cpp)
target_link_libraries(test_cross_board_link PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_cross_board_link COMMAND test_cross_board_link)

# V5 Phase 27: PDF Import tests
add_executable(test_pdf_import unit/test_pdf_import.cpp)
target_link_libraries(test_pdf_import PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_pdf_import COMMAND test_pdf_import)

# V5 Phase 28: Embedded Video tests
add_executable(test_video_embed unit/test_video_embed.cpp)
target_link_libraries(test_video_embed PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_video_embed COMMAND test_video_embed)

# V5 Phase 29: Bookmark Cards tests
add_executable(test_bookmark_card unit/test_bookmark_card.cpp)
target_link_libraries(test_bookmark_card PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_bookmark_card COMMAND test_bookmark_card)

# V5 Phase 30: Custom Icons & Emoji Packs tests
add_executable(test_icon_library unit/test_icon_library.cpp)
target_link_libraries(test_icon_library PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_icon_library COMMAND test_icon_library)

# V5 Phase 31: Layout Engine tests
add_executable(test_layout_engine unit/test_layout_engine.cpp)
target_link_libraries(test_layout_engine PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_layout_engine COMMAND test_layout_engine)

# V5 Phase 31: Sticky Aggregator tests
add_executable(test_sticky_aggregator unit/test_sticky_aggregator.cpp)
target_link_libraries(test_sticky_aggregator PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_sticky_aggregator COMMAND test_sticky_aggregator)

# V5 Phase 32: Mind Map tests
add_executable(test_mind_map unit/test_mind_map.cpp)
target_link_libraries(test_mind_map PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_mind_map COMMAND test_mind_map)

# V5 Phase 33: Kanban tests
add_executable(test_kanban unit/test_kanban.cpp)
target_link_libraries(test_kanban PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_kanban COMMAND test_kanban)

# V5 Phase 34: Diagram Shapes tests
add_executable(test_diagram_shapes unit/test_diagram_shapes.cpp)
target_link_libraries(test_diagram_shapes PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_diagram_shapes COMMAND test_diagram_shapes)

# V5 Phase 35: Board Templates tests
add_executable(test_board_templates unit/test_board_templates.cpp)
target_link_libraries(test_board_templates PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_board_templates COMMAND test_board_templates)

# ============================================================================
# V6 Phase 03: Startup Timer & Profiler Refactor
# ============================================================================
add_executable(test_startup_timer unit/test_startup_timer.cpp)
target_include_directories(test_startup_timer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_startup_timer PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_startup_timer COMMAND test_startup_timer)

# ============================================================================
# V6 Phase 05: Tracy Profiler Integration
# ============================================================================
add_executable(test_tracy_integration unit/test_tracy_integration.cpp)
target_include_directories(test_tracy_integration PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_tracy_integration PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_tracy_integration COMMAND test_tracy_integration)

# Phase 10: Allocator tests
add_executable(test_allocator unit/test_allocator.cpp)
target_include_directories(test_allocator PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_allocator PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_allocator COMMAND test_allocator)

# Phase 11: Constexpr table tests
add_executable(test_constexpr_tables unit/test_constexpr_tables.cpp)
target_include_directories(test_constexpr_tables PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_constexpr_tables PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_constexpr_tables COMMAND test_constexpr_tables)

# Phase 12: Lazy Service tests
add_executable(test_lazy_service unit/test_lazy_service.cpp)
target_include_directories(test_lazy_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_lazy_service PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_lazy_service COMMAND test_lazy_service)

# Phase 13: Deferred loading tests
add_executable(test_deferred_loading unit/test_deferred_loading.cpp)
target_include_directories(test_deferred_loading PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_deferred_loading PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_deferred_loading COMMAND test_deferred_loading)

# Phase 14: Arena expansion tests
add_executable(test_arena_expansion unit/test_arena_expansion.cpp)
target_include_directories(test_arena_expansion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_arena_expansion PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_arena_expansion COMMAND test_arena_expansion)

# Phase 16: Profiler v2 tests
add_executable(test_profiler_v2 unit/test_profiler_v2.cpp)
target_include_directories(test_profiler_v2 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_profiler_v2 PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_profiler_v2 COMMAND test_profiler_v2)

# Phase 17: ResolvedConfig strategy tests
add_executable(test_resolved_config unit/test_resolved_config.cpp)
target_include_directories(test_resolved_config PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_resolved_config PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_resolved_config COMMAND test_resolved_config)

# Phase 18: Incremental rendering pipeline tests
add_executable(test_incremental_rendering unit/test_incremental_rendering.cpp)
target_include_directories(test_incremental_rendering PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_incremental_rendering PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_incremental_rendering COMMAND test_incremental_rendering)

# Phase 19: Bounded containers and memory budget tests
add_executable(test_bounded_containers unit/test_bounded_containers.cpp)
target_include_directories(test_bounded_containers PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_bounded_containers PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_bounded_containers COMMAND test_bounded_containers)

add_executable(test_memory_budget unit/test_memory_budget.cpp)
target_include_directories(test_memory_budget PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_memory_budget PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_memory_budget COMMAND test_memory_budget)

# Phase 20: FlatEventBus tests
add_executable(test_eventbus_v2 unit/test_eventbus_v2.cpp)
target_include_directories(test_eventbus_v2 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_eventbus_v2 PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_eventbus_v2 COMMAND test_eventbus_v2)

# Phase 21: SnapshotStore tests
add_executable(test_lockfree_snapshot unit/test_lockfree_snapshot.cpp)
target_include_directories(test_lockfree_snapshot PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_lockfree_snapshot PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_lockfree_snapshot COMMAND test_lockfree_snapshot)

# Phase 22: ThreadBoundary tests
add_executable(test_thread_boundary unit/test_thread_boundary.cpp)
target_include_directories(test_thread_boundary PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_thread_boundary PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_thread_boundary COMMAND test_thread_boundary)

# Phase 23: SoA storage tests
add_executable(test_soa_storage unit/test_soa_storage.cpp)
target_include_directories(test_soa_storage PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_soa_storage PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_soa_storage COMMAND test_soa_storage)

# Phase 24: Plugin isolation tests
add_executable(test_plugin_isolation unit/test_plugin_isolation.cpp)
target_include_directories(test_plugin_isolation PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_plugin_isolation PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_plugin_isolation COMMAND test_plugin_isolation)

# Phase 25: UB edge case tests
add_executable(test_ub_edge_cases unit/test_ub_edge_cases.cpp)
target_include_directories(test_ub_edge_cases PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_ub_edge_cases PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_ub_edge_cases COMMAND test_ub_edge_cases)

# Phase 26: Constexpr map tests
add_executable(test_constexpr_map unit/test_constexpr_map.cpp)
target_include_directories(test_constexpr_map PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_constexpr_map PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_constexpr_map COMMAND test_constexpr_map)

# Phase 27: Frame budget enforcement tests
add_executable(test_frame_budget_enforcement unit/test_frame_budget_enforcement.cpp)
target_include_directories(test_frame_budget_enforcement PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_frame_budget_enforcement PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_frame_budget_enforcement COMMAND test_frame_budget_enforcement)

# Phase 28: Incremental tokenization tests
add_executable(test_incremental_tokenization unit/test_incremental_tokenization.cpp)
target_include_directories(test_incremental_tokenization PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_incremental_tokenization PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_incremental_tokenization COMMAND test_incremental_tokenization)

# Phase 29: Structured logging tests
add_executable(test_structured_logging unit/test_structured_logging.cpp)
target_include_directories(test_structured_logging PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_structured_logging PRIVATE
    Catch2::Catch2WithMain
    markamp_core
    spdlog::spdlog
)
add_test(NAME test_structured_logging COMMAND test_structured_logging)

# Phase 30: Crash reporter tests
add_executable(test_crash_reporter unit/test_crash_reporter.cpp)
target_include_directories(test_crash_reporter PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_crash_reporter PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_crash_reporter COMMAND test_crash_reporter)

# Phase 31: Watchdog tests
add_executable(test_watchdog unit/test_watchdog.cpp)
target_include_directories(test_watchdog PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_watchdog PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_watchdog COMMAND test_watchdog)

# Phase 32: Health panel tests
add_executable(test_health_panel unit/test_health_panel.cpp)
target_include_directories(test_health_panel PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_health_panel PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_health_panel COMMAND test_health_panel)

# Phase 33: Extension resource tracking tests
add_executable(test_extension_resource_tracking unit/test_extension_resource_tracking.cpp)
target_include_directories(test_extension_resource_tracking PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_extension_resource_tracking PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_extension_resource_tracking COMMAND test_extension_resource_tracking)

# Phase 35: Large file mode tests
add_executable(test_large_file_mode unit/test_large_file_mode.cpp)
target_include_directories(test_large_file_mode PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_large_file_mode PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_large_file_mode COMMAND test_large_file_mode)

# Phase 37: Chaos engine tests
add_executable(test_chaos_engine chaos/test_chaos_engine.cpp)
target_include_directories(test_chaos_engine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_chaos_engine PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_chaos_engine COMMAND test_chaos_engine)

# Phase 39: Load test harness tests
add_executable(test_load_harness load/test_load_harness.cpp)
target_include_directories(test_load_harness PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_load_harness PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_load_harness COMMAND test_load_harness)

# V8 Phase 5: SettingsCatalog tests
add_executable(test_settings_catalog unit/test_settings_catalog.cpp)
target_include_directories(test_settings_catalog PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_settings_catalog PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_settings_catalog COMMAND test_settings_catalog)

# V8 Phase 5: SettingsPanel staged-edit API compile tests
add_executable(test_settings_panel unit/test_settings_panel.cpp)
target_include_directories(test_settings_panel PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_settings_panel PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_settings_panel COMMAND test_settings_panel)

# V8 Phase 6: Canvas Workbench Shell compile tests
add_executable(test_canvas_workspace unit/test_canvas_workspace.cpp)
target_include_directories(test_canvas_workspace PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_workspace PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_workspace COMMAND test_canvas_workspace)

# V8 Phase 7: Realtime Collaboration tests
add_executable(test_canvas_collab unit/test_canvas_collab.cpp)
target_include_directories(test_canvas_collab PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_collab PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_collab COMMAND test_canvas_collab)

# V8 Phase 8: Canvas Apps Marketplace tests
add_executable(test_canvas_apps unit/test_canvas_apps.cpp)
target_include_directories(test_canvas_apps PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_apps PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_apps COMMAND test_canvas_apps)

# V8 Phase 10: Code Intelligence UX tests
add_executable(test_code_intelligence unit/test_code_intelligence.cpp)
target_include_directories(test_code_intelligence PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_code_intelligence PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_code_intelligence COMMAND test_code_intelligence)

# V8 Phase 11: Unified Workbench Navigation tests
add_executable(test_workbench_navigation unit/test_workbench_navigation.cpp)
target_include_directories(test_workbench_navigation PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_workbench_navigation PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_workbench_navigation COMMAND test_workbench_navigation)

# V8 Phase 12: Surface Link Router + Theme Infrastructure
add_executable(test_phase12_surface_theme unit/test_phase12_surface_theme.cpp)
target_include_directories(test_phase12_surface_theme PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase12_surface_theme PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase12_surface_theme COMMAND test_phase12_surface_theme)

# V8 Phase 13: FX Engine tests
add_executable(test_phase13_fx_engine unit/test_phase13_fx_engine.cpp)
target_include_directories(test_phase13_fx_engine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase13_fx_engine PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase13_fx_engine COMMAND test_phase13_fx_engine)
