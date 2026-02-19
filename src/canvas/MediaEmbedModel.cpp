#include "MediaEmbedModel.h"

#include <algorithm>

namespace markamp::canvas
{

void MediaEmbedModel::set_source(const std::string& url)
{
    source_ = url;
}
auto MediaEmbedModel::source() const -> const std::string&
{
    return source_;
}

void MediaEmbedModel::set_media_type(MediaType type)
{
    media_type_ = type;
}
auto MediaEmbedModel::media_type() const -> MediaType
{
    return media_type_;
}

void MediaEmbedModel::set_poster(const std::string& url)
{
    poster_ = url;
}
auto MediaEmbedModel::poster() const -> const std::string&
{
    return poster_;
}

void MediaEmbedModel::set_duration_seconds(double seconds)
{
    duration_seconds_ = std::max(0.0, seconds);
}
auto MediaEmbedModel::duration_seconds() const -> double
{
    return duration_seconds_;
}

void MediaEmbedModel::play()
{
    playback_state_ = PlaybackState::kPlaying;
}
void MediaEmbedModel::pause()
{
    playback_state_ = PlaybackState::kPaused;
}
void MediaEmbedModel::stop()
{
    playback_state_ = PlaybackState::kStopped;
}
auto MediaEmbedModel::playback_state() const -> PlaybackState
{
    return playback_state_;
}

void MediaEmbedModel::set_muted(bool muted)
{
    muted_ = muted;
}
auto MediaEmbedModel::is_muted() const -> bool
{
    return muted_;
}

void MediaEmbedModel::set_aspect_locked(bool locked)
{
    aspect_locked_ = locked;
}
auto MediaEmbedModel::aspect_locked() const -> bool
{
    return aspect_locked_;
}

void MediaEmbedModel::set_load_state(MediaLoadState state)
{
    load_state_ = state;
}
auto MediaEmbedModel::load_state() const -> MediaLoadState
{
    return load_state_;
}

void MediaEmbedModel::set_error_message(const std::string& message)
{
    error_message_ = message;
}
auto MediaEmbedModel::error_message() const -> const std::string&
{
    return error_message_;
}

} // namespace markamp::canvas
