#pragma once

#ifndef ERRORHANDLER_HPP
#define ERRORHANDLER_HPP

#include <chrono>
#include <ErrorHandler/BaseException.hpp>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <exception>


// 异常处理工具类
class ExceptionHandler {
public:
    // 处理异常：打印信息并记录日志
    static void handle(const std::exception &e);
  
private:
    // 日志记录：写入文件
    static void logToFile(const std::exception &e);
  
};



#endif    // !ERRORHANDLER_HPP
