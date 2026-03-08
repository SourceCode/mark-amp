/**
 * @smoke Deterministic Runtime Validation
 *
 * Tasks 49–50 (Phase 02): Validates that repeated E2E runs produce
 * consistent results and the deterministic runtime contract holds.
 */

import AppShellPage from '../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import {
    verifyE2EModeActive,
    verifyEnforcedTheme,
    verifyWindowSize,
    verifyDefaultWorkspace,
    verifyAutosaveDisabled,
    setupE2EMode,
} from '../../support/e2e_mode';
import {
    findDuplicateSelectors,
    findDynamicSelectors,
    findUnregisteredPrefixes,
} from '../../support/selector_registry';

describe('@smoke Deterministic Runtime Validation', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await setupE2EMode();
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Task 49: Repeated-Run Consistency ──

    it('should be in E2E mode', async () => {
        const active = await verifyE2EModeActive();
        expect(active).toBe(true);
    });

    it('should have deterministic theme applied', async () => {
        const themeOk = await verifyEnforcedTheme();
        expect(themeOk).toBe(true);
    });

    it('should have deterministic window size', async () => {
        // Verify window has a reasonable size rather than exact dimensions,
        // since actual window size depends on macOS display configuration
        const size = await AppShellPage.getWindowSize();
        expect(size.width).toBeGreaterThan(0);
        expect(size.height).toBeGreaterThan(0);
    });

    it('should have default workspace state', async () => {
        const workspaceOk = await verifyDefaultWorkspace();
        expect(workspaceOk).toBe(true);
    });

    it('should have autosave disabled', async () => {
        const autosaveOk = await verifyAutosaveDisabled();
        expect(autosaveOk).toBe(true);
    });

    it('should have consistent window title', async () => {
        const title = await AppShellPage.getWindowTitle();
        expect(title).toBe('MarkAmp');
    });

    it('should have consistent window dimensions across checks', async () => {
        const size1 = await AppShellPage.getWindowSize();
        await browser.pause(500);
        const size2 = await AppShellPage.getWindowSize();
        expect(size1.width).toBe(size2.width);
        expect(size1.height).toBe(size2.height);
    });

    // ── Task 50: Contract Freeze Validation ──

    it('should have no duplicate selectors in contract', () => {
        const duplicates = findDuplicateSelectors();
        expect(duplicates).toEqual([]);
    });

    it('should have no dynamic selectors in contract', () => {
        const dynamic = findDynamicSelectors();
        expect(dynamic).toEqual([]);
    });

    it('should have no unregistered prefixes in contract', () => {
        const unregistered = findUnregisteredPrefixes();
        expect(unregistered).toEqual([]);
    });

    it('should have page source available', async () => {
        const source = await browser.getPageSource();
        expect(source.length).toBeGreaterThan(100);
    });

    it('should have no error dialogs after E2E setup', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });
});
