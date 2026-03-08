# ADR-003: Page Object Standards

## Status

Accepted

## Date

2026-03-06

## Context

With 500+ E2E tests planned across 30+ product areas, element selectors and interaction patterns must be centralized to avoid duplication, simplify maintenance, and enable consistent patterns across tests.

### Options Considered

1. **Page Object Model (POM)** — One class per UI surface. Getters return element references. Methods encapsulate multi-step interactions.
2. **Screen Play Pattern** — Actor/task/question abstraction. Higher abstraction but steeper learning curve.
3. **Inline selectors** — No abstraction. Selectors embedded directly in spec files.

## Decision

Use the **Page Object Model** with the following conventions:

### Structure

- One file per UI surface: `<Surface>Page.ts` (e.g., `EditorPage.ts`, `CanvasPage.ts`)
- Located in `src/pages/`
- Exported as a singleton instance: `export default new EditorPage();`

### Element Getters

```typescript
get elementName(): ReturnType<WebdriverIO.Browser['$']> {
    return browser.$('~ma.<surface>.<control>');
}
```

- Use `~` prefix for accessibility identifier selectors.
- Use iOS predicate strings only when identifier is not available.
- Return type is always `ReturnType<WebdriverIO.Browser['$']>` — never `any`.

### Action Methods

```typescript
async performAction(): Promise<void> {
    const element = await this.elementName;
    await element.waitForExist({ timeout: 10000 });
    await element.click();
}
```

- All methods are `async` returning `Promise<void>` or `Promise<T>`.
- Methods include explicit waits — never assume element is immediately available.
- Methods handle common recovery (dismiss dialogs, retry on stale element).

### Guard Methods

```typescript
async waitForReady(timeout?: number): Promise<void> { ... }
async isReady(): Promise<boolean> { ... }
```

- Every page object has `waitForReady()` and `isReady()` methods.
- Guards are used in `before` hooks to ensure the page is in a testable state.

## Consequences

- **Positive**: Selector changes are localized to one file — all tests update automatically.
- **Positive**: IDE autocomplete surfaces available elements and actions.
- **Positive**: Singleton pattern avoids redundant instantiation.
- **Negative**: Non-trivial surfaces may result in large page object files.
- **Mitigation**: Split large page objects into sub-pages (e.g., `CanvasToolsPage.ts`, `CanvasObjectsPage.ts`).
