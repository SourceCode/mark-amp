/**
 * Workflow: Editor Cursor Movement
 *
 * 10 tests covering cursor navigation with arrow keys, Home/End, page up/down.
 */

import AppShellPage from '../../pages/AppShellPage';
import EditorPage from '../../pages/EditorPage';
import FileTreePage from '../../pages/FileTreePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, pressEscape, pressArrowDown, pressArrowUp, pressArrowLeft, pressArrowRight } from '../../support/keyboard';

describe('Editor Cursor Movement', () => {
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

    it('should handle arrow down without crash', async () => {
        await pressArrowDown();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle arrow up without crash', async () => {
        await pressArrowUp();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle arrow left without crash', async () => {
        await pressArrowLeft();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle arrow right without crash', async () => {
        await pressArrowRight();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle rapid consecutive arrow presses', async () => {
        for (let i = 0; i < 20; i++) {
            await pressArrowDown();
        }
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Home key (Cmd+Left) without crash', async () => {
        await sendShortcut('Cmd+Left');
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle End key (Cmd+Right) without crash', async () => {
        await sendShortcut('Cmd+Right');
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+Up (top of file) without crash', async () => {
        await sendShortcut('Cmd+Up');
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+Down (bottom of file) without crash', async () => {
        await sendShortcut('Cmd+Down');
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should NOT have error overlays after cursor movement', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });
});
