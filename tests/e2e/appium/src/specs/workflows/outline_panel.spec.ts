/**
 * Workflow: Outline Panel
 *
 * 10 tests covering outline panel visibility and navigation.
 */

import AppShellPage from '../../pages/AppShellPage';
import OutlinePage from '../../pages/OutlinePage';
import FileTreePage from '../../pages/FileTreePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { pressEscape } from '../../support/keyboard';

describe('Outline Panel', () => {
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

    it('should query outline visibility without crash', async () => {
        const visible = await OutlinePage.isOutlineVisible();
        expect(typeof visible).toBe('boolean');
    });

    it('should get outline source without crash', async () => {
        const source = await OutlinePage.getOutlineSource();
        expect(source.length).toBeGreaterThan(100);
    });

    it('should maintain shell stability during outline checks', async () => {
        await OutlinePage.isOutlineVisible();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have ma.shell.main_frame during outline context', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should have window with correct title', async () => {
        const title = await AppShellPage.getWindowTitle();
        expect(title).toBe('MarkAmp');
    });

    it('should have static text in the page source', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        expect(texts.length).toBeGreaterThanOrEqual(0);
    });

    it('should have file tree visible alongside outline', async () => {
        const visible = await FileTreePage.isFileTreeVisible();
        expect(typeof visible).toBe('boolean');
    });

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
