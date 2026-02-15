# Phase 38: Packaging & Distribution (CPack Integration)

## Metadata

| Field | Value |
|---|---|
| Phase ID | 38 |
| Prerequisites | Phase 04 |
| Estimated Complexity | Medium |
| Estimated File Count | 4 created, 3 modified |
| PRD Sections | 8.1 Packaging Formats |

---

## Objective

Create CPack-based packaging for macOS .pkg/.dmg, Windows MSI, Linux .deb/.rpm, and portable tarball. Establish reproducible distribution builds.

---

## Background

The PRD requires: "Produce: macOS .pkg, Windows MSI, Linux .deb, Linux .rpm, Portable binary tarball." The project has packaging skeleton files (`packaging/macos/Info.plist`, `packaging/linux/markamp.desktop`, `packaging/windows/installer.nsi`) but no CPack integration to actually produce distributable packages.

---

## Scope

### Tasks

1. **Create `cmake/Packaging.cmake`**:
   - CPack configuration for all target platforms
   - Common settings:
     - `CPACK_PACKAGE_NAME`: "MarkAmp"
     - `CPACK_PACKAGE_VERSION`: from CMake project version
     - `CPACK_PACKAGE_DESCRIPTION_SUMMARY`: from project description
     - `CPACK_RESOURCE_FILE_LICENSE`: path to LICENSE file
   - macOS:
     - Generator: `DragNDrop` (produces .dmg) or `productbuild` (produces .pkg)
     - Bundle identifier from Info.plist
     - Code signing placeholder (if MARKAMP_CODESIGN_IDENTITY set)
   - Windows:
     - Generator: `WIX` (produces .msi) or `NSIS` (produces .exe installer)
     - Start menu shortcuts, desktop shortcut option
     - Uninstaller registration
   - Linux:
     - Generator: `DEB` (produces .deb) + `RPM` (produces .rpm)
     - Dependencies: wxWidgets runtime libraries
     - .desktop file installation
     - Icon installation
   - Portable:
     - Generator: `TGZ` (produces .tar.gz)
     - Self-contained binary with runtime dependencies

2. **Configure install targets**:
   - Binary: `${CMAKE_INSTALL_BINDIR}/markamp`
   - Resources: themes, config_defaults.json to `${CMAKE_INSTALL_DATADIR}/markamp/`
   - Desktop integration:
     - macOS: Info.plist in .app bundle
     - Linux: .desktop file to `${CMAKE_INSTALL_DATADIR}/applications/`
     - Linux: icons to `${CMAKE_INSTALL_DATADIR}/icons/`
   - Documentation: user guide to `${CMAKE_INSTALL_DOCDIR}/`

3. **Create `packaging/CMakeLists.txt`**:
   - Install rules for all files
   - Platform-conditional installation
   - Resource embedding for macOS .app bundle structure

4. **Update existing packaging files**:
   - `packaging/macos/Info.plist`: verify version substitution (`@PROJECT_VERSION@`)
   - `packaging/linux/markamp.desktop`: verify Exec path, Icon path, Categories
   - `packaging/windows/installer.nsi`: verify sections, paths

5. **Create `scripts/build_packages.sh`**:
   - Configures with `release-lto` preset
   - Builds
   - Runs `cpack` for each target generator
   - Reports package file paths and sizes
   - Verifiable checksums (SHA256)

6. **Create `docs/v6_docs/packaging.md`**:
   - Document all supported package formats
   - How to build each package type
   - Prerequisites per platform
   - Code signing instructions (macOS, Windows)
   - How to test package installation
   - Release checklist

---

## Key Files

| Action | File Path |
|--------|-----------|
| Create | `cmake/Packaging.cmake` |
| Create | `packaging/CMakeLists.txt` |
| Create | `scripts/build_packages.sh` |
| Create | `docs/v6_docs/packaging.md` |
| Modify | `CMakeLists.txt` |
| Modify | `packaging/macos/Info.plist` |
| Modify | `packaging/linux/markamp.desktop` |

---

## Implementation Notes

- CPack is included with CMake. Use `include(CPack)` after setting all `CPACK_*` variables.
- macOS .app bundle structure:
  ```
  MarkAmp.app/
    Contents/
      Info.plist
      MacOS/markamp (binary)
      Resources/ (themes, icons)
  ```
- DEB package dependencies should list `libwxgtk3.0-gtk3-0v5` or equivalent.
- RPM requires `rpmbuild` to be installed. It's not available on all dev machines.
- For portable tarball, include a `README` with runtime dependency instructions.
- Code signing is platform-specific and requires credentials. Make it optional with placeholder.
- Version substitution: use `configure_file()` to replace `@PROJECT_VERSION@` in Info.plist.
- Test installation in a clean environment (VM or container) to verify no missing dependencies.

---

## Acceptance Criteria

- [ ] `cpack -G DragNDrop` produces macOS .dmg on macOS (or `cpack -G TGZ` on any platform)
- [ ] `cpack -G TGZ` produces portable tarball on all platforms
- [ ] `cpack -G DEB` produces .deb on Linux
- [ ] Install targets place files in correct locations
- [ ] `cmake --install build/release-lto --prefix /tmp/test-install` works
- [ ] `packaging.md` documents all supported formats and build instructions
- [ ] Version is correctly embedded in package metadata
- [ ] `build_packages.sh` produces checksums for each package

---

## Testing Strategy

- Build packages on each available platform
- Verify package metadata (version, description, dependencies)
- Install package to temporary location and verify file layout
- Verify binary runs from installed location
- Verify uninstall removes all files (where applicable)
