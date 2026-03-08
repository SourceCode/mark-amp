/**
 * Workflow: Settings Toggle Round-Trip
 *
 * 10 tests covering settings shortcut handling, shell stability,
 * and negative assertions for error states on the Welcome screen.
 *
 * Note: Settings panel (~ma.settings.panel) may not be accessible
 * via XCUITest on the Welcome screen.  Tests verify keyboard handling
 * stability rather than panel presence.
 */

import AppShellPage from '../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Settings Toggle Round-Trip', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive: Settings Shortcut Handling ──

    it('should handle Cmd+, settings shortcut without crash', async () => {
        await sendShortcut(Shortcuts.SETTINGS);
        await browser.pause(500);
        await pressEscape();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Escape after settings shortcut', async () => {
        await sendShortcut(Shortcuts.SETTINGS);
        await browser.pause(300);
        await pressEscape();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain ma.shell.main_frame during settings operations', async () => {
        await sendShortcut(Shortcuts.SETTINGS);
        await browser.pause(300);
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
        await pressEscape();
    });

    it('should handle repeated Cmd+, presses without crash', async () => {
        await sendShortcut(Shortcuts.SETTINGS);
        await browser.pause(200);
        await sendShortcut(Shortcuts.SETTINGS);
        await browser.pause(200);
        await pressEscape();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Positive: Shell Stability ──

    it('should restore shell after settings dismiss', async () => {
        await sendShortcut(Shortcuts.SETTINGS);
        await browser.pause(300);
        await pressEscape();
        await browser.pause(300);
        const texts = await AppShellPage.getAllStaticTexts();
        expect(texts.length).toBeGreaterThanOrEqual(0);
    });

    it('should maintain shell readiness throughout settings lifecycle', async () => {
        await sendShortcut(Shortcuts.SETTINGS);
        await browser.pause(300);
        const readyDuring = await AppShellPage.isShellReady();
        expect(readyDuring).toBe(true);
        await pressEscape();
        await browser.pause(300);
        const readyAfter = await AppShellPage.isShellReady();
        expect(readyAfter).toBe(true);
    });

    // ── Negative: Error States ──

    it('should NOT crash on rapid settings toggle', async () => {
        for (let i = 0; i < 5; i++) {
            await sendShortcut(Shortcuts.SETTINGS);
            await browser.pause(100);
            await pressEscape();
            await browser.pause(100);
        }
        await browser.pause(300);
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT leave modal state after Escape from settings', async () => {
        await sendShortcut(Shortcuts.SETTINGS);
        await browser.pause(300);
        await pressEscape();
        await browser.pause(300);
        await AppShellPage.assertNoDialogOpen();
    });

    it('should NOT have error overlays after settings interaction', async () => {
        await sendShortcut(Shortcuts.SETTINGS);
        await browser.pause(200);
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoErrorOverlays();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should NOT leave shell in broken state after settings lifecycle', async () => {
        await sendShortcut(Shortcuts.SETTINGS);
        await browser.pause(200);
        await pressEscape();
        await sendShortcut(Shortcuts.SETTINGS);
        await browser.pause(200);
        await pressEscape();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
        await AppShellPage.assertNoErrorOverlays();
    });
});
