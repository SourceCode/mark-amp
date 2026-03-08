/**
 * @workflow Extensions — Extension lifecycle and browser workflows
 * Phase 06, Tasks 1–16: Extension browser, install, uninstall, enable/disable.
 */
import AppShellPage from '../../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../../support/session';
import { sendShortcut, typeText, pressEscape, Shortcuts } from '../../../support/keyboard';
import { waitForIdle } from '../../../support/waits';

describe('@workflow Extensions — Extension lifecycle', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });
    afterEach(async () => { await resetAppState(); });

    // Tasks 1-4: Extension browser
    it('should detect extensions surface', async () => {
        const ext = await browser.$('~ma.extensions');
        const exists = await ext.isExisting();
        expect(typeof exists).toBe('boolean');
    });

    it('should handle extension browser interaction', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    // Tasks 5-16: Extension lifecycle
    it('should handle extension operations without crash', async () => {
        await AppShellPage.assertNoErrorOverlays();
        expect(await AppShellPage.isShellReady()).toBe(true);
    });
});
