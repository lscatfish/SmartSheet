/*****************************************************************************
 * @file Files.cpp
 *
 * 作者：lscatfish、KIMI
 ****************************************************************************/
#include <algorithm>
#include <basic.hpp>
#include <cstdio>
#include <cstdlib>
#include <cstdlib>
#include <Encoding.h>
#include <errhandlingapi.h>
#include <Files.h>
#include <filesystem>
#include <Fuzzy.h>
#include <helper.h>
#include <iostream>
#include <sstream>
#include <string>
#include <sys/stat.h>
#include <sys/types.h>
#include <utility>
#include <vector>
#include <xlnt/xlnt.hpp>

#ifdef _WIN32
#include <direct.h>
#include <Windows.h>
#define mkdir(path) _mkdir(path)
#else
#include <unistd.h>
#define mkdir(path) mkdir(path, 0755)
#endif
#include <fstream>
#include <stdexcept>

namespace file {

std::string _INPUT_DIR_             = "./input/";
std::string _INPUT_ALL_DIR_         = "./input/all/";
std::string _INPUT_APP_DIR_         = "./input/app/";
std::string _INPUT_ATT_IMGS_DIR_    = "./input/att_imgs/";
std::string _INPUT_SIGN_QC_ALL_DIR_ = "./input/sign_for_QingziClass/all/";

std::string _OUTPUT_DIR_             = "./output/";
std::string _OUTPUT_APP_DIR_         = "./output/app_out/";
std::string _OUTPUT_ATT_DIR_         = "./output/att_out/";
std::string _OUTPUT_SIGN_QC_DIR_     = "./output/sign_for_QingziClass_out/";
std::string _OUTPUT_SIGN_QC_PDF_DIR_ = "./output/sign_for_QingziClass_out/pdf/";

std::string _STORAGE_DIR_ = "./storage/";

/* ========================================================================================================================= */
/* ========================================================================================================================= */
/* ========================================================================================================================= */
/* ========================================================================================================================= */

// 递归遍历文件夹，收集所有文件路径到 vector 中
void DefFolder::traverse_folder(const std::string &folderPath, list< std::string > &filePaths) {
    // 构建搜索路径（添加通配符*匹配所有项）
    std::string searchPath = folderPath + "/*";

    // 用于存储搜索结果的结构体
    WIN32_FIND_DATAA findData;
    HANDLE           hFind = FindFirstFileA(searchPath.c_str( ), &findData);

    // 检查搜索是否成功
    if (hFind == INVALID_HANDLE_VALUE) {
        std::cerr << U8C(u8"无法打开文件夹: ") << folderPath << std::endl;
        return;
    }

    // 遍历所有找到的项
    do {
        std::string fileName = findData.cFileName;

        // 跳过当前目录(.)、上级目录(..)以及__MACOSX文件夹，还有~$预加载文件
        if (fileName == "." || fileName == ".."
            || fileName == "__MACOSX" || fileName == "_MACOSX"
            || (fileName.size( ) >= 2 && fileName.substr(0, 2) == "~$")) {
            continue;
        }

        // 构建完整路径
        std::string fullPath = folderPath + "/" + fileName;

        // 判断当前项是否为文件夹
        if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            // 若是文件夹，递归遍历
            traverse_folder(fullPath, filePaths);
        } else {
            // 若是文件，添加到路径列表
            filePaths.push_back(fullPath);


            // std::cout << encoding::sysdcode_to_utf8(fullPath) << std::endl;
        }

    } while (FindNextFileA(hFind, &findData) != 0);    // 继续搜索下一项

    // 关闭搜索句柄
    FindClose(hFind);
}

/*
 * @brief 输出文件夹下的各个文件路径
 * @return list<string>类型一个列表
 */
list< std::string > DefFolder::get_filepath_list( ) const {
    return filePathList_;
}

/*
 * @brief 输出文件夹下的各个文件路径(utf8编码)
 * @return list<string>类型一个列表
 */
list< std::string > file::DefFolder::get_u8filepath_list( ) const {
    return u8filePathList_;
}

/*
 * @brief 输出指定后缀的文件路径
 * @param _extension 指定的后缀
 * @return 输出指定后缀的文件路径
 */
list< std::string > DefFolder::get_filepath_list(const list< std::string > &_extension) const {
    list< std::string > out;
    for (auto it_path = this->filePathList_.begin( ); it_path != this->filePathList_.end( ); it_path++) {
        auto [n, ex] = split_filename_and_extension(*it_path);
        if (fuzzy::search(_extension, ex, fuzzy::LEVEL::High))
            out.push_back(*it_path);
    }
    return out;
}

/*
 * @brief 输出指定后缀的文件路径(u8编码)
 * @param _extension 指定的后缀
 * @return 输出指定后缀的文件路径（u8编码）
 */
list< std::string > DefFolder::get_u8filepath_list(const list< std::string > &_extension) const {
    list< std::string > out;
    for (auto it_path = this->u8filePathList_.begin( ); it_path != this->u8filePathList_.end( ); it_path++) {
        auto [n, ex] = split_filename_and_extension(*it_path);
        if (fuzzy::search(_extension, ex, fuzzy::LEVEL::High))
            out.push_back(*it_path);
    }
    return out;
}

/*
 * @brief 保留指定后缀的文件
 * @param _extension 指定的后缀
 * @return 剩余文件的数量
 */
size_t DefFolder::keep_with(const list< std::string > &_extension) {
    for (auto it_path = this->filePathList_.begin( ); it_path != this->filePathList_.end( );) {
        auto [n, ex] = split_filename_and_extension(*it_path);
        if (fuzzy::search(_extension, ex, fuzzy::LEVEL::High)) {
            it_path++;
        } else {
            it_path = this->filePathList_.erase(it_path);
        }
    }
    for (auto it_path = this->u8filePathList_.begin( ); it_path != this->u8filePathList_.end( );) {
        auto [n, ex] = split_filename_and_extension(*it_path);
        if (fuzzy::search(_extension, ex, fuzzy::LEVEL::High)) {
            it_path++;
        } else {
            it_path = this->u8filePathList_.erase(it_path);
        }
    }
    return this->filePathList_.size( );
}

/*
 * @brief 擦除指定的后缀
 * @param _extension 指定的后缀
 * @return 剩余文件的数量
 */
size_t DefFolder::erase_with(const list< std::string > &_extension) {
    for (auto it_path = this->filePathList_.begin( ); it_path != this->filePathList_.end( );) {
        auto [n, ex] = split_filename_and_extension(*it_path);
        if (fuzzy::search(_extension, ex, fuzzy::LEVEL::High)) {
            it_path = this->filePathList_.erase(it_path);
        } else {
            it_path++;
        }
    }
    for (auto it_path = this->u8filePathList_.begin( ); it_path != this->u8filePathList_.end( );) {
        auto [n, ex] = split_filename_and_extension(*it_path);
        if (fuzzy::search(_extension, ex, fuzzy::LEVEL::High)) {
            it_path = this->u8filePathList_.erase(it_path);
        } else {
            it_path++;
        }
    }
    return this->filePathList_.size( );
}

/*
 * @brief 擦除指定文件名的文件
 * @param _path 指定的文件路径
 * @return 是否成功
 */
bool file::DefFolder::erase_with(const std::string _path) {
    // 强行转化为utf8 的编码
    std::string u8 = encoding::sysdcode_to_utf8(_path);
    // 查找
    size_t idx    = 0;
    bool   iffind = false;
    for (; idx < this->u8filePathList_.size( ); idx++) {
        if (this->u8filePathList_.front( ) == u8) {
            iffind = true;
            break;
        }
    }
    // 擦除
    if (iffind) {
        auto it_u8 = this->u8filePathList_.begin( ) + idx;
        auto it_ss = this->filePathList_.begin( ) + idx;
        u8filePathList_.erase(it_u8);
        filePathList_.erase(it_ss);
        return true;
    }
    return false;
}

/*
 * @brief 删除指定文件名的文件
 * @param _path 指定的文件路径
 * @return 是否成功
 */
bool DefFolder::delete_with(const std::string _path) {
    if (erase_with(_path)) {
        // 强行转化为sys 的编码
        std::string sys = encoding::utf8_to_sysdcode(_path);
        if (is_file_exists(sys)) {
            erase_with(sys);    // 擦除
            std::cout << U8C(u8"删除 ") << sys << std::endl;
            return true;
        }
    }
    return false;
}

/*
 * @brief 删除指定后缀的文件
 * @param _extension 指定的后缀
 * @return 删除的文件数量
 */
size_t DefFolder::delete_with(const list< std::string > _extension) {
    size_t sum             = 0;
    auto   speFilePathList = get_filepath_list(_extension);    // 特定的文件
    for (auto it = speFilePathList.begin( ); it != speFilePathList.end( ); it++) {
        if (is_file_exists(*it)) {
            erase_with(*it);    // 擦除
            if (delete_file(*it)) {
                std::cout << U8C(u8"删除 ") << encoding::sysdcode_to_utf8(*it) << std::endl;    // 尝试删除u8
                sum++;
            }
        }
    }
    return sum;
}

/*
 * @brief 删除所有文件
 * @return 删除的文件数量
 */
size_t DefFolder::delete_with( ) {
    size_t sum = 0;
    for (auto it = filePathList_.begin( ); sum < filePathList_.size( ); sum++) {
        if (delete_file(*(it + sum))) {
            std::cout << U8C(u8"删除 ") << encoding::sysdcode_to_utf8(*(it + sum)) << std::endl;    // 尝试删除u8
        }
    }
    filePathList_.clear( );    // 清空
    u8filePathList_.clear( );
    filePathList_.shrink_to_fit( );
    u8filePathList_.shrink_to_fit( );
    std::cout << U8C(u8"已清空文件夹 ") << encoding::sysdcode_to_utf8(folderDir_) << std::endl;
    return sum;
}

/*
 * @brief 复制指定后缀的文件到指定的路径
 * @param _targetDir 指定路径
 * @param _extension 指定的后缀
 * @return 复制到的文件的数量
 */
size_t DefFolder::copy_files_to(const std::string &_targetDir, const list< std::string > &_extension) const {
    list< std::string > speFilePathList = get_filepath_list(_extension);    // 特定的文件

    size_t sum = 0;
    for (const auto &fp : speFilePathList) {
        if (copy_file_to_folder(fp, _targetDir)) {
            sum++;
        }
    }
    return size_t(sum);
}

/*
 * @brief 复制文件到指定的路径
 * @param _targetDir 指定路径
 * @return 复制到的文件的数量
 */
size_t DefFolder::copy_files_to(const std::string &_targetDir) const {
    list< std::string > speFilePathList = get_filepath_list( );    // 特定的文件

    size_t sum = 0;
    for (const auto &fp : speFilePathList) {
        if (copy_file_to_folder(fp, _targetDir)) {
            sum++;
        }
    }
    return size_t(sum);
}

/*
 * @brief 返回所有的文件名（包含后缀）
 * @param _extension 指定的后缀
 * @return 返回的文件（包含后缀）
 */
list< std::string > DefFolder::get_file_list( ) const {
    list< std::string > out;
    for (const auto &fp : filePathList_) {
        if (fp.size( ) > 0) {
            out.push_back(split_file_from_path(fp));
        }
    }
    return out;
}

/*
 * @brief 返回特定后缀的文件名（包含后缀）
 * @param _extension 指定的后缀
 * @return 返回的文件（包含后缀）
 */
list< std::string > DefFolder::get_file_list(const list< std::string > &_extension) const {
    list< std::string > speFilePathList = get_filepath_list(_extension);
    list< std::string > out;
    for (const auto &fp : speFilePathList) {
        if (fp.size( ) > 0) {
            out.push_back(split_file_from_path(fp));
        }
    }
    return out;
}

/*
 * @brief 返回所有的文件名（包含后缀）
 * @param _extension 指定的后缀
 * @return 返回的文件（包含后缀）
 */
list< std::string > DefFolder::get_u8file_list( ) const {
    list< std::string > out;
    for (const auto &u8fp : u8filePathList_) {
        if (u8fp.size( ) > 0) {
            out.push_back(split_file_from_path(u8fp));
        }
    }
    return out;
}

/*
 * @brief 返回特定后缀的文件名（包含后缀）
 * @param _extension 指定的后缀
 * @return 返回的文件（包含后缀）
 */
list< std::string > DefFolder::get_u8file_list(const list< std::string > &_extension) const {
    list< std::string > speu8FilePathList = get_u8filepath_list(_extension);
    list< std::string > out;
    for (const auto &u8fp : speu8FilePathList) {
        if (u8fp.size( ) > 0) {
            out.push_back(split_file_from_path(u8fp));
        }
    }
    return out;
}

/*
 * @brief 返回所有的文件名（不包含后缀）
 * @param _extension 指定的后缀
 * @return 返回的文件名（不包含后缀）
 */
list< std::string > DefFolder::get_filename_list( ) const {
    list< std::string > fileList = get_file_list( );
    list< std::string > out;
    for (const auto &f : fileList) {
        if (f.size( ) > 0) {
            auto [filename, ex] = split_filename_and_extension(f);
            out.push_back(filename);
        }
    }
    return out;
}

/*
 * @brief 返回特定后缀的文件名（不包含后缀）
 * @param _extension 指定的后缀
 * @return 返回的文件名（不包含后缀）
 */
list< std::string > DefFolder::get_filename_list(const list< std::string > &_extension) const {
    list< std::string > fileList = get_file_list(_extension);
    list< std::string > out;
    for (const auto &f : fileList) {
        if (f.size( ) > 0) {
            auto [filename, ex] = split_filename_and_extension(f);
            out.push_back(filename);
        }
    }
    return out;
}

/*
 * @brief 返回所有的文件名（不包含后缀）
 * @param _extension 指定的后缀
 * @return 返回的文件名（不包含后缀）
 */
list< std::string > DefFolder::get_u8filename_list( ) const {
    list< std::string > u8fileList = get_u8file_list( );
    list< std::string > out;
    for (const auto &u8f : u8fileList) {
        if (u8f.size( ) > 0) {
            auto [u8filename, ex] = split_filename_and_extension(u8f);
            out.push_back(u8filename);
        }
    }
    return out;
}

/*
 * @brief 返回特定后缀的文件名（不包含后缀）
 * @param _extension 指定的后缀
 * @return 返回的文件名（不包含后缀）
 */
list< std::string > DefFolder::get_u8filename_list(const list< std::string > &_extension) const {
    list< std::string > u8fileList = get_u8file_list(_extension);
    list< std::string > out;
    for (const auto &u8f : u8fileList) {
        if (u8f.size( ) > 0) {
            auto [u8filename, ex] = split_filename_and_extension(u8f);
            out.push_back(u8filename);
        }
    }
    return out;
}

/*
 * @brief 检测此文件夹下是否有有被占用的文件
 * @param ifp 是否打印被占用的文件
 * @param _occu8PathList 输出被占用的文件路径(utf8)
 */
list< std::string > DefFolder::check_occupied_utf8(bool ifp) const {
    list< std::string > out;
    for (size_t i = 0; i < filePathList_.size( ); i++) {
        if (is_file_inuse(filePathList_[i])) {
            if (ifp)
                std::cout << U8C(u8"被占用的文件: ") << u8filePathList_[i] << std::endl;
            out.push_back(u8filePathList_[i]);
        }
    }
    return out;
}

/*
 * @brief 检测此文件夹下是否有有被占用的文件
 * @param ifp 是否打印被占用的文件
 * @return 输出被占用的文件路径(sys)
 */
list< std::string > DefFolder::check_occupied_sys(bool ifp) const {
    list< std::string > out;
    for (size_t i = 0; i < filePathList_.size( ); i++) {
        if (is_file_inuse(filePathList_[i])) {
            if (ifp)
                std::cout << U8C(u8"被占用的文件: ") << encoding::sysdcode_to_utf8(u8filePathList_[i]) << std::endl;
            out.push_back(filePathList_[i]);
        }
    }
    return out;
}

/* ========================================================================================================================= */
/* ========================================================================================================================= */
/* ========================================================================================================================= */
/* ========================================================================================================================= */


/*
 * @brief 解析文件名的后缀与文件名字（不含后缀）
 * @param _input 输入的文件名
 * @return 文件名字（不含后缀）与 后缀 的pair
 */
std::pair< std::string, std::string > split_filename_and_extension(const std::string &_input) {
    // 找到最后一个 '.' 的位置
    size_t pos = _input.find_last_of('.');
    // 如果没有找到 '.'，返回原字符和空字符串
    if (pos == std::string::npos) {
        return { _input, "" };
    }
    // 返回从字符串开始到 '.' 的子字符串（不包括 '.'）
    std::string part1 = _input.substr(0, pos);
    // 返回从 '.' 开始到字符串末尾的子字符串（包括 '.'）
    std::string part2 = _input.substr(pos);
    return { part1, part2 };
}

/*
 * @brief 从路径中提取文件名（包含后缀）
 * @param _path 输入的文件路径
 */
std::string split_file_from_path(const std::string &_path) {
    size_t pos = _path.find_last_of("/\\");
    if (pos == std::string::npos) {
        return _path;    // 没有找到路径分隔符，整个路径就是文件名
    }
    return _path.substr(pos + 1);
}

/*
 * @brief 组合文件夹路径和文件名，处理路径分隔符
 * @param _folder 文件夹路径
 * @param _filename 文件名字（带后缀）
 */
std::string combine_folderdir_and_filename(const std::string &_folder, const std::string &_filename) {
    if (_folder.empty( )) {
        return _filename;
    }

    char lastChar = _folder.back( );
    if (lastChar == '/' || lastChar == '\\') {
        return _folder + _filename;
    } else {
        // 使用系统默认路径分隔符
#ifdef _WIN32
        return _folder + "\\" + _filename;
#else
        return folder + "/" + filename;
#endif
    }
}

/*
 * @brief 复制文件函数，目标为文件夹路径
 * @param _sourcePath 要复制的文件路径
 * @param _destFolder 要复制到的文件夹路径
 */
bool copy_file_to_folder(const std::string &_sourcePath, const std::string &_destFolder) {

    namespace fs = std::filesystem;

    // 获取源文件的文件名
    std::string fileName = split_file_from_path(_sourcePath);

    // 组合成完整的目标文件路径
    std::string destPath = combine_folderdir_and_filename(_destFolder, fileName);

    // 打开源文件（二进制模式）
    std::ifstream sourceFile(_sourcePath, std::ios::binary);
    if (!sourceFile) {
        throw std::runtime_error(encoding::sysdcode_to_utf8("无法打开源文件: " + _sourcePath));
    }

    // 确保目标文件夹存在
    try {
        fs::create_directories(_destFolder);
    } catch (const fs::filesystem_error &e) {
        throw std::runtime_error(encoding::sysdcode_to_utf8("无法创建目标文件夹: " + _destFolder + "，错误: " + e.what( )));
    }

    // 打开目标文件（二进制模式，创建新文件并截断已有内容）
    std::ofstream destFile(destPath, std::ios::binary | std::ios::trunc);
    if (!destFile) {
        throw std::runtime_error(encoding::sysdcode_to_utf8("无法创建目标文件: " + destPath));
    }

    // 缓冲区大小（4KB）
    const std::streamsize bufferSize = 4096;
    char                  buffer[bufferSize];

    // 读取并写入文件内容
    while (sourceFile.read(buffer, bufferSize)) {
        destFile.write(buffer, sourceFile.gcount( ));
    }

    // 处理最后一部分数据
    destFile.write(buffer, sourceFile.gcount( ));

    // 检查是否发生错误
    if (!sourceFile.eof( ) || !destFile) {
        throw std::runtime_error(U8C(u8"复制文件过程中发生错误"));
    }

    return true;
}

/*
 * @brief 从一个文件下获取所有符合后缀条件的文件
 * @param _name 返回的文件名（无格式后缀）
 * @param _path 返回的文件路径
 * @param _foldername 目标文件夹名称
 * @param _extension 文件的后缀集合
 * @return 错误返回false，读取成功返回true
 */
bool get_filepath_from_folder(
    list< std::string >       &_name,
    list< std::string >       &_path,
    std::string                _foldername,
    const list< std::string > &_extension) {

    namespace fs = std::filesystem;

    fs::path            foldern = _foldername;    // 目标目录
    list< std::string > fileName;                 // 文件名(包含后缀)

    try {
        for (const auto &entry : fs::recursive_directory_iterator(foldern)) {
            if (fs::is_regular_file(entry.status( ))) {
                fileName.emplace_back(U8C(entry.path( ).filename( ).u8string( ).c_str( )));    // 名字(包含后缀)
                _path.emplace_back(U8C(entry.path( ).u8string( ).c_str( )));                   // 路径
            }
        }
    } catch (const fs::filesystem_error &e) {
        std::cerr << e.what( ) << '\n';
        return false;
    }

    for (auto &aFileName : fileName) {
        // 文件名/后缀
        auto [a, b] = split_filename_and_extension(aFileName);
        if (fuzzy::search(_extension, b, fuzzy::LEVEL::High)) {
            // 匹配才加入
            _name.push_back(a);
        }
    }

    std::cout << U8C(u8"请确认各班（共")
              << _name.size( ) << U8C(u8"个班）：")
              << std::endl;
    for (auto &s : _name) {
        std::cout << s << std::endl;
    }
    pause( );    // 等待用户按回车

    return true;
}

/*
 * @brief 为了cv库,从一个文件下获取所有符合后缀条件的img
 * @param _path 返回的文件路径，cv使用
 * @param _u8name 返回的文件名（无格式后缀）
 * @param _u8path 返回的文件路径
 * @param _foldername 目标文件夹名称
 * @param _extension 文件的后缀集合
 * @return 错误返回false，读取成功返回true
 */
bool get_imgpath_from_folder(
    list< std::string >       &_path,
    list< std::string >       &_u8name,
    list< std::string >       &_u8path,
    std::string                _foldername,
    const list< std::string > &_extension) {

    namespace fs = std::filesystem;

    fs::path            foldern = _foldername;    // 目标目录
    list< std::string > u8fileName;               // 文件名(包含后缀)

    try {
        for (const auto &entry : fs::recursive_directory_iterator(foldern)) {
            if (fs::is_regular_file(entry.status( ))) {
                u8fileName.emplace_back(U8C(entry.path( ).filename( ).u8string( ).c_str( )));    // 名字(包含后缀)
                _path.emplace_back(U8C(entry.path( ).string( ).c_str( )));                       // 路径
                _u8path.emplace_back(U8C(entry.path( ).u8string( ).c_str( )));                   // 路径
            }
        }
    } catch (const fs::filesystem_error &e) {
        std::cerr << e.what( ) << '\n';
        return false;
    }

    for (auto &aFileName : u8fileName) {
        // 文件名/后缀
        auto [a, b] = split_filename_and_extension(aFileName);
        if (fuzzy::search(_extension, b, fuzzy::LEVEL::High)) {
            // 匹配才加入
            _u8name.push_back(a);
        }
    }

    std::cout << U8C(u8"请确认各图片（共")
              << _u8name.size( ) << U8C(u8"张图片）：") << std::endl;
    for (auto &s : _u8name) {
        std::cout << s << std::endl;
    }
    pause( );    // 等待用户按回车

    return true;
}


/*
 * @brief 用于读取表格（utf8编码）
 * @param _sheet 储存表格的二维数组（按照row，column的形式）
 * @param _path 文件的路径
 */
void load_sheet_from_xlsx(table< std::string > &_sheet, std::string _path) {
    xlnt::workbook wb;
    std::cout << U8C(u8"load file: ") << _path << std::endl;
    wb.load(_path);
    auto ws = wb.active_sheet( );    // 获取当前激活的工作表（唯一一张）

    // 按行遍历
    for (auto row : ws.rows(false)) {
        // 保存当前行所有单元格文本的临时向量
        list< std::string > aSingleRow;
        // 遍历当前行的每个单元格
        for (auto cell : row) {
            // cell.to_string() 把数字、日期、公式等统一转为字符串
            aSingleRow.push_back(cell.to_string( ));
        }
        _sheet.push_back(aSingleRow);
    }
}

/*
 * @brief 签到表表格的储存
 * @param _sheet 储存表格的二维数组
 * @param _path 文件的路径
 * @param _titleName 表格标题的名称
 */
void save_attSheet_to_xlsx(
    table< std::string > &_sheet,
    std::string           _path,
    std::string           _titleName) {

    // 定义字体
    xlnt::font f;
    f.name(U8C(u8"仿宋_GB2312"));
    f.size(14);

    // 创建边框样式
    xlnt::border::border_property border_prop;
    border_prop.style(xlnt::border_style::thin);    // 细线边框
    border_prop.color(xlnt::color::black( ));

    // 构造外侧框线
    xlnt::border b;
    b.side(xlnt::border_side::start, border_prop);
    b.side(xlnt::border_side::end, border_prop);
    b.side(xlnt::border_side::top, border_prop);
    b.side(xlnt::border_side::bottom, border_prop);

    // 水平居中 + 垂直居中
    xlnt::alignment align;
    align.horizontal(xlnt::horizontal_alignment::center);
    align.vertical(xlnt::vertical_alignment::center);

    xlnt::workbook wb;
    auto           ws = wb.active_sheet( );
    ws.title("Sheet1");

    size_t maxCol = 1;    // sheet中列的数量
    // 逐行逐列写入
    for (std::size_t r = 0; r < _sheet.size( ); ++r) {
        for (std::size_t c = 0; c < _sheet[r].size( ); ++c) {
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).value(_sheet[r][c]);
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).border(b);
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).font(f);
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).alignment(align);
            // 注意 xlnt 行列从 1 开始
        }
        if (maxCol < _sheet[r].size( )) {
            maxCol = _sheet[r].size( );
        }
    }

    // 列宽
    ws.column_properties(1).width        = 7.92;    // 8字符宽
    ws.column_properties(1).custom_width = true;
    for (int i = 2; i <= maxCol; i++) {
        ws.column_properties(i).width        = 23.92;    // 24字符宽
        ws.column_properties(i).custom_width = true;
    }

    for (std::size_t r = 0; r < _sheet.size( ); ++r) {
        ws.row_properties(r + 1).height        = 24;    // 24pt
        ws.row_properties(r + 1).custom_height = true;
    }

    // 插入表的标题
    xlnt::font f_title;
    f_title.name(U8C(u8"宋体"));
    f_title.size(24);
    ws.insert_rows(1, 1);    // 插入行

    // 合并单元格
    std::string end_col;
    int         temp = maxCol;
    while (temp > 0) {
        int remainder = (temp - 1) % 26;
        end_col       = char('A' + remainder) + end_col;
        temp          = (temp - 1) / 26;
    }
    // 构建合并单元格的范围字符串
    std::string merRange = "A1:" + end_col + "1";
    ws.merge_cells(merRange);

    ws.row_properties(1).height        = 40;    // 40pt
    ws.row_properties(1).custom_height = true;
    ws.cell("A1").value(_titleName);
    ws.cell("A1").font(f_title);
    ws.cell("A1").alignment(align);

    wb.save(_path);
}

/*
 * @brief 考勤表表格的储存
 * @param _sheet 储存表格的二维数组
 * @param _path 文件的路径
 * @param _titleName 表格标题的名称
 */
void save_sttSheet_to_xlsx(
    const table< std::string > &_sheet,
    std::string                &_path,
    std::string                &_titleName) {
    // 定义字体
    xlnt::font fbody;      // 正文字体
    xlnt::font fheader;    // 表头字体
    xlnt::font ftitle;     // 标题字体
    fbody.name(U8C(u8"宋体"));
    fheader.name(U8C(u8"宋体"));
    ftitle.name(U8C(u8"方正小标宋简体"));
    fbody.size(16);
    fheader.size(16);
    ftitle.size(26);
    fheader.bold(true);
    ftitle.bold(true);

    // 创建边框样式
    xlnt::border::border_property border_prop;
    border_prop.style(xlnt::border_style::thin);    // 细线边框
    border_prop.color(xlnt::color::black( ));

    // 构造外侧框线
    xlnt::border b;
    b.side(xlnt::border_side::start, border_prop);
    b.side(xlnt::border_side::end, border_prop);
    b.side(xlnt::border_side::top, border_prop);
    b.side(xlnt::border_side::bottom, border_prop);

    // 水平居中 + 垂直居中
    xlnt::alignment align;
    align.horizontal(xlnt::horizontal_alignment::center);
    align.vertical(xlnt::vertical_alignment::center);

    xlnt::workbook wb;
    auto           ws = wb.active_sheet( );
    ws.title("Sheet1");

    size_t maxCol = 1;    // sheet中列的数量
    // 逐行逐列写入
    for (std::size_t r = 0; r < _sheet.size( ); ++r) {
        for (std::size_t c = 0; c < _sheet[r].size( ); ++c) {
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).value(_sheet[r][c]);
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).border(b);
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).font(fbody);
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).alignment(align);
            // 注意 xlnt 行列从 1 开始
        }
        if (maxCol < _sheet[r].size( )) {
            maxCol = _sheet[r].size( );
        }
    }
    // 修改第一行（表头）的字体
    for (size_t c = 0; c < _sheet[0].size( ); c++) {
        ws.cell(xlnt::cell_reference(c + 1, 1)).border(b);
    }

    // 修改列宽
    for (size_t i = 1; i <= maxCol; i++) {
        ws.column_properties(i).width        = 39.92;    // 40字符宽
        ws.column_properties(i).custom_width = true;
    }
    // 修改行高
    for (size_t r = 1; r <= _sheet.size( ); r++) {
        ws.row_properties(r).height        = 25;    // 25pt
        ws.row_properties(r).custom_height = true;
    }

    // 插入标题
    ws.insert_rows(1, 1);
    // 合并单元格
    std::string end_col;
    int         temp = maxCol;
    while (temp > 0) {
        int remainder = (temp - 1) % 26;
        end_col       = char('A' + remainder) + end_col;
        temp          = (temp - 1) / 26;
    }
    // 构建合并单元格的范围字符串（例如 "A1:D1"）
    std::string merRange = "A1:" + end_col + "1";
    ws.merge_cells(merRange);
    ws.row_properties(1).height        = 45;    // 45pt
    ws.row_properties(1).custom_height = true;
    ws.cell("A1").value(_titleName);
    ws.cell("A1").font(ftitle);
    ws.cell("A1").alignment(align);

    // 保存
    wb.save(_path);
}

/*
 * @brief 保存缓存报名信息到xlsx
 * @param _sheet 表格
 */
void save_storageSheet_to_xlsx(const table< std::string > &_sheet) {
    xlnt::workbook wb;
    auto           ws = wb.active_sheet( );
    ws.title("Sheet1");
    // 逐行逐列写入
    for (std::size_t r = 0; r < _sheet.size( ); ++r)
        for (std::size_t c = 0; c < _sheet[r].size( ); ++c)
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).value(_sheet[r][c]);
    wb.save("./storage/storage.xlsx");
}

/*
 * @brief 加载缓存报名信息到xlsx
 * @param _sheet 表格
 */
void load_storageSheet_from_xlsx(table< std::string > &_sheet) {
    const std::string path = "./storage/storage.xlsx";
    namespace fs           = std::filesystem;
    // 判断此文件是否存在
    if (!fs::exists(path)) {
        std::cout << std::endl
                  << "\033[43;30mWARNING!!!\033[0m" << std::endl;
        std::cout << U8C(u8"\033[43;30m缓存文件 ")
                  << path << U8C(u8" 不存在") << std::endl;
        std::cout << "WARNING!!!\033[0m" << std::endl
                  << std::endl;
        return;
    }
    xlnt::workbook wb;
    wb.load(path);
    std::cout << std::endl
              << U8C(u8"load file: ") << path << std::endl;
    auto ws = wb.active_sheet( );

    // 按行遍历
    for (auto row : ws.rows(false)) {
        // 保存当前行所有单元格文本的临时向量
        list< std::string > aSingleRow;
        // 遍历当前行的每个单元格
        for (auto cell : row) {
            // cell.to_string() 把数字、日期、公式等统一转为字符串
            aSingleRow.push_back(cell.to_string( ));
            // std::cout << cell << "   ";
        }
        _sheet.push_back(aSingleRow);
        // std::cout << std::endl;
    }
}

/*
 * @brief 保存尚未搜索到的成员到xlsx
 * @param _sheet 表格
 */
void save_unknownPerSheet_to_xlsx(table< std::string > &_sheet) {
    xlnt::workbook wb;
    auto           ws = wb.active_sheet( );
    ws.title("Sheet1");
    // 逐行逐列写入
    for (std::size_t r = 0; r < _sheet.size( ); ++r)
        for (std::size_t c = 0; c < _sheet[r].size( ); ++c)
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).value(_sheet[r][c]);
    wb.save("./output/unknown.xlsx");
}

/*
 * @brief 保存青字班报名表
 * @param _sheet 表格
 */
void save_registrationSheet_to_xlsx(const table< std::string > &_sheet) {

    xlnt::workbook wb;
    auto           ws = wb.active_sheet( );
    ws.title("Sheet1");
    // 逐行逐列写入
    for (std::size_t r = 0; r < _sheet.size( ); ++r)
        for (std::size_t c = 0; c < _sheet[r].size( ); ++c)
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).value(_sheet[r][c]);
    wb.save(U8C(u8"./output/sign_for_QingziClass_out/报名.xlsx"));
}

// 替换字符串中的所有指定字符
std::string replace_all(const std::string &_str, char _oldChar, char _newChar) {
    std::string result = _str;
    std::replace(result.begin( ), result.end( ), _oldChar, _newChar);
    return result;
}

// 分割路径为各个组件，同时支持正反斜杠
std::vector< std::string > split_path(const std::string &_path) {
    std::vector< std::string > components;
    std::string                processedPath = _path;

#ifdef _WIN32
    // 在Windows系统下，将所有正斜杠转换为反斜杠统一处理
    processedPath = replace_all(processedPath, '/', '\\');
#endif

    std::stringstream ss(processedPath);
    std::string       component;
    char              delimiter = '/';

#ifdef _WIN32
    delimiter = '\\';    // Windows使用反斜杠作为分隔符
#endif

    while (std::getline(ss, component, delimiter)) {
        if (!component.empty( )) {
            components.push_back(component);
        }
    }
    return components;
}

// 检查文件夹是否存在
bool is_folder_exists(const std::string &_path) {
    struct stat info;
    if (stat(_path.c_str( ), &info) != 0) {
        return false;
    }
    return (info.st_mode & S_IFDIR) != 0;
}

// 检查文件夹是否为空
bool is_folder_empty(const std::string &folder_path) {
    namespace fs = std::filesystem;
    // 检查路径是否存在且是文件夹
    if (!fs::exists(folder_path) || !fs::is_directory(folder_path)) {
        std::cerr << U8C(u8"路径不存在或不是文件夹: ") << folder_path << std::endl;
        return false;    // 或根据需求抛出异常
    }

    // 遍历文件夹，只要有一个条目（文件或子文件夹）就不为空
    auto it = fs::directory_iterator(folder_path);
    return it == fs::directory_iterator( );    // 如果迭代器为空，则文件夹为空
}

// 检查文件是否存在（仅检测普通文件）
bool is_file_exists(const std::string &_path) {
    struct stat info;
    if (stat(_path.c_str( ), &info) != 0) {
        return false;    // 路径不存在
    }
    return (info.st_mode & S_IFREG) != 0;    // 检查是否为普通文件
}

/**
 * 删除指定路径的文件
 * @param file_path 要删除的文件路径
 * @return 成功返回 true，失败返回 false
 */
bool delete_file(const std::string &file_path) {
#ifdef _WIN32
    // Windows 系统使用 DeleteFileA 函数
    // 注意：如果文件正在被使用，会删除失败
    return DeleteFileA(file_path.c_str( )) != 0;
#else
    // Linux/macOS 等 POSIX 兼容系统使用 remove 函数
    return std::remove(file_path.c_str( )) == 0;
#endif
}

// 递归创建文件夹，支持Windows正反斜杠
bool create_folder_recursive(const std::string &_path) {
    // 如果路径已存在，直接返回成功
    if (is_folder_exists(_path)) {
        std::cout << U8C(u8"文件夹已存在: ") << _path << std::endl;
        return true;
    }

    // 分割路径为各个组件
    std::vector< std::string > components = split_path(_path);
    if (components.empty( )) {
        std::cerr << U8C(u8"无效的路径: ") << _path << std::endl;
        return false;
    }

    // 逐级构建路径并创建
    std::string currentPath;
#ifdef _WIN32
    // 处理Windows下的盘符（如C:）
    if (_path.find(':') != std::string::npos) {
        currentPath = components[0] + ":";
        components.erase(components.begin( ));
    }
#endif

    for (const std::string &component : components) {
        if (!currentPath.empty( )) {
#ifdef _WIN32
            currentPath += "\\";    // Windows使用反斜杠
#else
            currentPath += "/";    // Linux使用正斜杠
#endif
        }
        currentPath += component;

        // 如果当前路径不存在，则创建
        if (!is_folder_exists(currentPath)) {
            if (mkdir(currentPath.c_str( )) != 0) {
                std::cerr << U8C(u8"创建目录失败: ") << currentPath << std::endl;
                return false;
            }
            std::cout << U8C(u8"创建目录成功: ") << currentPath << std::endl;
        }
    }

    return true;
}


#ifdef _WIN32
/**
 * @brief 检查文件是否被其他程序占用（Windows 平台，支持 std::string 路径）
 * @param file_path 目标文件的路径（如 "D:/test.txt"，支持相对/绝对路径）,系统编码
 * @return true：文件被占用；false：文件未被占用或其他错误（如文件不存在）
 */
bool is_file_inuse(const std::string &file_path) {
    // 关键：使用 CreateFileA（ANSI 版本），直接接收 std::string 的 c_str()
    HANDLE hFile = CreateFileA(
        file_path.c_str( ),       // 1. 目标文件路径（ANSI 字符串）
        GENERIC_READ,             // 2. 访问权限：只读（避免误修改文件）
        0,                        // 3. 共享模式：0 = 禁止其他进程共享（核心参数）
                                  //    若文件已被占用，此参数会导致打开失败
        nullptr,                  // 4. 安全属性：默认（nullptr）
        OPEN_EXISTING,            // 5. 创建方式：仅打开已存在的文件（避免创建新文件）
        FILE_ATTRIBUTE_NORMAL,    // 6. 文件属性：普通文件
        nullptr                   // 7. 模板文件：无（nullptr）
    );

    // 1. 判断文件是否打开成功（无效句柄表示打开失败）
    if (hFile == INVALID_HANDLE_VALUE) {
        DWORD error_code = GetLastError( );    // 获取具体错误码

        // 2. 仅当错误码为 "共享冲突" 或 "锁定冲突" 时，判定为文件被占用
        if (error_code == ERROR_SHARING_VIOLATION ||    // 错误码 32：文件被其他进程独占
            error_code == ERROR_LOCK_VIOLATION) {       // 错误码 33：文件被其他进程锁定
            std::cout << U8C(u8"文件被占用：") << encoding::sysdcode_to_utf8(file_path) << std::endl;
            return true;
        }
        // 3. 其他错误（如文件不存在、权限不足等），判定为 "未被占用"
        else {
            // 可选：打印错误详情（便于调试）
            std::cout << U8C(u8"文件未被占用（或其他错误）：") << encoding::sysdcode_to_utf8(file_path)
                      << U8C(u8"，错误码：") << error_code << std::endl;
            return false;
        }
    }

    // 4. 若打开成功（文件未被占用），需关闭句柄避免内存泄漏
    CloseHandle(hFile);
    // std::cout << U8C(u8"文件未被占用：") << encoding::sysdcode_to_utf8(file_path) << std::endl;
    return false;
}
#else
#include <fcntl.h>     // 包含 open()、O_RDONLY 等宏
#include <unistd.h>    // 包含 close()
#include <errno.h>     // 包含 errno、EBUSY 等错误码

// 检查文件是否被占用（Linux/macOS 平台）
bool is_file_inuse(const std::string &file_path) {
    // 尝试以 "只读 + 非阻塞" 模式打开文件
    // 关键参数：
    // - O_RDONLY：只读权限
    // - O_NONBLOCK：非阻塞模式（避免因文件锁定导致进程阻塞）
    int fd = open(file_path.c_str( ), O_RDONLY | O_NONBLOCK);

    // 判断是否打开成功
    if (fd == -1) {
        // 检查是否为 "文件被占用" 错误（EBUSY）
        if (errno == EBUSY) {
            std::cout << U8C(u8"文件被占用：") << encoding::sysdcode_to_utf8(file_path) << std::endl;
            return true;
        } else {
            // 其他错误（如文件不存在、权限不足等）
            std::cout << "文件未被占用（或其他错误），错误码：" << errno << std::endl;
            return false;
        }
    }

    // 若打开成功，关闭文件描述符避免泄漏
    close(fd);
    // std::cout << "文件未被占用：" << file_path << std::endl;
    return false;
}
#endif                 // _WIN32


}    // namespace file
