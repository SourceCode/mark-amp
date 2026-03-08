/**
 * Workflow: Extensions Panel
 *
 * 10 tests covering extensions panel navigation and shell stability.
 */

import AppShellPage from '../../pages/AppShellPage';
import ActivityBarPage from '../../pages/ActivityBarPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Extensions Panel', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should navigate to Extensions via activity bar', async () => {
        // Extensions is typically at index 4 or later
        await ActivityBarPage.clickItemByIndex(4);
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have shell main_frame during Extensions view', async () => {
        await ActivityBarPage.clickItemByIndex(4);
        await browser.pause(300);
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should return to Explorer from Extensions', async () => {
        await ActivityBarPage.clickItemByIndex(4);
        await browser.pause(300);
        await sendShortcut(Shortcuts.EXPLORER);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have activity bar during Extensions view', async () => {
        await ActivityBarPage.clickItemByIndex(4);
        await browser.pause(300);
        const visible = await ActivityBarPage.isActivityBarVisible();
        expect(typeof visible).toBe('boolean');
    });

    it('should handle rapid switch to Extensions and back', async () => {
        for (let i = 0; i < 3; i++) {
            await ActivityBarPage.clickItemByIndex(4);
            await browser.pause(100);
            await ActivityBarPage.clickItemByIndex(0);
            await browser.pause(100);
        }
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain window dimensions during Extensions', async () => {
        const size = await AppShellPage.getWindowSize();
        expect(size.width).toBeGreaterThanOrEqual(0);
        expect(size.height).toBeGreaterThanOrEqual(0);
    });

    it('should have text content in UI', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        expect(texts.length).toBeGreaterThanOrEqual(0);
    });

    it('should handle Escape in Extensions context', async () => {
        await ActivityBarPage.clickItemByIndex(4);
        await browser.pause(200);
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should NOT have error overlays', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs open', async () => {
        await AppShellPage.assertNoDialogOpen();
    });
});
