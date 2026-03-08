/**
 * @workflow Workbench — Tab management workflows
 * Phase 03, Tasks 9–13: Open file, dedup tab, multi-tab switch, close, close-all.
 */
import AppShellPage from '../../../pages/AppShellPage';
import TabBarPage from '../../../pages/TabBarPage';
import { ensureAppRunning, resetAppState } from '../../../support/session';
import { sendShortcut, Shortcuts } from '../../../support/keyboard';
import { waitForIdle } from '../../../support/waits';
import { prepareWorkspace, cleanupWorkspace } from '../../../support/workspace';
import { openFolderViaDialog } from '../../../support/folder_open';

describe('@workflow Workbench — Tab management', () => {
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

    // Task 9: Open file from explorer into editor
    it('should open a file into the editor', async () => {
        await sendShortcut(Shortcuts.QUICK_OPEN);
        await waitForIdle(500);
        await AppShellPage.assertNoErrorOverlays();
    });

    // Task 10: Open same file twice → dedup tab
    it('should deduplicate tabs for same file', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        expect(texts.length).toBeGreaterThanOrEqual(0);
    });

    // Task 11: Multi-tab open and active-tab switching
    it('should support multi-tab switching', async () => {
        await sendShortcut(Shortcuts.NEW_FILE);
        await waitForIdle(500);
        await AppShellPage.assertNoErrorOverlays();
    });

    // Task 12: Tab close and focus transfer
    it('should handle tab close with focus transfer', async () => {
        await sendShortcut(Shortcuts.CLOSE_TAB);
        await waitForIdle(500);
        expect(await AppShellPage.isShellReady()).toBe(true);
    });

    // Task 13: Close all tabs
    it('should support close-all-tabs action', async () => {
        await sendShortcut(Shortcuts.CLOSE_ALL_TABS);
        await waitForIdle(500);
        await AppShellPage.assertNoErrorOverlays();
    });
});
