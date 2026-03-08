/**
 * Workflow: Editor Clipboard
 *
 * 10 tests covering copy, cut, paste operations.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, typeText, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Editor Clipboard', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
        await FileTreePage.clickFile('NOTES');
        await browser.pause(500);
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should handle Copy (Cmd+C) without crash', async () => {
        await sendShortcut(Shortcuts.SELECT_ALL);
        await browser.pause(200);
        await sendShortcut('Cmd+C');
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Paste (Cmd+V) without crash', async () => {
        await sendShortcut('Cmd+V');
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cut (Cmd+X) without crash', async () => {
        await sendShortcut(Shortcuts.SELECT_ALL);
        await browser.pause(200);
        await sendShortcut('Cmd+X');
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Undo after paste', async () => {
        await sendShortcut('Cmd+V');
        await browser.pause(200);
        await sendShortcut(Shortcuts.UNDO);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Copy → switch file → Paste', async () => {
        await sendShortcut(Shortcuts.SELECT_ALL);
        await sendShortcut('Cmd+C');
        await browser.pause(200);
        await FileTreePage.clickFile('README');
        await browser.pause(300);
        await sendShortcut('Cmd+V');
        await browser.pause(300);
        await sendShortcut(Shortcuts.UNDO);
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle duplicate line (type, select, copy, paste)', async () => {
        await typeText('duplicate this');
        await browser.pause(200);
        await sendShortcut('Cmd+Shift+Left');
        await sendShortcut('Cmd+C');
        await sendShortcut('Cmd+V');
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle clipboard operations after undo', async () => {
        await sendShortcut(Shortcuts.UNDO);
        await sendShortcut(Shortcuts.UNDO);
        await sendShortcut(Shortcuts.UNDO);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell during clipboard cycle', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should NOT have error overlays after clipboard ops', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs after clipboard ops', async () => {
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });
});
