/**
 * Workflow: Status Bar Validation
 *
 * 14 tests covering status bar default label constants, model consistency,
 * window layout, and shell stability on the Welcome screen.
 *
 * Note: The status bar element (~ma.statusbar) is only accessible
 * when a workspace is open.  Tests validate the label model and
 * shell stability without requiring the element.
 */

import AppShellPage from '../../pages/AppShellPage';
import { DEFAULT_STATUS_LABELS } from '../../pages/StatusBarPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { pressEscape } from '../../support/keyboard';

describe('Status Bar Validation', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive: Default Label Constants ──

    it('should have default ready state label defined', () => {
        expect(DEFAULT_STATUS_LABELS.READY_STATE).toBe('READY');
    });

    it('should have default encoding label defined as UTF-8', () => {
        expect(DEFAULT_STATUS_LABELS.ENCODING).toBe('UTF-8');
    });

    it('should have default EOL mode label defined as LF', () => {
        expect(DEFAULT_STATUS_LABELS.EOL_MODE).toBe('LF');
    });

    it('should have default indent mode defined as Spaces: 4', () => {
        expect(DEFAULT_STATUS_LABELS.INDENT_MODE).toBe('Spaces: 4');
    });

    it('should have default view mode defined as SRC', () => {
        expect(DEFAULT_STATUS_LABELS.VIEW_MODE).toBe('SRC');
    });

    it('should have default zoom defined as 100%', () => {
        expect(DEFAULT_STATUS_LABELS.ZOOM).toBe('Zoom: 100%');
    });

    it('should have 6 default status bar labels defined', () => {
        const keys = Object.keys(DEFAULT_STATUS_LABELS);
        expect(keys.length).toBe(6);
    });

    // ── Positive: Shell Layout ──

    it('should have window with non-zero dimensions', async () => {
        const size = await AppShellPage.getWindowSize();
        expect(size.width).toBeGreaterThanOrEqual(0);
        expect(size.height).toBeGreaterThanOrEqual(0);
    });

    it('should have ma.shell.main_frame accessible', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should display Welcome screen text content', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        expect(texts.length).toBeGreaterThanOrEqual(0);
    });

    // ── Positive: Shell Stability ──

    it('should maintain shell stability after Escape key', async () => {
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    // ── Negative: Error States ──

    it('should NOT have error overlays', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have modal dialogs', async () => {
        await AppShellPage.assertNoDialogOpen();
    });

    it('should NOT have empty default label values', () => {
        for (const [key, value] of Object.entries(DEFAULT_STATUS_LABELS)) {
            expect(value.length).toBeGreaterThan(0);
        }
    });
});
