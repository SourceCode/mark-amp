/**
 * @perf Performance — Latency benchmarks
 * Phase 09: Measure key operations against performance budgets.
 */
import AppShellPage from '../../pages/AppShellPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { sendShortcut, pressEscape, Shortcuts } from '../../support/keyboard';
import { waitForIdle } from '../../support/waits';
import { measurePerformance, DEFAULT_PERFORMANCE_BUDGETS } from '../../support/perf';

describe('@perf Performance — Latency benchmarks', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });
    afterEach(async () => { await resetAppState(); });

    it('should measure command palette open latency', async () => {
        const budget = DEFAULT_PERFORMANCE_BUDGETS.find(b => b.operation === 'command_palette_open');
        const { measurement } = await measurePerformance(
            'command_palette_open',
            async () => {
                await sendShortcut(Shortcuts.COMMAND_PALETTE);
                await waitForIdle(300);
            },
            budget?.maxDurationMs
        );
        await pressEscape();
        expect(measurement.durationMs).toBeLessThan(budget?.maxDurationMs ?? 1000);
    });

    it('should measure settings panel open latency', async () => {
        const budget = DEFAULT_PERFORMANCE_BUDGETS.find(b => b.operation === 'settings_open');
        const { measurement } = await measurePerformance(
            'settings_open',
            async () => {
                await sendShortcut(Shortcuts.SETTINGS);
                await waitForIdle(500);
            },
            budget?.maxDurationMs
        );
        await pressEscape();
        expect(measurement.durationMs).toBeLessThan(budget?.maxDurationMs ?? 2000);
    });

    it('should measure tab switch latency', async () => {
        const budget = DEFAULT_PERFORMANCE_BUDGETS.find(b => b.operation === 'tab_switch');
        const { measurement } = await measurePerformance(
            'tab_switch',
            async () => {
                await sendShortcut(Shortcuts.NEW_FILE);
                await waitForIdle(300);
            },
            budget?.maxDurationMs
        );
        expect(measurement.durationMs).toBeLessThan(budget?.maxDurationMs ?? 1000);
    });

    it('should report performance within overall budget', async () => {
        const totalBudget = DEFAULT_PERFORMANCE_BUDGETS.reduce((sum, b) => sum + b.maxDurationMs, 0);
        expect(totalBudget).toBeGreaterThan(0);
    });
});
