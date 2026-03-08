/**
 * Workflow: Go to Line
 *
 * 10 tests covering Go to Line dialog (Ctrl+G) operations.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, typeText, pressEscape, pressEnter, Shortcuts } from '../../support/keyboard';

describe('Go to Line', () => {
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

    it('should handle Ctrl+G without crash', async () => {
        await sendShortcut(Shortcuts.GO_TO_LINE);
        await browser.pause(300);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle typing line number and Enter', async () => {
        await sendShortcut(Shortcuts.GO_TO_LINE);
        await browser.pause(300);
        await typeText('10');
        await browser.pause(200);
        await pressEnter();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle dismiss via Escape', async () => {
        await sendShortcut(Shortcuts.GO_TO_LINE);
        await browser.pause(300);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell during Go to Line', async () => {
        await sendShortcut(Shortcuts.GO_TO_LINE);
        await browser.pause(300);
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
        await pressEscape();
    });

    it('should handle rapid open/close of Go to Line', async () => {
        for (let i = 0; i < 3; i++) {
            await sendShortcut(Shortcuts.GO_TO_LINE);
            await browser.pause(100);
            await pressEscape();
            await browser.pause(100);
        }
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle invalid line number', async () => {
        await sendShortcut(Shortcuts.GO_TO_LINE);
        await browser.pause(300);
        await typeText('99999');
        await pressEnter();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle zero as line number', async () => {
        await sendShortcut(Shortcuts.GO_TO_LINE);
        await browser.pause(300);
        await typeText('0');
        await pressEnter();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should NOT have error overlays after Go to Line', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs after Escape', async () => {
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });

    it('should NOT crash after all Go to Line operations', async () => {
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
