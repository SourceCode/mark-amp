#include "ui/FileTypeIconResolver.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::ui;

TEST_CASE("FileTypeIconResolver: resolves files", "[file_type_resolver]")
{
    CHECK(FileTypeIconResolver::GetFileIcon("main.cpp") == "filetype-cpp");
    CHECK(FileTypeIconResolver::GetFileIcon("test.h") == "filetype-header");
    CHECK(FileTypeIconResolver::GetFileIcon("script.py") == "filetype-python");
    CHECK(FileTypeIconResolver::GetFileIcon("README.md") == "filetype-markdown");
    CHECK(FileTypeIconResolver::GetFileIcon("CMakeLists.txt") == "filetype-default");
    CHECK(FileTypeIconResolver::GetFileIcon("unknown.xyz") == "filetype-default");
}

TEST_CASE("FileTypeIconResolver: resolves folders", "[file_type_resolver]")
{
    CHECK(FileTypeIconResolver::GetFolderIcon(false) == "filetype-folder");
    CHECK(FileTypeIconResolver::GetFolderIcon(true) == "filetype-folder-open");
}
