/**
 * accessibility_tree.ts — Helpers to dump, query, and audit
 * the macOS accessibility tree via the mac2 driver.
 *
 * XCUITest/mac2 attributes:
 *   - identifier  → NSAccessibility identifier (set via our bridge)
 *   - label       → NSAccessibility label
 *   - title       → window/element title
 *   - value       → text content
 *   - elementType → XCUIElementType numeric code
 */

/** Represents a node in the accessibility tree */
interface AccessibilityNode {
    role: string;
    name: string;
    value: string;
    label: string;
    enabled: boolean;
    children: AccessibilityNode[];
}

/**
 * Get the page source XML (accessibility tree dump).
 */
export async function getAccessibilityTree(): Promise<string> {
    return browser.getPageSource();
}

/**
 * Find all elements matching an accessibility identifier pattern via predicate.
 */
export async function findByIdentifierPattern(
    pattern: string
): Promise<WebdriverIO.ElementArray> {
    return $$(`-ios predicate string:identifier CONTAINS "${pattern}"`);
}

/**
 * Find a single element by exact accessibility identifier.
 */
export async function findByExactName(
    name: string
): Promise<WebdriverIO.Element> {
    return $(`~${name}`);
}

/**
 * Count elements with a specific elementType in the accessibility tree.
 * Uses the page source XML for efficient counting.
 */
export async function countElementsByType(elementType: string): Promise<number> {
    const source = await getAccessibilityTree();
    const typePattern = new RegExp(`XCUIElementType${elementType}`, 'g');
    const matches = source.match(typePattern);
    return matches ? matches.length : 0;
}

/**
 * Assert that an element with the given accessibility identifier exists
 * and has the expected properties.
 */
export async function assertElementProperties(
    accessibilityName: string,
    expectedProps: {
        enabled?: boolean;
        visible?: boolean;
    } = {}
): Promise<void> {
    const element = await $(`~${accessibilityName}`);
    await expect(element).toBeExisting();

    if (expectedProps.enabled !== undefined) {
        const isEnabled = await element.isEnabled();
        expect(isEnabled).toBe(expectedProps.enabled);
    }

    if (expectedProps.visible !== undefined) {
        const isDisplayed = await element.isDisplayed();
        expect(isDisplayed).toBe(expectedProps.visible);
    }
}

/**
 * Get all discoverable accessibility identifiers in the current window.
 * Returns unique non-empty identifiers found via page source parsing.
 */
export async function getAllAccessibilityNames(): Promise<string[]> {
    const source = await getAccessibilityTree();
    // XCUITest XML uses identifier="..." attribute
    const identifierPattern = /identifier="([^"]+)"/g;
    const names = new Set<string>();
    let match: RegExpExecArray | null;

    while ((match = identifierPattern.exec(source)) !== null) {
        const val = match[1].trim();
        // Skip system identifiers (start with _) and empty strings
        if (val.length > 0 && !val.startsWith('_')) {
            names.add(val);
        }
    }

    return Array.from(names).sort();
}

/**
 * Assert that no element with the given name exists.
 * Used for negative assertions.
 */
export async function assertElementAbsent(
    accessibilityName: string
): Promise<void> {
    const element = await $(`~${accessibilityName}`);
    const exists = await element.isExisting();
    expect(exists).toBe(false);
}

/**
 * Find all `ma.*` contract selectors present in the running app.
 */
export async function findContractSelectors(): Promise<string[]> {
    const allNames = await getAllAccessibilityNames();
    return allNames.filter(name => name.startsWith('ma.'));
}
