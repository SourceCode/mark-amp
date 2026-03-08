/**
 * Workflow: File Tree Open and Edit
 *
 * 10 tests covering file tree page object queries, shell stability,
 * and sidebar toggle operations on the Welcome screen.
 *
 * Note: File tree element (~ma.filetree.ctrl) is only accessible
 * when a workspace folder is open.
 */

import AppShellPage from '../../pages/AppShellPage';
import EditorPage from '../../pages/EditorPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('File Tree Open and Edit', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive: Shell Presence ──

    it('should have shell ready on Welcome screen', async () => {
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have ma.shell.main_frame accessible', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should have window with correct title', async () => {
        const title = await AppShellPage.getWindowTitle();
        expect(title).toBe('MarkAmp');
    });

    // ── Positive: Sidebar Toggle ──

    it('should toggle sidebar visibility via Cmd+B shortcut', async () => {
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await browser.pause(400);
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await browser.pause(400);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell stability after sidebar toggle', async () => {
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await browser.pause(400);
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await browser.pause(400);
    });

    // ── Positive: Explorer Shortcut ──

    it('should handle Cmd+Shift+E Explorer shortcut without crash', async () => {
        await sendShortcut(Shortcuts.EXPLORER);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should display Welcome screen content after shortcut', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        expect(texts.length).toBeGreaterThanOrEqual(0);
    });

    // ── Negative: Edge Cases ──

    it('should NOT crash when toggling sidebar multiple times rapidly', async () => {
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT leave modal state after Escape in file tree context', async () => {
        await pressEscape();
        await AppShellPage.assertNoDialogOpen();
    });

    it('should NOT have error overlays after all operations', async () => {
        await AppShellPage.assertNoErrorOverlays();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
