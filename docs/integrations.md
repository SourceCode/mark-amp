# Integrations

> Third-party systems, external dependencies, and integration points for MarkAmp v2.3.16.

---

## Build Dependencies

These are core library dependencies managed through vcpkg:

| Integration      | Library          | Purpose                           | Configuration                   |
| ---------------- | ---------------- | --------------------------------- | ------------------------------- |
| UI Toolkit       | wxWidgets 3.2.9  | Cross-platform native UI          | Linked via CMake `find_package` |
| Markdown Parsing | md4c             | CommonMark + GFM parser           | Compiled as static library      |
| JSON             | nlohmann-json    | JSON parsing (configs, manifests) | Header-only                     |
| YAML             | yaml-cpp         | Configuration file parsing        | Static library                  |
| Logging          | spdlog + fmt     | Structured logging                | Static library                  |
| Testing          | Catch2           | Unit test framework               | Header-only + main              |
| HTTP             | cpp-httplib      | HTTP client for gallery/sync      | Header-only                     |
| Crypto           | OpenSSL          | AES-256 encryption, hashing       | System library                  |
| Archive          | libzip           | VSIX package handling             | Static library                  |
| SVG              | nanosvg          | SVG icon rendering                | Header-only                     |
| Database         | SQLite3          | FTS5 full-text search, AV data    | Static library                  |
| Profiling        | Tracy            | Frame-level profiling             | Conditional compilation         |
| Benchmarking     | Google Benchmark | Performance benchmarks            | Test-only                       |
| Allocator        | mimalloc         | High-performance allocator        | Optional replacement            |

---

## Runtime Integrations

### Extension Marketplace (GalleryService / HttpClient)

**Purpose:** Search and download extensions from a VS Code-compatible marketplace.

- **Endpoint:** Configurable marketplace URL
- **Protocol:** HTTP/HTTPS REST API
- **Auth:** None required for search; optional for publishing
- **Local Testing:** Mock responses via `GalleryService` test mode
- **Error Handling:** Timeouts, retries, and fallback to cached results

### Cloud Sync Providers

#### S3 (S3Client)

- **Purpose:** Document sync to AWS S3 or S3-compatible storage
- **Configuration:** Bucket name, region, credentials
- **Error Handling:** Retry with exponential backoff
- **Local Testing:** Use MinIO for local S3-compatible storage

#### WebDAV (WebDavClient)

- **Purpose:** Document sync to WebDAV servers (Nextcloud, etc.)
- **Configuration:** Server URL, credentials
- **Error Handling:** Connection retry, conflict resolution
- **Local Testing:** Use local WebDAV server

### AI Providers (AIService)

| Provider     | Endpoint                             | Model Config                        |
| ------------ | ------------------------------------ | ----------------------------------- |
| OpenAI       | `api.openai.com/v1/chat/completions` | Model name, temperature, max tokens |
| Anthropic    | `api.anthropic.com/v1/messages`      | Model name, temperature, max tokens |
| Local/Ollama | `localhost:11434/api/generate`       | Model name                          |

- **Configuration:** API key in `Config` (encrypted via `EncryptionService`)
- **Error Handling:** Rate limiting, timeout, fallback
- **Local Testing:** Use Ollama with local models

### Pandoc (PandocExporter / PandocImporter)

- **Purpose:** Document format conversion (PDF, DOCX, etc.)
- **Requirement:** `pandoc` binary in system PATH
- **Invocation:** Subprocess via `std::system()` or `popen()`
- **Local Testing:** Install Pandoc: `brew install pandoc`

### Mermaid (MermaidBlockRenderer)

- **Purpose:** Render Mermaid diagram markup to SVG
- **Requirement:** `mmdc` (Mermaid CLI) in system PATH
- **Output:** SVG with XSS sanitization applied
- **Local Testing:** `npm install -g @mermaid.js/mermaid-cli`

### Git (GitService)

- **Purpose:** Version control operations
- **Requirement:** `git` binary in system PATH
- **Operations:** Commit, branch, stash, blame, remote, diff
- **Local Testing:** Standard git repository

---

## Platform-Specific Integrations

### macOS (MacPlatform)

- Native file dialogs via `NSOpenPanel` / `NSSavePanel`
- Appearance detection (dark mode) via `NSAppearance`
- Objective-C++ bridge in `MacPlatform.mm`

### Windows (WinPlatform)

- Win32 API for file associations
- Registry access for configuration
- Shell integration for context menus

### Linux (LinuxPlatform)

- XDG compliance for config/data paths
- GTK/GDK integration
- Desktop file registration

---

## Webhook/Event Contracts

MarkAmp does not expose HTTP endpoints. All integrations are outbound.

**Extension Events:**

| Event                             | Direction | Trigger                      |
| --------------------------------- | --------- | ---------------------------- |
| `ExtensionInstalledEvent`         | Internal  | After VSIX install completes |
| `ExtensionUninstalledEvent`       | Internal  | After extension removal      |
| `ExtensionEnablementChangedEvent` | Internal  | Enable/disable toggled       |

**Sync Events:**

| Event          | Direction | Trigger                         |
| -------------- | --------- | ------------------------------- |
| Sync started   | Internal  | Scheduled or manual sync begins |
| Sync completed | Internal  | All files synchronized          |
| Sync conflict  | Internal  | Conflicting changes detected    |

---

## Idempotency & Retry

| Integration       | Retry Policy                   | Idempotent             |
| ----------------- | ------------------------------ | ---------------------- |
| Gallery search    | 3 retries, exponential backoff | Yes                    |
| S3 upload         | 3 retries, exponential backoff | Yes (overwrite)        |
| WebDAV sync       | 3 retries, linear backoff      | Yes                    |
| AI API calls      | 2 retries, fixed delay         | No (non-deterministic) |
| Pandoc conversion | No retry                       | Yes                    |
