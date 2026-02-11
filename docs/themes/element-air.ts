import { Theme } from '@/types/index';

import { createDefaultEditorTheme } from './utils';

export const elementAir: Theme = {
    colors: {
    accent: '#38bdf8',
    border: 'rgba(148,163,184,0.35)',
    chromeBg: 'rgba(255,255,255,0.6)',
    chromeText: '#0f172a',
    error: '#dc2626',
    primary: '#0ea5e9',
    primaryText: '#ffffff',
    secondary: '#e5e7eb',
    success: '#22c55e',
    surface: 'rgba(255, 255, 255, 0.7)',
    surfaceHighlight: 'rgba(255, 255, 255, 0.95)',
    text: '#0f172a',
    textMuted: 'rgba(15,23,42,0.55)',
    wallpaper: 'radial-gradient(circle at 30% 30%, rgba(186,230,253,0.6) 0%, transparent 55%), linear-gradient(135deg, #f8fafc 0%, #ffffff 100%)',
    warning: '#f59e0b',
    windowBg: 'rgba(255, 255, 255, 0.75)',
    windowBorder: 'rgba(148,163,184,0.35)'
},
    description: 'Lightness, elevation, and whisper-soft translucency.',
    editor: createDefaultEditorTheme({
        background: 'rgba(255, 255, 255, 0.75)',
        caret: '#38bdf8',
        comment: 'rgba(15,23,42,0.55)',
        gutter: 'rgba(255, 255, 255, 0.75)',
        keyword: '#0ea5e9',
        line: 'rgba(15,23,42,0.55)',
        number: '#dc2626',
        selection: 'rgba(255, 255, 255, 0.95)',
        selectionText: '#0f172a',
        string: '#22c55e',
        text: '#0f172a'
}),
    elevation: {
        level1: '0px 1px 2px rgba(0, 0, 0, 0.3), 0px 1px 3px 1px rgba(0, 0, 0, 0.15)',
        level2: '0px 1px 2px rgba(0, 0, 0, 0.3), 0px 2px 6px 2px rgba(0, 0, 0, 0.15)',
        level3: '0px 1px 3px rgba(0, 0, 0, 0.3), 0px 4px 8px 3px rgba(0, 0, 0, 0.15)',
        level4: '0px 2px 3px rgba(0, 0, 0, 0.3), 0px 6px 10px 4px rgba(0, 0, 0, 0.15)',
        level5: '0px 4px 4px rgba(0, 0, 0, 0.3), 0px 8px 12px 6px rgba(0, 0, 0, 0.15)',
    },
    id: 'element-air',
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
    name: 'Element · Air',
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
        fontFamily: '"Inter", "SF Pro Display", system-ui, sans-serif',
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
    backdropBlur: '22px',
    borderWidth: '1px',
    controlsLayout: 'right',
    radius: '22px',
    shadow: '0 20px 45px rgba(15,23,42,0.18)',
    titleBarHeight: '42px'
},
};
