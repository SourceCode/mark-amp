#include "rendering/ScriptRenderer.h"

#include <catch2/catch_test_macros.hpp>

using namespace markamp::rendering;

TEST_CASE("ScriptRenderer extracts logic from C++ files", "[rendering][script]")
{
    ScriptRenderer renderer;
    std::string cpp_code = R"(
#include <iostream>
#include "myheader.h"

class MyClass {
public:
    void do_something();
};

struct MyStruct {
    int value;
};

void global_func() {
    if (true) {
        return;
    }
}
)";

    std::string html = renderer.render(cpp_code, ".cpp");

    REQUIRE(html.find("Script Structure (.cpp)") != std::string::npos);

    // Includes
    REQUIRE(html.find("Dependencies &amp; Includes") ==
            std::string::npos); // The title might not be escaped
    REQUIRE(html.find("Dependencies & Includes") != std::string::npos);
    REQUIRE(html.find("iostream") != std::string::npos);
    REQUIRE(html.find("myheader.h") != std::string::npos);

    // Classes
    REQUIRE(html.find("Classes & Interfaces") != std::string::npos);
    REQUIRE(html.find("MyClass") != std::string::npos);
    REQUIRE(html.find("MyStruct") != std::string::npos);

    // Functions
    REQUIRE(html.find("Functions & Methods") != std::string::npos);
    REQUIRE(html.find("global_func") != std::string::npos);
}

TEST_CASE("ScriptRenderer extracts logic from Python files", "[rendering][script]")
{
    ScriptRenderer renderer;
    std::string py_code = R"(
import os
from sys import argv

class MyProcessor:
    def process(self):
        pass

def global_helper():
    pass
)";

    std::string html = renderer.render(py_code, ".py");

    REQUIRE(html.find("Script Structure (.py)") != std::string::npos);

    // Includes
    REQUIRE(html.find("os") != std::string::npos);
    REQUIRE(html.find("sys") != std::string::npos);

    // Classes
    REQUIRE(html.find("MyProcessor") != std::string::npos);

    // Functions
    REQUIRE(html.find("process") != std::string::npos);
    REQUIRE(html.find("global_helper") != std::string::npos);
}

TEST_CASE("ScriptRenderer extracts logic from TypeScript files", "[rendering][script]")
{
    ScriptRenderer renderer;
    std::string ts_code = R"(
import React from 'react';
import { Button } from './components';

export class App extends React.Component {
    render() {
        return <Button/>;
    }
}

export function helper() {
    return true;
}

const arrowFunc = () => {
    console.log("arrow");
}
)";

    std::string html = renderer.render(ts_code, ".ts");

    REQUIRE(html.find("Script Structure (.ts)") != std::string::npos);

    // Includes
    REQUIRE(html.find("react") != std::string::npos);
    REQUIRE(html.find("./components") != std::string::npos);

    // Classes
    REQUIRE(html.find("App") != std::string::npos);

    // Functions
    REQUIRE(html.find("helper") != std::string::npos);
    REQUIRE(html.find("arrowFunc") != std::string::npos);
}

TEST_CASE("ScriptRenderer handles empty or irrelevant files", "[rendering][script]")
{
    ScriptRenderer renderer;
    std::string empty_code = "/* Just some comments */\n// no structure here";

    std::string html = renderer.render(empty_code, ".txt");

    REQUIRE(html.find("Script Structure (.txt)") != std::string::npos);
    REQUIRE(html.find("No structural components found.") != std::string::npos);
}
