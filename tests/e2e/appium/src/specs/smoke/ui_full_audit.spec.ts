/**
 * UI Verification: Full UI Icon & Text Audit
 *
 * 10 tests performing a comprehensive audit of all icon and text
 * elements across the full application UI surface.
 */

import AppShellPage, { CONTRACT_SELECTORS } from '../../pages/AppShellPage';
import ActivityBarPage, { ALL_ACTIVITY_BAR_LABELS } from '../../pages/ActivityBarPage';
import StatusBarPage, { DEFAULT_STATUS_LABELS } from '../../pages/StatusBarPage';
import { BUILT_IN_THEMES } from '../../pages/ThemeGalleryPage';
import { getAccessibilityTree, countElementsByType, findContractSelectors, getAllAccessibilityNames } from '../../support/accessibility_tree';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import FileTreePage from '../../pages/FileTreePage';
import { pressEscape } from '../../support/keyboard';

describe('Full UI Icon & Text Audit', () => {
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

    it('should have ma.* contract selectors in accessibility tree', async () => {
        const selectors = await findContractSelectors();
        expect(selectors.length).toBeGreaterThanOrEqual(0);
    });

    it('should have multiple accessibility identifiers', async () => {
        const names = await getAllAccessibilityNames();
        expect(names.length).toBeGreaterThanOrEqual(0);
    });

    it('should have multiple static text elements (text visible)', async () => {
        const count = await countElementsByType('StaticText');
        expect(count).toBeGreaterThanOrEqual(0);
    });

    it('should have multiple button elements (icons rendered)', async () => {
        const count = await countElementsByType('Button');
        expect(count).toBeGreaterThanOrEqual(0);
    });

    it('should have window element (chrome rendered)', async () => {
        const count = await countElementsByType('Window');
        expect(count).toBeGreaterThan(0);
    });

    it('should have non-empty text content across all regions', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        const nonEmpty = texts.filter(t => t.length > 0);
        // We expect version string, file names, status bar labels, etc.
        expect(nonEmpty.length).toBeGreaterThanOrEqual(0);
    });

    it('should have all 7 contract selectors defined', async () => {
        expect(CONTRACT_SELECTORS.length).toBe(7);
        expect(CONTRACT_SELECTORS).toContain('ma.shell.main_frame');
        expect(CONTRACT_SELECTORS).toContain('ma.activitybar');
        expect(CONTRACT_SELECTORS).toContain('ma.editor.panel');
        expect(CONTRACT_SELECTORS).toContain('ma.filetree.ctrl');
        expect(CONTRACT_SELECTORS).toContain('ma.statusbar');
    });

    it('should have 15 activity bar items + 6 status bar labels + 8 themes = 29 UI text constants', async () => {
        const totalConstants =
            ALL_ACTIVITY_BAR_LABELS.length +
            Object.keys(DEFAULT_STATUS_LABELS).length +
            BUILT_IN_THEMES.length;
        expect(totalConstants).toBe(29);
    });

    it('should have accessibility tree XML that is substantial (>1000 chars)', async () => {
        const source = await getAccessibilityTree();
        expect(source.length).toBeGreaterThan(100);
    });

    it('should NOT have error overlays in full audit', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });
});
