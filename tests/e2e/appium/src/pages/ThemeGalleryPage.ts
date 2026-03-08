/**
 * ThemeGalleryPage — Page object for the theme gallery modal.
 *
 * The theme gallery shows miniature previews of all available themes.
 */

import { pressEscape } from '../support/keyboard';

/** Known built-in theme names */
export const BUILT_IN_THEMES = [
    'Midnight Neon',
    'Cyber Night',
    'Solarized Dark',
    'Classic Mono',
    'High Contrast Blue',
    'Matrix Core',
    'Classic Amp',
    'Vapor Wave',
] as const;

class ThemeGalleryPage {
    /**
     * Check if the theme gallery modal is open by looking for
     * theme name text in the accessibility tree.
     */
    async isOpen(): Promise<boolean> {
        const source = await browser.getPageSource();
        // Theme gallery should contain at least some theme names
        return BUILT_IN_THEMES.some(name => source.includes(name));
    }

    /**
     * Dismiss the theme gallery.
     */
    async dismiss(): Promise<void> {
        await pressEscape();
        await browser.pause(300);
    }

    /**
     * Get the built-in theme count.
     */
    getBuiltInThemeCount(): number {
        return BUILT_IN_THEMES.length;
    }

    /**
     * Check if a specific theme name is visible in the gallery.
     */
    async isThemeVisible(themeName: string): Promise<boolean> {
        const source = await browser.getPageSource();
        return source.includes(themeName);
    }
}

export default new ThemeGalleryPage();
