import { Theme } from '@/types/index';

import { createDefaultEditorTheme } from './utils';

export const seasonSpring: Theme = {
    colors: {
    accent: '#4ade80',
    border: 'rgba(34,197,94,0.35)',
    chromeBg: 'rgba(255,255,255,0.7)',
    chromeText: '#14532d',
    error: '#dc2626',
    primary: '#22c55e',
    primaryText: '#ffffff',
    secondary: '#bbf7d0',
    success: '#16a34a',
    surface: 'rgba(236,253,245,0.8)',
    surfaceHighlight: 'rgba(255,255,255,0.95)',
    text: '#14532d',
    textMuted: 'rgba(20,83,45,0.55)',
    wallpaper: 'radial-gradient(circle at 30% 30%, rgba(134,239,172,0.45) 0%, transparent 55%), linear-gradient(135deg, #f0fdf4 0%, #ecfeff 100%)',
    warning: '#f59e0b',
    windowBg: 'rgba(240, 253, 244, 0.85)',
    windowBorder: 'rgba(34,197,94,0.35)'
},
    description: 'Fresh greens, soft blossoms, and gentle renewal.',
    editor: createDefaultEditorTheme({
        background: 'rgba(240, 253, 244, 0.85)',
        caret: '#4ade80',
        comment: 'rgba(20,83,45,0.55)',
        gutter: 'rgba(240, 253, 244, 0.85)',
        keyword: '#22c55e',
        line: 'rgba(20,83,45,0.55)',
        number: '#dc2626',
        selection: 'rgba(255,255,255,0.95)',
        selectionText: '#14532d',
        string: '#16a34a',
        text: '#14532d'
}),
    elevation: {
        level1: '0px 1px 2px rgba(0, 0, 0, 0.3), 0px 1px 3px 1px rgba(0, 0, 0, 0.15)',
        level2: '0px 1px 2px rgba(0, 0, 0, 0.3), 0px 2px 6px 2px rgba(0, 0, 0, 0.15)',
        level3: '0px 1px 3px rgba(0, 0, 0, 0.3), 0px 4px 8px 3px rgba(0, 0, 0, 0.15)',
        level4: '0px 2px 3px rgba(0, 0, 0, 0.3), 0px 6px 10px 4px rgba(0, 0, 0, 0.15)',
        level5: '0px 4px 4px rgba(0, 0, 0, 0.3), 0px 8px 12px 6px rgba(0, 0, 0, 0.15)',
    },
    id: 'season-spring',
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
    name: 'Season · Spring',
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
        fontFamily: '"Inter", "Nunito", system-ui, sans-serif',
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
    borderWidth: '1px',
    controlsLayout: 'right',
    radius: '20px',
    shadow: '0 18px 40px rgba(34,197,94,0.25)',
    titleBarHeight: '42px'
},
};
