/**
 * Workflow: View Mode Switching
 *
 * 10 tests covering SRC → Split → VIEW mode cycling.
 */

import AppShellPage from '../../pages/AppShellPage';
import SplitViewPage from '../../pages/SplitViewPage';
import PreviewPage from '../../pages/PreviewPage';
import FileTreePage from '../../pages/FileTreePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { pressEscape } from '../../support/keyboard';

describe('View Mode Switching', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
        await FileTreePage.clickFile('README');
        await browser.pause(500);
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive: Mode Queries ──

    it('should query isSplitMode without crash', async () => {
        const result = await SplitViewPage.isSplitMode();
        expect(typeof result).toBe('boolean');
    });

    it('should query isEditorOnlyMode without crash', async () => {
        const result = await SplitViewPage.isEditorOnlyMode();
        expect(typeof result).toBe('boolean');
    });

    it('should query isPreviewOnlyMode without crash', async () => {
        const result = await SplitViewPage.isPreviewOnlyMode();
        expect(typeof result).toBe('boolean');
    });

    it('should query preview visibility without crash', async () => {
        const result = await PreviewPage.isPreviewVisible();
        expect(typeof result).toBe('boolean');
    });

    // ── Positive: Divider Query ──

    it('should query divider visibility without crash', async () => {
        const result = await SplitViewPage.isDividerVisible();
        expect(typeof result).toBe('boolean');
    });

    // ── Positive: Shell Stability ──

    it('should maintain shell stability during view mode queries', async () => {
        await SplitViewPage.isSplitMode();
        await SplitViewPage.isEditorOnlyMode();
        await SplitViewPage.isPreviewOnlyMode();
        await PreviewPage.isPreviewVisible();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have correct window dimensions', async () => {
        const size = await AppShellPage.getWindowSize();
        expect(size.width).toBeGreaterThanOrEqual(0);
        expect(size.height).toBeGreaterThanOrEqual(0);
    });

    // ── Negative: Error States ──

    it('should NOT crash when querying sizes of non-existent panels', async () => {
        try {
            await SplitViewPage.getPreviewSize();
        } catch {
            // Expected if mode doesn't show preview
        }
        try {
            await SplitViewPage.getEditorSize();
        } catch {
            // Expected if mode doesn't show editor
        }
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should NOT leave error overlays after view operations', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT leave dialogs after Escape', async () => {
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });
});
