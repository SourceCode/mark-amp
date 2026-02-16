# V6 Performance Tuning Guide — Phase 40

## Frame Budget Tuning

The `FrameBudgetEnforcer` controls how subsystems share frame time.

| Mode   | Budget | Use Case         |
| ------ | ------ | ---------------- |
| Typing | 8ms    | Active editing   |
| Idle   | 16ms   | Background tasks |

### Adjusting Budgets

```cpp
AdaptiveThrottle throttle;
// Default idle threshold is 300ms — customize:
AdaptiveThrottle throttle(std::chrono::milliseconds(500));
```

## Memory Budget Tuning

Use `MemoryBudget` to set per-component limits:

```cpp
MemoryBudget budget(64 * 1024 * 1024); // 64MB
auto guard = budget.try_allocate(size);
```

## Large File Mode

Auto-activates at:

- **File size**: 2 MB (configurable via `set_size_threshold()`)
- **Line count**: 50,000 lines (configurable via `set_line_threshold()`)

Features progressively disabled:

1. Minimap, link detection, spell check (always off)
2. Folding (off at 2x threshold)
3. Bracket matching (off at 3x threshold)
4. Word wrap (off at 4x threshold)
5. Syntax highlighting (off at 5x threshold)

## Watchdog Configuration

The `Watchdog` detects UI stalls:

- **Default threshold**: 200ms
- **Warning**: >threshold, <2x threshold
- **Critical**: >2x threshold

## Structured Logging

Use `StructuredLogger` for production diagnostics:

```cpp
StructuredLogger logger("Engine", StructuredLogger::OutputFormat::Json);
logger.info("Frame completed", {{"elapsed_ms", "16.2"}, {"dropped", "false"}});
```

## Benchmarking

Run all benchmarks:

```bash
./build/debug/benchmarks/markamp_bench
```

Compare specific targets:

```bash
./build/debug/benchmarks/markamp_bench --benchmark_filter="BM_ConstexprMap"
```
