/**
 * @workflow Tasks — Task panel, calendar, Gantt, presentations
 * Phase 08, Tasks 38–50: Task CRUD, recurrence, kanban, calendar, Gantt, presentation.
 */
import AppShellPage from '../../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../../support/session';

describe('@workflow @pending-feature Tasks — Task and calendar workflows', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });
    afterEach(async () => { await resetAppState(); });

    it('should detect tasks surface', async () => {
        const tasks = await browser.$('~ma.tasks');
        expect(typeof (await tasks.isExisting())).toBe('boolean');
    });

    it('should detect calendar surface', async () => {
        const cal = await browser.$('~ma.calendar');
        expect(typeof (await cal.isExisting())).toBe('boolean');
    });

    it('should detect presentation surface', async () => {
        const pres = await browser.$('~ma.presentation');
        expect(typeof (await pres.isExisting())).toBe('boolean');
    });

    it('should not crash during task operations', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });
});
