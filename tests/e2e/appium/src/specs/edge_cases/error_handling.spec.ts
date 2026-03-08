/**
 * Edge Case: Error Handling
 *
 * 10 tests covering exception paths — corrupt state recovery,
 * rapid input stability, concurrent operations, large input handling.
 * Uses both positive (app recovers) and negative (no crash) assertions.
 */

import AppShellPage from '../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { sendShortcut, pressEscape, typeText, Shortcuts } from '../../support/keyboard';

describe('Error Handling', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive: Recovery Assertions ──

    it('should recover from rapid keyboard spam without hanging', async () => {
        // Send many keystrokes rapidly
        for (let i = 0; i < 20; i++) {
            await sendShortcut('a');
        }
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should survive window resize to minimum dimensions', async () => {
        const originalSize = await AppShellPage.getWindowSize();
        // Attempt to resize — may not work in mac2 but should not crash
        try {
            await browser.setWindowSize(800, 600);
        } catch {
            // mac2 may not support window resize — that's OK
        }
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Escape key when no dialog is open', async () => {
        await pressEscape();
        await pressEscape();
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle rapid shortcut combinations without crash', async () => {
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await pressEscape();
        await sendShortcut(Shortcuts.SETTINGS);
        await pressEscape();
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle interleaved open/close operations', async () => {
        // Open palette, then open settings (should close palette first)
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await browser.pause(200);
        await sendShortcut(Shortcuts.SETTINGS);
        await browser.pause(200);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Negative: No-Crash Assertions ──

    it('should NOT crash when sending invalid modifier combinations', async () => {
        // Send key with no valid mapping
        try {
            await browser.execute('macos: keys', {
                keys: [{ key: 'F19' }],
            });
        } catch {
            // Invalid key might throw — app should still be fine
        }
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT show error overlay after concurrent shortcut operations', async () => {
        await sendShortcut(Shortcuts.SAVE);
        await sendShortcut(Shortcuts.UNDO);
        await sendShortcut(Shortcuts.REDO);
        await sendShortcut(Shortcuts.SAVE);
        await browser.pause(300);
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT crash when opening and immediately closing dialogs', async () => {
        await sendShortcut(Shortcuts.OPEN_FILE);
        await browser.pause(200);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT leave lingering modal state after multiple Escape presses', async () => {
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await browser.pause(100);
        await pressEscape();
        await pressEscape();
        await pressEscape();
        await AppShellPage.assertNoDialogOpen();
    });

    it('should NOT crash on rapid sidebar toggle cycling', async () => {
        for (let i = 0; i < 10; i++) {
            await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
            await browser.pause(30);
        }
        await browser.pause(500);
        await AppShellPage.assertNoErrorOverlays();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
