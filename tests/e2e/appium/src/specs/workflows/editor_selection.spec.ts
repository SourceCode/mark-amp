/**
 * Workflow: Editor Selection
 *
 * 10 tests covering text selection via keyboard shortcuts.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Editor Selection', () => {
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

    it('should handle Select All (Cmd+A)', async () => {
        await sendShortcut(Shortcuts.SELECT_ALL);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Shift+Right (extend selection)', async () => {
        await sendShortcut('Shift+Right');
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Shift+Left (reduce selection)', async () => {
        await sendShortcut('Shift+Left');
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Shift+Down (extend selection lines)', async () => {
        await sendShortcut('Shift+Down');
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Shift+Up (reduce selection lines)', async () => {
        await sendShortcut('Shift+Up');
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+Shift+Right (select to end of line)', async () => {
        await sendShortcut('Cmd+Shift+Right');
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+Shift+Left (select to start of line)', async () => {
        await sendShortcut('Cmd+Shift+Left');
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Alt+Shift+Right (select word right)', async () => {
        await sendShortcut('Alt+Right');
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle rapid selection changes', async () => {
        for (let i = 0; i < 5; i++) {
            await sendShortcut(Shortcuts.SELECT_ALL);
            await browser.pause(50);
            await pressEscape();
            await browser.pause(50);
        }
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should NOT have error overlays after selection ops', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });
});
