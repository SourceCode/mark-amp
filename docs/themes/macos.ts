import { Theme } from '@/types/index';

import { createDefaultEditorTheme } from './utils';

export const macos: Theme = {
    colors: {
    accent: '#5AC8FA',
    border: 'rgba(0, 0, 0, 0.1)',
    chromeBg: 'transparent',
    chromeText: '#333333',
    error: '#FF3B30',
    primary: '#007AFF',
    primaryText: '#ffffff',
    secondary: '#e5e5e5',
    success: '#34C759',
    surface: 'rgba(255, 255, 255, 0.5)',
    surfaceHighlight: 'rgba(255, 255, 255, 0.8)',
    text: '#1a1a1a',
    textMuted: '#666666',
    wallpaper: 'linear-gradient(to bottom, #d53369 0%, #daae51 100%)',
    warning: '#FF9500',
    windowBg: 'rgba(255, 255, 255, 0.65)',
    windowBorder: 'rgba(255, 255, 255, 0.4)'
},
    description: 'Clean, frosted glass, and minimalist.',
    editor: createDefaultEditorTheme({
        background: 'rgba(255, 255, 255, 0.65)',
        caret: '#5AC8FA',
        comment: '#666666',
        gutter: 'rgba(255, 255, 255, 0.65)',
        keyword: '#007AFF',
        line: '#666666',
        number: '#FF3B30',
        selection: 'rgba(255, 255, 255, 0.8)',
        selectionText: '#1a1a1a',
        string: '#34C759',
        text: '#1a1a1a'
}),
    elevation: {
        level1: '0px 1px 2px rgba(0, 0, 0, 0.3), 0px 1px 3px 1px rgba(0, 0, 0, 0.15)',
        level2: '0px 1px 2px rgba(0, 0, 0, 0.3), 0px 2px 6px 2px rgba(0, 0, 0, 0.15)',
        level3: '0px 1px 3px rgba(0, 0, 0, 0.3), 0px 4px 8px 3px rgba(0, 0, 0, 0.15)',
        level4: '0px 2px 3px rgba(0, 0, 0, 0.3), 0px 6px 10px 4px rgba(0, 0, 0, 0.15)',
        level5: '0px 4px 4px rgba(0, 0, 0, 0.3), 0px 8px 12px 6px rgba(0, 0, 0, 0.15)',
    },
    id: 'macos',
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
    name: 'Monterey',
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
        fontFamily: '-apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, Helvetica, Arial, sans-serif',
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
    controlsLayout: 'left',
    radius: '18px',
    shadow: '0 20px 40px rgba(0,0,0,0.15)',
    titleBarHeight: '38px'
},
};
