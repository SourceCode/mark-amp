/**
 * Retry policy configuration for E2E tests.
 *
 * Test tiers:
 *   smoke     — 0 retries (must always pass)
 *   workflow  — 1 retry (allow recovery from timing issues)
 *   stress    — 2 retries (inherently flaky by nature)
 */

export interface RetryPolicy {
    maxRetries: number;
    retryDelay: number; // ms between retries
}

export const RETRY_POLICIES: Record<string, RetryPolicy> = {
    smoke: { maxRetries: 0, retryDelay: 0 },
    workflow: { maxRetries: 1, retryDelay: 1000 },
    stress: { maxRetries: 2, retryDelay: 2000 },
};

/**
 * Get the retry policy for a test tier.
 */
export function getRetryPolicy(tier: string): RetryPolicy {
    return RETRY_POLICIES[tier] ?? RETRY_POLICIES['smoke'];
}
