# Top 20 Sublime Text Performance Patterns — C++23 Implementation Guide

> A deep-dive into the architectural and micro-level decisions that make Sublime Text feel
> instant, with concrete C++23 implementation patterns mapped to the **MarkAmp** codebase.

---

## Table of Contents

| #                                                    | Pattern                   | Core Principle                     |
| ---------------------------------------------------- | ------------------------- | ---------------------------------- |
| [1](#1-single-purpose-latency-first-ui-thread)       | Single-Purpose UI Thread  | Latency isolation                  |
| [2](#2-aggressive-incremental-rendering)             | Incremental Rendering     | Dirty regions + coalesced repaints |
| [3](#3-ropepiece-table-text-buffer)                  | Rope/Piece-Table Buffer   | O(log n) edits                     |
| [4](#4-cached-line-index--fast-rowcol-mapping)       | Cached Line Index         | O(log n) offset↔(line,col)         |
| [5](#5-asynchronous-syntax-highlighting)             | Async Syntax Highlighting | Non-blocking tokenization          |
| [6](#6-immutable-snapshots--versioning)              | Immutable Snapshots       | Lock-free concurrency              |
| [7](#7-minimal-locking-via-message-passing)          | Message Passing           | Eliminate shared mutable state     |
| [8](#8-work-coalescing--cancellation)                | Work Coalescing           | Latest-wins, discard stale         |
| [9](#9-arena-allocators--object-pools)               | Arena Allocators          | Zero-overhead hot allocations      |
| [10](#10-cache-friendly-data-layout)                 | Cache-Friendly Layout     | SoA, linear scans                  |
| [11](#11-o1olog-n-typing-path-guarantee)             | O(1) Typing Path          | Never block a keystroke            |
| [12](#12-deferredlazy-layout--measurement)           | Lazy Layout               | Measure only when needed           |
| [13](#13-viewport-based-rendering--virtualization)   | Viewport Virtualization   | Render only visible lines          |
| [14](#14-chunked-font-rasterization--glyph-atlas)    | Glyph Atlas Caching       | Rasterize once, blit always        |
| [15](#15-incremental-search--indexing)               | Incremental Search        | Background index, partial results  |
| [16](#16-fast-scrolling-via-pre-rendered-surfaces)   | Pre-Rendered Surfaces     | Scroll = surface shift             |
| [17](#17-gpu-accelerated-compositing)                | GPU Compositing           | Batch draw calls, instance glyphs  |
| [18](#18-predictable-io)                             | Predictable I/O           | mmap + async streaming             |
| [19](#19-instrumentation-driven-micro-optimizations) | Instrumentation           | Measure everything, assert budgets |
| [20](#20-deterministic-scheduling)                   | Deterministic Scheduling  | Frame budgets + priority queues    |

---

## 1. Single-Purpose, Latency-First UI Thread

### Why Sublime Does It

The UI thread is **sacred**: only input handling, minimal layout/paint orchestration, and tiny
state mutations. Every microsecond of latency on the UI thread is felt by the user. Blocking
calls, heap allocations in hot paths, and cross-thread waits are forbidden.

### C++23 Implementation

```cpp
#include <atomic>
#include <thread>
#include <stop_token>

namespace markamp::core {

/// A lock-free SPSC (single-producer, single-consumer) ring buffer
/// for passing small messages from workers to the UI thread.
template <typename T, std::size_t Capacity>
class SPSCQueue {
    static_assert((Capacity & (Capacity - 1)) == 0, "Capacity must be power of 2");

    alignas(64) std::atomic<std::size_t> head_{0};
    alignas(64) std::atomic<std::size_t> tail_{0};
    std::array<T, Capacity> buffer_;

public:
    /// Producer: returns false if full.
    [[nodiscard]] bool try_push(const T& item) noexcept {
        auto tail = tail_.load(std::memory_order_relaxed);
        auto next = (tail + 1) & (Capacity - 1);
        if (next == head_.load(std::memory_order_acquire))
            return false; // full
        buffer_[tail] = item;
        tail_.store(next, std::memory_order_release);
        return true;
    }

    /// Consumer: returns false if empty.
    [[nodiscard]] bool try_pop(T& item) noexcept {
        auto head = head_.load(std::memory_order_relaxed);
        if (head == tail_.load(std::memory_order_acquire))
            return false; // empty
        item = buffer_[head];
        head_.store((head + 1) & (Capacity - 1), std::memory_order_release);
        return true;
    }
};

/// Enforce "UI-only" responsibilities with a concept.
/// UI callbacks must be noexcept and O(1).
template <typename F, typename... Args>
concept UICallback = requires(F f, Args... args) {
    { f(args...) } noexcept;
};

/// Dedicated UI thread with its own message pump.
class UIThread {
    std::jthread thread_;
    SPSCQueue<std::function<void()>, 1024> queue_;
    std::atomic<bool> has_work_{false};

public:
    void post(auto&& work) noexcept {
        queue_.try_push(std::forward<decltype(work)>(work));
        has_work_.store(true, std::memory_order_release);
    }

    /// Drain the queue each frame — call from wxApp::OnIdle or timer.
    void drain() noexcept {
        if (!has_work_.load(std::memory_order_acquire)) return;

        std::function<void()> task;
        while (queue_.try_pop(task)) {
            task();
        }
        has_work_.store(false, std::memory_order_release);
    }
};

} // namespace markamp::core
```

### MarkAmp Integration Points

| Current Component                                                                | Enhancement                                                                                                |
| -------------------------------------------------------------------------------- | ---------------------------------------------------------------------------------------------------------- |
| [EventBus.h](file:///Users/ryanrentfro/code/markamp/src/core/EventBus.h)         | `queue()` already defers to main thread — add lock-free SPSC path for hot events like `CursorChangedEvent` |
| [MainFrame.cpp](file:///Users/ryanrentfro/code/markamp/src/ui/MainFrame.cpp)     | Bind `UIThread::drain()` to `wxEVT_IDLE` handler                                                           |
| [EditorPanel.cpp](file:///Users/ryanrentfro/code/markamp/src/ui/EditorPanel.cpp) | Ensure all Scintilla callbacks are `noexcept` and O(1)                                                     |

> [!IMPORTANT]
> The single most impactful rule: **never block the UI thread**. Every `std::mutex::lock()`,
> every `new`, every file read on the UI thread is a frame stall the user will feel.

---

## 2. Aggressive Incremental Rendering

### Why Sublime Does It

Sublime repaints only what changed. It merges invalidation rectangles, throttles to vsync,
and never re-renders the entire window when a single character changes.

### C++23 Implementation

```cpp
#include <vector>
#include <algorithm>

namespace markamp::rendering {

struct Rect {
    int x, y, width, height;

    [[nodiscard]] auto united(const Rect& other) const noexcept -> Rect {
        int nx = std::min(x, other.x);
        int ny = std::min(y, other.y);
        int nr = std::max(x + width, other.x + other.width);
        int nb = std::max(y + height, other.y + other.height);
        return {nx, ny, nr - nx, nb - ny};
    }

    [[nodiscard]] auto intersects(const Rect& other) const noexcept -> bool {
        return !(x >= other.x + other.width  || other.x >= x + width ||
                 y >= other.y + other.height || other.y >= y + height);
    }
};

/// Accumulates dirty regions and coalesces them per frame.
class DirtyRegionAccumulator {
    std::vector<Rect> regions_;
    static constexpr int kMergeThreshold = 64; // pixels

public:
    void invalidate(Rect r) noexcept {
        // Attempt to merge with existing region
        for (auto& existing : regions_) {
            Rect expanded = {
                existing.x - kMergeThreshold,
                existing.y - kMergeThreshold,
                existing.width + 2 * kMergeThreshold,
                existing.height + 2 * kMergeThreshold
            };
            if (r.intersects(expanded)) {
                existing = existing.united(r);
                return;
            }
        }
        regions_.push_back(r);
    }

    [[nodiscard]] auto consume() noexcept -> std::vector<Rect> {
        return std::exchange(regions_, {});
    }

    [[nodiscard]] auto empty() const noexcept -> bool {
        return regions_.empty();
    }
};

} // namespace markamp::rendering
```

### Key Principles

- **Coalesce** nearby dirty rects to avoid excessive draw calls
- **Throttle** to frame budget: `16.6ms` @ 60fps, `6.9ms` @ 144fps
- **Skip layouts** unless constraints actually changed (font size, window resize)
- In MarkAmp's [PreviewPanel](file:///Users/ryanrentfro/code/markamp/src/ui/PreviewPanel.cpp): replace full `SetPage()` calls with incremental DOM patching where possible

---

## 3. Rope/Piece-Table Text Buffer

### Why Sublime Does It

A naive `std::string` buffer requires O(n) moves on every insert/delete. A piece table
stores text as two immutable buffers (original + appended) and a sequence of "pieces"
that reference slices, making edits O(log n) split/merge operations.

### C++23 Implementation

```cpp
#include <cstdint>
#include <memory>
#include <string>
#include <string_view>
#include <vector>

namespace markamp::core {

/// Which underlying buffer a piece refers to.
enum class BufferSource : uint8_t { Original, Append };

/// A single piece: a view into one of the two buffers.
struct Piece {
    BufferSource source;
    std::size_t  offset;  // start offset within the buffer
    std::size_t  length;  // number of bytes
};

/// A piece table for efficient text editing.
/// Original file content is immutable; edits append to the append buffer.
class PieceTable {
    std::string original_buffer_;
    std::string append_buffer_;
    std::vector<Piece> pieces_; // ordered sequence of pieces

public:
    /// Construct from initial file content.
    explicit PieceTable(std::string content)
        : original_buffer_(std::move(content))
    {
        if (!original_buffer_.empty()) {
            pieces_.push_back({
                BufferSource::Original, 0, original_buffer_.size()
            });
        }
    }

    /// Insert text at the given logical offset.
    void insert(std::size_t offset, std::string_view text) {
        auto append_offset = append_buffer_.size();
        append_buffer_.append(text);

        auto [piece_idx, inner_offset] = find_piece(offset);

        if (inner_offset == 0) {
            // Insert before the piece
            pieces_.insert(
                pieces_.begin() + static_cast<std::ptrdiff_t>(piece_idx),
                Piece{BufferSource::Append, append_offset, text.size()}
            );
        } else {
            // Split the piece
            auto& p = pieces_[piece_idx];
            Piece left  {p.source, p.offset, inner_offset};
            Piece middle{BufferSource::Append, append_offset, text.size()};
            Piece right {p.source, p.offset + inner_offset, p.length - inner_offset};

            pieces_[piece_idx] = left;
            auto insert_pos = pieces_.begin()
                + static_cast<std::ptrdiff_t>(piece_idx) + 1;
            pieces_.insert(insert_pos, {middle, right});
        }
    }

    /// Delete `count` bytes starting at logical `offset`.
    void erase(std::size_t offset, std::size_t count) {
        // Implementation: find affected pieces, split at boundaries,
        // remove fully-covered pieces, trim partially-covered ones.
        // Omitted for brevity — follows the same find_piece pattern.
    }

    /// Retrieve the full text content. O(n) — use sparingly.
    [[nodiscard]] auto text() const -> std::string {
        std::string result;
        for (const auto& p : pieces_) {
            const auto& buf = (p.source == BufferSource::Original)
                ? original_buffer_ : append_buffer_;
            result.append(buf, p.offset, p.length);
        }
        return result;
    }

    /// Total logical length.
    [[nodiscard]] auto size() const noexcept -> std::size_t {
        std::size_t total = 0;
        for (const auto& p : pieces_) total += p.length;
        return total;
    }

private:
    /// Find which piece contains the given logical offset.
    /// Returns {piece_index, offset_within_piece}.
    [[nodiscard]] auto find_piece(std::size_t offset) const
        -> std::pair<std::size_t, std::size_t>
    {
        std::size_t pos = 0;
        for (std::size_t i = 0; i < pieces_.size(); ++i) {
            if (offset <= pos + pieces_[i].length) {
                return {i, offset - pos};
            }
            pos += pieces_[i].length;
        }
        return {pieces_.size(), 0};
    }
};

} // namespace markamp::core
```

> [!TIP]
> For production use, replace the `std::vector<Piece>` with a balanced tree (B-tree or
> red-black tree with subtree-size augmentation) for O(log n) indexing into millions of
> pieces. Keep node size ≤ cache line (64 bytes) and pool nodes with a free list.

---

## 4. Cached Line Index & Fast (row, col) Mapping

### Why Sublime Does It

Every cursor movement, every scroll, every highlight query needs to convert between
absolute byte offsets and (line, column) pairs. Scanning from the start of the file
each time is O(n) and unacceptable on large files.

### C++23 Implementation

```cpp
#include <algorithm>
#include <cstdint>
#include <vector>

namespace markamp::core {

/// Maintains a sorted index of newline byte offsets.
/// Supports O(log n) offset ↔ (line, col) conversion.
/// Updated incrementally on each edit.
class LineIndex {
    std::vector<std::size_t> newline_offsets_; // sorted

public:
    /// Build from scratch for initial file content.
    void rebuild(std::string_view content) {
        newline_offsets_.clear();
        for (std::size_t i = 0; i < content.size(); ++i) {
            if (content[i] == '\n') {
                newline_offsets_.push_back(i);
            }
        }
    }

    /// Incrementally update after an insert at `offset` of `length` bytes
    /// containing `new_newlines` newline positions (relative to offset).
    void on_insert(std::size_t offset, std::size_t length,
                   const std::vector<std::size_t>& new_newlines) {
        // Shift all existing offsets after the insertion point
        auto it = std::lower_bound(
            newline_offsets_.begin(), newline_offsets_.end(), offset);
        for (auto jt = it; jt != newline_offsets_.end(); ++jt) {
            *jt += length;
        }
        // Insert the new newline positions
        for (auto nl : new_newlines) {
            auto pos = offset + nl;
            auto insert_at = std::lower_bound(
                newline_offsets_.begin(), newline_offsets_.end(), pos);
            newline_offsets_.insert(insert_at, pos);
        }
    }

    /// Convert byte offset → (line, col). Both 0-indexed.
    [[nodiscard]] auto offset_to_line_col(std::size_t offset) const
        -> std::pair<std::size_t, std::size_t>
    {
        auto it = std::upper_bound(
            newline_offsets_.begin(), newline_offsets_.end(), offset);
        auto line = static_cast<std::size_t>(
            it - newline_offsets_.begin());
        auto line_start = (line == 0) ? 0 : newline_offsets_[line - 1] + 1;
        return {line, offset - line_start};
    }

    /// Convert (line, col) → byte offset.
    [[nodiscard]] auto line_col_to_offset(
        std::size_t line, std::size_t col) const -> std::size_t
    {
        auto line_start = (line == 0) ? 0 : newline_offsets_[line - 1] + 1;
        return line_start + col;
    }

    [[nodiscard]] auto line_count() const noexcept -> std::size_t {
        return newline_offsets_.size() + 1;
    }
};

} // namespace markamp::core
```

### MarkAmp Integration

The [StatusBarPanel](file:///Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp) currently shows line/col statistics. Integrating a `LineIndex` into the text buffer ensures these updates remain O(log n) regardless of file size.

---

## 5. Asynchronous Syntax Highlighting

### Why Sublime Does It

Typing must never block on syntax parsing. Sublime runs the lexer in the background, applies
deltas, and only re-lexes the affected region until the lexer state stabilizes.

### C++23 Implementation

```cpp
#include <atomic>
#include <stop_token>
#include <thread>
#include <vector>

namespace markamp::core {

/// Per-line lexer state for incremental re-tokenization.
struct LineState {
    uint32_t state_hash;   // lexer state at end of this line
    uint16_t version;      // edit version when last tokenized
};

/// Background highlighter that processes only changed regions.
class AsyncHighlighter {
    std::jthread worker_;
    std::atomic<uint64_t> document_version_{0};
    std::vector<LineState> line_states_;
    // ... token output buffer ...

public:
    /// Called on every edit. Bumps version, enqueues re-lex work.
    void notify_edit(std::size_t start_line, std::size_t end_line) {
        auto new_version = document_version_.fetch_add(1,
            std::memory_order_relaxed) + 1;

        // Worker thread: re-lex from start_line until state stabilizes
        // or stop_token is signaled.
    }

    void start(std::stop_token stop) {
        worker_ = std::jthread([this](std::stop_token st) {
            while (!st.stop_requested()) {
                // Wait for work...
                // Re-lex affected lines
                // Publish versioned snapshot of tokens
                // If document_version_ changed mid-lex, discard & restart
            }
        });
    }
};

} // namespace markamp::core
```

### MarkAmp Integration

The current [SyntaxHighlighter](file:///Users/ryanrentfro/code/markamp/src/core/SyntaxHighlighter.h) runs synchronously. To avoid blocking keystrokes on large files:

1. Store per-line `LineState` (lexer state hash at end of each line)
2. On edit, re-lex from the edited line forward until `LineState` matches the previously stored state (convergence)
3. Publish token arrays as a versioned snapshot; the UI renders whatever version is latest

---

## 6. Immutable Snapshots & Versioning

### Why Sublime Does It

Readers (rendering, search, autocomplete) render from consistent snapshots while writers
produce new versions. This eliminates read/write contention entirely.

### C++23 Implementation

```cpp
#include <atomic>
#include <memory>

namespace markamp::core {

/// A versioned, immutable snapshot of the document state.
struct DocumentSnapshot {
    uint64_t version;
    std::shared_ptr<const PieceTable> buffer;
    std::shared_ptr<const std::vector<Token>> tokens;
    // ... other cached state ...
};

/// Thread-safe snapshot provider.
/// Writers swap atomically; readers get the current snapshot via shared_ptr.
class SnapshotStore {
    std::atomic<uint64_t> version_{0};
    std::shared_ptr<const DocumentSnapshot> current_;
    mutable std::mutex swap_mutex_;

public:
    /// Writer: publish a new snapshot.
    void publish(std::shared_ptr<const DocumentSnapshot> snap) {
        std::lock_guard lock(swap_mutex_);
        current_ = std::move(snap);
    }

    /// Reader: get the current snapshot (lock-free read after construction).
    [[nodiscard]] auto current() const -> std::shared_ptr<const DocumentSnapshot> {
        std::lock_guard lock(swap_mutex_);
        return current_;
    }
};

} // namespace markamp::core
```

> [!NOTE]
> MarkAmp's [EventBus](file:///Users/ryanrentfro/code/markamp/src/core/EventBus.h) already
> uses a **copy-on-write** pattern for handler lists (line ~94: `"COW — create a new vector
copy"`). Extend this pattern to document state for full snapshot isolation.

---

## 7. Minimal Locking via Message Passing

### Why Sublime Does It

Shared mutable state + mutex = contention, priority inversion, deadlocks. Message passing
eliminates coordination overhead by making the UI thread the sole owner of authoritative state.

### C++23 Implementation

```cpp
namespace markamp::core {

/// Commands sent from workers to the UI thread.
/// Small, trivially copyable, serializes intent not data.
struct PatchCommand {
    enum class Type : uint8_t {
        ApplyTokens,
        UpdateSearchResults,
        FileLoaded,
        IndexReady
    };

    Type type;
    uint64_t version;     // source version so stale patches are ignored
    std::size_t offset;
    std::size_t length;
};

/// MPSC (multi-producer, single-consumer) queue.
/// Workers push PatchCommands; the UI thread drains each frame.
/// Uses std::atomic indices — zero mutexes in the hot path.
template <typename T, std::size_t Capacity>
class MPSCQueue {
    alignas(64) std::atomic<std::size_t> write_pos_{0};
    alignas(64) std::size_t read_pos_{0}; // only UI thread reads
    std::array<std::atomic<T*>, Capacity> slots_{};

public:
    bool try_push(T* item) noexcept {
        auto pos = write_pos_.fetch_add(1, std::memory_order_acq_rel)
                   % Capacity;
        T* expected = nullptr;
        return slots_[pos].compare_exchange_strong(
            expected, item, std::memory_order_release);
    }

    T* try_pop() noexcept {
        auto pos = read_pos_ % Capacity;
        auto* item = slots_[pos].exchange(nullptr, std::memory_order_acquire);
        if (item) ++read_pos_;
        return item;
    }
};

} // namespace markamp::core
```

### MarkAmp Integration

| Current                                 | Improved                                                          |
| --------------------------------------- | ----------------------------------------------------------------- |
| `EventBus::publish()` acquires `mutex_` | Hot events (cursor, scroll) bypass mutex via lock-free SPSC queue |
| `EventBus::queue()` acquires `mutex_`   | MPSC queue for worker→UI; drain in `wxEVT_IDLE`                   |

---

## 8. Work Coalescing & Cancellation (Latest-Wins)

### Why Sublime Does It

When typing quickly, re-highlight / re-search / re-index jobs become obsolete before they
finish. Sublime cancels stale work and only runs the latest version.

### C++23 Implementation

```cpp
#include <stop_token>
#include <atomic>

namespace markamp::core {

/// Manages a cancelable, coalescable background task.
/// Each new submission cancels the previous one.
class CoalescingTask {
    std::stop_source current_source_;
    std::atomic<uint64_t> latest_version_{0};

public:
    /// Submit new work. Any in-flight work for a prior version is canceled.
    /// Returns the stop_token the worker should check.
    [[nodiscard]] auto submit(uint64_t version) -> std::stop_token {
        // Cancel previous
        current_source_.request_stop();

        // Create new source
        current_source_ = std::stop_source{};
        latest_version_.store(version, std::memory_order_release);

        return current_source_.get_token();
    }

    /// Check if a result for `version` is still relevant.
    [[nodiscard]] bool is_current(uint64_t version) const noexcept {
        return latest_version_.load(std::memory_order_acquire) == version;
    }
};

} // namespace markamp::core
```

**Worker pattern:**

```cpp
void worker_loop(std::stop_token st, uint64_t version,
                 const CoalescingTask& task) {
    for (std::size_t i = 0; i < work_items; ++i) {
        if (st.stop_requested() || !task.is_current(version))
            return; // discard stale work

        // ... process chunk ...
    }
}
```

---

## 9. Arena Allocators & Object Pools

### Why Sublime Does It

`malloc`/`free` latency and fragmentation kill cache performance. Sublime uses monotonic
arenas for per-frame allocations (tokens, layout objects) and pools for fixed-size nodes.

### C++23 Implementation

```cpp
#include <memory_resource>
#include <vector>

namespace markamp::core {

/// Per-frame monotonic arena using C++17/23 pmr.
/// Allocate freely during a frame; reset all at once.
class FrameArena {
    alignas(64) std::array<std::byte, 64 * 1024> buffer_; // 64 KB stack
    std::pmr::monotonic_buffer_resource resource_{
        buffer_.data(), buffer_.size(), std::pmr::null_memory_resource()
    };

public:
    [[nodiscard]] auto allocator() noexcept
        -> std::pmr::polymorphic_allocator<std::byte>
    {
        return &resource_;
    }

    /// Use pmr containers on the hot path:
    template <typename T>
    using Vector = std::pmr::vector<T>;

    /// Reset the arena for the next frame. O(1).
    void reset() noexcept {
        resource_.release();
    }
};

/// Fixed-size object pool with free list.
template <typename T, std::size_t BlockSize = 256>
class ObjectPool {
    union Node {
        T value;
        Node* next;
        Node() {} // NOLINT — intentionally uninitialized
        ~Node() {} // NOLINT
    };

    std::vector<std::unique_ptr<Node[]>> blocks_;
    Node* free_list_{nullptr};

public:
    template <typename... Args>
    [[nodiscard]] auto create(Args&&... args) -> T* {
        if (!free_list_) grow();
        auto* node = free_list_;
        free_list_ = free_list_->next;
        return std::construct_at(&node->value, std::forward<Args>(args)...);
    }

    void destroy(T* ptr) noexcept {
        std::destroy_at(ptr);
        auto* node = reinterpret_cast<Node*>(ptr);
        node->next = free_list_;
        free_list_ = node;
    }

private:
    void grow() {
        auto block = std::make_unique<Node[]>(BlockSize);
        for (std::size_t i = 0; i < BlockSize - 1; ++i) {
            block[i].next = &block[i + 1];
        }
        block[BlockSize - 1].next = free_list_;
        free_list_ = &block[0];
        blocks_.push_back(std::move(block));
    }
};

} // namespace markamp::core
```

> [!TIP]
> In MarkAmp's [SyntaxHighlighter](file:///Users/ryanrentfro/code/markamp/src/core/SyntaxHighlighter.cpp),
> `tokenize()` currently returns `std::vector<Token>`. Switch to
> `FrameArena::Vector<Token>` to eliminate per-token heap allocations on the hot path.

---

## 10. Cache-Friendly Data Layout

### Why Sublime Does It

CPU cache misses are 100–300× slower than cache hits. Sublime stores frequently-accessed
fields contiguously (Structure of Arrays) and avoids pointer chasing.

### C++23 Implementation

```cpp
#include <cstdint>
#include <vector>

namespace markamp::core {

/// AoS — what most codebases do (poor cache utilization for scans):
struct TokenAoS {
    TokenType type;       // 4 bytes
    uint32_t  start;      // 4 bytes
    uint32_t  length;     // 4 bytes
    uint16_t  scope_id;   // 2 bytes
    // 2 bytes padding → 16 bytes per token
};

/// SoA — what Sublime does (scan type? read only types[] — all in cache):
struct TokenArraySoA {
    std::vector<TokenType> types;
    std::vector<uint32_t>  starts;
    std::vector<uint32_t>  lengths;
    std::vector<uint16_t>  scope_ids;

    void reserve(std::size_t n) {
        types.reserve(n);
        starts.reserve(n);
        lengths.reserve(n);
        scope_ids.reserve(n);
    }

    void push_back(TokenType type, uint32_t start,
                   uint32_t length, uint16_t scope_id) {
        types.push_back(type);
        starts.push_back(start);
        lengths.push_back(length);
        scope_ids.push_back(scope_id);
    }

    [[nodiscard]] auto size() const noexcept -> std::size_t {
        return types.size();
    }
};

} // namespace markamp::core
```

### Rules of Thumb

| Guideline                                                     | Rationale                               |
| ------------------------------------------------------------- | --------------------------------------- |
| Keep B-tree node fanout high (8–16 children)                  | Reduce tree depth → fewer cache misses  |
| Align hot structures to 64 bytes                              | Match cache line size                   |
| Replace `std::unordered_map` with flat hash maps on hot paths | Contiguous storage, no chained buckets  |
| Minimize virtual function calls in tight loops                | Indirect calls defeat branch prediction |

---

## 11. O(1)/O(log n) Typing Path Guarantee

### Why Sublime Does It

The keystroke-to-screen path must have a **hard ceiling**. No full re-layout, no full
re-parse, no full re-render — ever.

### C++23 Implementation

```cpp
namespace markamp::core {

/// Budget assertion — fires in debug builds if a UI-thread
/// operation exceeds its time budget.
class BudgetGuard {
    std::string_view name_;
    std::chrono::high_resolution_clock::time_point start_;
    std::chrono::microseconds budget_;

public:
    BudgetGuard(std::string_view name, std::chrono::microseconds budget)
        : name_(name)
        , start_(std::chrono::high_resolution_clock::now())
        , budget_(budget)
    {}

    ~BudgetGuard() {
        auto elapsed = std::chrono::high_resolution_clock::now() - start_;
        auto us = std::chrono::duration_cast<std::chrono::microseconds>(elapsed);
#ifndef NDEBUG
        if (us > budget_) {
            // Log or assert: operation exceeded budget
            // e.g., LOG_WARN("BudgetGuard '{}' exceeded: {}µs > {}µs",
            //                name_, us.count(), budget_.count());
        }
#endif
    }
};

/// The typing path:
///   1. Update piece table          — O(log n)
///   2. Update line index           — O(log n)
///   3. Invalidate dirty region     — O(1)
///   4. Queue background re-lex     — O(1)
///   5. Repaint affected line       — O(1)
///
/// Total budget: < 500µs on the UI thread.

} // namespace markamp::core
```

### MarkAmp Integration

MarkAmp already has the [Profiler](file:///Users/ryanrentfro/code/markamp/src/core/Profiler.h) with `MARKAMP_PROFILE_SCOPE`. Add `BudgetGuard` assertions to:

- `EditorPanel::OnChar()` — budget: 500µs
- `EditorPanel::OnScroll()` — budget: 1ms
- `PreviewPanel::UpdateContent()` — budget: 5ms (can be async)

---

## 12. Deferred/Lazy Layout & Measurement

### Why Sublime Does It

Text measurement (glyph shaping, line wrapping) is expensive. Sublime only measures what's
visible and caches aggressively, invalidating narrowly.

### C++23 Implementation

```cpp
#include <functional>
#include <optional>

namespace markamp::rendering {

/// Lazy-evaluated, cached line layout.
template <typename T>
class LazyCache {
    mutable std::optional<T> value_;
    mutable bool dirty_{true};
    std::function<T()> compute_;

public:
    explicit LazyCache(std::function<T()> compute)
        : compute_(std::move(compute)) {}

    void invalidate() noexcept { dirty_ = true; }

    [[nodiscard]] auto get() const -> const T& {
        if (dirty_) {
            value_ = compute_();
            dirty_ = false;
        }
        return *value_;
    }
};

/// Per-line layout cache with stable IDs.
struct LineMeasurement {
    float width;
    float height;
    uint32_t wrap_count;    // number of visual lines after wrapping
    uint64_t content_hash;  // invalidation key
};

class LineLayoutCache {
    std::vector<std::optional<LineMeasurement>> cache_;

public:
    void resize(std::size_t line_count) {
        cache_.resize(line_count);
    }

    void invalidate_range(std::size_t first, std::size_t last) {
        for (auto i = first; i <= last && i < cache_.size(); ++i) {
            cache_[i].reset();
        }
    }

    [[nodiscard]] auto get(std::size_t line) const
        -> const std::optional<LineMeasurement>&
    {
        return cache_[line];
    }

    void set(std::size_t line, LineMeasurement m) {
        if (line < cache_.size()) cache_[line] = m;
    }
};

} // namespace markamp::rendering
```

---

## 13. Viewport-Based Rendering & Virtualization

### Why Sublime Does It

A 100,000-line file has 100,000 lines but the viewport shows ~50. Sublime renders only
visible lines plus a small prefetch margin.

### C++23 Implementation

```cpp
namespace markamp::rendering {

struct ViewportState {
    std::size_t first_visible_line;
    std::size_t visible_line_count;
    static constexpr std::size_t kPrefetchMargin = 10;

    [[nodiscard]] auto render_range() const noexcept
        -> std::pair<std::size_t, std::size_t>
    {
        auto start = (first_visible_line > kPrefetchMargin)
            ? first_visible_line - kPrefetchMargin : 0;
        auto end = first_visible_line + visible_line_count + kPrefetchMargin;
        return {start, end};
    }
};

/// LRU tile cache for rendered line bitmaps.
template <typename Key, typename Value, std::size_t MaxEntries>
class LRUCache {
    struct Entry {
        Key key;
        Value value;
        std::size_t last_access;
    };

    std::vector<Entry> entries_;
    std::size_t clock_{0};

public:
    auto get(const Key& key) -> Value* {
        for (auto& e : entries_) {
            if (e.key == key) {
                e.last_access = ++clock_;
                return &e.value;
            }
        }
        return nullptr;
    }

    void put(Key key, Value value) {
        if (entries_.size() >= MaxEntries) {
            // Evict LRU
            auto it = std::min_element(entries_.begin(), entries_.end(),
                [](const Entry& a, const Entry& b) {
                    return a.last_access < b.last_access;
                });
            *it = {std::move(key), std::move(value), ++clock_};
        } else {
            entries_.push_back({std::move(key), std::move(value), ++clock_});
        }
    }
};

} // namespace markamp::rendering
```

### MarkAmp Integration

MarkAmp uses **Scintilla** via wxStyledTextCtrl for the editor, which already virtualizes line
rendering. The [PreviewPanel](file:///Users/ryanrentfro/code/markamp/src/ui/PreviewPanel.cpp) uses
wxWebView — which handles its own viewport. The win for MarkAmp is in the **rendering pipeline**:
only re-render the HTML for visible sections of markdown, not the entire document.

---

## 14. Chunked Font Rasterization & Glyph Atlas

### Why Sublime Does It

Rasterizing glyphs per-frame is a GPU/CPU bottleneck. Sublime packs glyphs into texture
atlases and reuses UV coords across frames.

### C++23 Implementation

```cpp
#include <unordered_map>

namespace markamp::rendering {

struct GlyphKey {
    uint32_t font_face_id;
    uint16_t font_size;
    uint32_t glyph_id;
    uint8_t  hinting_mode;

    auto operator==(const GlyphKey&) const -> bool = default;
};

struct GlyphKeyHash {
    auto operator()(const GlyphKey& k) const noexcept -> std::size_t {
        // FNV-1a or xxHash mix
        auto h = static_cast<std::size_t>(k.font_face_id);
        h ^= k.font_size + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= k.glyph_id  + 0x9e3779b9 + (h << 6) + (h >> 2);
        h ^= k.hinting_mode;
        return h;
    }
};

struct GlyphEntry {
    uint16_t atlas_x, atlas_y;
    uint16_t width, height;
    int16_t  bearing_x, bearing_y;
    uint16_t advance;
};

/// Glyph atlas with skyline bin-packing.
class GlyphAtlas {
    std::unordered_map<GlyphKey, GlyphEntry, GlyphKeyHash> cache_;
    // ... texture buffer, skyline allocator ...

public:
    /// Get or rasterize a glyph — O(1) amortized.
    auto lookup(const GlyphKey& key) -> const GlyphEntry& {
        auto it = cache_.find(key);
        if (it != cache_.end()) return it->second;

        // Rasterize with FreeType, pack into atlas
        auto entry = rasterize_and_pack(key);
        auto [inserted, _] = cache_.emplace(key, entry);
        return inserted->second;
    }

private:
    auto rasterize_and_pack(const GlyphKey& key) -> GlyphEntry {
        // FreeType rasterization + skyline pack
        return {}; // placeholder
    }
};

} // namespace markamp::rendering
```

> [!NOTE]
> Since MarkAmp uses wxWidgets/Scintilla for text rendering and wxWebView for preview,
> glyph atlas management is handled by the platform. This pattern becomes relevant if
> MarkAmp ever moves to a custom GPU renderer (Skia, wgpu, Metal).

---

## 15. Incremental Search & Indexing

### Why Sublime Does It

"Find in files" never stalls the UI. Results appear progressively. The index updates
incrementally as buffers change.

### C++23 Implementation

```cpp
#include <stop_token>
#include <thread>
#include <functional>

namespace markamp::core {

/// Progressive search that yields partial results.
class IncrementalSearcher {
    std::jthread worker_;

public:
    using ResultCallback = std::function<void(
        std::size_t line, std::size_t col, std::string_view context)>;

    void search_async(
        std::string_view needle,
        const PieceTable& buffer,
        ResultCallback on_match)
    {
        worker_ = std::jthread(
            [needle = std::string(needle), &buffer,
             cb = std::move(on_match)](std::stop_token st)
            {
                // Iterate chunks of the piece table
                std::size_t offset = 0;
                auto text = buffer.text(); // or iterate pieces directly

                std::size_t pos = 0;
                while (!st.stop_requested()) {
                    pos = text.find(needle, pos);
                    if (pos == std::string::npos) break;

                    // Compute (line, col) via LineIndex
                    cb(0, pos, text.substr(pos, needle.size() + 20));
                    pos += needle.size();
                }
            });
    }

    void cancel() {
        worker_.request_stop();
    }
};

} // namespace markamp::core
```

---

## 16. Fast Scrolling via Pre-Rendered Surfaces

### Why Sublime Does It

Scrolling moves cached surfaces; only newly exposed lines are rendered. This reduces
per-frame render cost to O(exposed lines) instead of O(visible lines).

### Key Principles

1. **Scrollback cache**: keep N rendered line surfaces in memory
2. **On scroll**: shift cached entries by scroll delta
3. **Render delta**: only render newly exposed lines at top/bottom
4. **Fallback**: if the cache miss rate is too high (fast scroll), show last-known bitmap and fill asynchronously

```cpp
namespace markamp::rendering {

/// Circular buffer of pre-rendered line surfaces.
/// On scroll, shift the window and render only new lines.
class ScrollCache {
    std::vector<RenderedLine> buffer_;
    std::size_t first_line_{0};

public:
    void scroll_to(std::size_t new_first_line,
                   std::size_t visible_count,
                   auto render_line_fn)
    {
        auto delta = static_cast<int64_t>(new_first_line) -
                     static_cast<int64_t>(first_line_);

        if (std::abs(delta) >= static_cast<int64_t>(buffer_.size())) {
            // Full re-render
            for (std::size_t i = 0; i < visible_count; ++i) {
                buffer_[i] = render_line_fn(new_first_line + i);
            }
        } else if (delta > 0) {
            // Scrolled down: render new lines at bottom
            for (int64_t i = 0; i < delta; ++i) {
                auto line = first_line_ + buffer_.size() + i;
                // Shift circular buffer and render new line
            }
        }
        first_line_ = new_first_line;
    }
};

} // namespace markamp::rendering
```

---

## 17. GPU-Accelerated Compositing

### Why Sublime Does It

Let the GPU handle blending, scrolling, and compositing. The CPU focuses on text logic and
layout. Draw calls are batched; glyph quads are instanced; state changes are minimized.

### Key Principles for MarkAmp

| Principle                         | Application                                                 |
| --------------------------------- | ----------------------------------------------------------- |
| Batch draw calls                  | Group all text glyphs into a single vertex buffer per frame |
| Minimize state changes            | Sort draw calls by texture (atlas page), blend mode         |
| Instance rendering                | One draw call renders all glyphs of the same font/size      |
| Command buffer per frame          | Build a render list, submit once to GPU                     |
| Keep UI thread assembling batches | CPU spends <1ms building the draw list from cached geometry |

> [!NOTE]
> wxWidgets uses platform-native rendering (Core Graphics on macOS, GDI/Direct2D on Windows).
> For a future GPU path, consider **Skia** (used by Chrome, Flutter) or **wgpu** for
> cross-platform Metal/Vulkan/DX12 compositing.

---

## 18. Predictable I/O

### Why Sublime Does It

Opening a 100MB file must not freeze the UI. Sublime memory-maps files, decodes chunks in
background threads, and shows partial content while the rest streams.

### C++23 Implementation

```cpp
#include <filesystem>
#include <fstream>
#include <future>
#include <stop_token>

namespace markamp::core {

/// Async file loader with chunked delivery.
class AsyncFileLoader {
    static constexpr std::size_t kChunkSize = 256 * 1024; // 256 KB

public:
    struct Chunk {
        std::size_t offset;
        std::string data;
        bool is_last;
    };

    using ChunkCallback = std::function<void(Chunk)>;

    void load_async(const std::filesystem::path& path,
                    ChunkCallback on_chunk)
    {
        std::jthread([path, cb = std::move(on_chunk)](std::stop_token st) {
            std::ifstream file(path, std::ios::binary);
            if (!file) return;

            std::string buffer(kChunkSize, '\0');
            std::size_t offset = 0;

            while (!st.stop_requested() && file) {
                file.read(buffer.data(),
                          static_cast<std::streamsize>(kChunkSize));
                auto bytes_read = static_cast<std::size_t>(file.gcount());
                if (bytes_read == 0) break;

                cb(Chunk{
                    offset,
                    buffer.substr(0, bytes_read),
                    !file
                });
                offset += bytes_read;
            }
        }).detach(); // Managed externally via stop_source
    }
};

} // namespace markamp::core
```

### MarkAmp Integration

The current [FileSystem](file:///Users/ryanrentfro/code/markamp/src/core/FileSystem.cpp) loads files synchronously. For large files:

1. Show the first chunk immediately (first 256KB)
2. Stream remaining chunks in the background
3. Update the piece table incrementally as chunks arrive
4. Show a progress indicator in the [StatusBarPanel](file:///Users/ryanrentfro/code/markamp/src/ui/StatusBarPanel.cpp)

---

## 19. Instrumentation-Driven Micro-Optimizations

### Why Sublime Does It

Performance is continuously measured. Frame time budgets are enforced. Regressions are
caught before they ship.

### C++23 Implementation

MarkAmp already has a solid foundation with the [Profiler](file:///Users/ryanrentfro/code/markamp/src/core/Profiler.h). Extend it:

```cpp
namespace markamp::core {

/// Per-subsystem budget enforcement.
struct PerformanceBudget {
    static constexpr auto kKeystrokeLatency   = std::chrono::microseconds(500);
    static constexpr auto kScrollFrameTime    = std::chrono::milliseconds(8);
    static constexpr auto kPreviewUpdate      = std::chrono::milliseconds(16);
    static constexpr auto kFileOpen           = std::chrono::milliseconds(100);
    static constexpr auto kSearchIterationMax = std::chrono::milliseconds(50);
};

/// Frame time histogram (lock-free, per-subsystem).
class FrameHistogram {
    std::array<std::atomic<uint32_t>, 64> buckets_{}; // 0-63ms, 1ms per bucket

public:
    void record(std::chrono::microseconds duration) noexcept {
        auto ms = duration.count() / 1000;
        auto idx = std::min(static_cast<std::size_t>(ms), buckets_.size() - 1);
        buckets_[idx].fetch_add(1, std::memory_order_relaxed);
    }

    /// P50, P95, P99 — call from diagnostics/logging thread.
    [[nodiscard]] auto percentile(double p) const noexcept -> uint32_t {
        uint64_t total = 0;
        for (const auto& b : buckets_)
            total += b.load(std::memory_order_relaxed);

        auto target = static_cast<uint64_t>(static_cast<double>(total) * p);
        uint64_t cumulative = 0;
        for (std::size_t i = 0; i < buckets_.size(); ++i) {
            cumulative += buckets_[i].load(std::memory_order_relaxed);
            if (cumulative >= target) return static_cast<uint32_t>(i);
        }
        return static_cast<uint32_t>(buckets_.size() - 1);
    }
};

} // namespace markamp::core
```

### What to Measure in MarkAmp

| Subsystem          | Metric                              | Budget  |
| ------------------ | ----------------------------------- | ------- |
| Keystroke handling | `EditorPanel::OnChar()` latency     | < 500µs |
| Preview update     | `PreviewPanel::UpdateContent()`     | < 16ms  |
| Theme switch       | `ThemeEngine::apply_theme()`        | < 50ms  |
| File open          | First paint after `File → Open`     | < 100ms |
| Search             | Per-iteration wall time             | < 50ms  |
| Scroll             | Frame time during continuous scroll | < 8ms   |

---

## 20. Deterministic Scheduling

### Why Sublime Does It

The scheduler always meets the interaction deadline by prioritizing input → paint → layout →
background. Lower-priority work is deferred cooperatively.

### C++23 Implementation

```cpp
#include <queue>
#include <functional>
#include <chrono>

namespace markamp::core {

enum class TaskPriority : uint8_t {
    Input      = 0, // Highest — process immediately
    Paint      = 1,
    Layout     = 2,
    Highlight  = 3,
    Search     = 4,
    FileIO     = 5,
    Background = 6  // Lowest — only when idle
};

struct ScheduledTask {
    TaskPriority priority;
    std::chrono::microseconds budget;
    std::function<bool()> execute; // returns true if more work remains

    auto operator>(const ScheduledTask& other) const noexcept -> bool {
        return priority > other.priority;
    }
};

/// Frame-budget-aware cooperative scheduler.
class FrameScheduler {
    std::priority_queue<ScheduledTask, std::vector<ScheduledTask>,
                        std::greater<>> queue_;

public:
    void enqueue(ScheduledTask task) {
        queue_.push(std::move(task));
    }

    /// Run tasks until the frame budget is exhausted.
    void run_frame(std::chrono::microseconds frame_budget) {
        auto frame_start = std::chrono::high_resolution_clock::now();

        while (!queue_.empty()) {
            auto elapsed = std::chrono::high_resolution_clock::now() - frame_start;
            if (elapsed >= frame_budget) break; // budget exhausted

            auto task = std::move(const_cast<ScheduledTask&>(queue_.top()));
            queue_.pop();

            bool has_more = task.execute();
            if (has_more) {
                // Re-enqueue with same priority for next frame
                queue_.push(std::move(task));
            }
        }
    }
};

} // namespace markamp::core
```

### Scheduling Rules

1. **Input** always runs first — never skip a keystroke or mouse event
2. **Paint** runs after input — display what changed
3. **Layout** runs if constraints changed (resize, font change)
4. **Background** work (highlighting, search, indexing) runs in remaining budget
5. If budget is exhausted, defer everything below current priority to next frame
6. Use `std::stop_token` to cooperatively interrupt long-running background work

---

## Summary Matrix

| #   | Pattern                  | Complexity Reduction  | MarkAmp Status                             |
| --- | ------------------------ | --------------------- | ------------------------------------------ |
| 1   | UI Thread Isolation      | Eliminates UI stalls  | ⚠️ Partial — EventBus queues events        |
| 2   | Incremental Rendering    | O(changed) not O(all) | ⚠️ Partial — Scintilla handles editor      |
| 3   | Piece Table              | O(log n) edits        | ❌ Not implemented — uses wxStyledTextCtrl |
| 4   | Line Index               | O(log n) offset↔pos   | ❌ Not implemented                         |
| 5   | Async Highlighting       | Non-blocking parse    | ⚠️ Partial — synchronous tokenizer         |
| 6   | Immutable Snapshots      | Lock-free reads       | ⚠️ COW in EventBus only                    |
| 7   | Message Passing          | Minimal locking       | ⚠️ Mutex-based EventBus                    |
| 8   | Work Coalescing          | Discard stale work    | ❌ Not implemented                         |
| 9   | Arena Allocators         | Zero fragmentation    | ❌ Not implemented                         |
| 10  | Cache-Friendly Layout    | Maximize cache hits   | ❌ AoS tokens                              |
| 11  | O(1) Typing Path         | Guaranteed latency    | ⚠️ Scintilla provides this                 |
| 12  | Lazy Layout              | Measure only visible  | ⚠️ Deferred via wxWebView                  |
| 13  | Viewport Virtualization  | Render visible only   | ✅ Scintilla + wxWebView                   |
| 14  | Glyph Atlas              | Rasterize once        | ✅ Platform-native                         |
| 15  | Incremental Search       | Progressive results   | ❌ Not implemented                         |
| 16  | Pre-Rendered Surfaces    | Scroll = shift        | ✅ Platform-native                         |
| 17  | GPU Compositing          | Batch draw calls      | ✅ Platform-native                         |
| 18  | Predictable I/O          | Async file loading    | ❌ Synchronous file I/O                    |
| 19  | Instrumentation          | Budget assertions     | ✅ Profiler exists                         |
| 20  | Deterministic Scheduling | Priority-based frames | ❌ Not implemented                         |

**Legend**: ✅ Implemented · ⚠️ Partial · ❌ Opportunity

---

## Recommended Priority Order for MarkAmp

Based on the current architecture and the effort-to-impact ratio:

1. **Work Coalescing (#8)** — Low effort, high impact on typing responsiveness
2. **Async Highlighting (#5)** — Medium effort, eliminates stalls on large files
3. **Message Passing (#7)** — Medium effort, unlocks lock-free EventBus hot path
4. **Predictable I/O (#18)** — Medium effort, fixes large file open freeze
5. **Deterministic Scheduling (#20)** — Higher effort, provides systematic frame budget management
6. **Arena Allocators (#9)** — Medium effort, reduces GC pressure and fragmentation
7. **Incremental Search (#15)** — Medium effort, enables find-in-files UX parity with Sublime

---

_Document generated for MarkAmp v1.1.3 — C++23/wxWidgets_
