
import { createDefaultEditorTheme, getMotionVariables, getTypographyVariables } from './utils';
import { EditorTheme } from '@/types/index';

describe('Theme Utils', () => {
    describe('createDefaultEditorTheme', () => {
        const mockPalette = {
            background: '#000000',
            caret: '#ffffff',
            comment: '#666666',
            gutter: '#222222',
            keyword: '#ff00ff',
            line: '#333333',
            number: '#00ffff',
            selection: '#444444',
            selectionText: '#ffffff',
            string: '#00ff00',
            text: '#dddddd',
        };

        const theme: EditorTheme = createDefaultEditorTheme(mockPalette);

        it('maps general colors correctly', () => {
            expect(theme.general.background).toBe(mockPalette.background);
            expect(theme.general.caret).toBe(mockPalette.caret);
            expect(theme.general.text).toBe(mockPalette.text);
            expect(theme.general.selectionBackground).toBe(mockPalette.selection);
            expect(theme.general.selectionText).toBe(mockPalette.selectionText);
            expect(theme.general.gutterIcon).toBe(mockPalette.line);
        });

        it('maps language default colors correctly', () => {
            expect(theme.languageDefaults.keyword).toBe(mockPalette.keyword);
            expect(theme.languageDefaults.string).toBe(mockPalette.string);
            expect(theme.languageDefaults.number).toBe(mockPalette.number);
            expect(theme.languageDefaults.commentLine).toBe(mockPalette.comment);
            expect(theme.languageDefaults.operator).toBe(mockPalette.text);
        });

        it('generates specific language tokens', () => {
            // Check JavaScript/TypeScript specific mapping
            expect(theme.languages.javascript.functionDeclaration).toBe(mockPalette.text);
            expect(theme.languages.typescript.interface).toBe(mockPalette.text);

            // Check Python specific mapping
            expect(theme.languages.python.class).toBe(mockPalette.text);
            expect(theme.languages.python.decorator).toBe(mockPalette.keyword);

            // Check SQL specific mapping
            expect(theme.languages.sql.table).toBe(mockPalette.text);
            expect(theme.languages.sql.keyword).toBe(mockPalette.keyword);
        });

        it('generates consistent JSON colors', () => {
            expect(theme.languages.json.propertyKey).toBe(mockPalette.keyword);
            expect(theme.languages.json.string).toBe(mockPalette.string);
        });
    });

    describe('getMotionVariables', () => {
        it('returns empty object if motion is undefined', () => {
            // eslint-disable-next-line @typescript-eslint/no-explicit-any
            const theme: any = {};
            const vars = getMotionVariables(theme);
            expect(vars).toEqual({});
        });

        it('maps duration and easing correctly', () => {
            // eslint-disable-next-line @typescript-eslint/no-explicit-any
            const theme: any = {
                motion: {
                    duration: {
                        short: '100ms',
                        medium: '300ms',
                        long: '500ms'
                    },
                    easing: {
                        standard: 'ease-in-out',
                        emphasized: 'cubic-bezier(0.2, 0.0, 0, 1.0)'
                    }
                }
            };
            const vars = getMotionVariables(theme) as Record<string, string | number>;

            // Check Durations
            expect(vars['--os-duration-short']).toBe('100ms');
            expect(vars['--os-duration-medium']).toBe('300ms');
            expect(vars['--os-duration-long']).toBe('500ms');

            // Check Easings
            expect(vars['--os-easing-standard']).toBe('ease-in-out');
            expect(vars['--os-easing-emphasized']).toBe('cubic-bezier(0.2, 0.0, 0, 1.0)');
        });
    });

    describe('getTypographyVariables', () => {
        it('maps typography styles to css variables', () => {
            // eslint-disable-next-line @typescript-eslint/no-explicit-any
            const theme: any = {
                typography: {
                    displayLarge: {
                        size: '57px',
                        lineHeight: '64px',
                        weight: '400',
                        tracking: '-0.25px'
                    },
                    bodyMedium: {
                        size: '14px',
                        lineHeight: '20px',
                        weight: '400',
                        tracking: '0.25px'
                    }
                }
            };

            const vars = getTypographyVariables(theme) as Record<string, string | number>;

            // Display Large
            expect(vars['--os-type-display-large-size']).toBe('57px');
            expect(vars['--os-type-display-large-line-height']).toBe('64px');
            expect(vars['--os-type-display-large-weight']).toBe('400');
            expect(vars['--os-type-display-large-tracking']).toBe('-0.25px');

            // Body Medium
            expect(vars['--os-type-body-medium-size']).toBe('14px');
            expect(vars['--os-type-body-medium-line-height']).toBe('20px');
            expect(vars['--os-type-body-medium-weight']).toBe('400');
            expect(vars['--os-type-body-medium-tracking']).toBe('0.25px');
        });

        it('ignores missing typography keys', () => {
            // eslint-disable-next-line @typescript-eslint/no-explicit-any
            const theme: any = {
                typography: {
                    // Only displayLarge present
                    displayLarge: {
                        size: '57px',
                        lineHeight: '64px',
                        weight: '400',
                        tracking: '-0.25px'
                    }
                }
            };

            const vars = getTypographyVariables(theme) as Record<string, string | number>;
            // specific check for missing key
            expect(vars['--os-type-body-medium-size']).toBeUndefined();
        });
    });
});
