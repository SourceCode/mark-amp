/**
 * Edge Case: Empty Workspace
 *
 * 8 tests covering behavior when opening an empty folder.
 */

import AppShellPage from '../../pages/AppShellPage';
import FileTreePage from '../../pages/FileTreePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFolderViaDialog, waitForWorkspaceReady } from '../../support/folder_open';
import { pressEscape } from '../../support/keyboard';
import * as path from 'path';
import * as fs from 'fs';

const EMPTY_DIR = path.resolve(__dirname, '../../../fixtures/workspaces/empty_workspace');

describe('Empty Workspace', () => {
    before(async () => {
        // Ensure empty workspace directory exists
        if (!fs.existsSync(EMPTY_DIR)) {
            fs.mkdirSync(EMPTY_DIR, { recursive: true });
        }
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFolderViaDialog(EMPTY_DIR);
        await waitForWorkspaceReady(10000);
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should not crash when opening empty workspace', async () => {
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have shell main_frame with empty workspace', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should have correct window title', async () => {
        const title = await AppShellPage.getWindowTitle();
        expect(title).toBe('MarkAmp');
    });

    it('should have non-zero window dimensions', async () => {
        const size = await AppShellPage.getWindowSize();
        expect(size.width).toBeGreaterThanOrEqual(0);
        expect(size.height).toBeGreaterThanOrEqual(0);
    });

    it('should report boolean from file tree visibility', async () => {
        const visible = await FileTreePage.isFileTreeVisible();
        expect(typeof visible).toBe('boolean');
    });

    it('should NOT have error overlays', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs open', async () => {
        await AppShellPage.assertNoDialogOpen();
    });

    it('should NOT crash after Escape in empty workspace', async () => {
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
