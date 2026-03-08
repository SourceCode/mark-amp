#!/usr/bin/env bash
# preflight_check.sh — Pre-flight validation for E2E test host
#
# Run this script before executing E2E tests to verify all prerequisites.
# Usage: bash tests/e2e/appium/scripts/preflight_check.sh

set -e

RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m'

PASS=0
FAIL=0
WARN=0

check() {
    local name="$1"
    local cmd="$2"
    if eval "$cmd" >/dev/null 2>&1; then
        echo -e "  ${GREEN}✓${NC} $name"
        PASS=$((PASS + 1))
    else
        echo -e "  ${RED}✗${NC} $name"
        FAIL=$((FAIL + 1))
    fi
}

warn() {
    local name="$1"
    local cmd="$2"
    if eval "$cmd" >/dev/null 2>&1; then
        echo -e "  ${GREEN}✓${NC} $name"
        PASS=$((PASS + 1))
    else
        echo -e "  ${YELLOW}⚠${NC} $name (optional)"
        WARN=$((WARN + 1))
    fi
}

echo ""
echo "═══════════════════════════════════════════════════"
echo "  MarkAmp E2E Test Suite — Preflight Check"
echo "═══════════════════════════════════════════════════"
echo ""

echo "System:"
check "macOS version ≥ 14.0" "sw_vers -productVersion | awk -F. '{exit !(\$1 >= 14)}'"
check "Xcode CLI tools" "xcode-select -p"

echo ""
echo "Node.js:"
check "Node.js installed" "command -v node"
check "Node.js ≥ 20" "node -v | awk -F'[v.]' '{exit !(\$2 >= 20)}'"
check "npm installed" "command -v npm"

echo ""
echo "Appium:"
check "Appium installed" "command -v appium"
warn "mac2 driver installed" "appium driver list 2>/dev/null | grep -q mac2"

echo ""
echo "Accessibility:"
warn "Accessibility permission (AppleScript)" "osascript -e 'tell application \"System Events\" to get name of every process' 2>/dev/null"

echo ""
echo "MarkAmp Build:"
warn "Debug build exists" "ls build/debug/MarkAmp.app/Contents/MacOS/MarkAmp 2>/dev/null || ls build/MarkAmp.app/Contents/MacOS/MarkAmp 2>/dev/null"

echo ""
echo "═══════════════════════════════════════════════════"
echo -e "  Results: ${GREEN}${PASS} passed${NC}, ${RED}${FAIL} failed${NC}, ${YELLOW}${WARN} warnings${NC}"
echo "═══════════════════════════════════════════════════"
echo ""

if [ $FAIL -gt 0 ]; then
    echo -e "${RED}Some preflight checks failed. Fix them before running E2E tests.${NC}"
    exit 1
fi
echo -e "${GREEN}All required checks passed!${NC}"
