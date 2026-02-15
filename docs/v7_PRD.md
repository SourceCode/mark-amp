# MarkAmp Resilience Hardening Plan  
## 50 Defensive Programming Improvements for Zero-Crash Tolerance

Objective:  
MarkAmp must never crash due to user behavior, malformed input, plugin faults, race conditions, or environmental instability.  
Failures must be:

- Contained
- Logged
- User-visible (when relevant)
- Recoverable
- Diagnosable

Below are 50 concrete improvements across architecture, threading, input validation, plugins, memory, rendering, and operational diagnostics.

---

# I. Global Crash Containment

1. **Global Top-Level Exception Barrier**
   Wrap `main()` in a global try/catch that logs and shows a safe recovery dialog before exit.

2. **Thread Entry Guard Pattern**
   Every thread entry function must wrap logic in:
   ```cpp
   try { ... }
   catch (const std::exception& e) { log + isolate }
EventBus Subscriber Isolation
Ensure every subscriber invocation is individually wrapped (already partially done — enforce universally).

UI Message Loop Guard
Wrap wx event dispatch handlers in a crash barrier to prevent UI termination.

Plugin Activation Isolation
If activation throws, mark plugin as “faulted” and continue startup.

Plugin Deactivation Isolation
Ensure deactivation failures cannot cascade into shutdown failure.

II. Input Validation & Defensive Boundaries
Centralized Input Validation Utilities
Replace scattered validation logic with consistent validators for:

Paths

YAML keys

Extension manifests

Markdown blocks

All External Data is Untrusted
Treat:

Files

Extension manifests

Themes

YAML configs

VSIX contents
as adversarial.

Length Guards on All Strings
Enforce max lengths for:

Output channel messages

YAML keys

Notification content

Command palette entries

Filesystem Canonicalization Wrapper
Create a safe path resolver that:

Uses error_code overloads

Rejects traversal outside workspace

Detects symlink escapes

Bounded JSON/YAML Parsing
Reject configs exceeding defined size limits.

Safe Regex Execution
Add timeout and exception guards for all regex execution.

Numeric Range Enforcement
Clamp all user-configurable numeric values.

Enum Validation
Reject unknown enum strings during deserialization.

Strict Schema Validation
ExtensionManifest must validate against explicit schema.

III. Memory Safety & Resource Discipline
All Containers Must Be Bounded
Add explicit caps to:

Event queues

Logs

Diagnostics

Telemetry buffers

Safe Allocation Wrapper
Catch std::bad_alloc at subsystem boundaries.

No Raw Owning Pointers
Enforce unique_ptr/shared_ptr ownership rules.

No Cross-Thread Shared Raw References
All cross-thread data must use:

atomics

immutable snapshots

generation counters

Guard Against Integer Overflow
Use checked arithmetic for:

Buffer indexing

Offset math

Memory size computation

Prevent Underflow
Continue eliminating unsigned subtraction without guards.

Zero-Copy TextSpan Validation
Ensure TextSpan never references freed storage.

IV. Threading & Concurrency Hardening
Strict Thread Ownership Rules
Document and enforce which thread owns which data.

UI Thread Assertion Macro
Add runtime assertion for UI-only operations.

Deadlock Detection Debug Mode
Add watchdog to detect >100ms lock holds.

Queue Backpressure Mechanism
EventBus must drop or coalesce low-priority events under load.

Async Task Cancellation Tokens
All background tasks must support cancellation.

Generation-Based Invalidation Everywhere
Replace stale pointer risks with generation checks.

V. Plugin & Extension Containment
Per-Plugin Error Counter
Disable plugin after N consecutive failures.

Per-Plugin Execution Time Monitoring
Log long-running plugin operations.

Sandbox Filesystem Restrictions
Enforce path boundaries in extension file access.

Extension API Contract Versioning
Reject incompatible plugins early.

Fail-Closed Policy
If plugin signature validation fails → do not load.

Graceful Extension Host Restart
Detect crash and recover extension host without app restart.

VI. Rendering & UI Robustness
Full Damage Rect Validation
Clamp repaint rectangles to viewport bounds.

Null-Safe Rendering Pipeline
Validate all theme and font lookups before use.

Safe Glyph Cache Access
Never assume font metrics exist.

IME Defensive Handling
Guard composition ranges against invalid offsets.

Safe Scroll Calculations
Clamp scroll offsets before applying.

Fallback Theme Safety
If theme parsing fails → revert to safe built-in theme.

VII. Error Reporting & User Feedback
Central Error Reporting Service
Single structured error sink.

User-Facing Non-Blocking Error Toasts
Inform users without modal crashes.

Structured Log Format
JSON logs with:

Timestamp

Thread ID

Subsystem

Severity

Correlation ID

Crash Dump Generation
Produce stack trace and context snapshot.

Safe Mode Startup
Allow restart with:

Extensions disabled

Minimal config

Default theme

VIII. State & Persistence Safety
Atomic Config Writes
Write to temp file → rename swap.

Workspace State Snapshot Versioning
Reject incompatible state files.

Corruption Detection
Add checksum to workspace persistence.

Transactional Save Operations
Editor save must:

Write temp file

fsync

Rename atomically

IX. Observability & Diagnosis
Correlation IDs for Subsystems
Attach request IDs to:

Plugin activation

File loads

Event dispatch

Rendering passes

Enables tracing failure chains.

X. Cultural Rule for Zero-Crash Engineering
Add engineering mandates:

No unchecked casts

No silent catch(...)

No unbounded containers

No implicit narrowing conversions

No blocking I/O on UI thread

No external call without guard

No shared mutable global state

All async code cancelable

All failure paths logged

All logs actionable

Definition of Resilience
MarkAmp is resilient when:

Any plugin can fail without killing the host.

Any malformed file can be opened without crash.

Any user input can be rejected safely.

Any background task can be canceled safely.

Any thread failure is isolated.

Any unexpected exception is logged and contained.

Any corrupted config can be detected and recovered.

Any rendering fault falls back safely.

Any OOM results in controlled degradation.

The application never terminates unexpectedly.


```markdown
# MarkAmp Resilience Phase II  
## Advanced Containment, Self-Healing, and OTLP Remote Telemetry Integration

Objective:
Extend MarkAmp’s zero-crash tolerance into:

- Self-healing runtime behavior
- Fault-domain isolation
- Distributed diagnostics
- Enterprise observability
- Remote structured telemetry via OTLP (OpenTelemetry Protocol)

This phase assumes Phase I resilience controls are in place.

---

# SECTION I — Advanced Failure Containment

---

## 1. Fault Domains & Isolation Layers

1. **Subsystem Fault Domains**
   Explicitly isolate:
   - Rendering
   - Extension host
   - Markdown engine
   - YAML parsing
   - Workspace manager
   - Network services

   Failure in one domain must not cascade.

2. **Soft-Fail Rendering Mode**
   If rendering fails:
   - Switch to fallback rendering engine
   - Disable advanced visual effects
   - Continue editing capability

3. **Graceful Feature Degradation**
   Under instability:
   - Disable minimap
   - Disable live preview
   - Throttle diagnostics
   - Reduce background tasks

4. **Self-Healing State Reset**
   If subsystem detects corruption:
   - Reset subsystem state
   - Reinitialize with clean config
   - Notify user

5. **Plugin Fault Quarantine**
   After repeated faults:
   - Move plugin to quarantine list
   - Prevent auto-reload
   - Log signature + stack trace

---

# SECTION II — Advanced Memory & Resource Safeguards

---

6. **Memory Pressure Monitor**
   Periodically check:
   - RSS
   - Heap fragmentation
   - Arena consumption

   If threshold exceeded:
   - Trigger cache eviction
   - Compact data structures
   - Reduce background tasks

7. **Fail-Safe Memory Reserve**
   Reserve emergency memory block.
   If OOM detected:
   - Release reserve
   - Show safe warning
   - Disable heavy features

8. **Dynamic Backpressure Scaling**
   Under load:
   - Slow event dispatch
   - Batch UI refreshes
   - Coalesce background tasks

9. **Fragmentation Monitoring**
   Track allocation patterns via allocator stats.

10. **Hard Caps with Graceful Degradation**
   Instead of crash:
   - Refuse new extensions
   - Limit file size
   - Cap open buffers

---

# SECTION III — Threading Resilience Enhancements

---

11. **Thread Health Registry**
   Maintain registry of:
   - Thread name
   - Last heartbeat
   - Workload status

12. **Watchdog Timer for UI Thread**
   If UI blocked > threshold:
   - Log stall
   - Dump stack snapshot

13. **Deadlock Suspicion Detector**
   Detect circular lock acquisition patterns in debug builds.

14. **Safe Task Cancellation Enforcement**
   All async tasks must:
   - Check cancellation token frequently
   - Abort safely

15. **Thread Panic Escalation Policy**
   If background thread fails:
   - Log
   - Restart thread
   - Do not terminate process

---

# SECTION IV — Data Integrity & Persistence Hardening

---

16. **Write-Ahead Logging for Workspace State**
   Before state mutation:
   - Append WAL entry
   - Apply change
   - Commit

17. **Crash-Safe Recovery Replay**
   On startup:
   - Replay incomplete WAL entries

18. **State Snapshot Versioning**
   Embed schema version in all persisted files.

19. **Incremental State Validation**
   Periodically verify in-memory state consistency.

20. **Corruption Isolation Mode**
   If corruption detected:
   - Load workspace in safe read-only mode

---

# SECTION V — Advanced Defensive Input Strategy

---

21. **Strict MIME Validation for Embedded Content**
   Reject invalid embedded content types.

22. **Markdown Parsing Hard Limits**
   Limit:
   - Nesting depth
   - Table size
   - Mermaid node count

23. **Sandboxed HTML Rendering**
   Disable inline script execution permanently.

24. **Time-Limited Parsing**
   Abort parsing if exceeds threshold time.

25. **Rate-Limited Event Flood Protection**
   Protect from:
   - Rapid file changes
   - Malicious plugin event spam

---

# SECTION VI — Observability & Diagnostics (OTLP Integration)

---

# 1. Introduce OpenTelemetry Architecture

Add internal telemetry abstraction layer:

```

TelemetryService
├── Logger
├── MetricsCollector
├── TraceEmitter
└── OTLPExporter

```

---

## 26. OpenTelemetry SDK Integration

Use:

- OpenTelemetry C++ SDK (Apache 2.0)
- OTLP over gRPC or HTTP

---

## 27. Structured Trace Spans

Create spans for:

- Startup
- Plugin activation
- File open
- Rendering pass
- Extension execution
- Event dispatch
- Async pipeline stages

Each span includes:

- Duration
- Thread ID
- Correlation ID
- Error status
- Memory delta (optional)

---

## 28. OTLP Metrics Collection

Emit metrics:

- Frame time histogram
- Event queue depth
- Memory usage
- Extension CPU time
- Crash count
- Error rate
- Background task duration

---

## 29. OTLP Error Events

All errors reported with:

- Stack trace
- Subsystem
- Severity
- Plugin identifier (if applicable)
- File context
- User action context (if safe)

---

## 30. Privacy-Aware Telemetry Layer

Telemetry must support:

- Full opt-in
- Local-only mode
- Air-gapped disable mode
- Redaction of file paths
- Redaction of user content

---

## 31. Remote Telemetry Fail-Safe

If OTLP endpoint unreachable:

- Queue locally (bounded)
- Drop oldest on overflow
- Never block UI thread

---

## 32. Backpressure on Telemetry Export

If network slow:

- Reduce export frequency
- Batch spans
- Disable non-critical metrics

---

## 33. Correlation ID Propagation

Each:

- File operation
- Plugin activation
- Async pipeline stage

Gets unique correlation ID propagated through logs and spans.

---

## 34. Structured JSON Log Mirror

All logs emitted:

- To file
- To console
- To OTLP exporter

Unified schema.

---

## 35. Crash Event Export

On crash:

- Generate crash report
- Attach last 100 spans
- Export to OTLP if allowed

---

# SECTION VII — Safe Mode & Recovery Improvements

---

36. Multi-Tier Safe Mode

Allow startup flags:

- `--safe`
- `--safe-no-extensions`
- `--safe-no-preview`
- `--safe-minimal-ui`

---

37. Automatic Safe Mode Trigger

If N crashes within time window:
- Auto-start in safe mode
- Prompt user

---

38. Fault Recovery Dialog

If subsystem resets:
- Inform user
- Provide logs
- Offer safe-mode restart

---

# SECTION VIII — Enterprise Runtime Protections

---

39. Runtime Policy Enforcement Engine

Continuously enforce:

- Extension restrictions
- Memory caps
- Network restrictions

---

40. Extension Network Interceptor

Block outbound network calls unless explicitly permitted.

---

41. Immutable Execution Mode

Enterprise flag:
- Prevent runtime modification of config
- Disable extension install

---

42. Extension Resource Attribution

Track per-extension:
- Memory
- Events processed
- Errors generated

Expose via diagnostics panel.

---

# SECTION IX — Testing & Chaos Engineering

---

43. Continuous Fault Injection Framework

Inject at runtime:

- Random exceptions
- Simulated OOM
- Thread interruption
- File permission errors

Ensure survival.

---

44. Chaos Plugin Harness

Test misbehaving plugin scenarios:
- Infinite loops
- Throwing handlers
- Memory abuse

---

45. Synthetic Event Flood Testing

Simulate 10k events per second.
Ensure backpressure holds.

---

# SECTION X — Cultural & Structural Rules

---

46. All Public API Calls Must Be Guarded

No external call without try/catch boundary.

47. No Cross-Subsystem Direct Coupling

All communication via event or service abstraction.

48. Explicit Failure Return Types

Use:
- std::expected (when available)
- Result<T, Error>
Instead of throwing for routine failure.

49. No Silent Recovery

All recoveries must log.

50. Every Crash Must Produce Actionable Data

Crash report must include:
- Subsystem
- Stack
- Recent spans
- Config state hash
- Plugin state

---

# Definition of Industrial Resilience v2

MarkAmp is now:

- Self-healing under failure
- Observable across distributed systems
- Policy-enforceable
- Telemetry-enabled
- Privacy-aware
- Backpressure-capable
- Plugin-isolated
- Fault-domain segmented
- Crash-intolerant
- Recovery-driven

Resilience is not about preventing failure.

It is about containing, observing, degrading gracefully, and recovering.

OTLP gives you the visibility.
Containment gives you the stability.
Together, they give you industrial-grade reliability.
```

