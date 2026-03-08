# ADR-001: Appium mac2 Stack Choice

## Status

Accepted

## Date

2026-03-06

## Context

MarkAmp is a native macOS desktop application built with wxWidgets and C++. We need end-to-end UI automation to validate user workflows and catch regressions before release. The key requirements are:

1. **Native macOS support**: Must interact with real macOS accessibility tree (XCUITest elements).
2. **Cross-framework compatibility**: wxWidgets is not natively supported by most web testing tools.
3. **Accessibility-first selectors**: Tests should use accessibility identifiers for stability.
4. **Team familiarity**: Engineering team has experience with WebDriver-based tooling.

### Options Considered

1. **Appium mac2 driver** — Uses Apple's XCTest framework under the hood. Exposes the macOS accessibility tree via WebDriver protocol.
2. **AppleScript + osascript** — Native macOS scripting. Limited selector granularity, no structured test framework.
3. **Hammerspoon + Lua** — macOS automation. Limited ecosystem, no test reporting integrations.
4. **Accessibility Inspector + custom bridge** — Direct NSAccessibility API. High maintenance, no community.

## Decision

Use **Appium 2.x with the `appium-mac2-driver`**. This provides:

- Standard WebDriver protocol for element discovery and interaction
- Access to the full macOS accessibility tree (XCUIElementType, identifier, label, title, value)
- Compatibility with WDIO test runner and Mocha framework
- CI integration via GitHub Actions on `macos-14` runners
- Screenshot capture on failure via standard WebDriver API
- Session management with automatic recovery

## Consequences

- **Positive**: Industry-standard tooling, rich ecosystem, stable API surface.
- **Positive**: Tests are portable to TypeScript/JavaScript, familiar to web-oriented engineers.
- **Negative**: mac2 driver is single-instance (no parallel test execution on one machine).
- **Negative**: Requires macOS Accessibility permissions on the host.
- **Negative**: Cold start of Appium server adds ~3-5s to each CI run.
- **Mitigation**: Use session caching and `noReset: true` to minimize overhead.
