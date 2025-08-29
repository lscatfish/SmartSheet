
#include <memory>    // for allocator, __shared_ptr_access
#include <string>    // for char_traits, operator+, string, basic_string

#include "ftxui/component/captured_mouse.hpp"        // for ftxui
#include "ftxui/component/component.hpp"             // for Input, Renderer, Vertical
#include "ftxui/component/component_base.hpp"        // for ComponentBase
#include "ftxui/component/component_options.hpp"     // for InputOption
#include "ftxui/component/screen_interactive.hpp"    // for Component, ScreenInteractive
#include "ftxui/dom/elements.hpp"                    // for text, hbox, separator, Element, operator|, vbox, border
#include "ftxui/util/ref.hpp"                        // for Ref

void test_for_ftxui_input( ) {
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

void test_for_ftxui_resizable_split( ) {
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
