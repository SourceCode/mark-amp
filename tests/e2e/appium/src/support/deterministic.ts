/**
 * @file deterministic.ts
 *
 * Deterministic behavior utilities for E2E tests.
 * Tasks 36–38 (Phase 02): Deterministic clock, random-seed, and network stubs.
 */

// ── Task 36: Deterministic Clock Abstraction ──

/**
 * A deterministic clock that provides fixed or controllable timestamps.
 * Used for tests that depend on time-sensitive flows (e.g., flashcard scheduling,
 * sync intervals, autosave timing).
 */
export class DeterministicClock {
    private currentTime: number;

    constructor(startTime: number = Date.now()) {
        this.currentTime = startTime;
    }

    /** Get the current deterministic timestamp */
    now(): number {
        return this.currentTime;
    }

    /** Get the current deterministic date */
    date(): Date {
        return new Date(this.currentTime);
    }

    /** Advance time by the specified milliseconds */
    advance(ms: number): void {
        this.currentTime += ms;
    }

    /** Advance time by the specified seconds */
    advanceSeconds(seconds: number): void {
        this.currentTime += seconds * 1000;
    }

    /** Advance time by the specified minutes */
    advanceMinutes(minutes: number): void {
        this.currentTime += minutes * 60 * 1000;
    }

    /** Advance time by the specified hours */
    advanceHours(hours: number): void {
        this.currentTime += hours * 60 * 60 * 1000;
    }

    /** Advance time by the specified days */
    advanceDays(days: number): void {
        this.currentTime += days * 24 * 60 * 60 * 1000;
    }

    /** Reset clock to a specific time */
    reset(time: number): void {
        this.currentTime = time;
    }

    /** Get ISO string of current deterministic time */
    toISOString(): string {
        return new Date(this.currentTime).toISOString();
    }
}

// ── Task 37: Deterministic Random-Seed Support ──

/**
 * A seeded pseudo-random number generator for deterministic UI behavior.
 * Uses a simple linear congruential generator (LCG).
 */
export class SeededRandom {
    private seed: number;

    constructor(seed: number = 42) {
        this.seed = seed;
    }

    /** Generate next pseudo-random number in [0, 1) */
    next(): number {
        // LCG parameters (Numerical Recipes)
        this.seed = (this.seed * 1664525 + 1013904223) & 0x7fffffff;
        return this.seed / 0x7fffffff;
    }

    /** Generate a random integer in [min, max] */
    nextInt(min: number, max: number): number {
        return Math.floor(this.next() * (max - min + 1)) + min;
    }

    /** Pick a random element from an array */
    pick<T>(array: T[]): T {
        return array[this.nextInt(0, array.length - 1)];
    }

    /** Shuffle an array in place (Fisher-Yates) */
    shuffle<T>(array: T[]): T[] {
        const result = [...array];
        for (let i = result.length - 1; i > 0; i--) {
            const j = this.nextInt(0, i);
            [result[i], result[j]] = [result[j], result[i]];
        }
        return result;
    }

    /** Reset the generator to a specific seed */
    reset(seed: number): void {
        this.seed = seed;
    }
}

// ── Task 38: Deterministic Network Stubs ──

/**
 * Configuration for network stub behavior.
 * Used when optional online features (sync, AI, marketplace) need
 * to behave deterministically in tests.
 */
export interface NetworkStubConfig {
    /** Whether sync requests should succeed or fail */
    syncBehavior: 'success' | 'failure' | 'timeout';
    /** Whether AI provider requests should succeed or fail */
    aiBehavior: 'success' | 'failure' | 'timeout' | 'rate-limited';
    /** Whether marketplace requests should succeed or fail */
    marketplaceBehavior: 'success' | 'failure' | 'timeout';
    /** Simulated response delay in milliseconds */
    responseDelay: number;
}

export const DEFAULT_NETWORK_STUBS: NetworkStubConfig = {
    syncBehavior: 'success',
    aiBehavior: 'success',
    marketplaceBehavior: 'success',
    responseDelay: 100,
};

/**
 * Create a network stub configuration for testing specific failure modes.
 */
export function createNetworkStub(
    overrides: Partial<NetworkStubConfig> = {}
): NetworkStubConfig {
    return { ...DEFAULT_NETWORK_STUBS, ...overrides };
}

/**
 * Simulate network behavior by adding controlled delays.
 * Use in tests where network timing affects UI behavior.
 */
export async function simulateNetworkDelay(config: NetworkStubConfig): Promise<void> {
    if (config.responseDelay > 0) {
        await new Promise(resolve => setTimeout(resolve, config.responseDelay));
    }
}

// ── Default Instances ──

/** Default deterministic clock starting at a fixed epoch */
export const defaultClock = new DeterministicClock(
    new Date('2026-01-01T00:00:00Z').getTime()
);

/** Default seeded random with seed 42 */
export const defaultRandom = new SeededRandom(42);
