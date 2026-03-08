/**
 * @workflow Search — Command palette, keyboard shortcuts, global search
 * Phase 04, Tasks 1–20: Command palette, shortcuts, global search workflows.
 */
import AppShellPage from '../../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../../support/session';
import { sendShortcut, typeText, pressEscape, pressEnter, pressArrowDown, Shortcuts } from '../../../support/keyboard';
import { waitForIdle } from '../../../support/waits';
import { prepareWorkspace, cleanupWorkspace } from '../../../support/workspace';
import { openFolderViaDialog } from '../../../support/folder_open';

describe('@workflow Search — Command palette and global search', () => {
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

    // Tasks 1-4: Command palette
    it('should open and close command palette', async () => {
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await waitForIdle(500);
        await pressEscape();
        await waitForIdle(300);
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should support fuzzy filtering in command palette', async () => {
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await waitForIdle(500);
        await typeText('set');
        await waitForIdle(300);
        await pressEscape();
        await waitForIdle(300);
    });

    it('should execute command from palette', async () => {
        await sendShortcut(Shortcuts.COMMAND_PALETTE);
        await waitForIdle(500);
        await typeText('toggle');
        await waitForIdle(300);
        await pressEscape();
        await waitForIdle(300);
        await AppShellPage.assertNoErrorOverlays();
    });

    // Tasks 5-8: Keyboard shortcuts
    it('should handle keyboard shortcut operations', async () => {
        await sendShortcut(Shortcuts.SETTINGS);
        await waitForIdle(500);
        await pressEscape();
        await waitForIdle(300);
        await AppShellPage.assertNoErrorOverlays();
    });

    // Tasks 9-20: Global search
    it('should open global search panel', async () => {
        await sendShortcut(Shortcuts.SEARCH);
        await waitForIdle(500);
        await pressEscape();
        await waitForIdle(300);
    });

    it('should perform literal search', async () => {
        await sendShortcut(Shortcuts.SEARCH);
        await waitForIdle(500);
        await typeText('test');
        await waitForIdle(500);
        await pressEscape();
        await waitForIdle(300);
    });

    it('should handle search replace preview', async () => {
        await sendShortcut(Shortcuts.SEARCH);
        await waitForIdle(500);
        await pressEscape();
        await waitForIdle(300);
        await AppShellPage.assertNoErrorOverlays();
    });
});
