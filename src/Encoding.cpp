
#include <consoleapi2.h>
#include <cstdint>
#include <cstring>
#include <Encoding.h>
#include <helper.h>
#include <icu_encoding_handler.h>
#include <interlockedapi.h>    // PSLIST_HEADER
#include <iostream>
#include <string>
#include <string>
#include <stringapiset.h>
#include <vector>
#include <Windows.h>
#include <WinNls.h>
#include <winnt.h>    // CONTEXT, PCONTEXT

namespace encoding    // 使用时请用namespace ecoding=ec;
{

// 系统默认的编码方式
std::string systemDefaultEncoding = "windows-936";

// 启动函数，配置编码
void Init( ) {
    systemDefaultEncoding = ICUEncodingHandler::get_system_default_encoding( );
}

/*
 * @brief utf8转wstring
 * @param u8 utf8编码的string
 * @return 宽字符wstring（win里面实际是utf-16）
 * @note 只能在windows编译
 **/
std::wstring utf8_to_wstring_win(const std::string u8) {
    if (u8.empty( )) return { };
    int          len = MultiByteToWideChar(CP_UTF8, 0, u8.c_str( ), -1, nullptr, 0);
    std::wstring out(len, 0);
    MultiByteToWideChar(CP_UTF8, 0, u8.c_str( ), -1, &out[0], len);
    out.pop_back( );    // 去掉结尾 L'\0'
    return out;
}

/**
 * @brief 删除字符串中的所有ASCII字符（ASCII码0-127）
 * @param input 输入字符串，可能包含ASCII和非ASCII字符
 * @return 处理后的字符串，仅保留非ASCII字符
 */
static std::string remove_ascii_characters(const std::string &input) {
    std::string result;
    // 预留足够空间，避免频繁内存分配
    result.reserve(input.size( ));

    // 遍历输入字符串的每个字符
    for (char c : input) {
        // ASCII字符的范围是0-127
        if (static_cast< unsigned char >(c) > 127) {
            // 非ASCII字符，保留
            result += c;
        }
        // ASCII字符会被自动跳过
    }

    return result;
}

/*
 * @brief 将系统默认的中文（简体）格式转化为utf8
 * @param _anycode 任意中文（简体）格式的string
 * @return 转化为utf8格式的string
 */
std::string sysdcode_to_utf8(const std::string &_anycode) {

    std::string out;    // 输出
    std::string e;      // 错误

    if (ICUEncodingHandler::convert_to_utf8(_anycode.c_str( ), _anycode.size( ), systemDefaultEncoding, out, e)) {
        return out;
    } else {
        std::cout << std::endl
                  << e << std::endl;
        pause( );
        return "error";
    }
}

}    // namespace encoding