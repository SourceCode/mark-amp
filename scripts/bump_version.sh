#!/usr/bin/env bash
set -euo pipefail

# Bump the MarkAmp version across all packaging files and create a git tag.
# Usage: ./scripts/bump_version.sh <major.minor.patch>
#   e.g. ./scripts/bump_version.sh 1.1.0

if [[ $# -ne 1 ]]; then
    echo "Usage: $0 <major.minor.patch>"
    echo "  e.g. $0 1.1.0"
    exit 1
fi

NEW_VERSION="$1"

# Validate format
if ! [[ "${NEW_VERSION}" =~ ^[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
    echo "Error: version must be in X.Y.Z format (got '${NEW_VERSION}')"
    exit 1
fi

IFS='.' read -r MAJOR MINOR PATCH <<< "${NEW_VERSION}"

SCRIPT_DIR="$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)"
PROJECT_DIR="$(cd "${SCRIPT_DIR}/.." && pwd)"

echo "=== Bumping MarkAmp to v${NEW_VERSION} ==="

# 1. CMakeLists.txt
CMAKELISTS="${PROJECT_DIR}/CMakeLists.txt"
if [[ -f "${CMAKELISTS}" ]]; then
    sed -i.bak -E "s/project\(MarkAmp VERSION [0-9]+\.[0-9]+\.[0-9]+/project(MarkAmp VERSION ${NEW_VERSION}/" "${CMAKELISTS}"
    rm -f "${CMAKELISTS}.bak"
    echo "  Updated CMakeLists.txt"
fi

# 2. Info.plist (macOS)
INFO_PLIST="${PROJECT_DIR}/packaging/macos/Info.plist"
if [[ -f "${INFO_PLIST}" ]]; then
    # Update CFBundleShortVersionString and CFBundleVersion
    sed -i.bak -E "/<key>CFBundleShortVersionString<\/key>/{n;s/<string>[^<]*<\/string>/<string>${NEW_VERSION}<\/string>/;}" "${INFO_PLIST}"
    sed -i.bak -E "/<key>CFBundleVersion<\/key>/{n;s/<string>[^<]*<\/string>/<string>${NEW_VERSION}<\/string>/;}" "${INFO_PLIST}"
    rm -f "${INFO_PLIST}.bak"
    echo "  Updated Info.plist"
fi

# 3. NSIS installer
INSTALLER_NSI="${PROJECT_DIR}/packaging/windows/installer.nsi"
if [[ -f "${INSTALLER_NSI}" ]]; then
    sed -i.bak -E "s/!define VERSION \"[^\"]*\"/!define VERSION \"${NEW_VERSION}\"/" "${INSTALLER_NSI}"
    rm -f "${INSTALLER_NSI}.bak"
    echo "  Updated installer.nsi"
fi

# 4. AppxManifest.xml (MSIX)
APPX_MANIFEST="${PROJECT_DIR}/packaging/windows/AppxManifest.xml"
if [[ -f "${APPX_MANIFEST}" ]]; then
    sed -i.bak -E "s/Version=\"[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+\"/Version=\"${MAJOR}.${MINOR}.${PATCH}.0\"/" "${APPX_MANIFEST}"
    rm -f "${APPX_MANIFEST}.bak"
    echo "  Updated AppxManifest.xml"
fi

echo ""
echo "Version updated to ${NEW_VERSION} in all packaging files."
echo ""

# 5. Git tag
read -rp "Create git tag v${NEW_VERSION}? [y/N] " CONFIRM
if [[ "${CONFIRM}" =~ ^[Yy]$ ]]; then
    cd "${PROJECT_DIR}"
    git add -A
    git commit -m "chore: bump version to ${NEW_VERSION}"
    git tag -a "v${NEW_VERSION}" -m "Release v${NEW_VERSION}"
    echo "  Created tag v${NEW_VERSION}"
    echo "  Push with: git push origin main --tags"
else
    echo "  Skipped git tag. Commit and tag manually when ready."
fi

echo "=== Done ==="
