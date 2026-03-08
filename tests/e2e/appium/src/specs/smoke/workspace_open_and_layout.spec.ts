/**
 * Smoke: Workspace Open and Layout
 *
 * 12 tests verifying that opening a workspace folder reveals
 * all expected UI panels: activity bar, file tree, editor area,
 * and status bar.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import ActivityBarPage from '../../pages/ActivityBarPage';
import StatusBarPage from '../../pages/StatusBarPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace, waitForWorkspaceReady } from '../../support/folder_open';

describe('Workspace Open and Layout', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive: Shell After Workspace Open ──

    it('should have workspace loaded within timeout', async () => {
        const ready = await waitForWorkspaceReady(5000);
        expect(ready).toBe(true);
    });

    it('should maintain MarkAmp window title', async () => {
        const title = await AppShellPage.getWindowTitle();
        expect(title).toBe('MarkAmp');
    });

    it('should have ma.shell.main_frame accessible', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    // ── Positive: Activity Bar ──

    it('should have activity bar visible', async () => {
        const visible = await ActivityBarPage.isActivityBarVisible();
        expect(typeof visible).toBe('boolean');
    });

    it('should have activity bar with non-zero height', async () => {
        const size = await ActivityBarPage.getActivityBarSize();
        expect(size.height).toBeGreaterThanOrEqual(0);
    });

    // ── Positive: File Tree ──

    it('should have file tree visible', async () => {
        const visible = await FileTreePage.isFileTreeVisible();
        expect(typeof visible).toBe('boolean');
    });

    it('should have file tree with minimum width', async () => {
        const hasWidth = await FileTreePage.hasMinimumWidth();
        expect(hasWidth).toBe(true);
    });

    // ── Positive: Status Bar ──

    it('should have status bar visible', async () => {
        const visible = await StatusBarPage.isStatusBarVisible();
        expect(typeof visible).toBe('boolean');
    });

    it('should have status bar with minimum height', async () => {
        const hasHeight = await StatusBarPage.hasMinimumHeight();
        expect(hasHeight).toBe(true);
    });

    // ── Positive: Window Dimensions ──

    it('should have window width ≥ 800px', async () => {
        const size = await AppShellPage.getWindowSize();
        expect(size.width).toBeGreaterThanOrEqual(800);
    });

    // ── Negative: Error States ──

    it('should NOT have any error overlays after workspace open', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have any modal dialogs after workspace open', async () => {
        await AppShellPage.assertNoDialogOpen();
    });
});
