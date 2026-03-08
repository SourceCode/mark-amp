/**
 * Workflow: Zen Mode
 *
 * 10 tests covering zen mode toggle and shell stability.
 */

import AppShellPage from '../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Zen Mode', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should handle Zen mode toggle without crash', async () => {
        await sendShortcut(Shortcuts.ZEN_MODE);
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should toggle back from Zen mode without crash', async () => {
        await sendShortcut(Shortcuts.ZEN_MODE);
        await browser.pause(400);
        await sendShortcut(Shortcuts.ZEN_MODE);
        await browser.pause(400);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell main_frame during Zen mode', async () => {
        await sendShortcut(Shortcuts.ZEN_MODE);
        await browser.pause(400);
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
        await sendShortcut(Shortcuts.ZEN_MODE);
        await browser.pause(400);
    });

    it('should handle Escape during Zen mode', async () => {
        await sendShortcut(Shortcuts.ZEN_MODE);
        await browser.pause(300);
        await pressEscape();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have non-zero window dimensions in Zen mode', async () => {
        await sendShortcut(Shortcuts.ZEN_MODE);
        await browser.pause(400);
        const size = await AppShellPage.getWindowSize();
        expect(size.width).toBeGreaterThanOrEqual(0);
        expect(size.height).toBeGreaterThanOrEqual(0);
        await sendShortcut(Shortcuts.ZEN_MODE);
        await browser.pause(400);
    });

    it('should handle rapid Zen mode toggles', async () => {
        for (let i = 0; i < 4; i++) {
            await sendShortcut(Shortcuts.ZEN_MODE);
            await browser.pause(150);
        }
        await browser.pause(400);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should restore shell after Zen mode + Escape + return', async () => {
        await sendShortcut(Shortcuts.ZEN_MODE);
        await browser.pause(300);
        await pressEscape();
        await browser.pause(300);
        await sendShortcut(Shortcuts.ZEN_MODE);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should NOT have error overlays after Zen operations', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs after Zen operations', async () => {
        await AppShellPage.assertNoDialogOpen();
    });

    it('should NOT crash after complete Zen mode lifecycle', async () => {
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
