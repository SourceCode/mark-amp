#pragma once

#include <string>
#include <unordered_map>

namespace markamp::ui
{

/// Phase 42 Task 07: Stored scroll position for a file.
struct ScrollPosition
{
    int line{0};
    int column{0};
    double pixel_offset{0.0};
};

/// Phase 42 Task 07: Testable model for scroll position persistence.
///
/// Maintains a file → scroll position map. Not coupled to any persistence
/// format (JSON serialization done externally).
class ScrollPositionStore
{
public:
    /// Save scroll position for a file.
    void save(const std::string& file_path, const ScrollPosition& position);

    /// Restore scroll position for a file. Returns default if not found.
    [[nodiscard]] auto restore(const std::string& file_path) const -> ScrollPosition;

    /// Check if a position is stored for a file.
    [[nodiscard]] auto has(const std::string& file_path) const -> bool;

    /// Remove stored position for a file.
    void remove(const std::string& file_path);

    /// Clear all stored positions.
    void clear();

    /// Get number of stored positions.
    [[nodiscard]] auto count() const -> int;

    /// Set maximum entries (LRU eviction when exceeded).
    void set_max_entries(int max);
    [[nodiscard]] auto max_entries() const -> int;

private:
    std::unordered_map<std::string, ScrollPosition> positions_;
    int max_entries_{1000};

    void evict_if_needed();
};

} // namespace markamp::ui
