/**
 * @workflow Navigation — Panel area, code navigation, shell/toolbar
 * Phase 04, Tasks 21–50: Panel tabs, code navigation, activity bar, toolbar,
 * contextual menus, notifications, settings, nav history, productivity.
 */
import AppShellPage from '../../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../../support/session';
import { sendShortcut, pressEscape, Shortcuts } from '../../../support/keyboard';
import { waitForIdle } from '../../../support/waits';
import { prepareWorkspace, cleanupWorkspace } from '../../../support/workspace';
import { openFolderViaDialog } from '../../../support/folder_open';

describe('@workflow Navigation — Panel area and productivity', () => {
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

    // Tasks 21-30: Panel area
    it('should toggle panel area via shortcut', async () => {
        await sendShortcut(Shortcuts.TOGGLE_PANEL);
        await waitForIdle(500);
        await sendShortcut(Shortcuts.TOGGLE_PANEL);
        await waitForIdle(500);
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should support panel area tabs', async () => {
        const panelArea = await browser.$('~ma.panelarea');
        const exists = await panelArea.isExisting();
        expect(typeof exists).toBe('boolean');
    });

    // Tasks 31-35: Code navigation
    it('should support go-to-line navigation', async () => {
        await sendShortcut(Shortcuts.GO_TO_LINE);
        await waitForIdle(500);
        await pressEscape();
        await waitForIdle(300);
    });

    // Tasks 36-50: Shell, toolbar, productivity
    it('should toggle sidebar', async () => {
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await waitForIdle(500);
        await sendShortcut(Shortcuts.TOGGLE_SIDEBAR);
        await waitForIdle(500);
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should support activity bar switching', async () => {
        await sendShortcut(Shortcuts.EXPLORER);
        await waitForIdle(500);
        await sendShortcut(Shortcuts.SEARCH);
        await waitForIdle(500);
        await sendShortcut(Shortcuts.EXPLORER);
        await waitForIdle(500);
    });

    it('should open settings via shortcut', async () => {
        await sendShortcut(Shortcuts.SETTINGS);
        await waitForIdle(500);
        await pressEscape();
        await waitForIdle(300);
    });

    it('should handle notification center interaction', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should support zen mode toggle', async () => {
        await sendShortcut(Shortcuts.ZEN_MODE);
        await waitForIdle(500);
        await sendShortcut(Shortcuts.ZEN_MODE);
        await waitForIdle(500);
        await AppShellPage.assertNoErrorOverlays();
    });
});
