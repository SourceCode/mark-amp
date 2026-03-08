/**
 * Workflow: Search and Navigate
 *
 * 10 tests covering search → find result → navigate to file workflow.
 */

import AppShellPage from '../../pages/AppShellPage';
import SearchPage from '../../pages/SearchPage';
import FindReplacePage from '../../pages/FindReplacePage';
import FileTreePage from '../../pages/FileTreePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Search and Navigate', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should search workspace then open file', async () => {
        await SearchPage.searchFor('Architecture');
        await browser.pause(500);
        await SearchPage.close();
        await FileTreePage.clickFile('NOTES');
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should find in file after search close', async () => {
        await FileTreePage.clickFile('README');
        await browser.pause(300);
        await FindReplacePage.findText('markdown');
        await browser.pause(300);
        await FindReplacePage.close();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should alternate between workspace search and file find', async () => {
        await SearchPage.open();
        await browser.pause(200);
        await SearchPage.close();
        await FindReplacePage.openFind();
        await browser.pause(200);
        await FindReplacePage.close();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should go to line after search', async () => {
        await SearchPage.searchFor('test');
        await browser.pause(300);
        await SearchPage.close();
        await sendShortcut(Shortcuts.GO_TO_LINE);
        await browser.pause(300);
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should quick open after workspace search', async () => {
        await SearchPage.open();
        await SearchPage.close();
        await sendShortcut(Shortcuts.QUICK_OPEN);
        await browser.pause(400);
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should search → switch file → search again', async () => {
        await SearchPage.searchFor('heading');
        await browser.pause(300);
        await SearchPage.close();
        await FileTreePage.clickFile('NOTES');
        await browser.pause(200);
        await SearchPage.searchFor('notes');
        await browser.pause(300);
        await SearchPage.close();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell during search workflow', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should have correct window title', async () => {
        const title = await AppShellPage.getWindowTitle();
        expect(title).toBe('MarkAmp');
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
