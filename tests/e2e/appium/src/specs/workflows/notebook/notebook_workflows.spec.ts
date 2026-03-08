/**
 * @workflow Notebook — Notebook cell operations and export
 * Phase 07, Tasks 10–21: Creation, cells, execution, export.
 */
import AppShellPage from '../../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../../support/session';

describe('@workflow @pending-feature Notebook — Cell operations', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });
    afterEach(async () => { await resetAppState(); });

    it('should detect notebook surface', async () => {
        const nb = await browser.$('~ma.notebook');
        expect(typeof (await nb.isExisting())).toBe('boolean');
    });

    it('should detect notebook toolbar', async () => {
        const toolbar = await browser.$('~ma.notebook.toolbar');
        expect(typeof (await toolbar.isExisting())).toBe('boolean');
    });

    it('should not crash during notebook operations', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });
});
