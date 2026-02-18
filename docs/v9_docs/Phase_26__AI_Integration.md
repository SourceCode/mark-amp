# Phase 26: AI Integration

## Overview
AIService (82-line header, 336-line implementation), AIPanel, and AITypes exist in core. The AI infrastructure defines prompt types, model selection, and response handling but is not wired to the editor, canvas, or any workspace surface. This phase connects AI capabilities to user workflows across all surfaces.

## Prerequisites
- Phase 07 (Editor core for AI-assisted editing)
- Phase 11 (Canvas for AI-assisted canvas operations)
- Phase 19 (Code intelligence for AI-enhanced diagnostics)

## Tasks

### Task 1: Wire AIService Provider Configuration
**Files:** `src/core/AIService.cpp`, `src/core/AIService.h`, `src/core/AITypes.h`
**Description:** AIService exists. Wire provider configuration: API key management, model selection, endpoint configuration. Support OpenAI, Anthropic, and local models (Ollama).
**Acceptance Criteria:**
- Provider settings in preferences: API key, model, endpoint
- Support: OpenAI (GPT-4), Anthropic (Claude), Ollama (local)
- API key stored securely (OS keychain)
- Model selector with available models per provider
- Connection test: "AI: Test Connection" command
- `AIProviderConfiguredEvent` emitted

### Task 2: Wire AIPanel as Chat Interface
**Files:** `src/ui/AIPanel.cpp`, `src/ui/AIPanel.h`
**Description:** AIPanel exists. Wire it as a chat panel in the tool window area: message input, conversation history, context awareness.
**Acceptance Criteria:**
- AI panel in tool window (bottom or right)
- Message input with send button
- Conversation history with user/AI messages
- Markdown rendering in AI responses
- Code blocks with copy button
- Context: current file content included automatically

### Task 3: Wire AI Writing Assistance in Editor
**Files:** `src/ui/EditorPanel.cpp`, `src/core/AIService.cpp`
**Description:** AI writing actions in editor context menu: "Continue Writing", "Improve Writing", "Fix Grammar", "Simplify", "Expand", "Summarize".
**Acceptance Criteria:**
- Right-click selection: AI submenu with actions
- "Continue Writing" generates text after cursor
- "Improve Writing" rewrites selected text
- "Fix Grammar" corrects grammar and spelling
- "Summarize" creates summary of selection
- Each action shows preview before applying
- Undo support for AI modifications

### Task 4: Wire AI Inline Completion
**Files:** `src/ui/EditorPanel.cpp`, `src/core/AIService.cpp`
**Description:** Ghost text completion: AI suggests continuation of current text. Shown as dimmed text after cursor. Tab to accept, Escape to dismiss.
**Acceptance Criteria:**
- Ghost text appears after pause in typing (1.5 second debounce)
- Suggestion shown as dimmed text
- Tab accepts suggestion
- Escape dismisses
- Partial accept: Cmd+Right accepts word-by-word
- Configurable: enable/disable, debounce time

### Task 5: Wire AI Document Generation
**Files:** `src/core/AIService.cpp`, `src/ui/CommandPalette.cpp`
**Description:** Generate entire documents from prompts: "AI: Generate Document" takes a topic and outline style, produces a Markdown document.
**Acceptance Criteria:**
- "AI: Generate Document" command
- Input: topic, style (article, notes, outline, report)
- Generated document opens in new tab
- Streaming: text appears as generated
- Stop button to cancel generation
- Generation status in status bar

### Task 6: Wire AI Canvas Assistance
**Files:** `src/ui/CanvasWorkspacePanel.cpp`, `src/core/AIService.cpp`
**Description:** AI assistance on canvas: "Generate Mind Map from Text", "Suggest Connections", "Summarize Board Content", "Generate Diagram from Description".
**Acceptance Criteria:**
- "AI: Generate Mind Map" creates nodes from text prompt
- "AI: Suggest Connections" analyzes objects and suggests links
- "AI: Summarize Board" creates text summary of canvas content
- "AI: Generate Diagram" creates shapes/connectors from description
- Generated objects placed on canvas with undo support

### Task 7: Wire AI Search Enhancement
**Files:** `src/core/SearchEngine.cpp`, `src/core/AIService.cpp`
**Description:** AI-enhanced search: semantic search using embeddings, question-answering over workspace content.
**Acceptance Criteria:**
- "AI: Ask About Workspace" for natural language questions
- Semantic search: find conceptually similar content
- Answer synthesized from relevant documents
- Source documents cited in response
- Results include relevance explanation
- Fallback to text search when AI unavailable

### Task 8: Wire AI Tag Suggestions
**Files:** `src/core/AIService.cpp`, `src/core/TagService.cpp`
**Description:** AI suggests tags for documents based on content analysis. Suggestions shown in editor status bar or frontmatter.
**Acceptance Criteria:**
- "AI: Suggest Tags" analyzes document content
- Suggests existing tags (from workspace) and new tags
- Confidence score per suggestion
- One-click to add suggested tag
- Learns from accepted/rejected suggestions
- Suggestions update on save

### Task 9: Wire AI Link Suggestions
**Files:** `src/core/AIService.cpp`, `src/core/LinkSuggestionService.cpp`
**Description:** LinkSuggestionService exists. Wire AI to suggest wiki-links: analyze document content and suggest links to related documents in the workspace.
**Acceptance Criteria:**
- "AI: Suggest Links" scans document for linkable mentions
- Suggests wiki-links to existing documents
- Shows context: why each link is relevant
- One-click to insert wiki-link
- Filters: only suggest links to documents not already linked
- Suggestions ranked by relevance

### Task 10: Wire AI Flashcard Generation
**Files:** `src/core/AIService.cpp`, `src/core/fsrs/FlashcardStore.cpp`
**Description:** AI generates flashcards from document content: key concepts, definitions, Q&A pairs.
**Acceptance Criteria:**
- "AI: Generate Flashcards" from current document
- Generates: basic cards (Q&A), cloze deletions
- Preview generated cards before adding to deck
- Deck selection for generated cards
- Edit individual cards before accepting
- Configurable: card count, difficulty level

### Task 11: Wire AI Translation
**Files:** `src/core/AIService.cpp`, `src/ui/EditorPanel.cpp`
**Description:** Translate selected text or entire document to another language. Preserves Markdown formatting.
**Acceptance Criteria:**
- "AI: Translate" with language selector
- Translate selection or entire document
- Markdown formatting preserved
- Output in new document or replace selection
- Language detection for source text
- Support 20+ languages

### Task 12: Wire AI Tone Adjustment
**Files:** `src/core/AIService.cpp`, `src/ui/EditorPanel.cpp`
**Description:** Adjust writing tone: formal, casual, technical, friendly, academic. Applied to selection or entire document.
**Acceptance Criteria:**
- "AI: Adjust Tone" with tone selector
- Tones: formal, casual, technical, friendly, academic, concise
- Applied to selection or full document
- Preview before applying
- Side-by-side comparison available
- Undo support

### Task 13: Wire AI Chat Context Management
**Files:** `src/ui/AIPanel.cpp`, `src/core/AIService.cpp`
**Description:** AI chat panel manages context: current file, selected text, referenced documents. Users can pin context items.
**Acceptance Criteria:**
- Context panel shows: current file, selection, pinned items
- Pin file to context: always include in AI conversations
- Unpin to remove from context
- Context token count shown (model limit awareness)
- Auto-include: file being discussed
- Context clears on conversation reset

### Task 14: Wire AI Rate Limiting and Cost Tracking
**Files:** `src/core/AIService.cpp`, `src/core/Config.h`
**Description:** Track API usage: token count, request count, estimated cost. Configurable rate limits to prevent accidental overspending.
**Acceptance Criteria:**
- Token usage tracked per session and total
- Estimated cost displayed in AI panel
- Monthly usage limit configurable
- Warning at 80% of limit
- Hard stop at limit (configurable)
- Usage statistics in settings

### Task 15: Wire AI Conversation History
**Files:** `src/core/AIService.cpp`, `src/ui/AIPanel.cpp`
**Description:** Persist AI conversation history per workspace. Browse and search past conversations. Resume previous conversations.
**Acceptance Criteria:**
- Conversations saved in `.markamp/ai-history/`
- Conversation list in AI panel sidebar
- Search conversations by content
- Click to resume previous conversation
- Delete individual conversations
- Configurable history retention (default: 90 days)

### Task 16: Wire AI Custom Prompts
**Files:** `src/core/AIService.cpp`, `src/core/Config.h`
**Description:** Users can define custom AI prompts: name, system prompt, variables. Custom prompts appear in AI actions menu.
**Acceptance Criteria:**
- Custom prompts in `.markamp/ai-prompts/`
- Prompt format: YAML with name, system prompt, variables
- Custom prompts in AI action menu
- Variables: `{{selection}}`, `{{file}}`, `{{clipboard}}`
- Share prompts between workspaces
- Import/export prompts

### Task 17: Wire AI Offline Fallback
**Files:** `src/core/AIService.cpp`
**Description:** When no AI provider is configured or available, gracefully degrade: hide AI actions, show setup instructions, offer local model setup guide.
**Acceptance Criteria:**
- No API key: AI panel shows setup instructions
- Network error: notification with retry option
- Offline: hide real-time features, show cached suggestions
- Ollama detection: offer local model setup
- Status indicator in AI panel: connected/disconnected

### Task 18: Wire AI Command Palette Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register AI commands: "AI: Chat", "AI: Continue Writing", "AI: Improve Writing", "AI: Summarize", "AI: Generate Document", "AI: Suggest Tags", "AI: Suggest Links", "AI: Translate".
**Acceptance Criteria:**
- All commands registered in command palette
- Commands categorized under "AI:" prefix
- Commands disabled when no provider configured
- Selection-dependent commands only when text selected
- Keyboard shortcut for "AI: Chat" (Cmd+Shift+A)

### Task 19: Wire AI Theme Integration
**Files:** `src/ui/AIPanel.cpp`
**Description:** AI panel uses theme tokens: chat bubble colors, code block styling, input area, button styles.
**Acceptance Criteria:**
- User message bubble from `panel_bg` token
- AI message bubble from `editor_bg` token
- Code blocks use theme syntax highlighting
- Input area matches editor styling
- Loading animation uses accent color

### Task 20: Add AI Integration Tests
**Files:** `tests/unit/test_ai_service.cpp`
**Description:** Test AI system: provider configuration, request formatting, response parsing, rate limiting, and conversation management.
**Acceptance Criteria:**
- Provider config validation
- Request formatting for each provider API
- Response parsing and error handling
- Rate limiting enforcement
- Conversation save/load round-trip
- Mock provider for testing without API calls

## Testing Requirements
- Provider configuration and API format
- Request/response handling with error cases
- Rate limiting and cost tracking
- Conversation persistence

## Phase Completion Criteria
- AI chat panel functional with provider selection
- Writing assistance actions in editor
- AI canvas assistance (mind map, diagram generation)
- Tag and link suggestions
- Conversation history persistence
- Rate limiting and cost tracking
- All tests pass
