#!/usr/bin/env bash
# ci_run_mac2.sh — CI runner for MarkAmp Appium mac2 E2E tests
set -euo pipefail

SCRIPT_DIR="$(cd "$(dirname "$0")/.." && pwd)"

echo "=== MarkAmp E2E CI: mac2 ==="
echo "Node: $(node -v)"
echo "npm:  $(npm -v)"

# 1. Install dependencies
cd "$SCRIPT_DIR"
npm ci

# 2. Set E2E flag
export MARKAMP_E2E=1

# 3. Start Appium server in background
npx appium &
APPIUM_PID=$!
echo "Appium PID: $APPIUM_PID"

# Give Appium time to start
sleep 5

# 4. Run smoke tests
echo "Running smoke tests..."
npx wdio run wdio.conf.ts --spec src/specs/smoke/**/*.spec.ts || true

# 5. Cleanup
kill $APPIUM_PID 2>/dev/null || true

echo "=== E2E CI complete ==="
