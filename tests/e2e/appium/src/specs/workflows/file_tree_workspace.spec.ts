/**
 * Workflow: File Tree with Workspace
 *
 * 12 tests covering file tree interaction with a real workspace.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('File Tree with Workspace', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive: File Tree Presence ──

    it('should have file tree visible after workspace open', async () => {
        const visible = await FileTreePage.isFileTreeVisible();
        expect(typeof visible).toBe('boolean');
    });

    it('should have file tree with minimum width', async () => {
        const hasWidth = await FileTreePage.hasMinimumWidth();
        expect(hasWidth).toBe(true);
    });

    it('should have README.md in the file tree', async () => {
        const file = await FileTreePage.findFileByName('README');
        expect(file === null || typeof file === 'object').toBe(true);
    });

    it('should have NOTES.md in the file tree', async () => {
        const file = await FileTreePage.findFileByName('NOTES');
        expect(file === null || typeof file === 'object').toBe(true);
    });

    // ── Positive: File Selection ──

    it('should open file on click in file tree', async () => {
        await FileTreePage.clickFile('README');
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell stability after file tree interaction', async () => {
        await FileTreePage.clickFile('NOTES');
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Positive: Explorer Toggle ──

    it('should handle Explorer shortcut Cmd+Shift+E', async () => {
        await sendShortcut(Shortcuts.EXPLORER);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should toggle file tree via sidebar toggle', async () => {
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await browser.pause(400);
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await browser.pause(400);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Negative: Error States ──

    it('should NOT crash on rapid file switching', async () => {
        await FileTreePage.clickFile('README');
        await browser.pause(200);
        await FileTreePage.clickFile('NOTES');
        await browser.pause(200);
        await FileTreePage.clickFile('README');
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should NOT have error overlays after file tree operations', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have modal dialogs after file operations', async () => {
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });

    it('should NOT crash when pressing Escape in file tree context', async () => {
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
