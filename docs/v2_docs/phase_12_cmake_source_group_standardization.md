# Phase 12: CMake Source Group & Build Standardization

**Priority:** Medium
**Estimated Scope:** ~3 files affected
**Dependencies:** None

## Objective

Synchronize the `source_group()` IDE listing with the `add_executable()` build list in `src/CMakeLists.txt`, add the 20+ missing files from the v1.9.12 extension services, and add missing source files to the `markamp_core` test library in `tests/CMakeLists.txt`.

## Background/Context

The `source_group(TREE ...)` block in `src/CMakeLists.txt` (lines 131-219) is used by IDEs (Xcode, Visual Studio) to organize files into logical groups. Currently it only lists files through the original extension panels (ExtensionsBrowserPanel, ExtensionCard, ExtensionDetailPanel) and stops at `platform/MacPlatform.h` (line 218). It is missing:

1. **30+ core files** added in v1.9.12: all extension service `.h/.cpp` pairs (PluginContext.h, FeatureRegistry, ExtensionManifest, ExtensionScanner, ExtensionStorage, ExtensionEnablement, VsixService, HttpClient, GalleryService, ExtensionManagement, ContextKeyService, WhenClause, OutputChannelService, DiagnosticsService, TreeDataProviderRegistry, WebviewService, DecorationService, FileSystemProviderRegistry, LanguageProviderRegistry, NotificationServiceAPI, StatusBarItemService, InputBoxService, QuickPickService, SnippetEngine, WorkspaceService, TextEditorService, ProgressService, ExtensionEvents, EnvironmentService, GrammarEngine, TerminalService, TaskRunnerService, ExtensionHostRecovery, ExtensionRecommendations, ExtensionTelemetry, ExtensionSandbox)

2. **5 UI panel files**: OutputPanel.h/.cpp, ProblemsPanel.h/.cpp, TreeViewHost.h/.cpp, WebviewHostPanel.h/.cpp, WalkthroughPanel.h/.cpp (these ARE in `add_executable()` but NOT in `source_group()`)

3. **10+ UI files**: FileTreeCtrl, EditorPanel, PreviewPanel, TabBar, BreadcrumbBar, FloatingFormatBar, LinkPreviewPopover, ImagePreviewPopover, TableEditorOverlay, CommandPalette -- these are in `add_executable()` but not in `source_group()`

4. **Core files missing from source_group**: MarkdownDocument.cpp, Md4cWrapper, MarkdownParser, SyntaxHighlighter, FileNode, SampleFiles, FileSystem, EncodingDetector, RecentFiles, MermaidRenderer, HtmlSanitizer, PieceTable, LineIndex, AsyncHighlighter, AsyncFileLoader, IncrementalSearcher, loader/ThemeLoader

5. **Rendering files missing**: HtmlRenderer, CodeBlockRenderer, MermaidBlockRenderer

6. **Platform files missing**: WinPlatform.h, LinuxPlatform.h (only MacPlatform.h is listed)

Additionally, the `markamp_core` static library in `tests/CMakeLists.txt` (lines 5-40) is missing the P1-P4 service sources, which means test targets that depend on `markamp_core` cannot link against them.

## Detailed Tasks

### Task 1: Synchronize source_group with add_executable

**File:** `/Users/ryanrentfro/code/markamp/src/CMakeLists.txt`

Replace the existing `source_group(TREE ...)` block (lines 131-219) with a complete listing that mirrors all files in `add_executable()` plus their corresponding headers. Organize by directory:

```cmake
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} PREFIX "Src" FILES
    # App
    main.cpp
    app/MarkAmpApp.h
    app/MarkAmpApp.cpp

    # UI - Core Windows
    ui/MainFrame.h
    ui/MainFrame.cpp
    ui/CustomChrome.h
    ui/CustomChrome.cpp
    ui/LayoutManager.h
    ui/LayoutManager.cpp

    # UI - Panels & Components
    ui/BevelPanel.h
    ui/BevelPanel.cpp
    ui/ThemeAwareWindow.h
    ui/ThemeAwareWindow.cpp
    ui/ThemedScrollbar.h
    ui/ThemedScrollbar.cpp
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
    ui/TabBar.h
    ui/TabBar.cpp
    ui/CommandPalette.h
    ui/CommandPalette.cpp
    ui/BreadcrumbBar.h
    ui/BreadcrumbBar.cpp
    ui/StartupPanel.h
    ui/StartupPanel.cpp
    ui/ShortcutOverlay.h
    ui/ShortcutOverlay.cpp
    ui/FloatingFormatBar.h
    ui/FloatingFormatBar.cpp
    ui/LinkPreviewPopover.h
    ui/LinkPreviewPopover.cpp
    ui/ImagePreviewPopover.h
    ui/ImagePreviewPopover.cpp
    ui/TableEditorOverlay.h
    ui/TableEditorOverlay.cpp

    # UI - Themes
    ui/ThemeGallery.h
    ui/ThemeGallery.cpp
    ui/ThemePreviewCard.h
    ui/ThemePreviewCard.cpp
    ui/ThemeTokenEditor.h
    ui/ThemeTokenEditor.cpp

    # UI - Settings & Notifications
    ui/SettingsPanel.h
    ui/SettingsPanel.cpp
    ui/NotificationManager.h
    ui/NotificationManager.cpp
    ui/ActivityBar.h
    ui/ActivityBar.cpp

    # UI - Extension Panels
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

    # Core - Foundation
    core/Logger.h
    core/Logger.cpp
    core/EventBus.h
    core/EventBus.cpp
    core/Events.h
    core/Types.h
    core/ServiceRegistry.h
    core/AppState.h
    core/AppState.cpp
    core/Command.h
    core/Command.cpp
    core/Config.h
    core/Config.cpp

    # Core - Interfaces
    core/IThemeEngine.h
    core/IFileSystem.h
    core/IMarkdownParser.h
    core/IMermaidRenderer.h
    core/IPlugin.h

    # Core - Theme System
    core/Color.h
    core/Color.cpp
    core/Theme.h
    core/Theme.cpp
    core/BuiltinThemes.h
    core/BuiltinThemes.cpp
    core/ThemeRegistry.h
    core/ThemeRegistry.cpp
    core/ThemeValidator.h
    core/ThemeValidator.cpp
    core/ThemeEngine.h
    core/ThemeEngine.cpp
    core/loader/ThemeLoader.h
    core/loader/ThemeLoader.cpp

    # Core - File System & Documents
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
    core/MarkdownDocument.cpp

    # Core - Markdown & Rendering
    core/Md4cWrapper.h
    core/Md4cWrapper.cpp
    core/MarkdownParser.h
    core/MarkdownParser.cpp
    core/SyntaxHighlighter.h
    core/SyntaxHighlighter.cpp
    core/MermaidRenderer.h
    core/MermaidRenderer.cpp
    core/HtmlSanitizer.h
    core/HtmlSanitizer.cpp

    # Core - Editor Infrastructure
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

    # Core - Utilities
    core/ShortcutManager.h
    core/ShortcutManager.cpp
    core/AccessibilityManager.h
    core/AccessibilityManager.cpp
    core/Profiler.h
    core/Profiler.cpp

    # Core - Plugin System
    core/PluginContext.h
    core/PluginManager.h
    core/PluginManager.cpp
    core/BuiltInPlugins.h
    core/BuiltInPlugins.cpp
    core/FeatureRegistry.h
    core/FeatureRegistry.cpp

    # Core - Extension Management
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
    core/ContextKeyService.h
    core/ContextKeyService.cpp
    core/WhenClause.h
    core/WhenClause.cpp
    core/ExtensionHostRecovery.h
    core/ExtensionHostRecovery.cpp
    core/ExtensionRecommendations.h
    core/ExtensionRecommendations.cpp
    core/ExtensionTelemetry.h
    core/ExtensionTelemetry.cpp
    core/ExtensionSandbox.h
    core/ExtensionSandbox.cpp

    # Core - Contribution Point Registries
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

    # Core - P1-P4 Extension Services
    core/NotificationServiceAPI.h
    core/NotificationServiceAPI.cpp
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

    # Rendering
    rendering/HtmlRenderer.h
    rendering/HtmlRenderer.cpp
    rendering/CodeBlockRenderer.h
    rendering/CodeBlockRenderer.cpp
    rendering/MermaidBlockRenderer.h
    rendering/MermaidBlockRenderer.cpp

    # Platform
    platform/PlatformAbstraction.h
    platform/MacPlatform.h
    platform/WinPlatform.h
    platform/LinuxPlatform.h
)
```

### Task 2: Add P1-P4 services to markamp_core test library

**File:** `/Users/ryanrentfro/code/markamp/tests/CMakeLists.txt`

Add the following to the `markamp_core` static library (after line 39, before the closing `)`):

```cmake
    ${CMAKE_SOURCE_DIR}/src/core/PluginManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FeatureRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BuiltInPlugins.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionManifest.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionScanner.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionStorage.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionEnablement.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ContextKeyService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WhenClause.cpp
    ${CMAKE_SOURCE_DIR}/src/core/OutputChannelService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/DiagnosticsService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TreeDataProviderRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WebviewService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/DecorationService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FileSystemProviderRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/LanguageProviderRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/NotificationServiceAPI.cpp
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
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionHostRecovery.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionRecommendations.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionTelemetry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionSandbox.cpp
```

This allows test targets that link `markamp_core` to access the extension services without manually listing them.

### Task 3: Consolidate test targets to use markamp_core

After Task 2, many test targets that manually list source files could be simplified to just link `markamp_core`. Review `test_p1_p4_services`, `test_extension_services`, `test_extension_integration`, `test_builtin_plugins`, and `test_context_keys` and replace their manual source listings with a `markamp_core` link where possible. This reduces build maintenance burden.

## Acceptance Criteria

1. `source_group()` lists every file from `add_executable()` plus corresponding headers
2. IDE displays all source files in organized groups
3. `markamp_core` test library includes all extension service sources
4. All test targets compile and link
5. Full build succeeds on all platforms

## Testing Requirements

- Full build succeeds (`cmake --build build/debug`)
- All test targets pass (`ctest --output-on-failure`)
- Open in Xcode / VS Code CMake Tools and verify file organization
