# MarkAmp E2E Test Program — Onboarding

## 46. Developer Guide: Running E2E Tests Locally

### Prerequisites

1. macOS ≥ 14.0 (Sonoma)
2. Xcode Command Line Tools: `xcode-select --install`
3. Node.js ≥ 20.x: `nvm install 20`
4. MarkAmp debug build: `cmake -B build/debug -DCMAKE_BUILD_TYPE=Debug && cmake --build build/debug`
5. Accessibility permissions: System Settings → Privacy & Security → Accessibility → Add your Terminal app

### First-Time Setup

```bash
cd tests/e2e/appium
bash scripts/bootstrap_mac2.sh
```

This installs Appium 2.x globally, the mac2 driver, and project npm dependencies.

### Running Tests

```bash
# Run all tests
npm test

# Run smoke tests only (fast, must-pass)
npm run test:smoke

# Run workflow tests
npm run test:workflows

# Run a single spec file
npx wdio run wdio.conf.ts --spec src/specs/smoke/launch_and_shell_ready.spec.ts

# Run by grep pattern (tier filtering)
npx wdio run wdio.conf.ts --mochaOpts.grep "@critical"
```

### E2E Mode

Tests set `MARKAMP_E2E=1` automatically. This enables:

- Isolated config directory at `$TMPDIR/markamp_e2e/`
- No animations or tips
- Predictable initial state
- Deterministic theme

### Debugging Tips

1. **View the accessibility tree**: In a test, add `console.log(await browser.getPageSource())` to dump the full XML tree.
2. **Interactive debugging**: Set `mochaOpts.timeout` to `300000` (5 min) and add `await browser.debug()` in your test.
3. **Failure artifacts**: Check `tests/e2e/appium/artifacts/` for screenshots and page source from failures.
4. **Appium logs**: Start Appium with `appium --log-level debug` for verbose protocol logging.

### Common Issues

| Problem                           | Solution                                                             |
| --------------------------------- | -------------------------------------------------------------------- |
| "Session not created"             | Ensure MarkAmp is built and the app bundle exists                    |
| "Element not found"               | Check selector in `AppShellPage.ts`. Open a workspace first.         |
| "Accessibility permission denied" | Grant Terminal.app Accessibility permission in System Settings       |
| Flaky test failures               | Check for missing waits. Use `waitForExist()` before interactions.   |
| Session goes stale                | Close any macOS native dialogs. The `beforeTest` hook auto-recovers. |

---

## 47. Maintainer Guide: Evolving Suites Safely

### Adding a New Surface Area

1. Create a page object: `src/pages/<Surface>Page.ts`
2. Register selectors in `src/support/selector_registry.ts`
3. Update `selector_contract.snapshot.json`
4. Create spec directory: `src/specs/workflows/<surface>/`
5. Write initial smoke test
6. Update the coverage map in `docs/checklists.md`

### Renaming a Selector

1. Update the C++ source: `SetName("ma.<new.name>")`
2. Update `selector_registry.ts`
3. Update `selector_contract.snapshot.json`
4. Update the page object getter
5. Run `npm run typecheck` — TypeScript will catch any remaining references
6. Run `npm run test:smoke` to validate

### Adding a New Test Tier

1. Create the spec directory: `src/specs/<tier>/`
2. Add a `describe` tag: `@<tier>`
3. Add the retry policy in `retries.ts`
4. Add the npm script in `package.json`
5. Add the WDIO spec pattern in `wdio.conf.ts`
6. Add the CI workflow step

### Upgrading WDIO or Appium

1. Create a canary branch
2. Update `package.json` versions
3. Run `npm ci`
4. Run the full smoke suite
5. Check for API changes in WDIO/Appium changelogs
6. Update any affected page objects or support utilities
7. Document in ADR if the upgrade changes conventions

### Managing Quarantined Tests

1. Move the failing spec to `src/specs/quarantine/` (or add `@quarantined` tag)
2. File a tracking issue with failure artifacts
3. Assign to the subsystem owner
4. Set a 14-day deadline
5. Fix and move back, or remove with justification

---

## 48. Test Authoring Examples

### Example 1: Simple Smoke Test (Launch Verification)

```typescript
import AppShellPage from "../../pages/AppShellPage";
import { ensureAppRunning, resetAppState } from "../../support/session";

describe("@smoke App Shell — Launch verification", () => {
  before(async () => {
    await ensureAppRunning();
    await AppShellPage.waitForShellReady();
  });

  afterEach(async () => {
    await resetAppState();
  });

  it("should have MarkAmp window accessible", async () => {
    const exists = await (await AppShellPage.mainWindow).isExisting();
    expect(exists).toBe(true);
  });

  it('should have window title of "MarkAmp"', async () => {
    const title = await AppShellPage.getWindowTitle();
    expect(title).toBe("MarkAmp");
  });
});
```

### Example 2: Workflow Test with Fixture Workspace

```typescript
import AppShellPage from "../../pages/AppShellPage";
import FileTreePage from "../../pages/FileTreePage";
import { ensureAppRunning } from "../../support/session";
import { prepareWorkspace, cleanupWorkspace } from "../../support/workspace";
import { openFolderViaDialog } from "../../support/folder_open";

describe("@workflow File Explorer — Create new file", () => {
  let workspacePath: string;

  before(async () => {
    await ensureAppRunning();
    await AppShellPage.waitForShellReady();
    workspacePath = prepareWorkspace("basic");
    await openFolderViaDialog(workspacePath);
  });

  after(async () => {
    cleanupWorkspace(workspacePath);
  });

  it("should create a new file in the explorer", async () => {
    // Use keyboard shortcut to create new file
    await sendShortcut("Cmd+N");

    // Type file name
    await typeText("new_test_file.md");
    await pressEnter();

    // Verify file appears in tree
    const treeItems = await FileTreePage.getAllTreeItems();
    expect(treeItems).toContain("new_test_file.md");
  });
});
```

### Example 3: Page Object Pattern

```typescript
class EditorPage {
  /** Editor panel via NSAccessibility identifier */
  get panel(): ReturnType<WebdriverIO.Browser["$"]> {
    return browser.$("~ma.editor.panel");
  }

  /** Wait for editor to be ready for interaction */
  async waitForReady(timeout = 10000): Promise<void> {
    await (await this.panel).waitForExist({ timeout });
  }

  /** Check if editor is currently displayed */
  async isReady(): Promise<boolean> {
    try {
      return await (await this.panel).isExisting();
    } catch {
      return false;
    }
  }

  /** Get all static text content from the editor */
  async getEditorContent(): Promise<string[]> {
    const elements = await browser.$$(
      "-ios predicate string:elementType == 48",
    );
    const texts: string[] = [];
    for (const el of elements) {
      const value = await el.getAttribute("value");
      if (value) texts.push(value);
    }
    return texts;
  }
}

export default new EditorPage();
```

### Example 4: Support Utility Pattern

```typescript
// support/waits.ts pattern

/**
 * Wait for an element by accessibility name with custom timeout.
 * @param accessibilityName - The ma.* selector name
 * @param timeoutMs - Maximum wait time in milliseconds
 * @returns The resolved element
 */
export async function waitForAccessibilityElement(
  accessibilityName: string,
  timeoutMs = 10000,
): Promise<WebdriverIO.Element> {
  const element = await $(`~${accessibilityName}`);
  await element.waitForExist({
    timeout: timeoutMs,
    timeoutMsg: `Element ~${accessibilityName} not found within ${timeoutMs}ms`,
  });
  return element;
}
```
