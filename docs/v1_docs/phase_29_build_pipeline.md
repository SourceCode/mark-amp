# Phase 29: Build Pipeline, Packaging, and Distribution

## Objective

Create automated build pipelines for all three platforms, produce distributable packages (macOS .dmg, Windows .msi/.exe installer, Linux .deb/.AppImage), implement code signing, and set up CI/CD for continuous builds and testing.

## Prerequisites

- Phase 28 (Cross-Platform Testing and Polish)

## Deliverables

1. CI/CD pipeline configuration (GitHub Actions)
2. macOS: signed .app bundle in .dmg for direct distribution
3. **macOS: Mac App Store package (.pkg) with App Sandbox entitlements**
4. Windows: signed .exe installer (NSIS or WiX) for direct distribution
5. **Windows: MSIX package for Microsoft Store submission**
6. Linux: .deb package, .AppImage, .tar.gz
7. Automated build scripts for each platform
8. Release automation (versioning, changelog, tagging)
9. **App Store metadata, screenshots, and listing assets**

## Tasks

### Task 29.1: Set up GitHub Actions CI

Create `/Users/ryanrentfro/code/markamp/.github/workflows/ci.yml`:

**CI matrix:**
```yaml
strategy:
  matrix:
    os: [macos-latest, windows-latest, ubuntu-latest]
    build_type: [Debug, Release]
```

**CI steps:**
1. Checkout code
2. Install system dependencies (platform-specific)
3. Set up vcpkg (cache the installed packages)
4. Configure CMake with presets
5. Build
6. Run tests
7. Run format check (`clang-format --dry-run --Werror`)
8. Run lint check (`clang-tidy`)
9. Upload build artifacts

**Caching strategy:**
- Cache vcpkg installed packages (key: vcpkg.json hash + OS)
- Cache CMake build directory (key: source hash + OS + build type)

**Acceptance criteria:**
- CI runs on all three platforms
- Debug builds include tests with sanitizers
- Release builds produce distributable binaries
- Format and lint checks are enforced
- Build time is reasonable (under 30 minutes)

### Task 29.2: Create macOS packaging

Create `/Users/ryanrentfro/code/markamp/scripts/package_macos.sh`:

**Package: .app bundle in a .dmg disk image**

**App bundle structure:**
```
MarkAmp.app/
  Contents/
    MacOS/
      markamp (executable)
    Resources/
      markamp.icns
      themes/ (built-in theme JSON files)
      fonts/ (embedded font files)
      sample_files/ (sample markdown files)
    Frameworks/ (bundled wxWidgets dylibs, if not statically linked)
    Info.plist
```

**Info.plist contents:**
- CFBundleName: MarkAmp
- CFBundleVersion: 1.0.0
- CFBundleIdentifier: com.markamp.editor
- CFBundleDocumentTypes: `.md`, `.markdown`, `.mdown`, `.mkd`, `.txt`
- NSHighResolutionCapable: true
- LSMinimumSystemVersion: 12.0

**DMG creation:**
```bash
# Create DMG with background image, icon positioning
hdiutil create -volname "MarkAmp" -srcfolder MarkAmp.app -ov -format UDZO MarkAmp.dmg
```

**Code signing:**
```bash
codesign --deep --force --verify --verbose \
    --sign "Developer ID Application: ..." \
    --options runtime \
    MarkAmp.app
```

**Notarization:**
```bash
xcrun notarytool submit MarkAmp.dmg --apple-id ... --password ... --team-id ...
xcrun stapler staple MarkAmp.dmg
```

**Acceptance criteria:**
- .app bundle runs on macOS 12+
- DMG mounts and shows drag-to-Applications layout
- Code signing is valid
- Notarization passes (no Gatekeeper warnings)
- File associations work (double-click .md opens MarkAmp)

### Task 29.3: Create Windows packaging

Create `/Users/ryanrentfro/code/markamp/scripts/package_windows.sh` (or `package_windows.ps1`):

**Package: NSIS or WiX installer (.exe)**

**Installer features:**
- Installation directory selection (default: `C:\Program Files\MarkAmp\`)
- Desktop shortcut (optional)
- Start menu entry
- File association for `.md`, `.markdown` files
- "Open with MarkAmp" context menu entry
- Uninstaller with clean removal
- Bundled Visual C++ runtime (if not statically linked)

**NSIS script (`installer.nsi`):**
```nsis
Name "MarkAmp"
OutFile "MarkAmp-1.0.0-Setup.exe"
InstallDir "$PROGRAMFILES\MarkAmp"
; ... sections for files, shortcuts, registry entries
```

**Code signing (with Authenticode):**
```bash
signtool sign /f certificate.pfx /p password /t http://timestamp.server /d "MarkAmp" markamp.exe
signtool sign /f certificate.pfx /p password /t http://timestamp.server /d "MarkAmp Setup" MarkAmp-Setup.exe
```

**Portable version:**
Also produce a portable ZIP that can be run without installation:
```
MarkAmp-1.0.0-Portable.zip/
  markamp.exe
  resources/
  README.txt
```

**Acceptance criteria:**
- Installer runs on Windows 10 and 11
- Installation creates all expected shortcuts and file associations
- Uninstaller cleanly removes all files and registry entries
- Code signing eliminates SmartScreen warnings
- Portable version works without installation

### Task 29.4: Create Linux packaging

Create `/Users/ryanrentfro/code/markamp/scripts/package_linux.sh`:

**Package formats:**

**1. Debian package (.deb):**
```
markamp_1.0.0_amd64.deb
```
- Install to `/usr/bin/markamp`, `/usr/share/markamp/`
- Desktop entry: `/usr/share/applications/markamp.desktop`
- Icon: `/usr/share/icons/hicolor/256x256/apps/markamp.png`
- Dependencies: `libgtk-3-0, libwebkit2gtk-4.0-37` (or equivalent)

**markamp.desktop file:**
```ini
[Desktop Entry]
Name=MarkAmp
Comment=Retro-futuristic Markdown editor
Exec=markamp %F
Icon=markamp
Terminal=false
Type=Application
Categories=TextEditor;Development;
MimeType=text/markdown;text/x-markdown;
```

**2. AppImage:**
```
MarkAmp-1.0.0-x86_64.AppImage
```
- Self-contained (bundles all dependencies)
- Portable (no installation required)
- Use `linuxdeployqt` or `linuxdeploy` with the wxWidgets plugin

**3. Tarball (.tar.gz):**
```
MarkAmp-1.0.0-linux-x86_64.tar.gz
```
- Simple archive with binary and resources
- Include an `install.sh` script

**Acceptance criteria:**
- .deb installs on Ubuntu 22.04
- AppImage runs on multiple distributions without installation
- Desktop integration works (file associations, icon)
- Dependencies are correctly declared

### Task 29.5: Create release automation

Create `/Users/ryanrentfro/code/markamp/.github/workflows/release.yml`:

**Trigger:** Push of a version tag (`v*`)

**Release workflow:**
1. Build on all three platforms (parallel)
2. Run full test suite on all platforms
3. Package on all platforms
4. Code sign (using GitHub Secrets for certificates)
5. Create GitHub Release with:
   - Version tag
   - Auto-generated changelog (from git log since last release)
   - Upload all artifacts:
     - `MarkAmp-1.0.0-macOS.dmg`
     - `MarkAmp-1.0.0-Windows-Setup.exe`
     - `MarkAmp-1.0.0-Windows-Portable.zip`
     - `MarkAmp-1.0.0-linux-amd64.deb`
     - `MarkAmp-1.0.0-linux-x86_64.AppImage`
     - `MarkAmp-1.0.0-linux-x86_64.tar.gz`

**Version management:**
Create `/Users/ryanrentfro/code/markamp/scripts/bump_version.sh`:
- Updates version in CMakeLists.txt
- Updates version in Info.plist
- Updates version in installer scripts
- Creates a git tag

**Acceptance criteria:**
- Pushing a version tag triggers the release workflow
- All platform packages are built and uploaded
- GitHub Release is created with all artifacts
- Changelog is auto-generated

### Task 29.6: Create development build scripts

Create convenience scripts for developers:

**`scripts/build_debug.sh`:**
```bash
#!/usr/bin/env bash
cmake --preset debug
cmake --build --preset debug-build --parallel
```

**`scripts/build_release.sh`:**
```bash
#!/usr/bin/env bash
cmake --preset release
cmake --build --preset release-build --parallel
```

**`scripts/run_tests.sh`:**
```bash
#!/usr/bin/env bash
cmake --preset debug
cmake --build --preset debug-build --parallel
ctest --preset debug-test --output-on-failure
```

**`scripts/clean.sh`:**
```bash
#!/usr/bin/env bash
rm -rf build/ out/
```

All scripts should be executable and work on macOS and Linux. Provide `.bat` equivalents for Windows.

**Acceptance criteria:**
- All scripts work on their target platforms
- New developers can build, test, and run with a single script

### Task 29.7: Configure static linking options

For distribution, consider static linking to reduce dependencies:

**wxWidgets static linking:**
- CMake option: `wxWidgets_USE_STATIC=ON`
- Produces a single executable with no wxWidgets DLL dependencies
- Increases binary size but simplifies distribution

**Other static linking:**
- nlohmann/json: header-only (no linking needed)
- md4c: small C library, statically link
- spdlog/fmt: optionally static
- Catch2: test-only, no need to distribute

**Create CMake presets for static builds:**
```json
{
    "name": "release-static",
    "inherits": "release",
    "cacheVariables": {
        "wxWidgets_USE_STATIC": "ON",
        "BUILD_SHARED_LIBS": "OFF"
    }
}
```

**Acceptance criteria:**
- Static build produces a single executable (or minimal DLLs)
- Static build runs correctly on clean systems
- Binary size is documented and reasonable

### Task 29.8: Mac App Store packaging and submission

**CRITICAL**: MarkAmp will be published to the Mac App Store. This requires specific entitlements, sandboxing, and packaging.

**App Sandbox Requirements:**
Create `/Users/ryanrentfro/code/markamp/resources/entitlements/markamp.entitlements`:
```xml
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0">
<dict>
    <key>com.apple.security.app-sandbox</key>
    <true/>
    <key>com.apple.security.files.user-selected.read-write</key>
    <true/>
    <key>com.apple.security.files.bookmarks.app-scope</key>
    <true/>
    <key>com.apple.security.print</key>
    <true/>
</dict>
</plist>
```

**Key sandbox constraints to code for throughout the app:**
- File access ONLY through `NSOpenPanel`/`NSSavePanel` (user-selected files) or app container
- No arbitrary file system access -- use security-scoped bookmarks for "recent files"
- No network access (MarkAmp is local-only, which is good)
- No spawning external processes (affects Mermaid CLI fallback -- must use embedded JS engine)
- Print support requires the print entitlement

**Mac App Store .pkg creation:**
```bash
# Build with App Store provisioning profile
productbuild --component MarkAmp.app /Applications \
    --sign "3rd Party Mac Developer Installer: ..." \
    --product MarkAmp.app/Contents/Info.plist \
    MarkAmp.pkg
```

**App Store Connect requirements:**
- App category: Developer Tools > Text Editors
- Screenshots: 5 required (1280x800 or 2560x1600 for Retina)
- App preview video: optional but recommended (15-30 seconds)
- Description, keywords, support URL, privacy policy URL
- Age rating: 4+ (no objectionable content)
- Bundle ID: `com.markamp.editor` (must match provisioning profile)
- Version string: must follow `X.Y.Z` format
- Minimum macOS version: 12.0+

**Provisioning and signing:**
- Requires Apple Developer Program membership ($99/year)
- Create App ID in Apple Developer portal
- Create Mac App Store provisioning profile
- Sign with "3rd Party Mac Developer Application" certificate
- Package with "3rd Party Mac Developer Installer" certificate

**App Review compliance:**
- App must not crash or hang
- All features must work as described
- No placeholder content or "coming soon" features
- Must handle errors gracefully
- Must respect system preferences (Dark Mode, accessibility)
- Must support macOS standard keyboard shortcuts
- Custom window chrome is allowed but must support standard window management (minimize, fullscreen, close)

**Acceptance criteria:**
- .pkg builds successfully with App Sandbox entitlements
- App launches and functions correctly in sandboxed mode
- File open/save works through system file dialogs only
- No sandbox violations in Console.app
- App Store Connect metadata is prepared
- App passes `xcrun altool --validate-app` check

### Task 29.9: Microsoft Store (MSIX) packaging

**CRITICAL**: MarkAmp will be published to the Microsoft Store. This requires MSIX packaging.

**MSIX package creation:**
Create `/Users/ryanrentfro/code/markamp/packaging/windows/AppxManifest.xml`:
```xml
<?xml version="1.0" encoding="utf-8"?>
<Package xmlns="http://schemas.microsoft.com/appx/manifest/foundation/windows10"
         xmlns:uap="http://schemas.microsoft.com/appx/manifest/uap/windows10"
         xmlns:rescap="http://schemas.microsoft.com/appx/manifest/foundation/windows10/restrictedcapabilities">
    <Identity Name="MarkAmp" Publisher="CN=..." Version="1.0.0.0" />
    <Properties>
        <DisplayName>MarkAmp</DisplayName>
        <PublisherDisplayName>MarkAmp</PublisherDisplayName>
        <Description>Retro-futuristic Markdown editor with Mermaid diagram support</Description>
        <Logo>Assets\StoreLogo.png</Logo>
    </Properties>
    <Dependencies>
        <TargetDeviceFamily Name="Windows.Desktop" MinVersion="10.0.17763.0" MaxVersionTested="10.0.22621.0" />
    </Dependencies>
    <Resources>
        <Resource Language="en-us" />
    </Resources>
    <Applications>
        <Application Id="MarkAmp" Executable="markamp.exe" EntryPoint="Windows.FullTrustApplication">
            <uap:VisualElements DisplayName="MarkAmp" Description="Markdown Editor"
                BackgroundColor="transparent" Square150x150Logo="Assets\Square150x150Logo.png"
                Square44x44Logo="Assets\Square44x44Logo.png">
                <uap:DefaultTile Wide310x150Logo="Assets\Wide310x150Logo.png" />
            </uap:VisualElements>
            <Extensions>
                <uap:Extension Category="windows.fileTypeAssociation">
                    <uap:FileTypeAssociation Name="markdown">
                        <uap:SupportedFileTypes>
                            <uap:FileType>.md</uap:FileType>
                            <uap:FileType>.markdown</uap:FileType>
                            <uap:FileType>.mdown</uap:FileType>
                        </uap:SupportedFileTypes>
                    </uap:FileTypeAssociation>
                </uap:Extension>
            </Extensions>
        </Application>
    </Applications>
    <Capabilities>
        <rescap:Capability Name="runFullTrust" />
    </Capabilities>
</Package>
```

**MSIX build process:**
```powershell
# Create MSIX package
makeappx pack /d .\AppxContent\ /p MarkAmp.msix

# Sign with certificate
signtool sign /fd SHA256 /a /f certificate.pfx /p password MarkAmp.msix
```

**Microsoft Store requirements:**
- Store listing: description, screenshots (1366x768 minimum), category
- Category: Developer tools > Code editors
- Age rating: 3+ (IARC)
- Privacy policy URL (required for all Store apps)
- Windows App Certification Kit (WACK) must pass
- Must support Windows 10 1809+ (build 17763)
- Store assets required:
  - Square 44x44 logo
  - Square 150x150 logo
  - Wide 310x150 logo
  - Store logo (50x50)
  - Screenshots (at least 1, recommend 5+)

**Partner Center setup:**
- Microsoft Partner Center developer account ($19 one-time)
- Reserve app name "MarkAmp"
- Create app submission with all metadata
- Upload MSIX package
- Submit for certification

**Acceptance criteria:**
- MSIX package builds successfully
- Package passes Windows App Certification Kit (WACK)
- App installs from MSIX and runs correctly
- File associations work after MSIX install
- Store listing metadata is prepared
- All required logo assets are created

### Task 29.10: Write build and packaging documentation

Create `/Users/ryanrentfro/code/markamp/docs/v1_docs/build_guide.md`:

**Contents:**
1. Prerequisites for each platform
2. Step-by-step build instructions
3. How to run tests
4. How to create packages for each platform
5. Code signing procedures
6. CI/CD pipeline description
7. Release process
8. Troubleshooting common build issues

**Acceptance criteria:**
- A new developer can build MarkAmp from scratch using the guide
- All platform-specific steps are documented
- Troubleshooting covers common issues

## Files Created/Modified

| File | Action |
|---|---|
| `.github/workflows/ci.yml` | Created |
| `.github/workflows/release.yml` | Created |
| `scripts/package_macos.sh` | Created |
| `scripts/package_macos_appstore.sh` | Created |
| `scripts/package_windows.sh` | Created |
| `scripts/package_windows_msix.sh` | Created |
| `scripts/package_linux.sh` | Created |
| `scripts/build_debug.sh` | Created |
| `scripts/build_release.sh` | Created |
| `scripts/run_tests.sh` | Created |
| `scripts/clean.sh` | Created |
| `scripts/bump_version.sh` | Created |
| `resources/entitlements/markamp.entitlements` | Created |
| `resources/entitlements/markamp-inherit.entitlements` | Created |
| `packaging/windows/AppxManifest.xml` | Created |
| `packaging/windows/assets/` | Created (Store logos) |
| `packaging/macos/Info.plist` | Created |
| `installer.nsi` (or `installer.wxs`) | Created |
| `markamp.desktop` | Created |
| `CMakePresets.json` | Modified (add static preset) |
| `docs/v1_docs/build_guide.md` | Created |
| `docs/v1_docs/app_store_submission_guide.md` | Created |

## Dependencies

- GitHub Actions (CI/CD)
- **Apple Developer Program** ($99/year) -- for Mac App Store + Developer ID signing
- **Microsoft Partner Center** ($19 one-time) -- for Microsoft Store submission
- Code signing certificates:
  - Apple: "Developer ID Application", "3rd Party Mac Developer Application", "3rd Party Mac Developer Installer"
  - Windows: Authenticode EV certificate + MSIX signing certificate
- NSIS or WiX (Windows installer for direct distribution)
- makeappx.exe + Windows SDK (MSIX packaging)
- productbuild (macOS .pkg for App Store)
- dpkg-deb (Debian packaging)
- linuxdeploy or appimagetool (AppImage)
- hdiutil (macOS DMG for direct distribution)
- Windows App Certification Kit (WACK)
- xcrun altool / notarytool (macOS notarization)

## Estimated Complexity

**Critical** -- Build pipeline setup for three platforms with packaging, code signing, and CI is substantial DevOps work. Adding Mac App Store and Microsoft Store distribution significantly increases complexity: App Sandbox entitlements affect core application architecture (file access patterns), MSIX packaging requires specific manifest structure, and both stores have certification processes that may require multiple submission attempts. Getting CI caching right for vcpkg is important for build time.
