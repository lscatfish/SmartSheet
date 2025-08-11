#pragma once

#ifndef PPOCR_API_H
#define PPOCR_API_H

#include <consoleapi2.h>         // SetConsoleOutputCP / SetConsoleCP
#include <iostream>              // 标准输入输出
#include <libloaderapi.h>        // LoadLibrary / GetProcAddress
#include <opencv2/opencv.hpp>    // OpenCV 头文件
#include <string>                // 字符串
#include <vector>                // STL 容器
#include <Windows.h>             // Windows API

namespace ppocr {
/*========================================================
 * 1. OCR 结果结构体
 *    与 DLL 中返回的结构体字段保持一致
 *--------------------------------------------------------*/
struct OCRPredictResult {
    std::vector< std::vector< int > > box;                  // 文本框 4 个顶点坐标
    std::string                       text;                 // 识别出的文字
    float                             score     = -1.0f;    // 置信度
    float                             cls_score = 0.0f;     // 方向分类置信度
    int                               cls_label = -1;       // 方向分类标签
};

/*========================================================
 * 2. 函数指针类型定义
 *--------------------------------------------------------*/

/*
 * @brief OCR 推理接口：bool GetOcr(cv::Mat, vector<vector<OCRPredictResult>>&)
 * @param 导入的图片（cv::mat）
 * @param ocr结果结构体的引用
 */
using GetOcrFunc = bool (*)(cv::Mat, std::vector< std::vector< OCRPredictResult > > &);

/*
 * @brief 设置模型路径：void SetModelDir(const char*, const char*, const char*, bool)
 * @param det模型
 * @param rec模型
 * @param cls模型
 * @param true
 */
using SetModelDirFunc = void (*)(const char *, const char *, const char *, bool);

/*========================================================
 * 3. 工具函数：加载 DLL
 *    参数：DLL 的完整路径（宽字符）
 *    返回：模块句柄；失败返回 nullptr
 *--------------------------------------------------------*/
HMODULE LoadDll(const std::wstring &dllPath);

/*========================================================
 * 4. 工具函数：获取 DLL 导出函数地址
 *    模板函数，支持任意返回类型
 *--------------------------------------------------------*/
template < typename FuncPtr >
bool GetExport(HMODULE h, const char *procName, FuncPtr &outFn);

// 要传入的模型与字典库的地址
struct DefPPOCRDirs {
    const char *rec_char_dict_path;    // 字典库地址
    const char *det_model_dir;
    const char *rec_model_dir;
    const char *cls_model_dir;
};

/*
 * @brief 调用ocr进行检测
 * @param 输出的结果
 * @param 输入的图片的地址
 * @param 输入的模型与字典库的path或dir
 */
bool ocr(std::vector< std::vector< OCRPredictResult > > &_out,
         cv::Mat                                     &_img,
         DefPPOCRDirs                                   &_dirs);





}    // namespace ppocr


#endif    // !PPOCR_API_H
