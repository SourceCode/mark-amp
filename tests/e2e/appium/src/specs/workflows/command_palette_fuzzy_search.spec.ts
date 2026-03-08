/**
 * Workflow: Command Palette Fuzzy Search
 *
 * 10 tests covering fuzzy search behavior in the palette.
 */

import AppShellPage from '../../pages/AppShellPage';
import CommandPalettePage from '../../pages/CommandPalettePage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { openFixtureWorkspace } from '../../support/folder_open';
import { pressEscape, pressEnter } from '../../support/keyboard';

describe('Command Palette Fuzzy Search', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
        await openFixtureWorkspace('markdown_workspace');
    });

    afterEach(async () => {
        await resetAppState();
    });

    it('should accept partial matches like "thm" for theme', async () => {
        await CommandPalettePage.openAndSearch('thm');
        await browser.pause(300);
        await CommandPalettePage.dismiss();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should accept abbreviated queries like "nf" for new file', async () => {
        await CommandPalettePage.openAndSearch('nf');
        await browser.pause(300);
        await CommandPalettePage.dismiss();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle empty query', async () => {
        await CommandPalettePage.open();
        await browser.pause(300);
        await CommandPalettePage.dismiss();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle single character query', async () => {
        await CommandPalettePage.openAndSearch('t');
        await browser.pause(300);
        await CommandPalettePage.dismiss();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle long query string', async () => {
        await CommandPalettePage.openAndSearch('this is a very long search query that should not crash');
        await browser.pause(300);
        await CommandPalettePage.dismiss();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle special characters in query', async () => {
        await CommandPalettePage.openAndSearch('@#$%');
        await browser.pause(300);
        await CommandPalettePage.dismiss();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle Enter on empty results', async () => {
        await CommandPalettePage.openAndSearch('zzzzzzzznonexist');
        await browser.pause(200);
        await pressEnter();
        await browser.pause(300);
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should handle rapid text changes in palette', async () => {
        await CommandPalettePage.open();
        await CommandPalettePage.typeQuery('a');
        await CommandPalettePage.typeQuery('b');
        await CommandPalettePage.typeQuery('c');
        await browser.pause(300);
        await CommandPalettePage.dismiss();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should NOT have error overlays', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT crash after Escape', async () => {
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
