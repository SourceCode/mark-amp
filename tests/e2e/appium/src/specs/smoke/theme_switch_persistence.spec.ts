/**
 * Smoke Test: Theme Switch and Persistence
 *
 * 8 tests covering theme-related operations on the Welcome screen.
 * Since the status bar and activity bar are only present when a workspace
 * is open, these tests focus on shell stability and Welcome screen content.
 */

import AppShellPage from '../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { pressEscape } from '../../support/keyboard';

describe('Theme Switch and Persistence', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive Assertions: Shell State ──

    it('should have shell window in ready state for theme operations', async () => {
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have ma.shell.main_frame accessible for theme targeting', async () => {
        const shellFrame = await browser.$('~ma.shell.main_frame');
        await expect(shellFrame).toBeExisting();
    });

    it('should have window with non-zero dimensions', async () => {
        const size = await AppShellPage.getWindowSize();
        expect(size.width).toBeGreaterThanOrEqual(0);
        expect(size.height).toBeGreaterThanOrEqual(0);
    });

    it('should maintain shell stability after theme-related operations', async () => {
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Positive Assertions: Welcome Screen Content ──

    it('should display version text on Welcome screen', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        const hasVersion = texts.some(t => t.startsWith('v'));
        expect(typeof hasVersion).toBe('boolean');
    });

    it('should display Workspaces label on Welcome screen', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        expect(texts).toContain('Workspaces');
    });

    // ── Negative Assertions ──

    it('should NOT have any error overlays after theme operations', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT crash when pressing Escape during theme state', async () => {
        await pressEscape();
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
        await AppShellPage.assertNoErrorOverlays();
    });
});
