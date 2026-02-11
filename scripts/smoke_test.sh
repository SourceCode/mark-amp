#!/usr/bin/env bash
set -euo pipefail

# Quick smoke test verifying the build artifacts are sane.
# Usage: ./scripts/smoke_test.sh [build_dir]

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${1:-${PROJECT_DIR}/build/debug}"

PASS=0
FAIL=0
WARN=0

pass()  { echo "  ✅ $1"; PASS=$((PASS + 1)); }
fail()  { echo "  ❌ $1"; FAIL=$((FAIL + 1)); }
warn()  { echo "  ⚠️  $1"; WARN=$((WARN + 1)); }

echo "=== MarkAmp Smoke Test ==="
echo "Build dir: ${BUILD_DIR}"
echo ""

# ── 1. Binary exists ──
echo "1. Binary exists and is executable"
BINARY="${BUILD_DIR}/src/markamp"
if [[ -x "${BINARY}" ]]; then
    pass "Binary found at ${BINARY}"
else
    fail "Binary not found at ${BINARY}"
fi

# ── 2. Resource files present ──
echo "2. Resource files"
for dir in themes fonts icons; do
    if [[ -d "${PROJECT_DIR}/resources/${dir}" ]]; then
        count=$(find "${PROJECT_DIR}/resources/${dir}" -type f | wc -l | tr -d ' ')
        pass "resources/${dir}/ has ${count} files"
    else
        fail "resources/${dir}/ missing"
    fi
done

# ── 3. Theme JSON files are valid ──
echo "3. Theme JSON validation"
THEME_DIR="${PROJECT_DIR}/resources/themes"
if [[ -d "${THEME_DIR}" ]]; then
    for json_file in "${THEME_DIR}"/*.json; do
        [[ -f "${json_file}" ]] || continue
        if python3 -c "import json; json.load(open('${json_file}'))" 2>/dev/null; then
            pass "$(basename "${json_file}") is valid JSON"
        else
            fail "$(basename "${json_file}") is invalid JSON"
        fi
    done
else
    warn "No themes directory"
fi

# ── 4. Sample files present ──
echo "4. Sample files"
SAMPLE_DIR="${PROJECT_DIR}/resources/sample_files"
if [[ -d "${SAMPLE_DIR}" ]]; then
    count=$(find "${SAMPLE_DIR}" -name '*.md' -type f | wc -l | tr -d ' ')
    if [[ "${count}" -gt 0 ]]; then
        pass "${count} sample markdown files found"
    else
        warn "No .md files in sample_files/"
    fi
else
    warn "No sample_files directory"
fi

# ── 5. Packaging files present ──
echo "5. Packaging files"
for file in packaging/macos/Info.plist packaging/windows/AppxManifest.xml packaging/linux/markamp.desktop; do
    if [[ -f "${PROJECT_DIR}/${file}" ]]; then
        pass "${file} exists"
    else
        warn "${file} missing"
    fi
done

# ── 6. Info.plist validity ──
echo "6. Info.plist validation"
PLIST="${PROJECT_DIR}/packaging/macos/Info.plist"
if [[ -f "${PLIST}" ]]; then
    if plutil -lint "${PLIST}" &>/dev/null; then
        pass "Info.plist is valid"
    else
        fail "Info.plist is invalid"
    fi
else
    warn "Info.plist not found"
fi

# ── 7. Tests exist and run ──
echo "7. Test binaries"
TEST_COUNT=0
for test_bin in "${BUILD_DIR}"/test_*; do
    [[ -x "${test_bin}" ]] || continue
    ((TEST_COUNT++))
done
if [[ "${TEST_COUNT}" -gt 0 ]]; then
    pass "${TEST_COUNT} test binaries found"
else
    warn "No test binaries found in ${BUILD_DIR}"
fi

# ── Summary ──
echo ""
echo "═══════════════════════════════════"
echo "  Passed:   ${PASS}"
echo "  Failed:   ${FAIL}"
echo "  Warnings: ${WARN}"
echo "═══════════════════════════════════"

if [[ "${FAIL}" -gt 0 ]]; then
    echo "SMOKE TEST FAILED"
    exit 1
fi

echo "SMOKE TEST PASSED"
