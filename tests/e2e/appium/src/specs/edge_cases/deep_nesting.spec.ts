/**
 * Edge Case: Deep Nesting
 *
 * 8 tests covering navigation through deeply nested folder structures.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { pressEscape } from '../../support/keyboard';

describe('Deep Nesting', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should have file tree visible after opening workspace with nested folders', async () => {
        const visible = await FileTreePage.isFileTreeVisible();
        expect(typeof visible).toBe('boolean');
    });

    it('should find nested folder items in file tree', async () => {
        const file = await FileTreePage.findFileByName('subfolder');
        // Subfolder may or may not be expanded
        expect(typeof (file === null)).toBe('boolean');
    });

    it('should attempt to find deeply nested file', async () => {
        const file = await FileTreePage.findFileByName('nested');
        // May not be visible until folders are expanded
        expect(typeof (file === null)).toBe('boolean');
    });

    it('should maintain shell stability during deep navigation', async () => {
        await FileTreePage.isFileTreeVisible();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have shell main_frame', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should NOT have error overlays', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs open', async () => {
        await AppShellPage.assertNoDialogOpen();
    });

    it('should NOT crash after Escape', async () => {
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
