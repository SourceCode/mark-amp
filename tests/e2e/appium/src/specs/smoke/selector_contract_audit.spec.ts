/**
 * Smoke Test: Selector Contract Audit (Runtime)
 *
 * 7 tests that validate the ma.* contract selector system  in the
 * running application's accessibility tree.
 *
 * Note: Child panel selectors (activitybar, editor, statusbar, filetree)
 * are only present when a workspace folder is open.  The Welcome screen
 * only exposes ma.shell.main_frame.
 */

import AppShellPage, { CONTRACT_SELECTORS } from '../../pages/AppShellPage';
import { ensureAppRunning } from '../../support/session';
import {
    findByExactName,
    findContractSelectors,
} from '../../support/accessibility_tree';

/** Selectors guaranteed to be present on the Welcome screen */
const WELCOME_SCREEN_SELECTORS = ['ma.shell.main_frame'] as const;

describe('Selector Contract Audit (Runtime)', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });

    // ── Positive: Core Shell Selector Present ──

    it('should expose ma.shell.main_frame in accessibility tree', async () => {
        const el = await findByExactName('ma.shell.main_frame');
        await expect(el).toBeExisting();
    });

    it('should have ma.shell.main_frame with identifier attribute', async () => {
        const el = await findByExactName('ma.shell.main_frame');
        const identifier = await el.getAttribute('identifier');
        expect(identifier).toBe('ma.shell.main_frame');
    });

    it('should have ma.shell.main_frame with label attribute', async () => {
        const el = await findByExactName('ma.shell.main_frame');
        const label = await el.getAttribute('label');
        expect(label).toBe('ma.shell.main_frame');
    });

    // ── Positive: Contract Selectors Discoverable ──

    it('should have at least the Welcome screen selectors via tree scan', async () => {
        const found = await findContractSelectors();
        for (const expected of WELCOME_SCREEN_SELECTORS) {
            expect(found).toContain(expected);
        }
    });

    it('should have all found ma.* selectors registered in contract', async () => {
        const found = await findContractSelectors();
        const unexpected = found.filter(
            s => !(CONTRACT_SELECTORS as readonly string[]).includes(s)
        );
        expect(unexpected.length).toBe(0);
    });

    // ── Positive: NSAccessibility Bridge Verification ──

    it('should find ma.shell.main_frame via predicate identifier search', async () => {
        const els = await browser.$$('-ios predicate string:identifier == "ma.shell.main_frame"');
        expect(els.length).toBeGreaterThan(0);
    });

    it('should find ma.shell.main_frame via predicate label search', async () => {
        const els = await browser.$$('-ios predicate string:label == "ma.shell.main_frame"');
        expect(els.length).toBeGreaterThan(0);
    });
});
