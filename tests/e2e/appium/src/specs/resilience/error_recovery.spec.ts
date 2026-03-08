/**
 * @resilience Resilience — Error recovery and stability
 * Phase 09: Application resilience under adverse conditions.
 */
import AppShellPage from '../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';
import { waitForIdle } from '../../support/waits';
import { verifyAppRecovery } from '../../support/chaos';

describe('@resilience Resilience — Error recovery', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });
    afterEach(async () => { await resetAppState(); });

    it('should recover from multiple dialogs opened simultaneously', async () => {
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await waitForIdle(200);
        await sendShortcut(Shortcuts.FIND);
        await waitForIdle(200);
        await pressEscape();
        await waitForIdle(200);
        await pressEscape();
        await waitForIdle(200);
        expect(await verifyAppRecovery()).toBe(true);
    });

    it('should recover from rapid sidebar toggles', async () => {
        for (let i = 0; i < 5; i++) {
            await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
            await waitForIdle(100);
        }
        expect(await verifyAppRecovery()).toBe(true);
    });

    it('should maintain stability after error scenarios', async () => {
        await AppShellPage.assertNoErrorOverlays();
        expect(await AppShellPage.isShellReady()).toBe(true);
    });
});
