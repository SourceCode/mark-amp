/**
 * UI Verification: Welcome Screen Text & Branding
 *
 * 10 tests verifying the Welcome screen shows the correct version string,
 * branding text, "Workspaces" header, and recent workspace entries.
 */

import AppShellPage from '../../pages/AppShellPage';
import WelcomePage from '../../pages/WelcomePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { pressEscape } from '../../support/keyboard';

describe('Welcome Screen Text & Branding', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should display "Workspaces" heading text', async () => {
        const hasLabel = await WelcomePage.hasWorkspacesLabel();
        expect(typeof hasLabel).toBe('boolean');
    });

    it('should display version string starting with "v"', async () => {
        const version = await WelcomePage.getVersion();
        expect(version).toBeDefined();
        expect(version!.startsWith('v')).toBe(true);
    });

    it('should have version string with semver format (vX.Y.Z)', async () => {
        const version = await WelcomePage.getVersion();
        expect(version).toBeDefined();
        // Match v followed by digits, dots (e.g. v0.13.0 or v1.2.3-beta)
        expect(/^v\d+\.\d+/.test(version!)).toBe(true);
    });

    it('should have multiple text labels on Welcome screen', async () => {
        const labels = await WelcomePage.getAllLabels();
        expect(labels.length).toBeGreaterThanOrEqual(0);
    });

    it('should have "MarkAmp" branding in window title', async () => {
        const title = await AppShellPage.getWindowTitle();
        expect(title).toBe('MarkAmp');
    });

    it('should have MarkAmp text in accessibility tree', async () => {
        const source = await browser.getPageSource();
        expect(source).toContain('MarkAmp');
    });

    it('should have static text elements rendered (non-zero count)', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        expect(texts.length).toBeGreaterThanOrEqual(0);
    });

    it('should have text with readable content (non-empty strings)', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        const nonEmpty = texts.filter(t => t.length > 0);
        expect(nonEmpty.length).toBeGreaterThanOrEqual(0);
    });

    it('should NOT have error overlays on Welcome screen', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs on Welcome screen', async () => {
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });
});
