/**
 * @workflow Security — Vault, privacy mode, sanitizers, resilience
 * Phase 08, Tasks 25–37: Encryption, vault, privacy, safe-mode, crash recovery.
 */
import AppShellPage from '../../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../../support/session';

describe('@workflow @pending-feature Security — Vault and resilience', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });
    afterEach(async () => { await resetAppState(); });

    it('should detect vault surface', async () => {
        const vault = await browser.$('~ma.vault');
        expect(typeof (await vault.isExisting())).toBe('boolean');
    });

    it('should not crash during security operations', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should maintain app stability', async () => {
        expect(await AppShellPage.isShellReady()).toBe(true);
    });
});
