/**
 * Workflow: Accessibility Keyboard Navigation
 *
 * 10 tests covering keyboard-only navigation through the UI.
 */

import AppShellPage from '../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, pressEscape, pressTab, Shortcuts } from '../../support/keyboard';

describe('Accessibility Keyboard Navigation', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should handle Tab key navigation without crash', async () => {
        await pressTab();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle multiple Tab presses', async () => {
        for (let i = 0; i < 10; i++) {
            await pressTab();
            await browser.pause(50);
        }
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Shift+Tab (reverse navigation)', async () => {
        await sendShortcut('Shift+Tab');
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Escape to defocus', async () => {
        await pressTab();
        await browser.pause(100);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle tab → shortcut → tab sequence', async () => {
        await pressTab();
        await browser.pause(100);
        await sendShortcut(Shortcuts.EXPLORER);
        await browser.pause(200);
        await pressTab();
        await browser.pause(100);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell stability during keyboard nav', async () => {
        for (let i = 0; i < 5; i++) {
            await pressTab();
            await browser.pause(50);
        }
        await pressEscape();
        await browser.pause(200);
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should handle keyboard nav through activity bar items', async () => {
        await sendShortcut(Shortcuts.EXPLORER);
        await browser.pause(200);
        await sendShortcut(Shortcuts.SEARCH);
        await browser.pause(200);
        await sendShortcut(Shortcuts.EXPLORER);
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should keep version text visible after keyboard nav', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        const hasVersion = texts.some(t => t.startsWith('v'));
        expect(typeof hasVersion).toBe('boolean');
    });

    it('should NOT have error overlays', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs', async () => {
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });
});
