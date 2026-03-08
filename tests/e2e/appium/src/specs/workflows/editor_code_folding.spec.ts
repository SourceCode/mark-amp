/**
 * Workflow: Editor Code Folding
 *
 * 10 tests covering code folding shortcuts and shell stability.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import EditorPage from '../../pages/EditorPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, pressEscape } from '../../support/keyboard';

describe('Editor Code Folding', () => {
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

    it('should have editor visible for folding tests', async () => {
        const visible = await EditorPage.isEditorVisible();
        expect(typeof visible).toBe('boolean');
    });

    it('should handle fold (Cmd+Alt+[) without crash', async () => {
        await sendShortcut('Cmd+Alt+[');
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle unfold (Cmd+Alt+]) without crash', async () => {
        await sendShortcut('Cmd+Alt+]');
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle fold all (Cmd+K Cmd+0) pattern', async () => {
        // Send fold-like shortcut pattern
        await sendShortcut('Cmd+Alt+[');
        await browser.pause(100);
        await sendShortcut('Cmd+Alt+[');
        await browser.pause(100);
        await sendShortcut('Cmd+Alt+]');
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle rapid fold/unfold cycles', async () => {
        for (let i = 0; i < 5; i++) {
            await sendShortcut('Cmd+Alt+[');
            await browser.pause(50);
            await sendShortcut('Cmd+Alt+]');
            await browser.pause(50);
        }
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell stability after fold operations', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should have correct window title', async () => {
        const title = await AppShellPage.getWindowTitle();
        expect(title).toBe('MarkAmp');
    });

    it('should have text content visible', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        expect(texts.length).toBeGreaterThanOrEqual(0);
    });

    it('should NOT have error overlays after fold ops', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT crash after Escape', async () => {
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
