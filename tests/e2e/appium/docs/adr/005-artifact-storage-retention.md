# ADR-005: Artifact Storage Retention

## Status

Accepted

## Date

2026-03-06

## Context

Every failing E2E test captures a failure artifact bundle (screenshot, page source XML, app logs, Appium server logs, workspace state snapshot). These artifacts are essential for debugging but consume storage over time. We need a retention policy that balances debuggability with cost.

### Options Considered

1. **Short retention (7 days)** — Sufficient for immediate triage. Low cost.
2. **Medium retention (30 days)** — Covers a full sprint cycle. Moderate cost.
3. **Long retention (90 days)** — Covers multiple sprints. High cost.
4. **Tiered retention** — Different retention by suite tier and branch.

## Decision

Use **tiered retention** based on branch and test tier:

| Context            | Suite Tier           | Retention |
| ------------------ | -------------------- | --------- |
| PR builds          | Smoke, Critical      | 3 days    |
| `main` branch      | Smoke, Critical      | 7 days    |
| `main` branch      | Workflow, Regression | 14 days   |
| Nightly builds     | All tiers            | 14 days   |
| Release candidates | All tiers            | 30 days   |
| Release builds     | All tiers            | 90 days   |

### Storage Location

- **CI (GitHub Actions)**: `actions/upload-artifact@v4` with `retention-days` parameter.
- **Local development**: `tests/e2e/appium/artifacts/` — not auto-cleaned. Developer responsibility.

### Artifact Bundle Contents

Each bundle directory contains:

```
<test_name>_<ISO_timestamp>/
├── screenshot.png              # Visual state at failure
├── page_source.xml             # Full accessibility tree
├── logs.json                   # App log lines
├── appium_server.log           # Driver protocol traffic
├── workspace_snapshot.tar.gz   # Temp workspace directory
└── timeline.json               # Step-by-step timing data
```

### Size Budget

- Individual bundle: ≤ 10 MB (typical: 2–5 MB)
- Per-run total: ≤ 100 MB (assuming ≤ 20 failures)
- Monthly CI storage: ≤ 5 GB

## Consequences

- **Positive**: RC and release artifacts available for full sprint cycle.
- **Positive**: PR artifacts cleaned up quickly to reduce cost.
- **Positive**: Consistent artifact format simplifies tooling.
- **Negative**: 90-day retention for releases requires storage budgeting.
- **Mitigation**: Compress workspace snapshots. Limit screenshot resolution to 2x.
