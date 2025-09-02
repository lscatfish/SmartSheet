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
#include <sys/ioctl.h>
#include <unistd.h>
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
#include <ostream>
#include <cstdint>
#include <functional>
#include <limits>
#include <ios>
#include <iomanip>

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
void clear_console( ) {
#ifdef _WIN32
    // Windows 系统使用 "cls" 命令
    system("cls");
#else
    // Linux/macOS 系统使用 "clear" 命令
    system("clear");
#endif
}

// 清空控制台第i行之后的内容(i>=0)
void clear_console_after_line(int row) {
    set_cursor_position(row, 1);
    // 清除从当前位置到屏幕末尾的内容
    std::cout << "\033[0J" << std::flush;
}

// 设置控制台光标(i行j列)
void set_cursor_position(int row, int col) {
    if (row < 1) row = 1;
    if (col < 1) col = 1;
    std::cout << "\033[" << row << ";" << col << "H" << std::flush;
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

// 获取控制台宽度
int get_console_width( ) {
#ifdef _WIN32
    HANDLE                     hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        return csbi.srWindow.Right - csbi.srWindow.Left + 1;
    }
    return -1;    // 获取失败时返回-1
#else
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != -1) {
        return w.ws_col;
    }
    return -1;    // 获取失败时返回-1
#endif
}

// 跨平台获取控制台高度（以行为单位）
int get_console_height( ) {
#ifdef _WIN32
    // Windows平台实现
    HANDLE                     hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        // 计算可见窗口的高度（行数）
        return csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
    }
    return -1;    // 获取失败时返回-1
#else
    // Linux/macOS等Unix-like平台实现
    struct winsize w;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &w) != -1) {
        return w.ws_row;    // 返回行数
    }
    return -1;    // 获取失败时返回-1
#endif
}


// 跨平台获取光标位置
CursorPosition get_cursor_position( ) {
#ifdef _WIN32
    // Windows平台实现
    HANDLE                     hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;

    if (!GetConsoleScreenBufferInfo(hConsole, &csbi)) {
        throw std::runtime_error(U8C(u8"无法获取控制台信息"));
    }

    // Windows API中坐标从0开始，转换为从1开始
    return {
        csbi.dwCursorPosition.Y + 1,    // 行号
        csbi.dwCursorPosition.X + 1     // 列号
    };
#else
    // Linux/macOS等Unix-like平台实现
    // 使用ANSI转义序列请求光标位置
    std::cout << "\033[6n" << std::flush;

    // 解析响应，格式为: \033[行;列R
    char c;
    if (std::cin.get(c) && c != '\033') {
        throw std::runtime_error(U8C(u8"获取光标位置失败"));
    }

    if (std::cin.get(c) && c != '[') {
        throw std::runtime_error(U8C(u8"获取光标位置失败"));
    }

    CursorPosition pos;
    if (!(std::cin >> pos.row)) {
        throw std::runtime_error(U8C(u8"获取光标位置失败"));
    }

    if (std::cin.get(c) && c != ';') {
        throw std::runtime_error(U8C(u8"获取光标位置失败"));
    }

    if (!(std::cin >> pos.column)) {
        throw std::runtime_error(U8C(u8"获取光标位置失败"));
    }

    if (std::cin.get(c) && c != 'R') {
        throw std::runtime_error(U8C(u8"获取光标位置失败"));
    }

    return pos;
#endif
}

// 设置光标隐藏属性（默认为隐藏）
void set_cursor_hidden(bool hidden) {
    if (hidden) {
        std::cout << "\033[?25l" << std::flush;
    } else {
        std::cout << "\033[?25h" << std::flush;
    }
}

/*
 * @brief 设置一个进度条
 * @param maxOpt 总的要进行的操作数(>0)
 * @param allBar 总的进度条长度
 * @param nowOpt 现在的操作数(>0)
 * @param setRow 将进度条设置在第setRow行
 * @param setCol 将进度条设置在第setCol列
 * @param prompt 提示信息
 */
void set_progressBar(size_t maxOpt, size_t allBar, size_t nowOpt, int setRow, int setCol, const std::string &prompt) {
    set_cursor_hidden( );
    set_cursor_position(setRow, setCol);
    double persent = nowOpt * 1.0f / maxOpt;
    std::cout << prompt << U8C(u8"进度：") << std::fixed << std::setprecision(2) << std::setw(6) << persent * 100;
    std::cout << "%    [";
    for (size_t j = 0; j < size_t(persent * allBar); j++)
        std::cout << "#";
    for (size_t j = 0; j < allBar - size_t(persent * allBar); j++)
        std::cout << " ";
    std::cout << "]";
    if (nowOpt == maxOpt) std::cout << "    Done!" << std::endl;
    set_cursor_hidden(false);
}


/* ================================================================================================================== */
/* ================================================================================================================== */
/* ================================================================================================================== */


#if false

// -------------------------- 构造/析构函数 --------------------------
ThreadSafeConsole::ThreadSafeConsole( )
    : isOutputThreadRunning_(true), isMonitorThreadRunning_(false), checkIntervalMs_(500) {
    // 初始化：启动输出线程，获取初始控制台尺寸
    outputThread_ = std::thread(&ThreadSafeConsole::outputThreadFunc, this);
    currentSize_  = getSizeFromSystem( );
}

ThreadSafeConsole::~ThreadSafeConsole( ) {
    // 停止输出线程
    isOutputThreadRunning_ = false;
    outputCond_.notify_one( );    // 唤醒输出线程，避免阻塞
    if (outputThread_.joinable( )) {
        outputThread_.join( );
    }

    // 停止尺寸检测线程
    stopSizeMonitor( );
}

// -------------------------- 原有IO功能实现 --------------------------
void ThreadSafeConsole::write(const std::string &msg) {
    std::lock_guard< std::mutex > lock(outputMutex_);
    outputQueue_.push(msg);
    outputCond_.notify_one( );    // 通知输出线程有新数据
}

void ThreadSafeConsole::writeln(const std::string &msg) {
    write(msg + "\n");    // 换行符统一由writeln添加
}

std::string ThreadSafeConsole::readLine(const std::string &prompt) {
    std::lock_guard< std::mutex > lock(inputMutex_);    // 确保同一时间只有一个输入

    // 先输出提示（用线程安全的write，避免与输出线程冲突）
    if (!prompt.empty( )) {
        write(prompt);
        // 等待提示输出完成（关键：避免提示还在队列中，输入就先执行）
        std::unique_lock< std::mutex > outputLock(outputMutex_);
        while (!outputQueue_.empty( )) {
            outputLock.unlock( );
            std::this_thread::yield( );    // 让出CPU，等待输出线程处理
            outputLock.lock( );
        }
    }

    // 读取输入（标准输入，阻塞当前线程）
    std::string input;
    std::getline(std::cin, input);
    return input;
}

char ThreadSafeConsole::readChar(const std::string &prompt) {
    std::lock_guard< std::mutex > lock(inputMutex_);

    if (!prompt.empty( )) {
        write(prompt);
        // 等待提示输出完成
        std::unique_lock< std::mutex > outputLock(outputMutex_);
        while (!outputQueue_.empty( )) {
            outputLock.unlock( );
            std::this_thread::yield( );
            outputLock.lock( );
        }
    }

    // 读取单个字符（忽略换行）
    char c;
    std::cin.get(c);
    // 清除输入缓冲区的换行符（避免后续readLine读取空值）
    if (c != '\n') {
        std::cin.ignore(std::numeric_limits< std::streamsize >::max( ), '\n');
    }
    return c;
}

void ThreadSafeConsole::outputThreadFunc( ) {
    while (isOutputThreadRunning_) {
        std::unique_lock< std::mutex > lock(outputMutex_);
        // 等待队列非空或线程停止
        outputCond_.wait(lock, [this]( ) {
            return !outputQueue_.empty( ) || !isOutputThreadRunning_;
        });

        // 消费输出队列
        while (!outputQueue_.empty( ) && isOutputThreadRunning_) {
            std::cout << outputQueue_.front( );
            outputQueue_.pop( );
        }
        std::cout.flush( );    // 强制刷新缓冲区，确保即时输出
    }
}

// -------------------------- 尺寸检测功能实现 --------------------------
void ThreadSafeConsole::startSizeMonitor(uint32_t                                   checkIntervalMs,
                                         std::function< void(const ConsoleSize &) > onSizeChanged) {
    std::lock_guard< std::mutex > lock(sizeMutex_);
    if (isMonitorThreadRunning_) {
        return;    // 已在运行，避免重复启动
    }

    // 初始化检测参数
    checkIntervalMs_        = checkIntervalMs;
    onSizeChanged_          = onSizeChanged;
    isMonitorThreadRunning_ = true;

    // 启动检测线程
    sizeMonitorThread_ = std::thread(&ThreadSafeConsole::sizeMonitorThreadFunc, this);
}

void ThreadSafeConsole::stopSizeMonitor( ) {
    std::lock_guard< std::mutex > lock(sizeMutex_);
    if (!isMonitorThreadRunning_) {
        return;
    }

    isMonitorThreadRunning_ = false;
    if (sizeMonitorThread_.joinable( )) {
        sizeMonitorThread_.join( );
    }
}

ThreadSafeConsole::ConsoleSize ThreadSafeConsole::getCurrentSize( ) {
    std::lock_guard< std::mutex > lock(sizeMutex_);
    return currentSize_;
}

void ThreadSafeConsole::sizeMonitorThreadFunc( ) {
    while (isMonitorThreadRunning_) {
        // 1. 获取当前系统尺寸
        ConsoleSize newSize = getSizeFromSystem( );

        // 2. 对比尺寸，若变化则更新并触发回调
        {
            std::lock_guard< std::mutex > lock(sizeMutex_);
            if (newSize != currentSize_) {
                currentSize_ = newSize;
                // 触发回调（回调在检测线程中执行，若需主线程处理需自行加同步）
                if (onSizeChanged_) {
                    onSizeChanged_(currentSize_);
                }
            }
        }

        // 3. 等待检测间隔（避免CPU占用过高）
        std::this_thread::sleep_for(std::chrono::milliseconds(checkIntervalMs_));
    }
}

// 平台相关：获取控制台尺寸（核心系统API调用）
ThreadSafeConsole::ConsoleSize ThreadSafeConsole::getSizeFromSystem( ) {
    ConsoleSize size = { 80, 24 };    // 默认尺寸（兼容性 fallback）

#ifdef _WIN32
    // Windows：使用GetConsoleScreenBufferInfo获取尺寸
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsole != INVALID_HANDLE_VALUE) {
        CONSOLE_SCREEN_BUFFER_INFO csbi;
        if (GetConsoleScreenBufferInfo(hConsole, &csbi)) {
            // 窗口尺寸 = 右下角坐标 - 左上角坐标 + 1（字符数）
            size.width  = csbi.srWindow.Right - csbi.srWindow.Left + 1;
            size.height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
        }
    }
#else
    // Linux/macOS：使用ioctl获取窗口尺寸（TIOCGWINSZ命令）
    struct winsize ws;
    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) != -1) {
        size.width  = ws.ws_col;    // 列数（宽度）
        size.height = ws.ws_row;    // 行数（高度）
    }
#endif

    return size;
}



/* ================================================================================================================== */
/* ================================================================================================================== */
/* ================================================================================================================== */

DefConsole::DefConsole( ) {
    update_size( );
    set_console_utf8( );    // 设置控制台为UTF-8编码
}

DefConsole::~DefConsole( ) {
}

void DefConsole::print(const std::string &msg) {
    if (msg.empty( )) return;
    history_.push_back(msg);
    std::cout << msg;
}

void DefConsole::println(const std::string &msg) {
    if (msg.empty( )) {
        println( );
        return;
    }
    history_.push_back(msg + "\n");
    std::cout << msg << std::endl;
}

void DefConsole::println( ) {
    history_.push_back("\n");
    std::cout << std::endl;
}

void DefConsole::print(const std::vector< std::string > &msgs) {
    for (const auto &msg : msgs) {
        if (msg.empty( )) {
            println( );
            continue;
        }
        println(msg);
    }
}

std::string DefConsole::read(const std::string &prompt) {
    std::string in;
    std_console( );
    if (!prompt.empty( )) {
        std::cout << prompt;
    }
    std::cin >> in;
    return in;
}

void DefConsole::set_cursor_position(int row, int col) {
    console::set_cursor_position(row, col);
}

CursorPosition DefConsole::get_cursor_position( ) {
    return console::get_cursor_position( );
}

void DefConsole::clear( ) {
    clear_console( );
    history_.clear( );
    std_console( );
}

void DefConsole::clear_after_line(int row) {
    clear_console_after_line(row);
}

DefConsole::SIZE DefConsole::get_size( ) {
    update_size( );
    return size_;
}

void DefConsole::std_console( ) {
    update_size( );

    std::cout << std::endl
              << std::endl;
    set_cursor_position(0, size_.height - 3);    // 倒数第2行
    for (int i = 0; i < size_.width; i++)
        std::cout << "-";
    std::cout << std::endl
              << prompt_;
}

void DefConsole::update_size( ) {
    size_.width  = get_console_width( );
    size_.height = get_console_height( );
}

#endif

}    // namespace console
