/**
 * @file isolation.ts
 *
 * Environment isolation utilities for E2E tests.
 * Tasks 31–33 (Phase 02): Isolated config, cache, and extension storage.
 */

import * as fs from 'fs';
import * as path from 'path';
import * as os from 'os';

const E2E_BASE_DIR = path.join(os.tmpdir(), 'markamp_e2e');

// ── Task 31: Isolated User Config Directory ──

/**
 * Get or create the isolated user config directory for E2E sessions.
 * Returns the absolute path to the isolated config directory.
 */
export function getIsolatedConfigDir(): string {
    const configDir = path.join(E2E_BASE_DIR, 'config');
    fs.mkdirSync(configDir, { recursive: true });
    return configDir;
}

// ── Task 32: Isolated Cache Directory ──

/**
 * Get or create the isolated cache directory for E2E sessions.
 * Returns the absolute path to the isolated cache directory.
 */
export function getIsolatedCacheDir(): string {
    const cacheDir = path.join(E2E_BASE_DIR, 'cache');
    fs.mkdirSync(cacheDir, { recursive: true });
    return cacheDir;
}

// ── Task 33: Isolated Extension Storage ──

/**
 * Get or create the isolated extension storage directory for E2E sessions.
 * Returns the absolute path to the isolated extension storage directory.
 */
export function getIsolatedExtensionDir(): string {
    const extDir = path.join(E2E_BASE_DIR, 'extensions');
    fs.mkdirSync(extDir, { recursive: true });
    return extDir;
}

// ── Composite Utilities ──

/**
 * Initialize all isolated directories for an E2E session.
 * Returns an object with paths to each isolated directory.
 */
export function initializeIsolation(): {
    configDir: string;
    cacheDir: string;
    extensionDir: string;
    baseDir: string;
} {
    return {
        configDir: getIsolatedConfigDir(),
        cacheDir: getIsolatedCacheDir(),
        extensionDir: getIsolatedExtensionDir(),
        baseDir: E2E_BASE_DIR,
    };
}

/**
 * Clean up all isolated directories after an E2E session.
 */
export function cleanupIsolation(): void {
    if (fs.existsSync(E2E_BASE_DIR)) {
        fs.rmSync(E2E_BASE_DIR, { recursive: true, force: true });
    }
}

/**
 * Reset isolation directories (clean + reinitialize).
 */
export function resetIsolation(): {
    configDir: string;
    cacheDir: string;
    extensionDir: string;
    baseDir: string;
} {
    cleanupIsolation();
    return initializeIsolation();
}
