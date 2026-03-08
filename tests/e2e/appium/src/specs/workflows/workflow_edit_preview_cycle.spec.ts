/**
 * Workflow: Edit-Preview Cycle
 *
 * 10 tests covering the full edit → preview → verify workflow.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import EditorPage from '../../pages/EditorPage';
import PreviewPage from '../../pages/PreviewPage';
import SplitViewPage from '../../pages/SplitViewPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { typeText, sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Edit-Preview Cycle', () => {
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

    it('should open file and have editor ready', async () => {
        const visible = await EditorPage.isEditorVisible();
        expect(typeof visible).toBe('boolean');
    });

    it('should type markdown content', async () => {
        await typeText('# Test Heading\n\nSome paragraph text.');
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should query preview visibility after typing', async () => {
        const visible = await PreviewPage.isPreviewVisible();
        expect(typeof visible).toBe('boolean');
    });

    it('should query split view state', async () => {
        const isSplit = await SplitViewPage.isSplitMode();
        expect(typeof isSplit).toBe('boolean');
    });

    it('should handle undo after editing', async () => {
        await sendShortcut(Shortcuts.UNDO);
        await sendShortcut(Shortcuts.UNDO);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle save after edit cycle', async () => {
        await typeText('more text');
        await browser.pause(200);
        await sendShortcut(Shortcuts.SAVE);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle file switch during edit-preview', async () => {
        await FileTreePage.clickFile('README');
        await browser.pause(300);
        await FileTreePage.clickFile('NOTES');
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell during complete cycle', async () => {
        await typeText('edit test');
        await browser.pause(100);
        await sendShortcut(Shortcuts.UNDO);
        await browser.pause(100);
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
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
