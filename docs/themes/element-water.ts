import { Theme } from '@/types/index';

import { createDefaultEditorTheme } from './utils';

export const elementWater: Theme = {
    colors: {
    accent: '#22d3ee',
    border: 'rgba(56,189,248,0.55)',
    chromeBg: 'linear-gradient(180deg, rgba(14,165,233,0.25) 0%, rgba(2,6,23,0.95) 100%)',
    chromeText: '#e0f2fe',
    error: '#fb7185',
    primary: '#38bdf8',
    primaryText: '#020617',
    secondary: '#0f172a',
    success: '#22c55e',
    surface: 'rgba(15, 23, 42, 0.85)',
    surfaceHighlight: 'rgba(56,189,248,0.25)',
    text: '#e0f2fe',
    textMuted: 'rgba(224,242,254,0.55)',
    wallpaper: 'radial-gradient(circle at 40% 20%, rgba(56,189,248,0.45) 0%, transparent 55%), linear-gradient(135deg, #020617 0%, #020617 100%)',
    warning: '#facc15',
    windowBg: 'rgba(2, 6, 23, 0.82)',
    windowBorder: 'rgba(56,189,248,0.55)'
},
    description: 'Flowing translucency, cool blues, and liquid calm.',
    editor: createDefaultEditorTheme({
        background: 'rgba(2, 6, 23, 0.82)',
        caret: '#22d3ee',
        comment: 'rgba(224,242,254,0.55)',
        gutter: 'rgba(2, 6, 23, 0.82)',
        keyword: '#38bdf8',
        line: 'rgba(224,242,254,0.55)',
        number: '#fb7185',
        selection: 'rgba(56,189,248,0.25)',
        selectionText: '#e0f2fe',
        string: '#22c55e',
        text: '#e0f2fe'
}),
    elevation: {
        level1: '0px 1px 2px rgba(0, 0, 0, 0.3), 0px 1px 3px 1px rgba(0, 0, 0, 0.15)',
        level2: '0px 1px 2px rgba(0, 0, 0, 0.3), 0px 2px 6px 2px rgba(0, 0, 0, 0.15)',
        level3: '0px 1px 3px rgba(0, 0, 0, 0.3), 0px 4px 8px 3px rgba(0, 0, 0, 0.15)',
        level4: '0px 2px 3px rgba(0, 0, 0, 0.3), 0px 6px 10px 4px rgba(0, 0, 0, 0.15)',
        level5: '0px 4px 4px rgba(0, 0, 0, 0.3), 0px 8px 12px 6px rgba(0, 0, 0, 0.15)',
    },
    id: 'element-water',
    motion: {
        duration: {
            extraLong1: '700ms', extraLong2: '800ms', extraLong3: '900ms', extraLong4: '1000ms',
            long1: '450ms', long2: '500ms', long3: '550ms', long4: '600ms',
            medium1: '250ms', medium2: '300ms', medium3: '350ms', medium4: '400ms',
            short1: '50ms', short2: '100ms', short3: '150ms', short4: '200ms',
        },
        easing: {
            emphasized: 'cubic-bezier(0.2, 0, 0, 1)',
            emphasizedAccelerate: 'cubic-bezier(0.3, 0, 0.8, 0.15)',
            emphasizedDecelerate: 'cubic-bezier(0.05, 0.7, 0.1, 1)',
            linear: 'cubic-bezier(0, 0, 1, 1)',
            standard: 'cubic-bezier(0.2, 0, 0, 1)',
            standardAccelerate: 'cubic-bezier(0.3, 0, 1, 1)',
            standardDecelerate: 'cubic-bezier(0, 0, 0, 1)',
        },
    },
    name: 'Element · Water',
    states: {
        dragged: '0.16',
        focus: '0.12',
        hover: '0.08',
        pressed: '0.12',
    },
    type: 'dark',
    
    typography: {
        bodyLarge: { lineHeight: '24px', size: '16px', tracking: '0.5px', weight: '400' },
        bodyMedium: { lineHeight: '20px', size: '14px', tracking: '0.25px', weight: '400' },
        bodySmall: { lineHeight: '16px', size: '12px', tracking: '0.4px', weight: '400' },
        displayLarge: { lineHeight: '64px', size: '57px', tracking: '-0.25px', weight: '400' },
        displayMedium: { lineHeight: '52px', size: '45px', tracking: '0px', weight: '400' },
        displaySmall: { lineHeight: '44px', size: '36px', tracking: '0px', weight: '400' },
        fontFamily: '"Inter", "Manrope", system-ui, sans-serif',
        headlineLarge: { lineHeight: '40px', size: '32px', tracking: '0px', weight: '400' },
        headlineMedium: { lineHeight: '36px', size: '28px', tracking: '0px', weight: '400' },
        headlineSmall: { lineHeight: '32px', size: '24px', tracking: '0px', weight: '400' },
        labelLarge: { lineHeight: '20px', size: '14px', tracking: '0.1px', weight: '500' },
        labelMedium: { lineHeight: '16px', size: '12px', tracking: '0.5px', weight: '500' },
        labelSmall: { lineHeight: '16px', size: '11px', tracking: '0.5px', weight: '500' },
        titleLarge: { lineHeight: '28px', size: '22px', tracking: '0px', weight: '400' },
        titleMedium: { lineHeight: '24px', size: '16px', tracking: '0.15px', weight: '500' },
        titleSmall: { lineHeight: '20px', size: '14px', tracking: '0.1px', weight: '500' },
    },
    window: {
    backdropBlur: '20px',
    borderWidth: '2px',
    controlsLayout: 'right',
    radius: '18px',
    shadow: '0 30px 70px rgba(56,189,248,0.45)',
    titleBarHeight: '40px'
},
};
