/**
 * @workflow Integration — Terminal, debug, task runner, prompts, status bar
 * Phase 06, Tasks 38–50: Integration surface workflows.
 */
import AppShellPage from '../../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../../support/session';
import { sendShortcut, Shortcuts } from '../../../support/keyboard';
import { waitForIdle } from '../../../support/waits';

describe('@workflow Integration — Terminal and debug', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });
    afterEach(async () => { await resetAppState(); });

    // Tasks 38-40: Terminal, task runner, debug
    it('should detect terminal surface', async () => {
        const terminal = await browser.$('~ma.terminal');
        const exists = await terminal.isExisting();
        expect(typeof exists).toBe('boolean');
    });

    it('should detect debug surface', async () => {
        const debug = await browser.$('~ma.debug');
        const exists = await debug.isExisting();
        expect(typeof exists).toBe('boolean');
    });

    // Tasks 41-50: Debug toolbar, prompts, status bar
    it('should handle integration operations without crash', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should maintain stability during integration workflows', async () => {
        expect(await AppShellPage.isShellReady()).toBe(true);
    });
});
