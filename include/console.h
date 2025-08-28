#pragma once
/********************************************************************
 * 用于操作控制台
 *
 * 作者：lscatfish
 *********************************************************************/
#ifndef CONSOLE_H
#define CONSOLE_H

#include <basic.hpp>
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
#include <string>
#include <thread>
#include <vector>

// 控制台的空间
namespace console {

// 清空控制台
void clearConsole( );

/**
 * 设置控制台输入输出编码为UTF-8
 * 支持Windows和Linux/macOS系统
 */
void set_console_utf8( );

// 清理缓冲区
void clear_input_buffer( );


// 控制台分区器类：上方3/4区域显示程序提示，下方1/4区域显示其他消息
class SplitConsole {
public:
    SplitConsole( );
    ~SplitConsole( );

    void AppendRuntime(const std::string &line);
    void AppendOther(const std::string &line);
    void ClearRuntime( );
    void ClearOther( );

    // 读取用户输入（阻塞）
    std::string ReadLine( );

private:
    void             Refresh( );    // 触发 UI 重绘
    ftxui::Element BuildUI( );    // 构造带输入框的界面

    /* ---------- 数据 ---------- */
    std::mutex                 mutex_;
    std::vector< std::string > runtime_buf_;
    std::vector< std::string > other_buf_;

    /* ---------- 输入组件 ---------- */
    std::string      input_buf_;                 // 当前输入文字
    ftxui::Component input_box_;                 // ftxui::Input
    bool             new_line_ready_ = false;    // 一次完整输入已就绪
    std::string      last_line_;                 // 保存 ReadLine 结果

    /* ---------- UI ---------- */
    ftxui::Component root_;
    std::thread      ui_thread_;
};



}    // namespace console



#endif    // !CONSOLE_H
