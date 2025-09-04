/**********************************************************************************************
 *
 * 此文件用于处理日后更换各种必要的配置，例如ppocr的路径等
 *
 * 作者：lscatfish
 * 邮箱：2561925435@qq.com
 *
 ***********************************************************************************************/
#include <basic.hpp>
#include <chrono>
#include <console.h>
#include <cstdio>
#include <cstdlib>
#include <Encoding.h>
#include <Files.h>
#include <fstream>
#include <helper.h>
#include <imgs.h>
#include <iosfwd>
#include <iostream>
#include <map>
#include <ppocr_API.h>
#include <regex>
#include <settings.h>
#include <sstream>
#include <string>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>


namespace settings {

const std::string _SETTINGS_ = "settings.txt";

constexpr auto KEY_OCRMODEL_DICT                 = "dict";
constexpr auto KEY_OCRMODEL_CLS                  = "cls";
constexpr auto KEY_OCRMODEL_DET                  = "det";
constexpr auto KEY_OCRMODEL_REC                  = "rec";
constexpr auto KEY_ManualDocPerspectiveCorrector = "ManualDocPerspectiveCorrector";
constexpr auto KEY_ImageEnhancement              = "ImageEnhancement";    // 开放一个键
// constexpr auto KEY_ImageEnhancementLightSharp    = "ImageEnhancement";
// constexpr auto KEY_ImageEnhancementAuto          = "ImageEnhancement";

// 预先输出
static void preprint_error(size_t serLine, const std::string &line, const std::string &other = "") {
    std::cout << _SETTINGS_ << U8C(u8" 中行") << serLine
              << " : \"" << encoding::sysdcode_to_utf8(line) << "\"" << other << std::endl;
}

// 设置路径
void set_path( ) {

    console::clear_console( );
    auto printDirs = []( ) -> void {
        std::cout << U8C(u8"dict=") << ppocr::_ppocrDir_.rec_char_dict_path << std::endl;
        std::cout << U8C(u8"cls=") << ppocr::_ppocrDir_.cls_model_dir << std::endl;
        std::cout << U8C(u8"det=") << ppocr::_ppocrDir_.det_model_dir << std::endl;
        std::cout << U8C(u8"rec=") << ppocr::_ppocrDir_.rec_model_dir << std::endl;
        pause( );
    };

    // 先检测设置文件是否存在
    if (!file::is_file_exists(_SETTINGS_)) {
        // 不存在说明采用默认路径
        console::clear_console( );
        std::cout << U8C(u8"未检测到其他的模型路径，使用默认路径：") << std::endl;
        printDirs( );
        return;
    } else if (file::is_file_inuse(_SETTINGS_)) {
        // 被占用
        console::clear_console( );
        std::cout << U8C(u8"设置文件\"") << _SETTINGS_ << U8C(u8"\"被占用，使用默认路径：") << std::endl;
        printDirs( );
        return;
    }

    // 读取文件
    std::ifstream fin(_SETTINGS_);
    if (!fin.is_open( )) {
        console::clear_console( );
        std::cout << U8C(u8"设置文件\"") << _SETTINGS_ << U8C(u8"\"因未知原因打开失败，使用默认路径：") << std::endl;
        printDirs( );
        return;
    }
    std::string line;           // 按行读取
    size_t      serLine = 0;    // 行号
    while (std::getline(fin, line)) {
        serLine++;
        line = trim_whitespace(line);
        if ((line.size( ) != 0 && line[0] == '#') || line.size( ) == 0) continue;
        auto [usable, annotation] = split_by(line, '#');    // 分离注释
        auto [before, after]      = split_by_equal(usable);
        after                     = trim_whitespace(after);
        before                    = trim_whitespace(before);
        if (after.size( ) != 0) {
            if (before == KEY_OCRMODEL_CLS) {
                ppocr::_ppocrDir_.cls_model_dir = after;
                std::cout << U8C(u8"设置ocr模型cls模型地址： ") << KEY_OCRMODEL_CLS << "=" << encoding::sysdcode_to_utf8(after) << std::endl;
            } else if (before == KEY_OCRMODEL_DET) {
                ppocr::_ppocrDir_.det_model_dir = after;
                std::cout << U8C(u8"设置ocr模型det模型地址： ") << KEY_OCRMODEL_DET << "=" << encoding::sysdcode_to_utf8(after) << std::endl;
            } else if (before == KEY_OCRMODEL_DICT) {
                ppocr::_ppocrDir_.rec_char_dict_path = after;
                std::cout << U8C(u8"设置ocr模型字典库地址：  ") << KEY_OCRMODEL_DICT << "=" << encoding::sysdcode_to_utf8(after) << std::endl;
            } else if (before == KEY_OCRMODEL_REC) {
                ppocr::_ppocrDir_.rec_model_dir = after;
                std::cout << U8C(u8"设置ocr模型rec模型地址： ") << KEY_OCRMODEL_REC << "=" << encoding::sysdcode_to_utf8(after) << std::endl;
            } else if (before == KEY_ManualDocPerspectiveCorrector) {
                bool ep = false;    // 是否进行错误打印
                if (is_alpha_numeric(after)) {
                    after = lower_alpha_numeric(after);
                    if (after == "true") {
                        std::cout << U8C(u8"手动透视校正：启用") << std::endl;
                        img::enable_ManualDocPerspectiveCorrector = true;
                    } else if (after == "false") {
                        std::cout << U8C(u8"手动透视校正：禁用") << std::endl;
                        img::enable_ManualDocPerspectiveCorrector = false;
                    } else
                        ep = true;
                }
                if (ep) preprint_error(serLine, line, U8C(u8"键值只能是true或是false，你输入键值\"") + encoding::sysdcode_to_utf8(after) + U8C(u8"\"是错误的，采用默认值false"));
            } else if (before == KEY_ImageEnhancement) {
                bool ep = false;    // 是否进行错误打印
                if (is_alpha_numeric(after)) {
                    after = lower_alpha_numeric(after);
                    if (after == "auto") {
                        std::cout << U8C(u8"自动图片增强：启用") << std::endl;
                        img::enable_ImageEnhancementAuto = true;
                    } else if (after == "removeshadow") {
                        std::cout << U8C(u8"图片阴影去除：启用") << std::endl;
                        img::enable_ImageEnhancementRemoveShadow = true;
                    } else if (after == "sharpen") {
                        std::cout << U8C(u8"图片轻度锐化：启用") << std::endl;
                        img::enable_ImageEnhancementLightSharp = true;
                    } else
                        ep = true;
                }
                if (ep) preprint_error(serLine, line, U8C(u8"键值只能是auto、removeshadow、sharpen，你输入键值\"") + encoding::sysdcode_to_utf8(after) + U8C(u8"\"是错误的，采用默认值：不启用此功能"));
            } else {
                preprint_error(serLine, line, U8C(u8"键\"") + encoding::sysdcode_to_utf8(before) + U8C(u8"\"不存在"));
            }
        } else {
            preprint_error(serLine, line, U8C(u8"键\"") + encoding::sysdcode_to_utf8(before) + U8C(u8"\"设置为空，采用默认路径"));
        }
    }
    pause( );
    fin.close( );
    return;
}

}    // namespace settings
