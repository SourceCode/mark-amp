#!/usr/bin/env python3
"""benchmark_compare.py — CI benchmark regression detection.

Compares current Google Benchmark JSON output against a stored baseline.
Flags regressions exceeding the threshold (default 5%) and exits with code 1.

Usage:
    python3 scripts/benchmark_compare.py \\
        --current results.json \\
        --baseline benchmarks/baseline/baseline.json \\
        [--threshold 5.0] [--metric cpu_time]
"""

import argparse
import json
import sys
from pathlib import Path
from typing import NamedTuple


class ComparisonResult(NamedTuple):
    name: str
    baseline_time: float
    current_time: float
    percent_change: float
    status: str  # PASS, FAIL, INFO, WARN


def load_benchmarks(filepath: Path) -> dict[str, dict]:
    """Load benchmark JSON and index by name."""
    with filepath.open() as fh:
        data = json.load(fh)
    return {bm["name"]: bm for bm in data.get("benchmarks", [])}


def compare_benchmarks(
    baseline: dict[str, dict],
    current: dict[str, dict],
    threshold: float,
    metric: str,
) -> list[ComparisonResult]:
    """Compare current benchmark results against the baseline."""
    results: list[ComparisonResult] = []

    # Compare benchmarks present in current
    for name, cur_bm in sorted(current.items()):
        # Skip aggregate entries (mean, median, stddev)
        if cur_bm.get("run_type") in ("aggregate",):
            continue

        if name not in baseline:
            results.append(ComparisonResult(
                name=name,
                baseline_time=0.0,
                current_time=cur_bm.get(metric, 0.0),
                percent_change=0.0,
                status="INFO",
            ))
            continue

        base_bm = baseline[name]
        base_time = base_bm.get(metric, 0.0)
        cur_time = cur_bm.get(metric, 0.0)

        if base_time <= 0:
            results.append(ComparisonResult(
                name=name,
                baseline_time=base_time,
                current_time=cur_time,
                percent_change=0.0,
                status="WARN",
            ))
            continue

        pct_change = ((cur_time - base_time) / base_time) * 100.0

        if pct_change > threshold:
            status = "FAIL"
        elif pct_change < -10.0:
            status = "IMPROVED"
        else:
            status = "PASS"

        results.append(ComparisonResult(
            name=name,
            baseline_time=base_time,
            current_time=cur_time,
            percent_change=pct_change,
            status=status,
        ))

    # Warn about benchmarks in baseline but not in current
    for name in sorted(baseline.keys()):
        if name not in current:
            base_bm = baseline[name]
            # Skip aggregates
            if base_bm.get("run_type") in ("aggregate",):
                continue
            results.append(ComparisonResult(
                name=name,
                baseline_time=base_bm.get(metric, 0.0),
                current_time=0.0,
                percent_change=0.0,
                status="WARN",
            ))

    return results


def format_time(time_ns: float) -> str:
    """Format time with appropriate units."""
    if time_ns < 1000:
        return f"{time_ns:.0f}ns"
    if time_ns < 1_000_000:
        return f"{time_ns / 1000:.1f}us"
    if time_ns < 1_000_000_000:
        return f"{time_ns / 1_000_000:.1f}ms"
    return f"{time_ns / 1_000_000_000:.2f}s"


def print_results(results: list[ComparisonResult], threshold: float) -> bool:
    """Print comparison results and return True if any failures."""
    has_failures = False
    pass_count = 0
    fail_count = 0
    info_count = 0
    warn_count = 0
    improved_count = 0

    for result in results:
        if result.status == "FAIL":
            has_failures = True
            fail_count += 1
            print(
                f"  FAIL  {result.name}: {format_time(result.baseline_time)} -> "
                f"{format_time(result.current_time)} "
                f"(+{result.percent_change:.1f}%) [threshold: {threshold}%]"
            )
        elif result.status == "IMPROVED":
            improved_count += 1
            print(
                f"  IMPROVED  {result.name}: {format_time(result.baseline_time)} -> "
                f"{format_time(result.current_time)} "
                f"({result.percent_change:.1f}%)"
            )
        elif result.status == "INFO":
            info_count += 1
            print(f"  INFO  {result.name}: no baseline (new benchmark)")
        elif result.status == "WARN":
            warn_count += 1
            if result.current_time == 0.0:
                print(f"  WARN  {result.name}: in baseline but not in current (removed?)")
            else:
                print(f"  WARN  {result.name}: zero baseline time, cannot compare")
        else:
            pass_count += 1
            print(
                f"  PASS  {result.name}: {format_time(result.baseline_time)} -> "
                f"{format_time(result.current_time)} "
                f"({result.percent_change:+.1f}%)"
            )

    print()
    print(
        f"Summary: {pass_count} passed, {fail_count} failed, "
        f"{improved_count} improved, {info_count} new, {warn_count} warnings"
    )

    return has_failures


def main() -> int:
    parser = argparse.ArgumentParser(
        description="Compare benchmark results against a stored baseline"
    )
    parser.add_argument(
        "--current", required=True, type=Path,
        help="Path to current benchmark JSON output"
    )
    parser.add_argument(
        "--baseline", required=True, type=Path,
        help="Path to baseline benchmark JSON"
    )
    parser.add_argument(
        "--threshold", type=float, default=5.0,
        help="Regression threshold percentage (default: 5.0)"
    )
    parser.add_argument(
        "--metric", default="cpu_time",
        choices=["cpu_time", "real_time"],
        help="Metric to compare (default: cpu_time)"
    )
    args = parser.parse_args()

    if not args.current.exists():
        print(f"Error: current results file not found: {args.current}")
        return 2

    if not args.baseline.exists():
        print(f"Warning: baseline file not found: {args.baseline}")
        print("No baseline to compare against. Treating as first run (pass).")
        return 0

    print(f"Comparing benchmarks (threshold: {args.threshold}%)")
    print(f"  Baseline: {args.baseline}")
    print(f"  Current:  {args.current}")
    print(f"  Metric:   {args.metric}")
    print()

    baseline = load_benchmarks(args.baseline)
    current = load_benchmarks(args.current)

    results = compare_benchmarks(baseline, current, args.threshold, args.metric)

    has_failures = print_results(results, args.threshold)

    if has_failures:
        print("\nBenchmark regression detected! Failing CI check.")
        return 1

    print("\nAll benchmarks within threshold. CI check passed.")
    return 0


if __name__ == "__main__":
    sys.exit(main())
