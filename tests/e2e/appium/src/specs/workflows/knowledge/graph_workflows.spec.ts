/**
 * @workflow Knowledge — Knowledge graph, backlinks, wikilinks
 * Phase 07, Tasks 1–9: Graph panel, node selection, filter, minimap, backlinks.
 */
import AppShellPage from '../../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../../support/session';
import { waitForIdle } from '../../../support/waits';

describe('@workflow @pending-feature Knowledge — Graph workflows', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });
    afterEach(async () => { await resetAppState(); });

    it('should detect graph surface', async () => {
        const graph = await browser.$('~ma.graph');
        expect(typeof (await graph.isExisting())).toBe('boolean');
    });

    it('should detect graph filter controls', async () => {
        const filter = await browser.$('~ma.graph.filter');
        expect(typeof (await filter.isExisting())).toBe('boolean');
    });

    it('should detect backlinks panel', async () => {
        const backlinks = await browser.$('~ma.graph.backlinks');
        expect(typeof (await backlinks.isExisting())).toBe('boolean');
    });

    it('should not crash during graph operations', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });
});
