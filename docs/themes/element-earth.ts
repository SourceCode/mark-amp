import { Theme } from '@/types/index';

import { createDefaultEditorTheme } from './utils';

export const elementEarth: Theme = {
    colors: {
    accent: '#a8a29e',
    border: '#78716c',
    chromeBg: 'linear-gradient(180deg, #292524 0%, #1c1917 100%)',
    chromeText: '#e7e5e4',
    error: '#dc2626',
    primary: '#78716c',
    primaryText: '#020617',
    secondary: '#44403c',
    success: '#65a30d',
    surface: 'rgba(68, 64, 60, 0.85)',
    surfaceHighlight: 'rgba(168, 162, 158, 0.2)',
    text: '#e7e5e4',
    textMuted: 'rgba(231,229,228,0.55)',
    wallpaper: 'radial-gradient(circle at 30% 70%, rgba(120,113,108,0.45) 0%, transparent 60%), linear-gradient(135deg, #020617 0%, #1c1917 100%)',
    warning: '#d97706',
    windowBg: 'rgba(28, 25, 23, 0.9)',
    windowBorder: '#78716c'
},
    description: 'Grounded minerals, soil tones, and carved stone depth.',
    editor: createDefaultEditorTheme({
        background: 'rgba(28, 25, 23, 0.9)',
        caret: '#a8a29e',
        comment: 'rgba(231,229,228,0.55)',
        gutter: 'rgba(28, 25, 23, 0.9)',
        keyword: '#78716c',
        line: 'rgba(231,229,228,0.55)',
        number: '#dc2626',
        selection: 'rgba(168, 162, 158, 0.2)',
        selectionText: '#e7e5e4',
        string: '#65a30d',
        text: '#e7e5e4'
}),
    elevation: {
        level1: '0px 1px 2px rgba(0, 0, 0, 0.3), 0px 1px 3px 1px rgba(0, 0, 0, 0.15)',
        level2: '0px 1px 2px rgba(0, 0, 0, 0.3), 0px 2px 6px 2px rgba(0, 0, 0, 0.15)',
        level3: '0px 1px 3px rgba(0, 0, 0, 0.3), 0px 4px 8px 3px rgba(0, 0, 0, 0.15)',
        level4: '0px 2px 3px rgba(0, 0, 0, 0.3), 0px 6px 10px 4px rgba(0, 0, 0, 0.15)',
        level5: '0px 4px 4px rgba(0, 0, 0, 0.3), 0px 8px 12px 6px rgba(0, 0, 0, 0.15)',
    },
    id: 'element-earth',
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
    name: 'Element · Earth',
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
        fontFamily: '"Inter", system-ui, sans-serif',
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
    backdropBlur: '8px',
    borderWidth: '2px',
    controlsLayout: 'right',
    radius: '8px',
    shadow: '0 26px 60px rgba(0,0,0,0.8)',
    titleBarHeight: '38px'
},
};
