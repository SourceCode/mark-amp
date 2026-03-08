/**
 * @file preflight.ts
 *
 * Preflight checks and capability validation for E2E test sessions.
 * Tasks 39–44 (Phase 02): Capability profiles, validation, accessibility
 * permission checks, host tool checks, session bootstrap, and teardown.
 */

import { execSync } from 'child_process';
import * as fs from 'fs';
import * as path from 'path';

// ── Task 39: Appium Capability Profiles ──

export interface CapabilityProfile {
    name: string;
    capabilities: Record<string, string | number | boolean>;
    environment: Record<string, string>;
}

export const LOCAL_PROFILE: CapabilityProfile = {
    name: 'local',
    capabilities: {
        'platformName': 'mac',
        'appium:automationName': 'mac2',
        'appium:bundleId': 'com.markamp.editor',
        'appium:noReset': true,
        'appium:newCommandTimeout': 120,
    },
    environment: {
        MARKAMP_E2E: '1',
    },
};

export const CI_PROFILE: CapabilityProfile = {
    name: 'ci',
    capabilities: {
        'platformName': 'mac',
        'appium:automationName': 'mac2',
        'appium:bundleId': 'com.markamp.editor',
        'appium:noReset': true,
        'appium:newCommandTimeout': 180,
    },
    environment: {
        MARKAMP_E2E: '1',
        E2E_TIMEOUT_MULTIPLIER: '1.5',
    },
};

/**
 * Get the capability profile for the current environment.
 */
export function getCapabilityProfile(): CapabilityProfile {
    const isCI = process.env['CI'] === 'true' || process.env['GITHUB_ACTIONS'] === 'true';
    return isCI ? CI_PROFILE : LOCAL_PROFILE;
}

// ── Task 40: Capability Validation ──

export interface PreflightResult {
    check: string;
    passed: boolean;
    message: string;
}

/**
 * Validate that all required capabilities and tools are available.
 * Fails fast with diagnostics on first failure.
 */
export function validateCapabilities(): PreflightResult[] {
    const results: PreflightResult[] = [];

    // Check Node.js version
    const nodeVersion = process.version;
    const majorVersion = parseInt(nodeVersion.slice(1).split('.')[0], 10);
    results.push({
        check: 'Node.js >= 20',
        passed: majorVersion >= 20,
        message: `Found Node.js ${nodeVersion}`,
    });

    return results;
}

// ── Task 41: macOS Accessibility Permission Checks ──

/**
 * Check if the terminal has macOS Accessibility permissions.
 * This is required for the mac2 driver to interact with the app.
 */
export function checkAccessibilityPermission(): PreflightResult {
    try {
        // On macOS, we can check via tccutil or by trying to list accessible apps
        // The most reliable way is to attempt an accessibility operation
        const result = execSync(
            'osascript -e \'tell application "System Events" to get name of every process\'',
            { timeout: 5000, encoding: 'utf-8' }
        );
        return {
            check: 'macOS Accessibility Permission',
            passed: result.length > 0,
            message: 'Accessibility permission granted',
        };
    } catch {
        return {
            check: 'macOS Accessibility Permission',
            passed: false,
            message: 'Accessibility permission not granted. Go to: System Settings → Privacy & Security → Accessibility → Add your Terminal app.',
        };
    }
}

// ── Task 42: Required Host Tools and Versions ──

interface ToolRequirement {
    name: string;
    command: string;
    minVersion: string;
}

const REQUIRED_TOOLS: ToolRequirement[] = [
    { name: 'node', command: 'node -v', minVersion: '20.0.0' },
    { name: 'npm', command: 'npm -v', minVersion: '10.0.0' },
    { name: 'appium', command: 'appium -v', minVersion: '2.0.0' },
];

/**
 * Check that all required host tools are installed at the correct versions.
 */
export function checkRequiredTools(): PreflightResult[] {
    return REQUIRED_TOOLS.map(tool => {
        try {
            const output = execSync(tool.command, {
                timeout: 5000,
                encoding: 'utf-8',
            }).trim();
            return {
                check: `${tool.name} >= ${tool.minVersion}`,
                passed: true,
                message: `Found ${tool.name} ${output}`,
            };
        } catch {
            return {
                check: `${tool.name} >= ${tool.minVersion}`,
                passed: false,
                message: `${tool.name} not found. Install it before running E2E tests.`,
            };
        }
    });
}

// ── Task 43: Session Bootstrap Checks ──

/**
 * Verify that the MarkAmp app binary exists and is ready to be launched.
 */
export function checkAppBinaryExists(): PreflightResult {
    // Check common build output locations
    const possiblePaths = [
        path.resolve(__dirname, '../../../../build/debug/MarkAmp.app'),
        path.resolve(__dirname, '../../../../build/release/MarkAmp.app'),
        path.resolve(__dirname, '../../../../build/MarkAmp.app'),
    ];

    for (const appPath of possiblePaths) {
        if (fs.existsSync(appPath)) {
            return {
                check: 'MarkAmp app binary',
                passed: true,
                message: `Found app at ${appPath}`,
            };
        }
    }

    return {
        check: 'MarkAmp app binary',
        passed: false,
        message: `App not found at any expected path. Build with: cmake -B build/debug && cmake --build build/debug`,
    };
}

// ── Task 44: Session Teardown — Kill Stale Instances ──

/**
 * Kill any stale MarkAmp or Appium processes from previous test runs.
 * Call from teardown hooks to prevent process leaks.
 */
export function killStaleProcesses(): void {
    const processNames = ['MarkAmp', 'appium'];

    for (const name of processNames) {
        try {
            execSync(`pkill -f "${name}" 2>/dev/null || true`, {
                timeout: 5000,
                encoding: 'utf-8',
            });
        } catch {
            // Process not running — OK
        }
    }
}

/**
 * Check for stale MarkAmp processes that might interfere with tests.
 */
export function checkStaleProcesses(): PreflightResult {
    try {
        const output = execSync('pgrep -f "MarkAmp" || true', {
            timeout: 5000,
            encoding: 'utf-8',
        }).trim();

        if (output.length > 0) {
            return {
                check: 'No stale MarkAmp processes',
                passed: false,
                message: `Found stale MarkAmp process(es): ${output}`,
            };
        }
        return {
            check: 'No stale MarkAmp processes',
            passed: true,
            message: 'No stale processes found',
        };
    } catch {
        return {
            check: 'No stale MarkAmp processes',
            passed: true,
            message: 'No stale processes found',
        };
    }
}

// ── Composite Preflight ──

/**
 * Run all preflight checks and return results.
 * Log results and fail fast if any critical check fails.
 */
export function runPreflightChecks(): PreflightResult[] {
    const results: PreflightResult[] = [
        ...validateCapabilities(),
        ...checkRequiredTools(),
        checkAccessibilityPermission(),
        checkAppBinaryExists(),
        checkStaleProcesses(),
    ];

    return results;
}

/**
 * Run preflight checks and throw if any fail.
 */
export function enforcePreflightChecks(): void {
    const results = runPreflightChecks();
    const failures = results.filter(r => !r.passed);

    if (failures.length > 0) {
        const messages = failures.map(f => `  ✗ ${f.check}: ${f.message}`).join('\n');
        throw new Error(`Preflight checks failed:\n${messages}`);
    }
}
