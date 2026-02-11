import { Theme } from '@/types/index';

import { createDefaultEditorTheme } from './utils';

export const surrealDreamlogic: Theme = {
    colors: {
    accent: '#22d3ee',
    border: 'rgba(250, 204, 21, 0.45)',
    chromeBg: 'linear-gradient(90deg, rgba(168,85,247,0.35), rgba(34,211,238,0.35), rgba(250,204,21,0.35))',
    chromeText: '#fefce8',
    error: '#fb7185',
    primary: '#a855f7',
    primaryText: '#0b0014',
    secondary: '#1f2937',
    success: '#4ade80',
    surface: 'rgba(31, 41, 55, 0.75)',
    surfaceHighlight: 'rgba(250, 204, 21, 0.22)',
    text: '#fefce8',
    textMuted: 'rgba(254, 252, 232, 0.55)',
    wallpaper: 'radial-gradient(circle at 12% 18%, rgba(168,85,247,0.45) 0%, transparent 50%), radial-gradient(circle at 82% 24%, rgba(34,211,238,0.35) 0%, transparent 55%), radial-gradient(circle at 48% 86%, rgba(250,204,21,0.25) 0%, transparent 60%), linear-gradient(135deg, #0b0014 0%, #12001f 40%, #00141a 100%)',
    warning: '#fde047',
    windowBg: 'rgba(11, 0, 20, 0.88)',
    windowBorder: 'rgba(250, 204, 21, 0.45)'
},
    description: 'Unsettling pastels, impossible gradients, and liminal UI space.',
    editor: createDefaultEditorTheme({
        background: 'rgba(11, 0, 20, 0.88)',
        caret: '#22d3ee',
        comment: 'rgba(254, 252, 232, 0.55)',
        gutter: 'rgba(11, 0, 20, 0.88)',
        keyword: '#a855f7',
        line: 'rgba(254, 252, 232, 0.55)',
        number: '#fb7185',
        selection: 'rgba(250, 204, 21, 0.22)',
        selectionText: '#fefce8',
        string: '#4ade80',
        text: '#fefce8'
}),
    elevation: {
        level1: '0px 1px 2px rgba(0, 0, 0, 0.3), 0px 1px 3px 1px rgba(0, 0, 0, 0.15)',
        level2: '0px 1px 2px rgba(0, 0, 0, 0.3), 0px 2px 6px 2px rgba(0, 0, 0, 0.15)',
        level3: '0px 1px 3px rgba(0, 0, 0, 0.3), 0px 4px 8px 3px rgba(0, 0, 0, 0.15)',
        level4: '0px 2px 3px rgba(0, 0, 0, 0.3), 0px 6px 10px 4px rgba(0, 0, 0, 0.15)',
        level5: '0px 4px 4px rgba(0, 0, 0, 0.3), 0px 8px 12px 6px rgba(0, 0, 0, 0.15)',
    },
    id: 'surreal-dreamlogic',
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
    name: 'Dream Logic',
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
        fontFamily: '"Syne", "Inter", system-ui, sans-serif',
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
    borderWidth: '3px',
    controlsLayout: 'right',
    radius: '28px',
    shadow: '0 30px 70px rgba(168,85,247,0.45)',
    titleBarHeight: '46px'
},
};
