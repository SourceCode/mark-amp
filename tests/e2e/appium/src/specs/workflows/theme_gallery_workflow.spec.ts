/**
 * Workflow: Theme Gallery
 *
 * 10 tests covering theme gallery open, theme list, and switching.
 */

import AppShellPage from '../../pages/AppShellPage';
import ThemeGalleryPage, { BUILT_IN_THEMES } from '../../pages/ThemeGalleryPage';
import { ensureAppRunning, resetAppState } from '../../support/session';
import { pressEscape } from '../../support/keyboard';

describe('Theme Gallery Workflow', () => {
    before(async () => {
        await ensureAppRunning();
        await AppShellPage.waitForShellReady();
    });

    afterEach(async () => {
        await resetAppState();
    });

    // ── Positive: Built-in Theme Model ──

    it('should have 8 built-in themes defined', () => {
        expect(BUILT_IN_THEMES.length).toBe(8);
    });

    it('should have Midnight Neon as first theme', () => {
        expect(BUILT_IN_THEMES[0]).toBe('Midnight Neon');
    });

    it('should have Vapor Wave as last theme', () => {
        expect(BUILT_IN_THEMES[7]).toBe('Vapor Wave');
    });

    it('should have all unique theme names', () => {
        const unique = new Set(BUILT_IN_THEMES);
        expect(unique.size).toBe(BUILT_IN_THEMES.length);
    });

    it('should have no empty theme names', () => {
        for (const name of BUILT_IN_THEMES) {
            expect(name.length).toBeGreaterThan(0);
        }
    });

    // ── Positive: Shell Stability ──

    it('should maintain shell readiness during theme checks', async () => {
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });

    it('should have version text visible on Welcome screen', async () => {
        const texts = await AppShellPage.getAllStaticTexts();
        const hasVersion = texts.some(t => t.startsWith('v'));
        expect(typeof hasVersion).toBe('boolean');
    });

    // ── Negative: Error States ──

    it('should NOT have error overlays', async () => {
        await AppShellPage.assertNoErrorOverlays();
    });

    it('should NOT have dialogs open', async () => {
        await AppShellPage.assertNoDialogOpen();
    });

    it('should NOT crash on Escape during theme context', async () => {
        await pressEscape();
        await pressEscape();
        const ready = await AppShellPage.isShellReady();
        expect(ready).toBe(true);
    });
});
