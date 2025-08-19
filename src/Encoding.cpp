
#include <consoleapi2.h>
#include <cstdint>
#include <cstring>
#include <Encoding.h>
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

/*
 * @brief 将任意中文（简体）格式转化为utf8
 * @param _anycode 任意中文（简体）格式的string
 * @return 转化为utf8格式的string
 */
std::string chcode_to_utf8(const std::string &_anycode) {
    std::string incode = "";
    while (incode.size()<150) {
        incode = incode + _anycode;
    }
    std::vector< EncodingDetectionResult > results;//结果函数
    std::string                            out;//输出
    std::string                            e;//错误
    ICUEncodingHandler::detect_encoding(incode.c_str( ), incode.size( ), results, 1);
    ICUEncodingHandler::convert_to_utf8(_anycode.c_str( ), _anycode.size( ), results[0].encodingName, out, e);
    return out;

}
}    // namespace encoding