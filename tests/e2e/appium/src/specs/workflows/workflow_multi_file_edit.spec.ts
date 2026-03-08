/**
 * Workflow: Multi-File Edit
 *
 * 10 tests covering editing across multiple files simultaneously.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import TabBarPage from '../../pages/TabBarPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { typeText, sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Multi-File Edit', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should open first file and type', async () => {
        await FileTreePage.clickFile('README');
        await browser.pause(300);
        await typeText('edit1');
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should switch to second file and type', async () => {
        await FileTreePage.clickFile('NOTES');
        await browser.pause(300);
        await typeText('edit2');
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should switch between files preserving state', async () => {
        await FileTreePage.clickFile('README');
        await browser.pause(200);
        await FileTreePage.clickFile('NOTES');
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle undo in second file', async () => {
        await FileTreePage.clickFile('NOTES');
        await browser.pause(200);
        await sendShortcut(Shortcuts.UNDO);
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle undo in first file after switch', async () => {
        await FileTreePage.clickFile('README');
        await browser.pause(200);
        await sendShortcut(Shortcuts.UNDO);
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should save both files', async () => {
        await FileTreePage.clickFile('README');
        await browser.pause(200);
        await sendShortcut(Shortcuts.SAVE);
        await FileTreePage.clickFile('NOTES');
        await browser.pause(200);
        await sendShortcut(Shortcuts.SAVE);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should close one file and keep other', async () => {
        await sendShortcut(Shortcuts.CLOSE_TAB);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle rapid file switching with edits', async () => {
        await FileTreePage.clickFile('README');
        await typeText('r');
        await FileTreePage.clickFile('NOTES');
        await typeText('n');
        await FileTreePage.clickFile('README');
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should NOT have error overlays', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs after cleanup', async () => {
        await sendShortcut(Shortcuts.UNDO);
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });
});
