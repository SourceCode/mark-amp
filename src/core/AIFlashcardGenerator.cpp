// ============================================================================
// File: src/core/AIFlashcardGenerator.cpp
// Phase 26: AI Integration — flashcard generation implementation
// ============================================================================

#include "AIFlashcardGenerator.h"

#include "AIService.h"

#include <fmt/format.h>
#include <sstream>

namespace markamp::core
{

AIFlashcardGenerator::AIFlashcardGenerator(AIService& ai_service)
    : ai_service_(ai_service)
{
}

auto AIFlashcardGenerator::generate_cards(const std::string& document_content,
                                          int count,
                                          double difficulty) -> std::vector<GeneratedCard>
{
    std::string difficulty_desc;
    if (difficulty < 0.3)
    {
        difficulty_desc = "basic/introductory";
    }
    else if (difficulty < 0.7)
    {
        difficulty_desc = "intermediate";
    }
    else
    {
        difficulty_desc = "advanced/challenging";
    }

    const std::string prompt =
        fmt::format("Generate exactly {} flashcards from the following content. "
                    "Difficulty level: {}. "
                    "Format each card as:\n"
                    "Q: [question]\n"
                    "A: [answer]\n\n"
                    "Focus on key concepts, definitions, and important details.",
                    count,
                    difficulty_desc);

    const auto response = ai_service_.execute_action(AIAction::Custom, document_content, prompt);

    if (!response.success || response.content.empty())
    {
        return {};
    }

    return parse_cards(response.content, CardType::kBasic, difficulty);
}

auto AIFlashcardGenerator::generate_cloze(const std::string& document_content, int count)
    -> std::vector<GeneratedCard>
{
    const std::string prompt =
        fmt::format("Generate exactly {} cloze deletion flashcards from the following content. "
                    "Format each card as:\n"
                    "C: [sentence with {{{{c1::hidden term}}}} format]\n\n"
                    "The hidden term should be a key concept or important detail.",
                    count);

    const auto response = ai_service_.execute_action(AIAction::Custom, document_content, prompt);

    if (!response.success || response.content.empty())
    {
        return {};
    }

    return parse_cards(response.content, CardType::kCloze, 0.5);
}

auto AIFlashcardGenerator::parse_cards(const std::string& response,
                                       CardType card_type,
                                       double difficulty) -> std::vector<GeneratedCard>
{
    std::vector<GeneratedCard> cards;
    std::istringstream stream(response);
    std::string line;

    std::string current_front;
    bool expecting_answer = false;

    while (std::getline(stream, line))
    {
        // Trim whitespace.
        while (!line.empty() && std::isspace(line.front()) != 0)
        {
            line.erase(line.begin());
        }
        while (!line.empty() && std::isspace(line.back()) != 0)
        {
            line.pop_back();
        }
        if (line.empty())
        {
            continue;
        }

        if (card_type == CardType::kCloze && line.size() > 3 &&
            (line.substr(0, 2) == "C:" || line.substr(0, 2) == "c:"))
        {
            // Cloze card format: C: sentence with {{c1::hidden}}
            std::string cloze_text = line.substr(2);
            while (!cloze_text.empty() && cloze_text.front() == ' ')
            {
                cloze_text.erase(cloze_text.begin());
            }

            GeneratedCard card;
            card.front = cloze_text;
            card.back = cloze_text; // Full text is the "answer"
            card.card_type = CardType::kCloze;
            card.difficulty = difficulty;
            cards.push_back(std::move(card));
        }
        else if (line.size() > 3 && (line.substr(0, 2) == "Q:" || line.substr(0, 2) == "q:"))
        {
            // Q&A format: Q: question
            current_front = line.substr(2);
            while (!current_front.empty() && current_front.front() == ' ')
            {
                current_front.erase(current_front.begin());
            }
            expecting_answer = true;
        }
        else if (expecting_answer && line.size() > 3 &&
                 (line.substr(0, 2) == "A:" || line.substr(0, 2) == "a:"))
        {
            // A: answer
            std::string answer = line.substr(2);
            while (!answer.empty() && answer.front() == ' ')
            {
                answer.erase(answer.begin());
            }

            GeneratedCard card;
            card.front = current_front;
            card.back = answer;
            card.card_type = CardType::kBasic;
            card.difficulty = difficulty;
            cards.push_back(std::move(card));

            current_front.clear();
            expecting_answer = false;
        }
    }

    return cards;
}

} // namespace markamp::core
