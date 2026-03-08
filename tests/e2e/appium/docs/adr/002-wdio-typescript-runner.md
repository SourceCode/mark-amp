# ADR-002: WDIO + TypeScript Runner

## Status

Accepted

## Date

2026-03-06

## Context

With Appium mac2 chosen as the driver (ADR-001), we need a test runner and language. Requirements:

1. Strong typing for maintainability across 500+ tests.
2. Mature Appium client with async/await support.
3. Built-in reporting, retry, and parallel coordination.
4. CI-friendly configuration.

### Options Considered

1. **WDIO (WebdriverIO) + TypeScript** — First-class Appium support, built-in runner, Mocha/Jasmine framework adapters, spec reporter, TypeScript via ts-node.
2. **Playwright** — Excellent for Chromium-based apps. No native macOS accessibility tree support.
3. **wd.js + custom runner** — Low-level WebDriver client. Requires building runner infrastructure from scratch.
4. **Protractor** — Deprecated. Angular-focused.

## Decision

Use **WDIO 8.x with TypeScript** and **Mocha** as the test framework.

Configuration: `wdio.conf.ts` with `ts-node` auto-compile, strict TypeScript (`strict: true`), and `@wdio/globals/types` for global `browser`, `$`, `$$`, and `expect` types.

## Consequences

- **Positive**: Type-safe test code catches errors at compile time.
- **Positive**: WDIO's `$` and `$$` shorthand simplifies element queries.
- **Positive**: Built-in `spec` reporter and `mocha` framework — zero custom runner code.
- **Positive**: Mocha's `describe`/`it` pattern is familiar to the team.
- **Negative**: ts-node compilation adds ~1-2s startup overhead per test file.
- **Negative**: WDIO 8.x requires Node.js ≥ 18 (we target ≥ 20, so no issue).
- **Mitigation**: Use `skipLibCheck: true` to reduce compilation time.
