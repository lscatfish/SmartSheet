#pragma once

#ifndef BASEEXCEPTION_HPP
#define BASEEXCEPTION_HPP

/*
 * 所有的错误信息都请使用英文书写
 */

#include <exception>
#include <source_location>    // C++20 特性，用于获取代码位置
#include <stdexcept>
#include <string>

// 基础异常类（继承自 std::exception）
class BaseException : public std::exception {
public:
    BaseException(
        int                         code,
        const std::string          &msg,
        const std::source_location &loc = std::source_location::current( ));
    ~BaseException( ) = default;

    // 重写 what() 方法，返回错误描述
    const char *what( ) const noexcept override;

    // 获取错误码
    int code( ) const noexcept;

    // 获取异常发生位置的字符串描述
    std::string location( ) const;

private:
    int                  err_code_;    // 错误码
    std::string          err_msg_;     // 错误描述
    std::source_location loc_;         // 异常发生的位置（文件、行号、函数）
};

// PDF解析错误类
class PdfException : public BaseException {
public:
    using BaseException::BaseException;    // 继承构造函数
};

// DOCX解析错误
class DocxException : public BaseException {
public:
    using BaseException::BaseException;    // 继承构造函数
};

// xlsx解析错误
class XlsxException : public BaseException {
public:
    using BaseException::BaseException;    // 继承构造函数
};

// 文字编码解析错误
class EncodingException : public BaseException {
public:
    using BaseException::BaseException;    // 继承构造函数
};



#endif    // !BASEEXCEPTION_HPP
