#pragma once
/********************************************************************
 * 用于操作控制台
 *
 * 作者：lscatfish
 *********************************************************************/
#ifndef CONSOLE_H
#define CONSOLE_H

#include <atomic>
#include <basic.hpp>
#include <condition_variable>
#include <cstdint>
#include <cstdlib>
#include <ftxui/component/component.hpp>
#include <ftxui/component/component_base.hpp>
#include <ftxui/component/loop.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>
#include <ftxui/dom/node.hpp>
#include <ftxui/screen/screen.hpp>
#include <iostream>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

// 控制台的空间
namespace console {

// 定义光标位置结构体，行和列从1开始计数
struct CursorPosition {
    int row;       // 行号
    int column;    // 列号
};

// 清空控制台
void clear_console( );

// 清空控制台第i行之后的内容(i>=0)
void clear_console_after_line(int row);

// 设置控制台光标(i行j列)
void set_console_cursor(int row, int col);

/**
 * 设置控制台输入输出编码为UTF-8
 * 支持Windows和Linux/macOS系统
 */
void set_console_utf8( );

// 清理缓冲区
void clear_input_buffer( );

// 获取控制台宽度
int get_console_width( );

// 跨平台获取控制台高度（以行为单位）
int get_console_height( );

// 跨平台获取光标位置
CursorPosition get_cursor_position( );

/* ========================================================================================================= */
/* ========================================================================================================= */
/* ========================================================================================================= */
#if false
class ThreadSafeConsole {
public:
    // 控制台尺寸结构体（宽=列数，高=行数，单位：字符）
    struct ConsoleSize {
        uint16_t width;     // 控制台宽度（横向字符数）
        uint16_t height;    // 控制台高度（纵向字符数）

        // 重载==，用于对比尺寸是否变化
        bool operator!=(const ConsoleSize &other) const {
            return width != other.width || height != other.height;
        }
    };

    // 单例模式（避免全局对象初始化问题，确保唯一控制台实例）
    static ThreadSafeConsole &getInstance( ) {
        static ThreadSafeConsole instance;
        return instance;
    }

    // 禁止拷贝/赋值（确保单例唯一性）
    ThreadSafeConsole(const ThreadSafeConsole &)            = delete;
    ThreadSafeConsole &operator=(const ThreadSafeConsole &) = delete;

    // -------------------------- 原有IO功能 --------------------------
    void        write(const std::string &msg);
    void        writeln(const std::string &msg);
    std::string readLine(const std::string &prompt = "");
    char        readChar(const std::string &prompt = "");

    // -------------------------- 新增尺寸检测功能 --------------------------
    // 启动尺寸检测线程（参数：检测间隔毫秒，尺寸变化回调函数）
    void startSizeMonitor(uint32_t                                          checkIntervalMs = 500,
                          std::function< void(const ConsoleSize &newSize) > onSizeChanged   = nullptr);
    // 停止尺寸检测线程
    void stopSizeMonitor( );
    // 获取当前控制台尺寸（线程安全）
    ConsoleSize getCurrentSize( );

private:
    // 私有构造函数（单例模式）
    ThreadSafeConsole( );
    // 析构函数（停止所有线程）
    ~ThreadSafeConsole( );

    // -------------------------- IO模块成员 --------------------------
    std::queue< std::string > outputQueue_;              // 输出队列
    std::mutex                outputMutex_;              // 输出队列互斥锁
    std::mutex                inputMutex_;               // 输入操作互斥锁
    std::condition_variable   outputCond_;               // 输出队列条件变量
    std::thread               outputThread_;             // 独立输出线程
    bool                      isOutputThreadRunning_;    // 输出线程运行标志

    // 输出线程主函数（消费输出队列，打印到控制台）
    void outputThreadFunc( );

    // -------------------------- 尺寸检测模块成员 --------------------------
    ConsoleSize                                currentSize_;               // 当前控制台尺寸
    std::mutex                                 sizeMutex_;                 // 尺寸数据互斥锁
    std::thread                                sizeMonitorThread_;         // 尺寸检测线程
    bool                                       isMonitorThreadRunning_;    // 检测线程运行标志
    uint32_t                                   checkIntervalMs_;           // 检测间隔（毫秒）
    std::function< void(const ConsoleSize &) > onSizeChanged_;             // 尺寸变化回调

    // 尺寸检测线程主函数（定时检查尺寸，触发回调）
    void sizeMonitorThreadFunc( );
    // 底层：调用系统API获取当前尺寸（平台相关）
    ConsoleSize getSizeFromSystem( );
};

/* ========================================================================================================= */
/* ========================================================================================================= */
/* ========================================================================================================= */


// 重定义一个简单的控制台接口，方便调用
class DefConsole {
public:
    struct SIZE {
        int width;     // 宽度（列数）
        int height;    // 高度（行数）
    };

    DefConsole( );
    ~DefConsole( );

    // 禁止拷贝/赋值（确保单例唯一性）
    static DefConsole &getInstance( ) {
        static DefConsole instance;
        return instance;
    }
    DefConsole(const DefConsole &)            = delete;
    DefConsole &operator=(const DefConsole &) = delete;

    /* ===============================IO======================================== */
    void        print(const std::string &msg);                    // 输出字符串（不换行）
    void        print(const std::vector< std::string > &msgs);    // 输出多行字符串（自动换行）
    void        println(const std::string &msg);                  // 输出字符串（换行）
    void        println( );                                       // 输出空行
    std::string read(const std::string &prompt = "");             // 读取一行输入
    template < typename _T >
    void print(_T __input) {
        std::cout << __input;
        history_.push_back(std::to_string(__input));
    }
    template < typename _T >
    void println(_T __input) {
        std::cout << __input;
        history_.push_back(std::to_string(__input) + "\n");
    }

    /* ============================cursor_position========================================== */
    void           set_cursor_position(int row, int col);    // 设置光标位置
    CursorPosition get_cursor_position( );                   // 获取光标位置


    void           clear( );                                 // 清空控制台
    void           clear_after_line(int row);                // 清除从指定行到末尾的内容

    SIZE get_size( );    // 获取当前控制台尺寸

    void std_console( );

private:
    const std::string          prompt_ = ">>> ";    // 默认提示符
    std::vector< std::string > history_;            // 输入历史
    SIZE                       size_;               // 当前控制台尺寸

    void update_size( );    // 更新当前控制台尺寸
};


}    // namespace console

#define con console::DefConsole::getInstance( )    // 方便调用
#endif

#endif    // !CONSOLE_H
