/**
 * @file perf.ts
 *
 * Performance measurement utilities for E2E tests.
 * Phase 09: Timing, resource monitoring, and budget enforcement.
 */

// ── Performance Measurement ──

export interface PerfMeasurement {
    /** Name of the operation measured */
    operation: string;
    /** Duration in milliseconds */
    durationMs: number;
    /** Timestamp when measurement started */
    startTime: number;
    /** Timestamp when measurement ended */
    endTime: number;
    /** Optional budget threshold in milliseconds */
    budgetMs?: number;
    /** Whether the measurement exceeded its budget */
    exceededBudget: boolean;
}

/**
 * Measure the duration of an async operation.
 * @param operation - Human-readable name for the operation
 * @param fn - Async function to measure
 * @param budgetMs - Optional performance budget in milliseconds
 */
export async function measurePerformance<T>(
    operation: string,
    fn: () => Promise<T>,
    budgetMs?: number
): Promise<{ result: T; measurement: PerfMeasurement }> {
    const startTime = Date.now();
    const result = await fn();
    const endTime = Date.now();
    const durationMs = endTime - startTime;

    const measurement: PerfMeasurement = {
        operation,
        durationMs,
        startTime,
        endTime,
        budgetMs,
        exceededBudget: budgetMs !== undefined ? durationMs > budgetMs : false,
    };

    return { result, measurement };
}

// ── Performance Budgets ──

export interface PerformanceBudget {
    operation: string;
    maxDurationMs: number;
    description: string;
}

export const DEFAULT_PERFORMANCE_BUDGETS: PerformanceBudget[] = [
    { operation: 'app_launch', maxDurationMs: 15000, description: 'App launch to ready' },
    { operation: 'file_open', maxDurationMs: 3000, description: 'Open file in editor' },
    { operation: 'file_save', maxDurationMs: 2000, description: 'Save file' },
    { operation: 'search_results', maxDurationMs: 5000, description: 'Global search first results' },
    { operation: 'command_palette_open', maxDurationMs: 500, description: 'Command palette open' },
    { operation: 'tab_switch', maxDurationMs: 500, description: 'Tab switch' },
    { operation: 'settings_open', maxDurationMs: 1000, description: 'Settings panel open' },
    { operation: 'theme_switch', maxDurationMs: 2000, description: 'Theme switch apply' },
    { operation: 'canvas_load', maxDurationMs: 5000, description: 'Canvas board load' },
    { operation: 'preview_render', maxDurationMs: 3000, description: 'Markdown preview render' },
    { operation: 'graph_render', maxDurationMs: 5000, description: 'Knowledge graph render' },
    { operation: 'extension_install', maxDurationMs: 10000, description: 'Extension install' },
];

/**
 * Get the performance budget for an operation.
 */
export function getBudget(operation: string): PerformanceBudget | undefined {
    return DEFAULT_PERFORMANCE_BUDGETS.find(b => b.operation === operation);
}

// ── Performance Report ──

export interface PerfReport {
    measurements: PerfMeasurement[];
    timestamp: string;
    totalDurationMs: number;
    budgetViolations: PerfMeasurement[];
}

/**
 * Generate a performance report from a set of measurements.
 */
export function generatePerfReport(measurements: PerfMeasurement[]): PerfReport {
    const violations = measurements.filter(m => m.exceededBudget);
    const total = measurements.reduce((sum, m) => sum + m.durationMs, 0);

    return {
        measurements,
        timestamp: new Date().toISOString(),
        totalDurationMs: total,
        budgetViolations: violations,
    };
}
