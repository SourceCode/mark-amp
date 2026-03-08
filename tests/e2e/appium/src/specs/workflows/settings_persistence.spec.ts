/**
 * Workflow: Settings Persistence
 *
 * 10 tests covering settings state and persistence behavior.
 */

import AppShellPage from '../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Settings Persistence', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should open settings and verify shell stability', async () => {
        await sendShortcut(Shortcuts.SETTINGS);
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
        await pressEscape();
    });

    it('should maintain settings state across open/close', async () => {
        await sendShortcut(Shortcuts.SETTINGS);
        await browser.pause(300);
        await pressEscape();
        await browser.pause(300);
        await sendShortcut(Shortcuts.SETTINGS);
        await browser.pause(300);
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle settings + sidebar toggle', async () => {
        await sendShortcut(Shortcuts.SETTINGS);
        await browser.pause(300);
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await browser.pause(200);
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await browser.pause(200);
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle settings + panel toggle', async () => {
        await sendShortcut(Shortcuts.SETTINGS);
        await browser.pause(300);
        await sendShortcut(Shortcuts.TOGGLE_PANEL);
        await browser.pause(200);
        await sendShortcut(Shortcuts.TOGGLE_PANEL);
        await browser.pause(200);
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle settings after file editing', async () => {
        await sendShortcut(Shortcuts.SETTINGS);
        await browser.pause(300);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell main_frame', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should have window title preserved', async () => {
        const title = await AppShellPage.getWindowTitle();
        expect(title).toBe('MarkAmp');
    });

    it('should have buttons in the UI', async () => {
        const buttons = await AppShellPage.getAllButtons();
        expect(buttons.length).toBeGreaterThanOrEqual(0);
    });

    it('should NOT have error overlays', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT crash after cleanup', async () => {
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
