/**
 * Session management helpers for Appium mac2 E2E tests.
 *
 * Provides before/after hooks for test suites that need
 * deterministic session setup.
 */

/**
 * Ensure MarkAmp is running in E2E mode.
 * Call from a `before` hook.
 */
export async function ensureAppRunning(): Promise<void> {
    // Wait for the app to be available
    await browser.waitUntil(
        async () => {
            try {
                const status = await browser.status();
                return status.ready;
            } catch {
                return false;
            }
        },
        { timeout: 30000, timeoutMsg: 'MarkAmp app did not start within 30s' }
    );
}

/**
 * Reset app state between tests.
 * Avoids a full session restart by using mac2's reset capabilities.
 */
export async function resetAppState(): Promise<void> {
    // Bring app to known state — close any open dialogs
    try {
        // XCUITest key code for Escape (Unicode private use area)
        await browser.execute('macos: keys', {
            keys: [{ key: '\uE00C' }],
        });
    } catch {
        // Not critical if Escape key fails
    }
}
