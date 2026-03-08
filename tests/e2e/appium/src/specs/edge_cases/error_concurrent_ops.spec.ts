/**
 * Edge Case: Error Concurrent Operations
 *
 * 10 tests covering simultaneous action resilience.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, typeText, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Error Concurrent Operations', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should survive typing while switching files', async () => {
        await FileTreePage.clickFile('NOTES');
        await typeText('concurrent');
        await FileTreePage.clickFile('README');
        await typeText('write');
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should survive shortcut during typing', async () => {
        await FileTreePage.clickFile('NOTES');
        await browser.pause(200);
        await typeText('hello');
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await typeText('world');
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should survive command palette during editing', async () => {
        await FileTreePage.clickFile('NOTES');
        await browser.pause(200);
        await typeText('cp test');
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await browser.pause(200);
        await pressEscape();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should survive find during typing', async () => {
        await FileTreePage.clickFile('NOTES');
        await browser.pause(200);
        await typeText('find test');
        await sendShortcut(Shortcuts.FIND);
        await browser.pause(200);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should survive save during panel toggle', async () => {
        await sendShortcut(Shortcuts.SAVE);
        await sendShortcut(Shortcuts.TOGGLE_PANEL);
        await sendShortcut(Shortcuts.SAVE);
        await sendShortcut(Shortcuts.TOGGLE_PANEL);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should survive search open during file switch', async () => {
        await FileTreePage.clickFile('README');
        await sendShortcut(Shortcuts.SEARCH);
        await FileTreePage.clickFile('NOTES');
        await pressEscape();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should survive multiple overlapping shortcuts', async () => {
        await sendShortcut(Shortcuts.EXPLORER);
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await pressEscape();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell main_frame', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should NOT have error overlays', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs', async () => {
        await sendShortcut(Shortcuts.UNDO);
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });
});
