/**
 * Standardized explicit wait helpers for E2E tests.
 */

/**
 * Wait for an element by accessibility name with a custom timeout.
 */
export async function waitForAccessibilityElement(
    accessibilityName: string,
    timeoutMs = 10000
): Promise<WebdriverIO.Element> {
    const element = await $(`~${accessibilityName}`);
    await element.waitForExist({ timeout: timeoutMs });
    return element;
}

/**
 * Wait for an element to disappear.
 */
export async function waitForElementGone(
    accessibilityName: string,
    timeoutMs = 5000
): Promise<void> {
    const element = await $(`~${accessibilityName}`);
    await browser.waitUntil(
        async () => !(await element.isExisting()),
        { timeout: timeoutMs, timeoutMsg: `Element ~${accessibilityName} still exists after ${timeoutMs}ms` }
    );
}

/**
 * Wait for app to be idle (no animations/loading).
 */
export async function waitForIdle(delayMs = 500): Promise<void> {
    await browser.pause(delayMs);
}
