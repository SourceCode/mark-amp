#pragma once

#include <cstdint>
#include <string>

namespace markamp::canvas
{

/// Media type.
enum class MediaType : uint8_t
{
    kVideo,
    kAudio,
};

/// Playback state.
enum class PlaybackState : uint8_t
{
    kStopped,
    kPlaying,
    kPaused,
};

/// Media load state.
enum class MediaLoadState : uint8_t
{
    kLoading,
    kReady,
    kError,
    kPlaceholder,
};

/// Testable model for Video/Audio Embed (Phase 54).
///
/// Encapsulates:
/// - Source metadata (URL/path, poster, duration)
/// - Playback controls (play/pause/mute)
/// - Aspect ratio locking
/// - Load state (loading/ready/error/placeholder)
class MediaEmbedModel
{
public:
    // ── Metadata ────────────────────────────────────────────────────

    void set_source(const std::string& url);
    [[nodiscard]] auto source() const -> const std::string&;

    void set_media_type(MediaType type);
    [[nodiscard]] auto media_type() const -> MediaType;

    void set_poster(const std::string& url);
    [[nodiscard]] auto poster() const -> const std::string&;

    void set_duration_seconds(double seconds);
    [[nodiscard]] auto duration_seconds() const -> double;

    // ── Playback ────────────────────────────────────────────────────

    void play();
    void pause();
    void stop();
    [[nodiscard]] auto playback_state() const -> PlaybackState;

    void set_muted(bool muted);
    [[nodiscard]] auto is_muted() const -> bool;

    // ── Aspect ratio ────────────────────────────────────────────────

    void set_aspect_locked(bool locked);
    [[nodiscard]] auto aspect_locked() const -> bool;

    // ── Load state ──────────────────────────────────────────────────

    void set_load_state(MediaLoadState state);
    [[nodiscard]] auto load_state() const -> MediaLoadState;
    void set_error_message(const std::string& message);
    [[nodiscard]] auto error_message() const -> const std::string&;

private:
    std::string source_;
    MediaType media_type_{MediaType::kVideo};
    std::string poster_;
    double duration_seconds_{0.0};
    PlaybackState playback_state_{PlaybackState::kStopped};
    bool muted_{false};
    bool aspect_locked_{true};
    MediaLoadState load_state_{MediaLoadState::kPlaceholder};
    std::string error_message_;
};

} // namespace markamp::canvas
