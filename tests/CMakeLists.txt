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
    ${CMAKE_SOURCE_DIR}/src/rendering/HtmlRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/CodeBlockRenderer.cpp
    ${CMAKE_SOURCE_DIR}/src/rendering/MermaidBlockRenderer.cpp
)
target_include_directories(markamp_core PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(markamp_core PUBLIC markamp_dependencies)

# --- Dependency verification test ---
add_executable(test_dependencies
    unit/test_dependencies.cpp
)
target_link_libraries(test_dependencies PRIVATE
    Catch2::Catch2WithMain
    markamp_dependencies
)

# --- App initialization test ---
add_executable(test_app_init
    unit/test_app_init.cpp
)
target_link_libraries(test_app_init PRIVATE
    Catch2::Catch2WithMain
)

# --- EventBus test ---
add_executable(test_event_bus
    unit/test_event_bus.cpp
)
target_include_directories(test_event_bus PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_event_bus PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)

# --- ServiceRegistry test ---
add_executable(test_service_registry
    unit/test_service_registry.cpp
)
target_include_directories(test_service_registry PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_service_registry PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)

# --- AppState test ---
add_executable(test_app_state
    unit/test_app_state.cpp
)
target_include_directories(test_app_state PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_app_state PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)

# --- CommandHistory test ---
add_executable(test_command_history
    unit/test_command_history.cpp
)
target_include_directories(test_command_history PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_command_history PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)

# --- Config test ---
add_executable(test_config
    unit/test_config.cpp
)
target_include_directories(test_config PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_config PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)

# --- BevelPanel test ---
add_executable(test_bevel_panel
    unit/test_bevel_panel.cpp
)
target_include_directories(test_bevel_panel PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_bevel_panel PRIVATE
    Catch2::Catch2WithMain
    markamp_dependencies
)

# --- CustomChrome test ---
add_executable(test_custom_chrome
    unit/test_custom_chrome.cpp
)
target_include_directories(test_custom_chrome PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_custom_chrome PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)

# --- Theme test ---
add_executable(test_theme
    unit/test_theme.cpp
)
target_include_directories(test_theme PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_theme PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)

# --- ThemeEngine test ---
add_executable(test_theme_engine
    unit/test_theme_engine.cpp
)
target_include_directories(test_theme_engine PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_theme_engine PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)

# Register all with CTest
include(CTest)
add_test(NAME test_dependencies COMMAND test_dependencies)
add_test(NAME test_app_init COMMAND test_app_init)
add_test(NAME test_event_bus COMMAND test_event_bus)
add_test(NAME test_service_registry COMMAND test_service_registry)
add_test(NAME test_app_state COMMAND test_app_state)
add_test(NAME test_command_history COMMAND test_command_history)
add_test(NAME test_config COMMAND test_config)
add_test(NAME test_bevel_panel COMMAND test_bevel_panel)
add_test(NAME test_custom_chrome COMMAND test_custom_chrome)
add_test(NAME test_theme COMMAND test_theme)
add_test(NAME test_theme_engine COMMAND test_theme_engine)

# --- Layout test ---
add_executable(test_layout
    unit/test_layout.cpp
)
target_include_directories(test_layout PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_layout PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_layout COMMAND test_layout)

# --- File tree test ---
add_executable(test_file_tree
    unit/test_file_tree.cpp
)
target_include_directories(test_file_tree PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_file_tree PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_file_tree COMMAND test_file_tree)

# --- File system test ---
add_executable(test_file_system
    unit/test_file_system.cpp
)
target_include_directories(test_file_system PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_file_system PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_file_system COMMAND test_file_system)

# --- Editor test ---
add_executable(test_editor
    unit/test_editor.cpp
)
target_include_directories(test_editor PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_editor PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_editor COMMAND test_editor)

# --- Markdown parser test ---
add_executable(test_markdown_parser
    unit/test_markdown_parser.cpp
)
target_include_directories(test_markdown_parser PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_markdown_parser PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_markdown_parser COMMAND test_markdown_parser)

# --- Preview panel test ---
add_executable(test_preview_panel
    unit/test_preview_panel.cpp
)
target_include_directories(test_preview_panel PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_preview_panel PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_preview_panel COMMAND test_preview_panel)

# --- GFM extensions test ---
add_executable(test_gfm_extensions
    unit/test_gfm_extensions.cpp
)
target_include_directories(test_gfm_extensions PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_gfm_extensions PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_gfm_extensions COMMAND test_gfm_extensions)

# --- Syntax highlighter test ---
add_executable(test_syntax_highlighter
    unit/test_syntax_highlighter.cpp
)
target_include_directories(test_syntax_highlighter PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_syntax_highlighter PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_syntax_highlighter COMMAND test_syntax_highlighter)

# --- Mermaid renderer test ---
add_executable(test_mermaid_renderer
    unit/test_mermaid_renderer.cpp
)
target_include_directories(test_mermaid_renderer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_mermaid_renderer PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_mermaid_renderer COMMAND test_mermaid_renderer)

# --- Preview integration test ---
add_executable(test_preview_integration
    integration/test_preview_integration.cpp
)
target_include_directories(test_preview_integration PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_preview_integration PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_preview_integration COMMAND test_preview_integration)

# --- Split view test ---
add_executable(test_split_view
    unit/test_split_view.cpp
)
target_include_directories(test_split_view PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_split_view PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_split_view COMMAND test_split_view)

# --- Editor enhancements test ---
add_executable(test_editor_enhancements
    unit/test_editor_enhancements.cpp
)
target_include_directories(test_editor_enhancements PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_editor_enhancements PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_editor_enhancements COMMAND test_editor_enhancements)

# --- Theme gallery test ---
add_executable(test_theme_gallery
    unit/test_theme_gallery.cpp
)
target_include_directories(test_theme_gallery PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_theme_gallery PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_theme_gallery COMMAND test_theme_gallery)

# --- Theme import/export test ---
add_executable(test_theme_import_export
    unit/test_theme_import_export.cpp
)
target_include_directories(test_theme_import_export PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_theme_import_export PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_theme_import_export COMMAND test_theme_import_export)

# --- Status bar test ---
add_executable(test_status_bar
    unit/test_status_bar.cpp
)
target_include_directories(test_status_bar PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_status_bar PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_status_bar COMMAND test_status_bar)

# --- Keyboard navigation test ---
add_executable(test_keyboard_navigation
    unit/test_keyboard_navigation.cpp
)
target_include_directories(test_keyboard_navigation PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_keyboard_navigation PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_keyboard_navigation COMMAND test_keyboard_navigation)

# --- Accessibility test ---
add_executable(test_accessibility
    unit/test_accessibility.cpp
)
target_include_directories(test_accessibility PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_accessibility PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_accessibility COMMAND test_accessibility)

# --- Performance benchmark suite ---
add_executable(benchmark_suite
    performance/benchmark_suite.cpp
)
target_include_directories(benchmark_suite PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(benchmark_suite PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME benchmark_suite COMMAND benchmark_suite)

# --- Performance regression tests ---
add_executable(test_performance_regression
    performance/test_performance_regression.cpp
)
target_include_directories(test_performance_regression PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_performance_regression PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_performance_regression COMMAND test_performance_regression)

# --- Security test ---
add_executable(test_security
    unit/test_security.cpp
)
target_include_directories(test_security PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_security PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_security COMMAND test_security)

# --- Cross-platform edge case test ---
add_executable(test_cross_platform
    unit/test_cross_platform.cpp
)
target_include_directories(test_cross_platform PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_cross_platform PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_cross_platform COMMAND test_cross_platform)

# --- End-to-end integration test ---
add_executable(test_e2e
    integration/test_e2e.cpp
)
target_include_directories(test_e2e PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_e2e PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_e2e COMMAND test_e2e)

# --- Editor controls test ---
add_executable(test_editor_controls
    unit/test_editor_controls.cpp
)
target_include_directories(test_editor_controls PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_editor_controls PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_editor_controls COMMAND test_editor_controls)

# --- HTML renderer test ---
add_executable(test_html_renderer
    unit/test_html_renderer.cpp
)
target_include_directories(test_html_renderer PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(test_html_renderer PRIVATE
    Catch2::Catch2WithMain
    markamp_core
)
add_test(NAME test_html_renderer COMMAND test_html_renderer)
