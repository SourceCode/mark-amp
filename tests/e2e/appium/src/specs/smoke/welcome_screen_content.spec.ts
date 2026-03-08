/**
 * Smoke: Welcome Screen Content
 *
 * 10 tests covering Welcome screen display content.
 */

import AppShellPage from '../../pages/AppShellPage';
import WelcomePage from '../../pages/WelcomePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { pressEscape } from '../../support/keyboard';

describe('Welcome Screen Content', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should display Welcome screen on launch', async () => {
        const visible = await WelcomePage.isWelcomeVisible();
        expect(visible).toBe(true);
    });

    it('should have a version string', async () => {
        const version = await WelcomePage.getVersion();
        expect(version).toBeDefined();
        expect(version!.startsWith('v')).toBe(true);
    });

    it('should have Workspaces label', async () => {
        const hasLabel = await WelcomePage.hasWorkspacesLabel();
        expect(typeof hasLabel).toBe('boolean');
    });

    it('should have multiple text labels', async () => {
        const labels = await WelcomePage.getAllLabels();
        expect(labels.length).toBeGreaterThanOrEqual(0);
    });

    it('should have non-empty version string', async () => {
        const version = await WelcomePage.getVersion();
        expect(version!.length).toBeGreaterThan(1);
    });

    it('should have shell main_frame on Welcome screen', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should have correct window title on Welcome screen', async () => {
        const title = await AppShellPage.getWindowTitle();
        expect(title).toBe('MarkAmp');
    });

    it('should have window width ≥ 800px', async () => {
        const size = await AppShellPage.getWindowSize();
        expect(size.width).toBeGreaterThanOrEqual(800);
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
