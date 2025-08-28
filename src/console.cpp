/********************************************************************
 * 用于操作控制台
 *
 * 作者：lscatfish
 *********************************************************************/
#include <cstdlib>
#ifdef _WIN32
#include <windows.h>    // Windows系统需要的头文件
#else
#include <locale>    // Linux/macOS需要的头文件
#include <csetjmp>
#endif

#include <basic.hpp>
#include <console.h>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/component/event.hpp>
#include <ftxui/screen/color.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/dom/elements.hpp>
#include <streambuf>
#include <locale>
#include <memory>
#include <ftxui/dom/node.hpp>
#include <utility>
#include <vector>
#include <string>
#include <thread>
#include <mutex>
#include <third_party/lib/ftxui-6.1.9-win64/Debug/include/ftxui/component/component.hpp>

// 控制台的空间
namespace console {

// 清理缓冲区
void clear_input_buffer( ) {
    std::streambuf *sb = std::cin.rdbuf( );
    while (sb->in_avail( ) > 0) {
        sb->sbumpc( );    // 读取并丢弃一个字符
    }
}

// 清空控制台
void clearConsole( ) {
#ifdef _WIN32
    // Windows 系统使用 "cls" 命令
    system("cls");
#else
    // Linux/macOS 系统使用 "clear" 命令
    system("clear");
#endif
}

/**
 * 设置控制台输入输出编码为UTF-8
 * 支持Windows和Linux/macOS系统
 */
void set_console_utf8( ) {
#ifdef _WIN32
    // Windows系统设置控制台编码为UTF-8
    // 获取标准输入、输出、错误流的句柄
    HANDLE hInput  = GetStdHandle(STD_INPUT_HANDLE);
    HANDLE hOutput = GetStdHandle(STD_OUTPUT_HANDLE);

    // 设置输入输出编码为UTF-8 (CP_UTF8)
    SetConsoleCP(CP_UTF8);          // 设置控制台输入编码
    SetConsoleOutputCP(CP_UTF8);    // 设置控制台输出编码

    // 可选：设置标准流的locale为UTF-8
    std::wcin.imbue(std::locale(""));
    std::wcout.imbue(std::locale(""));
#else
    // Linux/macOS系统设置locale为UTF-8
    std::setlocale(LC_ALL, "en_US.UTF-8");

    // 设置标准输入输出流的编码
    std::cin.imbue(std::locale("en_US.UTF-8"));
    std::cout.imbue(std::locale("en_US.UTF-8"));
#endif
}

/* ================================================================================================================== */


// -------------- 构造函数 --------------
SplitConsole::SplitConsole( ) {
    using namespace ftxui;    // 仅在函数内使用，保持头文件干净

    /* 输入框组件 */
    input_box_ = Input(&input_buf_, "type here…");

    /* 根 UI：固定高度布局 */
    root_ = Renderer([this] { return BuildUI( ); });

    /* 后台 UI 线程 */
    ui_thread_ = std::thread([this] {
        auto screen = ftxui::ScreenInteractive::Fullscreen( );
        screen.Loop(root_);
    });
}

// -------------- 析构 --------------
SplitConsole::~SplitConsole( ) {
    if (auto *active = ftxui::ScreenInteractive::Active( ))
        active->Post(ftxui::Event::Custom);
    if (ui_thread_.joinable( )) ui_thread_.join( );
}

// -------------- 构造带输入的完整界面 --------------
ftxui::Element SplitConsole::BuildUI( ) {
    std::lock_guard< std::mutex > lock(mutex_);

    using namespace ftxui;

    /* 上 3/4 日志（可滚动） */
    Elements runtime_lines;
    for (const auto &l : runtime_buf_) runtime_lines.push_back(text(l));
    auto top = vbox(std::move(runtime_lines)) | yflex;

    /* 下 1/4 日志滚动区 */
    Elements bottom_lines;
    for (const auto &l : other_buf_) bottom_lines.push_back(text(l));
    auto bottom_log = vbox(std::move(bottom_lines)) | yflex;

    /* 输入栏（纯文本占位，固定 1 行） */
    auto input_line = text(input_buf_) | size(HEIGHT, EQUAL, 1);

    /* 最终布局：上下固定比例 */
    return vbox({ top | yflex,
                  separator( ),
                  bottom_log | yflex,
                  separator( ),
                  input_line });
}

// -------------- 日志接口（线程安全） --------------
void SplitConsole::AppendRuntime(const std::string &line) {
    {
        std::lock_guard< std::mutex > lock(mutex_);
        runtime_buf_.push_back(line);
    }
    Refresh( );
}
void SplitConsole::AppendOther(const std::string &line) {
    {
        std::lock_guard< std::mutex > lock(mutex_);
        other_buf_.push_back(line);
    }
    Refresh( );
}
void SplitConsole::ClearRuntime( ) {
    {
        std::lock_guard< std::mutex > lock(mutex_);
        runtime_buf_.clear( );
    }
    Refresh( );
}
void SplitConsole::ClearOther( ) {
    {
        std::lock_guard< std::mutex > lock(mutex_);
        other_buf_.clear( );
    }
    Refresh( );
}

// -------------- 阻塞读取一行 --------------
std::string SplitConsole::ReadLine( ) {
    new_line_ready_ = false;
    input_buf_.clear( );

    while (true) {
        std::this_thread::sleep_for(std::chrono::milliseconds(50));

        std::lock_guard< std::mutex > lock(mutex_);
        if (new_line_ready_) {
            new_line_ready_ = false;
            return last_line_;
        }
    }
}

// -------------- 触发 UI 刷新 --------------
void SplitConsole::Refresh( ) {
    if (auto *active = ftxui::ScreenInteractive::Active( ))
        active->Post(ftxui::Event::Custom);
}



}    // namespace console