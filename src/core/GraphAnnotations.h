/// @file GraphAnnotations.h
/// @brief V9 Phase 16 – Graph node annotations with persistence.

#pragma once

#include <algorithm>
#include <chrono>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

namespace markamp::core
{

// ============================================================================
// Annotation Types
// ============================================================================

/// A user annotation attached to a graph node.
struct GraphAnnotation
{
    std::string id;      ///< Unique annotation ID
    std::string node_id; ///< Node this annotation belongs to
    std::string text;    ///< Annotation content
    std::string color;   ///< Color label (e.g., "yellow", "#FF0000")
    std::string icon;    ///< Optional icon identifier
    std::chrono::system_clock::time_point created_at;
};

// ============================================================================
// GraphAnnotationManager
// ============================================================================

/// Manages annotations on graph nodes with persistence support.
class GraphAnnotationManager
{
public:
    // ── CRUD ──

    /// Add an annotation to a node. Returns the generated annotation ID.
    auto add_annotation(const std::string& node_id,
                        const std::string& text,
                        const std::string& color = "") -> std::string
    {
        GraphAnnotation annotation;
        annotation.id = generate_id();
        annotation.node_id = node_id;
        annotation.text = text;
        annotation.color = color;
        annotation.created_at = std::chrono::system_clock::now();

        const auto id = annotation.id;
        annotations_[id] = std::move(annotation);
        return id;
    }

    /// Remove an annotation by ID. Returns true if found and removed.
    auto remove_annotation(const std::string& annotation_id) -> bool
    {
        return annotations_.erase(annotation_id) > 0;
    }

    /// Update the text of an existing annotation.
    auto update_annotation(const std::string& annotation_id, const std::string& new_text) -> bool
    {
        auto iter = annotations_.find(annotation_id);
        if (iter == annotations_.end())
        {
            return false;
        }
        iter->second.text = new_text;
        return true;
    }

    /// Get all annotations for a specific node.
    [[nodiscard]] auto get_annotations(const std::string& node_id) const
        -> std::vector<GraphAnnotation>
    {
        std::vector<GraphAnnotation> result;
        for (const auto& [id, annotation] : annotations_)
        {
            if (annotation.node_id == node_id)
            {
                result.push_back(annotation);
            }
        }
        return result;
    }

    /// Get all annotations across all nodes.
    [[nodiscard]] auto get_all_annotations() const -> std::vector<GraphAnnotation>
    {
        std::vector<GraphAnnotation> result;
        result.reserve(annotations_.size());
        for (const auto& [id, annotation] : annotations_)
        {
            result.push_back(annotation);
        }
        return result;
    }

    // ── Search ──

    /// Search annotations by text content (case-insensitive substring match).
    [[nodiscard]] auto search_annotations(const std::string& query) const
        -> std::vector<GraphAnnotation>
    {
        std::vector<GraphAnnotation> result;
        const auto query_lower = to_lower(query);

        for (const auto& [id, annotation] : annotations_)
        {
            if (to_lower(annotation.text).find(query_lower) != std::string::npos)
            {
                result.push_back(annotation);
            }
        }
        return result;
    }

    // ── Accessors ──

    [[nodiscard]] auto annotation_count() const -> int
    {
        return static_cast<int>(annotations_.size());
    }

    void clear_all()
    {
        annotations_.clear();
    }

    // ── Persistence ──

    /// Serialize all annotations to a string format.
    [[nodiscard]] auto serialize() const -> std::string
    {
        std::ostringstream oss;
        for (const auto& [id, annotation] : annotations_)
        {
            const auto ts = std::chrono::system_clock::to_time_t(annotation.created_at);
            oss << "ANNOTATION\n"
                << "id=" << annotation.id << "\n"
                << "node_id=" << annotation.node_id << "\n"
                << "text=" << annotation.text << "\n"
                << "color=" << annotation.color << "\n"
                << "icon=" << annotation.icon << "\n"
                << "created_at=" << ts << "\n"
                << "---\n";
        }
        return oss.str();
    }

    /// Deserialize annotations from a string.
    void deserialize(const std::string& data)
    {
        annotations_.clear();
        std::istringstream iss(data);
        std::string line;

        GraphAnnotation current;
        bool in_annotation = false;

        while (std::getline(iss, line))
        {
            if (line == "ANNOTATION")
            {
                in_annotation = true;
                current = GraphAnnotation{};
                continue;
            }
            if (line == "---" && in_annotation)
            {
                if (!current.id.empty())
                {
                    annotations_[current.id] = std::move(current);
                }
                in_annotation = false;
                current = GraphAnnotation{};
                continue;
            }
            if (!in_annotation)
            {
                continue;
            }

            const auto eq_pos = line.find('=');
            if (eq_pos == std::string::npos)
            {
                continue;
            }
            const auto key = line.substr(0, eq_pos);
            const auto value = line.substr(eq_pos + 1);

            if (key == "id")
            {
                current.id = value;
            }
            else if (key == "node_id")
            {
                current.node_id = value;
            }
            else if (key == "text")
            {
                current.text = value;
            }
            else if (key == "color")
            {
                current.color = value;
            }
            else if (key == "icon")
            {
                current.icon = value;
            }
            else if (key == "created_at")
            {
                const auto ts = std::stol(value);
                current.created_at = std::chrono::system_clock::from_time_t(ts);
            }
        }
    }

private:
    std::unordered_map<std::string, GraphAnnotation> annotations_;
    int next_id_{0};

    auto generate_id() -> std::string
    {
        return "ann_" + std::to_string(++next_id_);
    }

    [[nodiscard]] static auto to_lower(const std::string& str) -> std::string
    {
        std::string result = str;
        std::transform(result.begin(),
                       result.end(),
                       result.begin(),
                       [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
        return result;
    }
};

} // namespace markamp::core
