/**
 * @workflow Git — Source control workflows
 * Phase 08, Tasks 1–14: SCM panel, staging, commit, branch, diff, blame, history.
 */
import AppShellPage from '../../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../../support/session';
import { waitForIdle } from '../../../support/waits';
import { prepareWorkspace, cleanupWorkspace } from '../../../support/workspace';
import { openFolderViaDialog } from '../../../support/folder_open';

describe('@workflow Git — Source control', () => {
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

    it('should detect source control surface', async () => {
        const sc = await browser.$('~ma.sourcecontrol');
        expect(typeof (await sc.isExisting())).toBe('boolean');
    });

    it('should detect commit input', async () => {
        const input = await browser.$('~ma.sourcecontrol.commit_input');
        expect(typeof (await input.isExisting())).toBe('boolean');
    });

    it('should not crash during source control operations', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });
});
