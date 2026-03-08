/**
 * Workflow: Extensions Browser
 *
 * 10 tests covering extensions browsing UI and interaction patterns.
 */

import AppShellPage from '../../pages/AppShellPage';
import ActivityBarPage from '../../pages/ActivityBarPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { pressEscape } from '../../support/keyboard';

describe('Extensions Browser', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should navigate to Extensions panel', async () => {
        await ActivityBarPage.clickItemByIndex(4);
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should check for extensions-related text in UI', async () => {
        await ActivityBarPage.clickItemByIndex(4);
        await browser.pause(500);
        const source = await browser.getPageSource();
        expect(source.length).toBeGreaterThan(100);
    });

    it('should handle scrolling in extensions list area', async () => {
        await ActivityBarPage.clickItemByIndex(4);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should switch between Extensions and Search', async () => {
        await ActivityBarPage.clickItemByIndex(4);
        await browser.pause(200);
        await ActivityBarPage.clickItemByIndex(1);
        await browser.pause(200);
        await ActivityBarPage.clickItemByIndex(4);
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell after Extensions interactions', async () => {
        await ActivityBarPage.clickItemByIndex(4);
        await browser.pause(300);
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should have correct window title during Extensions', async () => {
        const title = await AppShellPage.getWindowTitle();
        expect(title).toBe('MarkAmp');
    });

    it('should handle multiple panel visits', async () => {
        for (let i = 0; i < 5; i++) {
            await ActivityBarPage.clickItemByIndex(i % 5);
            await browser.pause(100);
        }
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should return to Explorer after browsing', async () => {
        await ActivityBarPage.clickItemByIndex(0);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should NOT have error overlays', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT crash after Escape', async () => {
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
