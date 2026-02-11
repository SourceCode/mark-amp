#!/usr/bin/env bash
set -euo pipefail

# Package MarkAmp as a macOS .app bundle inside a .dmg disk image.
# Usage: ./scripts/package_macos.sh
#
# Environment variables:
#   MARKAMP_DEVELOPER_ID   - Code signing identity (e.g. "Developer ID Application: ...")
#   MARKAMP_SKIP_CODESIGN  - Set to "1" to skip code signing (CI without certs)
#   MARKAMP_APPLE_ID       - Apple ID for notarization
#   MARKAMP_APPLE_PASSWORD - App-specific password for notarization
#   MARKAMP_TEAM_ID        - Apple Developer Team ID

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${PROJECT_DIR}/build/release"
DIST_DIR="${PROJECT_DIR}/dist"
APP_NAME="MarkAmp"
APP_BUNDLE="${DIST_DIR}/${APP_NAME}.app"
DMG_NAME="${APP_NAME}-macOS.dmg"

# Read version from CMakeLists.txt
VERSION=$(grep -oE 'project\(MarkAmp VERSION ([0-9]+\.[0-9]+\.[0-9]+)' "${PROJECT_DIR}/CMakeLists.txt" | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')
echo "=== Packaging MarkAmp v${VERSION} for macOS ==="

# Ensure release build exists
if [[ ! -f "${BUILD_DIR}/src/markamp" ]]; then
    echo "Error: Release binary not found at ${BUILD_DIR}/src/markamp"
    echo "Run ./scripts/build_release.sh first."
    exit 1
fi

# Clean previous packaging
rm -rf "${APP_BUNDLE}" "${DIST_DIR}/${DMG_NAME}"
mkdir -p "${DIST_DIR}"

# ── Create .app bundle structure ──
echo "--- Creating .app bundle ---"
mkdir -p "${APP_BUNDLE}/Contents/MacOS"
mkdir -p "${APP_BUNDLE}/Contents/Resources"

# Copy binary
cp "${BUILD_DIR}/src/markamp" "${APP_BUNDLE}/Contents/MacOS/markamp"

# Copy Info.plist
cp "${PROJECT_DIR}/packaging/macos/Info.plist" "${APP_BUNDLE}/Contents/"

# Copy resources
if [[ -d "${PROJECT_DIR}/resources/themes" ]]; then
    cp -r "${PROJECT_DIR}/resources/themes" "${APP_BUNDLE}/Contents/Resources/"
fi
if [[ -d "${PROJECT_DIR}/resources/fonts" ]]; then
    cp -r "${PROJECT_DIR}/resources/fonts" "${APP_BUNDLE}/Contents/Resources/"
fi
if [[ -d "${PROJECT_DIR}/resources/sample_files" ]]; then
    cp -r "${PROJECT_DIR}/resources/sample_files" "${APP_BUNDLE}/Contents/Resources/"
fi

# Copy icon (convert PNG to ICNS if needed)
ICON_SRC="${PROJECT_DIR}/resources/icons/markamp.png"
ICNS_DST="${APP_BUNDLE}/Contents/Resources/markamp.icns"
if command -v sips &>/dev/null && command -v iconutil &>/dev/null && [[ -f "${ICON_SRC}" ]]; then
    echo "  Converting icon to .icns"
    ICONSET_DIR=$(mktemp -d)/markamp.iconset
    mkdir -p "${ICONSET_DIR}"
    for SIZE in 16 32 64 128 256 512; do
        sips -z "${SIZE}" "${SIZE}" "${ICON_SRC}" --out "${ICONSET_DIR}/icon_${SIZE}x${SIZE}.png" &>/dev/null
        DOUBLE=$((SIZE * 2))
        if [[ ${DOUBLE} -le 1024 ]]; then
            sips -z "${DOUBLE}" "${DOUBLE}" "${ICON_SRC}" --out "${ICONSET_DIR}/icon_${SIZE}x${SIZE}@2x.png" &>/dev/null
        fi
    done
    iconutil -c icns "${ICONSET_DIR}" -o "${ICNS_DST}" 2>/dev/null || cp "${ICON_SRC}" "${APP_BUNDLE}/Contents/Resources/markamp.png"
    rm -rf "$(dirname "${ICONSET_DIR}")"
else
    echo "  Warning: sips/iconutil not available, copying PNG icon directly"
    cp "${ICON_SRC}" "${APP_BUNDLE}/Contents/Resources/markamp.png" 2>/dev/null || true
fi

# Write PkgInfo
echo -n "APPLMAMP" > "${APP_BUNDLE}/Contents/PkgInfo"

# ── Code signing ──
if [[ "${MARKAMP_SKIP_CODESIGN:-0}" != "1" ]]; then
    IDENTITY="${MARKAMP_DEVELOPER_ID:-}"
    if [[ -n "${IDENTITY}" ]]; then
        echo "--- Code signing ---"
        codesign --deep --force --verify --verbose \
            --sign "${IDENTITY}" \
            --options runtime \
            "${APP_BUNDLE}"
        echo "  Signed with: ${IDENTITY}"
    else
        echo "  Warning: MARKAMP_DEVELOPER_ID not set, skipping code signing"
    fi
else
    echo "  Skipping code signing (MARKAMP_SKIP_CODESIGN=1)"
fi

# ── Create DMG ──
echo "--- Creating DMG ---"
hdiutil create \
    -volname "${APP_NAME}" \
    -srcfolder "${APP_BUNDLE}" \
    -ov \
    -format UDZO \
    "${DIST_DIR}/${DMG_NAME}"

echo "  Created: ${DIST_DIR}/${DMG_NAME}"

# ── Notarize (if credentials available) ──
if [[ "${MARKAMP_SKIP_CODESIGN:-0}" != "1" ]] && [[ -n "${MARKAMP_APPLE_ID:-}" ]]; then
    echo "--- Notarizing ---"
    xcrun notarytool submit "${DIST_DIR}/${DMG_NAME}" \
        --apple-id "${MARKAMP_APPLE_ID}" \
        --password "${MARKAMP_APPLE_PASSWORD}" \
        --team-id "${MARKAMP_TEAM_ID}" \
        --wait
    xcrun stapler staple "${DIST_DIR}/${DMG_NAME}"
    echo "  Notarization complete"
fi

echo "=== macOS packaging complete ==="
echo "  App bundle: ${APP_BUNDLE}"
echo "  DMG:        ${DIST_DIR}/${DMG_NAME}"
