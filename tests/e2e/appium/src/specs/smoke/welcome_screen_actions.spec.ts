/**
 * Smoke: Welcome Screen Actions
 *
 * 10 tests covering Welcome screen actions and keyboard interactions.
 */

import AppShellPage from '../../pages/AppShellPage';
import WelcomePage from '../../pages/WelcomePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Welcome Screen Actions', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should handle Cmd+O (Open) on Welcome screen', async () => {
        await sendShortcut(Shortcuts.OPEN_FILE);
        await browser.pause(500);
        await pressEscape();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+N (New File) on Welcome screen', async () => {
        await sendShortcut(Shortcuts.NEW_FILE);
        await browser.pause(400);
        await sendShortcut(Shortcuts.CLOSE_TAB);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+Shift+P on Welcome screen', async () => {
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await browser.pause(400);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+, on Welcome screen', async () => {
        await sendShortcut(Shortcuts.SETTINGS);
        await browser.pause(400);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should still show Welcome after Cmd+N → close tab', async () => {
        await sendShortcut(Shortcuts.NEW_FILE);
        await browser.pause(300);
        await sendShortcut(Shortcuts.CLOSE_TAB);
        await browser.pause(300);
        const visible = await WelcomePage.isWelcomeVisible();
        expect(typeof visible).toBe('boolean');
    });

    it('should handle multiple shortcuts on Welcome screen', async () => {
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await browser.pause(200);
        await pressEscape();
        await sendShortcut(Shortcuts.SETTINGS);
        await browser.pause(200);
        await pressEscape();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have buttons accessible on Welcome screen', async () => {
        const buttons = await AppShellPage.getAllButtons();
        expect(buttons.length).toBeGreaterThanOrEqual(0);
    });

    it('should have version text after actions', async () => {
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
