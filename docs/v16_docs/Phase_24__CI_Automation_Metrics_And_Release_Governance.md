# Phase 24 -- CI Automation Metrics And Release Governance

## Objective
Operationalize continuous quality with CI automation, dashboards, and release governance for visual quality.

## Context
This phase is part of the v16 UI/UX overhaul program focused on replacing all icons with a VS Code Material Icon Theme-style system and elevating syntax highlighting/theme quality across macOS, Windows, and Linux.

## Definitions
- **Canonical Icon ID:** Stable string key used in code (for example, ).
- **Icon Manifest:** Versioned metadata file mapping file/folder/context keys to icon assets.
- **Scope Token:** TextMate-like token scope such as  or  used for syntax theme mapping.
- **Language Pack:** Bundle containing grammar, injection rules, and language metadata.
- **Quality Gate:** A test or metric threshold that must pass before merge/release.

## Reference Files (Full Paths)
- \
- \=== MarkAmp Test Runner ===
=== MarkAmp Debug Build ===
Project: /Users/ryanrentfro/code/markamp
--- Building (8 parallel jobs) ---
[1/589] Linking CXX executable tests/test_editor_qol
FAILED: [code=1] tests/test_editor_qol 
: && /usr/bin/c++ -pthread -g -arch arm64 -Wl,-search_paths_first -Wl,-headerpad_max_install_names tests/CMakeFiles/test_editor_qol.dir/unit/test_editor_qol.cpp.o tests/CMakeFiles/test_editor_qol.dir/__/src/ui/EditorPanel.cpp.o tests/CMakeFiles/test_editor_qol.dir/__/src/ui/MinimapPanel.cpp.o tests/CMakeFiles/test_editor_qol.dir/__/src/ui/OverviewRulerPanel.cpp.o tests/CMakeFiles/test_editor_qol.dir/__/src/ui/ThemeAwareWindow.cpp.o tests/CMakeFiles/test_editor_qol.dir/__/src/ui/FloatingFormatBar.cpp.o tests/CMakeFiles/test_editor_qol.dir/__/src/ui/LinkPreviewPopover.cpp.o tests/CMakeFiles/test_editor_qol.dir/__/src/ui/TableEditorOverlay.cpp.o tests/CMakeFiles/test_editor_qol.dir/__/src/ui/ImagePreviewPopover.cpp.o tests/CMakeFiles/test_editor_qol.dir/__/src/ui/GitBlameGutterProvider.cpp.o -o tests/test_editor_qol -L/Users/ryanrentfro/code/markamp/build/debug/vcpkg_installed/arm64-osx/debug/lib -Wl,-rpath,/Users/ryanrentfro/code/markamp/build/debug/vcpkg_installed/arm64-osx/debug/lib  vcpkg_installed/arm64-osx/debug/lib/manual-link/libCatch2Maind.a  tests/libmarkamp_core.a  vcpkg_installed/arm64-osx/debug/lib/libCatch2d.a  -L/Users/ryanrentfro/code/markamp/build/debug/vcpkg_installed/arm64-osx/debug/lib  -pthread  vcpkg_installed/arm64-osx/debug/lib/libwx_osx_cocoau_aui-3.2.a  vcpkg_installed/arm64-osx/debug/lib/libwx_osx_cocoau_stc-3.2.a  vcpkg_installed/arm64-osx/debug/lib/libwx_osx_cocoau_html-3.2.a  vcpkg_installed/arm64-osx/debug/lib/libwx_osx_cocoau_core-3.2.a  vcpkg_installed/arm64-osx/debug/lib/libwx_baseu_xml-3.2.a  vcpkg_installed/arm64-osx/debug/lib/libwx_baseu-3.2.a  -lwxscintilla-3.2  -lwx_osx_cocoau_core-3.2  vcpkg_installed/arm64-osx/debug/lib/libjpeg.a  vcpkg_installed/arm64-osx/debug/lib/libpng16d.a  vcpkg_installed/arm64-osx/debug/lib/libz.a  vcpkg_installed/arm64-osx/debug/lib/libtiffd.a  vcpkg_installed/arm64-osx/debug/lib/liblzma.a  -lm  vcpkg_installed/arm64-osx/debug/lib/libnanosvg.a  vcpkg_installed/arm64-osx/debug/lib/libnanosvgrast.a  -framework AudioToolbox  -lwx_baseu-3.2  vcpkg_installed/arm64-osx/debug/lib/libexpat.a  vcpkg_installed/arm64-osx/debug/lib/libpcre2-32.a  /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/libiconv.tbd  -framework CoreFoundation  -framework Security  -framework Carbon  -framework Cocoa  -framework IOKit  -framework QuartzCore  vcpkg_installed/arm64-osx/debug/lib/libnanosvgrast.a  vcpkg_installed/arm64-osx/debug/lib/libnanosvg.a  /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/libm.tbd  vcpkg_installed/arm64-osx/debug/lib/libmd4c.a  vcpkg_installed/arm64-osx/debug/lib/libspdlogd.a  vcpkg_installed/arm64-osx/debug/lib/libfmtd.a  vcpkg_installed/arm64-osx/debug/lib/libyaml-cppd.a  vcpkg_installed/arm64-osx/debug/lib/libzip.a  vcpkg_installed/arm64-osx/debug/lib/libbz2d.a  vcpkg_installed/arm64-osx/debug/lib/libz.a  vcpkg_installed/arm64-osx/debug/lib/libbrotlienc.a  vcpkg_installed/arm64-osx/debug/lib/libbrotlidec.a  vcpkg_installed/arm64-osx/debug/lib/libbrotlicommon.a  vcpkg_installed/arm64-osx/debug/lib/libssl.a  vcpkg_installed/arm64-osx/debug/lib/libcrypto.a  -ldl  vcpkg_installed/arm64-osx/debug/lib/libsqlite3.a  -lm && :
ld: warning: ignoring duplicate libraries: '-lm', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvg.a', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvgrast.a', 'vcpkg_installed/arm64-osx/debug/lib/libz.a'
ld: Undefined symbols:
  markamp::platform::set_accessibility_identifier(wxWindow*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&), referenced from:
      markamp::ui::EditorPanel::EditorPanel(wxWindow*, markamp::core::ThemeEngine&, markamp::core::EventBus&) in EditorPanel.cpp.o
clang: error: linker command failed with exit code 1 (use -v to see invocation)
[2/589] Linking CXX executable tests/test_mermaid_phase3
ld: warning: ignoring duplicate libraries: '-lm', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvg.a', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvgrast.a', 'vcpkg_installed/arm64-osx/debug/lib/libz.a'
[3/589] Linking CXX executable tests/test_phase11_tab_bar
ld: warning: ignoring duplicate libraries: '-lm', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvg.a', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvgrast.a', 'vcpkg_installed/arm64-osx/debug/lib/libz.a'
[4/589] Linking CXX executable tests/test_visual_polish
ld: warning: ignoring duplicate libraries: '-lm', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvg.a', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvgrast.a', 'vcpkg_installed/arm64-osx/debug/lib/libz.a'
[5/589] Linking CXX executable tests/test_phase09_secondary_sidebar
ld: warning: ignoring duplicate libraries: '-lm', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvg.a', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvgrast.a', 'vcpkg_installed/arm64-osx/debug/lib/libz.a'
[6/589] Linking CXX executable tests/test_phase10_panel_area
ld: warning: ignoring duplicate libraries: '-lm', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvg.a', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvgrast.a', 'vcpkg_installed/arm64-osx/debug/lib/libz.a'
[7/589] Linking CXX executable tests/test_theme_phase4
ld: warning: ignoring duplicate libraries: '-lm', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvg.a', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvgrast.a', 'vcpkg_installed/arm64-osx/debug/lib/libz.a'
[8/589] Building CXX object benchmarks/CMakeFiles/markamp_bench.dir/bench_startup.cpp.o
FAILED: [code=1] benchmarks/CMakeFiles/markamp_bench.dir/bench_startup.cpp.o 
/usr/bin/c++ -DBENCHMARK_STATIC_DEFINE -DCMAKE_SOURCE_DIR=\"/Users/ryanrentfro/code/markamp\" -DCPPHTTPLIB_BROTLI_SUPPORT -DMARKAMP_VERSION_MAJOR=2 -DMARKAMP_VERSION_MINOR=17 -DMARKAMP_VERSION_PATCH=61 -DSPDLOG_COMPILED_LIB -DSPDLOG_FMT_EXTERNAL -DYAML_CPP_STATIC_DEFINE -D_FILE_OFFSET_BITS=64 -D__WXMAC__ -D__WXOSX_COCOA__ -D__WXOSX__ -I/Users/ryanrentfro/code/markamp/src -isystem /Users/ryanrentfro/code/markamp/build/debug/vcpkg_installed/arm64-osx/debug/lib/wx/include/osx_cocoa-unicode-static-3.2 -isystem /Users/ryanrentfro/code/markamp/build/debug/vcpkg_installed/arm64-osx/include/wx-3.2 -isystem /Users/ryanrentfro/code/markamp/build/debug/vcpkg_installed/arm64-osx/include -isystem /Users/ryanrentfro/code/markamp/build/debug/vcpkg_installed/arm64-osx/include/nanosvg -pthread -g -std=c++2b -arch arm64 -MD -MT benchmarks/CMakeFiles/markamp_bench.dir/bench_startup.cpp.o -MF benchmarks/CMakeFiles/markamp_bench.dir/bench_startup.cpp.o.d -o benchmarks/CMakeFiles/markamp_bench.dir/bench_startup.cpp.o -c /Users/ryanrentfro/code/markamp/benchmarks/bench_startup.cpp
In file included from /Users/ryanrentfro/code/markamp/benchmarks/bench_startup.cpp:8:
In file included from /Users/ryanrentfro/code/markamp/src/core/Command.h:3:
In file included from /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/c++/v1/memory:898:
In file included from /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/c++/v1/__memory/shared_ptr.h:31:
/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/include/c++/v1/__memory/unique_ptr.h:686:30: error: no matching constructor for initialization of 'markamp::core::TerminalService'
  return unique_ptr<_Tp>(new _Tp(_VSTD::forward<_Args>(__args)...));
                             ^
/Users/ryanrentfro/code/markamp/benchmarks/bench_startup.cpp:105:30: note: in instantiation of function template specialization 'std::make_unique<markamp::core::TerminalService>' requested here
        auto terminal = std::make_unique<markamp::core::TerminalService>();
                             ^
/Users/ryanrentfro/code/markamp/src/core/TerminalService.h:51:14: note: candidate constructor not viable: requires single argument 'event_bus', but no arguments were provided
    explicit TerminalService(EventBus& event_bus);
             ^
/Users/ryanrentfro/code/markamp/src/core/TerminalService.h:48:7: note: candidate constructor (the implicit copy constructor) not viable: requires 1 argument, but 0 were provided
class TerminalService
      ^
/Users/ryanrentfro/code/markamp/src/core/TerminalService.h:48:7: note: candidate constructor (the implicit move constructor) not viable: requires 1 argument, but 0 were provided
1 error generated.
ninja: build stopped: subcommand failed.
- \=== MarkAmp Smoke Test ===
Build dir: /Users/ryanrentfro/code/markamp/build/debug

1. Binary exists and is executable
  ❌ Binary not found at /Users/ryanrentfro/code/markamp/build/debug/src/markamp
2. Resource files
  ✅ resources/themes/ has 11 files
  ✅ resources/fonts/ has 0 files
  ✅ resources/icons/ has 7 files
3. Theme JSON validation
  ✅ classic_amp.json is valid JSON
  ✅ classic_mono.json is valid JSON
  ✅ cyber_night.json is valid JSON
  ✅ high_contrast_blue.json is valid JSON
  ✅ matrix_core.json is valid JSON
  ✅ midnight_neon.json is valid JSON
  ✅ solarized_dark.json is valid JSON
  ✅ vapor_wave.json is valid JSON
4. Sample files
  ✅ 4 sample markdown files found
5. Packaging files
  ✅ packaging/macos/Info.plist exists
  ✅ packaging/windows/AppxManifest.xml exists
  ✅ packaging/linux/markamp.desktop exists
6. Info.plist validation
  ✅ Info.plist is valid
7. Test binaries
  ⚠️  No test binaries found in /Users/ryanrentfro/code/markamp/build/debug

═══════════════════════════════════
  Passed:   16
  Failed:   1
  Warnings: 1
═══════════════════════════════════
SMOKE TEST FAILED
- \
- \

## Implementation Details For AI Coding Agents
- Preserve backward compatibility by introducing adapter layers before removing legacy icon IDs or syntax classes.
- When replacing icons, prefer manifest-driven lookup over inline SVG literals or emoji.
- Add deterministic tests for mapping completeness, missing asset handling, and fallback behavior.
- Keep all newly introduced color/styling values theme-tokenized; avoid hardcoded colors.
- For cross-platform work, explicitly test font fallback and rendering parity on macOS/Windows/Linux runners.

## Example
\: command icon resolves to emoji from \.
\: command icon resolves via canonical ID -> manifest entry -> SVG asset -> themed render pipeline.

## Tasks
1. Audit current ci visual regression workflow implementation and document gaps against v16 targets.
2. Define contract/schema for ci visual regression workflow with migration notes and edge cases.
3. Implement ci visual regression workflow core refactor with backward-compatible adapter paths.
4. Add automated tests for ci visual regression workflow including failure-path coverage.
5. Publish ci visual regression workflow documentation and phase acceptance checklist.
6. Audit current ci syntax conformance workflow implementation and document gaps against v16 targets.
7. Define contract/schema for ci syntax conformance workflow with migration notes and edge cases.
8. Implement ci syntax conformance workflow core refactor with backward-compatible adapter paths.
9. Add automated tests for ci syntax conformance workflow including failure-path coverage.
10. Publish ci syntax conformance workflow documentation and phase acceptance checklist.
11. Audit current coverage metric publication implementation and document gaps against v16 targets.
12. Define contract/schema for coverage metric publication with migration notes and edge cases.
13. Implement coverage metric publication core refactor with backward-compatible adapter paths.
14. Add automated tests for coverage metric publication including failure-path coverage.
15. Publish coverage metric publication documentation and phase acceptance checklist.
16. Audit current flake detection and quarantine implementation and document gaps against v16 targets.
17. Define contract/schema for flake detection and quarantine with migration notes and edge cases.
18. Implement flake detection and quarantine core refactor with backward-compatible adapter paths.
19. Add automated tests for flake detection and quarantine including failure-path coverage.
20. Publish flake detection and quarantine documentation and phase acceptance checklist.
21. Audit current artifact retention and indexing implementation and document gaps against v16 targets.
22. Define contract/schema for artifact retention and indexing with migration notes and edge cases.
23. Implement artifact retention and indexing core refactor with backward-compatible adapter paths.
24. Add automated tests for artifact retention and indexing including failure-path coverage.
25. Publish artifact retention and indexing documentation and phase acceptance checklist.
26. Audit current release candidate visual certification implementation and document gaps against v16 targets.
27. Define contract/schema for release candidate visual certification with migration notes and edge cases.
28. Implement release candidate visual certification core refactor with backward-compatible adapter paths.
29. Add automated tests for release candidate visual certification including failure-path coverage.
30. Publish release candidate visual certification documentation and phase acceptance checklist.
31. Audit current quality dashboard automation implementation and document gaps against v16 targets.
32. Define contract/schema for quality dashboard automation with migration notes and edge cases.
33. Implement quality dashboard automation core refactor with backward-compatible adapter paths.
34. Add automated tests for quality dashboard automation including failure-path coverage.
35. Publish quality dashboard automation documentation and phase acceptance checklist.
36. Audit current ownership and on-call integration implementation and document gaps against v16 targets.
37. Define contract/schema for ownership and on-call integration with migration notes and edge cases.
38. Implement ownership and on-call integration core refactor with backward-compatible adapter paths.
39. Add automated tests for ownership and on-call integration including failure-path coverage.
40. Publish ownership and on-call integration documentation and phase acceptance checklist.
41. Audit current gate policy versioning implementation and document gaps against v16 targets.
42. Define contract/schema for gate policy versioning with migration notes and edge cases.
43. Implement gate policy versioning core refactor with backward-compatible adapter paths.
44. Add automated tests for gate policy versioning including failure-path coverage.
45. Publish gate policy versioning documentation and phase acceptance checklist.
46. Audit current post-release monitoring hooks implementation and document gaps against v16 targets.
47. Define contract/schema for post-release monitoring hooks with migration notes and edge cases.
48. Implement post-release monitoring hooks core refactor with backward-compatible adapter paths.
49. Add automated tests for post-release monitoring hooks including failure-path coverage.
50. Publish post-release monitoring hooks documentation and phase acceptance checklist.

## Phase Completion Criteria
- All 50 tasks completed with merged code, tests, and documentation.
- No blocker-level regressions in icon rendering, theme rendering, or syntax highlighting flows.
- Cross-platform verification evidence attached for macOS, Windows, and Linux.
