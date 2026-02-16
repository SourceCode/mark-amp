# Startup Profile — Phase 08 Analysis

## Benchmark Results (2026-02-15, Apple Silicon arm64, Debug)

| Phase                        | CPU Time    | Notes                                          |
| ---------------------------- | ----------- | ---------------------------------------------- |
| EventBus construction        | ~20ns       | Near-zero — struct init only                   |
| Config construction          | ~25ns       | No disk I/O in default ctor                    |
| ThemeRegistry (init + load)  | **~44.3ms** | **#1 bottleneck** — loads 126 themes from disk |
| CommandHistory               | ~32ns       | Trivial allocation                             |
| Extension Services (16 svcs) | ~2.9µs      | All lightweight default constructors           |
| PluginManager                | ~8.7µs      | EventBus + Config-dependent init               |
| FeatureRegistry              | ~0.5µs      | EventBus + Config references                   |
| **Full Non-GUI Startup**     | **~44.9ms** | Dominated by ThemeRegistry                     |

## Hot Path Analysis

```
ThemeRegistry::initialize()  ████████████████████████████████████████ 98.7%
PluginManager                █                                        0.02%
Extension Services           ▏                                        0.006%
Everything else              ▏                                       <0.001%
```

## Key Finding

**ThemeRegistry dominates startup at 98.7% of total non-GUI time.** It scans the user themes directory and parses 126 theme YAML files on every launch. All other subsystems are sub-microsecond.

## Optimization Opportunities (for Future Phases)

1. **Lazy theme loading** — load metadata only on startup, parse full theme on first access
2. **Theme cache** — serialize parsed themes to a binary cache; invalidate on file mtime change
3. **Parallel theme loading** — use thread pool to parse themes concurrently
4. **Reduce built-in theme count** — ship 3-4 defaults, lazy-load the rest from a themes package

## Target

Cold-start target: **< 150ms total** (including GUI). Current non-GUI portion is ~45ms, well within budget for core subsystems. ThemeRegistry optimization would reduce this to ~1ms.
