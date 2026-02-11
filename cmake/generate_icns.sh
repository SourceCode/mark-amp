#!/usr/bin/env bash
# Generate macOS .icns from a source image (PNG with alpha).
# Usage: generate_icns.sh <source_image> <output.icns>
#
# Prefers ImageMagick (magick) to preserve alpha/transparency.
# Falls back to sips + iconutil if ImageMagick is not available.
set -euo pipefail

SRC="$1"
OUTPUT="$2"
ICONSET="${OUTPUT%.icns}.iconset"

rm -rf "${ICONSET}"
mkdir -p "${ICONSET}"

if command -v magick &>/dev/null; then
    # ImageMagick — preserves RGBA and transparency correctly
    for SIZE in 16 32 128 256 512; do
        magick "${SRC}" -resize "${SIZE}x${SIZE}" -background none -flatten PNG32:"${ICONSET}/icon_${SIZE}x${SIZE}.png"
    done
    magick "${SRC}" -resize 32x32     -background none -flatten PNG32:"${ICONSET}/icon_16x16@2x.png"
    magick "${SRC}" -resize 64x64     -background none -flatten PNG32:"${ICONSET}/icon_32x32@2x.png"
    magick "${SRC}" -resize 256x256   -background none -flatten PNG32:"${ICONSET}/icon_128x128@2x.png"
    magick "${SRC}" -resize 512x512   -background none -flatten PNG32:"${ICONSET}/icon_256x256@2x.png"
    magick "${SRC}" -resize 1024x1024 -background none -flatten PNG32:"${ICONSET}/icon_512x512@2x.png"
else
    # Fallback: sips (may not preserve alpha from JPEG sources)
    for SIZE in 16 32 128 256 512; do
        sips -z "${SIZE}" "${SIZE}" -s format png "${SRC}" --out "${ICONSET}/icon_${SIZE}x${SIZE}.png" > /dev/null 2>&1
    done
    sips -z 32 32     -s format png "${SRC}" --out "${ICONSET}/icon_16x16@2x.png"   > /dev/null 2>&1
    sips -z 64 64     -s format png "${SRC}" --out "${ICONSET}/icon_32x32@2x.png"   > /dev/null 2>&1
    sips -z 256 256   -s format png "${SRC}" --out "${ICONSET}/icon_128x128@2x.png" > /dev/null 2>&1
    sips -z 512 512   -s format png "${SRC}" --out "${ICONSET}/icon_256x256@2x.png" > /dev/null 2>&1
    sips -z 1024 1024 -s format png "${SRC}" --out "${ICONSET}/icon_512x512@2x.png" > /dev/null 2>&1
fi

iconutil -c icns "${ICONSET}" -o "${OUTPUT}"
rm -rf "${ICONSET}"
