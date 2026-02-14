add_executable(markamp
    main.cpp
    app/MarkAmpApp.cpp
    ui/MainFrame.cpp
    ui/CustomChrome.cpp
    ui/BevelPanel.cpp
    ui/ThemeAwareWindow.cpp
    ui/ThemedScrollbar.cpp
    ui/LayoutManager.cpp
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
    # Platform
    platform/PlatformAbstraction.h
    platform/MacPlatform.h
    platform/WinPlatform.h
    platform/LinuxPlatform.h
)

# Apply quality tooling per-target
markamp_set_warnings(markamp)
markamp_configure_sanitizers(markamp)

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
