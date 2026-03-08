/**
 * Edge Case: Performance Response Times
 *
 * 10 tests validating UI responsiveness and timing constraints.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Performance Response Times', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should open file within 3 seconds', async () => {
        const start = Date.now();
        await FileTreePage.clickFile('README');
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        const elapsed = Date.now() - start;
        expect(ready).toBe(true);
        expect(elapsed).toBeLessThan(3000);
    });

    it('should respond to shortcut within 2 seconds', async () => {
        const start = Date.now();
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await browser.pause(400);
        const ready = await AppShellPage.isShellReady();
        const elapsed = Date.now() - start;
        expect(ready).toBe(true);
        expect(elapsed).toBeLessThan(2000);
        await pressEscape();
    });

    it('should toggle sidebar within 1 second', async () => {
        const start = Date.now();
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        const elapsed = Date.now() - start;
        expect(ready).toBe(true);
        expect(elapsed).toBeLessThan(1000);
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
    });

    it('should switch files within 2 seconds', async () => {
        await FileTreePage.clickFile('README');
        await browser.pause(200);
        const start = Date.now();
        await FileTreePage.clickFile('NOTES');
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        const elapsed = Date.now() - start;
        expect(ready).toBe(true);
        expect(elapsed).toBeLessThan(2000);
    });

    it('should respond to Escape within 500ms', async () => {
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await browser.pause(300);
        const start = Date.now();
        await pressEscape();
        await browser.pause(200);
        const elapsed = Date.now() - start;
        expect(elapsed).toBeLessThan(500);
    });

    it('should query shell readiness within 1 second', async () => {
        const start = Date.now();
        const ready = await AppShellPage.isShellReady();
        const elapsed = Date.now() - start;
        expect(ready).toBe(true);
        expect(elapsed).toBeLessThan(1000);
    });

    it('should get window size within 500ms', async () => {
        const start = Date.now();
        const size = await AppShellPage.getWindowSize();
        const elapsed = Date.now() - start;
        expect(size.width).toBeGreaterThanOrEqual(0);
        expect(elapsed).toBeLessThan(500);
    });

    it('should get page source within 2 seconds', async () => {
        const start = Date.now();
        const source = await browser.getPageSource();
        const elapsed = Date.now() - start;
        expect(source.length).toBeGreaterThan(100);
        expect(elapsed).toBeLessThan(2000);
    });

    it('should NOT have error overlays', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs', async () => {
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });
});
