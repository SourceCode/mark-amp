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
    core/Logger.cpp
    core/EventBus.cpp
    core/AppState.cpp
    core/Command.cpp
    core/Config.cpp
    core/Color.cpp
    core/Theme.cpp
    core/BuiltinThemes.cpp
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
    app/MarkAmpApp.h
    app/MarkAmpApp.cpp
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
    ui/StartupPanel.h
    ui/StartupPanel.cpp
    core/Logger.h
    core/Logger.cpp
    core/EventBus.h
    core/EventBus.cpp
    core/Events.h
    core/Types.h
    core/ServiceRegistry.h
    core/IThemeEngine.h
    core/IFileSystem.h
    core/IMarkdownParser.h
    core/IMermaidRenderer.h
    core/AppState.h
    core/AppState.cpp
    core/Command.h
    core/Command.cpp
    core/Config.h
    core/Config.cpp
    core/RecentWorkspaces.h
    core/RecentWorkspaces.cpp
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
    core/ShortcutManager.h
    core/ShortcutManager.cpp
    core/AccessibilityManager.h
    core/AccessibilityManager.cpp
    core/Profiler.h
    core/Profiler.cpp
    core/ThemeEngine.h
    core/ThemeEngine.cpp
    platform/PlatformAbstraction.h
    platform/MacPlatform.h
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
endif()
