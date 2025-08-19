/**
 * @file icu_encoding_handler.h
 * @brief ICU 77.1编码处理工具，支持编码检测、转换及系统编码获取
 */

#pragma once

#ifndef ICU_ENCODING_HANDLER_H
#define ICU_ENCODING_HANDLER_H

#include <string>
#include <unicode/utypes.h>
#include <vector>

/**
 * @brief 编码检测结果结构体
 */
struct EncodingDetectionResult {
    std::string encodingName;    ///< 编码名称
    float       confidence;      ///< 可信度(0.0-1.0)
    bool        hasBom;          ///< 是否包含BOM
};

/**
 * @brief ICU编码处理工具类
 */
namespace ICUEncodingHandler {

/**
 * @brief 检测字符串的编码格式
 * @param data 待检测的字节数据
 * @param dataLen 数据长度
 * @param results 输出参数，存储检测结果
 * @param maxResults 最大返回结果数量
 * @return 成功返回true，失败返回false
 */
bool detect_encoding(const char *data, int32_t dataLen,
                     std::vector< EncodingDetectionResult > &results,
                     int32_t                                 maxResults = 5);

/**
 * @brief 将指定编码的字符串转换为UTF-8编码
 * @param srcData 源数据
 * @param srcDataLen 源数据长度
 * @param srcEncoding 源数据编码
 * @param destUtf8 输出参数，存储转换后的UTF-8字符串
 * @param errorMsg 输出参数，存储错误信息
 * @return 成功返回true，失败返回false
 */
bool convert_to_utf8(const char *srcData, int32_t srcDataLen,
                     const std::string &srcEncoding,
                     std::string       &destUtf8,
                     std::string       &errorMsg);

/**
 * @brief 将UTF-8编码的字符串转换为指定编码
 * @param utf8Data UTF-8源数据
 * @param utf8DataLen 源数据长度
 * @param destEncoding 目标编码
 * @param destData 输出参数，存储转换后的字符串
 * @param errorMsg 输出参数，存储错误信息
 * @return 成功返回true，失败返回false
 */
bool convert_from_utf8(const char *utf8Data, int32_t utf8DataLen,
                       const std::string &destEncoding,
                       std::string       &destData,
                       std::string       &errorMsg);

/**
 * @brief 获取系统默认编码
 * @return 系统默认编码名称
 */
std::string get_system_default_encoding( );


}    // namespace ICUEncodingHandler

#endif    // ICU_ENCODING_HANDLER_H
