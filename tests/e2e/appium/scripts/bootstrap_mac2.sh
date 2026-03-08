#!/usr/bin/env bash
# bootstrap_mac2.sh — Automated Appium + mac2 driver setup for MarkAmp E2E
set -euo pipefail

echo "=== MarkAmp E2E: Bootstrapping Appium mac2 ==="

# 1. Check Node.js version
REQUIRED_NODE_MAJOR=20
CURRENT_NODE_MAJOR=$(node -v | sed 's/v\([0-9]*\).*/\1/')
if [ "$CURRENT_NODE_MAJOR" -lt "$REQUIRED_NODE_MAJOR" ]; then
    echo "ERROR: Node.js >= $REQUIRED_NODE_MAJOR required (found v$(node -v))"
    exit 1
fi
echo "✓ Node.js $(node -v)"

# 2. Install Appium 2.x globally if not present
if ! command -v appium &>/dev/null; then
    echo "Installing Appium 2.x globally..."
    npm install -g appium@latest
fi
echo "✓ Appium $(appium -v)"

# 3. Install mac2 driver if not already installed
if ! appium driver list --installed 2>/dev/null | grep -q "mac2"; then
    echo "Installing appium-mac2-driver..."
    appium driver install mac2
fi
echo "✓ mac2 driver installed"

# 4. Check macOS accessibility permissions
echo ""
echo "⚠ IMPORTANT: Appium requires macOS Accessibility permissions."
echo "  Go to: System Settings → Privacy & Security → Accessibility"
echo "  Add: Terminal.app (or your terminal emulator)"
echo ""

# 5. Install project dependencies
SCRIPT_DIR="$(cd "$(dirname "$0")/.." && pwd)"
echo "Installing npm dependencies in $SCRIPT_DIR..."
cd "$SCRIPT_DIR"
npm ci

echo ""
echo "=== Bootstrap complete ==="
echo "Run tests with: npm test"
