#include "FileIndex.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
#include <mutex>

namespace markamp::core
{

void FileIndex::build(const std::vector<std::string>& file_paths)
{
    std::lock_guard<std::mutex> lock(mutex_);
    files_.clear();
    trigram_index_.clear();

    for (const auto& path : file_paths)
    {
        IndexedFile indexed;
        indexed.file_path = path;
        indexed.filename = std::filesystem::path(path).filename().string();
        indexed.lowercase_path = to_lower(path);

        add_trigrams(path, indexed.lowercase_path);
        files_[path] = std::move(indexed);
    }
}

void FileIndex::add(const std::string& file_path)
{
    std::lock_guard<std::mutex> lock(mutex_);

    IndexedFile indexed;
    indexed.file_path = file_path;
    indexed.filename = std::filesystem::path(file_path).filename().string();
    indexed.lowercase_path = to_lower(file_path);

    add_trigrams(file_path, indexed.lowercase_path);
    files_[file_path] = std::move(indexed);
}

void FileIndex::remove(const std::string& file_path)
{
    std::lock_guard<std::mutex> lock(mutex_);

    auto file_iter = files_.find(file_path);
    if (file_iter != files_.end())
    {
        remove_trigrams(file_path, file_iter->second.lowercase_path);
        files_.erase(file_iter);
    }
}

auto FileIndex::search(const std::string& query, std::size_t limit) const
    -> std::vector<std::string>
{
    std::lock_guard<std::mutex> lock(mutex_);

    if (query.empty())
    {
        // Return all files up to limit
        std::vector<std::string> result;
        result.reserve(std::min(files_.size(), limit));
        for (const auto& [path, _] : files_)
        {
            result.push_back(path);
            if (result.size() >= limit)
            {
                break;
            }
        }
        return result;
    }

    auto lower_query = to_lower(query);
    auto trigrams = extract_trigrams(lower_query);

    if (trigrams.empty())
    {
        // Query too short for trigrams, fall back to substring match
        std::vector<std::string> result;
        for (const auto& [path, indexed] : files_)
        {
            if (indexed.lowercase_path.find(lower_query) != std::string::npos)
            {
                result.push_back(path);
                if (result.size() >= limit)
                {
                    break;
                }
            }
        }
        return result;
    }

    // Intersect trigram sets to get candidates
    std::unordered_set<std::string> candidates;
    bool first = true;

    for (const auto& tri : trigrams)
    {
        auto tri_iter = trigram_index_.find(tri);
        if (tri_iter == trigram_index_.end())
        {
            // No files contain this trigram
            return {};
        }

        if (first)
        {
            candidates = tri_iter->second;
            first = false;
        }
        else
        {
            std::unordered_set<std::string> intersection;
            for (const auto& path : candidates)
            {
                if (tri_iter->second.count(path) > 0)
                {
                    intersection.insert(path);
                }
            }
            candidates = std::move(intersection);
        }

        if (candidates.empty())
        {
            return {};
        }
    }

    // Convert to vector, apply limit
    std::vector<std::string> result;
    result.reserve(std::min(candidates.size(), limit));
    for (const auto& path : candidates)
    {
        result.push_back(path);
        if (result.size() >= limit)
        {
            break;
        }
    }
    return result;
}

auto FileIndex::all_files() const -> std::vector<std::string>
{
    std::lock_guard<std::mutex> lock(mutex_);
    std::vector<std::string> result;
    result.reserve(files_.size());
    for (const auto& [path, _] : files_)
    {
        result.push_back(path);
    }
    return result;
}

auto FileIndex::size() const -> std::size_t
{
    std::lock_guard<std::mutex> lock(mutex_);
    return files_.size();
}

void FileIndex::clear()
{
    std::lock_guard<std::mutex> lock(mutex_);
    files_.clear();
    trigram_index_.clear();
}

auto FileIndex::extract_trigrams(const std::string& text) -> std::vector<std::string>
{
    std::vector<std::string> trigrams;
    if (text.size() < 3)
    {
        return trigrams;
    }
    trigrams.reserve(text.size() - 2);
    for (std::size_t idx = 0; idx <= text.size() - 3; ++idx)
    {
        trigrams.push_back(text.substr(idx, 3));
    }
    return trigrams;
}

void FileIndex::add_trigrams(const std::string& file_path, const std::string& lowercase_path)
{
    auto trigrams = extract_trigrams(lowercase_path);
    for (const auto& tri : trigrams)
    {
        trigram_index_[tri].insert(file_path);
    }
}

void FileIndex::remove_trigrams(const std::string& file_path, const std::string& lowercase_path)
{
    auto trigrams = extract_trigrams(lowercase_path);
    for (const auto& tri : trigrams)
    {
        auto tri_iter = trigram_index_.find(tri);
        if (tri_iter != trigram_index_.end())
        {
            tri_iter->second.erase(file_path);
            if (tri_iter->second.empty())
            {
                trigram_index_.erase(tri_iter);
            }
        }
    }
}

auto FileIndex::to_lower(const std::string& text) -> std::string
{
    std::string result = text;
    std::transform(result.begin(),
                   result.end(),
                   result.begin(),
                   [](unsigned char chr) { return std::tolower(chr); });
    return result;
}

} // namespace markamp::core
