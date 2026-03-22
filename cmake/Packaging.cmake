# MarkAmp Packaging Configuration — Phase 38
#
# Multi-platform CPack configuration for generating installers.

include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "MarkAmp")
set(CPACK_PACKAGE_VENDOR "MarkAmp Project")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "High-performance scientific markdown editor")
set(CPACK_PACKAGE_VERSION "${PROJECT_VERSION}")
set(CPACK_PACKAGE_VERSION_MAJOR "${PROJECT_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${PROJECT_VERSION_MINOR}")
set(CPACK_PACKAGE_VERSION_PATCH "${PROJECT_VERSION_PATCH}")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_SOURCE_DIR}/LICENSE")
set(CPACK_RESOURCE_FILE_README "${CMAKE_SOURCE_DIR}/README.md")

# Platform-specific settings
if(APPLE)
    set(CPACK_GENERATOR "DragNDrop;ZIP")
    set(CPACK_DMG_VOLUME_NAME "MarkAmp")
    set(CPACK_BUNDLE_NAME "MarkAmp")
    set(CPACK_BUNDLE_PLIST "${CMAKE_SOURCE_DIR}/packaging/Info.plist")
    set(CPACK_BUNDLE_ICON "${CMAKE_SOURCE_DIR}/resources/markamp.icns")
elseif(WIN32)
    set(CPACK_GENERATOR "NSIS;ZIP")
    set(CPACK_NSIS_DISPLAY_NAME "MarkAmp")
    set(CPACK_NSIS_HELP_LINK "https://github.com/markamp/markamp")
    set(CPACK_NSIS_URL_INFO_ABOUT "https://github.com/markamp/markamp")
    set(CPACK_NSIS_MODIFY_PATH ON)
else()
    set(CPACK_GENERATOR "DEB;RPM;TGZ")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "MarkAmp Project")
    set(CPACK_DEBIAN_PACKAGE_SECTION "editors")
    set(CPACK_DEBIAN_PACKAGE_DEPENDS "libgtk-3-0, libwebkit2gtk-4.0-37")
    set(CPACK_RPM_PACKAGE_LICENSE "MIT")
    set(CPACK_RPM_PACKAGE_GROUP "Applications/Editors")
endif()

# Install targets
install(TARGETS markamp
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    BUNDLE DESTINATION .
)

# Install resources
install(DIRECTORY "${CMAKE_SOURCE_DIR}/resources/"
    DESTINATION share/markamp
    FILES_MATCHING
    PATTERN "*.css"
    PATTERN "*.js"
    PATTERN "*.html"
    PATTERN "*.png"
    PATTERN "*.svg"
    PATTERN "*.icns"
)

# Install documentation
install(FILES
    "${CMAKE_SOURCE_DIR}/README.md"
    "${CMAKE_SOURCE_DIR}/LICENSE"
    "${CMAKE_SOURCE_DIR}/HISTORY.md"
    DESTINATION share/doc/markamp
)

include(CPack)
