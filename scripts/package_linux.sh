#!/usr/bin/env bash
set -euo pipefail

# Package MarkAmp for Linux: .deb, AppImage, and .tar.gz
# Usage: ./scripts/package_linux.sh
#
# Prerequisites:
#   - dpkg-deb  (for .deb packaging)
#   - linuxdeploy (optional, for AppImage — download from GitHub releases)

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"
BUILD_DIR="${PROJECT_DIR}/build/release"
DIST_DIR="${PROJECT_DIR}/dist"
APP_NAME="markamp"
DISPLAY_NAME="MarkAmp"

VERSION=$(grep -oE 'project\(MarkAmp VERSION ([0-9]+\.[0-9]+\.[0-9]+)' "${PROJECT_DIR}/CMakeLists.txt" | grep -oE '[0-9]+\.[0-9]+\.[0-9]+')
ARCH="$(dpkg --print-architecture 2>/dev/null || echo "amd64")"

echo "=== Packaging MarkAmp v${VERSION} for Linux (${ARCH}) ==="

if [[ ! -f "${BUILD_DIR}/src/markamp" ]]; then
    echo "Error: Release binary not found at ${BUILD_DIR}/src/markamp"
    echo "Run ./scripts/build_release.sh first."
    exit 1
fi

rm -rf "${DIST_DIR}"
mkdir -p "${DIST_DIR}"

# ═══════════════════════════════════════════════
# 1. Debian package (.deb)
# ═══════════════════════════════════════════════
echo ""
echo "--- Creating .deb package ---"

DEB_ROOT="${DIST_DIR}/deb-staging"
DEB_NAME="${APP_NAME}_${VERSION}_${ARCH}.deb"

mkdir -p "${DEB_ROOT}/usr/bin"
mkdir -p "${DEB_ROOT}/usr/share/${APP_NAME}/themes"
mkdir -p "${DEB_ROOT}/usr/share/${APP_NAME}/fonts"
mkdir -p "${DEB_ROOT}/usr/share/applications"
mkdir -p "${DEB_ROOT}/usr/share/icons/hicolor/256x256/apps"
mkdir -p "${DEB_ROOT}/DEBIAN"

# Binary
cp "${BUILD_DIR}/src/markamp" "${DEB_ROOT}/usr/bin/markamp"
chmod 755 "${DEB_ROOT}/usr/bin/markamp"

# Resources
cp -r "${PROJECT_DIR}/resources/themes/"* "${DEB_ROOT}/usr/share/${APP_NAME}/themes/" 2>/dev/null || true
cp -r "${PROJECT_DIR}/resources/fonts/"* "${DEB_ROOT}/usr/share/${APP_NAME}/fonts/" 2>/dev/null || true

# Desktop entry
cp "${PROJECT_DIR}/packaging/linux/markamp.desktop" "${DEB_ROOT}/usr/share/applications/"

# Icon
cp "${PROJECT_DIR}/resources/icons/markamp.png" "${DEB_ROOT}/usr/share/icons/hicolor/256x256/apps/"

# Control file
cat > "${DEB_ROOT}/DEBIAN/control" << EOF
Package: ${APP_NAME}
Version: ${VERSION}
Section: editors
Priority: optional
Architecture: ${ARCH}
Depends: libgtk-3-0, libwebkit2gtk-4.0-37 | libwebkit2gtk-4.1-0
Maintainer: MarkAmp <hello@markamp.dev>
Description: Retro-futuristic Markdown editor
 MarkAmp is a cross-platform Markdown editor with live preview,
 Mermaid diagram rendering, syntax highlighting, and a unique
 retro-futuristic theme system.
Homepage: https://github.com/markamp/markamp
EOF

# Build .deb
dpkg-deb --build --root-owner-group "${DEB_ROOT}" "${DIST_DIR}/${DEB_NAME}" 2>/dev/null || \
    fakeroot dpkg-deb --build "${DEB_ROOT}" "${DIST_DIR}/${DEB_NAME}" 2>/dev/null || \
    echo "  Warning: dpkg-deb not available, skipping .deb"

if [[ -f "${DIST_DIR}/${DEB_NAME}" ]]; then
    echo "  Created: ${DIST_DIR}/${DEB_NAME}"
fi
rm -rf "${DEB_ROOT}"

# ═══════════════════════════════════════════════
# 2. AppImage
# ═══════════════════════════════════════════════
echo ""
echo "--- Creating AppImage ---"

APPIMAGE_NAME="${DISPLAY_NAME}-${VERSION}-x86_64.AppImage"
APPDIR="${DIST_DIR}/AppDir"

mkdir -p "${APPDIR}/usr/bin"
mkdir -p "${APPDIR}/usr/share/${APP_NAME}"
mkdir -p "${APPDIR}/usr/share/applications"
mkdir -p "${APPDIR}/usr/share/icons/hicolor/256x256/apps"

cp "${BUILD_DIR}/src/markamp" "${APPDIR}/usr/bin/markamp"
chmod 755 "${APPDIR}/usr/bin/markamp"
cp -r "${PROJECT_DIR}/resources/themes" "${APPDIR}/usr/share/${APP_NAME}/" 2>/dev/null || true
cp -r "${PROJECT_DIR}/resources/fonts" "${APPDIR}/usr/share/${APP_NAME}/" 2>/dev/null || true
cp "${PROJECT_DIR}/packaging/linux/markamp.desktop" "${APPDIR}/usr/share/applications/"
cp "${PROJECT_DIR}/resources/icons/markamp.png" "${APPDIR}/usr/share/icons/hicolor/256x256/apps/"

# Symlinks required by AppImage spec
ln -sf usr/share/applications/markamp.desktop "${APPDIR}/markamp.desktop"
ln -sf usr/share/icons/hicolor/256x256/apps/markamp.png "${APPDIR}/markamp.png"

if command -v linuxdeploy &>/dev/null; then
    linuxdeploy --appdir "${APPDIR}" --output appimage
    mv "${DISPLAY_NAME}"*.AppImage "${DIST_DIR}/${APPIMAGE_NAME}" 2>/dev/null || true
    echo "  Created: ${DIST_DIR}/${APPIMAGE_NAME}"
elif command -v appimagetool &>/dev/null; then
    appimagetool "${APPDIR}" "${DIST_DIR}/${APPIMAGE_NAME}"
    echo "  Created: ${DIST_DIR}/${APPIMAGE_NAME}"
else
    echo "  Warning: linuxdeploy/appimagetool not found, skipping AppImage"
    echo "  Install from: https://github.com/linuxdeploy/linuxdeploy/releases"
fi
rm -rf "${APPDIR}"

# ═══════════════════════════════════════════════
# 3. Tarball (.tar.gz)
# ═══════════════════════════════════════════════
echo ""
echo "--- Creating tarball ---"

TARBALL_NAME="${DISPLAY_NAME}-${VERSION}-linux-x86_64.tar.gz"
TARBALL_DIR="${DIST_DIR}/${DISPLAY_NAME}-${VERSION}"

mkdir -p "${TARBALL_DIR}"
cp "${BUILD_DIR}/src/markamp" "${TARBALL_DIR}/"
chmod 755 "${TARBALL_DIR}/markamp"
cp -r "${PROJECT_DIR}/resources/themes" "${TARBALL_DIR}/" 2>/dev/null || true
cp -r "${PROJECT_DIR}/resources/fonts" "${TARBALL_DIR}/" 2>/dev/null || true
cp "${PROJECT_DIR}/packaging/linux/markamp.desktop" "${TARBALL_DIR}/"
cp "${PROJECT_DIR}/resources/icons/markamp.png" "${TARBALL_DIR}/"

# Install script
cat > "${TARBALL_DIR}/install.sh" << 'INSTALL_EOF'
#!/usr/bin/env bash
set -euo pipefail
PREFIX="${1:-/usr/local}"
echo "Installing MarkAmp to ${PREFIX}..."
sudo install -Dm 755 markamp "${PREFIX}/bin/markamp"
sudo install -Dm 644 markamp.desktop "${PREFIX}/share/applications/markamp.desktop"
sudo install -Dm 644 markamp.png "${PREFIX}/share/icons/hicolor/256x256/apps/markamp.png"
if [[ -d themes ]]; then
    sudo mkdir -p "${PREFIX}/share/markamp"
    sudo cp -r themes "${PREFIX}/share/markamp/"
fi
if [[ -d fonts ]]; then
    sudo cp -r fonts "${PREFIX}/share/markamp/"
fi
echo "MarkAmp installed to ${PREFIX}/bin/markamp"
INSTALL_EOF
chmod +x "${TARBALL_DIR}/install.sh"

# Create tarball
tar -czf "${DIST_DIR}/${TARBALL_NAME}" -C "${DIST_DIR}" "$(basename "${TARBALL_DIR}")"
rm -rf "${TARBALL_DIR}"
echo "  Created: ${DIST_DIR}/${TARBALL_NAME}"

echo ""
echo "=== Linux packaging complete ==="
ls -lh "${DIST_DIR}/"
