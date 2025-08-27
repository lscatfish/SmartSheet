/*
 * @file ppocr_API.cpp
 * 
 * 作者：lscatfish、KIMI
 */
#include <dll.h>
#include <iostream>
#include <libloaderapi.h>
#include <opencv2/opencv.hpp>
#include <ppocr_API.h>
#include <string>
#include <vector>
#include <Windows.h>

namespace ppocr {

// 模型地址（采用系统编码）
DefDirs _ppocrDir_ = {
    "./models/ppocr_keys.txt",    // 字典库
    "./models/ch_PP-OCRv4_det_infer",
    "./models/ch_PP-OCRv4_rec_infer",
    "./models/ch_ppocr_mobile_v2.0_cls_infer"
};

//DefDirs _ppocrDir_ = {
//    "./models/ppocr_keys.txt",    // 字典库
//    "./models/ch_PP-OCRv4_det_infer",
//    "./models/ch_PP-OCRv4_rec_infer",
//    "./models/ch_ppocr_mobile_v2.0_cls_infer"
//};

// 获取导出函数地址
SetModelDirFunc setModelDir = nullptr;
GetOcrFunc      getOcr      = nullptr;
SetRecDictFunc  setRecDict  = nullptr;

// 全局dll句柄
HMODULE ppocrDLL = nullptr;

/*
 * @brief 初始化ppocr的dll
 * @return 是否成功
 */
bool Init( ) {
    // 准备 DLL 路径
    const std::wstring dllPath = L"ppocr.dll";
    // 加载 DLL
    ppocrDLL = dll::LoadDll(dllPath);
    if (!ppocrDLL) return false;    // 加载失败直接退出

    if (!dll::GetExport(ppocrDLL, "SetModelDir", setModelDir)
        || !dll::GetExport(ppocrDLL, "GetOcr", getOcr)
        || !dll::GetExport(ppocrDLL, "SetRecDictPath", setRecDict)) {
        Uninit( );    // 释放 DLL
        return false;
    }

    // 设置识别字典路径
    setRecDict(_ppocrDir_.rec_char_dict_path);
    // 设置模型目录
    setModelDir(_ppocrDir_.det_model_dir, _ppocrDir_.rec_model_dir, _ppocrDir_.cls_model_dir, true);
}

/*
 * @brief 卸载ppocr的dll
 */
void Uninit( ) {
    if (ppocrDLL != nullptr)
        FreeLibrary(ppocrDLL);    // 释放 DLL

    setModelDir = nullptr;
    getOcr      = nullptr;
    setRecDict  = nullptr;
    ppocrDLL    = nullptr;
}

/*
 * @brief 调用ocr进行检测
 * @param _out 输出的结果
 * @param _img 输入的图片
 */
bool ocr(std::vector< std::vector< OCRPredictResult > > &_out, cv::Mat _img) {

    // 调用 OCR 推理
    std::vector< std::vector< OCRPredictResult > > results;
    bool                                           ok = getOcr(_img, results);

    if (!ok) {
        std::cout << "OCR error!!!" << std::endl;
        Uninit( );
        return false;
    }

    //// 打印识别结果
    // std::cout << "OCR success: " << ok << ", result count: " << results.size( ) << std::endl;
    // for (const auto &page : results)    // 可能有多页（但通常一页）
    //{
    //    for (const auto &line : page)    // 每行文本
    //    {
    //        std::cout << line.text << std::endl;    // 直接输出文字
    //    }
    //}
    _out = results;
    return true;
}
}    // namespace ppocr
