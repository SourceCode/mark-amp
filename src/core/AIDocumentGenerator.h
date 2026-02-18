// ============================================================================
// File: src/core/AIDocumentGenerator.h
// Phase 26: AI Integration — full document generation from prompts
// ============================================================================
#pragma once

#include "AITypes.h"

#include <atomic>
#include <string>

namespace markamp::core
{

class AIService;
class EventBus;

/// Generates entire Markdown documents from a topic and style.
/// Supports synchronous and streaming generation with cancel support.
class AIDocumentGenerator
{
public:
    AIDocumentGenerator(AIService& ai_service, EventBus& event_bus);

    /// Generate a document synchronously. Returns complete content.
    [[nodiscard]] auto generate(const std::string& topic, DocumentStyle style) -> std::string;

    /// Generate a document with streaming callback.
    /// Callback receives (partial_content, is_done).
    auto generate_streaming(const std::string& topic,
                            DocumentStyle style,
                            AIStreamCallback callback) -> void;

    /// Cancel an in-progress generation.
    auto cancel() -> void;

    /// Check if generation is currently in progress.
    [[nodiscard]] auto is_generating() const -> bool;

    /// Get the current generation status.
    [[nodiscard]] auto last_status() const -> GenerationStatus;

    /// Get display name for a style.
    [[nodiscard]] static auto style_name(DocumentStyle style) -> std::string;

private:
    AIService& ai_service_;
    EventBus& event_bus_;
    std::atomic<bool> generating_{false};
    std::atomic<bool> cancel_requested_{false};
    GenerationStatus status_;

    /// Build the system prompt for a document style.
    [[nodiscard]] static auto build_style_prompt(DocumentStyle style) -> std::string;
};

} // namespace markamp::core
