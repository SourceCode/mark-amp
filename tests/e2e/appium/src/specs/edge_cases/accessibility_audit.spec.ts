/**
 * Edge Case: Accessibility Audit
 *
 * 15 tests auditing the accessibility tree for correctness:
 * - NSAccessibility bridge verified for ma.shell.main_frame
 * - Activity bar item metadata consistency
 * - Keyboard navigation stability
 * - Negative assertions for error and crash elements
 *
 * Note: Child panel identifiers are only present when a workspace folder
 * is open.  Tests are designed to work on the Welcome screen.
 */

import AppShellPage, { CONTRACT_SELECTORS } from '../../pages/AppShellPage';
import { ALL_ACTIVITY_BAR_LABELS } from '../../pages/ActivityBarPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import {
    getAllAccessibilityNames,
    findContractSelectors,
    findByExactName,
    assertElementProperties,
    assertElementAbsent,
    countElementsByType,
} from '../../support/accessibility_tree';
import { pressTab, pressEscape } from '../../support/keyboard';

describe('Accessibility Audit', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive: Core Shell Selector ──

    it('should have ma.shell.main_frame in accessibility tree', async () => {
        const el = await findByExactName('ma.shell.main_frame');
        const exists = await el.isExisting();
        expect(exists).toBe(true);
    });

    it('should have ma.shell.main_frame that is enabled', async () => {
        await assertElementProperties('ma.shell.main_frame', {
            enabled: true,
        });
    });

    it('should have ma.shell.main_frame as a Group element type', async () => {
        const el = await findByExactName('ma.shell.main_frame');
        const elementType = await el.getAttribute('elementType');
        // elementType 3 = XCUIElementTypeGroup (set via NSAccessibilityGroupRole)
        expect(elementType).toBe('3');
    });

    // ── Positive: Activity Bar Item Labels ──

    it('should have all 15 activity bar items defined with non-empty labels', () => {
        for (const label of ALL_ACTIVITY_BAR_LABELS) {
            if (label.length === 0) {
                throw new Error('Activity bar label must not be empty');
            }
            expect(label.length).toBeGreaterThan(0);
        }
    });

    it('should have no duplicate activity bar labels', () => {
        const unique = new Set(ALL_ACTIVITY_BAR_LABELS);
        expect(unique.size).toBe(ALL_ACTIVITY_BAR_LABELS.length);
    });

    it('should have exactly 15 activity bar items defined', () => {
        expect(ALL_ACTIVITY_BAR_LABELS.length).toBe(15);
    });

    // ── Positive: Accessibility Tree Integrity ──

    it('should have discoverable accessibility identifiers in the running app', async () => {
        const names = await getAllAccessibilityNames();
        // App should have at least ma.shell.main_frame + system identifiers
        expect(names.length).toBeGreaterThan(0);
    });

    it('should have at least one ma.* contract selector discoverable', async () => {
        const contractNames = await findContractSelectors();
        expect(contractNames.length).toBeGreaterThanOrEqual(1);
        expect(contractNames).toContain('ma.shell.main_frame');
    });

    it('should have Button elements in the accessibility tree', async () => {
        const count = await countElementsByType('Button');
        // Window close/minimize/fullscreen buttons
        expect(count).toBeGreaterThanOrEqual(3);
    });

    it('should have Window element in the accessibility tree', async () => {
        const count = await countElementsByType('Window');
        expect(count).toBeGreaterThanOrEqual(1);
    });

    // ── Positive: Keyboard Navigation ──

    it('should accept Tab key for focus navigation without crash', async () => {
        await pressTab();
        await browser.pause(200);
        await pressTab();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should accept Escape key after Tab navigation without crash', async () => {
        await pressTab();
        await pressTab();
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Negative: Accessibility Anti-Patterns ──

    it('should NOT have any ma.* selectors outside the contract registry', async () => {
        const found = await findContractSelectors();
        const registered = CONTRACT_SELECTORS as readonly string[];
        const unregistered = found.filter(s => !registered.includes(s));
        if (unregistered.length > 0) {
            throw new Error(`Unregistered selectors: ${unregistered.join(', ')}`);
        }
        expect(unregistered.length).toBe(0);
    });

    it('should NOT have any error or crash elements in the tree', async () => {
        await assertElementAbsent('Crash Report');
        await assertElementAbsent('Fatal Error');
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have modal dialogs open by default', async () => {
        await AppShellPage.assertNoDialogOpen();
    });
});
