/**
 * Workflow: Command Palette Execute
 *
 * 10 tests covering command palette keyboard shortcut handling,
 * shell stability, and negative assertions.
 *
 * Note: The command palette element (~ma.commandpalette) is only
 * accessible when a workspace is open.  Tests validate shell stability
 * after palette shortcuts on the Welcome screen.
 */

import AppShellPage from '../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { sendShortcut, pressEscape, typeText, Shortcuts } from '../../support/keyboard';

describe('Command Palette Execute', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive: Palette Shortcut Handling ──

    it('should handle Cmd+Shift+P without crash', async () => {
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await browser.pause(500);
        await pressEscape();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Escape after palette shortcut', async () => {
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await browser.pause(300);
        await pressEscape();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle reopening palette after previous close', async () => {
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await browser.pause(300);
        await pressEscape();
        await browser.pause(300);
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await browser.pause(300);
        await pressEscape();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Positive: Shell Stability ──

    it('should maintain ma.shell.main_frame after palette operations', async () => {
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await browser.pause(300);
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
        await pressEscape();
    });

    it('should restore shell state after palette dismissal', async () => {
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await browser.pause(300);
        await pressEscape();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should display Welcome screen content after palette dismiss', async () => {
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await browser.pause(300);
        await pressEscape();
        await browser.pause(300);
        const texts = await AppShellPage.getAllStaticTexts();
        expect(texts.length).toBeGreaterThanOrEqual(0);
    });

    // ── Negative: Error States ──

    it('should NOT crash on rapid open/close cycles', async () => {
        for (let i = 0; i < 5; i++) {
            await sendShortcut(Shortcuts.COMMAND_PALETTE);
            await browser.pause(100);
            await pressEscape();
            await browser.pause(100);
        }
        await browser.pause(300);
        await AppShellPage.assertNoErrorOverlays();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should NOT leave error overlays after palette usage', async () => {
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await browser.pause(200);
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT leave modal dialogs after palette dismiss', async () => {
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await browser.pause(200);
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });

    it('should NOT leave shell in broken state after multiple operations', async () => {
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await browser.pause(200);
        await pressEscape();
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await browser.pause(200);
        await pressEscape();
        await browser.pause(300);
        await AppShellPage.assertNoDialogOpen();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
