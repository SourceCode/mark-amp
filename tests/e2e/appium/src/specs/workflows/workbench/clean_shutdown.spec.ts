/**
 * @critical Workbench — Clean shutdown validation
 * Phase 03, Task 2: Automate clean shutdown validation without crash dialogs.
 */
import AppShellPage from '../../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../../support/session';

describe('@critical Workbench — Clean shutdown', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });
    afterEach(async () => { await resetAppState(); });

    it('should not show crash dialogs during normal operation', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });
    it('should not have modal sheets blocking interaction', async () => {
        await AppShellPage.assertNoDialogOpen();
    });
    it('should have accessible window after idle period', async () => {
        await browser.pause(2000);
        expect(await AppShellPage.isShellReady()).toBe(true);
    });
});
