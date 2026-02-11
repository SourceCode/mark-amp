#!/usr/bin/env bash
set -euo pipefail

# Package MarkAmp for the Mac App Store (.pkg with App Sandbox entitlements).
# Usage: ./scripts/package_macos_appstore.sh
#
# Environment variables:
#   MARKAMP_MAS_APP_CERT      - "3rd Party Mac Developer Application: ..." certificate
#   MARKAMP_MAS_INSTALLER_CERT - "3rd Party Mac Developer Installer: ..." certificate
#   MARKAMP_PROVISIONING_PROFILE - Path to .provisionprofile for App Store

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${PROJECT_DIR}/build/release"
DIST_DIR="${PROJECT_DIR}/dist"
APP_NAME="MarkAmp"
APP_BUNDLE="${DIST_DIR}/${APP_NAME}.app"
PKG_NAME="${APP_NAME}-macOS-AppStore.pkg"
ENTITLEMENTS="${PROJECT_DIR}/resources/entitlements/markamp.entitlements"

VERSION=$(grep -oE 'project\(MarkAmp VERSION ([0-9]+\.[0-9]+\.[0-9]+)' "${PROJECT_DIR}/CMakeLists.txt" | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')
echo "=== Packaging MarkAmp v${VERSION} for Mac App Store ==="

# Validate prerequisites
if [[ ! -f "${BUILD_DIR}/src/markamp" ]]; then
    echo "Error: Release binary not found. Run ./scripts/build_release.sh first."
    exit 1
fi
if [[ ! -f "${ENTITLEMENTS}" ]]; then
    echo "Error: Entitlements file not found at ${ENTITLEMENTS}"
    exit 1
fi

# Build the .app bundle first (reuse package_macos.sh logic)
export MARKAMP_SKIP_CODESIGN=1
"${SCRIPT_DIR}/package_macos.sh"

# ── Embed provisioning profile ──
PROFILE="${MARKAMP_PROVISIONING_PROFILE:-}"
if [[ -n "${PROFILE}" ]] && [[ -f "${PROFILE}" ]]; then
    echo "--- Embedding provisioning profile ---"
    cp "${PROFILE}" "${APP_BUNDLE}/Contents/embedded.provisionprofile"
fi

# ── Code sign with App Store certificate and entitlements ──
APP_CERT="${MARKAMP_MAS_APP_CERT:-}"
if [[ -n "${APP_CERT}" ]]; then
    echo "--- Signing for App Store ---"
    codesign --deep --force --verify --verbose \
        --sign "${APP_CERT}" \
        --entitlements "${ENTITLEMENTS}" \
        --options runtime \
        "${APP_BUNDLE}"
    echo "  Signed with: ${APP_CERT}"
else
    echo "  Warning: MARKAMP_MAS_APP_CERT not set. Set it to sign for App Store."
fi

# ── Create .pkg for App Store ──
INSTALLER_CERT="${MARKAMP_MAS_INSTALLER_CERT:-}"
SIGN_ARGS=()
if [[ -n "${INSTALLER_CERT}" ]]; then
    SIGN_ARGS=(--sign "${INSTALLER_CERT}")
fi

echo "--- Creating App Store .pkg ---"
productbuild \
    --component "${APP_BUNDLE}" /Applications \
    "${SIGN_ARGS[@]}" \
    --product "${APP_BUNDLE}/Contents/Info.plist" \
    "${DIST_DIR}/${PKG_NAME}"

echo "  Created: ${DIST_DIR}/${PKG_NAME}"

# ── Validate ──
echo "--- Validating .pkg ---"
if command -v xcrun &>/dev/null; then
    xcrun altool --validate-app -f "${DIST_DIR}/${PKG_NAME}" -t macos 2>&1 || echo "  Note: Validation requires Apple credentials. Upload to App Store Connect manually."
fi

echo ""
echo "=== Mac App Store packaging complete ==="
echo "  Package: ${DIST_DIR}/${PKG_NAME}"
echo ""
echo "Next steps:"
echo "  1. Upload to App Store Connect via Transporter or xcrun altool"
echo "  2. Fill out app metadata, screenshots, and description"
echo "  3. Submit for review"
