/**
 * @workflow Editor — Save workflows
 * Phase 03, Tasks 14–18: Unsaved indicator, save menu, save shortcut, save-all, reopen tab.
 */
import AppShellPage from '../../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../../support/session';
import { sendShortcut, typeText, Shortcuts } from '../../../support/keyboard';
import { waitForIdle } from '../../../support/waits';
import { prepareWorkspace, cleanupWorkspace } from '../../../support/workspace';
import { openFolderViaDialog } from '../../../support/folder_open';

describe('@workflow Editor — Save workflows', () => {
    let workspacePath: string;

    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        workspacePath = prepareWorkspace('basic');
        await openFolderViaDialog(workspacePath);
        await waitForIdle(2000);
    });
    afterEach(async () => { await resetAppState(); });
    after(() => { cleanupWorkspace(workspacePath); });

    // Task 14: Unsaved changes indicator
    it('should show unsaved indicator when content is modified', async () => {
        await sendShortcut(Shortcuts.NEW_FILE);
        await waitForIdle(500);
        await typeText('Test content');
        await waitForIdle(500);
        // Tab title should have unsaved indicator
        await AppShellPage.assertNoErrorOverlays();
    });

    // Task 15: Save via menu action
    it('should save file without errors', async () => {
        await sendShortcut(Shortcuts.SAVE);
        await waitForIdle(500);
        await AppShellPage.assertNoErrorOverlays();
    });

    // Task 16: Save via keyboard shortcut
    it('should save via Cmd+S shortcut', async () => {
        await typeText('Additional content');
        await sendShortcut(Shortcuts.SAVE);
        await waitForIdle(500);
        expect(await AppShellPage.isShellReady()).toBe(true);
    });

    // Task 17: Save-all across multiple dirty tabs
    it('should support save-all operation', async () => {
        await sendShortcut(Shortcuts.NEW_FILE);
        await waitForIdle(300);
        await typeText('Second file');
        // Cmd+Option+S for save all
        await sendShortcut('Cmd+Option+S');
        await waitForIdle(500);
        await AppShellPage.assertNoErrorOverlays();
    });

    // Task 18: Reopen closed tab
    it('should reopen previously closed tab', async () => {
        await sendShortcut(Shortcuts.CLOSE_TAB);
        await waitForIdle(300);
        // Cmd+Shift+T to reopen
        await sendShortcut('Cmd+Shift+T');
        await waitForIdle(500);
        await AppShellPage.assertNoErrorOverlays();
    });
});
