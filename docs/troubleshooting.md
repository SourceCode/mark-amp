# Troubleshooting

> Common issues, debugging tips, and recovery procedures for MarkAmp v2.3.16.

---

## Build Issues

### CMake Configuration Fails

**Error:** `CMake Error: CMake was unable to find a build program`

**Solution:** Install Ninja:

```bash
brew install ninja        # macOS
apt install ninja-build   # Linux
choco install ninja       # Windows
```

### wxWidgets Not Found

**Error:** `Could NOT find wxWidgets`

**Solution:** Ensure vcpkg is bootstrapped:

```bash
cd external/vcpkg && ./bootstrap-vcpkg.sh
cmake --preset debug  # Re-run configuration
```

### Duplicate Symbol Linker Errors

**Error:** `duplicate symbol 'markamp::core::FileWatcher::unwatch'`

**Cause:** Some implementations exist in aggregate files (`PlatformServices.cpp` includes `FileWatcher` methods; `TracingService.cpp` includes `ExtensionTelemetry` methods). Don't compile both the standalone `.cpp` and the aggregate file.

**Solution:** Check `src/CMakeLists.txt` for duplicate entries:

```bash
grep -n "FileWatcher.cpp\|ExtensionTelemetry.cpp" src/CMakeLists.txt
```

### Compiler Warnings as Errors

**Error:** `-Werror,-Wunused-parameter` or `-Werror,-Wunused-private-field`

**Solutions:**

```cpp
// Unused parameter in stub method
(void)param_name;

// Unused private field stored for future use
[[maybe_unused]] EventBus* event_bus_;

// Sign conversion
size_t index = 0;  // Use size_t, not int, for container indices
```

---

## Runtime Issues

### Preview Doesn't Update

1. Verify you're in **Split** or **Preview** mode (Ctrl+2 / Ctrl+3)
2. The preview debounces at ~300ms — wait briefly after typing
3. Check the Output panel for rendering errors

### Mermaid Diagrams Don't Render

1. Ensure the code block uses ` ```mermaid ` fence tag
2. Check for syntax errors — an error overlay describes the issue
3. Verify `mmdc` is installed: `which mmdc`

### Fonts Look Wrong

1. MarkAmp bundles JetBrains Mono and Rajdhani
2. Verify `resources/fonts/` directory exists and contains font files
3. On Linux, run `fc-cache -fv` after installing fonts

### Application Won't Start

| Platform | Resolution                                                    |
| -------- | ------------------------------------------------------------- |
| macOS    | Right-click → Open if Gatekeeper blocks it                    |
| Linux    | Ensure `libgtk-3.so` and `libwebkit2gtk-4.0.so` are installed |
| Windows  | Install Visual C++ Redistributable                            |

### Extension Won't Activate

1. Check `ExtensionHostRecovery` log output
2. Verify manifest `activationEvents` match current context
3. Check `PluginQuarantine` — the extension may have been quarantined after crashes
4. Inspect extension permissions in `ExtensionSandbox`

### Canvas Objects Missing After Load

1. Board format may be outdated — check `BoardSerializer` version
2. Verify file isn't corrupted: re-open from recent files
3. Check console for deserialization errors

---

## Test Failures

### Test Target Won't Link

**Cause:** Missing source files in test target's `target_link_libraries`.

**Solution:** Link `markamp_dependencies` and include required source files:

```cmake
target_link_libraries(test_my_feature PRIVATE
    Catch2::Catch2WithMain
    markamp_dependencies
)
```

### Tests Pass Locally But Fail in CI

1. Check for timing-dependent tests (use explicit waits, not sleeps)
2. Verify no file-system path assumptions (use `std::filesystem::temp_directory_path()`)
3. Check for thread-safety issues — run with TSan: `cmake --preset debug-tsan`

---

## Debugging Tips

### Structured Logging

MarkAmp uses spdlog. Set log level in config:

```yaml
log_level: "debug" # trace, debug, info, warn, error, critical
```

Log output locations:
| Platform | Path |
|----------|------|
| macOS | `~/Library/Logs/MarkAmp/markamp.log` |
| Linux | `~/.local/share/MarkAmp/markamp.log` |
| Windows | `%LOCALAPPDATA%\MarkAmp\markamp.log` |

### Tracy Profiler

```bash
cmake --preset profile
cmake --build build/profile -j$(sysctl -n hw.ncpu)
./build/profile/markamp
# Open Tracy profiler to connect
```

### AddressSanitizer

```bash
cmake --preset debug-asan
cmake --build build/debug-asan -j$(sysctl -n hw.ncpu)
./build/debug-asan/markamp  # Will abort with details on memory errors
```

---

## Reset Procedures

### Reset Configuration

```bash
# macOS
rm ~/Library/Application\ Support/MarkAmp/config.yaml

# Linux
rm ~/.config/MarkAmp/config.yaml

# Windows
del %APPDATA%\MarkAmp\config.yaml
```

### Reset All Data

```bash
# macOS
rm -rf ~/Library/Application\ Support/MarkAmp/

# Linux
rm -rf ~/.config/MarkAmp/ ~/.local/share/MarkAmp/

# Windows
Remove-Item -Recurse "$env:APPDATA\MarkAmp"
```

### Clean Build

```bash
scripts/clean.sh
cmake --preset debug
cmake --build build/debug -j$(sysctl -n hw.ncpu)
```

---

## Reporting Bugs

File issues at [github.com/markamp/markamp/issues](https://github.com/markamp/markamp/issues) with:

1. **OS and version** (macOS 15.x, Ubuntu 24.04, Windows 11, etc.)
2. **MarkAmp version** (`v2.3.16` etc.)
3. **Steps to reproduce**
4. **Expected vs actual behavior**
5. **Log output** (from log file or console)
6. **Sanitizer output** if applicable (ASan, TSan)
