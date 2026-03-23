add_executable(markamp
    main.cpp
    app/MarkAmpApp.cpp
    ui/MainFrame.cpp
    ui/CustomChrome.cpp
    ui/BevelPanel.cpp
    ui/ThemeAwareWindow.cpp
    # V10 Phase 01: Unified control state tracking
    ui/ControlState.cpp
    ui/LayoutMetrics.cpp
    ui/ColorPaletteGenerator.cpp
    ui/animation/EasingFunctions.cpp
    ui/animation/ValueInterpolator.cpp
    ui/animation/SpringPhysics.cpp
    ui/animation/AnimationTimeline.cpp
    ui/animation/TransitionManager.cpp
    ui/animation/SkeletonLoader.cpp
    ui/animation/FocusRingAnimator.cpp
    ui/animation/ProgressBar.cpp
    ui/ColorUtils.cpp
    ui/ComponentSizeResolver.cpp
    ui/DesignTokenRegistry.cpp
    ui/DesignTokenValue.cpp
    ui/ElevationSystem.cpp
    ui/SpacingGrid.cpp
    ui/TypographyScale.cpp
    ui/InputModel.cpp
    ui/FocusManager.cpp
    ui/accessibility/AccessibilityController.cpp
    ui/accessibility/SkipToContentButton.cpp
    ui/accessibility/LiveAnnouncer.cpp
    ui/FocusRingRenderer.cpp
    ui/ActivityBarModel.cpp
    ui/FileTreeModel.cpp
    ui/TabStripModel.cpp
    ui/ToolbarModel.cpp
    ui/CommandPaletteModel.cpp
    ui/SettingsNavModel.cpp
    ui/SettingsControlModel.cpp
    ui/StatusBarModel.cpp
    ui/BreadcrumbModel.cpp
    ui/SplitterModel.cpp
    ui/ContextMenuModel.cpp
    ui/ContextMenuBuilder.cpp
    ui/ContextMenuRegistry.cpp
    ui/ThemedContextMenu.cpp
    ui/ToolWindowTabControl.cpp
    ui/PanelLifecycleManager.cpp
    ui/NavigationCommandSet.cpp
    ui/JumpToController.cpp
    ui/ControlMotionTokens.cpp
    ui/ControlAnimator.cpp
    ui/DragAffordanceController.cpp
    ui/DropTargetHighlighter.cpp
    ui/MultiSelectModel.cpp
    ui/BulkActionController.cpp
    ui/PreflightValidator.cpp
    ui/RecoveryActionProvider.cpp
    ui/AccessibilitySemanticsController.cpp
    ui/ScreenReaderAnnouncer.cpp
    ui/DpiScalingController.cpp
    ui/CompactLayoutModel.cpp
    ui/LayoutPresetController.cpp
    ui/ControlVisibilityPrefs.cpp
    ui/UxMetricsCollector.cpp
    ui/TelemetryPrivacyController.cpp
    ui/ControlSnapshotTester.cpp
    ui/KeyboardWorkflowRunner.cpp
    ui/UxAuditRunner.cpp
    ui/ReleaseGateChecker.cpp
    ui/ShortcutOverlayModel.cpp
    ui/NotificationModel.cpp
    ui/DialogModel.cpp
    ui/StartupModel.cpp
    ui/SearchPanelModel.cpp
    ui/FindReplaceModel.cpp
    ui/EditorOverlayModel.cpp
    ui/PreviewControlModel.cpp
    ui/CanvasControlModel.cpp
    ui/GraphControlModel.cpp
    ui/ExtensionBrowserModel.cpp
    ui/ThemeGalleryModel.cpp
    ui/ToolWindowModel.cpp
    ui/NavigationModel.cpp
    ui/MotionModel.cpp
    ui/DragDropModel.cpp
    ui/MultiSelectModel.cpp
    ui/ErrorRecoveryModel.cpp
    ui/AccessibilityModel.cpp
    ui/ResponsiveModel.cpp
    ui/LayoutPresetModel.cpp
    ui/UxMetricsModel.cpp
    ui/RegressionHarnessModel.cpp
    ui/ReleaseAuditModel.cpp
    ui/ThemedScrollbar.cpp
    # V13 Phase 41: Component Library Polish
    ui/ComponentVariants.cpp
    ui/ThemedButton.cpp
    ui/ThemedTextInput.cpp
    ui/ThemedDropdown.cpp
    ui/ThemedCheckbox.cpp
    ui/ThemedRadio.cpp
    ui/ThemedToggleSwitch.cpp
    ui/ThemedSlider.cpp
    ui/ThemedProgressBar.cpp
    ui/Badge.cpp
    ui/ChipTag.cpp
    ui/SegmentedControl.cpp
    ui/NumberStepper.cpp
    ui/TreeViewItem.cpp
    # V13 Phase 42: Scrollbar & Scroll Behavior
    ui/ScrollPhysicsEngine.cpp
    ui/ScrollPositionStore.cpp
    # V13 Phase 43: Notification System V2
    ui/NotificationCenterModel.cpp
    # V13 Phase 44: Dialog & Modal System
    ui/DialogStackManager.cpp
    # V13 Phase 45: Drag & Drop System
    ui/DragController.cpp
    # V13 Phase 41 (cont): Additional widgets
    ui/RichTooltip.cpp
    ui/ThemedColorPicker.cpp
    ui/AnimationDriver.cpp
    # V13 Phase 42 (cont): Scroll decorations
    ui/ScrollDecorationProvider.cpp
    # V13 Phase 43 (cont): Notification persistence
    ui/NotificationPersistence.cpp
    # V13 Phase 44 (cont): Dialog widgets
    ui/ThemedDialog.cpp
    ui/UnsavedChangesGuard.cpp
    # V13 Phase 45 (cont): Drag rendering
    ui/DragGhostWindow.cpp
    ui/DropIndicatorOverlay.cpp
    ui/DragAutoScroll.cpp
    ui/DragUndoAction.cpp
    ui/LayoutManager.cpp
    ui/ToolWindowHost.cpp
    # V8 Phase 12: Surface link router + transition coordinator
    core/SurfaceLink.cpp
    ui/SurfaceTransitionCoordinator.cpp
    # V8 Phase 12: VSCode theme infrastructure
    core/VsCodeThemeAdapter.cpp
    core/ThemeScopeMapper.cpp
    ui/SplitterBar.cpp
    ui/layout/WorkbenchLayoutModel.cpp
    ui/layout/ResizeHandle.cpp
    ui/layout/WorkbenchShell.cpp
    ui/StatusBarPanel.cpp
    ui/FileTreeCtrl.cpp
    ui/EditorPanel.cpp
    ui/MinimapPanel.cpp
    ui/OverviewRulerPanel.cpp
    ui/GutterDecorationProvider.cpp
    ui/CodeLensProvider.cpp
    ui/GitGutterProvider.cpp
    ui/GitBlameGutterProvider.cpp
    ui/MergeConflictResolver.cpp
    ui/DiffPanel.cpp
    ui/PreviewPanel.cpp
    ui/SplitView.cpp
    ui/EditorGroupManager.cpp
    ui/Toolbar.cpp
    ui/RunConfigDropdown.cpp
    ui/TooltipWindow.cpp
    ui/ThemeGallery.cpp
    ui/ThemeTokenEditor.cpp
    ui/ThemeEditorPanel.cpp
    ui/ColorBlindnessPreviewOverlay.cpp
    ui/ThemePreviewCard.cpp
    ui/ShortcutOverlay.cpp
    ui/CommandPalette.cpp
    ui/StartupPanel.cpp
    ui/BreadcrumbBar.cpp
    ui/BreadcrumbDropdown.cpp
    ui/FloatingFormatBar.cpp
    ui/LightbulbWidget.cpp
    ui/CodeActionMenu.cpp
    ui/CodeActionPreview.cpp
    ui/FloatingToolbar.cpp
    ui/FloatingToolbarManager.cpp
    ui/DebugToolbar.cpp
    ui/SelectionActionBar.cpp
    ui/MinimapHoverToolbar.cpp
    ui/ImagePreviewToolbar.cpp
    # V13 Phase 31: Command Palette V2
    core/FuzzyScorer.cpp
    core/FrecencyTracker.cpp
    core/MarkdownSymbolProvider.cpp
    ui/IconProvider.cpp
    # V13 Phase 32: Go-To System
    core/NavigationService.cpp
    core/LinkResolver.cpp
    core/BacklinkFinder.cpp
    core/ChangeTracker.cpp
    core/WorkspaceSymbolIndex.cpp
    # V13 Phase 33: Outline Panel
    ui/OutlinePanel.cpp
    # V13 Phase 34: Find & Replace V2
    ui/FindBar.cpp
    core/ReplaceUndoBuffer.cpp
    core/PreserveCaseReplacer.cpp
    ui/SearchResultsPanel.cpp
    # V13 Phase 35: Quick Open & File Switching
    core/RecentFilesManager.cpp
    core/FileSearchScorer.cpp
    core/FileIndex.cpp
    ui/TabSwitcherOverlay.cpp
    # V13 Phase 36: Settings UI Overhaul
    ui/RichDescriptionRenderer.cpp
    ui/SettingsCompareView.cpp
    ui/NavigationHistoryPanel.cpp
    ui/LinkPreviewPopover.cpp
    ui/ImagePreviewPopover.cpp
    ui/TableEditorOverlay.cpp
    ui/TabBar.cpp
    ui/FileTypeIconRegistry.cpp
    ui/SettingsPanel.cpp
    ui/SettingsDialog.cpp
    ui/SettingsJsonEditor.cpp
    ui/ShortcutEditor.cpp
    ui/FirstRunWizard.cpp
    ui/NotificationManager.cpp
    ui/ActivityBar.cpp
    ui/SidebarPanelRegistry.cpp
    ui/SidebarToolbar.cpp
    ui/PanelHeader.cpp
    ui/PanelHeaderBar.cpp
    ui/PanelHeaderIcons.cpp
    ui/SidebarFooter.cpp
    ui/SidebarSection.cpp
    ui/SidebarSkeletonPlaceholder.cpp
    ui/EmptyPanelState.cpp
    ui/SearchSidebarPanel.cpp
    ui/SearchResultsTree.cpp
    ui/SecondarySidebarTabStrip.cpp
    ui/GraphSidebarPanel.cpp
    ui/ExtensionsBrowserPanel.cpp
    ui/ExplorerToolbar.cpp
    ui/ExplorerPanel.cpp
    ui/OpenEditorsSection.cpp
    ui/GitStatusProvider.cpp
    ui/SourceControlPanel.cpp
    ui/GitLogPanel.cpp
    ui/ExtensionCard.cpp
    ui/ExtensionDetailPanel.cpp
    ui/OutputPanel.cpp
    ui/PanelAreaModel.cpp
    ui/PanelTabBar.cpp
    ui/PanelContainer.cpp
    ui/TerminalPanel.cpp
    ui/DebugConsolePanel.cpp
    # Phase 19: Run and Debug Panel
    ui/RunDebugPanel.cpp
    ui/DebugToolbar.cpp
    core/LaunchConfig.cpp
    core/DebugSessionManager.cpp
    core/DebugAdapterClient.cpp
    core/TaskRunner.cpp
    core/ProcessRunner.cpp
    core/CompilerOutputParser.cpp
    # Phase 20: Extensions Panel V2
    core/MarketplaceCache.cpp
    core/ExtensionIconCache.cpp
    # Phase 21: Integrated Terminal Panel
    core/AnsiParser.cpp
    core/TerminalBuffer.cpp
    core/PtyProcess.cpp
    ui/TerminalTabBar.cpp
    # Phase 22-25: Output Panel V2, Problems V2, Debug Console, Build & Tasks
    ui/OutputPanelRenderer.cpp
    ui/ProblemsTreeModel.cpp
    core/DebugConsoleService.cpp
    core/ExpressionEvaluator.cpp
    core/CompilerErrorParser.cpp
    core/TaskConfig.cpp
    core/BuildService.cpp
    ui/BuildPanel.cpp
    ui/TaskListPanel.cpp
    ui/ProblemsPanel.cpp
    ui/TreeViewHost.cpp
    ui/WebviewHostPanel.cpp
    ui/WalkthroughPanel.cpp
    core/Logger.cpp
    core/PluginManager.cpp
    core/PluginHotReload.cpp
    core/ContributionValidator.cpp
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
    core/RunConfigService.cpp
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
    core/ExtensionSandbox.cpp
    ui/IconManager.cpp
    ui/FileTypeIconResolver.cpp
    ui/IconCache.cpp
    ui/IconGalleryDialog.cpp
    ui/IconLibrary.cpp
    ui/IconRegistry.cpp
    ui/IconRenderer.cpp
    ui/SvgDocument.cpp
    # V16 Phase 01-04: Icon Manifest & Pipeline
    ui/IconManifest.cpp
    ui/IconPipeline.cpp
    core/EventBus.cpp
    core/AppState.cpp
    core/Command.cpp
    core/Config.cpp
    core/ConfigAuditTrail.cpp
    core/ConfigFileWatcher.cpp
    core/ConfigMigration.cpp
    core/ConfigProfile.cpp
    core/ScopedConfig.cpp
    core/SettingsCatalog.cpp
    core/Color.cpp
    core/Theme.cpp
    core/BuiltInThemes.cpp
    core/ThemeRegistry.cpp
    core/ThemeValidator.cpp
    core/ThemeEngine.cpp
    core/TokenInheritanceResolver.cpp
    core/ScopedTokenMap.cpp
    core/ContrastRatioValidator.cpp
    core/ColorBlindnessSimulator.cpp
    core/ThemeCompatibility.cpp
    core/ThemeAutoRepair.cpp
    core/ThemeExporter.cpp
    core/ThemePerformanceMonitor.cpp
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
    core/LanguagePack.cpp
    core/SemanticTokenEngine.cpp
    core/ShortcutManager.cpp
    core/CommandRegistry.cpp
    core/WorkbenchCommands.cpp
    core/CommandFeedback.cpp
    core/ShellLayoutState.cpp
    core/WorkspaceOpenOrchestrator.cpp
    core/WorkbenchShellController.cpp
    # V20 Phase 01: Artifact Creation Spine
    core/ArtifactRegistry.cpp
    core/ArtifactCreationService.cpp
    core/ArtifactMountAdapter.cpp
    core/ArtifactNamingPolicy.cpp
    # V20 Phase 02: File Creation & Text Artifact Lifecycle
    core/UnsavedDocumentBuffer.cpp
    core/TextArtifactLifecycle.cpp
    core/EditorMountContext.cpp
    core/FileEntryPointResolver.cpp
    # V20 Phase 03: Notebook Creation & Computational Document Flows
    core/NotebookArtifactLifecycle.cpp
    core/NotebookStorageResolver.cpp
    core/NotebookCommandRouter.cpp
    # V20 Phase 04: Canvas Creation & Board Workbench Integration
    core/CanvasArtifactLifecycle.cpp
    core/CanvasEventBridge.cpp
    core/CanvasShellIntegration.cpp
    # V20 Phase 05: Persistence, Autosave, Restore & Recovery
    core/PersistenceOutcome.cpp
    core/AtomicWriteService.cpp
    core/SessionRestoreService.cpp
    # V20 Phase 06: Commands, Context, Focus & Surface Entry Points
    core/CommandModel.cpp
    core/ActiveContextService.cpp
    core/EntryPointNormalizer.cpp
    # V20 Phase 07: Syntax Highlighting & Language Presentation
    core/LanguageService.cpp
    core/TokenThemeMapper.cpp
    core/HighlightDegradation.cpp
    # V20 Phase 08: Editor, Notebook, Canvas Rendering & Content Styling
    core/ContentStyleSystem.cpp
    core/NotebookCellChrome.cpp
    core/CanvasContentPresenter.cpp
    # V20 Phase 09: Premium Visual System Polish & MUI Icon Completion
    core/ShellVisualSystem.cpp
    core/IconSemanticMapper.cpp
    core/AccessibilityAuditGate.cpp
    # V20 Phase 10: Diagnostics, Validation Harnesses & Consolidation
    core/LifecycleTracer.cpp
    core/WorkflowSmokeRunner.cpp
    core/ExecutionScoreboard.cpp
    core/CommandDispatchAdapter.cpp
    core/DirtyStateCoordinator.cpp
    core/WorkspaceSessionRestore.cpp
    core/DocumentPersistenceOrchestrator.cpp
    core/EditorViewModeManager.cpp
    core/EditorCommandAdapter.cpp
    core/ActiveDocumentTracker.cpp
    core/EditorMetadataPublisher.cpp
    core/FileOperationService.cpp
    core/SearchNavigationContract.cpp
    core/QuickOpenIntegration.cpp
    core/SidebarStateSynchronizer.cpp
    core/WorkspaceLoadStateModel.cpp
    core/SettingsDeepLinkRouter.cpp
    core/SettingsCatalogBootstrap.cpp
    core/LiveSettingsReactor.cpp
    core/SettingsStateOwner.cpp
    core/SettingsScopeResolver.cpp
    # V21 Phase 01: Control Audit Spine & Canonical Action Model
    core/ControlActionManifest.cpp
    core/ControlExecutionTracer.cpp
    core/ControlAuditReport.cpp
    # V21 Phase 02: Menu / Palette / Shortcut Parity
    core/MenuCommandBinder.cpp
    core/PaletteManifestSync.cpp
    # V21 Phase 03: Toolbar, Status Bar & Global Button Wiring
    core/ToolbarCommandBinder.cpp
    core/StatusBarActionRouter.cpp
    # V21 Phase 04: Context Menus, Inline Actions & Target-Aware Enablement
    core/ContextMenuActionBinder.cpp
    core/InlineActionRouter.cpp
    # V21 Phase 05: Panel Registry, Primary Sidebar & Controlled Surface Replacement
    core/PanelLifecycleAuditor.cpp
    core/SidebarStateContract.cpp
    # V21 Phase 06: Secondary, Bottom & Specialized Panel Lifecycle
    core/PanelCapabilityModel.cpp
    # V21 Phase 07: Workbench Surface Completion
    core/SurfaceActionAuditor.cpp
    # V21 Phase 08: Settings Architecture & Schema Consolidation
    core/SettingsArchitectureAuditor.cpp
    # V21 Phase 09: Settings UI & JSON Editor
    core/SettingsStagedContract.cpp
    # V21 Phase 10: Validation Harnesses & Dead UI Removal
    core/ControlCompletenessMatrix.cpp
    # V23 Phase 01: Completion Inventory & Severity Policy
    core/CompletionInventory.cpp
    core/CompletionSeverityPolicy.cpp
    # V23 Phase 02: Action & Panel Readiness Gate
    core/ActionReadinessGate.cpp
    # V23 Phase 03: Lifecycle Completion Tracker
    core/LifecycleCompletionTracker.cpp
    # V23 Phase 04: Persistence Completion Auditor
    core/PersistenceCompletionAuditor.cpp
    # V23 Phase 05: Editor Feature Completion Auditor
    core/EditorFeatureCompletionAuditor.cpp
    # V23 Phase 06: Notebook Execution Completion Auditor
    core/NotebookExecutionCompletionAuditor.cpp
    # V23 Phase 07: Canvas Completion Auditor
    core/CanvasCompletionAuditor.cpp
    # V23 Phase 08: Specialized Surface Completion Auditor
    core/SpecializedSurfaceCompletionAuditor.cpp
    # V23 Phase 09: Project Workflow Completion Auditor
    core/ProjectWorkflowCompletionAuditor.cpp
    # V23 Phase 10: Settings Completion Auditor
    core/SettingsCompletionAuditor.cpp
    # V23 Phase 11: Plugin Marketplace Completion Auditor
    core/PluginMarketplaceCompletionAuditor.cpp
    # V23 Phase 12: Cloud Security Completion Auditor
    core/CloudSecurityCompletionAuditor.cpp
    # V23 Phase 13: Rendering Output Completion Auditor
    core/RenderingOutputCompletionAuditor.cpp
    # V23 Phase 14: Node Editor Completion Auditor
    core/NodeEditorCompletionAuditor.cpp
    # V23 Phase 15: Structured Data Completion Auditor
    core/StructuredDataCompletionAuditor.cpp
    # V23 Phase 16: Platform Accessibility Completion Auditor
    core/PlatformAccessibilityCompletionAuditor.cpp
    # V23 Phase 17: Service Stub Completion Auditor
    core/ServiceStubCompletionAuditor.cpp
    # V23 Phase 18: Feature Flag Migration Completion Auditor
    core/FeatureFlagMigrationCompletionAuditor.cpp
    # V23 Phase 19: Testing Regression Completion Auditor
    core/TestingRegressionCompletionAuditor.cpp
    # V23 Phase 20: Release Gate Completion Auditor
    core/ReleaseGateCompletionAuditor.cpp
    core/CanvasWorkbenchMode.cpp
    core/CanvasShellStateModel.cpp
    core/CanvasSessionContext.cpp
    core/CanvasNavigationIntegration.cpp
    core/CrossSurfaceNavigator.cpp
    core/NotebookShellHost.cpp
    core/NotebookDocumentLifecycle.cpp
    core/NotebookCellController.cpp
    core/NotebookOutputIntegration.cpp
    core/NotebookCommandProvider.cpp
    core/FeedbackTaxonomy.cpp
    core/StatusBarAdapter.cpp
    core/PanelStateModel.cpp
    core/IntegrationTracer.cpp
    core/ShellAccessibilityPolicy.cpp
    core/IconInventory.cpp
    core/MuiIconPipeline.cpp
    core/MuiIconMigration.cpp
    core/IconMetricsPolicy.cpp
    core/IconValidationGate.cpp
    # V27 Phase 01-03: Icon Audit, Migration Tracker, Visual Overhaul
    core/V27IconAuditReport.cpp
    core/V27IconMigrationTracker.cpp
    # V27 Phase 17: Theme Audit
    core/V27ThemeAudit.cpp
    # V27 Phase 20: QA Matrix
    core/V27QAMatrix.cpp
    core/ChordShortcutManager.cpp
    core/CommandExecutionLog.cpp
    core/KeybindingEditor.cpp
    core/QuickOpenProvider.cpp
    core/BookmarkNavigator.cpp
    core/BookmarkExporter.cpp
    core/AssetOptimizer.cpp
    core/AssetLinkIntegrity.cpp
    core/BookmarkCommandProvider.cpp
    core/AssetCommandProvider.cpp
    core/CssSnippetLibrary.cpp
    core/WorkspaceProfile.cpp
    core/CustomFontRegistry.cpp
    core/ThemeOverrideService.cpp
    core/VaultStyleCommandProvider.cpp
    core/WorkspaceCustomizationCommandProvider.cpp
    core/NotificationCenter.cpp
    core/NotificationFilter.cpp
    core/ActivityFeed.cpp
    core/ActivityTimeline.cpp
    core/NotificationCommandProvider.cpp
    core/ActivityCommandProvider.cpp
    core/WorkspaceHookManager.cpp
    core/AutomationRule.cpp
    core/TaskScheduler.cpp
    core/FileWatcherService.cpp
    core/AutomationCommandProvider.cpp
    core/HooksCommandProvider.cpp
    core/SidebarWidgetManager.cpp
    core/WidgetContainer.cpp
    core/WidgetDataProvider.cpp
    core/CustomPanelRegistry.cpp
    core/WidgetCommandProvider.cpp
    core/SidebarCommandProvider.cpp
    core/PdfViewerService.cpp
    core/DocumentImporter.cpp
    core/PdfAnnotationManager.cpp
    core/ImportPresetRegistry.cpp
    core/PdfCommandProvider.cpp
    core/ImportCommandProvider.cpp
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
    rendering/JsonRenderer.cpp
    rendering/ScriptRenderer.cpp

    # V8 Phase 13: FX Engine
    rendering/FxEngine.cpp
    rendering/FxMotionPreset.cpp
    rendering/TextFxRenderer.cpp
    rendering/FxPresetRegistry.cpp
    rendering/FxSafetyController.cpp
    # Phase 28: FX Visual Effects System
    rendering/FxTransitionEngine.cpp
    rendering/FxCssGenerator.cpp
    rendering/FxProfilePersistence.cpp
    rendering/FxDiagnostics.cpp
    rendering/FxCommandProvider.cpp
    rendering/FxAccessibility.cpp
    # Phase 29: Security & Input Validation
    core/UrlSanitizer.cpp
    core/ContentSecurityPolicy.cpp
    core/InputRateLimiter.cpp
    core/ClipboardSanitizer.cpp
    core/SecurityAuditLog.cpp
    core/SecurityCommandProvider.cpp
    # V9 Phase 30: Performance Optimization
    core/PerformanceProfiler.cpp
    core/MemoryBudgetEnforcer.cpp
    core/IncrementalParsePolicy.cpp
    core/PerformanceCommandProvider.cpp
    # V9 Phase 31: Accessibility System
    core/FocusRingStyler.cpp
    core/ColorBlindnessSupport.cpp
    core/AccessibilityAudit.cpp
    core/AccessibilityCommandProvider.cpp
    ui/accessibility/AccessibilityTokenMapper.cpp
    ui/accessibility/AccessibilityAuditor.cpp
    # V9 Phase 32: Platform-Specific Optimization
    core/PlatformServices.cpp
    core/PlatformKeyboard.cpp
    core/PlatformFonts.cpp
    core/CrossPlatformPath.cpp
    # V9 Phase 33: Structured Logging & Observability
    core/ObservabilityService.cpp
    core/TracingService.cpp
    core/ObservabilityCommands.cpp
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
    core/av/AVFormulaEngine.cpp
    core/av/AVImportExport.cpp
    core/av/AVUndoManager.cpp
    core/av/AVAggregateRow.cpp
    core/av/AVTimelineView.cpp
    core/av/AVLinkedDatabase.cpp
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
    # V9 Phase 22: Flashcard & Spaced Repetition
    core/fsrs/FlashcardExtractor.cpp
    core/fsrs/ClozeParser.cpp
    core/fsrs/FlashcardImportExport.cpp
    core/fsrs/StudyStreakTracker.cpp
    core/fsrs/LeechDetector.cpp
    core/fsrs/FlashcardCommandProvider.cpp
    # Phase 23: Task Management & Calendar
    core/TaskRecurrence.cpp
    core/TaskReminder.cpp
    core/TaskAggregator.cpp
    core/TaskBoardEngine.cpp
    core/TaskGanttEngine.cpp
    core/TaskCommandProvider.cpp
    # Phase 24: Export & Publishing
    core/ExportService.cpp
    core/ExportTemplateEngine.cpp
    core/BatchExportEngine.cpp
    core/PrintPreparationService.cpp
    core/PublishingProfileManager.cpp
    core/ExportValidator.cpp
    core/ExportCommandProvider.cpp
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
    core/SearchEngine.h
    core/WorkspaceSearchEngine.cpp
    core/WorkspaceSearchEngine.h
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
    core/GitCommandRunner.cpp
    core/GitCommandRunner.h
    # Phase 25: Version Control Integration
    core/GitStashService.cpp
    core/GitBlameEngine.cpp
    core/GitRemoteService.cpp
    core/MergeConflictResolver.cpp
    core/CommitGraphEngine.cpp
    core/VCCommandProvider.cpp
    # Phase 26: AI Integration
    core/AIService.cpp
    core/AIProviderConfig.cpp
    core/AIRateLimiter.cpp
    core/AIConversationHistory.cpp
    core/AIWritingAssistant.cpp
    core/AIDocumentGenerator.cpp
    core/AIInlineCompleter.cpp
    core/AITagSuggester.cpp
    core/AILinkSuggester.cpp
    core/AIFlashcardGenerator.cpp
    core/AISearchEnhancer.cpp
    core/AICommandProvider.cpp
    # Phase 27: Cloud Sync & Collaboration
    core/SyncEngine.cpp
    core/CloudSyncService.cpp
    core/S3Client.cpp
    core/WebDavClient.cpp
    core/SyncScheduler.cpp
    core/SyncConflictManager.cpp
    core/SelectiveSyncFilter.cpp
    core/SyncHistoryLogger.cpp
    core/WorkspaceSharing.cpp
    core/SyncCommandProvider.cpp
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
    # V10 Phase 41-45: Canvas Input & Drawing Models
    canvas/CanvasInputModel.cpp
    canvas/CanvasToolStateMachine.cpp
    canvas/PointerEventRouter.cpp
    canvas/StrokeSmoother.cpp
    canvas/StrokeStylePreset.cpp
    canvas/ShapePrimitiveSet.cpp
    canvas/GeometryHandleController.cpp
    canvas/ConnectorAnchorModel.cpp
    canvas/ConnectorRouteEngine.cpp
    canvas/CanvasTextEditor.cpp
    canvas/TextStylePreset.cpp
    canvas/CanvasColorToken.cpp
    canvas/ColorPaletteController.cpp
    canvas/StrokeBorderStyle.cpp
    canvas/FillEffectsController.cpp
    canvas/SelectionSemantics.cpp
    canvas/TransformAlignController.cpp
    canvas/SnapTargetEngine.cpp
    canvas/GridGuideController.cpp
    canvas/FreehandModel.cpp
    canvas/ShapeModel.cpp
    canvas/ConnectorModel.cpp
    canvas/CanvasTextModel.cpp
    # V10 Phase 46-50: Canvas Styling Models
    canvas/CanvasColorModel.cpp
    canvas/StyleModel.cpp
    canvas/SelectionTransformModel.cpp
    canvas/SnappingModel.cpp
    canvas/LayeringModel.cpp
    # V10 Phase 51-55: Canvas Object Models
    canvas/FrameContainerModel.cpp
    canvas/StickyNoteModel.cpp
    canvas/ImageAssetModel.cpp
    canvas/MediaEmbedModel.cpp
    canvas/PdfObjectModel.cpp
    # V10 Phase 56-60: Canvas Data Object Models
    canvas/TableModel.cpp
    canvas/StencilModel.cpp
    canvas/MindMapModel.cpp
    canvas/KanbanModel.cpp
    canvas/InspectorModel.cpp
    # V10 Phase 61-65: Canvas Chrome Models
    canvas/ToolRailModel.cpp
    canvas/ContextMenuModel.cpp
    canvas/CanvasSearchModel.cpp
    canvas/MinimapModel.cpp
    canvas/CameraModel.cpp
    # V10 Phase 66-70: Canvas Data & Performance Models
    canvas/ClipboardModel.cpp
    canvas/UndoRedoModel.cpp
    canvas/BoardSerializerModel.cpp
    canvas/LargeBoardModel.cpp
    canvas/HitTestModel.cpp
    # V10 Phase 71-75: Canvas Completeness Models
    canvas/ExportModel.cpp
    canvas/KeyboardCommandModel.cpp
    canvas/AccessibilityModel.cpp
    canvas/CanvasThemeModel.cpp
    canvas/AutoLayoutModel.cpp
    # V10 Phase 76-80: Canvas Quality & Audit Models
    canvas/IntegrityModel.cpp
    canvas/BenchmarkModel.cpp
    canvas/OnboardingModel.cpp
    canvas/PolishModel.cpp
    canvas/AuditModel.cpp
    # Phase 01 W11-W20: Canvas Workbench Models
    canvas/BoardSettingsModel.cpp
    canvas/CanvasAnalyticsModel.cpp
    canvas/DragAutoscrollModel.cpp
    canvas/PenEngineModel.cpp
    canvas/PenPresetModel.cpp
    canvas/PrecisionNudgeModel.cpp
    canvas/SmartSpacingModel.cpp
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
    # Phase 11: Canvas Workbench Shell
    canvas/CanvasWorkbench.cpp
    canvas/CanvasInspector.cpp
    canvas/CanvasObjectFactory.cpp
    canvas/CanvasContextMenu.cpp
    canvas/CanvasPerformanceMonitor.cpp
    canvas/CanvasAccessibility.cpp
    canvas/CanvasClipboardService.cpp
    canvas/CanvasStylePresetService.cpp
    canvas/ConnectorRoutingService.cpp
    canvas/DiagramCoordinator.cpp
    canvas/CanvasObjectLifecycleEvents.cpp
    canvas/CanvasExportService.cpp
    canvas/CanvasObjectAligner.cpp
    # Phase 13: Canvas Collaboration & Facilitation
    canvas/RemoteCursorOverlay.cpp
    canvas/RemoteSelectionOverlay.cpp
    canvas/FollowModeController.cpp
    canvas/BoardLockController.cpp
    canvas/PrivateRevealController.cpp
    canvas/CollabUndoRedoManager.cpp
    canvas/CollabEventLogger.cpp
    canvas/OfflineSyncQueue.cpp
    # Phase 14: Canvas Extensibility & App Widgets
    canvas/CustomObjectTypeRegistry.cpp
    canvas/CanvasToolHost.cpp
    canvas/WidgetRenderer.cpp
    canvas/WidgetLifecycleManager.cpp
    canvas/WidgetPermissionGuard.cpp
    canvas/CanvasTemplateEngine.cpp
    canvas/WidgetMessageBus.cpp
    # Phase 15: Notebook System Completion
    core/NotebookSerializer.cpp
    core/NotebookExecutionPipeline.cpp
    core/NotebookSearchIndex.cpp
    core/NotebookSessionManager.cpp
    core/CellDependencyTracker.cpp
    core/NotebookToolbar.cpp
    # Phase 20: File Management & Workspace
    core/FileTreeModel.cpp
    core/WorkspaceSettings.cpp
    core/FileTemplateEngine.cpp
    core/PanelService.cpp
    core/TrashManager.cpp
    core/MultiRootWorkspace.cpp
    # V13 Phase 37: Compiler Configuration Panel
    core/CompilerConfig.cpp
    core/CompilerDetector.cpp
    core/CompilerConfigValidator.cpp
    core/CompilerProfileManager.cpp
    # V13 Phase 38: Build System Integration
    core/CMakeIntegration.cpp
    core/MakeIntegration.cpp
    core/BuildOutputParser.cpp
    core/BuildTaskManager.cpp
    core/BuildDependencyGraph.cpp
    core/IncrementalBuildTracker.cpp
    core/BuildLogManager.cpp
    # V13 Phase 40: Workspace & Project Configuration
    core/WorkspaceConfig.cpp
    core/WorkspaceTrustService.cpp
    core/RecentWorkspacesManager.cpp
    core/WorkspaceEnvironment.cpp
    # V13 Phase 46: Diff Editor and Merge Tool
    core/WordDiffEngine.cpp
    core/MergeEngine.cpp
    ui/DiffGutter.cpp
    ui/SideBySideDiffView.cpp
    ui/InlineDiffView.cpp
    ui/MergeEditor.cpp
    # V13 Phase 47: Peek View System
    core/PeekProvider.cpp
    ui/PeekView.cpp
    ui/PeekResultList.cpp
    ui/PeekEditorHost.cpp
    # V13 Phase 48: Welcome and Onboarding
    ui/WelcomeTab.cpp
    ui/RecentItemsList.cpp
    ui/InteractiveTutorial.cpp
    ui/ChangelogPanel.cpp
    ui/TipOfTheDay.cpp
    ui/SpotlightOverlay.cpp
    ui/PromptHost.cpp
    # V13 Phase 49: Performance Dashboard
    core/PerformanceCounters.cpp
    # V13 Phase 50: UI Integration Testing Harness
    testing/VisualRegressionRunner.cpp
    testing/UIAutomation.cpp
    testing/AccessibilityAuditor.cpp
    testing/ThemeCoverageChecker.cpp
    testing/MemoryLeakDetector.cpp
)

# Platform-specific sources
if(APPLE)
    target_sources(markamp PRIVATE platform/MacPlatform.mm platform/MacScreenReaderBridge.mm platform/MacAccessibilityIdentifier.mm)
    # Enable Objective-C++ for .mm files
    set_source_files_properties(platform/MacPlatform.mm platform/MacScreenReaderBridge.mm platform/MacAccessibilityIdentifier.mm PROPERTIES
        COMPILE_FLAGS "-x objective-c++"
    )
elseif(WIN32)
    target_sources(markamp PRIVATE platform/WinPlatform.cpp platform/WinScreenReaderBridge.cpp platform/StubAccessibilityIdentifier.cpp)
elseif(UNIX)
    target_sources(markamp PRIVATE platform/LinuxPlatform.cpp platform/LinuxScreenReaderBridge.cpp platform/StubAccessibilityIdentifier.cpp)
endif()

project(MarkAmp VERSION 2.22.69 LANGUAGES CXX)

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
    ui/ColorPaletteGenerator.cpp
    ui/animation/EasingFunctions.cpp
    ui/animation/ValueInterpolator.cpp
    ui/animation/SpringPhysics.cpp
    ui/animation/AnimationTimeline.cpp
    ui/animation/TransitionManager.cpp
    ui/ThemeEditorPanel.cpp
    ui/ColorBlindnessPreviewOverlay.cpp
    ui/SvgDocument.h
    ui/SvgDocument.cpp
    ui/IconRenderer.h
    ui/IconRenderer.cpp
    ui/IconCache.h
    ui/IconCache.cpp
    ui/IconRegistry.h
    ui/IconRegistry.cpp
    ui/IconManager.h
    ui/IconManager.cpp
    ui/IconLibrary.h
    ui/IconGalleryDialog.cpp
    ui/IconLibrary.cpp
    ui/FileTypeIconResolver.h
    ui/FileTypeIconResolver.cpp
    ui/ColorUtils.h
    ui/ColorUtils.cpp
    ui/ComponentSizeResolver.h
    ui/ComponentSizeResolver.cpp
    ui/DensityProfile.h
    ui/DesignTokenRegistry.h
    ui/DesignTokenRegistry.cpp
    ui/DesignTokenValue.h
    ui/DesignTokenValue.cpp
    ui/ElevationSystem.h
    ui/ElevationSystem.cpp
    ui/LayoutMetrics.h
    ui/LayoutMetrics.cpp
    ui/SpacingGrid.h
    ui/SpacingGrid.cpp
    ui/TypographyScale.h
    ui/TypographyScale.cpp
    ui/ThemedScrollbar.h
    ui/ThemedScrollbar.cpp
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
    ui/DiffPanel.cpp
    ui/PreviewPanel.h
    ui/PreviewPanel.cpp
    ui/SplitView.h
    ui/SplitView.cpp
    ui/EditorGroupManager.h
    ui/EditorGroupManager.cpp
    ui/Toolbar.cpp
    ui/RunConfigDropdown.h
    ui/RunConfigDropdown.cpp
    ui/TooltipWindow.h
    ui/TooltipWindow.cpp
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
    ui/LightbulbWidget.h
    ui/LightbulbWidget.cpp
    ui/CodeActionMenu.h
    ui/CodeActionMenu.cpp
    ui/CodeActionPreview.h
    ui/CodeActionPreview.cpp
    ui/FloatingToolbar.h
    ui/FloatingToolbar.cpp
    ui/FloatingToolbarManager.h
    ui/FloatingToolbarManager.cpp
    ui/DebugToolbar.h
    ui/DebugToolbar.cpp
    ui/SelectionActionBar.h
    ui/SelectionActionBar.cpp
    ui/MinimapHoverToolbar.h
    ui/MinimapHoverToolbar.cpp
    ui/ImagePreviewToolbar.h
    ui/ImagePreviewToolbar.cpp
    # V13 Phase 31: Command Palette V2
    core/FuzzyScorer.h
    core/FuzzyScorer.cpp
    core/FrecencyTracker.h
    core/FrecencyTracker.cpp
    core/ISymbolProvider.h
    core/MarkdownSymbolProvider.h
    core/MarkdownSymbolProvider.cpp
    ui/FileIconResolver.h
    ui/IconProvider.h
    ui/IconProvider.cpp
    # V13 Phase 32: Go-To System
    core/NavigationService.h
    core/NavigationService.cpp
    core/LinkResolver.h
    core/LinkResolver.cpp
    core/BacklinkFinder.h
    core/BacklinkFinder.cpp
    core/ChangeTracker.h
    core/ChangeTracker.cpp
    core/WorkspaceSymbolIndex.h
    core/WorkspaceSymbolIndex.cpp
    ui/NavigationHistoryPanel.h
    ui/NavigationHistoryPanel.cpp
    ui/LinkPreviewPopover.h
    ui/LinkPreviewPopover.cpp
    ui/ImagePreviewPopover.h
    ui/ImagePreviewPopover.cpp
    ui/TableEditorOverlay.h
    ui/TableEditorOverlay.cpp
    # V13 Phase 33: Outline Panel
    ui/OutlinePanel.h
    ui/OutlinePanel.cpp
    ui/OutlineIconProvider.h
    # V13 Phase 34: Find & Replace V2
    ui/FindBar.h
    ui/FindBar.cpp
    core/ReplaceUndoBuffer.h
    core/ReplaceUndoBuffer.cpp
    core/PreserveCaseReplacer.h
    core/PreserveCaseReplacer.cpp
    ui/SearchResultsPanel.h
    ui/SearchResultsPanel.cpp
    # V13 Phase 35: Quick Open & File Switching
    core/RecentFilesManager.h
    core/RecentFilesManager.cpp
    core/FileSearchScorer.h
    core/FileSearchScorer.cpp
    core/FileIndex.h
    core/FileIndex.cpp
    ui/TabSwitcherOverlay.h
    ui/TabSwitcherOverlay.cpp
    # V13 Phase 36: Settings UI Overhaul
    ui/RichDescriptionRenderer.h
    ui/RichDescriptionRenderer.cpp
    ui/SettingsCompareView.h
    ui/SettingsCompareView.cpp
    ui/TabBar.h
    ui/TabBar.cpp
    ui/SettingsPanel.h
    ui/SettingsPanel.cpp
    ui/SettingsDialog.h
    ui/SettingsDialog.cpp
    ui/SettingsJsonEditor.h
    ui/SettingsJsonEditor.cpp
    ui/ShortcutEditor.h
    ui/ShortcutEditor.cpp
    ui/FirstRunWizard.h
    ui/FirstRunWizard.cpp
    ui/NotificationManager.h
    ui/NotificationManager.cpp
    ui/ActivityBar.h
    ui/ActivityBar.cpp
    ui/SidebarMode.h
    ui/SidebarPanelRegistry.h
    ui/SidebarPanelRegistry.cpp
    ui/SidebarToolbar.h
    ui/SidebarToolbar.cpp
    ui/SidebarHeader.h
    ui/SidebarHeader.cpp
    ui/PanelHeader.h
    ui/PanelHeader.cpp
    ui/PanelHeaderBar.h
    ui/PanelHeaderBar.cpp
    ui/PanelHeaderIcons.h
    ui/PanelHeaderIcons.cpp
    ui/SidebarFooter.cpp
    ui/SidebarSection.h
    ui/SidebarSection.cpp
    ui/SidebarSkeletonPlaceholder.h
    ui/SidebarSkeletonPlaceholder.cpp
    ui/SearchSidebarPanel.h
    ui/EmptyPanelState.cpp
    ui/SearchSidebarPanel.cpp
    ui/GraphSidebarPanel.h
    ui/GraphSidebarPanel.cpp
    ui/ExtensionsBrowserPanel.h
    ui/ExtensionsBrowserPanel.cpp
    ui/ExplorerPanel.h
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
    core/IHealthCheckable.h
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
    core/ConfigAuditTrail.h
    core/ConfigAuditTrail.cpp
    core/ConfigFileWatcher.h
    core/ConfigFileWatcher.cpp
    core/ConfigMigration.h
    core/ConfigMigration.cpp
    core/ConfigProfile.h
    core/ConfigProfile.cpp
    core/ScopedConfig.h
    core/ScopedConfig.cpp
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
    core/ThemeCompatibility.h
    core/ThemeCompatibility.cpp
    core/ThemeAutoRepair.h
    core/ThemeAutoRepair.cpp
    core/ThemeExporter.h
    core/ThemeExporter.cpp
    core/ThemePerformanceMonitor.h
    core/ThemePerformanceMonitor.cpp
    core/PluginManager.h
    core/PluginManager.cpp
    core/PluginHotReload.h
    core/PluginHotReload.cpp
    core/BuiltInPlugins.h
    core/BuiltInPlugins.cpp
    core/ContributionValidator.h
    core/ContributionValidator.cpp
    core/FeatureRegistry.h
    core/FeatureRegistry.cpp
    # Phase 35: Documentation & Help System
    core/HelpArticle.h
    core/HelpService.h
    core/FeatureDiscoveryService.h
    core/OnboardingService.h
    core/HelpCommandProvider.h
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
    core/CommandRegistry.h
    core/CommandRegistry.cpp
    core/WorkbenchCommands.h
    core/WorkbenchCommands.cpp
    core/CommandFeedback.h
    core/CommandFeedback.cpp
    # V21 Phase 01: Control Audit Spine & Canonical Action Model
    core/ControlActionManifest.h
    core/ControlActionManifest.cpp
    core/ControlExecutionTracer.h
    core/ControlExecutionTracer.cpp
    core/ControlAuditReport.h
    core/ControlAuditReport.cpp
    # V21 Phase 02: Menu / Palette / Shortcut Parity
    core/MenuCommandBinder.h
    core/MenuCommandBinder.cpp
    core/PaletteManifestSync.h
    core/PaletteManifestSync.cpp
    # V21 Phase 03: Toolbar, Status Bar & Global Button Wiring
    core/ToolbarCommandBinder.h
    core/ToolbarCommandBinder.cpp
    core/StatusBarActionRouter.h
    core/StatusBarActionRouter.cpp
    # V21 Phase 04: Context Menus, Inline Actions & Target-Aware Enablement
    core/ContextMenuActionBinder.h
    core/ContextMenuActionBinder.cpp
    core/InlineActionRouter.h
    core/InlineActionRouter.cpp
    # V21 Phase 05: Panel Registry, Primary Sidebar & Controlled Surface Replacement
    core/PanelLifecycleAuditor.h
    core/PanelLifecycleAuditor.cpp
    core/SidebarStateContract.h
    core/SidebarStateContract.cpp
    # V21 Phase 06: Secondary, Bottom & Specialized Panel Lifecycle
    core/PanelCapabilityModel.h
    core/PanelCapabilityModel.cpp
    # V21 Phase 07: Workbench Surface Completion
    core/SurfaceActionAuditor.h
    core/SurfaceActionAuditor.cpp
    # V21 Phase 08: Settings Architecture & Schema Consolidation
    core/SettingsArchitectureAuditor.h
    core/SettingsArchitectureAuditor.cpp
    # V21 Phase 09: Settings UI & JSON Editor
    core/SettingsStagedContract.h
    core/SettingsStagedContract.cpp
    # V21 Phase 10: Validation Harnesses & Dead UI Removal
    core/ControlCompletenessMatrix.h
    core/ControlCompletenessMatrix.cpp
    # V23 Phase 01: Completion Inventory & Severity Policy
    core/CompletionInventory.h
    core/CompletionInventory.cpp
    core/CompletionSeverityPolicy.h
    core/CompletionSeverityPolicy.cpp
    # V23 Phase 02: Action & Panel Readiness Gate
    core/ActionReadinessGate.h
    core/ActionReadinessGate.cpp
    # V23 Phase 03: Lifecycle Completion Tracker
    core/LifecycleCompletionTracker.h
    core/LifecycleCompletionTracker.cpp
    # V23 Phase 04: Persistence Completion Auditor
    core/PersistenceCompletionAuditor.h
    core/PersistenceCompletionAuditor.cpp
    # V23 Phase 05: Editor Feature Completion Auditor
    core/EditorFeatureCompletionAuditor.h
    core/EditorFeatureCompletionAuditor.cpp
    # V23 Phase 06: Notebook Execution Completion Auditor
    core/NotebookExecutionCompletionAuditor.h
    core/NotebookExecutionCompletionAuditor.cpp
    # V23 Phase 07: Canvas Completion Auditor
    core/CanvasCompletionAuditor.h
    core/CanvasCompletionAuditor.cpp
    # V23 Phase 08: Specialized Surface Completion Auditor
    core/SpecializedSurfaceCompletionAuditor.h
    core/SpecializedSurfaceCompletionAuditor.cpp
    # V23 Phase 09: Project Workflow Completion Auditor
    core/ProjectWorkflowCompletionAuditor.h
    core/ProjectWorkflowCompletionAuditor.cpp
    # V23 Phase 10: Settings Completion Auditor
    core/SettingsCompletionAuditor.h
    core/SettingsCompletionAuditor.cpp
    # V23 Phase 11: Plugin Marketplace Completion Auditor
    core/PluginMarketplaceCompletionAuditor.h
    core/PluginMarketplaceCompletionAuditor.cpp
    # V23 Phase 12: Cloud Security Completion Auditor
    core/CloudSecurityCompletionAuditor.h
    core/CloudSecurityCompletionAuditor.cpp
    # V23 Phase 13: Rendering Output Completion Auditor
    core/RenderingOutputCompletionAuditor.h
    core/RenderingOutputCompletionAuditor.cpp
    # V23 Phase 14: Node Editor Completion Auditor
    core/NodeEditorCompletionAuditor.h
    core/NodeEditorCompletionAuditor.cpp
    # V23 Phase 15: Structured Data Completion Auditor
    core/StructuredDataCompletionAuditor.h
    core/StructuredDataCompletionAuditor.cpp
    # V23 Phase 16: Platform Accessibility Completion Auditor
    core/PlatformAccessibilityCompletionAuditor.h
    core/PlatformAccessibilityCompletionAuditor.cpp
    # V23 Phase 17: Service Stub Completion Auditor
    core/ServiceStubCompletionAuditor.h
    core/ServiceStubCompletionAuditor.cpp
    # V23 Phase 18: Feature Flag Migration Completion Auditor
    core/FeatureFlagMigrationCompletionAuditor.h
    core/FeatureFlagMigrationCompletionAuditor.cpp
    # V23 Phase 19: Testing Regression Completion Auditor
    core/TestingRegressionCompletionAuditor.h
    core/TestingRegressionCompletionAuditor.cpp
    # V23 Phase 20: Release Gate Completion Auditor
    core/ReleaseGateCompletionAuditor.h
    core/ReleaseGateCompletionAuditor.cpp
    core/ChordShortcutManager.h
    core/ChordShortcutManager.cpp
    core/QuickOpenProvider.h
    core/QuickOpenProvider.cpp
    core/SymbolProvider.h
    core/KeybindingEditor.h
    core/KeybindingEditor.cpp
    core/CommandExecutionLog.h
    core/CommandExecutionLog.cpp
    core/BookmarkNavigator.h
    core/BookmarkNavigator.cpp
    core/BookmarkExporter.h
    core/BookmarkExporter.cpp
    core/AssetOptimizer.h
    core/AssetOptimizer.cpp
    core/AssetLinkIntegrity.h
    core/AssetLinkIntegrity.cpp
    core/BookmarkCommandProvider.h
    core/BookmarkCommandProvider.cpp
    core/AssetCommandProvider.h
    core/AssetCommandProvider.cpp
    core/CssSnippetLibrary.h
    core/CssSnippetLibrary.cpp
    core/WorkspaceProfile.h
    core/WorkspaceProfile.cpp
    core/CustomFontRegistry.h
    core/CustomFontRegistry.cpp
    core/ThemeOverrideService.h
    core/ThemeOverrideService.cpp
    core/VaultStyleCommandProvider.h
    core/VaultStyleCommandProvider.cpp
    core/WorkspaceCustomizationCommandProvider.h
    core/WorkspaceCustomizationCommandProvider.cpp
    core/NotificationCenter.h
    core/NotificationCenter.cpp
    core/NotificationFilter.h
    core/NotificationFilter.cpp
    core/ActivityFeed.h
    core/ActivityFeed.cpp
    core/ActivityTimeline.h
    core/ActivityTimeline.cpp
    core/NotificationCommandProvider.h
    core/NotificationCommandProvider.cpp
    core/ActivityCommandProvider.h
    core/ActivityCommandProvider.cpp
    core/WorkspaceHookManager.h
    core/WorkspaceHookManager.cpp
    core/AutomationRule.h
    core/AutomationRule.cpp
    core/TaskScheduler.h
    core/TaskScheduler.cpp
    core/FileWatcherService.h
    core/FileWatcherService.cpp
    core/AutomationCommandProvider.h
    core/AutomationCommandProvider.cpp
    core/HooksCommandProvider.h
    core/HooksCommandProvider.cpp
    core/SidebarWidgetManager.h
    core/SidebarWidgetManager.cpp
    core/WidgetContainer.h
    core/WidgetContainer.cpp
    core/WidgetDataProvider.h
    core/WidgetDataProvider.cpp
    core/CustomPanelRegistry.h
    core/CustomPanelRegistry.cpp
    core/WidgetCommandProvider.h
    core/WidgetCommandProvider.cpp
    core/SidebarCommandProvider.h
    core/SidebarCommandProvider.cpp
    core/PdfViewerService.h
    core/PdfViewerService.cpp
    core/DocumentImporter.h
    core/DocumentImporter.cpp
    core/PdfAnnotationManager.h
    core/PdfAnnotationManager.cpp
    core/ImportPresetRegistry.h
    core/ImportPresetRegistry.cpp
    core/PdfCommandProvider.h
    core/PdfCommandProvider.cpp
    core/ImportCommandProvider.h
    core/ImportCommandProvider.cpp
    core/SlideEngine.h
    core/SlideEngine.cpp
    core/PresentationManager.h
    core/PresentationManager.cpp
    core/SlideThemeProvider.h
    core/SlideThemeProvider.cpp
    core/SlideExporter.h
    core/SlideExporter.cpp
    core/PresentationCommandProvider.h
    core/PresentationCommandProvider.cpp
    core/SlideCommandProvider.h
    core/SlideCommandProvider.cpp
    core/VaultEncryptionManager.h
    core/VaultEncryptionManager.cpp
    core/KeyManager.h
    core/KeyManager.cpp
    core/PrivacyManager.h
    core/PrivacyManager.cpp
    core/DataRedactionEngine.h
    core/DataRedactionEngine.cpp
    core/EncryptionCommandProvider.h
    core/EncryptionCommandProvider.cpp
    core/PrivacyCommandProvider.h
    core/PrivacyCommandProvider.cpp
    core/WindowManager.h
    core/WindowManager.cpp
    core/EditorGroupManager.h
    core/EditorGroupManager.cpp
    core/WorkspaceLayoutEngine.h
    core/WorkspaceLayoutEngine.cpp
    core/WorkspaceSessionRestore.h
    core/WorkspaceSessionRestore.cpp
    core/WindowCommandProvider.h
    core/WindowCommandProvider.cpp
    core/LayoutCommandProvider.h
    core/LayoutCommandProvider.cpp
    core/LocaleManager.h
    core/LocaleManager.cpp
    core/TranslationCatalog.h
    core/TranslationCatalog.cpp
    core/StringFormatter.h
    core/StringFormatter.cpp
    core/TextDirectionEngine.h
    core/TextDirectionEngine.cpp
    core/LocalizationCommandProvider.h
    core/LocalizationCommandProvider.cpp
    core/InternationalizationCommandProvider.h
    core/InternationalizationCommandProvider.cpp
    core/DataTableEngine.h
    core/DataTableEngine.cpp
    core/FormulaEvaluator.h
    core/FormulaEvaluator.cpp
    core/DataTransformPipeline.h
    core/DataTransformPipeline.cpp
    core/ChartDataProvider.h
    core/ChartDataProvider.cpp
    core/DataCommandProvider.h
    core/DataCommandProvider.cpp
    core/ComputationCommandProvider.h
    core/ComputationCommandProvider.cpp
    core/IntegrationTestRunner.h
    core/IntegrationTestRunner.cpp
    core/CrossModuleTestSuite.h
    core/CrossModuleTestSuite.cpp
    core/SystemHealthValidator.h
    core/SystemHealthValidator.cpp
    core/RegressionTestEngine.h
    core/RegressionTestEngine.cpp
    core/IntegrationTestCommandProvider.h
    core/IntegrationTestCommandProvider.cpp
    core/ValidationCommandProvider.h
    core/ValidationCommandProvider.cpp
    core/ReleaseNotesGenerator.h
    core/ReleaseNotesGenerator.cpp
    core/VersionManager.h
    core/VersionManager.cpp
    core/ChangelogEngine.h
    core/ChangelogEngine.cpp
    core/DeprecationTracker.h
    core/DeprecationTracker.cpp
    core/ReleaseCommandProvider.h
    core/ReleaseCommandProvider.cpp
    core/PolishCommandProvider.h
    core/PolishCommandProvider.cpp
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
    # V11 Node Editor core
    node_editor/NodeEditorTypes.h
    node_editor/NodeGraph.h
    node_editor/NodeGraph.cpp
    node_editor/NodeDefinition.h
    node_editor/NodeDefinitionRegistry.h
    node_editor/NodeDefinitionRegistry.cpp
    node_editor/GraphSerializer.h
    node_editor/GraphSerializer.cpp
    node_editor/NodeCommand.h
    node_editor/NodeCommand.cpp
    node_editor/NodeCommandStack.h
    node_editor/NodeCommandStack.cpp
    # V11 Node Editor Tier 2
    node_editor/NodeEditorContext.h
    node_editor/NodeEditorContext.cpp
    node_editor/EvaluationIR.h
    node_editor/EvaluationIR.cpp
    node_editor/ExecutionPlan.h
    node_editor/ExecutionPlan.cpp
    node_editor/IDomainRuntime.h
    node_editor/DomainRegistry.h
    node_editor/DomainRegistry.cpp
    node_editor/NodeScheduler.h
    node_editor/NodeScheduler.cpp
    node_editor/NodeValue.h
    node_editor/NodeValue.cpp
    node_editor/ValueConverter.h
    node_editor/ValueConverter.cpp
    node_editor/GraphicsRuntime.h
    node_editor/GraphicsRuntime.cpp
    node_editor/CodeFlowRuntime.h
    node_editor/CodeFlowRuntime.cpp
    node_editor/DataTransformRuntime.h
    node_editor/DataTransformRuntime.cpp
    node_editor/DataGenRuntime.h
    node_editor/DataGenRuntime.cpp
    node_editor/AudioRuntime.h
    node_editor/AudioRuntime.cpp
    node_editor/DomainSDK.h
    node_editor/DomainSDK.cpp
    node_editor/RuntimeSandbox.h
    node_editor/RuntimeSandbox.cpp
    # V11 Node Editor Tier 3
    node_editor/CanvasViewport.h
    node_editor/CanvasViewport.cpp
    node_editor/NodeThemeTokens.h
    node_editor/NodeLayout.h
    node_editor/NodeLayout.cpp
    node_editor/NodeEditorWorkbench.h
    node_editor/NodeEditorWorkbench.cpp
    node_editor/HitTester.h
    node_editor/HitTester.cpp
    node_editor/LinkRouter.h
    node_editor/LinkRouter.cpp
    node_editor/LinkDragState.h
    node_editor/SelectionModel.h
    node_editor/SelectionModel.cpp
    node_editor/DragState.h
    node_editor/NodeSearchProvider.h
    node_editor/NodeSearchProvider.cpp
    node_editor/NodeClipboard.h
    node_editor/NodeClipboard.cpp
    node_editor/NodeAnnotation.h
    node_editor/NodeAnnotation.cpp
    node_editor/NodeGroup.h
    node_editor/NodeGroup.cpp
    node_editor/InspectorModel.h
    node_editor/InspectorModel.cpp
    node_editor/PreviewSystem.h
    node_editor/PreviewSystem.cpp
    node_editor/DebugOverlay.h
    node_editor/DebugOverlay.cpp
    node_editor/DirtyPropagator.h
    node_editor/DirtyPropagator.cpp
    node_editor/ViewportCuller.h
    node_editor/ViewportCuller.cpp
    node_editor/AsyncEvaluator.h
    node_editor/AsyncEvaluator.cpp
    node_editor/NodeDiagnostics.h
    node_editor/NodeDiagnostics.cpp
    node_editor/NodeShortcuts.h
    node_editor/NodeShortcuts.cpp
    node_editor/WorkspacePersistence.h
    node_editor/WorkspacePersistence.cpp
    node_editor/NodeTestHarness.h
    node_editor/NodeTestHarness.cpp
    # ── V11 Node Editor Tier 5 ──
    node_editor/NodePackRegistry.h
    node_editor/NodePackRegistry.cpp
    node_editor/NodeWidgetEmbed.h
    node_editor/NodeWidgetEmbed.cpp
    node_editor/NodeDecorationProvider.h
    node_editor/NodeDecorationProvider.cpp
    node_editor/NodeStatusModel.h
    node_editor/NodeStatusModel.cpp
    node_editor/NodeTemplateLibrary.h
    node_editor/NodeTemplateLibrary.cpp
    node_editor/NodeFormatBridge.h
    node_editor/NodeFormatBridge.cpp
    node_editor/NodeTrustPolicy.h
    node_editor/NodeTrustPolicy.cpp
    node_editor/NodeDevTools.h
    node_editor/NodeDevTools.cpp
    node_editor/NodeMigrationEngine.h
    node_editor/NodeMigrationEngine.cpp
    node_editor/NodeRolloutController.h
    node_editor/NodeRolloutController.cpp
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
