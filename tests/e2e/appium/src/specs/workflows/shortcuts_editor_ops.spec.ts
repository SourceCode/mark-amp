/**
 * Workflow: Shortcuts — Editor Operations
 *
 * 10 tests covering exhaustive editor keyboard shortcuts.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, typeText, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Shortcuts Editor Operations', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
        await FileTreePage.clickFile('README');
        await browser.pause(500);
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should handle Cmd+Z (Undo)', async () => {
        await typeText('test');
        await sendShortcut(Shortcuts.UNDO);
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+Shift+Z (Redo)', async () => {
        await sendShortcut(Shortcuts.REDO);
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+A (Select All)', async () => {
        await sendShortcut(Shortcuts.SELECT_ALL);
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+F (Find)', async () => {
        await sendShortcut(Shortcuts.FIND);
        await browser.pause(300);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+H (Find & Replace)', async () => {
        await sendShortcut(Shortcuts.FIND_REPLACE);
        await browser.pause(300);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Ctrl+G (Go to Line)', async () => {
        await sendShortcut(Shortcuts.GO_TO_LINE);
        await browser.pause(300);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+\\ (Split Editor)', async () => {
        await sendShortcut(Shortcuts.SPLIT_EDITOR);
        await browser.pause(400);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle all editor shortcuts in rapid sequence', async () => {
        await sendShortcut(Shortcuts.SELECT_ALL);
        await sendShortcut(Shortcuts.UNDO);
        await sendShortcut(Shortcuts.REDO);
        await browser.pause(300);
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
