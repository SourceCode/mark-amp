import { Theme } from '@/types/index';

import { createDefaultEditorTheme } from './utils';

export const gamecubePurple: Theme = {
    colors: {
    accent: '#6366f1',
    border: 'rgba(168, 85, 247, 0.45)',
    chromeBg: 'linear-gradient(180deg, rgba(32,18,64,0.95) 0%, rgba(18,7,31,0.95) 100%)',
    chromeText: '#ede9fe',
    error: '#ef4444',
    primary: '#a855f7',
    primaryText: '#12071f',
    secondary: '#2e1065',
    success: '#22c55e',
    surface: 'rgba(32, 18, 64, 0.85)',
    surfaceHighlight: 'rgba(168, 85, 247, 0.18)',
    text: '#ede9fe',
    textMuted: 'rgba(237, 233, 254, 0.55)',
    wallpaper: 'radial-gradient(circle at 30% 25%, rgba(168,85,247,0.35) 0%, transparent 55%), radial-gradient(circle at 75% 70%, rgba(99,102,241,0.3) 0%, transparent 60%), linear-gradient(135deg, #12071f 0%, #1a0b2e 45%, #0b0416 100%)',
    warning: '#facc15',
    windowBg: 'rgba(18, 7, 31, 0.9)',
    windowBorder: 'rgba(168, 85, 247, 0.45)'
},
    description: 'Translucent indigo plastic, glowing highlights, and cube-era Nintendo futurism.',
    editor: createDefaultEditorTheme({
        background: 'rgba(18, 7, 31, 0.9)',
        caret: '#6366f1',
        comment: 'rgba(237, 233, 254, 0.55)',
        gutter: 'rgba(18, 7, 31, 0.9)',
        keyword: '#a855f7',
        line: 'rgba(237, 233, 254, 0.55)',
        number: '#ef4444',
        selection: 'rgba(168, 85, 247, 0.18)',
        selectionText: '#ede9fe',
        string: '#22c55e',
        text: '#ede9fe'
}),
    elevation: {
        level1: '0px 1px 2px rgba(0, 0, 0, 0.3), 0px 1px 3px 1px rgba(0, 0, 0, 0.15)',
        level2: '0px 1px 2px rgba(0, 0, 0, 0.3), 0px 2px 6px 2px rgba(0, 0, 0, 0.15)',
        level3: '0px 1px 3px rgba(0, 0, 0, 0.3), 0px 4px 8px 3px rgba(0, 0, 0, 0.15)',
        level4: '0px 2px 3px rgba(0, 0, 0, 0.3), 0px 6px 10px 4px rgba(0, 0, 0, 0.15)',
        level5: '0px 4px 4px rgba(0, 0, 0, 0.3), 0px 8px 12px 6px rgba(0, 0, 0, 0.15)',
    },
    id: 'gamecube-purple',
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
    name: 'GameCube Purple',
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
        fontFamily: '"Inter", "Trebuchet MS", system-ui, sans-serif',
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
    radius: '12px',
    shadow: '0 22px 48px rgba(18,7,31,0.9), inset 0 1px 0 rgba(255,255,255,0.08)',
    titleBarHeight: '36px'
},
};
