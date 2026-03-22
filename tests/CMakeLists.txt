# Test sources live under tests/unit/
# Each test links Catch2 and, when needed, the markamp core sources.

# --- Core source library (shared by tests that test core components) ---
add_library(markamp_core STATIC
    # Core infrastructure
    ${CMAKE_SOURCE_DIR}/src/core/EventBus.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AppState.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Command.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Config.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ConfigAuditTrail.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ConfigFileWatcher.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ConfigMigration.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ConfigProfile.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ScopedConfig.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SettingsCatalog.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Logger.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Color.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Theme.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BuiltInThemes.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ThemeRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ThemeValidator.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ThemeEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TokenInheritanceResolver.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ScopedTokenMap.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ContrastRatioValidator.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ColorBlindnessSimulator.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ThemeCompatibility.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ThemeAutoRepair.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ThemeExporter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ThemePerformanceMonitor.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ThemeScopeMapper.cpp
    ${CMAKE_SOURCE_DIR}/src/core/VsCodeThemeAdapter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FileNode.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SampleFiles.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FileSystem.cpp
    ${CMAKE_SOURCE_DIR}/src/core/EncodingDetector.cpp
    ${CMAKE_SOURCE_DIR}/src/core/RecentFiles.cpp
    ${CMAKE_SOURCE_DIR}/src/core/MarkdownDocument.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Md4cWrapper.cpp
    ${CMAKE_SOURCE_DIR}/src/core/MarkdownParser.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SyntaxHighlighter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/LanguagePack.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SemanticTokenEngine.cpp
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
    ${CMAKE_SOURCE_DIR}/src/rendering/JsonRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/ScriptRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/CodeBlockRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/loader/ThemeLoader.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/MermaidBlockRenderer.cpp
    # Plugin & Extension infrastructure
    ${CMAKE_SOURCE_DIR}/src/core/PluginManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PluginHotReload.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ContributionValidator.cpp
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
    ${CMAKE_SOURCE_DIR}/src/ui/OutputPanelRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ProblemsPanel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/TreeViewHost.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/PanelAreaModel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/PanelTabBar.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/PanelContainer.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/EmptyPanelState.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/PanelHeader.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SidebarSection.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SidebarSkeletonPlaceholder.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/WebviewHostPanel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/WalkthroughPanel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/GutterDecorationProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/CodeLensProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/GitGutterProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/MergeConflictResolver.cpp
    # V10 Phase 01: Unified control state tracking
    ${CMAKE_SOURCE_DIR}/src/ui/ControlState.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/FocusManager.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/accessibility/AccessibilityController.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/accessibility/AccessibilityAuditor.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/accessibility/AccessibilityTokenMapper.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/accessibility/SkipToContentButton.cpp
)
if(APPLE)
    target_sources(markamp_core PRIVATE ${CMAKE_SOURCE_DIR}/src/platform/MacScreenReaderBridge.mm)
elseif(WIN32)
    target_sources(markamp_core PRIVATE ${CMAKE_SOURCE_DIR}/src/platform/WinScreenReaderBridge.cpp)
elseif(UNIX AND NOT APPLE)
    target_sources(markamp_core PRIVATE ${CMAKE_SOURCE_DIR}/src/platform/LinuxScreenReaderBridge.cpp)
endif()

target_sources(markamp_core PRIVATE
    ${CMAKE_SOURCE_DIR}/src/ui/LayoutMetrics.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ColorPaletteGenerator.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/animation/EasingFunctions.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/animation/ValueInterpolator.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/animation/SpringPhysics.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/animation/AnimationTimeline.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/animation/TransitionManager.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/animation/SkeletonLoader.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/animation/FocusRingAnimator.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/animation/ProgressBar.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ColorUtils.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ComponentSizeResolver.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/layout/WorkbenchLayoutModel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/layout/ResizeHandle.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/layout/WorkbenchShell.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/DesignTokenRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/DesignTokenValue.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ElevationSystem.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SpacingGrid.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/TypographyScale.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SvgDocument.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/IconRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/IconCache.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/IconRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/IconManager.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/IconLibrary.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/FileTypeIconRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/FileTypeIconResolver.cpp
    # V16 Phase 01-04: Icon Manifest & Pipeline
    ${CMAKE_SOURCE_DIR}/src/ui/IconManifest.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/IconPipeline.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ThemeAwareWindow.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/BreadcrumbBar.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/BreadcrumbDropdown.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/FocusRingRenderer.cpp
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
    # Phase 28: FX Visual Effects System
    ${CMAKE_SOURCE_DIR}/src/rendering/FxTransitionEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/FxCssGenerator.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/FxProfilePersistence.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/FxDiagnostics.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/FxCommandProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/FxAccessibility.cpp
    # Phase 29: Security & Input Validation
    ${CMAKE_SOURCE_DIR}/src/core/UrlSanitizer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ContentSecurityPolicy.cpp
    ${CMAKE_SOURCE_DIR}/src/core/InputRateLimiter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ClipboardSanitizer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SecurityAuditLog.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SecurityCommandProvider.cpp
    # V9 Phase 30: Performance Optimization
    ${CMAKE_SOURCE_DIR}/src/core/PerformanceProfiler.cpp
    ${CMAKE_SOURCE_DIR}/src/core/MemoryBudgetEnforcer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/IncrementalParsePolicy.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PerformanceCommandProvider.cpp
    # V9 Phase 31: Accessibility System
    ${CMAKE_SOURCE_DIR}/src/core/FocusRingStyler.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ColorBlindnessSupport.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AccessibilityAudit.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AccessibilityCommandProvider.cpp
    # V9 Phase 32: Platform-Specific Optimization
    ${CMAKE_SOURCE_DIR}/src/core/PlatformServices.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PlatformKeyboard.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PlatformFonts.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CrossPlatformPath.cpp
    # V9 Phase 33: Structured Logging & Observability
    ${CMAKE_SOURCE_DIR}/src/core/ObservabilityService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TracingService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ObservabilityCommands.cpp
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
    # Phase 25: Version Control Integration
    ${CMAKE_SOURCE_DIR}/src/core/GitCommandRunner.cpp
    ${CMAKE_SOURCE_DIR}/src/core/GitStashService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/GitBlameEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/GitRemoteService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/MergeConflictResolver.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CommitGraphEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/VCCommandProvider.cpp
    # Phase 26: AI Integration
    ${CMAKE_SOURCE_DIR}/src/core/AIProviderConfig.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AIRateLimiter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AIConversationHistory.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AIWritingAssistant.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AIDocumentGenerator.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AIInlineCompleter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AITagSuggester.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AILinkSuggester.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AIFlashcardGenerator.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AISearchEnhancer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AICommandProvider.cpp
    # Phase 27: Cloud Sync & Collaboration
    ${CMAKE_SOURCE_DIR}/src/core/SyncEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CloudSyncService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/S3Client.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WebDavClient.cpp
    ${CMAKE_SOURCE_DIR}/src/core/EncryptionService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SyncScheduler.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SyncConflictManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SelectiveSyncFilter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SyncHistoryLogger.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WorkspaceSharing.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SyncCommandProvider.cpp
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
    ${CMAKE_SOURCE_DIR}/src/canvas/SmartSpacingModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/PrecisionNudgeModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/DragAutoscrollModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/PenEngineModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/PenPresetModel.cpp
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
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasSearchModel.cpp
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
    # V10 Phase 71-75: Canvas Completeness
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasWorkbench.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasContextMenu.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasObjectFactory.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasPerformanceMonitor.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasAccessibility.cpp
    # V7 Phase 02: Centralized Input Validation
    ${CMAKE_SOURCE_DIR}/src/core/ValidationUtils.cpp
    # V7 Phase 07: String Length Guards
    ${CMAKE_SOURCE_DIR}/src/core/StringGuards.cpp
    # V7 Phase 08: Safe Filesystem Path
    ${CMAKE_SOURCE_DIR}/src/core/SafePath.cpp
    # V7 Phase 10: Safe Regex
    ${CMAKE_SOURCE_DIR}/src/core/SafeRegex.cpp
    # V7 Phase 13: MIME Validator
    ${CMAKE_SOURCE_DIR}/src/core/MimeValidator.cpp
    # V7 Phase 24: API Version
    ${CMAKE_SOURCE_DIR}/src/core/ApiVersion.cpp
    # V7 Phase 25: Plugin Quarantine
    ${CMAKE_SOURCE_DIR}/src/core/PluginQuarantine.cpp
    # V7 Phase 27: Rendering Pipeline Safety
    ${CMAKE_SOURCE_DIR}/src/rendering/RenderingSafety.cpp
    # V7 Phase 30: Error Reporting Service
    ${CMAKE_SOURCE_DIR}/src/core/ErrorReportingService.cpp
    # V7 Phase 31: Atomic Writer
    ${CMAKE_SOURCE_DIR}/src/core/AtomicWriter.cpp
    # V7 Phase 32: Write-Ahead Log
    ${CMAKE_SOURCE_DIR}/src/core/WriteAheadLog.cpp
    # V7 Phase 33: Telemetry Abstraction
    ${CMAKE_SOURCE_DIR}/src/core/TelemetryAbstraction.cpp
    # V7 Phase 35: OTLP Exporter
    ${CMAKE_SOURCE_DIR}/src/core/OtlpExporter.cpp
    # V7 Phase 36: Safe Mode
    ${CMAKE_SOURCE_DIR}/src/core/SafeMode.cpp
    # V7 Phase 37: Subsystem Reset
    ${CMAKE_SOURCE_DIR}/src/core/SubsystemReset.cpp
    # V7 Phase 38: Fault Domains
    ${CMAKE_SOURCE_DIR}/src/core/FaultDomain.cpp
    # V7 Phase 39: Enterprise Runtime Policy
    ${CMAKE_SOURCE_DIR}/src/core/RuntimePolicy.cpp
    # V7 Phase 40: Chaos Plugin
    ${CMAKE_SOURCE_DIR}/src/core/ChaosPlugin.cpp
    # V8 Phase 5: Settings Catalog
    ${CMAKE_SOURCE_DIR}/src/core/SettingsCatalog.cpp
    # V8 Phase 7: Realtime Collaboration
    ${CMAKE_SOURCE_DIR}/src/core/CanvasCollabService.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CommentObject.cpp
    # V8 Phase 8: Canvas Apps Marketplace
    ${CMAKE_SOURCE_DIR}/src/canvas/AppWidgetObject.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CanvasIntegrationService.cpp
    # Phase 24: FSRS Scheduling Algorithm
    ${CMAKE_SOURCE_DIR}/src/core/fsrs/FSRSTypes.cpp
    ${CMAKE_SOURCE_DIR}/src/core/fsrs/FSRSEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/fsrs/FlashcardStore.cpp
    ${CMAKE_SOURCE_DIR}/src/core/fsrs/FSRSReviewSession.cpp
    # Phase 25: Flashcard Deck Management
    ${CMAKE_SOURCE_DIR}/src/core/DeckStore.cpp
    ${CMAKE_SOURCE_DIR}/src/core/DeckManager.cpp
    # Phase 26: Review Session
    ${CMAKE_SOURCE_DIR}/src/core/ReviewSession.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CardFactory.cpp
    # V9 Phase 22: Flashcard & Spaced Repetition
    ${CMAKE_SOURCE_DIR}/src/core/fsrs/FlashcardExtractor.cpp
    ${CMAKE_SOURCE_DIR}/src/core/fsrs/ClozeParser.cpp
    ${CMAKE_SOURCE_DIR}/src/core/fsrs/FlashcardImportExport.cpp
    ${CMAKE_SOURCE_DIR}/src/core/fsrs/StudyStreakTracker.cpp
    ${CMAKE_SOURCE_DIR}/src/core/fsrs/LeechDetector.cpp
    ${CMAKE_SOURCE_DIR}/src/core/fsrs/FlashcardCommandProvider.cpp
    # Phase 23: Task Management & Calendar
    ${CMAKE_SOURCE_DIR}/src/core/TaskRecurrence.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TaskReminder.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TaskAggregator.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TaskBoardEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TaskGanttEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TaskCommandProvider.cpp
    # Phase 24: Export & Publishing
    ${CMAKE_SOURCE_DIR}/src/core/ExportService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExportTemplateEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BatchExportEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PrintPreparationService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PublishingProfileManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExportValidator.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExportCommandProvider.cpp
    # Phase 36: Command System Completion
    ${CMAKE_SOURCE_DIR}/src/core/CommandRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ChordShortcutManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CommandExecutionLog.cpp
    ${CMAKE_SOURCE_DIR}/src/core/KeybindingEditor.cpp
    ${CMAKE_SOURCE_DIR}/src/core/QuickOpenProvider.cpp
    # Phase 37: Bookmark & Asset Management
    ${CMAKE_SOURCE_DIR}/src/core/BookmarkNavigator.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BookmarkExporter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AssetOptimizer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AssetLinkIntegrity.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BookmarkCommandProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AssetCommandProvider.cpp
    # Phase 38: Vault Style & Workspace Customization
    ${CMAKE_SOURCE_DIR}/src/core/CssSnippetLibrary.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WorkspaceProfile.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CustomFontRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ThemeOverrideService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/VaultStyleCommandProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WorkspaceCustomizationCommandProvider.cpp
    # Phase 39: Notification & Activity System
    ${CMAKE_SOURCE_DIR}/src/core/NotificationCenter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/NotificationFilter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ActivityFeed.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ActivityTimeline.cpp
    ${CMAKE_SOURCE_DIR}/src/core/NotificationCommandProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ActivityCommandProvider.cpp
    # Phase 40: Workspace Automation & Hooks
    ${CMAKE_SOURCE_DIR}/src/core/WorkspaceHookManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AutomationRule.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TaskScheduler.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FileWatcherService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AutomationCommandProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/HooksCommandProvider.cpp
    # Phase 41: Widget & Sidebar Extensions
    ${CMAKE_SOURCE_DIR}/src/core/SidebarWidgetManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WidgetContainer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WidgetDataProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CustomPanelRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WidgetCommandProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SidebarCommandProvider.cpp
    # Phase 42: PDF Viewer & Document Import
    ${CMAKE_SOURCE_DIR}/src/core/PdfViewerService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/DocumentImporter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PdfAnnotationManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ImportPresetRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PdfCommandProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ImportCommandProvider.cpp
    # Phase 43: Presentation & Slide System
    ${CMAKE_SOURCE_DIR}/src/core/SlideEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PresentationManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SlideThemeProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SlideExporter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PresentationCommandProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SlideCommandProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/VaultEncryptionManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/KeyManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PrivacyManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/DataRedactionEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/EncryptionCommandProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PrivacyCommandProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WindowManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/EditorGroupManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WorkspaceLayoutEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WorkspaceSessionRestore.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WindowCommandProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/LayoutCommandProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/LocaleManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TranslationCatalog.cpp
    ${CMAKE_SOURCE_DIR}/src/core/StringFormatter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TextDirectionEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/LocalizationCommandProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/InternationalizationCommandProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/DataTableEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FormulaEvaluator.cpp
    ${CMAKE_SOURCE_DIR}/src/core/DataTransformPipeline.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ChartDataProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/DataCommandProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ComputationCommandProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/IntegrationTestRunner.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CrossModuleTestSuite.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SystemHealthValidator.cpp
    ${CMAKE_SOURCE_DIR}/src/core/RegressionTestEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/IntegrationTestCommandProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ValidationCommandProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ReleaseNotesGenerator.cpp
    ${CMAKE_SOURCE_DIR}/src/core/VersionManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ChangelogEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/DeprecationTracker.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ReleaseCommandProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PolishCommandProvider.cpp
    # V11 Node Editor core
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeGraph.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeDefinitionRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/GraphSerializer.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeCommand.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeCommandStack.cpp
    # V11 Node Editor Tier 2
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeEditorContext.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/EvaluationIR.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/ExecutionPlan.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/DomainRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeScheduler.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeValue.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/ValueConverter.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/GraphicsRuntime.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/CodeFlowRuntime.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/DataTransformRuntime.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/DataGenRuntime.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/AudioRuntime.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/DomainSDK.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/RuntimeSandbox.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/SelectionModel.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeLayout.cpp
    # Tier 3 — Node Editor UI (Phases 21-30)
    ${CMAKE_SOURCE_DIR}/src/node_editor/HitTester.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeEditorWorkbench.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeGroup.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/CanvasViewport.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/LinkRouter.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeAnnotation.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeClipboard.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeSearchProvider.cpp
    # Tier 4 — Runtime Infrastructure (Phases 31-40)
    ${CMAKE_SOURCE_DIR}/src/node_editor/InspectorModel.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/PreviewSystem.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/DebugOverlay.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/DirtyPropagator.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/ViewportCuller.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/AsyncEvaluator.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeDiagnostics.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeShortcuts.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/WorkspacePersistence.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeTestHarness.cpp
    # Tier 5 — Extensibility & Rollout (Phases 41-50)
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodePackRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeWidgetEmbed.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeDecorationProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeStatusModel.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeTemplateLibrary.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeFormatBridge.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeTrustPolicy.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeDevTools.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeMigrationEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/node_editor/NodeRolloutController.cpp
    # Additional core and canvas sources
    ${CMAKE_SOURCE_DIR}/src/canvas/AccessibilityModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/AuditModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/AutoLayoutModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/BenchmarkModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/BoardLockController.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/BoardSerializerModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/BoardSettingsModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CameraModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasClipboardService.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasColorModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasAnalyticsModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasExportService.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasInputModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasInspector.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasObjectAligner.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasObjectLifecycleEvents.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasStylePresetService.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasTemplateEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasTextModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasThemeModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasToolHost.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/ClipboardModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CollabEventLogger.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CollabUndoRedoManager.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/ConnectorModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/ConnectorRoutingService.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/ContextMenuModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CustomObjectTypeRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasToolStateMachine.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/DiagramCoordinator.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/ExportModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/FollowModeController.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/FrameContainerModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/FreehandModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/HitTestModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/ImageAssetModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/InspectorModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/IntegrityModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/KanbanModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/KeyboardCommandModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/LargeBoardModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/LayeringModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/MediaEmbedModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/MindMapModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/MinimapModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/OfflineSyncQueue.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/OnboardingModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/PdfObjectModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/PolishModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/PrivateRevealController.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/RemoteCursorOverlay.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/RemoteSelectionOverlay.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/SelectionTransformModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/ShapeModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/SnappingModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/StencilModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/StickyNoteModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/StyleModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/TableModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/ToolRailModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/UndoRedoModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/WidgetLifecycleManager.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/WidgetMessageBus.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/WidgetPermissionGuard.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/WidgetRenderer.cpp
    # Additional core sources
    ${CMAKE_SOURCE_DIR}/src/core/AIPanel.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AIService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AssetService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AttributeService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Backlink.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BacklinkService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BlockDiffEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BlockRef.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BlockService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BlockTypeDetector.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Bookmark.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BookmarkService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CellDependencyTracker.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CodeActionProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CompletionProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ContentIndexer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/DiagramRendererRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/DiffEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/EChartsRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExportDialog.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FileTemplateEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FileTreeModel.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FileWatcher.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FlowchartRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ForceDirectedLayout.cpp
    ${CMAKE_SOURCE_DIR}/src/core/GoToDefinitionProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/GraphService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/GraphvizRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/HistoryService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/HoverProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/core/HtmlExporter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/HtmlImporter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/IALParser.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ImportDialog.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ImportService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/MarkdownExporter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/MarkdownImporter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/MathRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/MindmapRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/MultiRootWorkspace.cpp
    ${CMAKE_SOURCE_DIR}/src/core/NotebookExecutionPipeline.cpp
    ${CMAKE_SOURCE_DIR}/src/core/NotebookSearchIndex.cpp
    ${CMAKE_SOURCE_DIR}/src/core/NotebookSerializer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/NotebookSessionManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/NotebookToolbar.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Outline.cpp
    ${CMAKE_SOURCE_DIR}/src/core/OutlineService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PDFAnnotationStore.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PandocExporter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PandocImporter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PanicPolicy.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PdfExporter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PlantUMLRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/QueryService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/RecentWorkspaces.cpp
    ${CMAKE_SOURCE_DIR}/src/core/RefParser.cpp
    ${CMAKE_SOURCE_DIR}/src/core/RefResolver.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ReferenceScanner.cpp
    ${CMAKE_SOURCE_DIR}/src/core/RepositoryService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Search.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SearchService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SqlQuery.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Tag.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TemplateEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TemplateFunctions.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TrashManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WorkspaceSettings.cpp
    # AV (Attribute View) sources
    ${CMAKE_SOURCE_DIR}/src/core/av/AVAggregateRow.cpp
    ${CMAKE_SOURCE_DIR}/src/core/av/AVColumnType.cpp
    ${CMAKE_SOURCE_DIR}/src/core/av/AVFilter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/av/AVFormatters.cpp
    ${CMAKE_SOURCE_DIR}/src/core/av/AVFormulaEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/av/AVImportExport.cpp
    ${CMAKE_SOURCE_DIR}/src/core/av/AVLinkedDatabase.cpp
    ${CMAKE_SOURCE_DIR}/src/core/av/AVQueryEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/av/AVRelationService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/av/AVRollupEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/av/AVSort.cpp
    ${CMAKE_SOURCE_DIR}/src/core/av/AVTimelineView.cpp
    ${CMAKE_SOURCE_DIR}/src/core/av/AVTypes.cpp
    ${CMAKE_SOURCE_DIR}/src/core/av/AVUndoManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/av/AVValidators.cpp
    ${CMAKE_SOURCE_DIR}/src/core/av/AVValue.cpp
    ${CMAKE_SOURCE_DIR}/src/core/av/AttributeView.cpp
    ${CMAKE_SOURCE_DIR}/src/core/av/AttributeViewStore.cpp
    # Phase 21: Integrated Terminal
    ${CMAKE_SOURCE_DIR}/src/core/AnsiParser.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TerminalBuffer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PtyProcess.cpp
    # Phase 22-25: Output Panel V2, Problems, Debug Console, Build & Tasks
    ${CMAKE_SOURCE_DIR}/src/core/DebugConsoleService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExpressionEvaluator.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CompilerErrorParser.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TaskConfig.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BuildService.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ProblemsTreeModel.cpp
    # Phase 30: Floating Toolbars
    ${CMAKE_SOURCE_DIR}/src/ui/FloatingToolbar.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/FloatingToolbarManager.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/DebugToolbar.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SelectionActionBar.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/MinimapHoverToolbar.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ImagePreviewToolbar.cpp
    # Phase 31: Command Palette V2
    ${CMAKE_SOURCE_DIR}/src/core/FuzzyScorer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FrecencyTracker.cpp
    ${CMAKE_SOURCE_DIR}/src/core/MarkdownSymbolProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/IconProvider.cpp
    # Phase 32: Go-To System
    ${CMAKE_SOURCE_DIR}/src/core/NavigationService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/LinkResolver.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BacklinkFinder.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ChangeTracker.cpp
    # Phase 33: Outline Panel
    # (OutlinePanel.cpp excluded - wx dependency)
    # Phase 34: Find & Replace V2
    ${CMAKE_SOURCE_DIR}/src/core/ReplaceUndoBuffer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PreserveCaseReplacer.cpp
    # (FindBar.cpp, SearchResultsPanel.cpp excluded - wx dependency)
    # Phase 35: Quick Open & File Switching
    ${CMAKE_SOURCE_DIR}/src/core/RecentFilesManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FileSearchScorer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FileIndex.cpp
    # (TabSwitcherOverlay.cpp excluded - wx dependency)
    # Phase 36: Settings UI Overhaul
    ${CMAKE_SOURCE_DIR}/src/ui/RichDescriptionRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SettingsCompareView.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/CommandPaletteModel.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WorkspaceSymbolIndex.cpp
    # Phase 37: Compiler Configuration Panel
    ${CMAKE_SOURCE_DIR}/src/core/CompilerConfig.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CompilerDetector.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CompilerConfigValidator.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CompilerProfileManager.cpp
    # Phase 38: Build System Integration
    ${CMAKE_SOURCE_DIR}/src/core/CMakeIntegration.cpp
    ${CMAKE_SOURCE_DIR}/src/core/MakeIntegration.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BuildOutputParser.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BuildTaskManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BuildDependencyGraph.cpp
    ${CMAKE_SOURCE_DIR}/src/core/IncrementalBuildTracker.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BuildLogManager.cpp
    # Phase 40: Workspace & Project Configuration
    ${CMAKE_SOURCE_DIR}/src/core/WorkspaceConfig.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WorkspaceTrustService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/RecentWorkspacesManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WorkspaceEnvironment.cpp
    # Phase 46: Diff Editor and Merge Tool
    ${CMAKE_SOURCE_DIR}/src/core/WordDiffEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/MergeEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/DiffGutter.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SideBySideDiffView.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/InlineDiffView.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/MergeEditor.cpp
    # Phase 47: Peek View System
    ${CMAKE_SOURCE_DIR}/src/core/PeekProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/PeekView.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/PeekResultList.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/PeekEditorHost.cpp
    # Phase 48: Welcome and Onboarding
    ${CMAKE_SOURCE_DIR}/src/ui/WelcomeTab.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/RecentItemsList.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/InteractiveTutorial.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ChangelogPanel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/TipOfTheDay.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SpotlightOverlay.cpp
    # Phase 49: Performance Dashboard
    ${CMAKE_SOURCE_DIR}/src/core/PerformanceCounters.cpp
    # Phase 50: UI Integration Testing Harness
    ${CMAKE_SOURCE_DIR}/src/testing/VisualRegressionRunner.cpp
    ${CMAKE_SOURCE_DIR}/src/testing/UIAutomation.cpp
    ${CMAKE_SOURCE_DIR}/src/testing/AccessibilityAuditor.cpp
    ${CMAKE_SOURCE_DIR}/src/testing/ThemeCoverageChecker.cpp
    ${CMAKE_SOURCE_DIR}/src/testing/MemoryLeakDetector.cpp
    # V19 Phase 01: Shell orchestration services
    ${CMAKE_SOURCE_DIR}/src/core/WorkbenchShellController.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WorkspaceOpenOrchestrator.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WorkspaceLoadStateModel.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ShellLayoutState.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CanvasWorkbenchMode.cpp
    # V20 Phase 01: Artifact Creation Spine
    ${CMAKE_SOURCE_DIR}/src/core/ArtifactRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ArtifactCreationService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ArtifactMountAdapter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ArtifactNamingPolicy.cpp
    # V20 Phase 02: File Creation & Text Artifact Lifecycle
    ${CMAKE_SOURCE_DIR}/src/core/UnsavedDocumentBuffer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TextArtifactLifecycle.cpp
    ${CMAKE_SOURCE_DIR}/src/core/EditorMountContext.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FileEntryPointResolver.cpp
    # V20 Phase 03: Notebook Creation & Computational Document Flows
    ${CMAKE_SOURCE_DIR}/src/core/NotebookArtifactLifecycle.cpp
    ${CMAKE_SOURCE_DIR}/src/core/NotebookStorageResolver.cpp
    ${CMAKE_SOURCE_DIR}/src/core/NotebookCommandRouter.cpp
    # V20 Phase 04: Canvas Creation & Board Workbench Integration
    ${CMAKE_SOURCE_DIR}/src/core/CanvasArtifactLifecycle.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CanvasEventBridge.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CanvasShellIntegration.cpp
    # V20 Phase 05: Persistence, Autosave, Restore & Recovery
    ${CMAKE_SOURCE_DIR}/src/core/PersistenceOutcome.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AtomicWriteService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SessionRestoreService.cpp
    # V20 Phase 06: Commands, Context, Focus & Surface Entry Points
    ${CMAKE_SOURCE_DIR}/src/core/CommandModel.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ActiveContextService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/EntryPointNormalizer.cpp
    # V20 Phase 07: Syntax Highlighting & Language Presentation
    ${CMAKE_SOURCE_DIR}/src/core/LanguageService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TokenThemeMapper.cpp
    ${CMAKE_SOURCE_DIR}/src/core/HighlightDegradation.cpp
    # V20 Phase 08: Editor, Notebook, Canvas Rendering & Content Styling
    ${CMAKE_SOURCE_DIR}/src/core/ContentStyleSystem.cpp
    ${CMAKE_SOURCE_DIR}/src/core/NotebookCellChrome.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CanvasContentPresenter.cpp
    # V20 Phase 09: Premium Visual System Polish & MUI Icon Completion
    ${CMAKE_SOURCE_DIR}/src/core/ShellVisualSystem.cpp
    ${CMAKE_SOURCE_DIR}/src/core/IconSemanticMapper.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AccessibilityAuditGate.cpp
    # V20 Phase 10: Diagnostics, Validation Harnesses & Consolidation
    ${CMAKE_SOURCE_DIR}/src/core/LifecycleTracer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WorkflowSmokeRunner.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExecutionScoreboard.cpp
    # V19 Phase 02: Command dispatch and prompt host
    ${CMAKE_SOURCE_DIR}/src/core/CommandFeedback.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CommandDispatchAdapter.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/PromptHost.cpp
    # V19 Phase 03: Dirty state coordination
    ${CMAKE_SOURCE_DIR}/src/core/DirtyStateCoordinator.cpp
    # V21 Phase 01: Control Audit Spine & Canonical Action Model
    ${CMAKE_SOURCE_DIR}/src/core/ControlActionManifest.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ControlExecutionTracer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ControlAuditReport.cpp
    # V21 Phase 02: Menu / Palette / Shortcut Parity
    ${CMAKE_SOURCE_DIR}/src/core/MenuCommandBinder.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PaletteManifestSync.cpp
    # V21 Phase 03: Toolbar, Status Bar & Global Button Wiring
    ${CMAKE_SOURCE_DIR}/src/core/ToolbarCommandBinder.cpp
    ${CMAKE_SOURCE_DIR}/src/core/StatusBarActionRouter.cpp
    # V21 Phase 04: Context Menus, Inline Actions & Target-Aware Enablement
    ${CMAKE_SOURCE_DIR}/src/core/ContextMenuActionBinder.cpp
    ${CMAKE_SOURCE_DIR}/src/core/InlineActionRouter.cpp
    # V21 Phase 05: Panel Registry, Primary Sidebar & Controlled Surface Replacement
    ${CMAKE_SOURCE_DIR}/src/core/PanelLifecycleAuditor.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SidebarStateContract.cpp
    # V21 Phase 06: Secondary, Bottom & Specialized Panel Lifecycle
    ${CMAKE_SOURCE_DIR}/src/core/PanelCapabilityModel.cpp
    # V21 Phase 07: Workbench Surface Completion
    ${CMAKE_SOURCE_DIR}/src/core/SurfaceActionAuditor.cpp
    # V21 Phase 08: Settings Architecture & Schema Consolidation
    ${CMAKE_SOURCE_DIR}/src/core/SettingsArchitectureAuditor.cpp
    # V21 Phase 09: Settings UI & JSON Editor
    ${CMAKE_SOURCE_DIR}/src/core/SettingsStagedContract.cpp
    # V21 Phase 10: Validation Harnesses & Dead UI Removal
    ${CMAKE_SOURCE_DIR}/src/core/ControlCompletenessMatrix.cpp
    # V23 Phase 01: Completion Inventory & Severity Policy
    ${CMAKE_SOURCE_DIR}/src/core/CompletionInventory.cpp
    ${CMAKE_SOURCE_DIR}/src/core/CompletionSeverityPolicy.cpp
    # V23 Phase 02: Action & Panel Readiness Gate
    ${CMAKE_SOURCE_DIR}/src/core/ActionReadinessGate.cpp
    # V23 Phase 03: Lifecycle Completion Tracker
    ${CMAKE_SOURCE_DIR}/src/core/LifecycleCompletionTracker.cpp
    # V23 Phase 04: Persistence Completion Auditor
    ${CMAKE_SOURCE_DIR}/src/core/PersistenceCompletionAuditor.cpp
    # V23 Phase 05: Editor Feature Completion Auditor
    ${CMAKE_SOURCE_DIR}/src/core/EditorFeatureCompletionAuditor.cpp
    # V23 Phase 06: Notebook Execution Completion Auditor
    ${CMAKE_SOURCE_DIR}/src/core/NotebookExecutionCompletionAuditor.cpp
    # V23 Phase 07: Canvas Completion Auditor
    ${CMAKE_SOURCE_DIR}/src/core/CanvasCompletionAuditor.cpp
    # V23 Phase 08: Specialized Surface Completion Auditor
    ${CMAKE_SOURCE_DIR}/src/core/SpecializedSurfaceCompletionAuditor.cpp
    # V23 Phase 09: Project Workflow Completion Auditor
    ${CMAKE_SOURCE_DIR}/src/core/ProjectWorkflowCompletionAuditor.cpp
    # V23 Phase 10: Settings Completion Auditor
    ${CMAKE_SOURCE_DIR}/src/core/SettingsCompletionAuditor.cpp
    # V23 Phase 11: Plugin Marketplace Completion Auditor
    ${CMAKE_SOURCE_DIR}/src/core/PluginMarketplaceCompletionAuditor.cpp
    # V23 Phase 12: Cloud Security Completion Auditor
    ${CMAKE_SOURCE_DIR}/src/core/CloudSecurityCompletionAuditor.cpp
    # V23 Phase 13: Rendering Output Completion Auditor
    ${CMAKE_SOURCE_DIR}/src/core/RenderingOutputCompletionAuditor.cpp
    # V23 Phase 14: Node Editor Completion Auditor
    ${CMAKE_SOURCE_DIR}/src/core/NodeEditorCompletionAuditor.cpp
    # V23 Phase 15: Structured Data Completion Auditor
    ${CMAKE_SOURCE_DIR}/src/core/StructuredDataCompletionAuditor.cpp
    # V23 Phase 16: Platform Accessibility Completion Auditor
    ${CMAKE_SOURCE_DIR}/src/core/PlatformAccessibilityCompletionAuditor.cpp
    # V23 Phase 17: Service Stub Completion Auditor
    ${CMAKE_SOURCE_DIR}/src/core/ServiceStubCompletionAuditor.cpp
    # V23 Phase 18: Feature Flag Migration Completion Auditor
    ${CMAKE_SOURCE_DIR}/src/core/FeatureFlagMigrationCompletionAuditor.cpp
    # V23 Phase 19: Testing Regression Completion Auditor
    ${CMAKE_SOURCE_DIR}/src/core/TestingRegressionCompletionAuditor.cpp
    # V23 Phase 20: Release Gate Completion Auditor
    ${CMAKE_SOURCE_DIR}/src/core/ReleaseGateCompletionAuditor.cpp
    # V22 Phase 01: Visual Language & Design System Foundation
    ${CMAKE_SOURCE_DIR}/src/core/DesignSystemAudit.cpp
    # V22 Phase 02: Shell Chrome Coordination
    ${CMAKE_SOURCE_DIR}/src/core/ShellChromeCoordinator.cpp
    # V22 Phase 03: Panel Chrome Coordination
    ${CMAKE_SOURCE_DIR}/src/core/PanelChromeCoordinator.cpp
    # V22 Phase 04: Command Surface Coordination
    ${CMAKE_SOURCE_DIR}/src/core/CommandSurfaceCoordinator.cpp
    # V22 Phase 05: Navigation Chrome Coordination
    ${CMAKE_SOURCE_DIR}/src/core/NavigationChromeCoordinator.cpp
    # V22 Phase 06: Editor Chrome Coordination
    ${CMAKE_SOURCE_DIR}/src/core/EditorChromeCoordinator.cpp
    # V22 Phase 07: Syntax Presentation Coordination
    ${CMAKE_SOURCE_DIR}/src/core/SyntaxPresentationCoordinator.cpp
    # V22 Phase 08: Editor Overlay Coordination
    ${CMAKE_SOURCE_DIR}/src/core/EditorOverlayCoordinator.cpp
    # V22 Phase 09: Notebook Chrome Coordination
    ${CMAKE_SOURCE_DIR}/src/core/NotebookChromeCoordinator.cpp
    # V22 Phase 10: Canvas Chrome Coordination
    ${CMAKE_SOURCE_DIR}/src/core/CanvasChromeCoordinator.cpp
    # V22 Phase 11: Sidebar Panel Coordination
    ${CMAKE_SOURCE_DIR}/src/core/SidebarPanelCoordinator.cpp
    # V22 Phase 12: Utility Panel Coordination
    ${CMAKE_SOURCE_DIR}/src/core/UtilityPanelCoordinator.cpp
    # V22 Phase 13: Settings Surface Coordination
    ${CMAKE_SOURCE_DIR}/src/core/SettingsSurfaceCoordinator.cpp
    # V22 Phase 14: Transient Overlay Coordination
    ${CMAKE_SOURCE_DIR}/src/core/TransientOverlayCoordinator.cpp
    # V22 Phase 15: Dense Data Coordination
    ${CMAKE_SOURCE_DIR}/src/core/DenseDataCoordinator.cpp
    # V22 Phase 16: Onboarding State Coordination
    ${CMAKE_SOURCE_DIR}/src/core/OnboardingStateCoordinator.cpp
    # V22 Phase 17: Accessibility Coordination
    ${CMAKE_SOURCE_DIR}/src/core/AccessibilityCoordinator.cpp
    # V22 Phase 18: Theme Parity Coordination
    ${CMAKE_SOURCE_DIR}/src/core/ThemeParityCoordinator.cpp
    # V22 Phase 19: Motion Language Coordination
    ${CMAKE_SOURCE_DIR}/src/core/MotionLanguageCoordinator.cpp
    # V22 Phase 20: Visual Cleanup Coordination
    ${CMAKE_SOURCE_DIR}/src/core/VisualCleanupCoordinator.cpp
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

# --- Design System test ---
add_executable(test_design_system
    unit/test_design_system.cpp
)
target_include_directories(test_design_system PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_design_system PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_design_system COMMAND test_design_system)


# --- Editor QoL test ---
add_executable(test_editor_qol
    unit/test_editor_qol.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/EditorPanel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/MinimapPanel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/OverviewRulerPanel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ThemeAwareWindow.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/FloatingFormatBar.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/LinkPreviewPopover.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/TableEditorOverlay.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ImagePreviewPopover.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/GitBlameGutterProvider.cpp
)
if(APPLE)
    target_sources(test_editor_qol PRIVATE ${CMAKE_SOURCE_DIR}/src/platform/MacAccessibilityIdentifier.mm)
else()
    target_sources(test_editor_qol PRIVATE ${CMAKE_SOURCE_DIR}/src/platform/StubAccessibilityIdentifier.cpp)
endif()
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
add_test(NAME test_perf_patterns_21_40 COMMAND test_perf_patterns_21_40)
target_include_directories(test_perf_patterns_21_40 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_perf_patterns_21_40 PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)

# --- JSON Renderer test ---
add_executable(test_json_renderer
    unit/test_json_renderer.cpp
)
target_include_directories(test_json_renderer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_json_renderer PRIVATE
    Catch2::Catch2WithMain
    markamp_core
    nlohmann_json::nlohmann_json
)
add_test(NAME test_json_renderer COMMAND test_json_renderer)

# --- Script Renderer test ---
add_executable(test_script_renderer
    unit/test_script_renderer.cpp
)
target_include_directories(test_script_renderer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_script_renderer PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_script_renderer COMMAND test_script_renderer)

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

# --- Phase 11 Tab Bar Overhaul test ---
add_executable(test_phase11_tab_bar unit/test_phase11_tab_bar.cpp)
target_include_directories(test_phase11_tab_bar PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase11_tab_bar PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase11_tab_bar COMMAND test_phase11_tab_bar)

# --- Phase 10 Panel Area test ---
add_executable(test_phase10_panel_area unit/test_phase10_panel_area.cpp)
target_include_directories(test_phase10_panel_area PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase10_panel_area PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase10_panel_area COMMAND test_phase10_panel_area)

# --- Phase 09 Secondary Sidebar test ---
add_executable(test_phase09_secondary_sidebar
    test_phase09_secondary_sidebar.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SidebarPanelRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SecondarySidebarTabStrip.cpp
)
target_include_directories(test_phase09_secondary_sidebar PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase09_secondary_sidebar PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase09_secondary_sidebar COMMAND test_phase09_secondary_sidebar)

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

add_executable(test_git_command_runner
    unit/test_git_command_runner.cpp
)
target_include_directories(test_git_command_runner PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_git_command_runner PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_git_command_runner COMMAND test_git_command_runner)

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
add_executable(test_workbench_navigation
    unit/test_workbench_navigation.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SidebarPanelRegistry.cpp
)
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

# Phase 28: FX Visual Effects System tests
add_executable(test_phase28_fx_system unit/test_phase28_fx_system.cpp)
target_include_directories(test_phase28_fx_system PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase28_fx_system PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase28_fx_system COMMAND test_phase28_fx_system)

# Phase 29: Security & Input Validation tests
add_executable(test_phase29_security unit/test_phase29_security.cpp)
target_include_directories(test_phase29_security PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase29_security PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase29_security COMMAND test_phase29_security)

# Phase 30: Performance Optimization tests
add_executable(test_phase30_performance unit/test_phase30_performance.cpp)
target_include_directories(test_phase30_performance PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase30_performance PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase30_performance COMMAND test_phase30_performance)

# Phase 31: Accessibility System tests
add_executable(test_phase31_accessibility unit/test_phase31_accessibility.cpp)
target_include_directories(test_phase31_accessibility PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase31_accessibility PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase31_accessibility COMMAND test_phase31_accessibility)

# Phase 32: Platform-Specific Optimization tests
add_executable(test_phase32_platform unit/test_phase32_platform.cpp)
target_include_directories(test_phase32_platform PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase32_platform PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase32_platform COMMAND test_phase32_platform)

# Phase 33: Structured Logging & Observability tests
add_executable(test_phase33_observability unit/test_phase33_observability.cpp)
target_include_directories(test_phase33_observability PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase33_observability PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase33_observability COMMAND test_phase33_observability)

add_executable(test_phase8_rendering unit/test_phase8_rendering.cpp)
target_include_directories(test_phase8_rendering PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase8_rendering PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase8_rendering COMMAND test_phase8_rendering)

# ============================================================================
# V7 Phase 01: Result Types & Error Taxonomy
# ============================================================================
add_executable(test_result_types unit/test_result_types.cpp)
target_include_directories(test_result_types PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_result_types PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_result_types COMMAND test_result_types)

# ============================================================================
# V7 Phase 02: Centralized Input Validation
# ============================================================================
add_executable(test_validation_utils unit/test_validation_utils.cpp)
target_include_directories(test_validation_utils PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_validation_utils PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_validation_utils COMMAND test_validation_utils)

# ============================================================================
# V7 Phase 03: Structured Logger
# ============================================================================
add_executable(test_structured_logger unit/test_structured_logger.cpp)
target_include_directories(test_structured_logger PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_structured_logger PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_structured_logger COMMAND test_structured_logger)

# ============================================================================
# V7 Phase 07: String Length Guards
# ============================================================================
add_executable(test_string_guards unit/test_string_guards.cpp)
target_include_directories(test_string_guards PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_string_guards PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_string_guards COMMAND test_string_guards)

# ============================================================================
# V7 Phase 08: Safe Filesystem Path
# ============================================================================
add_executable(test_safe_path unit/test_safe_path.cpp)
target_include_directories(test_safe_path PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_safe_path PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_safe_path COMMAND test_safe_path)

# ============================================================================
# V7 Phase 10: Safe Regex
# ============================================================================
add_executable(test_safe_regex unit/test_safe_regex.cpp)
target_include_directories(test_safe_regex PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_safe_regex PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_safe_regex COMMAND test_safe_regex)

# ============================================================================
# V7 Phase 11: Numeric Guards
# ============================================================================
add_executable(test_numeric_guards unit/test_numeric_guards.cpp)
target_include_directories(test_numeric_guards PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_numeric_guards PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_numeric_guards COMMAND test_numeric_guards)

# ============================================================================
# V7 Phase 13: MIME Validator
# ============================================================================
add_executable(test_mime_validator unit/test_mime_validator.cpp)
target_include_directories(test_mime_validator PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_mime_validator PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_mime_validator COMMAND test_mime_validator)

# ============================================================================
# V7 Phase 16: Safe Allocation
# ============================================================================
add_executable(test_safe_allocation unit/test_safe_allocation.cpp)
target_include_directories(test_safe_allocation PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_safe_allocation PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_safe_allocation COMMAND test_safe_allocation)

# ============================================================================
# V7 Phase 17: Cancellation Tokens
# ============================================================================
add_executable(test_cancellation_token unit/test_cancellation_token.cpp)
target_include_directories(test_cancellation_token PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_cancellation_token PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_cancellation_token COMMAND test_cancellation_token)

# ============================================================================
# V7 Phase 19: Backpressure Queue & Rate Limiter
# ============================================================================
add_executable(test_backpressure unit/test_backpressure.cpp)
target_include_directories(test_backpressure PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_backpressure PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_backpressure COMMAND test_backpressure)

# ============================================================================
# V7 Phase 21: Panic Policy
# ============================================================================
add_executable(test_panic_policy unit/test_panic_policy.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PanicPolicy.cpp)
target_include_directories(test_panic_policy PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_panic_policy PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_panic_policy COMMAND test_panic_policy)

# ============================================================================
# V7 Phase 24: API Version
# ============================================================================
add_executable(test_api_version unit/test_api_version.cpp)
target_include_directories(test_api_version PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_api_version PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_api_version COMMAND test_api_version)

# ============================================================================
# V7 Phase 25: Plugin Quarantine
# ============================================================================
add_executable(test_plugin_quarantine unit/test_plugin_quarantine.cpp)
target_include_directories(test_plugin_quarantine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_plugin_quarantine PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_plugin_quarantine COMMAND test_plugin_quarantine)

# ============================================================================
# V7 Phase 27: Rendering Pipeline Safety
# ============================================================================
add_executable(test_rendering_safety unit/test_rendering_safety.cpp)
target_include_directories(test_rendering_safety PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_rendering_safety PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_rendering_safety COMMAND test_rendering_safety)

# ============================================================================
# V7 Phase 30: Error Reporting Service
# ============================================================================
add_executable(test_error_reporting unit/test_error_reporting.cpp)
target_include_directories(test_error_reporting PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_error_reporting PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_error_reporting COMMAND test_error_reporting)

# ============================================================================
# V7 Phase 31: Atomic Writer
# ============================================================================
add_executable(test_atomic_writer unit/test_atomic_writer.cpp)
target_include_directories(test_atomic_writer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_atomic_writer PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_atomic_writer COMMAND test_atomic_writer)

# ============================================================================
# V7 Phase 32: Write-Ahead Log
# ============================================================================
add_executable(test_wal unit/test_wal.cpp)
target_include_directories(test_wal PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_wal PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_wal COMMAND test_wal)

# ============================================================================
# V7 Phases 33-35: Telemetry, Trace Spans, OTLP Export
# ============================================================================
add_executable(test_telemetry unit/test_telemetry.cpp)
target_include_directories(test_telemetry PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_telemetry PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_telemetry COMMAND test_telemetry)

# ============================================================================
# V7 Phase 36: Safe Mode
# ============================================================================
add_executable(test_safe_mode unit/test_safe_mode.cpp)
target_include_directories(test_safe_mode PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_safe_mode PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_safe_mode COMMAND test_safe_mode)

# ============================================================================
# V7 Phases 37-38: Fault Recovery & Domains
# ============================================================================
add_executable(test_fault_recovery unit/test_fault_recovery.cpp)
target_include_directories(test_fault_recovery PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_fault_recovery PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_fault_recovery COMMAND test_fault_recovery)

# ============================================================================
# V7 Phases 39-40: Enterprise Runtime & Chaos Plugin
# ============================================================================
add_executable(test_enterprise_runtime unit/test_enterprise_runtime.cpp)
target_include_directories(test_enterprise_runtime PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_enterprise_runtime PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_enterprise_runtime COMMAND test_enterprise_runtime)

# ============================================================================
# Test Coverage Increase: 90 New Unit Tests
# ============================================================================
add_executable(test_alignment_guides unit/test_alignment_guides.cpp)
target_include_directories(test_alignment_guides PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_alignment_guides PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_alignment_guides COMMAND test_alignment_guides)

add_executable(test_asset_service unit/test_asset_service.cpp)
target_include_directories(test_asset_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_asset_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_asset_service COMMAND test_asset_service)

add_executable(test_attribute_service unit/test_attribute_service.cpp)
target_include_directories(test_attribute_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_attribute_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_attribute_service COMMAND test_attribute_service)

add_executable(test_backlink_service unit/test_backlink_service.cpp)
target_include_directories(test_backlink_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_backlink_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_backlink_service COMMAND test_backlink_service)

add_executable(test_block_database unit/test_block_database.cpp)
target_include_directories(test_block_database PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_block_database PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_block_database COMMAND test_block_database)

add_executable(test_block_service unit/test_block_service.cpp)
target_include_directories(test_block_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_block_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_block_service COMMAND test_block_service)

add_executable(test_block_tree_index unit/test_block_tree_index.cpp)
target_include_directories(test_block_tree_index PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_block_tree_index PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_block_tree_index COMMAND test_block_tree_index)

add_executable(test_block_type_detector unit/test_block_type_detector.cpp)
target_include_directories(test_block_type_detector PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_block_type_detector PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_block_type_detector COMMAND test_block_type_detector)

add_executable(test_board_template_data unit/test_board_template_data.cpp)
target_include_directories(test_board_template_data PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_board_template_data PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_board_template_data COMMAND test_board_template_data)

add_executable(test_bookmark_card_data unit/test_bookmark_card_data.cpp)
target_include_directories(test_bookmark_card_data PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_bookmark_card_data PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_bookmark_card_data COMMAND test_bookmark_card_data)

add_executable(test_bookmark_service unit/test_bookmark_service.cpp)
target_include_directories(test_bookmark_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_bookmark_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_bookmark_service COMMAND test_bookmark_service)

add_executable(test_canvas_commands unit/test_canvas_commands.cpp)
target_include_directories(test_canvas_commands PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_commands PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_canvas_commands COMMAND test_canvas_commands)

add_executable(test_canvas_object unit/test_canvas_object.cpp)
target_include_directories(test_canvas_object PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_object PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_canvas_object COMMAND test_canvas_object)

add_executable(test_chunked_storage unit/test_chunked_storage.cpp)
target_include_directories(test_chunked_storage PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_chunked_storage PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_chunked_storage COMMAND test_chunked_storage)

add_executable(test_connector_data unit/test_connector_data.cpp)
target_include_directories(test_connector_data PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_connector_data PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_connector_data COMMAND test_connector_data)

add_executable(test_content_indexer unit/test_content_indexer.cpp)
target_include_directories(test_content_indexer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_content_indexer PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_content_indexer COMMAND test_content_indexer)

add_executable(test_context_key_service unit/test_context_key_service.cpp)
target_include_directories(test_context_key_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_context_key_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_context_key_service COMMAND test_context_key_service)

add_executable(test_daily_note_service unit/test_daily_note_service.cpp)
target_include_directories(test_daily_note_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_daily_note_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_daily_note_service COMMAND test_daily_note_service)

add_executable(test_diagram_shape_data unit/test_diagram_shape_data.cpp)
target_include_directories(test_diagram_shape_data PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_diagram_shape_data PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_diagram_shape_data COMMAND test_diagram_shape_data)

add_executable(test_diff_engine unit/test_diff_engine.cpp)
target_include_directories(test_diff_engine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_diff_engine PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_diff_engine COMMAND test_diff_engine)

add_executable(test_dirty_region unit/test_dirty_region.cpp)
target_include_directories(test_dirty_region PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_dirty_region PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_dirty_region COMMAND test_dirty_region)

add_executable(test_encoding_detector unit/test_encoding_detector.cpp)
target_include_directories(test_encoding_detector PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_encoding_detector PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_encoding_detector COMMAND test_encoding_detector)

add_executable(test_environment_service unit/test_environment_service.cpp)
target_include_directories(test_environment_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_environment_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_environment_service COMMAND test_environment_service)

add_executable(test_extension_enablement unit/test_extension_enablement.cpp)
target_include_directories(test_extension_enablement PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_extension_enablement PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_extension_enablement COMMAND test_extension_enablement)

add_executable(test_extension_events unit/test_extension_events.cpp)
target_include_directories(test_extension_events PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_extension_events PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_extension_events COMMAND test_extension_events)

add_executable(test_extension_host_recovery unit/test_extension_host_recovery.cpp)
target_include_directories(test_extension_host_recovery PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_extension_host_recovery PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_extension_host_recovery COMMAND test_extension_host_recovery)

add_executable(test_extension_recommendations unit/test_extension_recommendations.cpp)
target_include_directories(test_extension_recommendations PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_extension_recommendations PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_extension_recommendations COMMAND test_extension_recommendations)

add_executable(test_extension_storage unit/test_extension_storage.cpp)
target_include_directories(test_extension_storage PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_extension_storage PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_extension_storage COMMAND test_extension_storage)

add_executable(test_file_snapshot_service unit/test_file_snapshot_service.cpp)
target_include_directories(test_file_snapshot_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_file_snapshot_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_file_snapshot_service COMMAND test_file_snapshot_service)

add_executable(test_frame_arena unit/test_frame_arena.cpp)
target_include_directories(test_frame_arena PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_frame_arena PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_frame_arena COMMAND test_frame_arena)

add_executable(test_frame_object_data unit/test_frame_object_data.cpp)
target_include_directories(test_frame_object_data PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_frame_object_data PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_frame_object_data COMMAND test_frame_object_data)

add_executable(test_glyph_advance_cache unit/test_glyph_advance_cache.cpp)
target_include_directories(test_glyph_advance_cache PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_glyph_advance_cache PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_glyph_advance_cache COMMAND test_glyph_advance_cache)

add_executable(test_grouping_service unit/test_grouping_service.cpp)
target_include_directories(test_grouping_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_grouping_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_grouping_service COMMAND test_grouping_service)

add_executable(test_hit_test_accelerator unit/test_hit_test_accelerator.cpp)
target_include_directories(test_hit_test_accelerator PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_hit_test_accelerator PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_hit_test_accelerator COMMAND test_hit_test_accelerator)

add_executable(test_ial_parser unit/test_ial_parser.cpp)
target_include_directories(test_ial_parser PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_ial_parser PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_ial_parser COMMAND test_ial_parser)

add_executable(test_image_object_data unit/test_image_object_data.cpp)
target_include_directories(test_image_object_data PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_image_object_data PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_image_object_data COMMAND test_image_object_data)

add_executable(test_import_types unit/test_import_types.cpp)
target_include_directories(test_import_types PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_import_types PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_import_types COMMAND test_import_types)

add_executable(test_incremental_searcher unit/test_incremental_searcher.cpp)
target_include_directories(test_incremental_searcher PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_incremental_searcher PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_incremental_searcher COMMAND test_incremental_searcher)

add_executable(test_incremental_tokenizer unit/test_incremental_tokenizer.cpp)
target_include_directories(test_incremental_tokenizer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_incremental_tokenizer PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_incremental_tokenizer COMMAND test_incremental_tokenizer)

add_executable(test_line_index unit/test_line_index.cpp)
target_include_directories(test_line_index PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_line_index PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_line_index COMMAND test_line_index)

add_executable(test_notebook_diff_engine unit/test_notebook_diff_engine.cpp)
target_include_directories(test_notebook_diff_engine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_notebook_diff_engine PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_notebook_diff_engine COMMAND test_notebook_diff_engine)

add_executable(test_notification_service unit/test_notification_service.cpp)
target_include_directories(test_notification_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_notification_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_notification_service COMMAND test_notification_service)

add_executable(test_outline_service unit/test_outline_service.cpp)
target_include_directories(test_outline_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_outline_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_outline_service COMMAND test_outline_service)

add_executable(test_pdf_annotation_store unit/test_pdf_annotation_store.cpp)
target_include_directories(test_pdf_annotation_store PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_pdf_annotation_store PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_pdf_annotation_store COMMAND test_pdf_annotation_store)

add_executable(test_piece_table unit/test_piece_table.cpp)
target_include_directories(test_piece_table PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_piece_table PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_piece_table COMMAND test_piece_table)

add_executable(test_plugin_context unit/test_plugin_context.cpp)
target_include_directories(test_plugin_context PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_plugin_context PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_plugin_context COMMAND test_plugin_context)

add_executable(test_plugin_memory_tracker unit/test_plugin_memory_tracker.cpp)
target_include_directories(test_plugin_memory_tracker PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_plugin_memory_tracker PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_plugin_memory_tracker COMMAND test_plugin_memory_tracker)

add_executable(test_plugin_system_phase04 unit/test_plugin_system_phase04.cpp)
target_include_directories(test_plugin_system_phase04 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_plugin_system_phase04 PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_plugin_system_phase04 COMMAND test_plugin_system_phase04)

add_executable(test_plugin_safe_call unit/test_plugin_safe_call.cpp)
target_include_directories(test_plugin_safe_call PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_plugin_safe_call PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_plugin_safe_call COMMAND test_plugin_safe_call)

add_executable(test_progress_service unit/test_progress_service.cpp)
target_include_directories(test_progress_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_progress_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_progress_service COMMAND test_progress_service)

add_executable(test_query_service unit/test_query_service.cpp)
target_include_directories(test_query_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_query_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_query_service COMMAND test_query_service)

add_executable(test_recent_files unit/test_recent_files.cpp)
target_include_directories(test_recent_files PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_recent_files PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_recent_files COMMAND test_recent_files)

add_executable(test_recent_workspaces unit/test_recent_workspaces.cpp)
target_include_directories(test_recent_workspaces PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_recent_workspaces PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_recent_workspaces COMMAND test_recent_workspaces)

add_executable(test_ref_parser unit/test_ref_parser.cpp)
target_include_directories(test_ref_parser PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_ref_parser PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_ref_parser COMMAND test_ref_parser)

add_executable(test_ref_resolver unit/test_ref_resolver.cpp)
target_include_directories(test_ref_resolver PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_ref_resolver PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_ref_resolver COMMAND test_ref_resolver)

add_executable(test_reference_scanner unit/test_reference_scanner.cpp)
target_include_directories(test_reference_scanner PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_reference_scanner PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_reference_scanner COMMAND test_reference_scanner)

add_executable(test_repository_service unit/test_repository_service.cpp)
target_include_directories(test_repository_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_repository_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_repository_service COMMAND test_repository_service)

add_executable(test_ring_buffer unit/test_ring_buffer.cpp)
target_include_directories(test_ring_buffer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_ring_buffer PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_ring_buffer COMMAND test_ring_buffer)

add_executable(test_search_service unit/test_search_service.cpp)
target_include_directories(test_search_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_search_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_search_service COMMAND test_search_service)

add_executable(test_section_object_data unit/test_section_object_data.cpp)
target_include_directories(test_section_object_data PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_section_object_data PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_section_object_data COMMAND test_section_object_data)

add_executable(test_shape_data unit/test_shape_data.cpp)
target_include_directories(test_shape_data PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_shape_data PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_shape_data COMMAND test_shape_data)

add_executable(test_shortcut_manager unit/test_shortcut_manager.cpp)
target_include_directories(test_shortcut_manager PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_shortcut_manager PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_shortcut_manager COMMAND test_shortcut_manager)

add_executable(test_snapshot_store unit/test_snapshot_store.cpp)
target_include_directories(test_snapshot_store PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_snapshot_store PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_snapshot_store COMMAND test_snapshot_store)

add_executable(test_snippet_engine unit/test_snippet_engine.cpp)
target_include_directories(test_snippet_engine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_snippet_engine PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_snippet_engine COMMAND test_snippet_engine)

add_executable(test_spsc_queue unit/test_spsc_queue.cpp)
target_include_directories(test_spsc_queue PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_spsc_queue PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_spsc_queue COMMAND test_spsc_queue)

add_executable(test_sql_query unit/test_sql_query.cpp)
target_include_directories(test_sql_query PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_sql_query PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_sql_query COMMAND test_sql_query)

add_executable(test_stable_line_id unit/test_stable_line_id.cpp)
target_include_directories(test_stable_line_id PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_stable_line_id PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_stable_line_id COMMAND test_stable_line_id)

add_executable(test_sticky_note_object unit/test_sticky_note_object.cpp)
target_include_directories(test_sticky_note_object PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_sticky_note_object PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_sticky_note_object COMMAND test_sticky_note_object)

add_executable(test_string_utils unit/test_string_utils.cpp)
target_include_directories(test_string_utils PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_string_utils PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_string_utils COMMAND test_string_utils)

add_executable(test_style_run_store unit/test_style_run_store.cpp)
target_include_directories(test_style_run_store PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_style_run_store PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_style_run_store COMMAND test_style_run_store)

add_executable(test_surface_link unit/test_surface_link.cpp)
target_include_directories(test_surface_link PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_surface_link PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_surface_link COMMAND test_surface_link)

add_executable(test_template_engine unit/test_template_engine.cpp)
target_include_directories(test_template_engine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_template_engine PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_template_engine COMMAND test_template_engine)

add_executable(test_template_functions unit/test_template_functions.cpp)
target_include_directories(test_template_functions PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_template_functions PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_template_functions COMMAND test_template_functions)

add_executable(test_template_types unit/test_template_types.cpp)
target_include_directories(test_template_types PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_template_types PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_template_types COMMAND test_template_types)

add_executable(test_text_box_object unit/test_text_box_object.cpp)
target_include_directories(test_text_box_object PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_text_box_object PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_text_box_object COMMAND test_text_box_object)

add_executable(test_text_editor_service unit/test_text_editor_service.cpp)
target_include_directories(test_text_editor_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_text_editor_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_text_editor_service COMMAND test_text_editor_service)

add_executable(test_text_span unit/test_text_span.cpp)
target_include_directories(test_text_span PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_text_span PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_text_span COMMAND test_text_span)

add_executable(test_theme_registry unit/test_theme_registry.cpp)
target_include_directories(test_theme_registry PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_theme_registry PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_theme_registry COMMAND test_theme_registry)

add_executable(test_theme_scope_mapper unit/test_theme_scope_mapper.cpp)
target_include_directories(test_theme_scope_mapper PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_theme_scope_mapper PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_theme_scope_mapper COMMAND test_theme_scope_mapper)

add_executable(test_theme_validator unit/test_theme_validator.cpp)
target_include_directories(test_theme_validator PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_theme_validator PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_theme_validator COMMAND test_theme_validator)

add_executable(test_undo_redo_stack unit/test_undo_redo_stack.cpp)
target_include_directories(test_undo_redo_stack PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_undo_redo_stack PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_undo_redo_stack COMMAND test_undo_redo_stack)

add_executable(test_viewport_cache unit/test_viewport_cache.cpp)
target_include_directories(test_viewport_cache PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_viewport_cache PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_viewport_cache COMMAND test_viewport_cache)

add_executable(test_vscode_theme_adapter unit/test_vscode_theme_adapter.cpp)
target_include_directories(test_vscode_theme_adapter PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_vscode_theme_adapter PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_vscode_theme_adapter COMMAND test_vscode_theme_adapter)

add_executable(test_when_clause unit/test_when_clause.cpp)
target_include_directories(test_when_clause PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_when_clause PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_when_clause COMMAND test_when_clause)

add_executable(test_wiki_link unit/test_wiki_link.cpp)
target_include_directories(test_wiki_link PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_wiki_link PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_wiki_link COMMAND test_wiki_link)

add_executable(test_wiki_link_parser unit/test_wiki_link_parser.cpp)
target_include_directories(test_wiki_link_parser PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_wiki_link_parser PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_wiki_link_parser COMMAND test_wiki_link_parser)

add_executable(test_workspace_service unit/test_workspace_service.cpp)
target_include_directories(test_workspace_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_workspace_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_workspace_service COMMAND test_workspace_service)

add_executable(test_write_ahead_log unit/test_write_ahead_log.cpp)
target_include_directories(test_write_ahead_log PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_write_ahead_log PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_write_ahead_log COMMAND test_write_ahead_log)

# ═══════════════════════════════════════
# Batch 2 — Additional Coverage Tests
# ═══════════════════════════════════════

add_executable(test_a_i_panel unit/test_a_i_panel.cpp)
target_include_directories(test_a_i_panel PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_a_i_panel PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_a_i_panel COMMAND test_a_i_panel)

add_executable(test_a_i_service unit/test_a_i_service.cpp)
target_include_directories(test_a_i_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_a_i_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_a_i_service COMMAND test_a_i_service)

add_executable(test_a_i_types unit/test_a_i_types.cpp)
target_include_directories(test_a_i_types PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_a_i_types PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_a_i_types COMMAND test_a_i_types)

add_executable(test_accessibility unit/test_accessibility.cpp)
target_include_directories(test_accessibility PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_accessibility PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_accessibility COMMAND test_accessibility)

add_executable(test_accessibility_manager unit/test_accessibility_manager.cpp)
target_include_directories(test_accessibility_manager PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_accessibility_manager PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_accessibility_manager COMMAND test_accessibility_manager)

add_executable(test_adaptive_throttle unit/test_adaptive_throttle.cpp)
target_include_directories(test_adaptive_throttle PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_adaptive_throttle PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_adaptive_throttle COMMAND test_adaptive_throttle)

add_executable(test_allocator_config unit/test_allocator_config.cpp)
target_include_directories(test_allocator_config PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_allocator_config PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_allocator_config COMMAND test_allocator_config)

add_executable(test_app_init unit/test_app_init.cpp)
target_include_directories(test_app_init PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_app_init PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_app_init COMMAND test_app_init)

add_executable(test_app_state unit/test_app_state.cpp)
target_include_directories(test_app_state PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_app_state PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_app_state COMMAND test_app_state)

add_executable(test_app_widget_object unit/test_app_widget_object.cpp)
target_include_directories(test_app_widget_object PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_app_widget_object PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_app_widget_object COMMAND test_app_widget_object)

add_executable(test_asset_types unit/test_asset_types.cpp)
target_include_directories(test_asset_types PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_asset_types PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_asset_types COMMAND test_asset_types)

add_executable(test_async_file_loader unit/test_async_file_loader.cpp)
target_include_directories(test_async_file_loader PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_async_file_loader PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_async_file_loader COMMAND test_async_file_loader)

add_executable(test_async_highlighter unit/test_async_highlighter.cpp)
target_include_directories(test_async_highlighter PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_async_highlighter PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_async_highlighter COMMAND test_async_highlighter)

add_executable(test_async_pipeline unit/test_async_pipeline.cpp)
target_include_directories(test_async_pipeline PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_async_pipeline PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_async_pipeline COMMAND test_async_pipeline)

add_executable(test_backlink unit/test_backlink.cpp)
target_include_directories(test_backlink PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_backlink PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_backlink COMMAND test_backlink)

add_executable(test_bevel_panel unit/test_bevel_panel.cpp)
target_include_directories(test_bevel_panel PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_bevel_panel PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_bevel_panel COMMAND test_bevel_panel)

add_executable(test_block unit/test_block.cpp)
target_include_directories(test_block PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_block PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_block COMMAND test_block)

add_executable(test_block_diff_engine unit/test_block_diff_engine.cpp)
target_include_directories(test_block_diff_engine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_block_diff_engine PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_block_diff_engine COMMAND test_block_diff_engine)

add_executable(test_block_i_d unit/test_block_i_d.cpp)
target_include_directories(test_block_i_d PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_block_i_d PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_block_i_d COMMAND test_block_i_d)

add_executable(test_block_ref unit/test_block_ref.cpp)
target_include_directories(test_block_ref PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_block_ref PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_block_ref COMMAND test_block_ref)

add_executable(test_board unit/test_board.cpp)
target_include_directories(test_board PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_board PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_board COMMAND test_board)

add_executable(test_board_navigator unit/test_board_navigator.cpp)
target_include_directories(test_board_navigator PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_board_navigator PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_board_navigator COMMAND test_board_navigator)

add_executable(test_board_template unit/test_board_template.cpp)
target_include_directories(test_board_template PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_board_template PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_board_template COMMAND test_board_template)

add_executable(test_bookmark unit/test_bookmark.cpp)
target_include_directories(test_bookmark PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_bookmark PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_bookmark COMMAND test_bookmark)

add_executable(test_bookmark_card_object unit/test_bookmark_card_object.cpp)
target_include_directories(test_bookmark_card_object PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_bookmark_card_object PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_bookmark_card_object COMMAND test_bookmark_card_object)

add_executable(test_bounded_container unit/test_bounded_container.cpp)
target_include_directories(test_bounded_container PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_bounded_container PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_bounded_container COMMAND test_bounded_container)

add_executable(test_built_in_plugins unit/test_built_in_plugins.cpp)
target_include_directories(test_built_in_plugins PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_built_in_plugins PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_built_in_plugins COMMAND test_built_in_plugins)

add_executable(test_built_in_themes unit/test_built_in_themes.cpp)
target_include_directories(test_built_in_themes PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_built_in_themes PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_built_in_themes COMMAND test_built_in_themes)

add_executable(test_canvas_app_manifest unit/test_canvas_app_manifest.cpp)
target_include_directories(test_canvas_app_manifest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_app_manifest PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_canvas_app_manifest COMMAND test_canvas_app_manifest)

add_executable(test_canvas_collab_service unit/test_canvas_collab_service.cpp)
target_include_directories(test_canvas_collab_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_collab_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_canvas_collab_service COMMAND test_canvas_collab_service)

add_executable(test_canvas_input_manager unit/test_canvas_input_manager.cpp)
target_include_directories(test_canvas_input_manager PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_input_manager PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_canvas_input_manager COMMAND test_canvas_input_manager)

add_executable(test_canvas_integration_service unit/test_canvas_integration_service.cpp)
target_include_directories(test_canvas_integration_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_integration_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_canvas_integration_service COMMAND test_canvas_integration_service)

add_executable(test_card_factory unit/test_card_factory.cpp)
target_include_directories(test_card_factory PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_card_factory PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_card_factory COMMAND test_card_factory)

add_executable(test_chaos_plugin unit/test_chaos_plugin.cpp)
target_include_directories(test_chaos_plugin PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_chaos_plugin PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_chaos_plugin COMMAND test_chaos_plugin)

add_executable(test_cloud_sync_service unit/test_cloud_sync_service.cpp)
target_include_directories(test_cloud_sync_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_cloud_sync_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_cloud_sync_service COMMAND test_cloud_sync_service)

add_executable(test_cloud_sync_types unit/test_cloud_sync_types.cpp)
target_include_directories(test_cloud_sync_types PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_cloud_sync_types PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_cloud_sync_types COMMAND test_cloud_sync_types)

add_executable(test_coalescing_task unit/test_coalescing_task.cpp)
target_include_directories(test_coalescing_task PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_coalescing_task PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_coalescing_task COMMAND test_coalescing_task)

add_executable(test_color unit/test_color.cpp)
target_include_directories(test_color PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_color PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_color COMMAND test_color)

add_executable(test_command unit/test_command.cpp)
target_include_directories(test_command PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_command PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_command COMMAND test_command)

add_executable(test_command_history unit/test_command_history.cpp)
target_include_directories(test_command_history PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_command_history PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_command_history COMMAND test_command_history)

add_executable(test_comment_object unit/test_comment_object.cpp)
target_include_directories(test_comment_object PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_comment_object PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_comment_object COMMAND test_comment_object)

add_executable(test_compiler_hints unit/test_compiler_hints.cpp)
target_include_directories(test_compiler_hints PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_compiler_hints PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_compiler_hints COMMAND test_compiler_hints)

add_executable(test_config unit/test_config.cpp)
target_include_directories(test_config PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_config PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_config COMMAND test_config)

add_executable(test_config_v9 unit/test_config_v9.cpp)
target_include_directories(test_config_v9 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_config_v9 PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_config_v9 COMMAND test_config_v9)

add_executable(test_theme_v9 unit/test_theme_v9.cpp)
target_include_directories(test_theme_v9 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_theme_v9 PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_theme_v9 COMMAND test_theme_v9)

add_executable(test_cross_platform unit/test_cross_platform.cpp)
target_include_directories(test_cross_platform PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_cross_platform PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_cross_platform COMMAND test_cross_platform)

add_executable(test_custom_chrome unit/test_custom_chrome.cpp)
target_include_directories(test_custom_chrome PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_custom_chrome PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_custom_chrome COMMAND test_custom_chrome)

add_executable(test_deck_manager unit/test_deck_manager.cpp)
target_include_directories(test_deck_manager PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_deck_manager PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_deck_manager COMMAND test_deck_manager)

add_executable(test_deck_store unit/test_deck_store.cpp)
target_include_directories(test_deck_store PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_deck_store PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_deck_store COMMAND test_deck_store)

add_executable(test_decoration_service unit/test_decoration_service.cpp)
target_include_directories(test_decoration_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_decoration_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_decoration_service COMMAND test_decoration_service)

add_executable(test_dependencies unit/test_dependencies.cpp)
target_include_directories(test_dependencies PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_dependencies PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_dependencies COMMAND test_dependencies)

add_executable(test_diagnostics_service unit/test_diagnostics_service.cpp)
target_include_directories(test_diagnostics_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_diagnostics_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_diagnostics_service COMMAND test_diagnostics_service)

add_executable(test_diagram_renderer_registry unit/test_diagram_renderer_registry.cpp)
target_include_directories(test_diagram_renderer_registry PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_diagram_renderer_registry PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_diagram_renderer_registry COMMAND test_diagram_renderer_registry)

add_executable(test_diagram_shape_object unit/test_diagram_shape_object.cpp)
target_include_directories(test_diagram_shape_object PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_diagram_shape_object PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_diagram_shape_object COMMAND test_diagram_shape_object)

add_executable(test_diff_types unit/test_diff_types.cpp)
target_include_directories(test_diff_types PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_diff_types PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_diff_types COMMAND test_diff_types)

add_executable(test_document_file_system unit/test_document_file_system.cpp)
target_include_directories(test_document_file_system PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_document_file_system PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_document_file_system COMMAND test_document_file_system)

add_executable(test_document_snapshot unit/test_document_snapshot.cpp)
target_include_directories(test_document_snapshot PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_document_snapshot PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_document_snapshot COMMAND test_document_snapshot)

add_executable(test_e_charts_renderer unit/test_e_charts_renderer.cpp)
target_include_directories(test_e_charts_renderer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_e_charts_renderer PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_e_charts_renderer COMMAND test_e_charts_renderer)

add_executable(test_editor unit/test_editor.cpp)
target_include_directories(test_editor PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_editor PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_editor COMMAND test_editor)

add_executable(test_editor_controls unit/test_editor_controls.cpp)
target_include_directories(test_editor_controls PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_editor_controls PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_editor_controls COMMAND test_editor_controls)

add_executable(test_editor_enhancements unit/test_editor_enhancements.cpp)
target_include_directories(test_editor_enhancements PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_editor_enhancements PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_editor_enhancements COMMAND test_editor_enhancements)

add_executable(test_error_reporting_service unit/test_error_reporting_service.cpp)
target_include_directories(test_error_reporting_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_error_reporting_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_error_reporting_service COMMAND test_error_reporting_service)

add_executable(test_event_bus unit/test_event_bus.cpp)
target_include_directories(test_event_bus PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_event_bus PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_event_bus COMMAND test_event_bus)

add_executable(test_event_type_id unit/test_event_type_id.cpp)
target_include_directories(test_event_type_id PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_event_type_id PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_event_type_id COMMAND test_event_type_id)

add_executable(test_events unit/test_events.cpp)
target_include_directories(test_events PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_events PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_events COMMAND test_events)

add_executable(test_export_dialog unit/test_export_dialog.cpp)
target_include_directories(test_export_dialog PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_export_dialog PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_export_dialog COMMAND test_export_dialog)

add_executable(test_export_service unit/test_export_service.cpp)
target_include_directories(test_export_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_export_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_export_service COMMAND test_export_service)

add_executable(test_export_types unit/test_export_types.cpp)
target_include_directories(test_export_types PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_export_types PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_export_types COMMAND test_export_types)

add_executable(test_extension_resource_tracker unit/test_extension_resource_tracker.cpp)
target_include_directories(test_extension_resource_tracker PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_extension_resource_tracker PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_extension_resource_tracker COMMAND test_extension_resource_tracker)

add_executable(test_extension_sandbox unit/test_extension_sandbox.cpp)
target_include_directories(test_extension_sandbox PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_extension_sandbox PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_extension_sandbox COMMAND test_extension_sandbox)

add_executable(test_extension_telemetry unit/test_extension_telemetry.cpp)
target_include_directories(test_extension_telemetry PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_extension_telemetry PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_extension_telemetry COMMAND test_extension_telemetry)

add_executable(test_fault_domain unit/test_fault_domain.cpp)
target_include_directories(test_fault_domain PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_fault_domain PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_fault_domain COMMAND test_fault_domain)

add_executable(test_file_node unit/test_file_node.cpp)
target_include_directories(test_file_node PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_file_node PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_file_node COMMAND test_file_node)

add_executable(test_file_system unit/test_file_system.cpp)
target_include_directories(test_file_system PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_file_system PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_file_system COMMAND test_file_system)

add_executable(test_file_system_provider_registry unit/test_file_system_provider_registry.cpp)
target_include_directories(test_file_system_provider_registry PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_file_system_provider_registry PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_file_system_provider_registry COMMAND test_file_system_provider_registry)

add_executable(test_file_tree unit/test_file_tree.cpp)
target_include_directories(test_file_tree PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_file_tree PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_file_tree COMMAND test_file_tree)

add_executable(test_find_replace_service unit/test_find_replace_service.cpp)
target_include_directories(test_find_replace_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_find_replace_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_find_replace_service COMMAND test_find_replace_service)

add_executable(test_flashcard_types unit/test_flashcard_types.cpp)
target_include_directories(test_flashcard_types PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_flashcard_types PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_flashcard_types COMMAND test_flashcard_types)

add_executable(test_flat_event_bus unit/test_flat_event_bus.cpp)
target_include_directories(test_flat_event_bus PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_flat_event_bus PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_flat_event_bus COMMAND test_flat_event_bus)

add_executable(test_flowchart_renderer unit/test_flowchart_renderer.cpp)
target_include_directories(test_flowchart_renderer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_flowchart_renderer PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_flowchart_renderer COMMAND test_flowchart_renderer)

add_executable(test_force_directed_layout unit/test_force_directed_layout.cpp)
target_include_directories(test_force_directed_layout PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_force_directed_layout PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_force_directed_layout COMMAND test_force_directed_layout)

add_executable(test_frame_budget_enforcer unit/test_frame_budget_enforcer.cpp)
target_include_directories(test_frame_budget_enforcer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_frame_budget_enforcer PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_frame_budget_enforcer COMMAND test_frame_budget_enforcer)

add_executable(test_frame_budget_token unit/test_frame_budget_token.cpp)
target_include_directories(test_frame_budget_token PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_frame_budget_token PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_frame_budget_token COMMAND test_frame_budget_token)

add_executable(test_frame_scheduler unit/test_frame_scheduler.cpp)
target_include_directories(test_frame_scheduler PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_frame_scheduler PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_frame_scheduler COMMAND test_frame_scheduler)

add_executable(test_generation_counter unit/test_generation_counter.cpp)
target_include_directories(test_generation_counter PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_generation_counter PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_generation_counter COMMAND test_generation_counter)

add_executable(test_gfm_extensions unit/test_gfm_extensions.cpp)
target_include_directories(test_gfm_extensions PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_gfm_extensions PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_gfm_extensions COMMAND test_gfm_extensions)

add_executable(test_grammar_engine unit/test_grammar_engine.cpp)
target_include_directories(test_grammar_engine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_grammar_engine PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_grammar_engine COMMAND test_grammar_engine)

add_executable(test_graph_service unit/test_graph_service.cpp)
target_include_directories(test_graph_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_graph_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_graph_service COMMAND test_graph_service)

add_executable(test_graph_types unit/test_graph_types.cpp)
target_include_directories(test_graph_types PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_graph_types PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_graph_types COMMAND test_graph_types)

add_executable(test_grapheme_boundary_cache unit/test_grapheme_boundary_cache.cpp)
target_include_directories(test_grapheme_boundary_cache PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_grapheme_boundary_cache PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_grapheme_boundary_cache COMMAND test_grapheme_boundary_cache)

add_executable(test_graphviz_renderer unit/test_graphviz_renderer.cpp)
target_include_directories(test_graphviz_renderer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_graphviz_renderer PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_graphviz_renderer COMMAND test_graphviz_renderer)

add_executable(test_history_service unit/test_history_service.cpp)
target_include_directories(test_history_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_history_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_history_service COMMAND test_history_service)

add_executable(test_history_types unit/test_history_types.cpp)
target_include_directories(test_history_types PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_history_types PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_history_types COMMAND test_history_types)

add_executable(test_html_exporter unit/test_html_exporter.cpp)
target_include_directories(test_html_exporter PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_html_exporter PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_html_exporter COMMAND test_html_exporter)

add_executable(test_html_importer unit/test_html_importer.cpp)
target_include_directories(test_html_importer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_html_importer PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_html_importer COMMAND test_html_importer)

add_executable(test_html_renderer unit/test_html_renderer.cpp)
target_include_directories(test_html_renderer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_html_renderer PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_html_renderer COMMAND test_html_renderer)

add_executable(test_html_sanitizer unit/test_html_sanitizer.cpp)
target_include_directories(test_html_sanitizer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_html_sanitizer PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_html_sanitizer COMMAND test_html_sanitizer)

add_executable(test_http_client unit/test_http_client.cpp)
target_include_directories(test_http_client PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_http_client PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_http_client COMMAND test_http_client)

add_executable(test_i_a_l_parser unit/test_i_a_l_parser.cpp)
target_include_directories(test_i_a_l_parser PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_i_a_l_parser PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_i_a_l_parser COMMAND test_i_a_l_parser)

add_executable(test_i_diagram_renderer unit/test_i_diagram_renderer.cpp)
target_include_directories(test_i_diagram_renderer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_i_diagram_renderer PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_i_diagram_renderer COMMAND test_i_diagram_renderer)

add_executable(test_icon_object unit/test_icon_object.cpp)
target_include_directories(test_icon_object PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_icon_object PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_icon_object COMMAND test_icon_object)

add_executable(test_kanban_controller unit/test_kanban_controller.cpp)
target_include_directories(test_kanban_controller PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_kanban_controller PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_kanban_controller COMMAND test_kanban_controller)

add_executable(test_kanban_objects unit/test_kanban_objects.cpp)
target_include_directories(test_kanban_objects PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_kanban_objects PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_kanban_objects COMMAND test_kanban_objects)

add_executable(test_keyboard_navigation unit/test_keyboard_navigation.cpp)
target_include_directories(test_keyboard_navigation PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_keyboard_navigation PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_keyboard_navigation COMMAND test_keyboard_navigation)

add_executable(test_layout unit/test_layout.cpp)
target_include_directories(test_layout PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_layout PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_layout COMMAND test_layout)

add_executable(test_link_suggestion unit/test_link_suggestion.cpp)
target_include_directories(test_link_suggestion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_link_suggestion PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_link_suggestion COMMAND test_link_suggestion)

add_executable(test_mark_amp_app unit/test_mark_amp_app.cpp)
target_include_directories(test_mark_amp_app PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_mark_amp_app PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_mark_amp_app COMMAND test_mark_amp_app)

add_executable(test_markdown_parser unit/test_markdown_parser.cpp)
target_include_directories(test_markdown_parser PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_markdown_parser PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_markdown_parser COMMAND test_markdown_parser)

add_executable(test_mermaid_renderer unit/test_mermaid_renderer.cpp)
target_include_directories(test_mermaid_renderer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_mermaid_renderer PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_mermaid_renderer COMMAND test_mermaid_renderer)

add_executable(test_metadata_scraper unit/test_metadata_scraper.cpp)
target_include_directories(test_metadata_scraper PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_metadata_scraper PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_metadata_scraper COMMAND test_metadata_scraper)

add_executable(test_mind_map_controller unit/test_mind_map_controller.cpp)
target_include_directories(test_mind_map_controller PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_mind_map_controller PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_mind_map_controller COMMAND test_mind_map_controller)

add_executable(test_mind_map_node unit/test_mind_map_node.cpp)
target_include_directories(test_mind_map_node PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_mind_map_node PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_mind_map_node COMMAND test_mind_map_node)

add_executable(test_outline unit/test_outline.cpp)
target_include_directories(test_outline PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_outline PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_outline COMMAND test_outline)

add_executable(test_p_d_f_importer unit/test_p_d_f_importer.cpp)
target_include_directories(test_p_d_f_importer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_p_d_f_importer PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_p_d_f_importer COMMAND test_p_d_f_importer)

add_executable(test_p_d_f_page_object unit/test_p_d_f_page_object.cpp)
target_include_directories(test_p_d_f_page_object PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_p_d_f_page_object PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_p_d_f_page_object COMMAND test_p_d_f_page_object)

add_executable(test_plugin_manager unit/test_plugin_manager.cpp)
target_include_directories(test_plugin_manager PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_plugin_manager PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_plugin_manager COMMAND test_plugin_manager)

add_executable(test_preview_panel unit/test_preview_panel.cpp)
target_include_directories(test_preview_panel PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_preview_panel PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_preview_panel COMMAND test_preview_panel)

add_executable(test_profiler unit/test_profiler.cpp)
target_include_directories(test_profiler PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_profiler PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_profiler COMMAND test_profiler)

add_executable(test_result unit/test_result.cpp)
target_include_directories(test_result PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_result PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_result COMMAND test_result)

add_executable(test_security unit/test_security.cpp)
target_include_directories(test_security PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_security PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_security COMMAND test_security)

add_executable(test_service_registry unit/test_service_registry.cpp)
target_include_directories(test_service_registry PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_service_registry PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_service_registry COMMAND test_service_registry)

add_executable(test_split_view unit/test_split_view.cpp)
target_include_directories(test_split_view PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_split_view PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_split_view COMMAND test_split_view)

add_executable(test_svg_document unit/test_svg_document.cpp)
target_include_directories(test_svg_document PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_svg_document PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_svg_document COMMAND test_svg_document)

add_executable(test_icon_renderer unit/test_icon_renderer.cpp)
target_include_directories(test_icon_renderer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_icon_renderer PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_icon_renderer COMMAND test_icon_renderer)

add_executable(test_icon_cache unit/test_icon_cache.cpp)
target_include_directories(test_icon_cache PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_icon_cache PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_icon_cache COMMAND test_icon_cache)

# V16 Phases 21-23: Polish, cache, resilience tests
add_executable(test_v16_polish unit/test_v16_polish.cpp)
target_include_directories(test_v16_polish PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v16_polish PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v16_polish COMMAND test_v16_polish)

# V16: Theme integration — scope mapping, token completeness, syntax defaults, round-trips
add_executable(test_v16_theme_integration unit/test_v16_theme_integration.cpp)
target_include_directories(test_v16_theme_integration PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v16_theme_integration PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v16_theme_integration COMMAND test_v16_theme_integration)

# V16: Icon coverage — color overrides, HiDPI, cache warming, compound extensions
add_executable(test_v16_icon_coverage unit/test_v16_icon_coverage.cpp)
target_include_directories(test_v16_icon_coverage PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v16_icon_coverage PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v16_icon_coverage COMMAND test_v16_icon_coverage)

# V16: Grammar coverage — custom registration, nested scopes, semantic delta, overlay
add_executable(test_v16_grammar_coverage unit/test_v16_grammar_coverage.cpp)
target_include_directories(test_v16_grammar_coverage PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v16_grammar_coverage PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v16_grammar_coverage COMMAND test_v16_grammar_coverage)

add_executable(test_icon_registry unit/test_icon_registry.cpp)
target_include_directories(test_icon_registry PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_icon_registry PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_icon_registry COMMAND test_icon_registry)

# V16 Phase 01-04: Icon Manifest tests
add_executable(test_icon_manifest unit/test_icon_manifest.cpp)
target_include_directories(test_icon_manifest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_icon_manifest PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_icon_manifest COMMAND test_icon_manifest)

# V16 Phase 09-10: Icon Pipeline tests
add_executable(test_icon_pipeline unit/test_icon_pipeline.cpp)
target_include_directories(test_icon_pipeline PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_icon_pipeline PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_icon_pipeline COMMAND test_icon_pipeline)


add_executable(test_ui_icon_library unit/test_ui_icon_library.cpp)
target_include_directories(test_ui_icon_library PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_ui_icon_library PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_ui_icon_library COMMAND test_ui_icon_library)

add_executable(test_file_type_resolver unit/test_file_type_resolver.cpp)
target_include_directories(test_file_type_resolver PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_file_type_resolver PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_file_type_resolver COMMAND test_file_type_resolver)

add_executable(test_split_view_sync unit/test_split_view_sync.cpp)
target_include_directories(test_split_view_sync PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_split_view_sync PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_split_view_sync COMMAND test_split_view_sync)

# Phase 18: Tool Window Host tests
add_executable(test_tool_window_host unit/test_tool_window_host.cpp)
target_include_directories(test_tool_window_host PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_tool_window_host PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_tool_window_host COMMAND test_tool_window_host)

add_executable(test_editor_core_improvements unit/test_editor_core_improvements.cpp)
target_include_directories(test_editor_core_improvements PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_editor_core_improvements PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_editor_core_improvements COMMAND test_editor_core_improvements)

add_executable(test_status_bar unit/test_status_bar.cpp)
target_include_directories(test_status_bar PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_status_bar PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_status_bar COMMAND test_status_bar)

add_executable(test_syntax_highlighter unit/test_syntax_highlighter.cpp)
target_include_directories(test_syntax_highlighter PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_syntax_highlighter PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_syntax_highlighter COMMAND test_syntax_highlighter)

add_executable(test_tag unit/test_tag.cpp)
target_include_directories(test_tag PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_tag PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_tag COMMAND test_tag)

add_executable(test_theme unit/test_theme.cpp)
target_include_directories(test_theme PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_theme PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_theme COMMAND test_theme)

add_executable(test_theme_engine unit/test_theme_engine.cpp)
target_include_directories(test_theme_engine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_theme_engine PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_theme_engine COMMAND test_theme_engine)

add_executable(test_theme_gallery unit/test_theme_gallery.cpp)
target_include_directories(test_theme_gallery PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_theme_gallery PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_theme_gallery COMMAND test_theme_gallery)

add_executable(test_theme_import_export unit/test_theme_import_export.cpp)
target_include_directories(test_theme_import_export PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_theme_import_export PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_theme_import_export COMMAND test_theme_import_export)

add_executable(test_scoped_token_map unit/test_scoped_token_map.cpp)
target_include_directories(test_scoped_token_map PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_scoped_token_map PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_scoped_token_map COMMAND test_scoped_token_map)

add_executable(test_contrast_validator unit/test_contrast_validator.cpp)
target_include_directories(test_contrast_validator PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_contrast_validator PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_contrast_validator COMMAND test_contrast_validator)

add_executable(test_color_blindness_simulator unit/test_color_blindness_simulator.cpp)
target_include_directories(test_color_blindness_simulator PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_color_blindness_simulator PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_color_blindness_simulator COMMAND test_color_blindness_simulator)

add_executable(test_theme_loader_v2 unit/test_theme_loader_v2.cpp)
target_include_directories(test_theme_loader_v2 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_theme_loader_v2 PRIVATE Catch2::Catch2WithMain markamp_core yaml-cpp)
add_test(NAME test_theme_loader_v2 COMMAND test_theme_loader_v2)

add_executable(test_tile_cache unit/test_tile_cache.cpp)
target_include_directories(test_tile_cache PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_tile_cache PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_tile_cache COMMAND test_tile_cache)

add_executable(test_animation_math unit/test_animation_math.cpp)
target_include_directories(test_animation_math PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_animation_math PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_animation_math COMMAND test_animation_math)

add_executable(test_animation_timeline unit/test_animation_timeline.cpp)
target_include_directories(test_animation_timeline PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_animation_timeline PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_animation_timeline COMMAND test_animation_timeline)

add_executable(test_animation_ui unit/test_animation_ui.cpp)
target_include_directories(test_animation_ui PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_animation_ui PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_animation_ui COMMAND test_animation_ui)

add_executable(test_video_embed_object unit/test_video_embed_object.cpp)
target_include_directories(test_video_embed_object PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_video_embed_object PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_video_embed_object COMMAND test_video_embed_object)

# V9 Phase 19: Code Intelligence Providers tests (header-only providers)
add_executable(test_phase19_code_intelligence test_phase19_code_intelligence.cpp)
target_include_directories(test_phase19_code_intelligence PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase19_code_intelligence PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase19_code_intelligence COMMAND test_phase19_code_intelligence)

# V9 Phase 45: Live Preview & WYSIWYG tests (header-only)
add_executable(test_phase45_wysiwyg unit/test_phase45_wysiwyg.cpp)
target_include_directories(test_phase45_wysiwyg PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase45_wysiwyg PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase45_wysiwyg COMMAND test_phase45_wysiwyg)

# V9 Phase 10: Search System Completion tests (header-only)
add_executable(test_phase10_search unit/test_phase10_search.cpp)
target_include_directories(test_phase10_search PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase10_search PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase10_search COMMAND test_phase10_search)

# V9 Phase 16: Knowledge Graph & Backlinks tests (header-only)
add_executable(test_phase16_knowledge_graph unit/test_phase16_knowledge_graph.cpp)
target_include_directories(test_phase16_knowledge_graph PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase16_knowledge_graph PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase16_knowledge_graph COMMAND test_phase16_knowledge_graph)

# V9 Phase 17: Cross-Surface Navigation & Linking tests (header-only)
add_executable(test_phase17_navigation unit/test_phase17_navigation.cpp)
target_include_directories(test_phase17_navigation PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase17_navigation PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase17_navigation COMMAND test_phase17_navigation)

# Phase 11: Canvas Workbench Shell tests
add_executable(test_phase11_canvas unit/test_phase11_canvas.cpp)
target_include_directories(test_phase11_canvas PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase11_canvas PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase11_canvas COMMAND test_phase11_canvas)

# Canvas sources for testing (exclude wx-dependent Renderer/Panel/Tool/UI files)
file(GLOB ALL_CANVAS_SOURCES "${CMAKE_SOURCE_DIR}/src/canvas/*.cpp")
set(CANVAS_TEST_SOURCES ${ALL_CANVAS_SOURCES})
list(FILTER CANVAS_TEST_SOURCES EXCLUDE REGEX "Renderer\\.cpp$")
list(FILTER CANVAS_TEST_SOURCES EXCLUDE REGEX "Panel\\.cpp$")
list(FILTER CANVAS_TEST_SOURCES EXCLUDE REGEX "Tool\\.cpp$")
list(FILTER CANVAS_TEST_SOURCES EXCLUDE REGEX "Workbench\\.cpp$")
list(FILTER CANVAS_TEST_SOURCES EXCLUDE REGEX "ContextMenu\\.cpp$")
list(FILTER CANVAS_TEST_SOURCES EXCLUDE REGEX "Inspector\\.cpp$")

list(FILTER CANVAS_TEST_SOURCES EXCLUDE REGEX "Accessibility\\.cpp$")
list(FILTER CANVAS_TEST_SOURCES EXCLUDE REGEX "InputManager\\.cpp$")
list(FILTER CANVAS_TEST_SOURCES EXCLUDE REGEX "SearchBar\\.cpp$")
list(FILTER CANVAS_TEST_SOURCES EXCLUDE REGEX "AlignmentGuides\\.cpp$")

# Phase 12: Canvas Advanced Objects & Diagrams tests
add_executable(test_phase12_canvas_advanced
    unit/test_phase12_canvas_advanced.cpp
    ${CANVAS_TEST_SOURCES}
    ${CMAKE_SOURCE_DIR}/src/core/EventBus.cpp
)
target_include_directories(test_phase12_canvas_advanced PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase12_canvas_advanced PRIVATE
    Catch2::Catch2WithMain
    markamp_dependencies
)
add_test(NAME test_phase12_canvas_advanced COMMAND test_phase12_canvas_advanced)

# Phase 13: Canvas Collaboration & Facilitation tests
add_executable(test_phase13_canvas_collab
    unit/test_phase13_canvas_collab.cpp
    ${CANVAS_TEST_SOURCES}
    ${CMAKE_SOURCE_DIR}/src/core/EventBus.cpp
)
target_include_directories(test_phase13_canvas_collab PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase13_canvas_collab PRIVATE
    Catch2::Catch2WithMain
    markamp_dependencies
)
add_test(NAME test_phase13_canvas_collab COMMAND test_phase13_canvas_collab)




# Phase 14: Canvas Extensibility & App Widgets tests
add_executable(test_phase14_canvas_extensibility
    unit/test_phase14_canvas_extensibility.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CustomObjectTypeRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasToolHost.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/WidgetRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/WidgetLifecycleManager.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/WidgetPermissionGuard.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasTemplateEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/WidgetMessageBus.cpp
)
target_include_directories(test_phase14_canvas_extensibility PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase14_canvas_extensibility PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase14_canvas_extensibility COMMAND test_phase14_canvas_extensibility)

# Phase 15: Notebook System Completion
add_executable(test_phase15_notebook_completion
    unit/test_phase15_notebook_completion.cpp
)
target_include_directories(test_phase15_notebook_completion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase15_notebook_completion PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase15_notebook_completion COMMAND test_phase15_notebook_completion)

# Phase 20: File Management & Workspace
add_executable(test_phase20_file_management
    unit/test_phase20_file_management.cpp
)
target_include_directories(test_phase20_file_management PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase20_file_management PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase20_file_management COMMAND test_phase20_file_management)

# Phase 21: Attribute View Database
add_executable(test_phase21_av_database
    unit/test_phase21_av_database.cpp
)
target_include_directories(test_phase21_av_database PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase21_av_database PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase21_av_database COMMAND test_phase21_av_database)

add_executable(test_phase22_flashcard_system
    unit/test_phase22_flashcard_system.cpp
)
target_include_directories(test_phase22_flashcard_system PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase22_flashcard_system PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase22_flashcard_system COMMAND test_phase22_flashcard_system)

add_executable(test_phase23_task_calendar
    unit/test_phase23_task_calendar.cpp
)
target_include_directories(test_phase23_task_calendar PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase23_task_calendar PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase23_task_calendar COMMAND test_phase23_task_calendar)

add_executable(test_phase24_export_publishing
    unit/test_phase24_export_publishing.cpp
)
target_include_directories(test_phase24_export_publishing PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase24_export_publishing PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase24_export_publishing COMMAND test_phase24_export_publishing)

# Phase 25: Version Control Integration
add_executable(test_phase25_version_control
    unit/test_phase25_version_control.cpp
)
target_include_directories(test_phase25_version_control PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase25_version_control PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase25_version_control COMMAND test_phase25_version_control)

add_executable(test_phase26_ai_integration
    unit/test_phase26_ai_integration.cpp
)
target_include_directories(test_phase26_ai_integration PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase26_ai_integration PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase26_ai_integration COMMAND test_phase26_ai_integration)

# Phase 27: Cloud Sync & Collaboration
add_executable(test_phase27_cloud_sync
    unit/test_phase27_cloud_sync.cpp
)
target_include_directories(test_phase27_cloud_sync PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase27_cloud_sync PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase27_cloud_sync COMMAND test_phase27_cloud_sync)

# Phase 34: Testing Completeness — integration test
add_executable(test_phase34_integration
    integration/test_workspace_lifecycle.cpp
)
target_include_directories(test_phase34_integration PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase34_integration PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase34_integration COMMAND test_phase34_integration)

# Phase 34: Testing Completeness — panel rendering tests
add_executable(test_phase34_panels
    unit/test_panel_rendering.cpp
)
target_include_directories(test_phase34_panels PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase34_panels PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase34_panels COMMAND test_phase34_panels)

# Phase 34: Testing Completeness — rendering pipeline tests
add_executable(test_phase34_rendering
    unit/test_rendering_pipeline.cpp
)
target_include_directories(test_phase34_rendering PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase34_rendering PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase34_rendering COMMAND test_phase34_rendering)

# Phase 34 Batch 34B: Canvas objects tests
add_executable(test_phase34_canvas
    unit/test_canvas_objects.cpp
)
target_include_directories(test_phase34_canvas PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase34_canvas PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase34_canvas COMMAND test_phase34_canvas)

# Phase 34 Batch 34B: EventBus stress tests
add_executable(test_phase34_eventbus
    unit/test_eventbus_stress.cpp
)
target_include_directories(test_phase34_eventbus PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase34_eventbus PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase34_eventbus COMMAND test_phase34_eventbus)

# Phase 34 Batch 34B: Config system tests
add_executable(test_phase34_config
    unit/test_config_system.cpp
)
target_include_directories(test_phase34_config PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase34_config PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase34_config COMMAND test_phase34_config)

# Phase 34 Batch 34B: Plugin lifecycle tests
add_executable(test_phase34_plugin
    unit/test_plugin_lifecycle.cpp
)
target_include_directories(test_phase34_plugin PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase34_plugin PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase34_plugin COMMAND test_phase34_plugin)

# Phase 34 Batch 34C: Theme system tests
add_executable(test_phase34_theme
    unit/test_theme_system.cpp
)
target_include_directories(test_phase34_theme PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase34_theme PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase34_theme COMMAND test_phase34_theme)

# Phase 34 Batch 34C: Search system tests
add_executable(test_phase34_search
    unit/test_search_system.cpp
)
target_include_directories(test_phase34_search PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase34_search PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase34_search COMMAND test_phase34_search)

# Phase 34 Batch 34C: Serialization round-trip tests
add_executable(test_phase34_serial
    unit/test_serialization.cpp
)
target_include_directories(test_phase34_serial PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase34_serial PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase34_serial COMMAND test_phase34_serial)

# Phase 34 Batch 34C: Error handling tests
add_executable(test_phase34_error
    unit/test_error_handling.cpp
)
target_include_directories(test_phase34_error PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase34_error PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase34_error COMMAND test_phase34_error)

# Phase 34 Batch 34C: Memory leak tests
add_executable(test_phase34_memory
    unit/test_memory_leaks.cpp
)
target_include_directories(test_phase34_memory PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase34_memory PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase34_memory COMMAND test_phase34_memory)

# Phase 34 Batch 34C: Thread safety tests
add_executable(test_phase34_thread
    unit/test_thread_safety.cpp
)
target_include_directories(test_phase34_thread PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase34_thread PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase34_thread COMMAND test_phase34_thread)

# Phase 34 Batch 34D: Regression tests (known issues)
add_executable(test_phase34_regression
    regression/test_known_issues.cpp
)
target_include_directories(test_phase34_regression PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase34_regression PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase34_regression COMMAND test_phase34_regression)

# Phase 34 Batch 34D: Property-based tests
add_executable(test_phase34_properties
    unit/test_properties.cpp
)
target_include_directories(test_phase34_properties PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase34_properties PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase34_properties COMMAND test_phase34_properties)

# Phase 34 Batch 34D: Fuzz stub — markdown parser
add_executable(test_phase34_fuzz_parser
    fuzz/fuzz_markdown_parser.cpp
)
target_include_directories(test_phase34_fuzz_parser PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase34_fuzz_parser PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase34_fuzz_parser COMMAND test_phase34_fuzz_parser)

# Phase 34 Batch 34D: Fuzz stub — board serializer
add_executable(test_phase34_fuzz_board
    fuzz/fuzz_board_serializer.cpp
)
target_include_directories(test_phase34_fuzz_board PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase34_fuzz_board PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase34_fuzz_board COMMAND test_phase34_fuzz_board)

# Phase 34 Batch 34D: Core benchmark suite
add_executable(test_phase34_bench
    benchmark/bench_core.cpp
)
target_include_directories(test_phase34_bench PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase34_bench PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase34_bench COMMAND test_phase34_bench)

# Phase 35: Documentation & Help System tests
add_executable(test_phase35_help_system
    unit/test_phase35_help_system.cpp
)
target_include_directories(test_phase35_help_system PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase35_help_system PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase35_help_system COMMAND test_phase35_help_system)

# V9 Phase 36: Command System Completion
add_executable(test_phase36_command_system
    unit/test_phase36_command_system.cpp
)
target_include_directories(test_phase36_command_system PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase36_command_system PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase36_command_system COMMAND test_phase36_command_system)

# V9 Phase 37: Bookmark & Asset Management
add_executable(test_phase37_bookmark_asset
    unit/test_phase37_bookmark_asset.cpp
)
target_include_directories(test_phase37_bookmark_asset PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase37_bookmark_asset PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase37_bookmark_asset COMMAND test_phase37_bookmark_asset)

# V9 Phase 38: Vault Style & Workspace Customization
add_executable(test_phase38_vault_workspace
    unit/test_phase38_vault_workspace.cpp
)
target_include_directories(test_phase38_vault_workspace PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase38_vault_workspace PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase38_vault_workspace COMMAND test_phase38_vault_workspace)

# V9 Phase 39: Notification & Activity System
add_executable(test_phase39_notification_activity
    unit/test_phase39_notification_activity.cpp
)
target_include_directories(test_phase39_notification_activity PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase39_notification_activity PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase39_notification_activity COMMAND test_phase39_notification_activity)

# V9 Phase 40: Workspace Automation & Hooks
add_executable(test_phase40_automation_hooks
    unit/test_phase40_automation_hooks.cpp
)
target_include_directories(test_phase40_automation_hooks PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase40_automation_hooks PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase40_automation_hooks COMMAND test_phase40_automation_hooks)

# V9 Phase 41: Widget & Sidebar Extensions
add_executable(test_phase41_widget_sidebar
    unit/test_phase41_widget_sidebar.cpp
)
target_include_directories(test_phase41_widget_sidebar PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase41_widget_sidebar PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase41_widget_sidebar COMMAND test_phase41_widget_sidebar)

# V9 Phase 42: PDF Viewer & Document Import
add_executable(test_phase42_pdf_import
    unit/test_phase42_pdf_import.cpp
)
target_include_directories(test_phase42_pdf_import PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase42_pdf_import PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase42_pdf_import COMMAND test_phase42_pdf_import)

# V9 Phase 43: Presentation & Slide System
add_executable(test_phase43_presentation
    unit/test_phase43_presentation.cpp
)
target_include_directories(test_phase43_presentation PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase43_presentation PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase43_presentation COMMAND test_phase43_presentation)

# V9 Phase 44: Encryption & Privacy
add_executable(test_phase44_encryption_privacy
    unit/test_phase44_encryption_privacy.cpp
)
target_include_directories(test_phase44_encryption_privacy PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase44_encryption_privacy PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase44_encryption_privacy COMMAND test_phase44_encryption_privacy)

# V9 Phase 46: Multi-Window & Workspace Management
add_executable(test_phase46_multi_window
    unit/test_phase46_multi_window.cpp
)
target_include_directories(test_phase46_multi_window PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase46_multi_window PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase46_multi_window COMMAND test_phase46_multi_window)

# V9 Phase 47: Localization & Internationalization
add_executable(test_phase47_localization
    unit/test_phase47_localization.cpp
)
target_include_directories(test_phase47_localization PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase47_localization PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase47_localization COMMAND test_phase47_localization)

# V9 Phase 48: Data Processing & Computation
add_executable(test_phase48_data_processing
    unit/test_phase48_data_processing.cpp
)
target_include_directories(test_phase48_data_processing PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase48_data_processing PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase48_data_processing COMMAND test_phase48_data_processing)

# V9 Phase 49: End-to-End Integration Testing
add_executable(test_phase49_integration_testing
    unit/test_phase49_integration_testing.cpp
)
target_include_directories(test_phase49_integration_testing PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase49_integration_testing PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase49_integration_testing COMMAND test_phase49_integration_testing)

# V9 Phase 50: Release Preparation & Polish
add_executable(test_phase50_release_preparation
    unit/test_phase50_release_preparation.cpp
)
target_include_directories(test_phase50_release_preparation PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase50_release_preparation PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase50_release_preparation COMMAND test_phase50_release_preparation)

# V10 Phase 01: Control Interaction Standards
add_executable(test_control_interaction_contracts
    unit/test_control_interaction_contracts.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ControlState.cpp
)
target_include_directories(test_control_interaction_contracts PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_control_interaction_contracts PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_control_interaction_contracts COMMAND test_control_interaction_contracts)

# V10 Phase 02: Control Visual Tokens and State Styling
add_executable(test_theme_control_tokens
    unit/test_theme_control_tokens.cpp
)
target_include_directories(test_theme_control_tokens PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_theme_control_tokens PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_theme_control_tokens COMMAND test_theme_control_tokens)

# V10 Phase 03: Layout Density and Spacing System
add_executable(test_density_hit_targets
    unit/test_density_hit_targets.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/LayoutMetrics.cpp
)
target_include_directories(test_density_hit_targets PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_density_hit_targets PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_density_hit_targets COMMAND test_density_hit_targets)

# V10 Phase 04: Unified Input and Gesture Model
add_executable(test_input_model_contracts
    unit/test_input_model_contracts.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/InputModel.cpp
)
target_include_directories(test_input_model_contracts PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_input_model_contracts PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_input_model_contracts COMMAND test_input_model_contracts)

# V10 Phase 05: Focus and Keyboard Navigation Framework
add_executable(test_keyboard_navigation_controls
    unit/test_keyboard_navigation_controls.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/FocusManager.cpp
)
target_include_directories(test_keyboard_navigation_controls PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_keyboard_navigation_controls PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_keyboard_navigation_controls COMMAND test_keyboard_navigation_controls)

# V10 Phase 06: Activity Bar Control Evolution
add_executable(test_activity_bar_accessibility
    unit/test_activity_bar_accessibility.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ActivityBarModel.cpp
)
target_include_directories(test_activity_bar_accessibility PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_activity_bar_accessibility PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_activity_bar_accessibility COMMAND test_activity_bar_accessibility)

# V10 Phase 07: Explorer and File Tree Control Refinement
add_executable(test_file_tree_controls
    unit/test_file_tree_controls.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/FileTreeModel.cpp
)
target_include_directories(test_file_tree_controls PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_file_tree_controls PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_file_tree_controls COMMAND test_file_tree_controls)

# V10 Phase 08: Tab Strip Interaction and Management UX
add_executable(test_tab_bar_interactions
    unit/test_tab_bar_interactions.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/TabStripModel.cpp
)
target_include_directories(test_tab_bar_interactions PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_tab_bar_interactions PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_tab_bar_interactions COMMAND test_tab_bar_interactions)

# V10 Phase 09: Toolbar Action Surface Redesign
add_executable(test_toolbar_behavior
    unit/test_toolbar_behavior.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ToolbarModel.cpp
)
target_include_directories(test_toolbar_behavior PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_toolbar_behavior PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_toolbar_behavior COMMAND test_toolbar_behavior)

# V10 Phase 10: Command Palette Information Architecture
add_executable(test_command_palette_ranking
    unit/test_command_palette_ranking.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/CommandPaletteModel.cpp
)
target_include_directories(test_command_palette_ranking PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_command_palette_ranking PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_command_palette_ranking COMMAND test_command_palette_ranking)

# V10 Phase 11: Settings Navigation and Discovery Redesign
add_executable(test_settings_navigation
    unit/test_settings_navigation.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SettingsNavModel.cpp
)
target_include_directories(test_settings_navigation PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_settings_navigation PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_settings_navigation COMMAND test_settings_navigation)

# V10 Phase 12: Settings Control Primitives and Validation UX
add_executable(test_settings_control_types
    unit/test_settings_control_types.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SettingsControlModel.cpp
)
target_include_directories(test_settings_control_types PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_settings_control_types PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_settings_control_types COMMAND test_settings_control_types)

# V10 Phase 13: Status Bar as Actionable Control Rail
add_executable(test_status_bar_actions
    unit/test_status_bar_actions.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/StatusBarModel.cpp
)
target_include_directories(test_status_bar_actions PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_status_bar_actions PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_status_bar_actions COMMAND test_status_bar_actions)

# V10 Phase 14: Breadcrumb and Context Navigation Controls
add_executable(test_breadcrumb_navigation
    unit/test_breadcrumb_navigation.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/BreadcrumbModel.cpp
)
target_include_directories(test_breadcrumb_navigation PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_breadcrumb_navigation PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_breadcrumb_navigation COMMAND test_breadcrumb_navigation)

add_executable(test_breadcrumb_bar
    unit/test_breadcrumb_bar.cpp
)
target_include_directories(test_breadcrumb_bar PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_breadcrumb_bar PRIVATE
    Catch2::Catch2WithMain
    markamp_core
    ${wxWidgets_LIBRARIES}
)
add_test(NAME test_breadcrumb_bar COMMAND test_breadcrumb_bar)

# V10 Phase 15: Pane Splitter and Resizing Control Improvements
add_executable(test_splitter_behavior
    unit/test_splitter_behavior.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SplitterModel.cpp
)
target_include_directories(test_splitter_behavior PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_splitter_behavior PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_splitter_behavior COMMAND test_splitter_behavior)

# V10 Phase 16: Context Menu Consistency Program
add_executable(test_context_menu_enablement
    unit/test_context_menu_enablement.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ContextMenuModel.cpp
)
target_include_directories(test_context_menu_enablement PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_context_menu_enablement PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_context_menu_enablement COMMAND test_context_menu_enablement)

# V10 Phase 17: Shortcut Discovery and Cheat Sheet UX
add_executable(test_shortcut_overlay
    unit/test_shortcut_overlay.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ShortcutOverlayModel.cpp
)
target_include_directories(test_shortcut_overlay PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_shortcut_overlay PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_shortcut_overlay COMMAND test_shortcut_overlay)

# V10 Phase 18: Notification and Action Center UX
add_executable(test_notification_manager
    unit/test_notification_manager.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/NotificationModel.cpp
)
target_include_directories(test_notification_manager PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_notification_manager PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_notification_manager COMMAND test_notification_manager)

# V10 Phase 19: Dialogs and Confirmation Control Patterns
add_executable(test_dialog_patterns
    unit/test_dialog_patterns.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/DialogModel.cpp
)
target_include_directories(test_dialog_patterns PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_dialog_patterns PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_dialog_patterns COMMAND test_dialog_patterns)

# V10 Phase 20: Startup and Onboarding Control Experience
add_executable(test_startup_controls
    unit/test_startup_controls.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/StartupModel.cpp
)
target_include_directories(test_startup_controls PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_startup_controls PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_startup_controls COMMAND test_startup_controls)

# V10 Phase 21: Search Panel Control Surface Completion
add_executable(test_search_panel
    unit/test_search_panel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SearchPanelModel.cpp
)
target_include_directories(test_search_panel PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_search_panel PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_search_panel COMMAND test_search_panel)

# V10 Phase 22: Find/Replace In-Editor Control Refinement
add_executable(test_editor_find_replace_controls
    unit/test_editor_find_replace_controls.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/FindReplaceModel.cpp
)
target_include_directories(test_editor_find_replace_controls PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_editor_find_replace_controls PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_editor_find_replace_controls COMMAND test_editor_find_replace_controls)

# V10 Phase 23: Editor Inline Control Overlays
add_executable(test_editor_overlays
    unit/test_editor_overlays.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/EditorOverlayModel.cpp
)
target_include_directories(test_editor_overlays PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_editor_overlays PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_editor_overlays COMMAND test_editor_overlays)

# V10 Phase 24: Preview Surface Control and Sync UX
add_executable(test_preview_controls
    unit/test_preview_controls.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/PreviewControlModel.cpp
)
target_include_directories(test_preview_controls PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_preview_controls PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_preview_controls COMMAND test_preview_controls)

# V10 Phase 25: Canvas Tooling and Inspector Control UX
add_executable(test_canvas_control_ux
    unit/test_canvas_control_ux.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/CanvasControlModel.cpp
)
target_include_directories(test_canvas_control_ux PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_control_ux PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_control_ux COMMAND test_canvas_control_ux)

# V10 Phase 26: Graph and Backlink Control UX
add_executable(test_graph_controls
    unit/test_graph_controls.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/GraphControlModel.cpp
)
target_include_directories(test_graph_controls PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_graph_controls PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_graph_controls COMMAND test_graph_controls)

# V10 Phase 27: Extensions Browser Control Experience
add_executable(test_extensions_browser_controls
    unit/test_extensions_browser_controls.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ExtensionBrowserModel.cpp
)
target_include_directories(test_extensions_browser_controls PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_extensions_browser_controls PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_extensions_browser_controls COMMAND test_extensions_browser_controls)

# V10 Phase 28: Theme Gallery and Customization Control UX
add_executable(test_theme_gallery_controls
    unit/test_theme_gallery_controls.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ThemeGalleryModel.cpp
)
target_include_directories(test_theme_gallery_controls PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_theme_gallery_controls PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_theme_gallery_controls COMMAND test_theme_gallery_controls)

# V10 Phase 29: Tool Windows, Output and Problems Control UX
add_executable(test_tool_window_controls
    unit/test_tool_window_controls.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ToolWindowModel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ToolWindowTabControl.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/PanelLifecycleManager.cpp
)
target_include_directories(test_tool_window_controls PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_tool_window_controls PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_tool_window_controls COMMAND test_tool_window_controls)

# V10 Phase 30: Cross-Surface Navigation Control Unification
add_executable(test_cross_surface_navigation_controls
    unit/test_cross_surface_navigation_controls.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/NavigationModel.cpp
)
target_include_directories(test_cross_surface_navigation_controls PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_cross_surface_navigation_controls PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_cross_surface_navigation_controls COMMAND test_cross_surface_navigation_controls)

# V10 Phase 31: Motion, Microinteraction, and Control Feedback
add_executable(test_control_motion_budget
    unit/test_control_motion_budget.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/MotionModel.cpp
)
target_include_directories(test_control_motion_budget PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_control_motion_budget PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_control_motion_budget COMMAND test_control_motion_budget)

# V10 Phase 32: Drag Drop and Manipulation Affordance Improvements
add_executable(test_drag_drop_controls
    unit/test_drag_drop_controls.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/DragDropModel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/DragAffordanceController.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/DropTargetHighlighter.cpp
)
target_include_directories(test_drag_drop_controls PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_drag_drop_controls PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_drag_drop_controls COMMAND test_drag_drop_controls)

# V10 Phase 33: Multi-Select and Bulk Action Control Patterns
add_executable(test_multi_select_controls
    unit/test_multi_select_controls.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/MultiSelectModel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/BulkActionController.cpp
)
target_include_directories(test_multi_select_controls PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_multi_select_controls PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_multi_select_controls COMMAND test_multi_select_controls)

# V10 Phase 34: Error Prevention and Recovery Control UX
add_executable(test_control_error_recovery
    unit/test_control_error_recovery.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/PreflightValidator.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/RecoveryActionProvider.cpp
)
target_include_directories(test_control_error_recovery PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_control_error_recovery PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_control_error_recovery COMMAND test_control_error_recovery)

# V10 Phase 35: Accessibility and Assistive Control Semantics
add_executable(test_control_accessibility_semantics
    unit/test_control_accessibility_semantics.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/AccessibilitySemanticsController.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ScreenReaderAnnouncer.cpp
)
target_include_directories(test_control_accessibility_semantics PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_control_accessibility_semantics PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_control_accessibility_semantics COMMAND test_control_accessibility_semantics)

# Phase 05: Accessibility Tasks 22-25
add_executable(test_phase05_accessibility_audit
    unit/test_phase05_accessibility_audit.cpp
)
target_include_directories(test_phase05_accessibility_audit PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase05_accessibility_audit PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_phase05_accessibility_audit COMMAND test_phase05_accessibility_audit)

# V10 Phase 36: Responsive, DPI, and Platform-Adaptive Controls
add_executable(test_control_responsiveness
    unit/test_control_responsiveness.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/DpiScalingController.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/CompactLayoutModel.cpp
)
target_include_directories(test_control_responsiveness PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_control_responsiveness PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_control_responsiveness COMMAND test_control_responsiveness)

# V10 Phase 37: Personalization and Layout Preset Controls
add_executable(test_layout_personalization_controls
    unit/test_layout_personalization_controls.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/LayoutPresetController.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ControlVisibilityPrefs.cpp
)
target_include_directories(test_layout_personalization_controls PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_layout_personalization_controls PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_layout_personalization_controls COMMAND test_layout_personalization_controls)

# V10 Phase 38: UX Instrumentation and Control Quality Metrics
add_executable(test_control_metrics_instrumentation
    unit/test_control_metrics_instrumentation.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/UxMetricsCollector.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/TelemetryPrivacyController.cpp
)
target_include_directories(test_control_metrics_instrumentation PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_control_metrics_instrumentation PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_control_metrics_instrumentation COMMAND test_control_metrics_instrumentation)

# V10 Phase 39: Control Regression Harness and UX QA Automation
add_executable(test_control_regression_harness
    unit/test_control_regression_harness.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ControlSnapshotTester.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/KeyboardWorkflowRunner.cpp
)
target_include_directories(test_control_regression_harness PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_control_regression_harness PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_control_regression_harness COMMAND test_control_regression_harness)

# V10 Phase 40: Final Control UX Audit and Release Gates
add_executable(test_release_audit_gates
    unit/test_release_audit_gates.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/UxAuditRunner.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ReleaseGateChecker.cpp
)
target_include_directories(test_release_audit_gates PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_release_audit_gates PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_release_audit_gates COMMAND test_release_audit_gates)

# V10 Phase 41: Canvas Input Model and Tool State Machine
add_executable(test_canvas_input_state_machine
    unit/test_canvas_input_state_machine.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasToolStateMachine.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/PointerEventRouter.cpp
)
target_include_directories(test_canvas_input_state_machine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_input_state_machine PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_input_state_machine COMMAND test_canvas_input_state_machine)

# V10 Phase 42: Freehand Drawing Engine Quality
add_executable(test_canvas_freehand
    unit/test_canvas_freehand.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/StrokeSmoother.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/StrokeStylePreset.cpp
)
target_include_directories(test_canvas_freehand PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_freehand PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_freehand COMMAND test_canvas_freehand)

# V10 Phase 43: Shape Tools and Geometry Editing
add_executable(test_canvas_shapes
    unit/test_canvas_shapes.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/ShapePrimitiveSet.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/GeometryHandleController.cpp
)
target_include_directories(test_canvas_shapes PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_shapes PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_shapes COMMAND test_canvas_shapes)

# V10 Phase 44: Connector Routing and Line Editing Fundamentals
add_executable(test_canvas_connectors
    unit/test_canvas_connectors.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/ConnectorAnchorModel.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/ConnectorRouteEngine.cpp
)
target_include_directories(test_canvas_connectors PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_connectors PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_connectors COMMAND test_canvas_connectors)

# V10 Phase 45: Text Editing and Typography on Canvas
add_executable(test_canvas_text
    unit/test_canvas_text.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasTextEditor.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/TextStylePreset.cpp
)
target_include_directories(test_canvas_text PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_text PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_text COMMAND test_canvas_text)

# V10 Phase 46: Canvas Color System and Palette Controls
add_executable(test_canvas_color_system
    unit/test_canvas_color_system.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasColorToken.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/ColorPaletteController.cpp
)
target_include_directories(test_canvas_color_system PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_color_system PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_color_system COMMAND test_canvas_color_system)

# V10 Phase 47: Stroke Fill Border and Effects Controls
add_executable(test_canvas_style_rendering
    unit/test_canvas_style_rendering.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/StrokeBorderStyle.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/FillEffectsController.cpp
)
target_include_directories(test_canvas_style_rendering PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_style_rendering PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_style_rendering COMMAND test_canvas_style_rendering)

# V10 Phase 48: Selection Transform and Alignment Core
add_executable(test_canvas_selection_transform
    unit/test_canvas_selection_transform.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/SelectionSemantics.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/TransformAlignController.cpp
)
target_include_directories(test_canvas_selection_transform PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_selection_transform PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_selection_transform COMMAND test_canvas_selection_transform)

# V10 Phase 49: Snapping Grid and Guide Precision
add_executable(test_canvas_snapping
    unit/test_canvas_snapping.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/SnapTargetEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/GridGuideController.cpp
)
target_include_directories(test_canvas_snapping PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_snapping PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_snapping COMMAND test_canvas_snapping)

# V10 Phase 50: Layering Z-Order and Locking Fundamentals
add_executable(test_canvas_layering
    unit/test_canvas_layering.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/LayeringModel.cpp
)
target_include_directories(test_canvas_layering PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_layering PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_layering COMMAND test_canvas_layering)

# V10 Phase 51: Frames Sections and Container Workflows
add_executable(test_canvas_frames_sections
    unit/test_canvas_frames_sections.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/FrameContainerModel.cpp
)
target_include_directories(test_canvas_frames_sections PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_frames_sections PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_frames_sections COMMAND test_canvas_frames_sections)

# V10 Phase 52: Sticky Notes and Quick Idea Capture
add_executable(test_canvas_sticky_notes
    unit/test_canvas_sticky_notes.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/StickyNoteModel.cpp
)
target_include_directories(test_canvas_sticky_notes PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_sticky_notes PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_sticky_notes COMMAND test_canvas_sticky_notes)

# V10 Phase 53: Image Asset Workflows and Rendering
add_executable(test_canvas_images
    unit/test_canvas_images.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/ImageAssetModel.cpp
)
target_include_directories(test_canvas_images PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_images PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_images COMMAND test_canvas_images)

# V10 Phase 54: Video and Audio Embed Fundamentals
add_executable(test_canvas_media_embeds
    unit/test_canvas_media_embeds.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/MediaEmbedModel.cpp
)
target_include_directories(test_canvas_media_embeds PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_media_embeds PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_media_embeds COMMAND test_canvas_media_embeds)

# V10 Phase 55: PDF Object Controls and Page Workflows
add_executable(test_canvas_pdf_objects
    unit/test_canvas_pdf_objects.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/PdfObjectModel.cpp
)
target_include_directories(test_canvas_pdf_objects PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_pdf_objects PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_pdf_objects COMMAND test_canvas_pdf_objects)

# V10 Phase 56: Table Object Editing and Data Formatting
add_executable(test_canvas_tables
    unit/test_canvas_tables.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/TableModel.cpp
)
target_include_directories(test_canvas_tables PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_tables PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_tables COMMAND test_canvas_tables)

# V10 Phase 57: Diagram Library and Stencil System
add_executable(test_canvas_stencils
    unit/test_canvas_stencils.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/StencilModel.cpp
)
target_include_directories(test_canvas_stencils PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_stencils PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_stencils COMMAND test_canvas_stencils)

# V10 Phase 58: Mind Map Core Editing Experience
add_executable(test_canvas_mindmap
    unit/test_canvas_mindmap.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/MindMapModel.cpp
)
target_include_directories(test_canvas_mindmap PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_mindmap PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_mindmap COMMAND test_canvas_mindmap)

# V10 Phase 59: Kanban Object Fundamentals on Canvas
add_executable(test_canvas_kanban
    unit/test_canvas_kanban.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/KanbanModel.cpp
)
target_include_directories(test_canvas_kanban PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_kanban PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_kanban COMMAND test_canvas_kanban)

# V10 Phase 60: Canvas Inspector and Properties Panel
add_executable(test_canvas_inspector
    unit/test_canvas_inspector.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/InspectorModel.cpp
)
target_include_directories(test_canvas_inspector PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_inspector PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_inspector COMMAND test_canvas_inspector)

# V10 Phase 61: Canvas Tool Rail and Toolbar Redesign
add_executable(test_canvas_tool_rail
    unit/test_canvas_tool_rail.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/ToolRailModel.cpp
)
target_include_directories(test_canvas_tool_rail PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_tool_rail PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_tool_rail COMMAND test_canvas_tool_rail)

# V10 Phase 62: Canvas Context Menus and Quick Actions
add_executable(test_canvas_context_actions
    unit/test_canvas_context_actions.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/ContextMenuModel.cpp
)
target_include_directories(test_canvas_context_actions PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_context_actions PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_context_actions COMMAND test_canvas_context_actions)

# V10 Phase 63: Canvas Search Filter and Object Discovery
add_executable(test_canvas_search_model
    unit/test_canvas_search.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasSearchModel.cpp
)
target_include_directories(test_canvas_search_model PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_search_model PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_search_model COMMAND test_canvas_search_model)

# V10 Phase 64: Minimap and Board Navigator Improvements
add_executable(test_canvas_minimap
    unit/test_canvas_minimap.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/MinimapModel.cpp
)
target_include_directories(test_canvas_minimap PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_minimap PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_minimap COMMAND test_canvas_minimap)

# V10 Phase 65: Pan Zoom and Camera Ergonomics
add_executable(test_canvas_camera
    unit/test_canvas_camera.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CameraModel.cpp
)
target_include_directories(test_canvas_camera PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_camera PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_camera COMMAND test_canvas_camera)

# V10 Phase 66: Clipboard Duplicate and Template Workflows
add_executable(test_canvas_clipboard_templates
    unit/test_canvas_clipboard_templates.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/ClipboardModel.cpp
)
target_include_directories(test_canvas_clipboard_templates PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_clipboard_templates PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_clipboard_templates COMMAND test_canvas_clipboard_templates)

# V10 Phase 67: Undo Redo Transaction System for Canvas
add_executable(test_canvas_undo_redo
    unit/test_canvas_undo_redo.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/UndoRedoModel.cpp
)
target_include_directories(test_canvas_undo_redo PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_undo_redo PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_undo_redo COMMAND test_canvas_undo_redo)

# V10 Phase 68: Board Serialization Recovery and Compatibility
add_executable(test_canvas_board_serializer
    unit/test_canvas_board_serializer.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/BoardSerializerModel.cpp
)
target_include_directories(test_canvas_board_serializer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_board_serializer PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_board_serializer COMMAND test_canvas_board_serializer)

# V10 Phase 69: Large Board Rendering and Object Virtualization
add_executable(test_canvas_large_board_performance
    unit/test_canvas_large_board_performance.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/LargeBoardModel.cpp
)
target_include_directories(test_canvas_large_board_performance PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_large_board_performance PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_large_board_performance COMMAND test_canvas_large_board_performance)

# V10 Phase 70: Hit Testing Precision and Interaction Latency
add_executable(test_canvas_hit_testing
    unit/test_canvas_hit_testing.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/HitTestModel.cpp
)
target_include_directories(test_canvas_hit_testing PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_hit_testing PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_hit_testing COMMAND test_canvas_hit_testing)

# V10 Phase 71: Canvas Export Print and Snapshot Fundamentals
add_executable(test_canvas_export
    unit/test_canvas_export.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/ExportModel.cpp
)
target_include_directories(test_canvas_export PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_export PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_export COMMAND test_canvas_export)

# V10 Phase 72: Keyboard-First Canvas Command Coverage
add_executable(test_canvas_keyboard_commands
    unit/test_canvas_keyboard_commands.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/KeyboardCommandModel.cpp
)
target_include_directories(test_canvas_keyboard_commands PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_keyboard_commands PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_keyboard_commands COMMAND test_canvas_keyboard_commands)

# V10 Phase 73: Canvas Accessibility and Assistive Navigation
add_executable(test_canvas_accessibility
    unit/test_canvas_accessibility.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/AccessibilityModel.cpp
)
target_include_directories(test_canvas_accessibility PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_accessibility PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_accessibility COMMAND test_canvas_accessibility)

# V10 Phase 74: Canvas Theming and Visual Consistency
add_executable(test_canvas_theming
    unit/test_canvas_theming.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/CanvasThemeModel.cpp
)
target_include_directories(test_canvas_theming PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_theming PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_theming COMMAND test_canvas_theming)

# V10 Phase 75: Smart Layout and Auto Arrange Fundamentals
add_executable(test_canvas_auto_layout
    unit/test_canvas_auto_layout.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/AutoLayoutModel.cpp
)
target_include_directories(test_canvas_auto_layout PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_auto_layout PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_auto_layout COMMAND test_canvas_auto_layout)

# V10 Phase 76: Canvas Validation and Data Integrity Guards
add_executable(test_canvas_integrity
    unit/test_canvas_integrity.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/IntegrityModel.cpp
)
target_include_directories(test_canvas_integrity PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_integrity PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_integrity COMMAND test_canvas_integrity)

# V10 Phase 77: Canvas Test Automation and Benchmark Suite
add_executable(test_canvas_benchmark
    unit/test_canvas_benchmark.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/BenchmarkModel.cpp
)
target_include_directories(test_canvas_benchmark PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_benchmark PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_benchmark COMMAND test_canvas_benchmark)

# V10 Phase 78: Canvas Onboarding Help and Fundamentals Tutorials
add_executable(test_canvas_onboarding
    unit/test_canvas_onboarding.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/OnboardingModel.cpp
)
target_include_directories(test_canvas_onboarding PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_onboarding PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_onboarding COMMAND test_canvas_onboarding)

# V10 Phase 79: Canvas Polish Microinteractions and Control Feel
add_executable(test_canvas_polish_contracts
    unit/test_canvas_polish_contracts.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/PolishModel.cpp
)
target_include_directories(test_canvas_polish_contracts PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_polish_contracts PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_polish_contracts COMMAND test_canvas_polish_contracts)

# V10 Phase 80: Final Canvas Fundamentals Audit and Miro-Class Gates
add_executable(test_canvas_audit
    unit/test_canvas_audit.cpp
    ${CMAKE_SOURCE_DIR}/src/canvas/AuditModel.cpp
)
target_include_directories(test_canvas_audit PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_canvas_audit PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_canvas_audit COMMAND test_canvas_audit)

# V11 Node Editor core tests
add_executable(test_node_editor_core
    unit/test_node_editor_core.cpp
)
target_include_directories(test_node_editor_core PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_node_editor_core PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_node_editor_core COMMAND test_node_editor_core)

# V11 Node Editor Tier 2 tests
add_executable(test_node_editor_tier2
    unit/test_node_editor_tier2.cpp
)
target_include_directories(test_node_editor_tier2 PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/tests/unit
)
target_link_libraries(test_node_editor_tier2 PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_node_editor_tier2 COMMAND test_node_editor_tier2)

add_executable(test_node_editor_tier3
    unit/test_node_editor_tier3.cpp
)
target_include_directories(test_node_editor_tier3 PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/tests/unit
)
target_link_libraries(test_node_editor_tier3 PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_node_editor_tier3 COMMAND test_node_editor_tier3)

add_executable(test_node_editor_tier4
    unit/test_node_editor_tier4.cpp
)
target_include_directories(test_node_editor_tier4 PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/tests/unit
)
target_link_libraries(test_node_editor_tier4 PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_node_editor_tier4 COMMAND test_node_editor_tier4)

add_executable(test_node_editor_tier5
    unit/test_node_editor_tier5.cpp
)
target_include_directories(test_node_editor_tier5 PRIVATE
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/tests/unit
)
target_link_libraries(test_node_editor_tier5 PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_node_editor_tier5 COMMAND test_node_editor_tier5)

# ============================================================================
# Icon Library test
# ============================================================================
add_executable(test_icon_manager
    unit/test_icon_manager.cpp
)
target_include_directories(test_icon_manager PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_icon_manager PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_icon_manager COMMAND test_icon_manager)
add_executable(test_phase05_keyboard_mode unit/test_phase05_keyboard_mode.cpp)
target_link_libraries(test_phase05_keyboard_mode PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_phase05_keyboard_mode COMMAND test_phase05_keyboard_mode)

add_executable(test_phase06_workbench_zones unit/test_phase06_workbench_zones.cpp)
target_link_libraries(test_phase06_workbench_zones PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_phase06_workbench_zones COMMAND test_phase06_workbench_zones)

# Phase 07
add_executable(test_phase07_activity_bar 
    unit/test_phase07_activity_bar.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ActivityBarModel.cpp
)
target_link_libraries(test_phase07_activity_bar PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_phase07_activity_bar COMMAND test_phase07_activity_bar)

add_executable(test_phase08_sidebar 
    unit/test_phase08_sidebar.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SidebarHeader.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SidebarSection.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/EmptyPanelState.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SidebarFooter.cpp
)
target_link_libraries(test_phase08_sidebar PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_phase08_sidebar COMMAND test_phase08_sidebar)

# Phase 14: Editor Gutter and Decorations
add_executable(test_phase14_gutter unit/test_phase14_gutter.cpp)
target_include_directories(test_phase14_gutter PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase14_gutter PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_phase14_gutter COMMAND test_phase14_gutter)

# Phase 22: Output Panel V2
add_executable(test_output_panel_v2 unit/test_output_panel_v2.cpp)
target_include_directories(test_output_panel_v2 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_output_panel_v2 PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_output_panel_v2 COMMAND test_output_panel_v2)

# Phase 24: Debug Console
add_executable(test_debug_console unit/test_debug_console.cpp)
target_include_directories(test_debug_console PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_debug_console PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_debug_console COMMAND test_debug_console)

# Phase 25: Compiler Error Parser, TaskConfig, BuildService
add_executable(test_compiler_error_parser unit/test_compiler_error_parser.cpp)
target_include_directories(test_compiler_error_parser PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_compiler_error_parser PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_compiler_error_parser COMMAND test_compiler_error_parser)

# Phase 23: Problems Tree Model
add_executable(test_problems_tree_model unit/test_problems_tree_model.cpp)
target_include_directories(test_problems_tree_model PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_problems_tree_model PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_problems_tree_model COMMAND test_problems_tree_model)

# Phase 26: Toolbar V2 (RunConfigService, NotificationService stored, responsive collapse, run flow)
add_executable(test_toolbar_v2
    unit/test_toolbar_v2.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ToolbarModel.cpp
    ${CMAKE_SOURCE_DIR}/src/core/RunConfigService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/NotificationService.cpp
)
target_include_directories(test_toolbar_v2 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_toolbar_v2 PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_toolbar_v2 COMMAND test_toolbar_v2)

# Phase 27: Panel Header Bar (PanelHeaderBar, PanelHeaderIcons per-panel action definitions)
add_executable(test_panel_header_bar
    unit/test_panel_header_bar.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/PanelHeaderIcons.cpp
)
target_include_directories(test_panel_header_bar PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_panel_header_bar PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_panel_header_bar COMMAND test_panel_header_bar)

# Phase 28: Context Menu System V2 (Builder, Registry, built-in menus)
add_executable(test_context_menu_v2
    unit/test_context_menu_v2.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ContextMenuModel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ContextMenuBuilder.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ContextMenuRegistry.cpp
)
target_include_directories(test_context_menu_v2 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_context_menu_v2 PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_context_menu_v2 COMMAND test_context_menu_v2)

# Phase 30: Navigation Commands (NavigationCommandSet, JumpToController)
add_executable(test_navigation_commands
    unit/test_navigation_commands.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/NavigationCommandSet.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/JumpToController.cpp
)
target_include_directories(test_navigation_commands PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_navigation_commands PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_navigation_commands COMMAND test_navigation_commands)

# Phase 31: Control Motion (ControlMotionTokens, ControlAnimator)
add_executable(test_control_motion
    unit/test_control_motion.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ControlMotionTokens.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ControlAnimator.cpp
)
target_include_directories(test_control_motion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_control_motion PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_control_motion COMMAND test_control_motion)

# Phase 29: Code Actions V2 (CodeActionSet, extended CodeActionProvider, LightbulbWidget)
add_executable(test_code_actions_v2
    unit/test_code_actions_v2.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/LightbulbWidget.cpp
)
target_include_directories(test_code_actions_v2 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_code_actions_v2 PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_code_actions_v2 COMMAND test_code_actions_v2)

# Phase 30: Floating Toolbars (FloatingToolbar base, FloatingToolbarManager, events)
add_executable(test_floating_toolbars
    unit/test_floating_toolbars.cpp
)
target_include_directories(test_floating_toolbars PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_floating_toolbars PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_floating_toolbars COMMAND test_floating_toolbars)

# Phase 31: Command Palette V2 (FuzzyScorer, FrecencyTracker, SymbolProvider, FileIconResolver, IconProvider)
add_executable(test_command_palette_v2
    unit/test_command_palette_v2.cpp
)
target_include_directories(test_command_palette_v2 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_command_palette_v2 PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_command_palette_v2 COMMAND test_command_palette_v2)

# Phase 32: Go-To System (NavigationService, LinkResolver, BacklinkFinder, ChangeTracker, WorkspaceSymbolIndex)
add_executable(test_goto_system
    unit/test_goto_system.cpp
)
target_include_directories(test_goto_system PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_goto_system PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_goto_system COMMAND test_goto_system)

# Phase 34: Find & Replace V2 (PreserveCaseReplacer, ReplaceUndoBuffer, FindBarState, SearchResultsModel)
add_executable(test_find_replace_v2
    unit/test_find_replace_v2.cpp
)
target_include_directories(test_find_replace_v2 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_find_replace_v2 PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_find_replace_v2 COMMAND test_find_replace_v2)

# Phase 35: Quick Open (RecentFilesManager, FileSearchScorer, FileIndex, TabSwitcherModel)
add_executable(test_quick_open
    unit/test_quick_open.cpp
)
target_include_directories(test_quick_open PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_quick_open PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_quick_open COMMAND test_quick_open)

# Phase 36: Settings UI Overhaul (RichDescriptionRenderer, SettingsCompareModel)
add_executable(test_settings_ui_v2
    unit/test_settings_ui_v2.cpp
)
target_include_directories(test_settings_ui_v2 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_settings_ui_v2 PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_settings_ui_v2 COMMAND test_settings_ui_v2)

# Phase 37: Compiler Configuration Panel
add_executable(test_compiler_config
    unit/test_compiler_config.cpp
)
target_include_directories(test_compiler_config PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_compiler_config PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_compiler_config COMMAND test_compiler_config)

# Phase 38: Build System Integration
add_executable(test_build_system
    unit/test_build_system.cpp
)
target_include_directories(test_build_system PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_build_system PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_build_system COMMAND test_build_system)

# Phase 39: Keyboard Shortcuts Editor
add_executable(test_shortcuts_editor
    unit/test_shortcuts_editor.cpp
)
target_include_directories(test_shortcuts_editor PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_shortcuts_editor PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_shortcuts_editor COMMAND test_shortcuts_editor)

# Phase 40: Workspace & Project Configuration
add_executable(test_workspace_config
    unit/test_workspace_config.cpp
)
target_include_directories(test_workspace_config PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_workspace_config PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_workspace_config COMMAND test_workspace_config)

# Phase 41: Component Library Polish
add_executable(test_component_library
    unit/test_component_library.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ComponentVariants.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ThemedTextInput.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ThemedDropdown.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ThemedCheckbox.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ThemedRadio.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ThemedToggleSwitch.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ThemedSlider.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ThemedProgressBar.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/Badge.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ChipTag.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SegmentedControl.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/NumberStepper.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/TreeViewItem.cpp
)
target_include_directories(test_component_library PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_component_library PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_component_library COMMAND test_component_library)

# Phase 42: Scrollbar & Scroll Behavior
add_executable(test_scroll_system
    unit/test_scroll_system.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ScrollPhysicsEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ScrollPositionStore.cpp
)
target_include_directories(test_scroll_system PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_scroll_system PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_scroll_system COMMAND test_scroll_system)

# Phase 43: Notification System V2
add_executable(test_notification_system_v2
    unit/test_notification_system_v2.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/NotificationCenterModel.cpp
)
target_include_directories(test_notification_system_v2 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_notification_system_v2 PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_notification_system_v2 COMMAND test_notification_system_v2)

# Phase 44: Dialog & Modal System
add_executable(test_dialog_system
    unit/test_dialog_system.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/DialogStackManager.cpp
)
target_include_directories(test_dialog_system PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_dialog_system PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_dialog_system COMMAND test_dialog_system)

# Phase 45: Drag & Drop System
add_executable(test_drag_drop_system
    unit/test_drag_drop_system.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/DragController.cpp
)
target_include_directories(test_drag_drop_system PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_drag_drop_system PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_drag_drop_system COMMAND test_drag_drop_system)

# Phase 46: Diff Editor and Merge Tool
add_executable(test_diff_editor
    unit/test_diff_editor.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WordDiffEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/MergeEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/core/DiffEngine.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/DiffGutter.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SideBySideDiffView.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/InlineDiffView.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/MergeEditor.cpp
)
target_include_directories(test_diff_editor PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_diff_editor PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_diff_editor COMMAND test_diff_editor)

# Phase 47: Peek View System
add_executable(test_peek_view
    unit/test_peek_view.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PeekProvider.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/PeekView.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/PeekResultList.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/PeekEditorHost.cpp
)
target_include_directories(test_peek_view PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_peek_view PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_peek_view COMMAND test_peek_view)

# Phase 48: Welcome and Onboarding
add_executable(test_welcome_onboarding
    unit/test_welcome_onboarding.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/WelcomeTab.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/RecentItemsList.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/InteractiveTutorial.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ChangelogPanel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/TipOfTheDay.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/SpotlightOverlay.cpp
)
target_include_directories(test_welcome_onboarding PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_welcome_onboarding PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_welcome_onboarding COMMAND test_welcome_onboarding)

# Phase 49: Performance Dashboard
add_executable(test_performance_dashboard
    unit/test_performance_dashboard.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PerformanceCounters.cpp
)
target_include_directories(test_performance_dashboard PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_performance_dashboard PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_performance_dashboard COMMAND test_performance_dashboard)

# Phase 50: UI Integration Testing Harness
add_executable(test_ui_harness
    unit/test_ui_harness.cpp
    ${CMAKE_SOURCE_DIR}/src/testing/VisualRegressionRunner.cpp
    ${CMAKE_SOURCE_DIR}/src/testing/UIAutomation.cpp
    ${CMAKE_SOURCE_DIR}/src/testing/AccessibilityAuditor.cpp
    ${CMAKE_SOURCE_DIR}/src/testing/ThemeCoverageChecker.cpp
    ${CMAKE_SOURCE_DIR}/src/testing/MemoryLeakDetector.cpp
)
target_include_directories(test_ui_harness PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_ui_harness PRIVATE markamp_core Catch2::Catch2WithMain)
add_test(NAME test_ui_harness COMMAND test_ui_harness)

# Phase 14: Appium Selector Contract Audit
add_executable(test_phase14_appium_selector_contract
    unit/test_phase14_appium_selector_contract.cpp
)
target_include_directories(test_phase14_appium_selector_contract PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_phase14_appium_selector_contract PRIVATE Catch2::Catch2WithMain)
add_test(NAME test_phase14_appium_selector_contract COMMAND test_phase14_appium_selector_contract)

# --- Batch 5-8 Unit Tests (400-test expansion) ---

add_executable(test_command_execution_log unit/test_command_execution_log.cpp)
target_include_directories(test_command_execution_log PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_command_execution_log PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_command_execution_log COMMAND test_command_execution_log)

add_executable(test_language_detector unit/test_language_detector.cpp)
target_include_directories(test_language_detector PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_language_detector PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_language_detector COMMAND test_language_detector)

add_executable(test_accessibility_audit unit/test_accessibility_audit.cpp)
target_include_directories(test_accessibility_audit PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_accessibility_audit PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_accessibility_audit COMMAND test_accessibility_audit)

add_executable(test_automation_rule unit/test_automation_rule.cpp)
target_include_directories(test_automation_rule PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_automation_rule PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_automation_rule COMMAND test_automation_rule)

add_executable(test_batch_export_engine unit/test_batch_export_engine.cpp)
target_include_directories(test_batch_export_engine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_batch_export_engine PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_batch_export_engine COMMAND test_batch_export_engine)

add_executable(test_activity_timeline unit/test_activity_timeline.cpp)
target_include_directories(test_activity_timeline PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_activity_timeline PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_activity_timeline COMMAND test_activity_timeline)

add_executable(test_frecency_tracker unit/test_frecency_tracker.cpp)
target_include_directories(test_frecency_tracker PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_frecency_tracker PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_frecency_tracker COMMAND test_frecency_tracker)

add_executable(test_ansi_parser unit/test_ansi_parser.cpp)
target_include_directories(test_ansi_parser PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_ansi_parser PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_ansi_parser COMMAND test_ansi_parser)

add_executable(test_expression_evaluator unit/test_expression_evaluator.cpp)
target_include_directories(test_expression_evaluator PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_expression_evaluator PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_expression_evaluator COMMAND test_expression_evaluator)

add_executable(test_formula_evaluator unit/test_formula_evaluator.cpp)
target_include_directories(test_formula_evaluator PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_formula_evaluator PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_formula_evaluator COMMAND test_formula_evaluator)

add_executable(test_cross_platform_path unit/test_cross_platform_path.cpp)
target_include_directories(test_cross_platform_path PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_cross_platform_path PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_cross_platform_path COMMAND test_cross_platform_path)

add_executable(test_fuzzy_scorer unit/test_fuzzy_scorer.cpp)
target_include_directories(test_fuzzy_scorer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_fuzzy_scorer PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_fuzzy_scorer COMMAND test_fuzzy_scorer)

add_executable(test_change_tracker unit/test_change_tracker.cpp)
target_include_directories(test_change_tracker PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_change_tracker PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_change_tracker COMMAND test_change_tracker)

add_executable(test_deprecation_tracker unit/test_deprecation_tracker.cpp)
target_include_directories(test_deprecation_tracker PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_deprecation_tracker PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_deprecation_tracker COMMAND test_deprecation_tracker)

add_executable(test_changelog_engine unit/test_changelog_engine.cpp)
target_include_directories(test_changelog_engine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_changelog_engine PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_changelog_engine COMMAND test_changelog_engine)

# --- Coverage Gap 400-test Expansion (Batch 1-8) ---

# Batch 1: AI Integration
add_executable(test_ai_conversation_history unit/test_ai_conversation_history.cpp)
target_include_directories(test_ai_conversation_history PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_ai_conversation_history PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_ai_conversation_history COMMAND test_ai_conversation_history)

add_executable(test_ai_types unit/test_ai_types.cpp)
target_include_directories(test_ai_types PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_ai_types PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_ai_types COMMAND test_ai_types)

add_executable(test_ai_document_generator unit/test_ai_document_generator.cpp)
target_include_directories(test_ai_document_generator PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_ai_document_generator PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_ai_document_generator COMMAND test_ai_document_generator)

add_executable(test_ai_search_enhancer unit/test_ai_search_enhancer.cpp)
target_include_directories(test_ai_search_enhancer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_ai_search_enhancer PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_ai_search_enhancer COMMAND test_ai_search_enhancer)

add_executable(test_ai_rate_limiter unit/test_ai_rate_limiter.cpp)
target_include_directories(test_ai_rate_limiter PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_ai_rate_limiter PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_ai_rate_limiter COMMAND test_ai_rate_limiter)

# Batch 2: Security & Input Validation
add_executable(test_url_sanitizer unit/test_url_sanitizer.cpp)
target_include_directories(test_url_sanitizer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_url_sanitizer PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_url_sanitizer COMMAND test_url_sanitizer)

add_executable(test_clipboard_sanitizer unit/test_clipboard_sanitizer.cpp)
target_include_directories(test_clipboard_sanitizer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_clipboard_sanitizer PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_clipboard_sanitizer COMMAND test_clipboard_sanitizer)

add_executable(test_data_redaction_engine unit/test_data_redaction_engine.cpp)
target_include_directories(test_data_redaction_engine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_data_redaction_engine PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_data_redaction_engine COMMAND test_data_redaction_engine)

add_executable(test_security_audit_log unit/test_security_audit_log.cpp)
target_include_directories(test_security_audit_log PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_security_audit_log PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_security_audit_log COMMAND test_security_audit_log)

add_executable(test_content_security_policy unit/test_content_security_policy.cpp)
target_include_directories(test_content_security_policy PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_content_security_policy PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_content_security_policy COMMAND test_content_security_policy)

# Batch 3: Localization & i18n
add_executable(test_translation_catalog unit/test_translation_catalog.cpp)
target_include_directories(test_translation_catalog PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_translation_catalog PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_translation_catalog COMMAND test_translation_catalog)

add_executable(test_locale_manager unit/test_locale_manager.cpp)
target_include_directories(test_locale_manager PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_locale_manager PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_locale_manager COMMAND test_locale_manager)

add_executable(test_text_direction_engine unit/test_text_direction_engine.cpp)
target_include_directories(test_text_direction_engine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_text_direction_engine PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_text_direction_engine COMMAND test_text_direction_engine)

add_executable(test_locale_info unit/test_locale_info.cpp)
target_include_directories(test_locale_info PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_locale_info PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_locale_info COMMAND test_locale_info)

add_executable(test_translation_entry unit/test_translation_entry.cpp)
target_include_directories(test_translation_entry PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_translation_entry PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_translation_entry COMMAND test_translation_entry)

# Batch 4: Cloud Sync
add_executable(test_sync_notification_types unit/test_sync_notification_types.cpp)
target_include_directories(test_sync_notification_types PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_sync_notification_types PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_sync_notification_types COMMAND test_sync_notification_types)

add_executable(test_selective_sync_filter unit/test_selective_sync_filter.cpp)
target_include_directories(test_selective_sync_filter PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_selective_sync_filter PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_selective_sync_filter COMMAND test_selective_sync_filter)

add_executable(test_sync_history_logger unit/test_sync_history_logger.cpp)
target_include_directories(test_sync_history_logger PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_sync_history_logger PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_sync_history_logger COMMAND test_sync_history_logger)

add_executable(test_sync_scheduler unit/test_sync_scheduler.cpp)
target_include_directories(test_sync_scheduler PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_sync_scheduler PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_sync_scheduler COMMAND test_sync_scheduler)

add_executable(test_sync_conflict_manager unit/test_sync_conflict_manager.cpp)
target_include_directories(test_sync_conflict_manager PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_sync_conflict_manager PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_sync_conflict_manager COMMAND test_sync_conflict_manager)

# Batch 5: WYSIWYG & Presentation
add_executable(test_wysiwyg_types unit/test_wysiwyg_types.cpp)
target_include_directories(test_wysiwyg_types PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_wysiwyg_types PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_wysiwyg_types COMMAND test_wysiwyg_types)

add_executable(test_slide_engine unit/test_slide_engine.cpp)
target_include_directories(test_slide_engine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_slide_engine PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_slide_engine COMMAND test_slide_engine)

add_executable(test_slide_theme_provider unit/test_slide_theme_provider.cpp)
target_include_directories(test_slide_theme_provider PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_slide_theme_provider PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_slide_theme_provider COMMAND test_slide_theme_provider)

add_executable(test_presentation_manager unit/test_presentation_manager.cpp)
target_include_directories(test_presentation_manager PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_presentation_manager PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_presentation_manager COMMAND test_presentation_manager)

add_executable(test_wysiwyg_state_persistence unit/test_wysiwyg_state_persistence.cpp)
target_include_directories(test_wysiwyg_state_persistence PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_wysiwyg_state_persistence PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_wysiwyg_state_persistence COMMAND test_wysiwyg_state_persistence)

# Batch 6: Encryption, Vault, Privacy
add_executable(test_key_manager unit/test_key_manager.cpp)
target_include_directories(test_key_manager PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_key_manager PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_key_manager COMMAND test_key_manager)

add_executable(test_privacy_manager unit/test_privacy_manager.cpp)
target_include_directories(test_privacy_manager PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_privacy_manager PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_privacy_manager COMMAND test_privacy_manager)

add_executable(test_runtime_policy unit/test_runtime_policy.cpp)
target_include_directories(test_runtime_policy PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_runtime_policy PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_runtime_policy COMMAND test_runtime_policy)

add_executable(test_vault_encryption_manager unit/test_vault_encryption_manager.cpp)
target_include_directories(test_vault_encryption_manager PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_vault_encryption_manager PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_vault_encryption_manager COMMAND test_vault_encryption_manager)

add_executable(test_slide_exporter unit/test_slide_exporter.cpp)
target_include_directories(test_slide_exporter PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_slide_exporter PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_slide_exporter COMMAND test_slide_exporter)

# Batch 7: Additional AI / Core
add_executable(test_ai_writing_assistant unit/test_ai_writing_assistant.cpp)
target_include_directories(test_ai_writing_assistant PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_ai_writing_assistant PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_ai_writing_assistant COMMAND test_ai_writing_assistant)

add_executable(test_ai_flashcard_generator unit/test_ai_flashcard_generator.cpp)
target_include_directories(test_ai_flashcard_generator PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_ai_flashcard_generator PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_ai_flashcard_generator COMMAND test_ai_flashcard_generator)

add_executable(test_ai_inline_completer unit/test_ai_inline_completer.cpp)
target_include_directories(test_ai_inline_completer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_ai_inline_completer PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_ai_inline_completer COMMAND test_ai_inline_completer)

add_executable(test_ai_tag_suggester unit/test_ai_tag_suggester.cpp)
target_include_directories(test_ai_tag_suggester PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_ai_tag_suggester PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_ai_tag_suggester COMMAND test_ai_tag_suggester)

add_executable(test_ai_link_suggester unit/test_ai_link_suggester.cpp)
target_include_directories(test_ai_link_suggester PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_ai_link_suggester PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_ai_link_suggester COMMAND test_ai_link_suggester)

# Batch 8: Safety, Network, Patterns
add_executable(test_safe_mode_v2 unit/test_safe_mode_v2.cpp)
target_include_directories(test_safe_mode_v2 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_safe_mode_v2 PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_safe_mode_v2 COMMAND test_safe_mode_v2)

add_executable(test_network_interceptor unit/test_network_interceptor.cpp)
target_include_directories(test_network_interceptor PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_network_interceptor PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_network_interceptor COMMAND test_network_interceptor)

add_executable(test_sync_ignore_patterns unit/test_sync_ignore_patterns.cpp)
target_include_directories(test_sync_ignore_patterns PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_sync_ignore_patterns PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_sync_ignore_patterns COMMAND test_sync_ignore_patterns)

add_executable(test_slide_block_types unit/test_slide_block_types.cpp)
target_include_directories(test_slide_block_types PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_slide_block_types PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_slide_block_types COMMAND test_slide_block_types)

# ============================================================================
# V19 Phase 01: WorkbenchShellController test
# ============================================================================
add_executable(test_v19_phase01_shell_controller unit/test_v19_phase01_shell_controller.cpp)
target_include_directories(test_v19_phase01_shell_controller PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v19_phase01_shell_controller PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v19_phase01_shell_controller COMMAND test_v19_phase01_shell_controller)

# ============================================================================
# V19 Phase 02: CommandDispatchAdapter + PromptHost tests
# ============================================================================
add_executable(test_v19_phase02_command_dispatch unit/test_v19_phase02_command_dispatch.cpp)
target_include_directories(test_v19_phase02_command_dispatch PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v19_phase02_command_dispatch PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v19_phase02_command_dispatch COMMAND test_v19_phase02_command_dispatch)

# ============================================================================
# V19 Phase 03: Session save / DirtyStateCoordinator tests
# ============================================================================
add_executable(test_v19_phase03_session_save unit/test_v19_phase03_session_save.cpp)
target_include_directories(test_v19_phase03_session_save PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v19_phase03_session_save PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v19_phase03_session_save COMMAND test_v19_phase03_session_save)

# ============================================================================
# V19 Phase 04: Panels tests
# ============================================================================
add_executable(test_v19_phase04_panels unit/test_v19_phase04_panels.cpp)
target_include_directories(test_v19_phase04_panels PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v19_phase04_panels PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v19_phase04_panels COMMAND test_v19_phase04_panels)

# ============================================================================
# V19 Phase 05: Navigation tests
# ============================================================================
add_executable(test_v19_phase05_navigation unit/test_v19_phase05_navigation.cpp)
target_include_directories(test_v19_phase05_navigation PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v19_phase05_navigation PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v19_phase05_navigation COMMAND test_v19_phase05_navigation)

# ============================================================================
# V19 Phase 06: Settings tests
# ============================================================================
add_executable(test_v19_phase06_settings unit/test_v19_phase06_settings.cpp)
target_include_directories(test_v19_phase06_settings PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v19_phase06_settings PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v19_phase06_settings COMMAND test_v19_phase06_settings)

# ============================================================================
# V19 Phase 07: Notebook tests
# ============================================================================
add_executable(test_v19_phase07_notebook unit/test_v19_phase07_notebook.cpp)
target_include_directories(test_v19_phase07_notebook PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v19_phase07_notebook PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v19_phase07_notebook COMMAND test_v19_phase07_notebook)

# ============================================================================
# V19 Phase 08: Canvas tests
# ============================================================================
add_executable(test_v19_phase08_canvas unit/test_v19_phase08_canvas.cpp)
target_include_directories(test_v19_phase08_canvas PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v19_phase08_canvas PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v19_phase08_canvas COMMAND test_v19_phase08_canvas)

# ============================================================================
# V19 Phase 09: Feedback tests
# ============================================================================
add_executable(test_v19_phase09_feedback unit/test_v19_phase09_feedback.cpp)
target_include_directories(test_v19_phase09_feedback PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v19_phase09_feedback PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v19_phase09_feedback COMMAND test_v19_phase09_feedback)

# ============================================================================
# V19 Phase 10: Icons tests
# ============================================================================
add_executable(test_v19_phase10_icons unit/test_v19_phase10_icons.cpp)
target_include_directories(test_v19_phase10_icons PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v19_phase10_icons PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v19_phase10_icons COMMAND test_v19_phase10_icons)

# ============================================================================
# V20 Phase 01: Artifact Registry
# ============================================================================
add_executable(test_v20_artifact_registry unit/test_v20_artifact_registry.cpp)
target_include_directories(test_v20_artifact_registry PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_artifact_registry PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_artifact_registry COMMAND test_v20_artifact_registry)

# ============================================================================
# V20 Phase 01: Artifact Creation Service
# ============================================================================
add_executable(test_v20_artifact_creation_service unit/test_v20_artifact_creation_service.cpp)
target_include_directories(test_v20_artifact_creation_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_artifact_creation_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_artifact_creation_service COMMAND test_v20_artifact_creation_service)

# ============================================================================
# V20 Phase 01: Artifact Mount
# ============================================================================
add_executable(test_v20_artifact_mount unit/test_v20_artifact_mount.cpp)
target_include_directories(test_v20_artifact_mount PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_artifact_mount PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_artifact_mount COMMAND test_v20_artifact_mount)

# ============================================================================
# V20 Phase 01: Artifact Naming
# ============================================================================
add_executable(test_v20_artifact_naming unit/test_v20_artifact_naming.cpp)
target_include_directories(test_v20_artifact_naming PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_artifact_naming PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_artifact_naming COMMAND test_v20_artifact_naming)

# ============================================================================
# V20 Phase 01: Artifact Events
# ============================================================================
add_executable(test_v20_artifact_events unit/test_v20_artifact_events.cpp)
target_include_directories(test_v20_artifact_events PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_artifact_events PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_artifact_events COMMAND test_v20_artifact_events)

# ============================================================================
# V20 Phase 01: Shell Artifact Integration
# ============================================================================
add_executable(test_v20_shell_artifact_integration unit/test_v20_shell_artifact_integration.cpp)
target_include_directories(test_v20_shell_artifact_integration PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_shell_artifact_integration PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_shell_artifact_integration COMMAND test_v20_shell_artifact_integration)

# ============================================================================
# V20 Phase 02: Unsaved Document Buffer
# ============================================================================
add_executable(test_v20_unsaved_buffer unit/test_v20_unsaved_buffer.cpp)
target_include_directories(test_v20_unsaved_buffer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_unsaved_buffer PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_unsaved_buffer COMMAND test_v20_unsaved_buffer)

# ============================================================================
# V20 Phase 02: Text Artifact Lifecycle
# ============================================================================
add_executable(test_v20_text_artifact_lifecycle unit/test_v20_text_artifact_lifecycle.cpp)
target_include_directories(test_v20_text_artifact_lifecycle PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_text_artifact_lifecycle PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_text_artifact_lifecycle COMMAND test_v20_text_artifact_lifecycle)

# ============================================================================
# V20 Phase 02: Editor Mount Context
# ============================================================================
add_executable(test_v20_editor_mount_context unit/test_v20_editor_mount_context.cpp)
target_include_directories(test_v20_editor_mount_context PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_editor_mount_context PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_editor_mount_context COMMAND test_v20_editor_mount_context)

# ============================================================================
# V20 Phase 02: File Entry Point Resolver
# ============================================================================
add_executable(test_v20_file_entry_point unit/test_v20_file_entry_point.cpp)
target_include_directories(test_v20_file_entry_point PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_file_entry_point PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_file_entry_point COMMAND test_v20_file_entry_point)

# ============================================================================
# V20 Phase 02: File Lifecycle Integration
# ============================================================================
add_executable(test_v20_file_lifecycle_integration unit/test_v20_file_lifecycle_integration.cpp)
target_include_directories(test_v20_file_lifecycle_integration PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_file_lifecycle_integration PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_file_lifecycle_integration COMMAND test_v20_file_lifecycle_integration)

# ============================================================================
# V20 Phase 03: Notebook Artifact Lifecycle
# ============================================================================
add_executable(test_v20_notebook_lifecycle unit/test_v20_notebook_lifecycle.cpp)
target_include_directories(test_v20_notebook_lifecycle PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_notebook_lifecycle PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_notebook_lifecycle COMMAND test_v20_notebook_lifecycle)

# ============================================================================
# V20 Phase 03: Notebook Storage Resolver
# ============================================================================
add_executable(test_v20_notebook_storage unit/test_v20_notebook_storage.cpp)
target_include_directories(test_v20_notebook_storage PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_notebook_storage PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_notebook_storage COMMAND test_v20_notebook_storage)

# ============================================================================
# V20 Phase 03: Notebook Command Router
# ============================================================================
add_executable(test_v20_notebook_command_router unit/test_v20_notebook_command_router.cpp)
target_include_directories(test_v20_notebook_command_router PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_notebook_command_router PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_notebook_command_router COMMAND test_v20_notebook_command_router)

# ============================================================================
# V20 Phase 03: Notebook Integration
# ============================================================================
add_executable(test_v20_notebook_integration unit/test_v20_notebook_integration.cpp)
target_include_directories(test_v20_notebook_integration PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_notebook_integration PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_notebook_integration COMMAND test_v20_notebook_integration)

# ============================================================================
# V20 Phase 04: Canvas Artifact Lifecycle
# ============================================================================
add_executable(test_v20_canvas_lifecycle unit/test_v20_canvas_lifecycle.cpp)
target_include_directories(test_v20_canvas_lifecycle PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_canvas_lifecycle PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_canvas_lifecycle COMMAND test_v20_canvas_lifecycle)

# ============================================================================
# V20 Phase 04: Canvas Event Bridge
# ============================================================================
add_executable(test_v20_canvas_event_bridge unit/test_v20_canvas_event_bridge.cpp)
target_include_directories(test_v20_canvas_event_bridge PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_canvas_event_bridge PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_canvas_event_bridge COMMAND test_v20_canvas_event_bridge)

# ============================================================================
# V20 Phase 04: Canvas Shell Integration
# ============================================================================
add_executable(test_v20_canvas_shell unit/test_v20_canvas_shell.cpp)
target_include_directories(test_v20_canvas_shell PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_canvas_shell PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_canvas_shell COMMAND test_v20_canvas_shell)

# ============================================================================
# V20 Phase 04: Canvas Integration
# ============================================================================
add_executable(test_v20_canvas_integration unit/test_v20_canvas_integration.cpp)
target_include_directories(test_v20_canvas_integration PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_canvas_integration PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_canvas_integration COMMAND test_v20_canvas_integration)

# ============================================================================
# V20 Phase 05: Persistence, Autosave, Restore & Recovery
# ============================================================================
add_executable(test_v20_persistence_outcome unit/test_v20_persistence_outcome.cpp)
target_include_directories(test_v20_persistence_outcome PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_persistence_outcome PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_persistence_outcome COMMAND test_v20_persistence_outcome)

add_executable(test_v20_atomic_write unit/test_v20_atomic_write.cpp)
target_include_directories(test_v20_atomic_write PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_atomic_write PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_atomic_write COMMAND test_v20_atomic_write)

add_executable(test_v20_session_restore unit/test_v20_session_restore.cpp)
target_include_directories(test_v20_session_restore PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_session_restore PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_session_restore COMMAND test_v20_session_restore)

add_executable(test_v20_persistence_integration unit/test_v20_persistence_integration.cpp)
target_include_directories(test_v20_persistence_integration PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_persistence_integration PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_persistence_integration COMMAND test_v20_persistence_integration)

# ============================================================================
# V20 Phase 06: Commands, Context, Focus & Surface Entry Points
# ============================================================================
add_executable(test_v20_command_model unit/test_v20_command_model.cpp)
target_include_directories(test_v20_command_model PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_command_model PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_command_model COMMAND test_v20_command_model)

add_executable(test_v20_active_context unit/test_v20_active_context.cpp)
target_include_directories(test_v20_active_context PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_active_context PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_active_context COMMAND test_v20_active_context)

add_executable(test_v20_entry_point unit/test_v20_entry_point.cpp)
target_include_directories(test_v20_entry_point PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_entry_point PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_entry_point COMMAND test_v20_entry_point)

add_executable(test_v20_command_integration unit/test_v20_command_integration.cpp)
target_include_directories(test_v20_command_integration PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_command_integration PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_command_integration COMMAND test_v20_command_integration)

# ============================================================================
# V20 Phase 07: Syntax Highlighting & Language Presentation
# ============================================================================
add_executable(test_v20_language_service unit/test_v20_language_service.cpp)
target_include_directories(test_v20_language_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_language_service PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_language_service COMMAND test_v20_language_service)

add_executable(test_v20_token_theme unit/test_v20_token_theme.cpp)
target_include_directories(test_v20_token_theme PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_token_theme PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_token_theme COMMAND test_v20_token_theme)

add_executable(test_v20_highlight_degradation unit/test_v20_highlight_degradation.cpp)
target_include_directories(test_v20_highlight_degradation PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_highlight_degradation PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_highlight_degradation COMMAND test_v20_highlight_degradation)

add_executable(test_v20_syntax_integration unit/test_v20_syntax_integration.cpp)
target_include_directories(test_v20_syntax_integration PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_syntax_integration PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_syntax_integration COMMAND test_v20_syntax_integration)

# V20 Phase 08: Editor, Notebook, Canvas Rendering & Content Styling
add_executable(test_v20_content_style unit/test_v20_content_style.cpp)
target_include_directories(test_v20_content_style PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_content_style PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_content_style COMMAND test_v20_content_style)

add_executable(test_v20_notebook_cell_chrome unit/test_v20_notebook_cell_chrome.cpp)
target_include_directories(test_v20_notebook_cell_chrome PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_notebook_cell_chrome PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_notebook_cell_chrome COMMAND test_v20_notebook_cell_chrome)

add_executable(test_v20_canvas_content_presenter unit/test_v20_canvas_content_presenter.cpp)
target_include_directories(test_v20_canvas_content_presenter PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_canvas_content_presenter PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_canvas_content_presenter COMMAND test_v20_canvas_content_presenter)

add_executable(test_v20_rendering_integration unit/test_v20_rendering_integration.cpp)
target_include_directories(test_v20_rendering_integration PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_rendering_integration PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_rendering_integration COMMAND test_v20_rendering_integration)

# V20 Phase 09: Premium Visual System Polish & MUI Icon Completion
add_executable(test_v20_shell_visual unit/test_v20_shell_visual.cpp)
target_include_directories(test_v20_shell_visual PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_shell_visual PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_shell_visual COMMAND test_v20_shell_visual)

add_executable(test_v20_icon_semantic unit/test_v20_icon_semantic.cpp)
target_include_directories(test_v20_icon_semantic PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_icon_semantic PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_icon_semantic COMMAND test_v20_icon_semantic)

add_executable(test_v20_accessibility_audit unit/test_v20_accessibility_audit.cpp)
target_include_directories(test_v20_accessibility_audit PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_accessibility_audit PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_accessibility_audit COMMAND test_v20_accessibility_audit)

add_executable(test_v20_visual_integration unit/test_v20_visual_integration.cpp)
target_include_directories(test_v20_visual_integration PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_visual_integration PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_visual_integration COMMAND test_v20_visual_integration)

# V20 Phase 10: Diagnostics, Validation Harnesses & Consolidation
add_executable(test_v20_lifecycle_tracer unit/test_v20_lifecycle_tracer.cpp)
target_include_directories(test_v20_lifecycle_tracer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_lifecycle_tracer PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_lifecycle_tracer COMMAND test_v20_lifecycle_tracer)

add_executable(test_v20_smoke_runner unit/test_v20_smoke_runner.cpp)
target_include_directories(test_v20_smoke_runner PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_smoke_runner PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_smoke_runner COMMAND test_v20_smoke_runner)

add_executable(test_v20_execution_scoreboard unit/test_v20_execution_scoreboard.cpp)
target_include_directories(test_v20_execution_scoreboard PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_execution_scoreboard PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_execution_scoreboard COMMAND test_v20_execution_scoreboard)

add_executable(test_v20_diagnostics_integration unit/test_v20_diagnostics_integration.cpp)
target_include_directories(test_v20_diagnostics_integration PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v20_diagnostics_integration PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v20_diagnostics_integration COMMAND test_v20_diagnostics_integration)

# V21 Phase 01: Control Audit Spine & Canonical Action Model
add_executable(test_v21_control_audit unit/test_v21_control_audit.cpp)
target_include_directories(test_v21_control_audit PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v21_control_audit PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v21_control_audit COMMAND test_v21_control_audit)

# V21 Phase 02: Menu / Palette / Shortcut Parity
add_executable(test_v21_menu_palette unit/test_v21_menu_palette.cpp)
target_include_directories(test_v21_menu_palette PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v21_menu_palette PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v21_menu_palette COMMAND test_v21_menu_palette)

# V21 Phase 03: Toolbar, Status Bar & Global Button Wiring
add_executable(test_v21_toolbar_statusbar unit/test_v21_toolbar_statusbar.cpp)
target_include_directories(test_v21_toolbar_statusbar PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v21_toolbar_statusbar PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v21_toolbar_statusbar COMMAND test_v21_toolbar_statusbar)

# V21 Phase 04: Context Menus, Inline Actions & Target-Aware Enablement
add_executable(test_v21_context_menus unit/test_v21_context_menus.cpp)
target_include_directories(test_v21_context_menus PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v21_context_menus PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v21_context_menus COMMAND test_v21_context_menus)

# V21 Phase 05: Panel Registry, Primary Sidebar & Controlled Surface Replacement
add_executable(test_v21_primary_sidebar unit/test_v21_primary_sidebar.cpp)
target_include_directories(test_v21_primary_sidebar PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v21_primary_sidebar PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v21_primary_sidebar COMMAND test_v21_primary_sidebar)

# V21 Phase 06: Secondary, Bottom & Specialized Panel Lifecycle
add_executable(test_v21_panel_lifecycle unit/test_v21_panel_lifecycle.cpp)
target_include_directories(test_v21_panel_lifecycle PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v21_panel_lifecycle PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v21_panel_lifecycle COMMAND test_v21_panel_lifecycle)

# V21 Phase 07: Workbench Surface Completion
add_executable(test_v21_workbench_surfaces unit/test_v21_workbench_surfaces.cpp)
target_include_directories(test_v21_workbench_surfaces PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v21_workbench_surfaces PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v21_workbench_surfaces COMMAND test_v21_workbench_surfaces)

# V21 Phase 08: Settings Architecture & Schema Consolidation
add_executable(test_v21_settings_architecture unit/test_v21_settings_architecture.cpp)
target_include_directories(test_v21_settings_architecture PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v21_settings_architecture PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v21_settings_architecture COMMAND test_v21_settings_architecture)

# V21 Phase 09: Settings UI & JSON Editor
add_executable(test_v21_settings_ui unit/test_v21_settings_ui.cpp)
target_include_directories(test_v21_settings_ui PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v21_settings_ui PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v21_settings_ui COMMAND test_v21_settings_ui)

# V21 Phase 10: Validation Harnesses & Dead UI Removal
add_executable(test_v21_validation_harness unit/test_v21_validation_harness.cpp)
target_include_directories(test_v21_validation_harness PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v21_validation_harness PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v21_validation_harness COMMAND test_v21_validation_harness)

# ============================================================================
# V22 Phase 01: Visual Language & Design System Foundation
# ============================================================================
add_executable(test_v22_visual_language unit/test_v22_visual_language.cpp)
target_include_directories(test_v22_visual_language PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v22_visual_language PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v22_visual_language COMMAND test_v22_visual_language)

# V22 Phase 01: Design System Audit
add_executable(test_v22_design_audit unit/test_v22_design_audit.cpp)
target_include_directories(test_v22_design_audit PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v22_design_audit PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v22_design_audit COMMAND test_v22_design_audit)

# V22 Phase 02: Shell Chrome Coordination
add_executable(test_v22_shell_chrome unit/test_v22_shell_chrome.cpp)
target_include_directories(test_v22_shell_chrome PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v22_shell_chrome PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v22_shell_chrome COMMAND test_v22_shell_chrome)

# V22 Phase 03: Panel Chrome Coordination
add_executable(test_v22_panel_chrome unit/test_v22_panel_chrome.cpp)
target_include_directories(test_v22_panel_chrome PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v22_panel_chrome PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v22_panel_chrome COMMAND test_v22_panel_chrome)

# V22 Phase 04: Command Surface Coordination
add_executable(test_v22_command_surface unit/test_v22_command_surface.cpp)
target_include_directories(test_v22_command_surface PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v22_command_surface PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v22_command_surface COMMAND test_v22_command_surface)

# V22 Phase 05: Navigation Chrome Coordination
add_executable(test_v22_navigation_chrome unit/test_v22_navigation_chrome.cpp)
target_include_directories(test_v22_navigation_chrome PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v22_navigation_chrome PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v22_navigation_chrome COMMAND test_v22_navigation_chrome)

# V22 Phase 06: Editor Chrome Coordination
add_executable(test_v22_editor_chrome unit/test_v22_editor_chrome.cpp)
target_include_directories(test_v22_editor_chrome PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v22_editor_chrome PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v22_editor_chrome COMMAND test_v22_editor_chrome)

# V22 Phase 07: Syntax Presentation Coordination
add_executable(test_v22_syntax_presentation unit/test_v22_syntax_presentation.cpp)
target_include_directories(test_v22_syntax_presentation PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v22_syntax_presentation PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v22_syntax_presentation COMMAND test_v22_syntax_presentation)

# V22 Phase 08: Editor Overlay Coordination
add_executable(test_v22_editor_overlay unit/test_v22_editor_overlay.cpp)
target_include_directories(test_v22_editor_overlay PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v22_editor_overlay PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v22_editor_overlay COMMAND test_v22_editor_overlay)

# V22 Phase 09: Notebook Chrome Coordination
add_executable(test_v22_notebook_chrome unit/test_v22_notebook_chrome.cpp)
target_include_directories(test_v22_notebook_chrome PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v22_notebook_chrome PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v22_notebook_chrome COMMAND test_v22_notebook_chrome)

# V22 Phase 10: Canvas Chrome Coordination
add_executable(test_v22_canvas_chrome unit/test_v22_canvas_chrome.cpp)
target_include_directories(test_v22_canvas_chrome PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v22_canvas_chrome PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v22_canvas_chrome COMMAND test_v22_canvas_chrome)

# V22 Phase 11: Sidebar Panel Coordination
add_executable(test_v22_sidebar_panel unit/test_v22_sidebar_panel.cpp)
target_include_directories(test_v22_sidebar_panel PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v22_sidebar_panel PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v22_sidebar_panel COMMAND test_v22_sidebar_panel)

# V22 Phase 12: Utility Panel Coordination
add_executable(test_v22_utility_panel unit/test_v22_utility_panel.cpp)
target_include_directories(test_v22_utility_panel PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v22_utility_panel PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v22_utility_panel COMMAND test_v22_utility_panel)

# V22 Phase 13: Settings Surface Coordination
add_executable(test_v22_settings_surface unit/test_v22_settings_surface.cpp)
target_include_directories(test_v22_settings_surface PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v22_settings_surface PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v22_settings_surface COMMAND test_v22_settings_surface)

# V22 Phase 14: Transient Overlay Coordination
add_executable(test_v22_transient_overlay unit/test_v22_transient_overlay.cpp)
target_include_directories(test_v22_transient_overlay PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v22_transient_overlay PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v22_transient_overlay COMMAND test_v22_transient_overlay)

# V22 Phase 15: Dense Data Coordination
add_executable(test_v22_dense_data unit/test_v22_dense_data.cpp)
target_include_directories(test_v22_dense_data PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v22_dense_data PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v22_dense_data COMMAND test_v22_dense_data)

# V22 Phase 16: Onboarding State Coordination
add_executable(test_v22_onboarding_state unit/test_v22_onboarding_state.cpp)
target_include_directories(test_v22_onboarding_state PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v22_onboarding_state PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v22_onboarding_state COMMAND test_v22_onboarding_state)

# V22 Phase 17: Accessibility Coordination
add_executable(test_v22_accessibility unit/test_v22_accessibility.cpp)
target_include_directories(test_v22_accessibility PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v22_accessibility PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v22_accessibility COMMAND test_v22_accessibility)

# V22 Phase 18: Theme Parity Coordination
add_executable(test_v22_theme_parity unit/test_v22_theme_parity.cpp)
target_include_directories(test_v22_theme_parity PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v22_theme_parity PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v22_theme_parity COMMAND test_v22_theme_parity)

# V22 Phase 19: Motion Language Coordination
add_executable(test_v22_motion_language unit/test_v22_motion_language.cpp)
target_include_directories(test_v22_motion_language PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v22_motion_language PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v22_motion_language COMMAND test_v22_motion_language)

# V22 Phase 20: Visual Cleanup Coordination
add_executable(test_v22_visual_cleanup unit/test_v22_visual_cleanup.cpp)
target_include_directories(test_v22_visual_cleanup PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v22_visual_cleanup PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v22_visual_cleanup COMMAND test_v22_visual_cleanup)

# ============================================================================
# V23 Phase 01: Completion Inventory & Severity Policy
# ============================================================================
add_executable(test_v23_completion_inventory unit/test_v23_completion_inventory.cpp)
target_include_directories(test_v23_completion_inventory PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v23_completion_inventory PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v23_completion_inventory COMMAND test_v23_completion_inventory)

add_executable(test_v23_severity_policy unit/test_v23_severity_policy.cpp)
target_include_directories(test_v23_severity_policy PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v23_severity_policy PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v23_severity_policy COMMAND test_v23_severity_policy)

add_executable(test_v23_action_readiness unit/test_v23_action_readiness.cpp)
target_include_directories(test_v23_action_readiness PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v23_action_readiness PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v23_action_readiness COMMAND test_v23_action_readiness)

add_executable(test_v23_lifecycle_completion unit/test_v23_lifecycle_completion.cpp)
target_include_directories(test_v23_lifecycle_completion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v23_lifecycle_completion PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v23_lifecycle_completion COMMAND test_v23_lifecycle_completion)

add_executable(test_v23_persistence_completion unit/test_v23_persistence_completion.cpp)
target_include_directories(test_v23_persistence_completion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v23_persistence_completion PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v23_persistence_completion COMMAND test_v23_persistence_completion)

add_executable(test_v23_editor_completion unit/test_v23_editor_completion.cpp)
target_include_directories(test_v23_editor_completion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v23_editor_completion PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v23_editor_completion COMMAND test_v23_editor_completion)

add_executable(test_v23_notebook_completion unit/test_v23_notebook_completion.cpp)
target_include_directories(test_v23_notebook_completion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v23_notebook_completion PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v23_notebook_completion COMMAND test_v23_notebook_completion)

add_executable(test_v23_canvas_completion unit/test_v23_canvas_completion.cpp)
target_include_directories(test_v23_canvas_completion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v23_canvas_completion PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v23_canvas_completion COMMAND test_v23_canvas_completion)

add_executable(test_v23_surface_completion unit/test_v23_surface_completion.cpp)
target_include_directories(test_v23_surface_completion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v23_surface_completion PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v23_surface_completion COMMAND test_v23_surface_completion)

add_executable(test_v23_project_completion unit/test_v23_project_completion.cpp)
target_include_directories(test_v23_project_completion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v23_project_completion PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v23_project_completion COMMAND test_v23_project_completion)

add_executable(test_v23_settings_completion unit/test_v23_settings_completion.cpp)
target_include_directories(test_v23_settings_completion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v23_settings_completion PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v23_settings_completion COMMAND test_v23_settings_completion)

add_executable(test_v23_plugin_completion unit/test_v23_plugin_completion.cpp)
target_include_directories(test_v23_plugin_completion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v23_plugin_completion PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v23_plugin_completion COMMAND test_v23_plugin_completion)

add_executable(test_v23_cloud_security_completion unit/test_v23_cloud_security_completion.cpp)
target_include_directories(test_v23_cloud_security_completion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v23_cloud_security_completion PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v23_cloud_security_completion COMMAND test_v23_cloud_security_completion)

add_executable(test_v23_rendering_completion unit/test_v23_rendering_completion.cpp)
target_include_directories(test_v23_rendering_completion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v23_rendering_completion PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v23_rendering_completion COMMAND test_v23_rendering_completion)

add_executable(test_v23_node_editor_completion unit/test_v23_node_editor_completion.cpp)
target_include_directories(test_v23_node_editor_completion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v23_node_editor_completion PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v23_node_editor_completion COMMAND test_v23_node_editor_completion)

add_executable(test_v23_structured_data_completion unit/test_v23_structured_data_completion.cpp)
target_include_directories(test_v23_structured_data_completion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v23_structured_data_completion PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v23_structured_data_completion COMMAND test_v23_structured_data_completion)

add_executable(test_v23_platform_completion unit/test_v23_platform_completion.cpp)
target_include_directories(test_v23_platform_completion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v23_platform_completion PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v23_platform_completion COMMAND test_v23_platform_completion)

add_executable(test_v23_service_stub_completion unit/test_v23_service_stub_completion.cpp)
target_include_directories(test_v23_service_stub_completion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v23_service_stub_completion PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v23_service_stub_completion COMMAND test_v23_service_stub_completion)

add_executable(test_v23_feature_flag_completion unit/test_v23_feature_flag_completion.cpp)
target_include_directories(test_v23_feature_flag_completion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v23_feature_flag_completion PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v23_feature_flag_completion COMMAND test_v23_feature_flag_completion)

add_executable(test_v23_testing_regression_completion unit/test_v23_testing_regression_completion.cpp)
target_include_directories(test_v23_testing_regression_completion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v23_testing_regression_completion PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v23_testing_regression_completion COMMAND test_v23_testing_regression_completion)

add_executable(test_v23_release_gate_completion unit/test_v23_release_gate_completion.cpp)
target_include_directories(test_v23_release_gate_completion PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_v23_release_gate_completion PRIVATE Catch2::Catch2WithMain markamp_core)
add_test(NAME test_v23_release_gate_completion COMMAND test_v23_release_gate_completion)
