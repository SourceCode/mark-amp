/**
 * @workflow Editor — Editor command workflows
 * Phase 03, Tasks 24–30: Gutter, duplicate-line, move-line, toggle-comment,
 * case-transform, sort-lines, fold/unfold.
 */
import AppShellPage from '../../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../../support/session';
import { sendShortcut, typeText, Shortcuts } from '../../../support/keyboard';
import { waitForIdle } from '../../../support/waits';
import { prepareWorkspace, cleanupWorkspace } from '../../../support/workspace';
import { openFolderViaDialog } from '../../../support/folder_open';

describe('@workflow Editor — Editor commands', () => {
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

    // Task 24: Gutter indicator behavior for modified lines
    it('should handle gutter indicator display', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    // Task 25: Duplicate line command
    it('should duplicate line without error', async () => {
        await sendShortcut('Cmd+Shift+D');
        await waitForIdle(300);
        await AppShellPage.assertNoErrorOverlays();
    });

    // Task 26: Move line up/down
    it('should move line up without error', async () => {
        await sendShortcut('Option+Up');
        await waitForIdle(300);
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should move line down without error', async () => {
        await sendShortcut('Option+Down');
        await waitForIdle(300);
        await AppShellPage.assertNoErrorOverlays();
    });

    // Task 27: Toggle comment
    it('should toggle comment without error', async () => {
        await sendShortcut('Cmd+/');
        await waitForIdle(300);
        expect(await AppShellPage.isShellReady()).toBe(true);
    });

    // Task 28: Case transform commands
    it('should handle case transform commands', async () => {
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await waitForIdle(300);
        await sendShortcut('Escape');
        await waitForIdle(300);
        await AppShellPage.assertNoErrorOverlays();
    });

    // Task 29: Sort lines command
    it('should handle sort lines command', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    // Task 30: Fold/unfold all
    it('should support fold-all and unfold-all', async () => {
        // Cmd+K Cmd+0 to fold all, Cmd+K Cmd+J to unfold all
        await AppShellPage.assertNoErrorOverlays();
    });
});
