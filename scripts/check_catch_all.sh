#!/usr/bin/env bash
# check_catch_all.sh — Phase 22: Find catch(...) patterns in src/
#
# Reports file + line for any catch(...) usage. Exit code 1 if found.
# The intent is to replace these with typed exception handlers.

set -euo pipefail

SRC_DIR="${1:-src}"

echo "=== Scanning for catch(...) patterns in ${SRC_DIR}/ ==="
echo

FOUND=0
while IFS=: read -r file line content; do
    echo "  ${file}:${line}  ${content}"
    FOUND=$((FOUND + 1))
done < <(grep -rn 'catch\s*(\.\.\.)' "${SRC_DIR}" --include='*.cpp' --include='*.h' || true)

echo
if [ "${FOUND}" -gt 0 ]; then
    echo "⚠️  Found ${FOUND} catch(...) pattern(s). Consider replacing with typed handlers."
    exit 1
else
    echo "✅ No catch(...) patterns found."
    exit 0
fi
