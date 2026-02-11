import { Theme } from '@/types/index';

import { createDefaultEditorTheme } from './utils';

export const seasonAutumn: Theme = {
    colors: {
    accent: '#fb923c',
    border: 'rgba(249,115,22,0.45)',
    chromeBg: 'linear-gradient(180deg, #451a03 0%, #1c0f0a 100%)',
    chromeText: '#fff7ed',
    error: '#ef4444',
    primary: '#f97316',
    primaryText: '#020617',
    secondary: '#7c2d12',
    success: '#84cc16',
    surface: 'rgba(124,45,18,0.85)',
    surfaceHighlight: 'rgba(249,115,22,0.3)',
    text: '#fff7ed',
    textMuted: 'rgba(255,247,237,0.55)',
    wallpaper: 'radial-gradient(circle at 35% 65%, rgba(249,115,22,0.45) 0%, transparent 60%), linear-gradient(135deg, #020617 0%, #1c0f0a 100%)',
    warning: '#facc15',
    windowBg: 'rgba(28, 15, 10, 0.9)',
    windowBorder: 'rgba(249,115,22,0.45)'
},
    description: 'Falling leaves, amber light, and cozy earth tones.',
    editor: createDefaultEditorTheme({
        background: 'rgba(28, 15, 10, 0.9)',
        caret: '#fb923c',
        comment: 'rgba(255,247,237,0.55)',
        gutter: 'rgba(28, 15, 10, 0.9)',
        keyword: '#f97316',
        line: 'rgba(255,247,237,0.55)',
        number: '#ef4444',
        selection: 'rgba(249,115,22,0.3)',
        selectionText: '#fff7ed',
        string: '#84cc16',
        text: '#fff7ed'
}),
    elevation: {
        level1: '0px 1px 2px rgba(0, 0, 0, 0.3), 0px 1px 3px 1px rgba(0, 0, 0, 0.15)',
        level2: '0px 1px 2px rgba(0, 0, 0, 0.3), 0px 2px 6px 2px rgba(0, 0, 0, 0.15)',
        level3: '0px 1px 3px rgba(0, 0, 0, 0.3), 0px 4px 8px 3px rgba(0, 0, 0, 0.15)',
        level4: '0px 2px 3px rgba(0, 0, 0, 0.3), 0px 6px 10px 4px rgba(0, 0, 0, 0.15)',
        level5: '0px 4px 4px rgba(0, 0, 0, 0.3), 0px 8px 12px 6px rgba(0, 0, 0, 0.15)',
    },
    id: 'season-autumn',
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
    name: 'Season · Autumn',
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
        fontFamily: '"Inter", "Merriweather", system-ui, serif',
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
    backdropBlur: '10px',
    borderWidth: '2px',
    controlsLayout: 'right',
    radius: '10px',
    shadow: '0 28px 65px rgba(249,115,22,0.45)',
    titleBarHeight: '38px'
},
};
