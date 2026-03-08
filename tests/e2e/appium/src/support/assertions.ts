/**
 * Common assertion utilities for E2E tests.
 */

import { expect } from 'expect-webdriverio';

/**
 * Assert that an element with the given accessibility name exists.
 */
export async function assertElementExists(
    accessibilityName: string,
    timeoutMs = 10000
): Promise<void> {
    const element = await $(`~${accessibilityName}`);
    await expect(element).toBeExisting({ wait: timeoutMs });
}

/**
 * Assert that an element with the given accessibility name does NOT exist.
 */
export async function assertElementNotExists(
    accessibilityName: string
): Promise<void> {
    const element = await $(`~${accessibilityName}`);
    await expect(element).not.toBeExisting();
}
