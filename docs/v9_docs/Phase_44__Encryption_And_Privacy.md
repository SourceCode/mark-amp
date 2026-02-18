# Phase 44: Encryption and Privacy

## Overview
EncryptionService exists in core (test file test_encryption_service.cpp exists). CloudSyncService has encryption infrastructure. However, document-level encryption, vault locking, and privacy controls are not wired to user-facing features. This phase builds encryption and privacy capabilities.

## Prerequisites
- Phase 29 (Security and input validation)
- Phase 20 (File management)
- Phase 27 (Cloud sync for encrypted sync)

## Tasks

### Task 1: Wire EncryptionService for Document Encryption
**Files:** `src/core/EncryptionService.cpp`, `src/core/EncryptionService.h`
**Description:** Wire document-level encryption: individual files can be encrypted at rest. Encrypted files require password to open.
**Acceptance Criteria:**
- Encrypt file: AES-256-GCM with password-derived key
- Decrypt on open: password prompt
- File format: encrypted wrapper with metadata (algorithm, salt, iv)
- Key derivation: Argon2id or PBKDF2 with high iterations
- Encrypted files identified by `.enc.md` extension or header
- `FileEncryptedEvent` / `FileDecryptedEvent` emitted

### Task 2: Wire Vault Lock/Unlock
**Files:** `src/core/EncryptionService.cpp`, `src/core/VaultService.cpp`
**Description:** Vault-level lock: entire workspace can be locked with a master password. Locking encrypts all files in memory, unlocking decrypts.
**Acceptance Criteria:**
- "Lock Vault" command encrypts workspace state
- Locked state: all files closed, editor cleared
- Unlock: master password prompt
- Auto-lock: after configurable idle time (default: 15 minutes)
- Lock indicator in status bar
- Lock on app minimize (configurable)

### Task 3: Wire Password Management
**Files:** `src/core/EncryptionService.cpp`, `src/platform/PlatformAbstraction.h`
**Description:** Password management: master password stored in OS keychain (optional), password change, password recovery hint.
**Acceptance Criteria:**
- Password stored in OS keychain (opt-in)
- Password required on each lock/unlock (if not in keychain)
- "Change Password" re-encrypts all encrypted files
- Password strength indicator
- Recovery hint (user-defined, stored unencrypted)
- No backdoor: forgot password = data inaccessible

### Task 4: Wire Per-File Encryption Toggle
**Files:** `src/ui/FileTreeCtrl.cpp`, `src/core/EncryptionService.cpp`
**Description:** Right-click file: "Encrypt File" / "Decrypt File". Encrypted files show lock icon in explorer.
**Acceptance Criteria:**
- "Encrypt File" prompts for password
- "Decrypt File" prompts for password
- Encrypted files: lock icon in file tree
- Encrypted files: lock icon in tab
- Batch encrypt: select multiple files
- Encrypted directories supported

### Task 5: Wire Encrypted File Editing
**Files:** `src/ui/EditorPanel.cpp`, `src/core/EncryptionService.cpp`
**Description:** Encrypted files: decrypt on open (password prompt), edit normally, re-encrypt on save (same password).
**Acceptance Criteria:**
- Open encrypted file: password prompt
- Decrypted content in editor (normal editing)
- Save: re-encrypt with same password
- Close: clear decrypted content from memory
- Wrong password: clear error message
- Retry: allow multiple password attempts (max 5)

### Task 6: Wire Encrypted Search
**Files:** `src/core/SearchEngine.cpp`, `src/core/EncryptionService.cpp`
**Description:** Search across encrypted files: search index encrypted, search requires vault unlock. Encrypted files searchable only when unlocked.
**Acceptance Criteria:**
- Encrypted file index: encrypted in search index
- Vault locked: encrypted files excluded from search
- Vault unlocked: encrypted files included in search
- Search results indicate encrypted status
- Index re-encrypted on lock

### Task 7: Wire Secure Memory Management
**Files:** `src/core/EncryptionService.cpp`
**Description:** Sensitive data (passwords, decrypted content, encryption keys) securely managed in memory: zeroed on free, not swapped to disk.
**Acceptance Criteria:**
- Password zeroed from memory after use
- Decrypted content zeroed on file close
- Encryption keys zeroed when not needed
- Memory locking: prevent swap (mlock/VirtualLock)
- No sensitive data in crash dumps
- Secure allocator for sensitive buffers

### Task 8: Wire Privacy Settings
**Files:** `src/ui/SettingsPanel.cpp`, `src/core/Config.h`
**Description:** Privacy settings: telemetry opt-out, metadata fetch toggle, AI data usage controls, cloud sync privacy.
**Acceptance Criteria:**
- Telemetry: opt-in/opt-out toggle
- Metadata fetching: enable/disable URL preview
- AI: "Don't send to cloud" option (local models only)
- Cloud sync: encryption toggle
- Search: exclude specific files/folders from index
- Settings section: "Privacy & Security"

### Task 9: Wire Data Retention Controls
**Files:** `src/core/Config.h`, `src/core/StructuredLogger.h`
**Description:** Data retention controls: how long to keep logs, search history, AI conversations, sync history.
**Acceptance Criteria:**
- Log retention: 1, 7, 30, 90 days (configurable)
- Search history: max entries and age
- AI conversation history: retention period
- Sync history: retention period
- "Clear All Data" command
- Auto-cleanup on schedule

### Task 10: Wire Export Encryption
**Files:** `src/core/ExportService.cpp`, `src/core/EncryptionService.cpp`
**Description:** Exported documents can be encrypted: password-protected PDF, encrypted HTML.
**Acceptance Criteria:**
- PDF export with password protection
- HTML export with encryption (JavaScript-based decryption)
- Password prompt during export
- Encryption strength configurable
- Export indicates encrypted status
- "Export: Encrypted PDF" command

### Task 11: Wire Clipboard Security
**Files:** `src/core/ClipboardService.cpp`, `src/core/EncryptionService.cpp`
**Description:** Option to auto-clear clipboard after copying sensitive content. Content from encrypted files auto-cleared after configurable timeout.
**Acceptance Criteria:**
- Auto-clear clipboard after timeout (30 seconds, configurable)
- Clear on vault lock
- Clear on app close
- Notification: "Clipboard will be cleared"
- Toggle: enable/disable clipboard security
- Never copy password to clipboard

### Task 12: Wire Encrypted Backup
**Files:** `src/core/EncryptionService.cpp`, `src/core/CloudSyncService.cpp`
**Description:** Encrypted workspace backup: export entire workspace as encrypted archive. Backup can be restored with password.
**Acceptance Criteria:**
- "Backup: Create Encrypted Backup" command
- Backup includes all workspace files and state
- Backup encrypted with AES-256-GCM
- Backup format: single file with metadata
- Restore: "Backup: Restore from Backup" command
- Backup verification: checksum validation

### Task 13: Wire Two-Factor Authentication Preparation
**Files:** `src/core/EncryptionService.cpp`
**Description:** Infrastructure for future 2FA: TOTP token generation, recovery codes, hardware key preparation (FIDO2/WebAuthn concepts).
**Acceptance Criteria:**
- TOTP generator: QR code display for authenticator apps
- Recovery codes: generate 10 one-time recovery codes
- Recovery code verification
- Infrastructure ready for cloud sync 2FA
- No external service dependency for local 2FA
- Settings UI for 2FA configuration

### Task 14: Wire Encryption Status Indicators
**Files:** `src/ui/StatusBarPanel.cpp`, `src/ui/TabBar.cpp`, `src/ui/FileTreeCtrl.cpp`
**Description:** Visual indicators for encryption status: lock icons in tabs, file tree, and status bar.
**Acceptance Criteria:**
- Tab: lock icon for encrypted files
- File tree: lock icon for encrypted files
- Status bar: vault lock/unlock indicator
- Color: locked (muted), unlocked (normal)
- Hover tooltip: "Encrypted" or "Locked vault"
- Icons from theme icon set

### Task 15: Wire Secure Share
**Files:** `src/core/EncryptionService.cpp`
**Description:** Securely share individual documents: encrypt file, generate share link with embedded decryption key (or separate key sharing).
**Acceptance Criteria:**
- "Share: Create Secure Share" command
- Encrypted file with unique key
- Share link format: `markamp://share/{hash}`
- Key can be shared separately (password)
- Expiration: optional expiry for shared files
- Share tracking: view count, access log

### Task 16: Wire Encryption Commands
**Files:** `src/ui/MainFrame.cpp`
**Description:** Register encryption commands: "Encryption: Encrypt File", "Encryption: Decrypt File", "Encryption: Lock Vault", "Encryption: Unlock Vault", "Encryption: Change Password", "Backup: Create", "Backup: Restore".
**Acceptance Criteria:**
- All commands registered in command palette
- Commands categorized under "Encryption:" and "Backup:" prefixes
- Lock/Unlock vault with keyboard shortcut (Cmd+Shift+L)
- Commands context-aware
- Password input: secure field (masked)

### Task 17: Wire Encryption Key Derivation Performance
**Files:** `src/core/EncryptionService.cpp`
**Description:** Key derivation performance: balance security with usability. Derivation time target: 200-500ms.
**Acceptance Criteria:**
- Argon2id or PBKDF2 with adaptive iterations
- Target derivation time: 200-500ms
- Memory parameter: 64MB for Argon2
- Parallelism: use available cores
- Benchmark on first use to calibrate parameters
- Parameters stored with encrypted data

### Task 18: Wire Encryption Theme Integration
**Files:** `src/ui/EditorPanel.cpp`, `src/ui/FileTreeCtrl.cpp`
**Description:** Encryption UI elements use theme tokens: lock icons, password prompts, encrypted status indicators.
**Acceptance Criteria:**
- Lock icon color from theme
- Password prompt styled with theme
- Encrypted file: subtle visual distinction
- Lock/unlock transition animation
- All 64 themes support encryption indicators

### Task 19: Wire Encryption Audit Log
**Files:** `src/core/EncryptionService.cpp`, `src/core/StructuredLogger.h`
**Description:** Audit logging for encryption operations: encrypt, decrypt, lock, unlock, password change, backup.
**Acceptance Criteria:**
- All encryption operations logged (without sensitive data)
- Log: operation, file (if applicable), timestamp, success/failure
- Failed decryption attempts logged with count
- Audit log in "Security" output channel
- Audit log exportable
- No passwords or keys in logs

### Task 20: Add Encryption Tests
**Files:** `tests/unit/test_encryption_service.cpp`
**Description:** Test encryption system: encrypt/decrypt round-trip, wrong password handling, key derivation, secure memory.
**Acceptance Criteria:**
- Encrypt/decrypt round-trip: data matches
- Wrong password: clear error, no data corruption
- Key derivation: correct key from password + salt
- Large file encryption: 100MB file handles correctly
- Concurrent access: thread-safe encryption
- Memory: sensitive data zeroed after use

## Testing Requirements
- Encrypt/decrypt round-trip for all file sizes
- Wrong password: error handling
- Vault lock/unlock lifecycle
- Key derivation performance benchmark

## Phase Completion Criteria
- Document-level encryption with AES-256-GCM
- Vault lock/unlock with master password
- Encrypted file editing transparent
- Encrypted search when vault unlocked
- Secure memory management
- Encrypted backup/restore
- All tests pass
