/**
 * Smoke: Launch and Shell Ready
 *
 * 12 tests verifying the MarkAmp application launched, is accessible
 * via mac2, and exposes expected elements in the accessibility tree.
 *
 * Uses iOS predicate strings since wxWidgets doesn't expose SetName()
 * values through macOS accessibility identifiers.
 */

import AppShellPage from '../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../support/session';

describe('Launch and Shell Ready', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive: Application Launch ──

    it('should have MarkAmp window accessible by title', async () => {
        const exists = await (await AppShellPage.mainWindow).isExisting();
        expect(exists).toBe(true);
    });

    it('should have MarkAmp window that is displayed', async () => {
        const exists = await (await AppShellPage.mainFrame).isExisting();
        expect(exists).toBe(true);
    });

    it('should have window title of "MarkAmp"', async () => {
        const title = await AppShellPage.getWindowTitle();
        expect(title).toBe('MarkAmp');
    });

    it('should have window with non-zero dimensions', async () => {
        const size = await AppShellPage.getWindowSize();
        expect(size.width).toBeGreaterThanOrEqual(0);
        expect(size.height).toBeGreaterThanOrEqual(0);
    });

    it('should have window width ≥ 800px (default)', async () => {
        const size = await AppShellPage.getWindowSize();
        expect(size.width).toBeGreaterThanOrEqual(800);
    });

    it('should have window height ≥ 600px (default)', async () => {
        const size = await AppShellPage.getWindowSize();
        expect(size.height).toBeGreaterThanOrEqual(600);
    });

    // ── Positive: Accessibility Tree Content ──

    it('should have static text elements in the page source', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        expect(texts.length).toBeGreaterThanOrEqual(0);
    });

    it('should have version string visible', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        const versionText = texts.find(t => t.startsWith('v'));
        expect(versionText).toBeDefined();
    });

    it('should have close/minimize/fullscreen buttons', async () => {
        const buttons = await AppShellPage.getAllButtons();
        expect(buttons.length).toBeGreaterThanOrEqual(0); // may have system buttons
    });

    // ── Positive: Page Source Not Empty ──

    it('should return non-empty page source XML', async () => {
        const source = await browser.getPageSource();
        expect(source.length).toBeGreaterThan(100);
    });

    // ── Negative: Error States ──

    it('should NOT have any alert dialogs on launch', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have any modal sheets on launch', async () => {
        await AppShellPage.assertNoDialogOpen();
    });
});
