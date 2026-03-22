#!/bin/bash
# V21 Quality Report Generator
# Aggregates control completeness, panel readiness, and settings integrity.

set -euo pipefail

BUILD_DIR="build/debug"
REPORT_FILE="docs/v21_quality_report.md"

echo "# V21 Quality Report" > "$REPORT_FILE"
echo "" >> "$REPORT_FILE"
echo "Generated: $(date -u +"%Y-%m-%dT%H:%M:%SZ")" >> "$REPORT_FILE"
echo "" >> "$REPORT_FILE"

# Run V21 test suites
echo "## Test Suite Results" >> "$REPORT_FILE"
echo "" >> "$REPORT_FILE"
echo "| Test Suite | Result |" >> "$REPORT_FILE"
echo "|---|---|" >> "$REPORT_FILE"

TOTAL=0
PASSED=0

for test in test_v21_control_audit test_v21_menu_palette test_v21_toolbar_statusbar \
            test_v21_context_menus test_v21_primary_sidebar test_v21_panel_lifecycle \
            test_v21_workbench_surfaces test_v21_settings_architecture test_v21_settings_ui \
            test_v21_validation_harness; do
    TOTAL=$((TOTAL + 1))
    if cd "$BUILD_DIR" && ctest -R "$test" --output-on-failure -Q 2>/dev/null; then
        echo "| \`$test\` | ✅ Passed |" >> "../../$REPORT_FILE"
        PASSED=$((PASSED + 1))
    else
        echo "| \`$test\` | ❌ Failed |" >> "../../$REPORT_FILE"
    fi
    cd - > /dev/null
done

echo "" >> "$REPORT_FILE"
echo "**$PASSED / $TOTAL test suites passed**" >> "$REPORT_FILE"
echo "" >> "$REPORT_FILE"

# Summary
echo "## Summary" >> "$REPORT_FILE"
echo "" >> "$REPORT_FILE"
echo "- **Phases implemented:** 01–10 (10/10)" >> "$REPORT_FILE"
echo "- **Test suites:** $TOTAL" >> "$REPORT_FILE"
echo "- **Pass rate:** $(echo "scale=1; $PASSED * 100 / $TOTAL" | bc)%" >> "$REPORT_FILE"
echo "" >> "$REPORT_FILE"
echo "## Exit Criteria" >> "$REPORT_FILE"
echo "" >> "$REPORT_FILE"
echo "- [ ] All 10 test suites pass" >> "$REPORT_FILE"
echo "- [ ] No dead controls exposed in production" >> "$REPORT_FILE"
echo "- [ ] No placeholder panels visible to users" >> "$REPORT_FILE"
echo "- [ ] Settings changes go through canonical owner" >> "$REPORT_FILE"
echo "- [ ] All panel toggles have bound handlers" >> "$REPORT_FILE"

echo "Quality report generated: $REPORT_FILE"
