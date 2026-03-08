/**
 * Workspace fixture utilities for E2E tests.
 *
 * Copies fixture workspaces to a temp directory for isolation,
 * then cleans up after each test suite.
 */

import * as fs from 'fs';
import * as path from 'path';
import * as os from 'os';

const FIXTURES_ROOT = path.resolve(__dirname, '../../../fixtures/workspaces');

/**
 * Copy a named fixture workspace to a temp directory.
 * Returns the absolute path to the temp workspace.
 */
export function prepareWorkspace(fixtureName: string): string {
    const src = path.join(FIXTURES_ROOT, fixtureName);
    if (!fs.existsSync(src)) {
        throw new Error(`Fixture workspace not found: ${src}`);
    }

    const tmpDir = fs.mkdtempSync(path.join(os.tmpdir(), `markamp_e2e_${fixtureName}_`));
    copyDirSync(src, tmpDir);
    return tmpDir;
}

/**
 * Remove a temporary workspace directory.
 */
export function cleanupWorkspace(workspacePath: string): void {
    if (workspacePath.includes('markamp_e2e_') && fs.existsSync(workspacePath)) {
        fs.rmSync(workspacePath, { recursive: true, force: true });
    }
}

/**
 * Recursive directory copy.
 */
function copyDirSync(src: string, dest: string): void {
    fs.mkdirSync(dest, { recursive: true });
    for (const entry of fs.readdirSync(src, { withFileTypes: true })) {
        const srcPath = path.join(src, entry.name);
        const destPath = path.join(dest, entry.name);
        if (entry.isDirectory()) {
            copyDirSync(srcPath, destPath);
        } else {
            fs.copyFileSync(srcPath, destPath);
        }
    }
}
