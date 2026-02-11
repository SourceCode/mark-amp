# v1.0.0 Release Checklist

Verify all items before tagging and publishing the release.

---

## Build & Tests

- [ ] All unit tests pass on macOS
- [ ] All unit tests pass on Windows
- [ ] All unit tests pass on Linux
- [ ] All integration tests pass (`test_e2e`)
- [ ] Smoke test passes (`scripts/smoke_test.sh`)
- [ ] No memory leaks (ASAN clean)
- [ ] No undefined behavior (UBSAN clean)

## Performance

- [ ] Cold start < 500ms
- [ ] Preview update < 300ms debounce
- [ ] Theme switching < 100ms
- [ ] 10,000-line file renders without perceptible lag

## Packaging

- [ ] macOS .dmg installs and runs on clean system
- [ ] macOS .dmg is code-signed with Developer ID
- [ ] macOS .dmg is notarized and stapled
- [ ] macOS App Store .pkg installs in sandboxed mode
- [ ] macOS .pkg passes `xcrun altool --validate-app`
- [ ] No App Sandbox violations in Console.app
- [ ] Windows NSIS installer installs and runs on clean system
- [ ] Windows portable ZIP runs on clean system
- [ ] Windows MSIX passes Windows App Certification Kit (WACK)
- [ ] Windows MSIX installs and runs correctly
- [ ] Linux .deb installs and runs on clean Ubuntu
- [ ] Linux AppImage runs on clean system
- [ ] File associations work on all platforms

## Functionality

- [ ] All 8 themes render correctly on all platforms
- [ ] Theme import/export works cross-platform
- [ ] Mermaid diagrams render on all platforms
- [ ] Keyboard shortcuts work on all platforms
- [ ] Find and replace works correctly
- [ ] Sidebar file tree navigates correctly
- [ ] UI visually matches design spec across all themes

## Documentation

- [ ] `README.md` is accurate and up to date
- [ ] `docs/user_guide.md` covers all features
- [ ] `docs/architecture.md` reflects current codebase
- [ ] `docs/api_reference.md` documents all interfaces
- [ ] `CONTRIBUTING.md` has correct setup instructions
- [ ] `CHANGELOG.md` lists all v1.0.0 features
- [ ] `docs/v1_docs/build_guide.md` build instructions verified

## Release

- [ ] Version bumped to 1.0.0 in all files (`scripts/bump_version.sh 1.0.0`)
- [ ] Version numbers correct in: CMakeLists.txt, Info.plist, installer.nsi, AppxManifest.xml
- [ ] Git tag `v1.0.0` created
- [ ] GitHub Release created with all platform packages
- [ ] Release description includes download links and installation instructions

## App Store — Mac App Store

- [ ] App Store Connect metadata complete (description, keywords, category)
- [ ] 5 screenshots uploaded (2560×1600, showing different themes)
- [ ] Privacy policy URL published
- [ ] Pricing set
- [ ] Submitted for review

## App Store — Microsoft Store

- [ ] Partner Center listing complete (description, screenshots, category)
- [ ] MSIX uploaded and validated
- [ ] Privacy policy URL published
- [ ] Pricing set
- [ ] Submitted for certification
