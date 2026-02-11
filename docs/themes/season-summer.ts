import { Theme } from '@/types/index';

import { createDefaultEditorTheme } from './utils';

export const seasonSummer: Theme = {
    colors: {
    accent: '#38bdf8',
    border: 'rgba(250,204,21,0.45)',
    chromeBg: 'rgba(255,255,255,0.75)',
    chromeText: '#78350f',
    error: '#dc2626',
    primary: '#facc15',
    primaryText: '#78350f',
    secondary: '#fde68a',
    success: '#22c55e',
    surface: 'rgba(254,243,199,0.85)',
    surfaceHighlight: 'rgba(255,255,255,0.95)',
    text: '#78350f',
    textMuted: 'rgba(120,53,15,0.55)',
    wallpaper: 'radial-gradient(circle at 40% 20%, rgba(250,204,21,0.5) 0%, transparent 55%), linear-gradient(135deg, #fefce8 0%, #e0f2fe 100%)',
    warning: '#f97316',
    windowBg: 'rgba(255, 251, 235, 0.88)',
    windowBorder: 'rgba(250,204,21,0.45)'
},
    description: 'Bright sunlight, warm sand, and saturated sky blues.',
    editor: createDefaultEditorTheme({
        background: 'rgba(255, 251, 235, 0.88)',
        caret: '#38bdf8',
        comment: 'rgba(120,53,15,0.55)',
        gutter: 'rgba(255, 251, 235, 0.88)',
        keyword: '#facc15',
        line: 'rgba(120,53,15,0.55)',
        number: '#dc2626',
        selection: 'rgba(255,255,255,0.95)',
        selectionText: '#78350f',
        string: '#22c55e',
        text: '#78350f'
}),
    elevation: {
        level1: '0px 1px 2px rgba(0, 0, 0, 0.3), 0px 1px 3px 1px rgba(0, 0, 0, 0.15)',
        level2: '0px 1px 2px rgba(0, 0, 0, 0.3), 0px 2px 6px 2px rgba(0, 0, 0, 0.15)',
        level3: '0px 1px 3px rgba(0, 0, 0, 0.3), 0px 4px 8px 3px rgba(0, 0, 0, 0.15)',
        level4: '0px 2px 3px rgba(0, 0, 0, 0.3), 0px 6px 10px 4px rgba(0, 0, 0, 0.15)',
        level5: '0px 4px 4px rgba(0, 0, 0, 0.3), 0px 8px 12px 6px rgba(0, 0, 0, 0.15)',
    },
    id: 'season-summer',
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
    name: 'Season · Summer',
    states: {
        dragged: '0.16',
        focus: '0.12',
        hover: '0.08',
        pressed: '0.12',
    },
    type: 'light',
    
    typography: {
        bodyLarge: { lineHeight: '24px', size: '16px', tracking: '0.5px', weight: '400' },
        bodyMedium: { lineHeight: '20px', size: '14px', tracking: '0.25px', weight: '400' },
        bodySmall: { lineHeight: '16px', size: '12px', tracking: '0.4px', weight: '400' },
        displayLarge: { lineHeight: '64px', size: '57px', tracking: '-0.25px', weight: '400' },
        displayMedium: { lineHeight: '52px', size: '45px', tracking: '0px', weight: '400' },
        displaySmall: { lineHeight: '44px', size: '36px', tracking: '0px', weight: '400' },
        fontFamily: '"Inter", "Poppins", system-ui, sans-serif',
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
    backdropBlur: '18px',
    borderWidth: '1px',
    controlsLayout: 'right',
    radius: '18px',
    shadow: '0 20px 45px rgba(250,204,21,0.35)',
    titleBarHeight: '40px'
},
};
