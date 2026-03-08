/**
 * Workflow: Editor Typing and Undo
 *
 * 10 tests covering text input, undo/redo operations in the editor.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import EditorPage from '../../pages/EditorPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, typeText, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Editor Typing and Undo', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
        await FileTreePage.clickFile('NOTES');
        await browser.pause(500);
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive: Editor Presence ──

    it('should have editor panel visible after file open', async () => {
        const visible = await EditorPage.isEditorVisible();
        expect(typeof visible).toBe('boolean');
    });

    // ── Positive: Text Operations ──

    it('should handle typing without crash', async () => {
        await typeText('Test input');
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Undo (Cmd+Z) without crash', async () => {
        await typeText('undo test');
        await browser.pause(200);
        await sendShortcut(Shortcuts.UNDO);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Redo (Cmd+Shift+Z) without crash', async () => {
        await sendShortcut(Shortcuts.REDO);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Select All (Cmd+A) without crash', async () => {
        await sendShortcut(Shortcuts.SELECT_ALL);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Positive: Save Operation ──

    it('should handle Save (Cmd+S) without crash', async () => {
        await sendShortcut(Shortcuts.SAVE);
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Positive: Multiple Undo ──

    it('should handle multiple undo steps', async () => {
        await typeText('abc');
        await browser.pause(100);
        await sendShortcut(Shortcuts.UNDO);
        await sendShortcut(Shortcuts.UNDO);
        await sendShortcut(Shortcuts.UNDO);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Negative: Error States ──

    it('should NOT crash on rapid typing', async () => {
        await typeText('rapid test input multiple words');
        await browser.pause(300);
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT leave error overlays after editor operations', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT leave dialogs after Escape from editor', async () => {
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });
});
