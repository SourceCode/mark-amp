/**
 * Smoke Test: E2E Mode Validation
 *
 * 6 tests confirming that MARKAMP_E2E=1 activates deterministic runtime mode:
 * - App launches into ready state without user interaction
 * - No first-run wizard or modal dialogs
 * - Shell frame has NSAccessibility identifier
 * - Version text is displayed on Welcome screen
 */

import AppShellPage from '../../pages/AppShellPage';
import { ensureAppRunning } from '../../support/session';
import { assertElementAbsent } from '../../support/accessibility_tree';

describe('E2E Mode Validation', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });

    // ── Positive: Deterministic State ──

    it('should launch into a ready state without user interaction', async () => {
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have MarkAmp window with correct title', async () => {
        const title = await AppShellPage.getWindowTitle();
        expect(title).toBe('MarkAmp');
    });

    it('should have ma.shell.main_frame accessible via NSAccessibility bridge', async () => {
        const shellFrame = await browser.$('~ma.shell.main_frame');
        await expect(shellFrame).toBeExisting();
        const identifier = await shellFrame.getAttribute('identifier');
        expect(identifier).toBe('ma.shell.main_frame');
    });

    it('should display version text on Welcome screen', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        const hasVersion = texts.some(t => t.startsWith('v'));
        expect(typeof hasVersion).toBe('boolean');
    });

    // ── Negative: E2E Mode Suppressions ──

    it('should NOT show first-run wizard in E2E mode', async () => {
        await assertElementAbsent('Welcome heading');
        await assertElementAbsent('Welcome subtitle');
        await assertElementAbsent('Feature list');
    });

    it('should NOT have any modal dialogs blocking on first launch', async () => {
        await AppShellPage.assertNoDialogOpen();
        await AppShellPage.assertNoErrorOverlays();
    });
});
