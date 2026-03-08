/**
 * Edge Case: Error Recovery Workflow
 *
 * 10 tests covering recovery from error-like states.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, pressEscape, pressEnter, typeText, Shortcuts } from '../../support/keyboard';

describe('Error Recovery Workflow', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should recover from multiple Escape presses', async () => {
        for (let i = 0; i < 20; i++) {
            await pressEscape();
            await browser.pause(20);
        }
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should recover from dialog dismissal sequence', async () => {
        await sendShortcut(Shortcuts.OPEN_FILE);
        await browser.pause(400);
        await pressEscape();
        await browser.pause(300);
        await sendShortcut(Shortcuts.SETTINGS);
        await browser.pause(300);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should recover from Go to Line with invalid input', async () => {
        await sendShortcut(Shortcuts.GO_TO_LINE);
        await browser.pause(300);
        await typeText('abc');
        await pressEnter();
        await browser.pause(300);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should recover from rapid new file → close × 10', async () => {
        for (let i = 0; i < 10; i++) {
            await sendShortcut(Shortcuts.NEW_FILE);
            await browser.pause(100);
            await sendShortcut(Shortcuts.CLOSE_TAB);
            await browser.pause(100);
        }
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should recover from sidebar toggle during panel toggle', async () => {
        for (let i = 0; i < 5; i++) {
            await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
            await sendShortcut(Shortcuts.TOGGLE_PANEL);
            await browser.pause(50);
        }
        // Restore defaults
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await sendShortcut(Shortcuts.TOGGLE_PANEL);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should recover from search → file switch → search', async () => {
        await sendShortcut(Shortcuts.SEARCH);
        await browser.pause(200);
        await FileTreePage.clickFile('README');
        await sendShortcut(Shortcuts.SEARCH);
        await browser.pause(200);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should recover from zen mode + command palette', async () => {
        await sendShortcut(Shortcuts.ZEN_MODE);
        await browser.pause(200);
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await browser.pause(200);
        await pressEscape();
        await browser.pause(200);
        await sendShortcut(Shortcuts.ZEN_MODE);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell main_frame after recovery', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should NOT have error overlays after recovery', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs after full recovery', async () => {
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });
});
