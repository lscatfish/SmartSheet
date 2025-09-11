
#include <ErrorHandler/BaseException.hpp>
#include <string>
#include <source_location>

/*
 * 所有的错误信息都请使用英文书写
 */

BaseException::BaseException(
    int                         code,
    const std::string          &msg,
    const std::source_location &loc)
    : err_code_(code), err_msg_(msg), loc_(loc) {}

// 重写 what() 方法，返回错误描述
const char *BaseException::what( ) const noexcept {
    return err_msg_.c_str( );
}

// 获取错误码
int BaseException::code( ) const noexcept {
    return err_code_;
}

// 获取异常发生位置的字符串描述
std::string BaseException::location( ) const {
    return std::string("File: ")
           + loc_.file_name( )
           + " Line: " + std::to_string(loc_.line( ))
           + " Func: " + loc_.function_name( );
}
