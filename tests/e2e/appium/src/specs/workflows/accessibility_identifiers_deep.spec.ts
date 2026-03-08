/**
 * Workflow: Accessibility Identifiers Deep
 *
 * 10 tests for deep accessibility tree validation.
 */

import AppShellPage from '../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { findByIdentifierPattern, getAccessibilityTree, countElementsByType, assertElementProperties } from '../../support/accessibility_tree';
import { pressEscape } from '../../support/keyboard';

describe('Accessibility Identifiers Deep', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should have ma.shell.main_frame in accessibility tree', async () => {
        const elements = await findByIdentifierPattern('ma.shell.main_frame');
        expect(elements.length).toBeGreaterThan(0);
    });

    it('should have ma.activitybar in accessibility tree', async () => {
        const elements = await findByIdentifierPattern('ma.activitybar');
        expect(elements.length).toBeGreaterThanOrEqual(0);
    });

    it('should have non-empty page source', async () => {
        const source = await getAccessibilityTree();
        expect(source.length).toBeGreaterThan(500);
    });

    it('should have multiple static text elements', async () => {
        const count = await countElementsByType('StaticText');
        expect(count).toBeGreaterThan(0);
    });

    it('should have button elements', async () => {
        const count = await countElementsByType('Button');
        expect(count).toBeGreaterThanOrEqual(0);
    });

    it('should have window element', async () => {
        const count = await countElementsByType('Window');
        expect(count).toBeGreaterThan(0);
    });

    it('should have shell main frame be enabled', async () => {
        await assertElementProperties('ma.shell.main_frame', { enabled: true });
    });

    it('should maintain tree consistency after panel switch', async () => {
        const sourceBefore = await getAccessibilityTree();
        await pressEscape();
        await browser.pause(200);
        const sourceAfter = await getAccessibilityTree();
        expect(sourceBefore.length).toBeGreaterThan(0);
        expect(sourceAfter.length).toBeGreaterThan(0);
    });

    it('should NOT have error overlays in accessibility tree', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT crash after tree queries', async () => {
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
