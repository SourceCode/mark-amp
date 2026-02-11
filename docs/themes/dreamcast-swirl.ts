import { Theme } from '@/types/index';

import { createDefaultEditorTheme } from './utils';

export const dreamcastSwirl: Theme = {
    colors: {
    accent: '#0ea5e9',
    border: 'rgba(17, 24, 39, 0.12)',
    chromeBg: 'linear-gradient(180deg, rgba(255,255,255,0.9) 0%, rgba(243,244,246,0.9) 100%)',
    chromeText: '#111827',
    error: '#dc2626',
    primary: '#f97316',
    primaryText: '#ffffff',
    secondary: '#e5e7eb',
    success: '#16a34a',
    surface: 'rgba(255, 255, 255, 0.75)',
    surfaceHighlight: 'rgba(255, 255, 255, 0.95)',
    text: '#111827',
    textMuted: 'rgba(17, 24, 39, 0.55)',
    wallpaper: 'radial-gradient(circle at 35% 35%, rgba(249,115,22,0.25) 0%, transparent 55%), radial-gradient(circle at 70% 70%, rgba(14,165,233,0.18) 0%, transparent 60%), linear-gradient(135deg, #ffffff 0%, #f3f4f6 55%, #e5e7eb 100%)',
    warning: '#f59e0b',
    windowBg: 'rgba(255, 255, 255, 0.82)',
    windowBorder: 'rgba(249, 115, 22, 0.45)'
},
    description: 'Clean white plastic, bold orange accents, and airy console UI nostalgia.',
    editor: createDefaultEditorTheme({
        background: 'rgba(255, 255, 255, 0.82)',
        caret: '#0ea5e9',
        comment: 'rgba(17, 24, 39, 0.55)',
        gutter: 'rgba(255, 255, 255, 0.82)',
        keyword: '#f97316',
        line: 'rgba(17, 24, 39, 0.55)',
        number: '#dc2626',
        selection: 'rgba(255, 255, 255, 0.95)',
        selectionText: '#111827',
        string: '#16a34a',
        text: '#111827'
}),
    elevation: {
        level1: '0px 1px 2px rgba(0, 0, 0, 0.3), 0px 1px 3px 1px rgba(0, 0, 0, 0.15)',
        level2: '0px 1px 2px rgba(0, 0, 0, 0.3), 0px 2px 6px 2px rgba(0, 0, 0, 0.15)',
        level3: '0px 1px 3px rgba(0, 0, 0, 0.3), 0px 4px 8px 3px rgba(0, 0, 0, 0.15)',
        level4: '0px 2px 3px rgba(0, 0, 0, 0.3), 0px 6px 10px 4px rgba(0, 0, 0, 0.15)',
        level5: '0px 4px 4px rgba(0, 0, 0, 0.3), 0px 8px 12px 6px rgba(0, 0, 0, 0.15)',
    },
    id: 'dreamcast-swirl',
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
    name: 'Dreamcast Swirl',
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
        fontFamily: '"Trebuchet MS", "Inter", system-ui, sans-serif',
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
    backdropBlur: '14px',
    borderWidth: '2px',
    controlsLayout: 'right',
    radius: '10px',
    shadow: '0 16px 34px rgba(0,0,0,0.18), inset 0 1px 0 rgba(255,255,255,0.9)',
    titleBarHeight: '34px'
},
};
