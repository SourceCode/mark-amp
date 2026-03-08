/**
 * Failure artifact bundle — captures screenshots, page source, and
 * workspace snapshots when a test fails.
 */

import * as fs from 'fs';
import * as path from 'path';

const ARTIFACT_DIR = path.resolve(__dirname, '../../artifacts');

/**
 * Capture a complete failure artifact bundle.
 */
export async function captureFailureBundle(
    testName: string
): Promise<string> {
    const timestamp = new Date().toISOString().replace(/[:.]/g, '-');
    const bundleDir = path.join(ARTIFACT_DIR, `${testName}_${timestamp}`);
    fs.mkdirSync(bundleDir, { recursive: true });

    // 1. Screenshot
    try {
        await browser.saveScreenshot(path.join(bundleDir, 'screenshot.png'));
    } catch {
        fs.writeFileSync(path.join(bundleDir, 'screenshot_error.txt'), 'Failed to capture screenshot');
    }

    // 2. Page source (accessibility tree)
    try {
        const source = await browser.getPageSource();
        fs.writeFileSync(path.join(bundleDir, 'page_source.xml'), source);
    } catch {
        fs.writeFileSync(path.join(bundleDir, 'page_source_error.txt'), 'Failed to capture page source');
    }

    // 3. Browser logs
    try {
        const logs = await browser.getLogs('syslog');
        fs.writeFileSync(path.join(bundleDir, 'logs.json'), JSON.stringify(logs, null, 2));
    } catch {
        fs.writeFileSync(path.join(bundleDir, 'logs_error.txt'), 'Failed to capture logs');
    }

    return bundleDir;
}
