/**
 * Edge Case: Panel Toggle Stress
 *
 * 8 tests covering rapid panel, sidebar, and zen mode toggling.
 */

import AppShellPage from '../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Panel Toggle Stress', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should handle rapid sidebar toggle', async () => {
        for (let i = 0; i < 10; i++) {
            await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
            await browser.pause(50);
        }
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle rapid panel toggle', async () => {
        for (let i = 0; i < 10; i++) {
            await sendShortcut(Shortcuts.TOGGLE_PANEL);
            await browser.pause(50);
        }
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle interleaved sidebar and panel toggles', async () => {
        for (let i = 0; i < 5; i++) {
            await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
            await browser.pause(50);
            await sendShortcut(Shortcuts.TOGGLE_PANEL);
            await browser.pause(50);
        }
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle rapid zen mode toggles with sidebar', async () => {
        await sendShortcut(Shortcuts.ZEN_MODE);
        await browser.pause(100);
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await browser.pause(100);
        await sendShortcut(Shortcuts.ZEN_MODE);
        await browser.pause(100);
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell main_frame after stress', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should NOT have error overlays after stress', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs after stress', async () => {
        await AppShellPage.assertNoDialogOpen();
    });

    it('should NOT crash after Escape following stress', async () => {
        await pressEscape();
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
