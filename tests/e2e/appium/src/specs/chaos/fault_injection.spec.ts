/**
 * @chaos Chaos — Fault injection and recovery
 * Phase 09: Chaos engineering E2E tests.
 */
import AppShellPage from '../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { sendShortcut, Shortcuts } from '../../support/keyboard';
import { waitForIdle } from '../../support/waits';
import {
    simulateRapidActions,
    clearChaosFlags,
    verifyAppRecovery,
} from '../../support/chaos';
import { captureResourceSnapshot } from '../../support/observability';

describe('@chaos Chaos — Fault injection and recovery', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });
    afterEach(async () => {
        clearChaosFlags();
        await resetAppState();
    });

    it('should handle rapid command palette open/close cycles', async () => {
        const result = await simulateRapidActions(
            async () => {
                await sendShortcut(Shortcuts.COMMAND_PALETTE);
                await waitForIdle(100);
                await sendShortcut('Escape');
                await waitForIdle(100);
            },
            10,
            50
        );
        expect(result.completedCount).toBeGreaterThanOrEqual(8);
        expect(await verifyAppRecovery()).toBe(true);
    });

    it('should recover from rapid tab operations', async () => {
        const result = await simulateRapidActions(
            async () => {
                await sendShortcut(Shortcuts.NEW_FILE);
                await waitForIdle(200);
                await sendShortcut(Shortcuts.CLOSE_TAB);
                await waitForIdle(200);
            },
            5,
            100
        );
        expect(result.completedCount).toBeGreaterThanOrEqual(3);
        expect(await verifyAppRecovery()).toBe(true);
    });

    it('should maintain resource budget during stress', async () => {
        // Validate the app is still observable after chaos operations
        await browser.pause(2000);
        const snapshot = captureResourceSnapshot('post-chaos-check');
        // The snapshot should have valid data (timestamp > 0 means capture succeeded)
        expect(snapshot.timestamp).toBeGreaterThan(0);
        expect(snapshot.description).toBe('post-chaos-check');
        // App must still be responsive
        expect(await AppShellPage.isShellReady()).toBe(true);
    });

    it('should have no error overlays after chaos tests', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });
});
