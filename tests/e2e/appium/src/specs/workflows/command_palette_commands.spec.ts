/**
 * Workflow: Command Palette Commands
 *
 * 10 tests covering specific command execution via the palette.
 */

import AppShellPage from '../../pages/AppShellPage';
import CommandPalettePage from '../../pages/CommandPalettePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { pressEscape } from '../../support/keyboard';

describe('Command Palette Commands', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should open palette and search for "theme"', async () => {
        await CommandPalettePage.openAndSearch('theme');
        await browser.pause(300);
        await CommandPalettePage.dismiss();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should open palette and search for "settings"', async () => {
        await CommandPalettePage.openAndSearch('settings');
        await browser.pause(300);
        await CommandPalettePage.dismiss();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should open palette and search for "new file"', async () => {
        await CommandPalettePage.openAndSearch('new file');
        await browser.pause(300);
        await CommandPalettePage.dismiss();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should open palette and search for "toggle sidebar"', async () => {
        await CommandPalettePage.openAndSearch('toggle sidebar');
        await browser.pause(300);
        await CommandPalettePage.dismiss();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should open palette and search for "zen"', async () => {
        await CommandPalettePage.openAndSearch('zen');
        await browser.pause(300);
        await CommandPalettePage.dismiss();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should open palette and search for non-existent command', async () => {
        await CommandPalettePage.openAndSearch('xyznonexistent');
        await browser.pause(300);
        await CommandPalettePage.dismiss();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should open palette → type → dismiss → reopen', async () => {
        await CommandPalettePage.openAndSearch('test');
        await CommandPalettePage.dismiss();
        await CommandPalettePage.open();
        await CommandPalettePage.dismiss();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should maintain shell during command palette ops', async () => {
        const shell = await browser.$('~ma.shell.main_frame');
        await expect(shell).toBeExisting();
    });

    it('should NOT have error overlays', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs', async () => {
        await pressEscape();
        await browser.pause(200);
        await AppShellPage.assertNoDialogOpen();
    });
});
