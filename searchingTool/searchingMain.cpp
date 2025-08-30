
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


#if false
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
#elif false

// Copyright 2020 Arthur Sonzogni. All rights reserved.
// Use of this source code is governed by the MIT license that can be found in
// the LICENSE file.
#include <memory>    // for shared_ptr, allocator, __shared_ptr_access

#include "ftxui/component/captured_mouse.hpp"        // for ftxui
#include "ftxui/component/component.hpp"             // for Renderer, ResizableSplitBottom, ResizableSplitLeft, ResizableSplitRight, ResizableSplitTop
#include "ftxui/component/component_base.hpp"        // for ComponentBase
#include "ftxui/component/screen_interactive.hpp"    // for ScreenInteractive
#include "ftxui/dom/elements.hpp"                    // for Element, operator|, text, center, border
#include <ftxui/dom/take_any_args.hpp>

using namespace ftxui;

int main( ) {
    using namespace ftxui;

    // The data:
    std::string first_name;
    std::string last_name;
    std::string password;
    std::string phoneNumber;

    // The basic input components:
    Component input_first_name = Input(&first_name, "first name");
    Component input_last_name  = Input(&last_name, "last name");

    // The password input component:
    InputOption password_option;
    password_option.password = true;
    Component input_password = Input(&password, "password", password_option);

    // The phone number input component:
    // We are using `CatchEvent` to filter out non-digit characters.
    Component input_phone_number = Input(&phoneNumber, "phone number");
    input_phone_number |= CatchEvent([&](Event event) {
        return event.is_character( ) && !std::isdigit(event.character( )[0]);
    });
    input_phone_number |= CatchEvent([&](Event event) {
        return event.is_character( ) && phoneNumber.size( ) > 10;
    });

    // The component tree:
    auto component = Container::Vertical({
        input_first_name,
        input_last_name,
        input_password,
        input_phone_number,
    });

    // Tweak how the component tree is rendered:
    auto renderer = Renderer(component, [&] {
        return vbox({
                   hbox(text(" First name : "), input_first_name->Render( )),
                   hbox(text(" Last name  : "), input_last_name->Render( )),
                   hbox(text(" Password   : "), input_password->Render( )),
                   hbox(text(" Phone num  : "), input_phone_number->Render( )),
                   separator( ),
                   text("Hello " + first_name + " " + last_name),
                   text("Your password is " + password),
                   text("Your phone number is " + phoneNumber),
               })
               | border;
    });

    auto screen = ScreenInteractive::TerminalOutput( );
    screen.Loop(renderer);
}

#elif true

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <thread>
#include <chrono>
#include <string>
#include <ftxui/component/event.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/take_any_args.hpp>

/* =========================================试写ui============================================ */

int main( ) {
    // 输入
    std::string inStr  = "";
    std::string outStr = "";

    ftxui::Component input_inStr = ftxui::Input(&inStr, "");    // 输入

    auto component = ftxui::Container::Vertical({ input_inStr });

    auto renderer = ftxui::Renderer(component, [&] {
        return ftxui::vbox({ ftxui::hbox(ftxui::text("Input>>>"), input_inStr->Render( )),
                             ftxui::separator( ),
                             ftxui::hbox(ftxui::paragraph("Your input is " + outStr)) });
    });

    auto screen = ftxui::ScreenInteractive::TerminalOutput( );

    std::thread background_thread(
        [&]( ) {

        });

    // 添加输入机制
    auto input_handler = ftxui::CatchEvent(
        [&](ftxui::Event event) {
            if (event == ftxui::Event::Return) {
                screen.Post(
                    [&]( ) {
                        outStr = inStr;
                        inStr  = "";
                    });
                return true;
            }
            return false;
        });
    // 为UI添加退出机制（按ESC键退出）
    auto exit_handler = ftxui::CatchEvent([&](ftxui::Event event) {    // 用 CatchEvent 替代
        if (event == ftxui::Event::Escape) {
            screen.ExitLoopClosure( );    // 退出事件循环
            return true;                  // 表示事件已处理
        }
        return false;    // 其他事件继续传递
    });
    renderer          = renderer | input_handler | exit_handler;
    screen.Loop(renderer);
}








#else

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <thread>
#include <chrono>
#include <string>
#include <ftxui/component/event.hpp>
#include <ftxui/screen/color.hpp>

using namespace ftxui;

int main( ) {
    // 存储实时提示信息（主线程和后台线程共享）
    std::string status_message = U8C(u8"程序启动中...");
    bool        is_running     = true;    // 控制后台线程的运行状态

    // 创建一个简单的UI组件（用于展示提示信息）
    Component status_component = Renderer([&] {
        return vbox({ text(U8C(u8"实时运行提示:")) | bold,
                      separator( ),
                      text(status_message) | color(Color::Green),    // 绿色显示提示
                      separator( ),
                      text(U8C(u8"按ESC键退出")) | color(Color::GrayLight) })
               | border | center;    // 居中带边框
    });

    // 创建交互屏幕
    auto screen = ScreenInteractive::TerminalOutput( );

    // 启动后台线程：模拟程序运行并生成提示信息
    std::thread background_thread([&]( ) {
        std::vector< std::string > steps = {
            U8C(u8"正在初始化模块..."),
            U8C(u8"加载配置文件成功"),
            U8C(u8"连接到服务器..."),
            U8C(u8"服务器连接成功"),
            U8C(u8"数据同步中..."),
            U8C(u8"所有操作完成！")
        };

        for (const auto &step : steps) {
            if (!is_running) break;

            std::this_thread::sleep_for(std::chrono::milliseconds(1500));

            // 关键：使用 screen.Post() 提交UI更新
            screen.Post([&, step] {    // 最新版正确用法
                status_message = step;
            });
        }
    });

    // 为UI添加退出机制（按ESC键退出）
    auto exit_handler = status_component | CatchEvent([&](Event event) {    // 用 CatchEvent 替代
                            if (event == Event::Escape) {
                                is_running = false;
                                screen.ExitLoopClosure( )( );    // 退出事件循环
                                return true;                     // 表示事件已处理
                            }
                            return false;    // 其他事件继续传递
                        });

    // 启动UI事件循环
    screen.Loop(exit_handler);

    // 等待后台线程结束
    if (background_thread.joinable( )) {
        background_thread.join( );
    }

    return 0;
}

#endif
