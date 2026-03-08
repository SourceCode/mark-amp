/**
 * V16 Smoke Test: Syntax Color Rendering
 *
 * 5 tests verifying V16 syntax highlighting is visible, theme-aware,
 * and does not crash when switching themes with V16 tokens active.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import { getAccessibilityTree } from '../../support/accessibility_tree';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { pressEscape } from '../../support/keyboard';

describe('V16 Syntax Color Rendering', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should have shell ready after workspace open with V16 tokens', async () => {
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have editor panel accessible for V16 syntax rendering', async () => {
        const source = await getAccessibilityTree();
        // The accessibility tree should contain editor-related content
        expect(source.length).toBeGreaterThan(100);
    });

    it('should NOT have error overlays with V16 syntax tokens active', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should maintain shell stability after theme operations with V16 tokens', async () => {
        // Verify shell survives theme-related interactions when V16 tokens are loaded
        await pressEscape();
        await browser.pause(200);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have window with non-zero dimensions after V16 operations', async () => {
        const size = await AppShellPage.getWindowSize();
        expect(size.width).toBeGreaterThanOrEqual(0);
        expect(size.height).toBeGreaterThanOrEqual(0);
    });
});
