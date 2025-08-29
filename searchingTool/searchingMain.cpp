
#include <basic.hpp>
#include <console.h>
#include <Encoding.h>
#include <Files.h>
#include <helper.h>
#include <iostream>
#include <pdf.h>
#include <searchingTool/searchingTool.hpp>
#include <string>
#include <thread>


#if true
int main( ) {
    console::set_console_utf8( );    // 设置控制台为UTF-8编码
    encoding::Init( );
    pdf::Init( );    // 初始化poppler

    SearchingTool s;
    std::string   inputStr = "";
    while (true) {
        console::clearConsole( );
        my_inputer(inputStr);
        if (inputStr == "$exit$") {
            break;
        }
        std::cout << "Searching for \"" << inputStr << "\" ..." << std::endl;
        list< std::string > out;
        if (s.search_value(out, inputStr)) {
            for (const auto &line : out) {
                std::cout << line << std::endl;
            }
        } else {
            std::cout << "Not Found!" << std::endl;
        }
        pause( );
    }

    return 0;    // 程序正常结束
}
#else



// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <memory>    // for shared_ptr, allocator, __shared_ptr_access

#include "ftxui/component/captured_mouse.hpp"        // for ftxui
#include "ftxui/component/component.hpp"             // for Renderer, ResizableSplitBottom, ResizableSplitLeft, ResizableSplitRight, ResizableSplitTop
#include "ftxui/component/component_base.hpp"        // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"    // for ScreenInteractive
#include "ftxui/dom/elements.hpp"                    // for Element, operator|, text, center, border

using namespace ftxui;

int main( ) {
    auto screen = ScreenInteractive::Fullscreen( );

    auto middle = Renderer([] { return text("middle") | center; });
    auto left   = Renderer([] { return text("Left") | center; });
    auto right  = Renderer([] { return text("right") | center; });
    auto top    = Renderer([] { return text("top") | center; });
    auto bottom = Renderer([] { return text("bottom") | center; });

    int left_size   = 20;
    int right_size  = 20;
    int top_size    = 10;
    int bottom_size = 10;

    auto container = middle;
    container      = ResizableSplitLeft(left, container, &left_size);
    container      = ResizableSplitRight(right, container, &right_size);
    container      = ResizableSplitTop(top, container, &top_size);
    container      = ResizableSplitBottom(bottom, container, &bottom_size);

    auto renderer =
        Renderer(container, [&] { return container->Render( ) | border; });

    screen.Loop(renderer);
}

#endif
