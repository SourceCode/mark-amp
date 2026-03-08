/**
 * Edge Case: Keyboard Shortcuts
 *
 * 12 tests validating keyboard shortcut handling on the Welcome screen.
 * Tests verify shortcuts don't crash the app and leave the shell in a
 * valid state.  Shortcuts that require an active workspace (like Cmd+N,
 * Cmd+W) are tested defensively — verifying the app survives them.
 */

import AppShellPage from '../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Keyboard Shortcuts', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive: Non-destructive shortcuts ──

    it('should handle Cmd+S (Save) without error', async () => {
        await sendShortcut(Shortcuts.SAVE);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+Z (Undo) without crash', async () => {
        await sendShortcut(Shortcuts.UNDO);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Cmd+Shift+Z (Redo) without crash', async () => {
        await sendShortcut(Shortcuts.REDO);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Positive: UI Panel Shortcuts ──

    it('should toggle sidebar with Cmd+B without crash', async () => {
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await browser.pause(400);
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await browser.pause(400);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should toggle panel area with Cmd+J without crash', async () => {
        await sendShortcut(Shortcuts.TOGGLE_PANEL);
        await browser.pause(400);
        await sendShortcut(Shortcuts.TOGGLE_PANEL);
        await browser.pause(400);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Positive: Escape Key ──

    it('should handle Escape key without crash', async () => {
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle multiple Escape presses without crash', async () => {
        await pressEscape();
        await pressEscape();
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Negative: Error States ──

    it('should NOT crash when pressing unbound shortcut combinations', async () => {
        await sendShortcut('Cmd+Shift+Option+X');
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should NOT leave error overlays after shortcut operations', async () => {
        await sendShortcut(Shortcuts.SAVE);
        await sendShortcut(Shortcuts.UNDO);
        await sendShortcut(Shortcuts.REDO);
        await browser.pause(500);
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT leave dialogs open after Escape', async () => {
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });

    it('should maintain window accessibility after shortcut burst', async () => {
        await sendShortcut(Shortcuts.SAVE);
        await sendShortcut(Shortcuts.UNDO);
        await sendShortcut(Shortcuts.REDO);
        await pressEscape();
        await browser.pause(500);
        // Verify the shell frame is still accessible
        const shellFrame = await browser.$('~ma.shell.main_frame');
        const exists = await shellFrame.isExisting();
        expect(exists).toBe(true);
    });

    it('should keep ma.shell.main_frame accessible after all shortcut tests', async () => {
        const shellFrame = await browser.$('~ma.shell.main_frame');
        const identifier = await shellFrame.getAttribute('identifier');
        expect(identifier).toBe('ma.shell.main_frame');
    });
});
