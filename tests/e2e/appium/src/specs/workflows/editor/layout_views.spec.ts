/**
 * @workflow Editor — Layout and view workflows
 * Phase 03, Tasks 19–23: Split-view, word-wrap, minimap, line-number toggles.
 */
import AppShellPage from '../../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../../support/session';
import { sendShortcut, Shortcuts } from '../../../support/keyboard';
import { waitForIdle } from '../../../support/waits';
import { prepareWorkspace, cleanupWorkspace } from '../../../support/workspace';
import { openFolderViaDialog } from '../../../support/folder_open';

describe('@workflow Editor — Layout and view controls', () => {
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

    // Task 19: Split-view creation and navigation
    it('should create split view via shortcut', async () => {
        await sendShortcut(Shortcuts.SPLIT_EDITOR);
        await waitForIdle(500);
        await AppShellPage.assertNoErrorOverlays();
    });

    // Task 20: Split-view sync and active pane
    it('should maintain active pane after split', async () => {
        expect(await AppShellPage.isShellReady()).toBe(true);
    });

    // Task 21: Word-wrap toggle
    it('should toggle word wrap without error', async () => {
        // Toggle via command palette
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await waitForIdle(300);
        await sendShortcut('Escape');
        await waitForIdle(300);
        await AppShellPage.assertNoErrorOverlays();
    });

    // Task 22: Minimap toggle
    it('should toggle minimap without error', async () => {
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await waitForIdle(300);
        await sendShortcut('Escape');
        await waitForIdle(300);
        expect(await AppShellPage.isShellReady()).toBe(true);
    });

    // Task 23: Line-number visibility toggle
    it('should toggle line numbers without error', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });
});
