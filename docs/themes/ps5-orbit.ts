import { Theme } from '@/types/index';

import { createDefaultEditorTheme } from './utils';

export const ps5Orbit: Theme = {
    colors: {
    accent: '#6366f1',
    border: 'rgba(56, 189, 248, 0.35)',
    chromeBg: 'rgba(10, 15, 44, 0.85)',
    chromeText: '#e5f6ff',
    error: '#ef4444',
    primary: '#38bdf8',
    primaryText: '#020617',
    secondary: '#0f172a',
    success: '#22c55e',
    surface: 'rgba(15, 23, 42, 0.85)',
    surfaceHighlight: 'rgba(56, 189, 248, 0.15)',
    text: '#e5f6ff',
    textMuted: 'rgba(229, 246, 255, 0.55)',
    wallpaper: 'radial-gradient(circle at 50% 0%, rgba(56,189,248,0.25) 0%, transparent 55%), radial-gradient(circle at 80% 80%, rgba(99,102,241,0.25) 0%, transparent 55%), linear-gradient(180deg, #050816 0%, #0a0f2c 45%, #020617 100%)',
    warning: '#facc15',
    windowBg: 'rgba(5, 8, 22, 0.88)',
    windowBorder: 'rgba(56, 189, 248, 0.35)'
},
    description: 'Deep space blacks, electric blues, and futuristic console UI.',
    editor: createDefaultEditorTheme({
        background: 'rgba(5, 8, 22, 0.88)',
        caret: '#6366f1',
        comment: 'rgba(229, 246, 255, 0.55)',
        gutter: 'rgba(5, 8, 22, 0.88)',
        keyword: '#38bdf8',
        line: 'rgba(229, 246, 255, 0.55)',
        number: '#ef4444',
        selection: 'rgba(56, 189, 248, 0.15)',
        selectionText: '#e5f6ff',
        string: '#22c55e',
        text: '#e5f6ff'
}),
    elevation: {
        level1: '0px 1px 2px rgba(0, 0, 0, 0.3), 0px 1px 3px 1px rgba(0, 0, 0, 0.15)',
        level2: '0px 1px 2px rgba(0, 0, 0, 0.3), 0px 2px 6px 2px rgba(0, 0, 0, 0.15)',
        level3: '0px 1px 3px rgba(0, 0, 0, 0.3), 0px 4px 8px 3px rgba(0, 0, 0, 0.15)',
        level4: '0px 2px 3px rgba(0, 0, 0, 0.3), 0px 6px 10px 4px rgba(0, 0, 0, 0.15)',
        level5: '0px 4px 4px rgba(0, 0, 0, 0.3), 0px 8px 12px 6px rgba(0, 0, 0, 0.15)',
    },
    id: 'ps5-orbit',
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
    name: 'PS5 Orbit',
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
    backdropBlur: '10px',
    borderWidth: '1px',
    controlsLayout: 'right',
    radius: '12px',
    shadow: '0 20px 50px rgba(2,6,23,0.8)',
    titleBarHeight: '40px'
},
};
