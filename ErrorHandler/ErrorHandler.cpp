
#include <chrono>
#include <corecrt.h>
#include <ctime>
#include <ErrorHandler/BaseException.hpp>
#include <ErrorHandler/ErrorHandler.hpp>
#include <exception>
#include <iomanip>
#include <iostream>
#include <fstream>


// 处理异常：打印信息并记录日志
void ExceptionHandler::handle(const std::exception &e) {
    // 1. 打印异常信息到控制台
    std::cerr << "\n[Exception Caught]\n";
    std::cerr << "What: " << e.what( ) << "\n";

    // 2. 如果是自定义异常，打印更多信息（通过 dynamic_cast 判断）
    if (const auto *base_ex = dynamic_cast< const BaseException * >(&e)) {
        std::cerr << "Code: " << base_ex->code( ) << "\n";
        std::cerr << "Location: " << base_ex->location( ) << "\n";
    }

    // 3. 记录日志到文件
    logToFile(e);
}

// 日志记录：写入文件
void ExceptionHandler::logToFile(const std::exception &e) {
    std::ofstream logFile("exception_log.txt", std::ios::app);
    if (!logFile.is_open( )) {
        std::cerr << "Failed to open log file!\n";
        return;
    }

    // 记录时间
    auto        now      = std::chrono::system_clock::now( );
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    logFile << "[" << std::put_time(std::localtime(&now_time), "%Y-%m-%d %H:%M:%S") << "]\n";

    // 记录异常信息
    logFile << "What: " << e.what( ) << "\n";
    if (const auto *base_ex = dynamic_cast< const BaseException * >(&e)) {
        logFile << "Code: " << base_ex->code( ) << "\n";
        logFile << "Location: " << base_ex->location( ) << "\n";
    }
    logFile << "-------------------------\n";
    logFile.close( );
}