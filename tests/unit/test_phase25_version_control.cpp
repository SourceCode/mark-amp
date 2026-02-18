/// @file test_phase25_version_control.cpp
/// @brief Phase 25: Version Control Integration — 60 tests / 160+ assertions.
/// Tests all 6 new components: GitStashService, GitBlameEngine,
/// GitRemoteService, MergeConflictResolver, CommitGraphEngine, VCCommandProvider.

#include "core/CommitGraphEngine.h"
#include "core/Config.h"
#include "core/EventBus.h"
#include "core/Events.h"
#include "core/GitBlameEngine.h"
#include "core/GitRemoteService.h"
#include "core/GitService.h"
#include "core/GitStashService.h"
#include "core/MergeConflictResolver.h"
#include "core/VCCommandProvider.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::core;
using namespace markamp::core::events;

// ============================================================================
// GitStashService tests (10 tests)
// ============================================================================

TEST_CASE("GitStashService initial state", "[phase25][stash]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    const GitStashService stash(bus, git);

    REQUIRE(stash.size() == 0);
    REQUIRE(stash.list().empty());
}

TEST_CASE("GitStashService save stash", "[phase25][stash]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("file1.md", "content1");
    git.test_modify_file("file1.md", "modified");

    GitStashService stash(bus, git);
    const auto result = stash.save("WIP: feature");

    REQUIRE(result.has_value());
    REQUIRE(stash.size() == 1);
    const auto entries = stash.list();
    REQUIRE(entries.size() == 1);
    REQUIRE(entries[0].message == "WIP: feature");
    REQUIRE_FALSE(entries[0].id.empty());
}

TEST_CASE("GitStashService pop stash", "[phase25][stash]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("file1.md", "content1");
    git.test_modify_file("file1.md", "modified");

    GitStashService stash(bus, git);
    stash.save("WIP");
    REQUIRE(stash.size() == 1);

    const auto result = stash.pop();
    REQUIRE(result.has_value());
    REQUIRE(stash.size() == 0);
}

TEST_CASE("GitStashService apply stash", "[phase25][stash]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("file1.md", "content1");
    git.test_modify_file("file1.md", "modified");

    GitStashService stash(bus, git);
    stash.save("WIP");

    // apply() with empty string defaults to top entry.
    const auto result = stash.apply();
    REQUIRE(result.has_value());
    REQUIRE(stash.size() == 1); // apply doesn't remove from stack
}

TEST_CASE("GitStashService drop stash", "[phase25][stash]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("file1.md", "content1");
    git.test_modify_file("file1.md", "modified");

    GitStashService stash(bus, git);
    stash.save("WIP");

    // drop() with empty string defaults to top entry.
    const auto result = stash.drop();
    REQUIRE(result.has_value());
    REQUIRE(stash.size() == 0);
}

TEST_CASE("GitStashService show stash", "[phase25][stash]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("file1.md", "content1");
    git.test_modify_file("file1.md", "modified");

    GitStashService stash(bus, git);
    const auto save_result = stash.save("WIP");
    REQUIRE(save_result.has_value());

    // show() returns list of file paths in the stash.
    const auto files = stash.show(save_result.value());
    REQUIRE(files.has_value());
    REQUIRE_FALSE(files->empty());
}

TEST_CASE("GitStashService multiple stashes LIFO", "[phase25][stash]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("file1.md", "content1");
    git.test_modify_file("file1.md", "mod1");

    GitStashService stash(bus, git);
    stash.save("first");
    git.test_modify_file("file1.md", "mod2");
    stash.save("second");

    REQUIRE(stash.size() == 2);

    const auto entries = stash.list();
    REQUIRE(entries.size() == 2);
    // Newest first in list (but stack stores newest at back).
    REQUIRE(entries[0].message == "second");
}

TEST_CASE("GitStashService pop empty fails", "[phase25][stash]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    GitStashService stash(bus, git);

    const auto result = stash.pop();
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("GitStashService clear", "[phase25][stash]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("a.md", "a");
    git.test_modify_file("a.md", "b");

    GitStashService stash(bus, git);
    stash.save("one");
    git.test_modify_file("a.md", "c");
    stash.save("two");

    stash.clear();
    REQUIRE(stash.size() == 0);
}

TEST_CASE("GitStashService publishes events", "[phase25][stash]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");

    int saved_count = 0;
    int applied_count = 0;
    auto saved_sub = bus.subscribe<GitStashSavedEvent>([&](const GitStashSavedEvent& /*event*/)
                                                       { ++saved_count; });
    auto applied_sub = bus.subscribe<GitStashAppliedEvent>(
        [&](const GitStashAppliedEvent& /*event*/) { ++applied_count; });

    git.test_add_file("f.md", "x");
    git.test_modify_file("f.md", "y");

    GitStashService stash(bus, git);
    stash.save("test");
    REQUIRE(saved_count == 1);

    stash.apply();
    REQUIRE(applied_count == 1);
}

// ============================================================================
// GitBlameEngine tests (10 tests)
// ============================================================================

TEST_CASE("GitBlameEngine blame_file basic", "[phase25][blame]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("doc.md", "line1\nline2\nline3");
    git.stage_file("doc.md");
    git.commit("Initial commit");

    GitBlameEngine blame(git);
    const auto result = blame.blame_file("doc.md");

    REQUIRE_FALSE(result.lines.empty());
    REQUIRE(result.file_path == "doc.md");
}

TEST_CASE("GitBlameEngine blame lines have commit hashes", "[phase25][blame]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("doc.md", "content");
    git.stage_file("doc.md");
    git.commit("Add doc");

    GitBlameEngine blame(git);
    const auto result = blame.blame_file("doc.md");

    for (const auto& blame_line : result.lines)
    {
        REQUIRE_FALSE(blame_line.commit_hash.empty());
        REQUIRE_FALSE(blame_line.author.empty());
        REQUIRE(blame_line.line_number > 0);
    }
}

TEST_CASE("GitBlameEngine blame_range", "[phase25][blame]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("doc.md", "a\nb\nc\nd\ne");
    git.stage_file("doc.md");
    git.commit("Add doc");

    GitBlameEngine blame(git);
    const auto result = blame.blame_range("doc.md", 2, 4);

    REQUIRE_FALSE(result.lines.empty());
    REQUIRE(result.lines.size() <= 3);
}

TEST_CASE("GitBlameEngine top contributors", "[phase25][blame]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("doc.md", "content");
    git.stage_file("doc.md");
    git.commit("Work");

    GitBlameEngine blame(git);
    const auto contributors = blame.top_contributors("doc.md", 5);

    REQUIRE_FALSE(contributors.empty());
    REQUIRE_FALSE(contributors[0].author.empty());
    REQUIRE(contributors[0].line_count > 0);
}

TEST_CASE("GitBlameEngine line age", "[phase25][blame]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("doc.md", "content");
    git.stage_file("doc.md");
    git.commit("Add");

    GitBlameEngine blame(git);
    const auto ages = blame.line_age("doc.md");

    // Each pair is (line_number, age_days).
    if (!ages.empty())
    {
        REQUIRE(ages[0].first > 0);
        REQUIRE(ages[0].second >= 0.0);
    }
}

TEST_CASE("GitBlameEngine empty file", "[phase25][blame]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");

    GitBlameEngine blame(git);
    const auto result = blame.blame_file("nonexistent.md");

    REQUIRE(result.lines.empty());
}

TEST_CASE("GitBlameEngine blame result total lines", "[phase25][blame]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("doc.md", "a\nb\nc");
    git.stage_file("doc.md");
    git.commit("Add");

    GitBlameEngine blame(git);
    const auto result = blame.blame_file("doc.md");

    REQUIRE(result.total_lines == static_cast<int>(result.lines.size()));
}

TEST_CASE("GitBlameEngine multiple commits", "[phase25][blame]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("doc.md", "first");
    git.stage_file("doc.md");
    git.commit("First commit");
    git.test_modify_file("doc.md", "second");
    git.stage_file("doc.md");
    git.commit("Second commit");

    GitBlameEngine blame(git);
    const auto result = blame.blame_file("doc.md");

    REQUIRE_FALSE(result.lines.empty());
}

TEST_CASE("GitBlameEngine blame range clamping", "[phase25][blame]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("doc.md", "a\nb");
    git.stage_file("doc.md");
    git.commit("Add");

    GitBlameEngine blame(git);
    const auto result = blame.blame_range("doc.md", 1, 100);

    REQUIRE_FALSE(result.lines.empty());
}

TEST_CASE("GitBlameEngine contributors ordered by lines", "[phase25][blame]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("doc.md", "a\nb\nc\nd\ne");
    git.stage_file("doc.md");
    git.commit("Add");

    GitBlameEngine blame(git);
    const auto contributors = blame.top_contributors("doc.md", 10);

    for (size_t idx = 1; idx < contributors.size(); ++idx)
    {
        REQUIRE(contributors[idx - 1].line_count >= contributors[idx].line_count);
    }
}

// ============================================================================
// GitRemoteService tests (10 tests)
// ============================================================================

TEST_CASE("GitRemoteService initial state", "[phase25][remote]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    const GitRemoteService remote(bus, git);

    REQUIRE(remote.remote_count() == 0);
    REQUIRE(remote.list_remotes().empty());
}

TEST_CASE("GitRemoteService add remote", "[phase25][remote]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    GitRemoteService remote(bus, git);

    const auto result = remote.add_remote("origin", "https://github.com/test/repo.git");
    REQUIRE(result.has_value());
    REQUIRE(remote.remote_count() == 1);
}

TEST_CASE("GitRemoteService add duplicate fails", "[phase25][remote]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    GitRemoteService remote(bus, git);

    remote.add_remote("origin", "https://github.com/test/repo.git");
    const auto result = remote.add_remote("origin", "https://other.git");
    REQUIRE_FALSE(result.has_value());
}

TEST_CASE("GitRemoteService remove remote", "[phase25][remote]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    GitRemoteService remote(bus, git);

    remote.add_remote("origin", "https://github.com/test/repo.git");
    const auto result = remote.remove_remote("origin");
    REQUIRE(result.has_value());
    REQUIRE(remote.remote_count() == 0);
}

TEST_CASE("GitRemoteService get remote", "[phase25][remote]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    GitRemoteService remote(bus, git);

    remote.add_remote("origin", "https://github.com/test/repo.git");
    const auto info = remote.get_remote("origin");
    REQUIRE(info.has_value());
    REQUIRE(info->name == "origin");
    REQUIRE(info->url == "https://github.com/test/repo.git");
}

TEST_CASE("GitRemoteService get missing remote fails", "[phase25][remote]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    const GitRemoteService remote(bus, git);

    const auto info = remote.get_remote("origin");
    REQUIRE_FALSE(info.has_value());
}

TEST_CASE("GitRemoteService fetch", "[phase25][remote]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    GitRemoteService remote(bus, git);

    remote.add_remote("origin", "https://github.com/test/repo.git");
    const auto result = remote.fetch("origin");
    REQUIRE(result.remote_name == "origin");
}

TEST_CASE("GitRemoteService push", "[phase25][remote]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("f.md", "x");
    git.stage_file("f.md");
    git.commit("Test commit");

    GitRemoteService remote(bus, git);
    remote.add_remote("origin", "https://github.com/test/repo.git");

    const auto result = remote.push("origin");
    REQUIRE(result.remote_name == "origin");
    REQUIRE_FALSE(result.pushed_refs.empty());
}

TEST_CASE("GitRemoteService pull", "[phase25][remote]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");

    GitRemoteService remote(bus, git);
    remote.add_remote("origin", "https://github.com/test/repo.git");

    const auto result = remote.pull("origin");
    REQUIRE(result.remote_name == "origin");
}

TEST_CASE("GitRemoteService push to nonexistent remote", "[phase25][remote]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");

    GitRemoteService remote(bus, git);
    const auto result = remote.push("origin");
    REQUIRE_FALSE(result.rejected_refs.empty());
}

// ============================================================================
// MergeConflictResolver tests (10 tests)
// ============================================================================

TEST_CASE("MergeConflictResolver has_conflicts", "[phase25][merge]")
{
    const std::string content = "normal\n<<<<<<< HEAD\nlocal\n=======\nremote\n>>>>>>> branch\n";
    REQUIRE(MergeConflictResolver::has_conflicts(content));
    REQUIRE_FALSE(MergeConflictResolver::has_conflicts("no conflicts here"));
}

TEST_CASE("MergeConflictResolver count_conflicts", "[phase25][merge]")
{
    const std::string content = "<<<<<<< HEAD\na\n=======\nb\n>>>>>>> b1\n"
                                "normal\n"
                                "<<<<<<< HEAD\nc\n=======\nd\n>>>>>>> b2\n";
    REQUIRE(MergeConflictResolver::count_conflicts(content) == 2);
}

TEST_CASE("MergeConflictResolver parse single conflict", "[phase25][merge]")
{
    EventBus bus;
    MergeConflictResolver resolver(bus);

    const std::string content =
        "before\n<<<<<<< HEAD\nlocal line\n=======\nremote line\n>>>>>>> branch\nafter\n";
    auto file = resolver.parse_conflicts("test.md", content);

    REQUIRE(file.regions.size() == 1);
    REQUIRE(file.regions[0].local_content == "local line");
    REQUIRE(file.regions[0].remote_content == "remote line");
    REQUIRE_FALSE(file.regions[0].is_resolved);
}

TEST_CASE("MergeConflictResolver parse multiple conflicts", "[phase25][merge]")
{
    EventBus bus;
    MergeConflictResolver resolver(bus);

    const std::string content = "<<<<<<< HEAD\na\n=======\nb\n>>>>>>> b1\n"
                                "mid\n"
                                "<<<<<<< HEAD\nc\n=======\nd\n>>>>>>> b2\n";

    auto file = resolver.parse_conflicts("test.md", content);
    REQUIRE(file.regions.size() == 2);
    REQUIRE(file.regions[0].local_content == "a");
    REQUIRE(file.regions[1].remote_content == "d");
}

TEST_CASE("MergeConflictResolver resolve use local", "[phase25][merge]")
{
    EventBus bus;
    MergeConflictResolver resolver(bus);

    const std::string content = "<<<<<<< HEAD\nours\n=======\ntheirs\n>>>>>>> branch\n";
    auto file = resolver.parse_conflicts("test.md", content);

    REQUIRE(resolver.resolve_region(file, 0, ResolutionStrategy::kUseLocal));
    REQUIRE(file.regions[0].is_resolved);
    REQUIRE(file.regions[0].resolved_content == "ours");
}

TEST_CASE("MergeConflictResolver resolve use remote", "[phase25][merge]")
{
    EventBus bus;
    MergeConflictResolver resolver(bus);

    const std::string content = "<<<<<<< HEAD\nours\n=======\ntheirs\n>>>>>>> branch\n";
    auto file = resolver.parse_conflicts("test.md", content);

    REQUIRE(resolver.resolve_region(file, 0, ResolutionStrategy::kUseRemote));
    REQUIRE(file.regions[0].resolved_content == "theirs");
}

TEST_CASE("MergeConflictResolver resolve manual", "[phase25][merge]")
{
    EventBus bus;
    MergeConflictResolver resolver(bus);

    const std::string content = "<<<<<<< HEAD\nours\n=======\ntheirs\n>>>>>>> branch\n";
    auto file = resolver.parse_conflicts("test.md", content);

    REQUIRE(resolver.resolve_region_manual(file, 0, "custom merge"));
    REQUIRE(file.regions[0].resolved_content == "custom merge");
}

TEST_CASE("MergeConflictResolver resolve all", "[phase25][merge]")
{
    EventBus bus;
    MergeConflictResolver resolver(bus);

    const std::string content = "<<<<<<< HEAD\na\n=======\nb\n>>>>>>> b1\n"
                                "<<<<<<< HEAD\nc\n=======\nd\n>>>>>>> b2\n";

    auto file = resolver.parse_conflicts("test.md", content);
    const int resolved = resolver.resolve_all(file, ResolutionStrategy::kUseLocal);

    REQUIRE(resolved == 2);
    REQUIRE(file.all_resolved());
}

TEST_CASE("MergeConflictResolver generate merged content", "[phase25][merge]")
{
    EventBus bus;
    MergeConflictResolver resolver(bus);

    const std::string content =
        "before\n<<<<<<< HEAD\nours\n=======\ntheirs\n>>>>>>> branch\nafter\n";
    auto file = resolver.parse_conflicts("test.md", content);

    resolver.resolve_region(file, 0, ResolutionStrategy::kUseLocal);
    const auto merged = resolver.generate_merged_content(file);

    REQUIRE(merged.find("ours") != std::string::npos);
    REQUIRE(merged.find("<<<<<<<") == std::string::npos);
    REQUIRE(merged.find("before") != std::string::npos);
    REQUIRE(merged.find("after") != std::string::npos);
}

TEST_CASE("MergeConflictResolver unresolved returns original", "[phase25][merge]")
{
    EventBus bus;
    MergeConflictResolver resolver(bus);

    const std::string content = "<<<<<<< HEAD\na\n=======\nb\n>>>>>>> branch\n";
    auto file = resolver.parse_conflicts("test.md", content);

    const auto merged = resolver.generate_merged_content(file);
    REQUIRE(merged == content);
}

// ============================================================================
// CommitGraphEngine tests (10 tests)
// ============================================================================

TEST_CASE("CommitGraphEngine empty graph", "[phase25][graph]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");

    CommitGraphEngine engine(git);
    const auto graph = engine.build_graph();

    REQUIRE(graph.total_commits == 0);
    REQUIRE(graph.nodes.empty());
}

TEST_CASE("CommitGraphEngine build with commits", "[phase25][graph]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("a.md", "a");
    git.stage_file("a.md");
    git.commit("First");
    git.test_modify_file("a.md", "b");
    git.stage_file("a.md");
    git.commit("Second");

    CommitGraphEngine engine(git);
    const auto graph = engine.build_graph();

    REQUIRE(graph.total_commits == 2);
    REQUIRE(graph.nodes.size() == 2);
    REQUIRE_FALSE(graph.nodes[0].commit_hash.empty());
}

TEST_CASE("CommitGraphEngine limited commits", "[phase25][graph]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("a.md", "a");
    git.stage_file("a.md");
    git.commit("First");
    git.test_modify_file("a.md", "b");
    git.stage_file("a.md");
    git.commit("Second");
    git.test_modify_file("a.md", "c");
    git.stage_file("a.md");
    git.commit("Third");

    CommitGraphEngine engine(git);
    const auto graph = engine.build_graph(2);

    REQUIRE(graph.total_commits == 2);
}

TEST_CASE("CommitGraphEngine graph has edges", "[phase25][graph]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("a.md", "a");
    git.stage_file("a.md");
    git.commit("First");
    git.test_modify_file("a.md", "b");
    git.stage_file("a.md");
    git.commit("Second");

    CommitGraphEngine engine(git);
    const auto graph = engine.build_graph();

    REQUIRE_FALSE(graph.edges.empty());
}

TEST_CASE("CommitGraphEngine filter by author", "[phase25][graph]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("a.md", "a");
    git.stage_file("a.md");
    git.commit("A commit");

    CommitGraphEngine engine(git);
    const auto graph = engine.build_graph();

    // GitService commits are authored by a default author.
    const auto filtered = engine.filter_by_author(graph, "nonexistent");
    REQUIRE(filtered.total_commits == 0);
}

TEST_CASE("CommitGraphEngine search commits", "[phase25][graph]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("a.md", "a");
    git.stage_file("a.md");
    git.commit("Feature: dark mode");
    git.test_modify_file("a.md", "b");
    git.stage_file("a.md");
    git.commit("Bugfix: typo");

    CommitGraphEngine engine(git);
    const auto graph = engine.build_graph();

    const auto results = engine.search_commits(graph, "dark");
    REQUIRE(results.total_commits == 1);
    REQUIRE(results.nodes[0].message.find("dark") != std::string::npos);
}

TEST_CASE("CommitGraphEngine search case insensitive", "[phase25][graph]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("a.md", "a");
    git.stage_file("a.md");
    git.commit("Feature: DARK MODE");

    CommitGraphEngine engine(git);
    const auto graph = engine.build_graph();

    const auto results = engine.search_commits(graph, "dark mode");
    REQUIRE(results.total_commits == 1);
}

TEST_CASE("CommitGraphEngine render ascii", "[phase25][graph]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("a.md", "a");
    git.stage_file("a.md");
    git.commit("Init");

    CommitGraphEngine engine(git);
    const auto graph = engine.build_graph();

    const auto ascii = engine.render_ascii(graph);
    REQUIRE_FALSE(ascii.empty());
    REQUIRE(ascii.find('*') != std::string::npos);
    REQUIRE(ascii.find("Init") != std::string::npos);
}

TEST_CASE("CommitGraphEngine max columns assigned", "[phase25][graph]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("a.md", "a");
    git.stage_file("a.md");
    git.commit("Init");

    CommitGraphEngine engine(git);
    const auto graph = engine.build_graph();

    REQUIRE(graph.max_columns >= 1);
}

TEST_CASE("CommitGraphEngine branch head marked", "[phase25][graph]")
{
    EventBus bus;
    Config config;
    GitService git(bus, config);
    git.open_repository("/test/repo");
    git.test_add_file("a.md", "a");
    git.stage_file("a.md");
    git.commit("Init");

    CommitGraphEngine engine(git);
    const auto graph = engine.build_graph();

    // With a single branch (main), first node should be head.
    bool has_head = false;
    for (const auto& node : graph.nodes)
    {
        if (node.is_branch_head)
        {
            has_head = true;
        }
    }
    REQUIRE(has_head);
}

// ============================================================================
// VCCommandProvider tests (10 tests)
// ============================================================================

TEST_CASE("VCCommandProvider has 10 commands", "[phase25][commands]")
{
    const VCCommandProvider provider;
    REQUIRE(provider.command_count() == 10);
}

TEST_CASE("VCCommandProvider find by id", "[phase25][commands]")
{
    const VCCommandProvider provider;

    const auto* cmd = provider.find_command("git.commit");
    REQUIRE(cmd != nullptr);
    REQUIRE(cmd->title == "Git: Commit");
}

TEST_CASE("VCCommandProvider find missing returns null", "[phase25][commands]")
{
    const VCCommandProvider provider;
    REQUIRE(provider.find_command("git.nonexistent") == nullptr);
}

TEST_CASE("VCCommandProvider categories", "[phase25][commands]")
{
    const VCCommandProvider provider;
    const auto cats = provider.categories();

    REQUIRE_FALSE(cats.empty());
    REQUIRE(cats.size() >= 4);
}

TEST_CASE("VCCommandProvider commands for category", "[phase25][commands]")
{
    const VCCommandProvider provider;
    const auto staging = provider.commands_for_category("Git: Staging");

    REQUIRE(staging.size() == 2); // stage + unstage
}

TEST_CASE("VCCommandProvider stash commands", "[phase25][commands]")
{
    const VCCommandProvider provider;
    const auto stash_cmds = provider.commands_for_category("Git: Stash");

    REQUIRE(stash_cmds.size() == 2); // stash + stash_pop
}

TEST_CASE("VCCommandProvider remote commands", "[phase25][commands]")
{
    const VCCommandProvider provider;
    const auto remote_cmds = provider.commands_for_category("Git: Remote");

    REQUIRE(remote_cmds.size() == 2); // push + pull
}

TEST_CASE("VCCommandProvider tools commands", "[phase25][commands]")
{
    const VCCommandProvider provider;
    const auto tool_cmds = provider.commands_for_category("Git: Tools");

    REQUIRE(tool_cmds.size() == 2); // blame + graph
}

TEST_CASE("VCCommandProvider all commands have ids", "[phase25][commands]")
{
    const VCCommandProvider provider;
    for (const auto& cmd : provider.commands())
    {
        REQUIRE_FALSE(cmd.id.empty());
        REQUIRE_FALSE(cmd.title.empty());
        REQUIRE_FALSE(cmd.category.empty());
        REQUIRE_FALSE(cmd.description.empty());
    }
}

TEST_CASE("VCCommandProvider commands list is stable", "[phase25][commands]")
{
    const VCCommandProvider provider;
    const auto& cmds = provider.commands();
    REQUIRE(cmds.size() == 10);

    // Verify specific known commands exist.
    bool has_init = false;
    bool has_graph = false;
    for (const auto& cmd : cmds)
    {
        if (cmd.id == "git.init")
        {
            has_init = true;
        }
        if (cmd.id == "git.graph")
        {
            has_graph = true;
        }
    }
    REQUIRE(has_init);
    REQUIRE(has_graph);
}
