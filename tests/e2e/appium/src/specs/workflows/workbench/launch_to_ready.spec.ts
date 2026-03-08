/**
 * @critical Workbench — Launch to ready smoke validation
 * Phase 03, Task 1: Automate launch-to-ready smoke validation.
 */
import AppShellPage from '../../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../../support/session';
import { setupE2EMode } from '../../../support/e2e_mode';

describe('@critical Workbench — Launch to ready', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await setupE2EMode();
    });
    afterEach(async () => { await resetAppState(); });

    it('should detect MarkAmp window via accessibility tree', async () => {
        expect(await (await AppShellPage.mainWindow).isExisting()).toBe(true);
    });
    it('should have shell frame identifier', async () => {
        expect(await (await AppShellPage.shellFrame).isExisting()).toBe(true);
    });
    it('should have window title "MarkAmp"', async () => {
        expect(await AppShellPage.getWindowTitle()).toBe('MarkAmp');
    });
    it('should have non-zero window size', async () => {
        const size = await AppShellPage.getWindowSize();
        expect(size.width).toBeGreaterThan(0);
        expect(size.height).toBeGreaterThan(0);
    });
    it('should have no error dialogs', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });
    it('should have non-empty page source', async () => {
        const source = await browser.getPageSource();
        expect(source.length).toBeGreaterThan(100);
    });
});
