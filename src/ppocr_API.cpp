#include <ppocr_API.h>
#include <vector>
#include<opencv2/opencv.hpp>

namespace ppocr {
/*========================================================
 *    工具函数：加载 DLL
 *    参数：DLL 的完整路径（宽字符）
 *    返回：模块句柄；失败返回 nullptr
 *--------------------------------------------------------*/
HMODULE LoadDll(const std::wstring &dllPath) {
    HMODULE h = ::LoadLibraryW(dllPath.c_str( ));    // 加载 DLL
    if (!h) {
        std::cerr << "LoadLibrary failed: " << GetLastError( )
                  << "  DLL path: " << std::string(dllPath.begin( ), dllPath.end( )) << std::endl;
    }
    return h;
}

/*========================================================
 *    工具函数：获取 DLL 导出函数地址
 *    模板函数，支持任意返回类型
 *--------------------------------------------------------*/
template < typename FuncPtr >
bool GetExport(HMODULE h, const char *procName, FuncPtr &outFn) {
    outFn = reinterpret_cast< FuncPtr >(::GetProcAddress(h, procName));
    if (!outFn) {
        std::cerr << "GetProcAddress failed for " << procName
                  << ", error: " << GetLastError( ) << std::endl;
        return false;
    }
    return true;
}

/*
 * @brief 调用ocr进行检测
 * @param 输出的结果
 * @param 输入的图片
 * @param 输入的模型与字典库的path或dir
 */
bool ocr(std::vector< std::vector< OCRPredictResult > > &_out,
         cv::Mat                                        &_img,
         DefPPOCRDirs                                   &_dirs) {

    // 准备 DLL 路径
    const std::wstring dllPath = L"ppocr.dll";

    // 加载 DLL
    HMODULE hDll = LoadDll(dllPath);
    if (!hDll) return false;    // 加载失败直接退出

    // 获取导出函数地址
    SetModelDirFunc setModelDir = nullptr;
    GetOcrFunc      getOcr      = nullptr;

    if (!GetExport(hDll, "SetModelDir", setModelDir) || !GetExport(hDll, "GetOcr", getOcr)) {
        FreeLibrary(hDll);    // 释放 DLL
        return false;
    }

    // 5-5 设置识别字典路径（可选，但强烈建议）
    auto setRecDict = (void (*)(const char *))GetProcAddress(hDll, "SetRecDictPath");
    if (setRecDict) {
        setRecDict(_dirs.rec_char_dict_path);    // 绝对路径最稳
    }

    // 设置模型目录
    setModelDir(_dirs.det_model_dir, _dirs.rec_model_dir, _dirs.cls_model_dir, true);

    // 调用 OCR 推理
    std::vector< std::vector< OCRPredictResult > > results;
    bool                                           ok = getOcr(_img, results);
    if (!ok) {
        std::cout << "OCR error!!!" << std::endl;
        FreeLibrary(hDll);
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
    // 释放资源
    FreeLibrary(hDll);
    // system("pause");
    return true;
}
}    // namespace ppocr