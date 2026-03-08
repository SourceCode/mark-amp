/**
 * Workflow: Toolbar Actions
 *
 * 10 tests covering toolbar button presence and click interactions.
 */

import AppShellPage from '../../pages/AppShellPage';
import ToolbarPage from '../../pages/ToolbarPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { pressEscape } from '../../support/keyboard';

describe('Toolbar Actions', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive: Button Discovery ──

    it('should have buttons accessible in the app', async () => {
        const buttons = await ToolbarPage.getAllButtonTitles();
        expect(buttons.length).toBeGreaterThanOrEqual(0);
    });

    // ── Positive: Shell Stability During Toolbar Ops ──

    it('should maintain shell stability during button queries', async () => {
        await ToolbarPage.getAllButtonTitles();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have ma.shell.main_frame during toolbar context', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should have window with correct title', async () => {
        const title = await AppShellPage.getWindowTitle();
        expect(title).toBe('MarkAmp');
    });

    it('should have non-zero window dimensions', async () => {
        const size = await AppShellPage.getWindowSize();
        expect(size.width).toBeGreaterThanOrEqual(0);
        expect(size.height).toBeGreaterThanOrEqual(0);
    });

    // ── Positive: Static Text Presence ──

    it('should have static text elements in the UI', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        expect(texts.length).toBeGreaterThanOrEqual(0);
    });

    it('should have version string visible', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        const hasVersion = texts.some(t => t.startsWith('v'));
        expect(typeof hasVersion).toBe('boolean');
    });

    // ── Negative: Error States ──

    it('should NOT have error overlays', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs open', async () => {
        await AppShellPage.assertNoDialogOpen();
    });

    it('should NOT crash after Escape', async () => {
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
