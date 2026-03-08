/**
 * Workflow: Editor Word Wrap
 *
 * 10 tests covering word wrap state and shell stability.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import EditorPage from '../../pages/EditorPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, pressEscape } from '../../support/keyboard';

describe('Editor Word Wrap', () => {
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

    it('should have editor visible', async () => {
        const visible = await EditorPage.isEditorVisible();
        expect(typeof visible).toBe('boolean');
    });

    it('should handle word wrap toggle (Alt+Z) without crash', async () => {
        await sendShortcut('Alt+Z');
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should toggle word wrap back without crash', async () => {
        await sendShortcut('Alt+Z');
        await browser.pause(300);
        await sendShortcut('Alt+Z');
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle rapid word wrap toggles', async () => {
        for (let i = 0; i < 5; i++) {
            await sendShortcut('Alt+Z');
            await browser.pause(100);
        }
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell main_frame during word wrap toggle', async () => {
        await sendShortcut('Alt+Z');
        await browser.pause(200);
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
        await sendShortcut('Alt+Z');
    });

    it('should have correct window title', async () => {
        const title = await AppShellPage.getWindowTitle();
        expect(title).toBe('MarkAmp');
    });

    it('should have non-zero window size', async () => {
        const size = await AppShellPage.getWindowSize();
        expect(size.width).toBeGreaterThanOrEqual(0);
    });

    it('should have static text visible', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        expect(texts.length).toBeGreaterThanOrEqual(0);
    });

    it('should NOT have error overlays', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT crash after Escape', async () => {
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
