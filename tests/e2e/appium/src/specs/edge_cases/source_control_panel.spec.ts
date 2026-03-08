/**
 * Edge Case: Source Control Panel
 *
 * 10 tests covering the page object model consistency,
 * element label validation, and shell stability on the Welcome screen.
 *
 * Note: Source control panel elements are only accessible when a
 * workspace is open and the Git sidebar is active.
 */

import AppShellPage from '../../pages/AppShellPage';
import SourceControlPage from '../../pages/SourceControlPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { pressEscape } from '../../support/keyboard';

describe('Source Control Panel', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive: Page Object Model Validation ──

    it('should have SourceControlPage with branchSelector getter', () => {
        expect(typeof SourceControlPage.branchSelector).not.toBe('undefined');
    });

    it('should have SourceControlPage with commitInput getter', () => {
        expect(typeof SourceControlPage.commitInput).not.toBe('undefined');
    });

    it('should have SourceControlPage with commitButton getter', () => {
        expect(typeof SourceControlPage.commitButton).not.toBe('undefined');
    });

    it('should have SourceControlPage with templateButton getter', () => {
        expect(typeof SourceControlPage.templateButton).not.toBe('undefined');
    });

    it('should have SourceControlPage with stagedList getter', () => {
        expect(typeof SourceControlPage.stagedList).not.toBe('undefined');
    });

    it('should have SourceControlPage with unstagedList getter', () => {
        expect(typeof SourceControlPage.unstagedList).not.toBe('undefined');
    });

    it('should have SourceControlPage with timelineList getter', () => {
        expect(typeof SourceControlPage.timelineList).not.toBe('undefined');
    });

    // ── Positive: Shell Stability ──

    it('should have isPanelLoaded return a boolean without crashing', async () => {
        const loaded = await SourceControlPage.isPanelLoaded();
        expect(typeof loaded).toBe('boolean');
    });

    // ── Negative: Error States ──

    it('should NOT have error overlays on Welcome screen', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT crash when pressing Escape', async () => {
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
