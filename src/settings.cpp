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
#include <cstdio>
#include <cstdlib>
#include <Encoding.h>
#include <Files.h>
#include <fstream>
#include <helper.h>
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

constexpr auto KEY_OCRMODEL_DICT = "dict";
constexpr auto KEY_OCRMODEL_CLS  = "cls";
constexpr auto KEY_OCRMODEL_DET  = "det";
constexpr auto KEY_OCRMODEL_REC  = "rec";

// 设置路径
void set_path( ) {

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
        clearConsole( );
        std::cout << U8C(u8"未检测到其他的模型路径，使用默认路径：") << std::endl;
        printDirs( );
        return;
    } else if (file::is_file_inuse(_SETTINGS_)) {
        // 被占用
        clearConsole( );
        std::cout << U8C(u8"设置文件\"") << _SETTINGS_ << U8C(u8"\"被占用，使用默认路径：") << std::endl;
        printDirs( );
        return;
    }

    // 读取文件
    std::ifstream fin(_SETTINGS_);
    if (!fin.is_open( )) {
        clearConsole( );
        std::cout << U8C(u8"设置文件\"") << _SETTINGS_ << U8C(u8"\"因未知原因打开失败，使用默认路径：") << std::endl;
        printDirs( );
        return;
    }
    std::string line;           // 按行读取
    size_t      serLine = 0;    // 行号
    while (std::getline(fin, line)) {
        serLine++;
        if ((line.size( ) != 0 && line[0] == '#') || line.size( ) == 0) continue;
        line                 = trim_whitespace(line);
        auto [before, after] = split_by_equal(line);

        if (before == KEY_OCRMODEL_CLS) {
            ppocr::_ppocrDir_.cls_model_dir = after.c_str( );
            std::cout << U8C(u8"设置ocr模型cls模型地址： ") << KEY_OCRMODEL_CLS << "=" << encoding::sysdcode_to_utf8(after) << std::endl;
        } else if (before == KEY_OCRMODEL_DET) {
            ppocr::_ppocrDir_.det_model_dir = after.c_str( );
            std::cout << U8C(u8"设置ocr模型det模型地址： ") << KEY_OCRMODEL_DET << "=" << encoding::sysdcode_to_utf8(after) << std::endl;
        } else if (before == KEY_OCRMODEL_DICT) {
            ppocr::_ppocrDir_.rec_char_dict_path = after.c_str( );
            std::cout << U8C(u8"设置ocr模型字典库地址：  ") << KEY_OCRMODEL_DICT << "=" << encoding::sysdcode_to_utf8(after) << std::endl;
        } else if (before == KEY_OCRMODEL_REC) {
            ppocr::_ppocrDir_.rec_model_dir = after.c_str( );
            std::cout << U8C(u8"设置ocr模型rec模型地址： ") << KEY_OCRMODEL_REC << "=" << encoding::sysdcode_to_utf8(after) << std::endl;
        } else {
            std::cout << _SETTINGS_ << U8C(u8" 中行") << serLine << " : \"" << encoding::sysdcode_to_utf8(line) << "\""
                      << U8C(u8" 键值\"") << encoding::sysdcode_to_utf8(before) << U8C(u8"\"不存在") << std::endl;
        }
    }
    pause( );
    return;
}

}    // namespace settings
