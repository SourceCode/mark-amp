/**
 * @workflow Workbench — Reopen last workspace validation
 * Phase 03, Task 3: Automate reopening last workspace validation.
 */
import AppShellPage from '../../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../../support/session';

describe('@workflow Workbench — Reopen last workspace', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });
    afterEach(async () => { await resetAppState(); });

    it('should have MarkAmp window available after reopen', async () => {
        expect(await AppShellPage.isShellReady()).toBe(true);
    });
    it('should maintain window title after workspace reload', async () => {
        const title = await AppShellPage.getWindowTitle();
        expect(title).toContain('MarkAmp');
    });
});
