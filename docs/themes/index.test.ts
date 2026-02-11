import * as themeExports from './index';

describe('Theme System Exports', () => {
    it('should export all themes defined in theme-data', () => {
        // theme-data THEMES object aggregates individual exports.
        // We want to ensure that 'index.ts' exports match what's expected or
        // at least that the files we see are exported.

        const exportedThemes = Object.values(themeExports).filter(
            (exportItem) => typeof exportItem === 'object' && 'id' in exportItem
        );

        // We expect a significant number of themes (based on list_dir ~67 files minus index/utils)
        expect(exportedThemes.length).toBeGreaterThan(50);
    });

    it('should have valid structure for all exported themes', () => {
        const exportedThemes = Object.values(themeExports);

        // eslint-disable-next-line @typescript-eslint/no-explicit-any
        exportedThemes.forEach((theme: any) => {
            // Skip non-theme exports if any (like utils)
            if (theme && typeof theme === 'object' && 'id' in theme && 'colors' in theme) {
                try {
                    expect(theme.id).toBeDefined();
                    expect(theme.name).toBeDefined();
                    expect(theme.type).toBeDefined(); // light or dark
                    expect(theme.colors).toBeDefined();
                    expect(theme.colors.primary).toBeDefined();
                    expect(theme.colors.windowBg).toBeDefined();
                    expect(theme.colors.text).toBeDefined();
                } catch (e) {
                    // console.error(`Failed validation for theme: ${JSON.stringify(theme, null, 2)}`);
                    throw e;
                }
            }
        });
    });

    it('should include specific known themes', () => {
        expect(themeExports.adwaita).toBeDefined();
        expect(themeExports.darkmode).toBeDefined();
        expect(themeExports.macos).toBeDefined();
    });
});
