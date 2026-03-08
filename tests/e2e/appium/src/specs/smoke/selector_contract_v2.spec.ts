/**
 * @smoke Selector Contract Validation
 *
 * Tasks 20–22 (Phase 02): Validates the selector registry for
 * required presence, uniqueness, and prefix compliance.
 */

import {
    SELECTOR_REGISTRY,
    SURFACE_PREFIXES,
    findDuplicateSelectors,
    findDynamicSelectors,
    findUnregisteredPrefixes,
    getRequiredSelectors,
} from '../../support/selector_registry';
import * as contract from '../../support/selector_contract.snapshot.json';

describe('@smoke Selector Contract Validation', () => {
    // ── Task 20: Required Selector Presence ──

    it('should have all required selectors registered', () => {
        const requiredSelectors = getRequiredSelectors();
        expect(requiredSelectors.length).toBeGreaterThan(0);

        for (const selector of contract.requiredSelectors) {
            const found = requiredSelectors.includes(selector);
            expect(found).toBe(true);
        }
    });

    it('should have required selectors match the contract snapshot', () => {
        const requiredSelectors = getRequiredSelectors();
        for (const expected of contract.requiredSelectors) {
            expect(requiredSelectors).toContain(expected);
        }
    });

    // ── Task 21: No Duplicate Selectors ──

    it('should have no duplicate selectors in the registry', () => {
        const duplicates = findDuplicateSelectors();
        expect(duplicates).toEqual([]);
    });

    it('should have all registry selectors unique', () => {
        const allSelectors = SELECTOR_REGISTRY.map(e => e.selector);
        const uniqueSelectors = new Set(allSelectors);
        expect(uniqueSelectors.size).toBe(allSelectors.length);
    });

    // ── Task 22: Reserved Prefix Rules ──

    it('should have all selectors use registered surface prefixes', () => {
        const unregistered = findUnregisteredPrefixes();
        expect(unregistered).toEqual([]);
    });

    it('should have all selectors start with "ma."', () => {
        for (const entry of SELECTOR_REGISTRY) {
            expect(entry.selector.startsWith('ma.')).toBe(true);
        }
    });

    it('should have no dynamic selectors', () => {
        const dynamic = findDynamicSelectors();
        expect(dynamic).toEqual([]);
    });

    it('should have all surface prefixes defined', () => {
        expect(SURFACE_PREFIXES.length).toBeGreaterThan(0);
        for (const prefix of SURFACE_PREFIXES) {
            expect(prefix.startsWith('ma.')).toBe(true);
        }
    });

    it('should have contract snapshot match registered prefix count', () => {
        expect(contract.surfacePrefixes.length).toBe(SURFACE_PREFIXES.length);
    });

    it('should have contract snapshot contain all registered selectors', () => {
        const registrySelectors = SELECTOR_REGISTRY.map(e => e.selector);
        for (const selector of registrySelectors) {
            expect(contract.allSelectors).toContain(selector);
        }
    });
});
