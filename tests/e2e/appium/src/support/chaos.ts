/**
 * @file chaos.ts
 *
 * Chaos engineering simulation utilities for E2E tests.
 * Phase 09: Fault injection, resource stress, and recovery validation.
 */

// ── Chaos Scenarios ──

export type ChaosScenario =
    | 'disk_full'
    | 'network_drop'
    | 'slow_disk'
    | 'cpu_spike'
    | 'memory_pressure'
    | 'process_crash'
    | 'permission_denied';

export interface ChaosConfig {
    scenario: ChaosScenario;
    durationMs: number;
    intensity: 'low' | 'medium' | 'high';
    description: string;
}

/**
 * Simulate a disk-full scenario by filling the temp workspace.
 */
export async function simulateDiskFull(workspacePath: string): Promise<void> {
    // In E2E context, we simulate disk full by setting a flag
    // that the app can check via MARKAMP_E2E_CHAOS_DISK_FULL=1
    process.env['MARKAMP_E2E_CHAOS_DISK_FULL'] = '1';
}

/**
 * Simulate network drop by setting chaos environment variable.
 */
export async function simulateNetworkDrop(): Promise<void> {
    process.env['MARKAMP_E2E_CHAOS_NETWORK_DROP'] = '1';
}

/**
 * Simulate slow disk I/O.
 */
export async function simulateSlowDisk(delayMs: number): Promise<void> {
    process.env['MARKAMP_E2E_CHAOS_SLOW_DISK'] = String(delayMs);
}

/**
 * Clear all chaos simulation flags.
 */
export function clearChaosFlags(): void {
    delete process.env['MARKAMP_E2E_CHAOS_DISK_FULL'];
    delete process.env['MARKAMP_E2E_CHAOS_NETWORK_DROP'];
    delete process.env['MARKAMP_E2E_CHAOS_SLOW_DISK'];
    delete process.env['MARKAMP_E2E_CHAOS_CPU_SPIKE'];
    delete process.env['MARKAMP_E2E_CHAOS_MEMORY_PRESSURE'];
}

/**
 * Simulate rapid, repeated user actions to test resilience.
 */
export async function simulateRapidActions(
    actionFn: () => Promise<void>,
    repetitions: number,
    intervalMs: number
): Promise<{ completedCount: number; errors: string[] }> {
    let completedCount = 0;
    const errors: string[] = [];

    for (let i = 0; i < repetitions; i++) {
        try {
            await actionFn();
            completedCount++;
        } catch (error) {
            errors.push(error instanceof Error ? error.message : String(error));
        }
        if (i < repetitions - 1) {
            await new Promise(resolve => setTimeout(resolve, intervalMs));
        }
    }

    return { completedCount, errors };
}

// ── Recovery Validation ──

/**
 * Verify app is in a healthy state after chaos simulation.
 */
export async function verifyAppRecovery(): Promise<boolean> {
    try {
        const mainWindow = await browser.$(
            '-ios predicate string:elementType == 4 AND title == "MarkAmp"'
        );
        return mainWindow.isExisting();
    } catch {
        return false;
    }
}
