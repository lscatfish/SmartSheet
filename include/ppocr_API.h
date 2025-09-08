#pragma once
/*
 * @file ppocr_API.h
 *
 * 详细的API接口说明请参考README.md
 * ppocr的API源码文件请参看API_sourc_code中的文件
 *
 * 作者：lscatfish、KIMI
 */
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
    std::vector< std::vector< int > > box;                  // 文本框 4 个顶点坐标[左上][右上][右下][左下]
    std::string                       text;                 // 识别出的文字
    float                             score     = -1.0f;    // 置信度
    float                             cls_score = 0.0f;     // 方向分类置信度
    int                               cls_label = -1;       // 方向分类标签
};


// 要传入的模型与字典库的地址
struct DefDirs {
    std::string rec_char_dict_path;    // 字典库地址
    std::string det_model_dir;
    std::string rec_model_dir;
    std::string cls_model_dir;
};

// 模型地址（采用系统编码）
extern DefDirs _ppocrDir_;


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

/*
 * @brief 设置字典路径
 */
using SetRecDictFunc = void (*)(const char *);


/*
 * @brief 调用ocr进行检测
 * @param _out 输出的结果
 * @param _img 输入的图片
 */
bool ocr(std::vector< std::vector< OCRPredictResult > > &_out, cv::Mat _img);

/*
 * @brief 初始化ppocr的dll
 * @return 是否成功
 */
bool Init( );

/*
 * @brief 卸载ppocr的dll
 */
void Uninit( );

}    // namespace ppocr


#endif    // !PPOCR_API_H
