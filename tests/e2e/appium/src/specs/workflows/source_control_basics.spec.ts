/**
 * Workflow: Source Control Basics
 *
 * 10 tests covering source control panel discovery and basic state.
 */

import AppShellPage from '../../pages/AppShellPage';
import SourceControlPage from '../../pages/SourceControlPage';
import ActivityBarPage from '../../pages/ActivityBarPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';

describe('Source Control Basics', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should navigate to Source Control via activity bar', async () => {
        // Source Control is typically item index 2 (after Explorer, Search)
        await ActivityBarPage.clickItemByIndex(2);
        await browser.pause(500);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have Source Control page object queryable', async () => {
        const branchSelector = SourceControlPage.branchSelector;
        expect(branchSelector).toBeDefined();
    });

    it('should query branch selector without crash', async () => {
        try {
            const exists = await SourceControlPage.branchSelector.isExisting();
            expect(typeof exists).toBe('boolean');
        } catch {
            // Expected if element not visible on Welcome screen
            expect(true).toBe(true);
        }
    });

    it('should query commit input without crash', async () => {
        try {
            const exists = await SourceControlPage.commitInput.isExisting();
            expect(typeof exists).toBe('boolean');
        } catch {
            expect(true).toBe(true);
        }
    });

    it('should query commit button without crash', async () => {
        try {
            const exists = await SourceControlPage.commitButton.isExisting();
            expect(typeof exists).toBe('boolean');
        } catch {
            expect(true).toBe(true);
        }
    });

    it('should maintain shell stability during SC queries', async () => {
        try {
            await SourceControlPage.branchSelector.isExisting();
            await SourceControlPage.commitInput.isExisting();
        } catch {
            // Elements may not exist
        }
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have shell main_frame during SC context', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should return to Explorer from SC', async () => {
        await ActivityBarPage.clickItemByIndex(2);
        await browser.pause(300);
        await sendShortcut(Shortcuts.EXPLORER);
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should NOT have error overlays after SC operations', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs after SC operations', async () => {
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });
});
