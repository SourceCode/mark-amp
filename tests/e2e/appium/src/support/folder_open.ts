/**
 * folder_open.ts — Workspace folder opening helpers for E2E tests.
 *
 * Opens a workspace via Cmd+O → Cmd+Shift+G → type path → Enter and
 * provides utilities for waiting until the workspace is fully loaded.
 *
 * NOTE: Only `ma.shell.main_frame` is exposed in the accessibility tree.
 * Child identifiers (ma.activitybar, ma.filetree.ctrl, etc.) do NOT
 * appear in the accessibility tree even after opening a workspace folder.
 * Therefore we detect workspace readiness by looking for workspace-specific
 * text in the page source (e.g. file names, folder name, "Explorer" text).
 *
 * WARNING: The native macOS Open dialog can destabilize the Appium session.
 * All dialog interaction is wrapped in try/catch to absorb session errors.
 */

import * as path from 'path';
import { sendShortcut, pressEnter, pressEscape, Shortcuts } from './keyboard';

const FIXTURE_ROOT = path.resolve(__dirname, '../../../fixtures/workspaces');

/**
 * Get the absolute path to a named fixture workspace.
 */
export function getFixturePath(fixtureName: string): string {
    return path.join(FIXTURE_ROOT, fixtureName);
}

/**
 * Open a folder workspace using the native macOS Open dialog.
 * All steps are wrapped in try/catch because the native dialog
 * can corrupt the Appium session.
 *
 * @param folderPath - Absolute path to the folder to open
 */
export async function openFolderViaDialog(folderPath: string): Promise<void> {
    try {
        // 1. Trigger File → Open (Cmd+O)
        await sendShortcut(Shortcuts.OPEN_FILE);
        await browser.pause(1500);
    } catch {
        // Session may have been invalidated — soft-fail
        return;
    }

    try {
        // 2. In the Open dialog, use Cmd+Shift+G for "Go to Folder" sheet
        await sendShortcut('Cmd+Shift+G');
        await browser.pause(800);
    } catch {
        return;
    }

    try {
        // 3. Type the path character by character
        await browser.execute('macos: keys', {
            keys: folderPath.split('').map((c: string) => ({ key: c })),
        });
        await browser.pause(500);
    } catch {
        return;
    }

    try {
        // 4. Press Enter to navigate to the folder
        await pressEnter();
        await browser.pause(800);

        // 5. Press Enter again to open the selected folder
        await pressEnter();
        await browser.pause(2000);
    } catch {
        // Session errors during dialog close — soft-fail
    }
}

/**
 * Wait for the workspace to be fully loaded.
 *
 * Since child accessibility identifiers (ma.activitybar, etc.) are NOT
 * available in the accessibility tree, we detect workspace readiness by:
 * 1. First checking for the shell main frame
 * 2. Then checking for any of: workspace text in page source, new text
 *    elements, or specific marker text like "Explorer" panel header.
 */
export async function waitForWorkspaceReady(timeoutMs = 20000): Promise<boolean> {
    const start = Date.now();
    while (Date.now() - start < timeoutMs) {
        try {
            // Check #1: Is the main frame still up?
            const shell = await browser.$('~ma.shell.main_frame');
            const shellExists = await shell.isExisting();
            if (!shellExists) {
                await browser.pause(500);
                continue;
            }

            // Check #2: Look for workspace indicators in the page source
            const source = await browser.getPageSource();

            // After opening a folder, the window title changes to include folder name,
            // "Explorer" panel text appears, or file names appear as text
            const hasExplorer = source.includes('Explorer');
            const hasActivityBar = source.includes('ma.activitybar');
            const hasFileTree = source.includes('ma.filetree');
            const hasStatusBar = source.includes('ma.statusbar');

            // Any of these indicates the workspace loaded
            if (hasExplorer || hasActivityBar || hasFileTree || hasStatusBar) {
                await browser.pause(500);
                return true;
            }

            // Check #3: If a file name from the fixture workspace appears
            // in page content, the workspace is loaded
            const hasReadme = source.includes('README');
            const hasNotes = source.includes('NOTES');
            if (hasReadme || hasNotes) {
                await browser.pause(500);
                return true;
            }
        } catch {
            // Session lost or element not found — soft-fail and return false
            return false;
        }
        await browser.pause(500);
    }

    // Timeout reached — workspace may not have opened, but continue anyway
    return false;
}

/**
 * Open a named fixture workspace and wait for it to load.
 * Returns true if workspace loaded successfully.
 * Returns false if the session was invalidated or workspace didn't load.
 */
export async function openFixtureWorkspace(fixtureName: string): Promise<boolean> {
    const folderPath = getFixturePath(fixtureName);

    // First dismiss any open dialogs
    try {
        await pressEscape();
        await browser.pause(300);
    } catch {
        // Session may already be unstable — return false
        return false;
    }

    await openFolderViaDialog(folderPath);

    try {
        return await waitForWorkspaceReady();
    } catch {
        return false;
    }
}
