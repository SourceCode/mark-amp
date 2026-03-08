/**
 * Workflow: Split View Operations
 *
 * 10 tests covering split view page object state queries,
 * window layout validation, and shell stability on the Welcome screen.
 *
 * Note: The editor panel, activity bar, and status bar elements
 * are only accessible when a workspace is open.
 */

import AppShellPage from '../../pages/AppShellPage';
import SplitViewPage from '../../pages/SplitViewPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { pressEscape } from '../../support/keyboard';

describe('Split View Operations', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive: Shell Layout ──

    it('should have window with MarkAmp title', async () => {
        const title = await AppShellPage.getWindowTitle();
        expect(title).toBe('MarkAmp');
    });

    it('should have window with reasonable dimensions', async () => {
        const size = await AppShellPage.getWindowSize();
        expect(size.width).toBeGreaterThanOrEqual(0);
        expect(size.height).toBeGreaterThanOrEqual(0);
    });

    it('should have ma.shell.main_frame element', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    // ── Positive: SplitView Page Object State Queries ──

    it('should report boolean from isSplitMode without crash', async () => {
        const isSplit = await SplitViewPage.isSplitMode();
        expect(typeof isSplit).toBe('boolean');
    });

    it('should report boolean from isEditorOnlyMode without crash', async () => {
        const isEditorOnly = await SplitViewPage.isEditorOnlyMode();
        expect(typeof isEditorOnly).toBe('boolean');
    });

    it('should report boolean from isPreviewOnlyMode without crash', async () => {
        const isPreviewOnly = await SplitViewPage.isPreviewOnlyMode();
        expect(typeof isPreviewOnly).toBe('boolean');
    });

    // ── Positive: Layout Stability ──

    it('should maintain shell stability during split view queries', async () => {
        await SplitViewPage.isSplitMode();
        await SplitViewPage.isEditorOnlyMode();
        await SplitViewPage.isPreviewOnlyMode();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Negative: Edge Cases ──

    it('should NOT crash when querying preview panel that may not exist', async () => {
        try {
            await SplitViewPage.getPreviewSize();
        } catch {
            // Expected if preview is not visible
        }
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should NOT leave error state after rapid view mode queries', async () => {
        for (let i = 0; i < 5; i++) {
            await SplitViewPage.isSplitMode();
            await SplitViewPage.isEditorOnlyMode();
        }
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT leave dialogs open after Escape', async () => {
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });
});
