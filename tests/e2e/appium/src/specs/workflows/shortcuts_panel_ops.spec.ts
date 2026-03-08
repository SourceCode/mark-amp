/**
 * Workflow: Shortcuts — Panel Operations
 *
 * 10 tests covering exhaustive panel/layout keyboard shortcuts.
 */

import AppShellPage from '../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Shortcuts Panel Operations', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should handle Cmd+B (Toggle Sidebar)', async () => {
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await browser.pause(300);
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+J (Toggle Panel)', async () => {
        await sendShortcut(Shortcuts.TOGGLE_PANEL);
        await browser.pause(300);
        await sendShortcut(Shortcuts.TOGGLE_PANEL);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+Shift+P (Command Palette)', async () => {
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await browser.pause(400);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+, (Settings)', async () => {
        await sendShortcut(Shortcuts.SETTINGS);
        await browser.pause(400);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+Shift+E (Explorer)', async () => {
        await sendShortcut(Shortcuts.EXPLORER);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+Shift+F (Search)', async () => {
        await sendShortcut(Shortcuts.SEARCH);
        await browser.pause(300);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+Shift+Return (Zen Mode)', async () => {
        await sendShortcut(Shortcuts.ZEN_MODE);
        await browser.pause(400);
        await sendShortcut(Shortcuts.ZEN_MODE);
        await browser.pause(400);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle all panel shortcuts in rapid sequence', async () => {
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await sendShortcut(Shortcuts.TOGGLE_PANEL);
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await sendShortcut(Shortcuts.TOGGLE_PANEL);
        await browser.pause(400);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
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
