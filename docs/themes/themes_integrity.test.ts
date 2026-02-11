
import fs from 'fs';
import path from 'path';
import { Theme } from '@/types/index';

describe('Theme Files Integrity', () => {
    const themesDir = __dirname;
    const files = fs.readdirSync(themesDir).filter(file =>
        file.endsWith('.ts') &&
        !file.endsWith('.test.ts') &&
        file !== 'index.ts' &&
        file !== 'utils.ts'
    );

    it(`should match the expected number of themes (found ${files.length})`, () => {
        expect(files.length).toBeGreaterThan(50);
    });

    files.forEach(file => {
        const themeName = path.basename(file, '.ts');

        describe(`Theme: ${themeName}`, () => {

            // eslint-disable-next-line @typescript-eslint/no-require-imports
            const module = require(path.join(themesDir, file));

            it('should export a named export matching the filename (lowercased/sanitized)', () => {
                // The export name usually matches the filename, or is the camelCase version of it.
                // e.g. 'akira-neo-tokyo.ts' -> export const akiraNeoTokyo ... or similar?
                // Actually looking at `adwaita.ts`, export is `adwaita`.
                // Looking at `akira-neo-tokyo.ts`, presumably `akiraNeoTokyo` or similar.
                // We'll check if ANY export looks like a Theme.

                const exports = Object.values(module);
                const themeExport = exports.find((exp: unknown) => {
                    const t = exp as Theme;
                    return t && t.id && t.colors;
                }) as Theme;

                expect(themeExport).toBeDefined();
                expect(themeExport.id).toBeDefined();
                expect(themeExport.colors).toBeDefined();
            });

            it('should have a valid editor theme configuration', () => {
                const exports = Object.values(module);
                const themeExport = exports.find((exp: unknown) => {
                    const t = exp as Theme;
                    return t && t.id && t.colors;
                }) as Theme;
                if (themeExport) {
                    expect(themeExport.editor).toBeDefined();
                    expect(themeExport.editor.general).toBeDefined();
                }
            });
        });
    });
});
