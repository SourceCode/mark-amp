/**
 * @file observability.ts
 *
 * Observability and resource monitoring utilities for E2E tests.
 * Phase 09: CPU, memory, log monitoring, and telemetry.
 */

import { execSync } from 'child_process';

// ── Resource Monitoring ──

export interface ResourceSnapshot {
    timestamp: number;
    cpuPercent: number;
    memoryMB: number;
    description: string;
}

/**
 * Capture a resource snapshot for the MarkAmp process.
 */
export function captureResourceSnapshot(description: string): ResourceSnapshot {
    try {
        const output = execSync(
            'ps aux | grep -i markamp | grep -v grep | head -1',
            { timeout: 5000, encoding: 'utf-8' }
        ).trim();

        if (output.length === 0) {
            return {
                timestamp: Date.now(),
                cpuPercent: 0,
                memoryMB: 0,
                description,
            };
        }

        const parts = output.split(/\s+/);
        const cpuPercent = parseFloat(parts[2] ?? '0');
        const memoryMB = parseFloat(parts[5] ?? '0') / 1024; // RSS in KB → MB

        return {
            timestamp: Date.now(),
            cpuPercent,
            memoryMB,
            description,
        };
    } catch {
        return {
            timestamp: Date.now(),
            cpuPercent: 0,
            memoryMB: 0,
            description,
        };
    }
}

// ── Resource Budget ──

export interface ResourceBudget {
    maxCpuPercent: number;
    maxMemoryMB: number;
}

export const DEFAULT_RESOURCE_BUDGET: ResourceBudget = {
    maxCpuPercent: 80,
    maxMemoryMB: 1024,
};

/**
 * Check if current resources are within budget.
 */
export function isWithinBudget(
    snapshot: ResourceSnapshot,
    budget: ResourceBudget = DEFAULT_RESOURCE_BUDGET
): boolean {
    return (
        snapshot.cpuPercent <= budget.maxCpuPercent &&
        snapshot.memoryMB <= budget.maxMemoryMB
    );
}

// ── Log Monitoring ──

export interface LogEntry {
    timestamp: string;
    level: 'debug' | 'info' | 'warn' | 'error';
    message: string;
    source: string;
}

/**
 * Scan for error-level log entries from captured app logs.
 */
export function findErrorLogs(logs: LogEntry[]): LogEntry[] {
    return logs.filter(log => log.level === 'error');
}

/**
 * Scan for warning-level log entries from captured app logs.
 */
export function findWarningLogs(logs: LogEntry[]): LogEntry[] {
    return logs.filter(log => log.level === 'warn');
}
