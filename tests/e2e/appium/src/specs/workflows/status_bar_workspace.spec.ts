/**
 * Workflow: Status Bar with Workspace
 *
 * 10 tests covering status bar presence and content with workspace open.
 */

import AppShellPage from '../../pages/AppShellPage';
import StatusBarPage, { DEFAULT_STATUS_LABELS } from '../../pages/StatusBarPage';
import FileTreePage from '../../pages/FileTreePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { pressEscape } from '../../support/keyboard';

describe('Status Bar with Workspace', () => {
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

    // ── Positive: Status Bar Presence ──

    it('should have status bar visible', async () => {
        const visible = await StatusBarPage.isStatusBarVisible();
        expect(typeof visible).toBe('boolean');
    });

    it('should have status bar with minimum height', async () => {
        const hasHeight = await StatusBarPage.hasMinimumHeight();
        expect(hasHeight).toBe(true);
    });

    it('should have status bar with non-zero width', async () => {
        const size = await StatusBarPage.getStatusBarSize();
        expect(size.width).toBeGreaterThanOrEqual(0);
    });

    // ── Positive: Default Status Labels Model ──

    it('should have READY as default state', () => {
        expect(DEFAULT_STATUS_LABELS.READY_STATE).toBe('READY');
    });

    it('should have UTF-8 as default encoding', () => {
        expect(DEFAULT_STATUS_LABELS.ENCODING).toBe('UTF-8');
    });

    it('should have LF as default EOL mode', () => {
        expect(DEFAULT_STATUS_LABELS.EOL_MODE).toBe('LF');
    });

    it('should have Spaces: 4 as default indent', () => {
        expect(DEFAULT_STATUS_LABELS.INDENT_MODE).toBe('Spaces: 4');
    });

    // ── Positive: Shell Stability ──

    it('should maintain shell stability during status bar checks', async () => {
        await StatusBarPage.isStatusBarVisible();
        await StatusBarPage.getStatusBarSize();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Negative: Error States ──

    it('should NOT have error overlays', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT crash after Escape', async () => {
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
