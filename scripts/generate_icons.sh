#!/usr/bin/env bash
set -euo pipefail

# Generate platform-specific icon formats from the source PNG.
# Usage: ./scripts/generate_icons.sh
#
# Outputs:
#   resources/icons/markamp.icns  - macOS icon (requires sips + iconutil)
#   resources/icons/markamp.ico   - Windows icon (requires ImageMagick convert)

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
ICON_DIR="${PROJECT_DIR}/resources/icons"
SRC_PNG="${ICON_DIR}/markamp.png"

if [[ ! -f "${SRC_PNG}" ]]; then
    echo "Error: Source icon not found at ${SRC_PNG}"
    exit 1
fi

echo "=== Generating platform icons from ${SRC_PNG} ==="

# ── macOS .icns ──
if command -v sips &>/dev/null && command -v iconutil &>/dev/null; then
    echo "--- Generating macOS .icns ---"
    ICONSET_DIR=$(mktemp -d)/markamp.iconset
    mkdir -p "${ICONSET_DIR}"

    for SIZE in 16 32 128 256 512; do
        sips -z "${SIZE}" "${SIZE}" "${SRC_PNG}" --out "${ICONSET_DIR}/icon_${SIZE}x${SIZE}.png" &>/dev/null
        DOUBLE=$((SIZE * 2))
        if [[ ${DOUBLE} -le 1024 ]]; then
            sips -z "${DOUBLE}" "${DOUBLE}" "${SRC_PNG}" --out "${ICONSET_DIR}/icon_${SIZE}x${SIZE}@2x.png" &>/dev/null
        fi
    done

    iconutil -c icns "${ICONSET_DIR}" -o "${ICON_DIR}/markamp.icns"
    rm -rf "$(dirname "${ICONSET_DIR}")"
    echo "  Created: ${ICON_DIR}/markamp.icns"
else
    echo "  Skipped .icns (requires macOS sips + iconutil)"
fi

# ── Windows .ico ──
if command -v convert &>/dev/null; then
    echo "--- Generating Windows .ico ---"
    convert "${SRC_PNG}" \
        -define icon:auto-resize=256,128,64,48,32,16 \
        "${ICON_DIR}/markamp.ico"
    echo "  Created: ${ICON_DIR}/markamp.ico"
elif command -v magick &>/dev/null; then
    echo "--- Generating Windows .ico (ImageMagick 7) ---"
    magick "${SRC_PNG}" \
        -define icon:auto-resize=256,128,64,48,32,16 \
        "${ICON_DIR}/markamp.ico"
    echo "  Created: ${ICON_DIR}/markamp.ico"
else
    echo "  Skipped .ico (requires ImageMagick: brew install imagemagick)"
fi

echo "=== Icon generation complete ==="
ls -la "${ICON_DIR}"/markamp.*
