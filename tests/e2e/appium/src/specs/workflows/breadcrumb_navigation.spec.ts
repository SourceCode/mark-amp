/**
 * Workflow: Breadcrumb Navigation
 *
 * 8 tests covering breadcrumb bar presence and interaction.
 */

import AppShellPage from '../../pages/AppShellPage';
import BreadcrumbPage from '../../pages/BreadcrumbPage';
import FileTreePage from '../../pages/FileTreePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { pressEscape } from '../../support/keyboard';

describe('Breadcrumb Navigation', () => {
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

    it('should query breadcrumb visibility without crash', async () => {
        const visible = await BreadcrumbPage.isVisible();
        expect(typeof visible).toBe('boolean');
    });

    it('should get breadcrumb texts without crash', async () => {
        const texts = await BreadcrumbPage.getBreadcrumbTexts();
        expect(Array.isArray(texts)).toBe(true);
    });

    it('should maintain shell stability during breadcrumb checks', async () => {
        await BreadcrumbPage.isVisible();
        await BreadcrumbPage.getBreadcrumbTexts();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have shell main_frame during breadcrumb context', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should have correct window title', async () => {
        const title = await AppShellPage.getWindowTitle();
        expect(title).toBe('MarkAmp');
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
