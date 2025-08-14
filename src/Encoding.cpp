
#include <Encoding.h>
#include <consoleapi2.h>
#include <cstdint>
#include <cstring>
#include <interlockedapi.h>    // PSLIST_HEADER
#include <iostream>
#include <string>
#include <string>
#include <stringapiset.h>
#include <uchardet.h>
#include <vector>
#include <Windows.h>
#include <WinNls.h>
#include <winnt.h>    // CONTEXT, PCONTEXT

namespace encoding    // 使用时请用namespace ecoding=ec;
{

/*
 * @brief 使用uchardet检测string内容
 * @param str 输入
 * @return 编码值
 */
static std::string _detect_encoding(const std::string &str) {
    // 1. 创建编码检测器实例
    uchardet_t detector = uchardet_new( );
    if (!detector) {
        return "unknown";    // 检测器创建失败
    }

    // 2. 处理输入数据
    // 支持空字符串输入（直接返回unknown）
    if (!str.empty( )) {
        uchardet_handle_data(detector, str.data( ), str.size( ));
    }

    // 3. 通知数据处理完成
    uchardet_data_end(detector);

    // 4. 获取检测结果
    const char *encoding = uchardet_get_charset(detector);
    std::string result;

    if (encoding && *encoding != '\0') {
        result = encoding;
    } else {
        result = "unknown";
    }

    // 5. 释放资源
    uchardet_delete(detector);

    return result;
}

/*
 * @brief 转换string为标准的枚举类型
 * @param in 输入的string
 * @return ENCODING编码枚举
 */
static ENCODING _trans_string_to_enumEncoding(std::string in) {
    if (in == "UTF-8" || in == "UTF-8BOM")
        return ENCODING::UTF8;    // 带不带签名都混用
    else if (in == "UTF-16BE")
        return ENCODING::UTF16BE;
    else if (in == "UTF-16LE")
        return ENCODING::UTF16LE;
    else if (in == "UTF-32BE")
        return ENCODING::UTF32BE;
    else if (in == "UTF-32LE")
        return ENCODING::UTF32LE;
    else if (in == "GBK")
        return ENCODING::GBK;
    else if (in == "GB2312")
        return ENCODING::GBK2312;
    else if (in == "GB18030")
        return ENCODING::GB18030;
    else if (in == "Big5")
        return ENCODING::Big5;
    else if (in == "KOI8-R")
        return ENCODING::KOI8_R;
    else if (in == "ISO-8859-1")
        return ENCODING::ISO_8859_1;    // 西欧语言（英语、法语、德语等）
    else if (in == "ISO-8859-2")
        return ENCODING::ISO_8859_2;
    else if (in == "ISO-8859-5")
        return ENCODING::ISO_8859_5;
    else if (in == "ISO-8859-7")
        return ENCODING::ISO_8859_7;
    else if (in == "windows-1252" || in == "CP1252")
        return ENCODING::windows_1252;
    else if (in == "ASCII")
        return ENCODING::ASCII;
    else if (in == "Shift_JIS")
        return ENCODING::Shift_JIS;
    else if (in == "EUC-JP")
        return ENCODING::EUC_JP;
    else if (in == "EUC-KR")
        return ENCODING::EUC_KR;
    else if (in == "ISO-8859-9")
        return ENCODING::ISO_8859_9;
    else if (in == "unknown")
        return ENCODING::UNKNOWN;
    else
        return ENCODING::UNKNOWN;
}

/*
 * @brief 使用uchardet检测string内容
 * @param str 输入
 * @return 编码值
 */
ENCODING detect_encoding(const std::string &str) {
    return _trans_string_to_enumEncoding(
        _detect_encoding(str));
}

/*
 * @brief 将gbk格式转化为utf8
 * @param gbk格式的string
 * @return 转化为utf8格式的string
 */
std::string gbk_to_utf8(const std::string &gbk_str) {
    if (gbk_str.empty( )) return "";

    // GBK → UTF-16
    int          wlen = MultiByteToWideChar(936, 0, gbk_str.c_str( ), -1, nullptr, 0);
    std::wstring wstr(wlen, 0);
    MultiByteToWideChar(936, 0, gbk_str.c_str( ), -1, &wstr[0], wlen);

    // UTF-16 → UTF-8
    int         len = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str( ), -1, nullptr, 0, nullptr, nullptr);
    std::string utf8_str(len, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str( ), -1, &utf8_str[0], len, nullptr, nullptr);

    // 去掉结尾的 '\0'
    if (!utf8_str.empty( ) && utf8_str.back( ) == '\0') utf8_str.pop_back( );
    return utf8_str;
}

/*
 * @brief 将utf8格式转化为gbk
 * @param utf8格式的string
 * @return 转化为gbk格式的string
 */
std::string utf8_to_gbk(const std::string &utf8_str) {
    if (utf8_str.empty( )) return "";

    // UTF-8 → UTF-16
    int          wlen = MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str( ), -1, nullptr, 0);
    std::wstring wstr(wlen, 0);
    MultiByteToWideChar(CP_UTF8, 0, utf8_str.c_str( ), -1, &wstr[0], wlen);

    // UTF-16 → GBK
    int         len = WideCharToMultiByte(936, 0, wstr.c_str( ), -1, nullptr, 0, nullptr, nullptr);
    std::string gbk_str(len, 0);
    WideCharToMultiByte(936, 0, wstr.c_str( ), -1, &gbk_str[0], len, nullptr, nullptr);

    if (!gbk_str.empty( ) && gbk_str.back( ) == '\0') gbk_str.pop_back( );
    return gbk_str;
}

/*
 * @brief utf8转wstring
 * @param utf8编码的string
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
    ENCODING chcode = detect_encoding(_anycode);

    if (_anycode.size( ) == 0)
        return "";
    else if (chcode == ENCODING::UTF8)
        return _anycode;
    else
        return gbk_to_utf8(_anycode);
}
}    // namespace encoding