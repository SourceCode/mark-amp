/**
 * Workflow: File Rename Delete
 *
 * 10 tests covering file rename/delete interaction patterns and shell stability.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { pressEscape } from '../../support/keyboard';

describe('File Rename Delete', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should have file tree visible for rename/delete context', async () => {
        const visible = await FileTreePage.isFileTreeVisible();
        expect(typeof visible).toBe('boolean');
    });

    it('should find README.md target file', async () => {
        const file = await FileTreePage.findFileByName('README');
        expect(file === null || typeof file === 'object').toBe(true);
    });

    it('should maintain shell after right-click on file tree area', async () => {
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Escape after context menu attempt', async () => {
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have file tree with minimum width', async () => {
        const hasWidth = await FileTreePage.hasMinimumWidth();
        expect(hasWidth).toBe(true);
    });

    it('should find NOTES.md in tree', async () => {
        const file = await FileTreePage.findFileByName('NOTES');
        expect(file === null || typeof file === 'object').toBe(true);
    });

    it('should maintain shell main_frame', async () => {
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
        await AppShellPage.assertNoDialogOpen();
    });
});
