import type { Options } from '@wdio/types';
import * as path from 'path';
import * as fs from 'fs';

const capsPath = path.resolve(__dirname, 'config/caps.mac2.json');
const caps = JSON.parse(fs.readFileSync(capsPath, 'utf-8')) as Record<string, string | number | boolean | string[] | Record<string, string>>;

export const config: Options.Testrunner = {
    runner: 'local',
    autoCompileOpts: {
        tsNodeOpts: {
            project: './tsconfig.json',
        },
    },

    hostname: '127.0.0.1',
    port: 4723,
    path: '/',

    specs: ['./src/specs/**/*.spec.ts'],
    exclude: [],

    maxInstances: 1, // mac2 is single-instance

    capabilities: [
        {
            'platformName': 'mac',
            'appium:automationName': 'mac2',
            'appium:bundleId': caps['bundleId'] as string,
            'appium:noReset': caps['noReset'] as boolean,
            'appium:newCommandTimeout': caps['newCommandTimeout'] as number,
        },
    ],

    logLevel: 'warn',
    bail: 0,

    waitforTimeout: 10000,
    connectionRetryTimeout: 120000,
    connectionRetryCount: 3,

    services: [],

    framework: 'mocha',
    reporters: ['spec'],

    mochaOpts: {
        ui: 'bdd',
        timeout: 60000,
    },

    /**
     * Set up MARKAMP_E2E environment variable before launching
     */
    onPrepare: function () {
        process.env['MARKAMP_E2E'] = '1';
    },

    /**
     * Recover from dead sessions before each test.
     * The native macOS Open dialog (Cmd+O → Cmd+Shift+G) can
     * invalidate the Appium session. This hook detects that and
     * forces a session reload so subsequent specs can run.
     */
    beforeTest: async function () {
        try {
            await browser.status();
        } catch {
            // Session is dead — try to reload
            try {
                await browser.reloadSession();
                await browser.pause(1000);
            } catch {
                // Reload also failed — test will fail on its own
            }
        }
    },

    /**
     * Capture screenshot on failure
     */
    afterTest: async function (
        test: { title: string },
        _context: Record<string, string>,
        result: { passed: boolean }
    ) {
        if (!result.passed) {
            const timestamp = new Date().toISOString().replace(/[:.]/g, '-');
            const screenshotName = `failure_${test.title.replace(/\s+/g, '_')}_${timestamp}`;
            const artifactDir = path.resolve(__dirname, 'artifacts');

            if (!fs.existsSync(artifactDir)) {
                fs.mkdirSync(artifactDir, { recursive: true });
            }

            try {
                await browser.saveScreenshot(
                    path.join(artifactDir, `${screenshotName}.png`)
                );
            } catch {
                console.warn('Failed to capture failure screenshot');
            }
        }
    },
};
