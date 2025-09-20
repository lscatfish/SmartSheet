#pragma once

#ifndef ERRORHANDLER_HPP
#define ERRORHANDLER_HPP

#include <chrono>
#include <ErrorHandler/BaseException.hpp>
#include <exception>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <source_location>

/*
 * 所有的错误信息都请使用英文书写
 */

// 异常处理工具类
class ExceptionHandler {
public:
    ExceptionHandler( )  = default;
    ~ExceptionHandler( ) = default;

    static ExceptionHandler &getInstance( );

    // 处理异常：打印信息并记录日志
    void handle(const std::exception &e);

private:
    // 日志记录：写入文件
    void logToFile(const std::exception &e);

    ExceptionHandler(const ExceptionHandler &)            = delete;
    ExceptionHandler &operator=(const ExceptionHandler &) = delete;
};
#define exceptionhandler ExceptionHandler::getInstance( )


// 宏：简化异常抛出，自动携带当前位置信息
#define THROW_EXCEPTION(EXCEPTION_TYPE, code, msg) \
    throw EXCEPTION_TYPE(code, msg, std::source_location::current( ))
#define TRYANY(_IN_CON_TRY) \
    try {                   \
        _IN_CON_TRY;        \
    }
#define CATCH_EXCEPTION(EXCEPTION_TYPE, _IN_CON_CATCH) \
    catch (const EXCEPTION_TYPE &e) {                  \
        _IN_CON_CATCH;                                 \
        exceptionhandler.handle(e);                    \
    }
#define CATCH_STD_ERROR(_IN_CON_CATCH) \
    CATCH_EXCEPTION(std::exception, _IN_CON_CATCH)
#define CATCH_UNKNOWN_ERROR(_IN_CON_CATCH)                   \
    catch (...) {                                            \
        _IN_CON_CATCH;                                       \
        std::cerr << std::endl                               \
                  << "Unknown error: Fatal!!!" << std::endl; \
    }

#define THROW_PDF_ERROR(code, msg) \
    THROW_EXCEPTION(PdfException, code, msg)
#define CATCH_PDF_ERROR(_IN_CON_CATCH) \
    CATCH_EXCEPTION(PdfException, _IN_CON_CATCH)
// 为PDF分配错误码（0-100），请使用宏表达
#define errPDF_render   0    // 渲染错误,pdfdoc错误
#define errPDF_document 1    // poppler开放层读取不到文档


#define THROW_DOCX_ERROR(code, msg) \
    THROW_EXCEPTION(DocxException, code, msg)
#define CATCH_DOCX_ERROR(_IN_CON_CATCH) \
    CATCH_EXCEPTION(DocxException, _IN_CON_CATCH)
// 为docx分配错误码（101-200），请使用宏表达
#define errDocx_ 101


#define THROW_XLSX_ERROR(code, msg) \
    THROW_EXCEPTION(XlsxException, code, msg)
#define THROW_ENCODING_ERROR(code, msg) \
    THROW_EXCEPTION(EncodingException, code, msg)
// 为xlsx分配错误码（201-300），请使用宏表达
//#define errXlsx_newloader  201    // 分配loader空间失败
//#define errXlsx_newwritter 202    // 分配writter空间失败

#endif    // !ERRORHANDLER_HPP
