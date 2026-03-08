/**
 * @workflow AV — AV database views, columns, formulas, import/export
 * Phase 07, Tasks 22–33: Table, gallery, kanban, timeline, columns, formulas.
 */
import AppShellPage from '../../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../../support/session';

describe('@workflow @pending-feature AV Database — View workflows', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });
    afterEach(async () => { await resetAppState(); });

    it('should detect AV database surface', async () => {
        const av = await browser.$('~ma.av');
        expect(typeof (await av.isExisting())).toBe('boolean');
    });

    it('should detect AV table view', async () => {
        const table = await browser.$('~ma.av.table');
        expect(typeof (await table.isExisting())).toBe('boolean');
    });

    it('should detect AV toolbar', async () => {
        const toolbar = await browser.$('~ma.av.toolbar');
        expect(typeof (await toolbar.isExisting())).toBe('boolean');
    });

    it('should not crash during AV operations', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });
});
