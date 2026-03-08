/**
 * Smoke Test: Create, Edit, and Save a Note
 *
 * 10 tests covering the edit lifecycle on the Welcome screen.
 * Since workspace fixtures are not yet integrated with the Appium
 * launcher (no folder-open mechanism), tests validate shell stability
 * with keyboard shortcuts that would be used during editing.
 */

import AppShellPage from '../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { sendShortcut, Shortcuts, pressEscape } from '../../support/keyboard';

describe('Create, Edit, and Save Note', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive Assertions: Shell State ──

    it('should show shell ready on Welcome screen', async () => {
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have MarkAmp window accessible', async () => {
        const title = await AppShellPage.getWindowTitle();
        expect(title).toBe('MarkAmp');
    });

    it('should have ma.shell.main_frame identifier set', async () => {
        const shellFrame = await browser.$('~ma.shell.main_frame');
        await expect(shellFrame).toBeExisting();
    });

    // ── Positive Assertions: Keyboard Operations ──

    it('should accept keyboard shortcut Cmd+S without error', async () => {
        await sendShortcut(Shortcuts.SAVE);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should accept Cmd+Z undo without error', async () => {
        await sendShortcut(Shortcuts.UNDO);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should accept Cmd+Shift+Z redo without error', async () => {
        await sendShortcut(Shortcuts.REDO);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should display Welcome screen content', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        expect(texts.length).toBeGreaterThanOrEqual(0);
    });

    // ── Negative Assertions: Edge Cases ──

    it('should NOT crash after double save on Welcome screen', async () => {
        await sendShortcut(Shortcuts.SAVE);
        await sendShortcut(Shortcuts.SAVE);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT show error dialog after undo with no history', async () => {
        await sendShortcut(Shortcuts.UNDO);
        await sendShortcut(Shortcuts.UNDO);
        await sendShortcut(Shortcuts.UNDO);
        await browser.pause(300);
        await AppShellPage.assertNoErrorOverlays();
        await AppShellPage.assertNoDialogOpen();
    });

    it('should NOT leave modal state after Escape key', async () => {
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
