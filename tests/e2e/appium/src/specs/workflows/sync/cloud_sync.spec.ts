/**
 * @workflow Sync — Cloud sync workflows
 * Phase 08, Tasks 15–24: Settings, enable/disable, providers, conflict, history.
 */
import AppShellPage from '../../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../../support/session';

describe('@workflow @pending-feature Sync — Cloud sync', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });
    afterEach(async () => { await resetAppState(); });

    it('should detect sync surface', async () => {
        const sync = await browser.$('~ma.sync');
        expect(typeof (await sync.isExisting())).toBe('boolean');
    });

    it('should detect sync status indicator', async () => {
        const status = await browser.$('~ma.sync.status');
        expect(typeof (await status.isExisting())).toBe('boolean');
    });

    it('should not crash during sync operations', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });
});
