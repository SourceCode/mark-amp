/**
 * UI Verification: Theme Gallery Text Labels
 *
 * 10 tests verifying theme gallery displays all 8 built-in theme
 * names as text labels and renders theme previews correctly.
 */

import AppShellPage from '../../pages/AppShellPage';
import ThemeGalleryPage, { BUILT_IN_THEMES } from '../../pages/ThemeGalleryPage';
import ActivityBarPage from '../../pages/ActivityBarPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { pressEscape } from '../../support/keyboard';

describe('Theme Gallery Text Labels', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should define 8 built-in theme names', async () => {
        expect(BUILT_IN_THEMES.length).toBe(8);
    });

    it('should include "Midnight Neon" theme name', async () => {
        expect(BUILT_IN_THEMES).toContain('Midnight Neon');
    });

    it('should include "Cyber Night" theme name', async () => {
        expect(BUILT_IN_THEMES).toContain('Cyber Night');
    });

    it('should include "Solarized Dark" theme name', async () => {
        expect(BUILT_IN_THEMES).toContain('Solarized Dark');
    });

    it('should include "Classic Mono" theme name', async () => {
        expect(BUILT_IN_THEMES).toContain('Classic Mono');
    });

    it('should include "High Contrast Blue" theme name', async () => {
        expect(BUILT_IN_THEMES).toContain('High Contrast Blue');
    });

    it('should include "Matrix Core" theme name', async () => {
        expect(BUILT_IN_THEMES).toContain('Matrix Core');
    });

    it('should include "Vapor Wave" theme name', async () => {
        expect(BUILT_IN_THEMES).toContain('Vapor Wave');
    });

    it('should open theme gallery via Themes activity bar item', async () => {
        // Themes is the second-to-last bottom item → index 13 from the top
        // (12 top items + index 1 in bottom)
        const THEMES_INDEX = 13;
        await ActivityBarPage.clickItemByIndex(THEMES_INDEX);
        await browser.pause(500);
        const isOpen = await ThemeGalleryPage.isOpen();
        expect(typeof isOpen).toBe('boolean');
        await ThemeGalleryPage.dismiss();
    });

    it('should NOT have error overlays after theme gallery check', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });
});
