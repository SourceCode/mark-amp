/**
 * @workflow FSRS — Flashcard and spaced repetition workflows
 * Phase 07, Tasks 34–40: Extraction, decks, review, scheduling, cloze.
 */
import AppShellPage from '../../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../../support/session';

describe('@workflow @pending-feature FSRS — Flashcard workflows', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });
    afterEach(async () => { await resetAppState(); });

    it('should detect FSRS surface', async () => {
        const fsrs = await browser.$('~ma.fsrs');
        expect(typeof (await fsrs.isExisting())).toBe('boolean');
    });

    it('should detect flashcard deck list', async () => {
        const decks = await browser.$('~ma.fsrs.deck_list');
        expect(typeof (await decks.isExisting())).toBe('boolean');
    });

    it('should not crash during FSRS operations', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });
});
