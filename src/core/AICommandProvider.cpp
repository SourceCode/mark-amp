/// @file AICommandProvider.cpp
/// @brief Phase 26: AI Integration — AI command provider implementation.

#include "AICommandProvider.h"

#include <algorithm>
#include <ranges>
#include <set>

namespace markamp::core
{

AICommandProvider::AICommandProvider()
{
    register_commands();
}

auto AICommandProvider::commands() const -> const std::vector<AICommandInfo>&
{
    return commands_;
}

auto AICommandProvider::find_command(const std::string& command_id) const -> const AICommandInfo*
{
    for (const auto& cmd : commands_)
    {
        if (cmd.id == command_id)
        {
            return &cmd;
        }
    }
    return nullptr;
}

auto AICommandProvider::commands_for_category(const std::string& category) const
    -> std::vector<AICommandInfo>
{
    std::vector<AICommandInfo> result;
    for (const auto& cmd : commands_)
    {
        if (cmd.category == category)
        {
            result.push_back(cmd);
        }
    }
    return result;
}

auto AICommandProvider::categories() const -> std::vector<std::string>
{
    std::set<std::string> unique_categories;
    for (const auto& cmd : commands_)
    {
        unique_categories.insert(cmd.category);
    }
    return {unique_categories.begin(), unique_categories.end()};
}

auto AICommandProvider::command_count() const -> int
{
    return static_cast<int>(commands_.size());
}

auto AICommandProvider::selection_commands() const -> std::vector<AICommandInfo>
{
    std::vector<AICommandInfo> result;
    for (const auto& cmd : commands_)
    {
        if (cmd.requires_selection)
        {
            result.push_back(cmd);
        }
    }
    return result;
}

auto AICommandProvider::global_commands() const -> std::vector<AICommandInfo>
{
    std::vector<AICommandInfo> result;
    for (const auto& cmd : commands_)
    {
        if (!cmd.requires_selection)
        {
            result.push_back(cmd);
        }
    }
    return result;
}

auto AICommandProvider::register_commands() -> void
{
    commands_.clear();

    // Chat
    AICommandInfo chat_cmd;
    chat_cmd.id = "ai.chat";
    chat_cmd.title = "AI: Chat";
    chat_cmd.category = "AI";
    chat_cmd.keybinding = "Cmd+Shift+A";
    chat_cmd.description = "Open AI chat panel for conversation";
    chat_cmd.requires_selection = false;
    commands_.push_back(std::move(chat_cmd));

    // Continue Writing
    AICommandInfo continue_cmd;
    continue_cmd.id = "ai.continue_writing";
    continue_cmd.title = "AI: Continue Writing";
    continue_cmd.category = "AI Writing";
    continue_cmd.keybinding = "Cmd+Shift+Enter";
    continue_cmd.description = "Generate text continuation from cursor position";
    continue_cmd.requires_selection = false;
    commands_.push_back(std::move(continue_cmd));

    // Improve Writing
    AICommandInfo improve_cmd;
    improve_cmd.id = "ai.improve_writing";
    improve_cmd.title = "AI: Improve Writing";
    improve_cmd.category = "AI Writing";
    improve_cmd.keybinding = "";
    improve_cmd.description = "Rewrite selected text with improvements";
    improve_cmd.requires_selection = true;
    commands_.push_back(std::move(improve_cmd));

    // Fix Grammar
    AICommandInfo grammar_cmd;
    grammar_cmd.id = "ai.fix_grammar";
    grammar_cmd.title = "AI: Fix Grammar";
    grammar_cmd.category = "AI Writing";
    grammar_cmd.keybinding = "";
    grammar_cmd.description = "Fix grammar and spelling in selected text";
    grammar_cmd.requires_selection = true;
    commands_.push_back(std::move(grammar_cmd));

    // Summarize
    AICommandInfo summarize_cmd;
    summarize_cmd.id = "ai.summarize";
    summarize_cmd.title = "AI: Summarize";
    summarize_cmd.category = "AI Writing";
    summarize_cmd.keybinding = "";
    summarize_cmd.description = "Create a summary of selected text";
    summarize_cmd.requires_selection = true;
    commands_.push_back(std::move(summarize_cmd));

    // Generate Document
    AICommandInfo generate_cmd;
    generate_cmd.id = "ai.generate_document";
    generate_cmd.title = "AI: Generate Document";
    generate_cmd.category = "AI";
    generate_cmd.keybinding = "";
    generate_cmd.description = "Generate a new document from a topic prompt";
    generate_cmd.requires_selection = false;
    commands_.push_back(std::move(generate_cmd));

    // Suggest Tags
    AICommandInfo tags_cmd;
    tags_cmd.id = "ai.suggest_tags";
    tags_cmd.title = "AI: Suggest Tags";
    tags_cmd.category = "AI Suggestions";
    tags_cmd.keybinding = "";
    tags_cmd.description = "Get AI-suggested tags for current document";
    tags_cmd.requires_selection = false;
    commands_.push_back(std::move(tags_cmd));

    // Suggest Links
    AICommandInfo links_cmd;
    links_cmd.id = "ai.suggest_links";
    links_cmd.title = "AI: Suggest Links";
    links_cmd.category = "AI Suggestions";
    links_cmd.keybinding = "";
    links_cmd.description = "Get AI-suggested wiki-links for current document";
    links_cmd.requires_selection = false;
    commands_.push_back(std::move(links_cmd));

    // Translate
    AICommandInfo translate_cmd;
    translate_cmd.id = "ai.translate";
    translate_cmd.title = "AI: Translate";
    translate_cmd.category = "AI Writing";
    translate_cmd.keybinding = "";
    translate_cmd.description = "Translate selected text to another language";
    translate_cmd.requires_selection = true;
    commands_.push_back(std::move(translate_cmd));

    // Adjust Tone
    AICommandInfo tone_cmd;
    tone_cmd.id = "ai.adjust_tone";
    tone_cmd.title = "AI: Adjust Tone";
    tone_cmd.category = "AI Writing";
    tone_cmd.keybinding = "";
    tone_cmd.description = "Change the writing tone of selected text";
    tone_cmd.requires_selection = true;
    commands_.push_back(std::move(tone_cmd));

    // Generate Flashcards
    AICommandInfo flashcards_cmd;
    flashcards_cmd.id = "ai.generate_flashcards";
    flashcards_cmd.title = "AI: Generate Flashcards";
    flashcards_cmd.category = "AI Suggestions";
    flashcards_cmd.keybinding = "";
    flashcards_cmd.description = "Generate flashcards from current document";
    flashcards_cmd.requires_selection = false;
    commands_.push_back(std::move(flashcards_cmd));

    // Ask About Workspace
    AICommandInfo ask_cmd;
    ask_cmd.id = "ai.ask_workspace";
    ask_cmd.title = "AI: Ask About Workspace";
    ask_cmd.category = "AI";
    ask_cmd.keybinding = "";
    ask_cmd.description = "Ask a question about your workspace content";
    ask_cmd.requires_selection = false;
    commands_.push_back(std::move(ask_cmd));
}

} // namespace markamp::core
