#pragma once

#include "FileNode.h"

#include <string>
#include <utility>
#include <vector>

namespace markamp::core
{

/// Returns the default sample file tree matching the MarkAmp reference project.
[[nodiscard]] auto get_sample_file_tree() -> FileNode;

// Phase 35: Extended sample content generators

/// Returns a comprehensive Markdown showcase document.
[[nodiscard]] auto get_sample_markdown_showcase() -> std::string;

/// Returns a sample canvas board as JSON string with diverse object types.
[[nodiscard]] auto get_sample_canvas_board() -> std::string;

/// Returns a sample notebook with Markdown and code cells.
[[nodiscard]] auto get_sample_notebook() -> std::string;

/// Returns a vector of 10+ linked documents for knowledge graph demonstration.
[[nodiscard]] auto get_sample_knowledge_base() -> std::vector<std::pair<std::string, std::string>>;

/// Returns a sample flashcard deck as a vector of Q/A pairs.
[[nodiscard]] auto get_sample_flashcard_deck() -> std::vector<std::pair<std::string, std::string>>;

} // namespace markamp::core
