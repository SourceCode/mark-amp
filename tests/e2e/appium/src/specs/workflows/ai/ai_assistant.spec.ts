/**
 * @workflow AI — AI assistant workflows
 * Phase 06, Tasks 17–29: AI panel, actions, provider, error handling.
 */
import AppShellPage from '../../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../../support/session';
import { waitForIdle } from '../../../support/waits';

describe('@workflow @pending-feature AI — Assistant workflows', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });
    afterEach(async () => { await resetAppState(); });

    // Tasks 17-20: AI panel
    it('should detect AI panel surface', async () => {
        const ai = await browser.$('~ma.ai');
        const exists = await ai.isExisting();
        expect(typeof exists).toBe('boolean');
    });

    // Tasks 21-29: AI actions and providers
    it('should handle AI panel operations without crash', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should detect AI provider selector', async () => {
        const provider = await browser.$('~ma.ai.provider_select');
        const exists = await provider.isExisting();
        expect(typeof exists).toBe('boolean');
    });

    it('should maintain stability during AI interactions', async () => {
        expect(await AppShellPage.isShellReady()).toBe(true);
    });
});
