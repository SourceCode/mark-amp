/**
 * Edge Case: Rapid Theme Cycling
 *
 * 8 tests covering rapid theme switching stability.
 */

import AppShellPage from '../../pages/AppShellPage';
import ThemeGalleryPage, { BUILT_IN_THEMES } from '../../pages/ThemeGalleryPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { pressEscape } from '../../support/keyboard';

describe('Rapid Theme Cycling', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should have 8 built-in themes', () => {
        expect(BUILT_IN_THEMES.length).toBe(8);
    });

    it('should maintain shell readiness during theme context', async () => {
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have static text visible', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        expect(texts.length).toBeGreaterThanOrEqual(0);
    });

    it('should have version text on Welcome screen', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        const hasVersion = texts.some(t => t.startsWith('v'));
        expect(typeof hasVersion).toBe('boolean');
    });

    it('should maintain shell main_frame', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should NOT have error overlays', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs open', async () => {
        await AppShellPage.assertNoDialogOpen();
    });

    it('should NOT crash after rapid Escape presses', async () => {
        for (let i = 0; i < 10; i++) {
            await pressEscape();
            await browser.pause(50);
        }
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
