// ============================================================================
// File: src/core/CloudSyncService.cpp
// Phase 38: Cloud Sync — CloudSyncService implementation
// ============================================================================

#include "CloudSyncService.h"

#include "Events.h"
#include "S3Client.h"
#include "SyncEngine.h"
#include "WebDavClient.h"

#include <chrono>
#include <fstream>
#include <random>

namespace markamp::core
{

CloudSyncService::CloudSyncService(EventBus& event_bus, Config& config, SyncEngine& sync_engine)
    : event_bus_(event_bus)
    , config_(config)
    , sync_engine_(sync_engine)
{
}

auto CloudSyncService::sync(const CloudSyncConfig& cloud_config, CloudProgressCallback progress)
    -> CloudSyncResult
{
    std::lock_guard lock(mutex_);
    auto start = std::chrono::steady_clock::now();
    CloudSyncResult result;
    result.status = CloudSyncStatus::Connecting;

    // Test connection first.
    auto conn = test_connection(cloud_config);
    if (!conn)
    {
        result.status = CloudSyncStatus::Failed;
        result.errors.push_back("Connection failed: " + conn.error());
        last_result_ = result;
        return result;
    }

    // Generate local snapshot.
    auto local_snap = sync_engine_.generate_snapshot(cloud_config.local_data_dir);
    if (!local_snap)
    {
        result.status = CloudSyncStatus::Failed;
        result.errors.push_back("Local scan failed: " + local_snap.error());
        last_result_ = result;
        return result;
    }

    // Upload local changes.
    result.status = CloudSyncStatus::Uploading;
    if (progress)
        progress("", 25, CloudSyncStatus::Uploading);

    auto upload_result = upload_snapshot(*local_snap, cloud_config, progress);
    if (upload_result)
    {
        result.files_uploaded = *upload_result;
    }
    else
    {
        result.errors.push_back("Upload failed: " + upload_result.error());
    }

    // Download remote changes.
    result.status = CloudSyncStatus::Downloading;
    if (progress)
        progress("", 75, CloudSyncStatus::Downloading);

    auto download_result = download_snapshot(cloud_config, cloud_config.local_data_dir, progress);
    if (download_result)
    {
        result.files_downloaded = *download_result;
    }
    else
    {
        result.warnings.push_back("Download issues: " + download_result.error());
    }

    auto end = std::chrono::steady_clock::now();
    result.elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    result.status = result.errors.empty() ? CloudSyncStatus::Completed : CloudSyncStatus::Failed;

    if (progress)
        progress("", 100, result.status);
    last_result_ = result;
    return result;
}

auto CloudSyncService::push(const CloudSyncConfig& cloud_config, CloudProgressCallback progress)
    -> CloudSyncResult
{
    std::lock_guard lock(mutex_);
    auto start = std::chrono::steady_clock::now();
    CloudSyncResult result;

    auto local_snap = sync_engine_.generate_snapshot(cloud_config.local_data_dir);
    if (!local_snap)
    {
        result.status = CloudSyncStatus::Failed;
        result.errors.push_back(local_snap.error());
        return result;
    }

    result.status = CloudSyncStatus::Uploading;
    auto upload = upload_snapshot(*local_snap, cloud_config, progress);
    if (upload)
    {
        result.files_uploaded = *upload;
        result.status = CloudSyncStatus::Completed;
    }
    else
    {
        result.status = CloudSyncStatus::Failed;
        result.errors.push_back(upload.error());
    }

    auto end = std::chrono::steady_clock::now();
    result.elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    last_result_ = result;
    return result;
}

auto CloudSyncService::pull(const CloudSyncConfig& cloud_config, CloudProgressCallback progress)
    -> CloudSyncResult
{
    std::lock_guard lock(mutex_);
    auto start = std::chrono::steady_clock::now();
    CloudSyncResult result;

    result.status = CloudSyncStatus::Downloading;
    auto download = download_snapshot(cloud_config, cloud_config.local_data_dir, progress);
    if (download)
    {
        result.files_downloaded = *download;
        result.status = CloudSyncStatus::Completed;
    }
    else
    {
        result.status = CloudSyncStatus::Failed;
        result.errors.push_back(download.error());
    }

    auto end = std::chrono::steady_clock::now();
    result.elapsed_ms = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    last_result_ = result;
    return result;
}

auto CloudSyncService::test_connection(const CloudSyncConfig& cloud_config)
    -> std::expected<void, std::string>
{
    if (cloud_config.provider == CloudProvider::S3)
    {
        auto client = create_s3_client(cloud_config.s3);
        return client->test_connection();
    }
    else
    {
        auto client = create_webdav_client(cloud_config.webdav);
        return client->test_connection();
    }
}

// ── Encryption ──────────────────────────────────────────────────────────────

auto CloudSyncService::encrypt(const std::vector<uint8_t>& plaintext,
                               const EncryptionConfig& enc_config) const
    -> std::expected<std::vector<uint8_t>, std::string>
{
    if (!enc_config.enabled)
        return plaintext;

    auto key = derive_key(enc_config.passphrase, enc_config);
    if (!key)
        return std::unexpected(key.error());

    // Stub: AES-256-GCM encryption.
    // In production, use OpenSSL EVP_aes_256_gcm:
    //   1. Generate random 12-byte nonce
    //   2. EVP_EncryptInit_ex with key and nonce
    //   3. EVP_EncryptUpdate for plaintext
    //   4. EVP_EncryptFinal_ex
    //   5. EVP_CTRL_GCM_GET_TAG for auth tag
    //   6. Output: nonce || ciphertext || tag

    // Placeholder: XOR with key bytes (NOT secure, for compilation only).
    std::vector<uint8_t> result;
    result.reserve(plaintext.size() + 12 + 16); // nonce + data + tag

    // Generate random nonce.
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<int> dist(0, 255);
    for (int i = 0; i < enc_config.nonce_size; ++i)
    {
        result.push_back(static_cast<uint8_t>(dist(gen)));
    }

    // XOR placeholder (NOT real encryption).
    for (size_t i = 0; i < plaintext.size(); ++i)
    {
        result.push_back(plaintext[i] ^ (*key)[i % key->size()]);
    }

    // Placeholder tag.
    for (int i = 0; i < enc_config.tag_size; ++i)
    {
        result.push_back(0);
    }

    return result;
}

auto CloudSyncService::decrypt(const std::vector<uint8_t>& ciphertext,
                               const EncryptionConfig& enc_config) const
    -> std::expected<std::vector<uint8_t>, std::string>
{
    if (!enc_config.enabled)
        return ciphertext;

    auto key = derive_key(enc_config.passphrase, enc_config);
    if (!key)
        return std::unexpected(key.error());

    auto overhead = static_cast<size_t>(enc_config.nonce_size + enc_config.tag_size);
    if (ciphertext.size() < overhead)
    {
        return std::unexpected("Ciphertext too short");
    }

    // Stub: reverse the XOR placeholder.
    auto data_size = ciphertext.size() - overhead;
    std::vector<uint8_t> plaintext(data_size);

    auto nonce_end = static_cast<size_t>(enc_config.nonce_size);
    for (size_t i = 0; i < data_size; ++i)
    {
        plaintext[i] = ciphertext[nonce_end + i] ^ (*key)[i % key->size()];
    }

    return plaintext;
}

auto CloudSyncService::derive_key(const std::string& passphrase,
                                  const EncryptionConfig& enc_config) const
    -> std::expected<std::vector<uint8_t>, std::string>
{
    if (passphrase.empty())
    {
        return std::unexpected("Passphrase is required for encryption");
    }

    // Stub: Argon2id KDF.
    // In production, use libsodium crypto_pwhash or argon2 library:
    //   crypto_pwhash(key, 32, passphrase, passphrase.size(),
    //                 salt, enc_config.argon2_iterations,
    //                 enc_config.argon2_memory_kb * 1024,
    //                 crypto_pwhash_ALG_ARGON2ID13);

    // Placeholder: simple PBKDF-like key derivation (NOT secure).
    std::vector<uint8_t> key(32);
    uint64_t hash = 14695981039346656037ULL;
    for (char c : passphrase)
    {
        hash ^= static_cast<uint8_t>(c);
        hash *= 1099511628211ULL;
    }
    for (uint32_t iter = 0; iter < enc_config.argon2_iterations; ++iter)
    {
        hash ^= (iter + 1);
        hash *= 1099511628211ULL;
    }
    for (size_t i = 0; i < 32; ++i)
    {
        key[i] = static_cast<uint8_t>((hash >> ((i % 8) * 8)) & 0xFF);
        hash *= 1099511628211ULL;
    }

    return key;
}

auto CloudSyncService::last_result() const -> const CloudSyncResult&
{
    return last_result_;
}

// ── Private helpers ─────────────────────────────────────────────────────────

auto CloudSyncService::create_s3_client(const S3Config& s3_config) const
    -> std::unique_ptr<IS3Client>
{
    return std::make_unique<S3Client>(s3_config);
}

auto CloudSyncService::create_webdav_client(const WebDavConfig& webdav_config) const
    -> std::unique_ptr<IWebDavClient>
{
    return std::make_unique<WebDavClient>(webdav_config);
}

auto CloudSyncService::upload_snapshot(const SyncSnapshot& snapshot,
                                       const CloudSyncConfig& cloud_config,
                                       CloudProgressCallback progress)
    -> std::expected<int32_t, std::string>
{
    int32_t uploaded = 0;
    int total = snapshot.file_count();
    int current = 0;

    for (const auto& [path, entry] : snapshot.files)
    {
        if (entry.is_directory)
            continue;
        ++current;

        auto local_path = snapshot.root_path / entry.relative_path;
        if (!std::filesystem::exists(local_path))
            continue;

        if (progress)
        {
            progress(entry.relative_path.string(),
                     total > 0 ? (current * 100 / total) : 100,
                     CloudSyncStatus::Uploading);
        }

        // Encrypt if configured.
        auto upload_path = local_path;
        std::filesystem::path temp_encrypted;
        if (cloud_config.encryption.enabled)
        {
            temp_encrypted = std::filesystem::temp_directory_path() /
                             ("enc_" + entry.relative_path.filename().string());
            auto enc_result = encrypt_file(local_path, temp_encrypted, cloud_config.encryption);
            if (!enc_result)
                continue;
            upload_path = temp_encrypted;
        }

        // Upload via the appropriate client.
        if (cloud_config.provider == CloudProvider::S3)
        {
            auto client = create_s3_client(cloud_config.s3);
            auto result = client->put_object(path, upload_path);
            if (result)
                ++uploaded;
        }
        else
        {
            auto client = create_webdav_client(cloud_config.webdav);
            auto result = client->upload(cloud_config.webdav.remote_path + path, upload_path);
            if (result)
                ++uploaded;
        }

        // Clean up temp encrypted file.
        if (!temp_encrypted.empty())
        {
            std::filesystem::remove(temp_encrypted);
        }
    }

    return uploaded;
}

auto CloudSyncService::download_snapshot(const CloudSyncConfig& cloud_config,
                                         const std::filesystem::path& target_dir,
                                         CloudProgressCallback progress)
    -> std::expected<int32_t, std::string>
{
    // Stub: list remote objects, download each to target_dir.
    (void)target_dir;
    if (progress)
        progress("", 100, CloudSyncStatus::Downloading);
    return 0;
}

auto CloudSyncService::encrypt_file(const std::filesystem::path& source,
                                    const std::filesystem::path& target,
                                    const EncryptionConfig& enc_config) const
    -> std::expected<void, std::string>
{
    std::ifstream in(source, std::ios::binary);
    if (!in)
        return std::unexpected("Cannot open source file");

    std::vector<uint8_t> plaintext((std::istreambuf_iterator<char>(in)),
                                   std::istreambuf_iterator<char>());

    auto encrypted = encrypt(plaintext, enc_config);
    if (!encrypted)
        return std::unexpected(encrypted.error());

    std::ofstream out(target, std::ios::binary);
    if (!out)
        return std::unexpected("Cannot create encrypted file");
    out.write(reinterpret_cast<const char*>(encrypted->data()),
              static_cast<std::streamsize>(encrypted->size()));

    return {};
}

auto CloudSyncService::decrypt_file(const std::filesystem::path& source,
                                    const std::filesystem::path& target,
                                    const EncryptionConfig& enc_config) const
    -> std::expected<void, std::string>
{
    std::ifstream in(source, std::ios::binary);
    if (!in)
        return std::unexpected("Cannot open encrypted file");

    std::vector<uint8_t> ciphertext((std::istreambuf_iterator<char>(in)),
                                    std::istreambuf_iterator<char>());

    auto decrypted = decrypt(ciphertext, enc_config);
    if (!decrypted)
        return std::unexpected(decrypted.error());

    std::ofstream out(target, std::ios::binary);
    if (!out)
        return std::unexpected("Cannot create decrypted file");
    out.write(reinterpret_cast<const char*>(decrypted->data()),
              static_cast<std::streamsize>(decrypted->size()));

    return {};
}

} // namespace markamp::core
