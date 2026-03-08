# Phase 10 -- Icon Testing Visual Regression And CI Gates

## Objective
Build robust icon QA automation including visual regression, functional checks, and CI gates.

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
- \
- \
- \=== MarkAmp Test Runner ===
=== MarkAmp Debug Build ===
Project: /Users/ryanrentfro/code/markamp
--- Building (8 parallel jobs) ---
[1/595] Linking CXX executable tests/test_editor_qol
FAILED: [code=1] tests/test_editor_qol 
: && /usr/bin/c++ -pthread -g -arch arm64 -Wl,-search_paths_first -Wl,-headerpad_max_install_names tests/CMakeFiles/test_editor_qol.dir/unit/test_editor_qol.cpp.o tests/CMakeFiles/test_editor_qol.dir/__/src/ui/EditorPanel.cpp.o tests/CMakeFiles/test_editor_qol.dir/__/src/ui/MinimapPanel.cpp.o tests/CMakeFiles/test_editor_qol.dir/__/src/ui/OverviewRulerPanel.cpp.o tests/CMakeFiles/test_editor_qol.dir/__/src/ui/ThemeAwareWindow.cpp.o tests/CMakeFiles/test_editor_qol.dir/__/src/ui/FloatingFormatBar.cpp.o tests/CMakeFiles/test_editor_qol.dir/__/src/ui/LinkPreviewPopover.cpp.o tests/CMakeFiles/test_editor_qol.dir/__/src/ui/TableEditorOverlay.cpp.o tests/CMakeFiles/test_editor_qol.dir/__/src/ui/ImagePreviewPopover.cpp.o tests/CMakeFiles/test_editor_qol.dir/__/src/ui/GitBlameGutterProvider.cpp.o -o tests/test_editor_qol -L/Users/ryanrentfro/code/markamp/build/debug/vcpkg_installed/arm64-osx/debug/lib -Wl,-rpath,/Users/ryanrentfro/code/markamp/build/debug/vcpkg_installed/arm64-osx/debug/lib  vcpkg_installed/arm64-osx/debug/lib/manual-link/libCatch2Maind.a  tests/libmarkamp_core.a  vcpkg_installed/arm64-osx/debug/lib/libCatch2d.a  -L/Users/ryanrentfro/code/markamp/build/debug/vcpkg_installed/arm64-osx/debug/lib  -pthread  vcpkg_installed/arm64-osx/debug/lib/libwx_osx_cocoau_aui-3.2.a  vcpkg_installed/arm64-osx/debug/lib/libwx_osx_cocoau_stc-3.2.a  vcpkg_installed/arm64-osx/debug/lib/libwx_osx_cocoau_html-3.2.a  vcpkg_installed/arm64-osx/debug/lib/libwx_osx_cocoau_core-3.2.a  vcpkg_installed/arm64-osx/debug/lib/libwx_baseu_xml-3.2.a  vcpkg_installed/arm64-osx/debug/lib/libwx_baseu-3.2.a  -lwxscintilla-3.2  -lwx_osx_cocoau_core-3.2  vcpkg_installed/arm64-osx/debug/lib/libjpeg.a  vcpkg_installed/arm64-osx/debug/lib/libpng16d.a  vcpkg_installed/arm64-osx/debug/lib/libz.a  vcpkg_installed/arm64-osx/debug/lib/libtiffd.a  vcpkg_installed/arm64-osx/debug/lib/liblzma.a  -lm  vcpkg_installed/arm64-osx/debug/lib/libnanosvg.a  vcpkg_installed/arm64-osx/debug/lib/libnanosvgrast.a  -framework AudioToolbox  -lwx_baseu-3.2  vcpkg_installed/arm64-osx/debug/lib/libexpat.a  vcpkg_installed/arm64-osx/debug/lib/libpcre2-32.a  /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/libiconv.tbd  -framework CoreFoundation  -framework Security  -framework Carbon  -framework Cocoa  -framework IOKit  -framework QuartzCore  vcpkg_installed/arm64-osx/debug/lib/libnanosvgrast.a  vcpkg_installed/arm64-osx/debug/lib/libnanosvg.a  /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk/usr/lib/libm.tbd  vcpkg_installed/arm64-osx/debug/lib/libmd4c.a  vcpkg_installed/arm64-osx/debug/lib/libspdlogd.a  vcpkg_installed/arm64-osx/debug/lib/libfmtd.a  vcpkg_installed/arm64-osx/debug/lib/libyaml-cppd.a  vcpkg_installed/arm64-osx/debug/lib/libzip.a  vcpkg_installed/arm64-osx/debug/lib/libbz2d.a  vcpkg_installed/arm64-osx/debug/lib/libz.a  vcpkg_installed/arm64-osx/debug/lib/libbrotlienc.a  vcpkg_installed/arm64-osx/debug/lib/libbrotlidec.a  vcpkg_installed/arm64-osx/debug/lib/libbrotlicommon.a  vcpkg_installed/arm64-osx/debug/lib/libssl.a  vcpkg_installed/arm64-osx/debug/lib/libcrypto.a  -ldl  vcpkg_installed/arm64-osx/debug/lib/libsqlite3.a  -lm && :
ld: warning: ignoring duplicate libraries: '-lm', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvg.a', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvgrast.a', 'vcpkg_installed/arm64-osx/debug/lib/libz.a'
ld: Undefined symbols:
  markamp::platform::set_accessibility_identifier(wxWindow*, std::__1::basic_string<char, std::__1::char_traits<char>, std::__1::allocator<char>> const&), referenced from:
      markamp::ui::EditorPanel::EditorPanel(wxWindow*, markamp::core::ThemeEngine&, markamp::core::EventBus&) in EditorPanel.cpp.o
clang: error: linker command failed with exit code 1 (use -v to see invocation)
[2/595] Linking CXX executable tests/test_performance_infra
ld: warning: ignoring duplicate libraries: '-lm', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvg.a', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvgrast.a', 'vcpkg_installed/arm64-osx/debug/lib/libz.a'
[3/595] Linking CXX executable tests/test_perf_patterns_21_40
ld: warning: ignoring duplicate libraries: '-lm', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvg.a', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvgrast.a', 'vcpkg_installed/arm64-osx/debug/lib/libz.a'
[4/595] Linking CXX executable tests/test_sidebar_filter
ld: warning: ignoring duplicate libraries: '-lm', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvg.a', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvgrast.a', 'vcpkg_installed/arm64-osx/debug/lib/libz.a'
[5/595] Linking CXX executable tests/test_json_renderer
ld: warning: ignoring duplicate libraries: '-lm', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvg.a', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvgrast.a', 'vcpkg_installed/arm64-osx/debug/lib/libz.a'
[6/595] Linking CXX executable tests/test_split_view_advanced
ld: warning: ignoring duplicate libraries: '-lm', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvg.a', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvgrast.a', 'vcpkg_installed/arm64-osx/debug/lib/libz.a'
[7/595] Linking CXX executable tests/test_script_renderer
ld: warning: ignoring duplicate libraries: '-lm', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvg.a', 'vcpkg_installed/arm64-osx/debug/lib/libnanosvgrast.a', 'vcpkg_installed/arm64-osx/debug/lib/libz.a'
[8/595] Building CXX object benchmarks/CMakeFiles/markamp_bench.dir/bench_startup.cpp.o
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
1. Audit current icon manifest contract tests implementation and document gaps against v16 targets.
2. Define contract/schema for icon manifest contract tests with migration notes and edge cases.
3. Implement icon manifest contract tests core refactor with backward-compatible adapter paths.
4. Add automated tests for icon manifest contract tests including failure-path coverage.
5. Publish icon manifest contract tests documentation and phase acceptance checklist.
6. Audit current file/folder mapping snapshot tests implementation and document gaps against v16 targets.
7. Define contract/schema for file/folder mapping snapshot tests with migration notes and edge cases.
8. Implement file/folder mapping snapshot tests core refactor with backward-compatible adapter paths.
9. Add automated tests for file/folder mapping snapshot tests including failure-path coverage.
10. Publish file/folder mapping snapshot tests documentation and phase acceptance checklist.
11. Audit current command/icon id coverage tests implementation and document gaps against v16 targets.
12. Define contract/schema for command/icon id coverage tests with migration notes and edge cases.
13. Implement command/icon id coverage tests core refactor with backward-compatible adapter paths.
14. Add automated tests for command/icon id coverage tests including failure-path coverage.
15. Publish command/icon id coverage tests documentation and phase acceptance checklist.
16. Audit current visual golden image tests implementation and document gaps against v16 targets.
17. Define contract/schema for visual golden image tests with migration notes and edge cases.
18. Implement visual golden image tests core refactor with backward-compatible adapter paths.
19. Add automated tests for visual golden image tests including failure-path coverage.
20. Publish visual golden image tests documentation and phase acceptance checklist.
21. Audit current high-dpi icon rendering tests implementation and document gaps against v16 targets.
22. Define contract/schema for high-dpi icon rendering tests with migration notes and edge cases.
23. Implement high-dpi icon rendering tests core refactor with backward-compatible adapter paths.
24. Add automated tests for high-dpi icon rendering tests including failure-path coverage.
25. Publish high-dpi icon rendering tests documentation and phase acceptance checklist.
26. Audit current theme contrast icon tests implementation and document gaps against v16 targets.
27. Define contract/schema for theme contrast icon tests with migration notes and edge cases.
28. Implement theme contrast icon tests core refactor with backward-compatible adapter paths.
29. Add automated tests for theme contrast icon tests including failure-path coverage.
30. Publish theme contrast icon tests documentation and phase acceptance checklist.
31. Audit current accessibility name/role icon tests implementation and document gaps against v16 targets.
32. Define contract/schema for accessibility name/role icon tests with migration notes and edge cases.
33. Implement accessibility name/role icon tests core refactor with backward-compatible adapter paths.
34. Add automated tests for accessibility name/role icon tests including failure-path coverage.
35. Publish accessibility name/role icon tests documentation and phase acceptance checklist.
36. Audit current performance benchmarks for icon rendering implementation and document gaps against v16 targets.
37. Define contract/schema for performance benchmarks for icon rendering with migration notes and edge cases.
38. Implement performance benchmarks for icon rendering core refactor with backward-compatible adapter paths.
39. Add automated tests for performance benchmarks for icon rendering including failure-path coverage.
40. Publish performance benchmarks for icon rendering documentation and phase acceptance checklist.
41. Audit current artifact collection for icon regressions implementation and document gaps against v16 targets.
42. Define contract/schema for artifact collection for icon regressions with migration notes and edge cases.
43. Implement artifact collection for icon regressions core refactor with backward-compatible adapter paths.
44. Add automated tests for artifact collection for icon regressions including failure-path coverage.
45. Publish artifact collection for icon regressions documentation and phase acceptance checklist.
46. Audit current CI gate definitions for icon quality implementation and document gaps against v16 targets.
47. Define contract/schema for CI gate definitions for icon quality with migration notes and edge cases.
48. Implement CI gate definitions for icon quality core refactor with backward-compatible adapter paths.
49. Add automated tests for CI gate definitions for icon quality including failure-path coverage.
50. Publish CI gate definitions for icon quality documentation and phase acceptance checklist.

## Phase Completion Criteria
- All 50 tasks completed with merged code, tests, and documentation.
- No blocker-level regressions in icon rendering, theme rendering, or syntax highlighting flows.
- Cross-platform verification evidence attached for macOS, Windows, and Linux.
