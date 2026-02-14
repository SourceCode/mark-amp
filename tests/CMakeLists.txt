# Test sources live under tests/unit/
# Each test links Catch2 and, when needed, the markamp core sources.

# --- Core source library (shared by tests that test core components) ---
add_library(markamp_core STATIC
    ${CMAKE_SOURCE_DIR}/src/core/EventBus.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AppState.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Command.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Config.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Logger.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Color.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Theme.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BuiltinThemes.cpp
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
    ${CMAKE_SOURCE_DIR}/src/core/MermaidRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/HtmlSanitizer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PieceTable.cpp
    ${CMAKE_SOURCE_DIR}/src/core/LineIndex.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AsyncHighlighter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/AsyncFileLoader.cpp
    ${CMAKE_SOURCE_DIR}/src/core/IncrementalSearcher.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/HtmlRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/CodeBlockRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/loader/ThemeLoader.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/MermaidBlockRenderer.cpp
)
target_include_directories(markamp_core PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(markamp_core PUBLIC markamp_dependencies)

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
    ${CMAKE_SOURCE_DIR}/src/core/PieceTable.cpp
    ${CMAKE_SOURCE_DIR}/src/core/LineIndex.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SyntaxHighlighter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Profiler.cpp
)
target_include_directories(test_performance_infra PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_performance_infra PRIVATE
    Catch2::Catch2WithMain
    markamp_dependencies
)
add_test(NAME test_performance_infra COMMAND test_performance_infra)

# --- Performance Patterns #21-#40 test ---
add_executable(test_perf_patterns_21_40
    unit/test_perf_patterns_21_40.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PieceTable.cpp
    ${CMAKE_SOURCE_DIR}/src/core/LineIndex.cpp
    ${CMAKE_SOURCE_DIR}/src/core/SyntaxHighlighter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Profiler.cpp
)
target_include_directories(test_perf_patterns_21_40 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_perf_patterns_21_40 PRIVATE
    Catch2::Catch2WithMain
    markamp_dependencies
)
add_test(NAME test_perf_patterns_21_40 COMMAND test_perf_patterns_21_40)

# --- Split View Advanced test ---
add_executable(test_split_view_advanced
    unit/test_split_view_advanced.cpp
    ${CMAKE_SOURCE_DIR}/src/core/EventBus.cpp
)
target_include_directories(test_split_view_advanced PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_split_view_advanced PRIVATE
    Catch2::Catch2WithMain
    markamp_dependencies
)
add_test(NAME test_split_view_advanced COMMAND test_split_view_advanced)

# --- Sidebar Filter test ---
add_executable(test_sidebar_filter
    unit/test_sidebar_filter.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FileNode.cpp
)
target_include_directories(test_sidebar_filter PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_sidebar_filter PRIVATE
    Catch2::Catch2WithMain
    markamp_dependencies
)
add_test(NAME test_sidebar_filter COMMAND test_sidebar_filter)

# --- Mermaid Phase 3 test ---
add_executable(test_mermaid_phase3
    unit/test_mermaid_phase3.cpp
    ${CMAKE_SOURCE_DIR}/src/core/MermaidRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Color.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Theme.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/MermaidBlockRenderer.cpp
)
target_include_directories(test_mermaid_phase3 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_mermaid_phase3 PRIVATE
    Catch2::Catch2WithMain
    markamp_dependencies
)
add_test(NAME test_mermaid_phase3 COMMAND test_mermaid_phase3)

# Phase 4: Tokenized Theme Design Engine
add_executable(test_theme_phase4
    unit/test_theme_phase4.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Color.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Theme.cpp
)
target_include_directories(test_theme_phase4 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_theme_phase4 PRIVATE
    Catch2::Catch2WithMain
    markamp_dependencies
)
add_test(NAME test_theme_phase4 COMMAND test_theme_phase4)

# Phase 8: Visual Polish & Spatial Design
add_executable(test_visual_polish
    unit/test_visual_polish.cpp
    ${CMAKE_SOURCE_DIR}/src/core/Color.cpp
)
target_include_directories(test_visual_polish PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_visual_polish PRIVATE
    Catch2::Catch2WithMain
    markamp_dependencies
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
    ${CMAKE_SOURCE_DIR}/src/core/FeatureRegistry.cpp
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
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionManifest.cpp
)
target_include_directories(test_extension_manifest PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_extension_manifest PRIVATE
    Catch2::Catch2WithMain
    markamp_core
    nlohmann_json::nlohmann_json
)
add_test(NAME test_extension_manifest COMMAND test_extension_manifest)

# --- Extension Scanner / Storage / Enablement test ---
add_executable(test_extension_scanner
    unit/test_extension_scanner.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionManifest.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionScanner.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionStorage.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionEnablement.cpp
)
target_include_directories(test_extension_scanner PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_extension_scanner PRIVATE
    Catch2::Catch2WithMain
    markamp_core
    nlohmann_json::nlohmann_json
)
add_test(NAME test_extension_scanner COMMAND test_extension_scanner)

# --- Plugin Manager V2 test ---
add_executable(test_plugin_manager_v2
    unit/test_plugin_manager_v2.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PluginManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionManifest.cpp
)
target_include_directories(test_plugin_manager_v2 PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_plugin_manager_v2 PRIVATE
    Catch2::Catch2WithMain
    markamp_core
    nlohmann_json::nlohmann_json
)
add_test(NAME test_plugin_manager_v2 COMMAND test_plugin_manager_v2)

# --- VSIX Service test ---
add_executable(test_vsix_service
    unit/test_vsix_service.cpp
    ${CMAKE_SOURCE_DIR}/src/core/VsixService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionManifest.cpp
)
target_include_directories(test_vsix_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_vsix_service PRIVATE
    Catch2::Catch2WithMain
    markamp_core
    nlohmann_json::nlohmann_json
    libzip::zip
)
add_test(NAME test_vsix_service COMMAND test_vsix_service)

# --- Gallery Service test ---
add_executable(test_gallery_service
    unit/test_gallery_service.cpp
    ${CMAKE_SOURCE_DIR}/src/core/GalleryService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/HttpClient.cpp
)
target_include_directories(test_gallery_service PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_gallery_service PRIVATE
    Catch2::Catch2WithMain
    markamp_core
    nlohmann_json::nlohmann_json
    httplib::httplib
)
add_test(NAME test_gallery_service COMMAND test_gallery_service)

# --- Extension Management test ---
add_executable(test_extension_management
    unit/test_extension_management.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionManagement.cpp
    ${CMAKE_SOURCE_DIR}/src/core/GalleryService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/HttpClient.cpp
    ${CMAKE_SOURCE_DIR}/src/core/VsixService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionScanner.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionManifest.cpp
)
target_include_directories(test_extension_management PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_extension_management PRIVATE
    Catch2::Catch2WithMain
    markamp_core
    nlohmann_json::nlohmann_json
    libzip::zip
    httplib::httplib
)
add_test(NAME test_extension_management COMMAND test_extension_management)

# --- Built-in Plugins test (Phase 9/10) ---
add_executable(test_builtin_plugins
    unit/test_builtin_plugins.cpp
    ${CMAKE_SOURCE_DIR}/src/core/BuiltInPlugins.cpp
    ${CMAKE_SOURCE_DIR}/src/core/PluginManager.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FeatureRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionManifest.cpp
)
target_include_directories(test_builtin_plugins PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_builtin_plugins PRIVATE
    Catch2::Catch2WithMain
    markamp_core
    nlohmann_json::nlohmann_json
)
add_test(NAME test_builtin_plugins COMMAND test_builtin_plugins)

# --- Context Keys & When-Clause test ---
add_executable(test_context_keys
    unit/test_context_keys.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ContextKeyService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WhenClause.cpp
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
    ${CMAKE_SOURCE_DIR}/src/core/ContextKeyService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/OutputChannelService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/DiagnosticsService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TreeDataProviderRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WebviewService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/DecorationService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FileSystemProviderRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/LanguageProviderRegistry.cpp
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
    ${CMAKE_SOURCE_DIR}/src/core/ContextKeyService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WhenClause.cpp
    ${CMAKE_SOURCE_DIR}/src/core/OutputChannelService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/DiagnosticsService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/TreeDataProviderRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/WebviewService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/DecorationService.cpp
    ${CMAKE_SOURCE_DIR}/src/core/FileSystemProviderRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/LanguageProviderRegistry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionHostRecovery.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionRecommendations.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionTelemetry.cpp
    ${CMAKE_SOURCE_DIR}/src/core/ExtensionSandbox.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/OutputPanel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/ProblemsPanel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/TreeViewHost.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/WebviewHostPanel.cpp
    ${CMAKE_SOURCE_DIR}/src/ui/WalkthroughPanel.cpp
)
target_include_directories(test_extension_integration PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_extension_integration PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_extension_integration COMMAND test_extension_integration)
