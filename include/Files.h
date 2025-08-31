#pragma once

/******************************************************************************
 * @brief 直接操作xlsx的文件
 * @file Files.h
 *
 * 作者：lscatfish、KIMI
 ****************************************************************************/


#ifndef FILES_H
#define FILES_H

/*
 * 此文件用于操作文件
 */
#include <basic.hpp>
#include <cstdlib>
#include <Encoding.h>
#include <filesystem>
#include <Fuzzy.h>
#include <iostream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

// 此空间用于操作系统的文件以及文件夹
namespace file {

extern std::string _INPUT_DIR_;
extern std::string _INPUT_ALL_DIR_;
extern std::string _INPUT_APP_DIR_;
extern std::string _INPUT_ATT_IMGS_DIR_;
extern std::string _INPUT_SIGN_QC_ALL_DIR_;

extern std::string _OUTPUT_DIR_;
extern std::string _OUTPUT_APP_DIR_;
extern std::string _OUTPUT_ATT_DIR_;
extern std::string _OUTPUT_SIGN_QC_DIR_;
extern std::string _OUTPUT_SIGN_QC_UNPDF_DIR_;
extern std::string _OUTPUT_SIGN_QC_CMT_DIR_;

extern std::string _STORAGE_DIR_;

// 此类用于获取文件夹中的 所有 文件
class DefFolder {
public:
    /*
     * @brief 标准构造
     * @param _folderDir 文件夹的地址dir（请按照工作电脑编码）
     * @param ifp 是否打印加载的文件夹和文件
     */
    DefFolder(std::string _folderDir, bool ifp) {
        if (*_folderDir.rbegin( ) == '\\' || *_folderDir.rbegin( ) == '/' && _folderDir.size( ) != 0)
            _folderDir.pop_back( );

        traverse_folder(_folderDir, this->filePathList_);
        // 输出u8的文件夹地址，用于在控制台输出
        for (const auto &fP : this->filePathList_) {
            u8filePathList_.push_back(encoding::sysdcode_to_utf8(fP));
        }
        if (ifp)
            std::cout << U8C(u8"已加载文件夹：") << encoding::sysdcode_to_utf8(_folderDir) << std::endl;
        folderDir_ = _folderDir;
    };

    // @brief 按照DefFolder变量来构造
    DefFolder(const DefFolder &other)
        : folderDir_(other.folderDir_),
          filePathList_(other.filePathList_),
          u8filePathList_(other.u8filePathList_) {};

    /*
     * @brief 选择一个DefFolder，按照一定的后缀来选择构造
     * @param _other 另一个DefFolder
     * @param _extension 指定的后缀
     */
    DefFolder(const DefFolder &_other, const list< std::string > &_extension)
        : folderDir_(_other.folderDir_) {
        filePathList_   = _other.get_filepath_list(_extension);
        u8filePathList_ = _other.get_u8filepath_list(_extension);
    };

    ~DefFolder( ) = default;

    /*
     * @brief 递归遍历文件夹，收集所有文件路径到 vector 中
     * @param folderPath 文件夹的地址dir(文件夹的名称)
     * @param filePaths 文件夹下的文件路径（按照此电脑编码）
     */
    static void traverse_folder(const std::string &folderPath, list< std::string > &filePaths);

    /*
     * @brief 输出文件夹下的各个文件的相对路径
     * @return list<string>类型一个列表
     */
    list< std::string > get_filepath_list( ) const;

    /*
     * @brief 输出文件夹下的各个文件的相对路径(utf8编码)
     * @return list<string>类型一个列表
     */
    list< std::string > get_u8filepath_list( ) const;

    /*
     * @brief 输出指定后缀的文件路径
     * @param _extension 指定的后缀
     * @return 输出指定后缀的文件路径
     */
    list< std::string > get_filepath_list(const list< std::string > &_extension) const;

    /*
     * @brief 输出指定后缀的文件路径(u8编码)
     * @param _extension 指定的后缀
     * @return 输出指定后缀的文件路径（u8编码）
     */
    list< std::string > get_u8filepath_list(const list< std::string > &_extension) const;

    /*
     * @brief 保留指定后缀的文件
     * @param _extension 指定的后缀
     * @return 剩余文件的数量
     */
    size_t keep_with(const list< std::string > &_extension);

    /*
     * @brief 擦除指定的后缀
     * @param _extension 指定的后缀
     * @return 剩余文件的数量
     */
    size_t erase_with(const list< std::string > &_extension);

    /*
     * @brief 擦除指定文件名的文件
     * @param _path 指定的文件路径
     * @return 是否成功
     */
    bool erase_with(const std::string _path);

    /*
     * @brief 删除指定文件名的文件
     * @param _path 指定的文件路径
     * @return 是否成功
     */
    bool delete_with(const std::string _path);

    /*
     * @brief 删除指定后缀的文件
     * @param _extension 指定的后缀
     * @return 删除的文件数量
     */
    size_t delete_with(const list< std::string > _extension);

    /*
     * @brief 删除所有文件
     * @return 删除的文件数量
     */
    size_t delete_with( );

    /*
     * @brief 复制指定后缀的文件到指定的路径
     * @param _targetDir 指定路径
     * @param _extension 指定的后缀
     * @return 复制到的文件的数量
     */
    size_t copy_files_to(const std::string &_targetDir, const list< std::string > &_extension) const;

    /*
     * @brief 复制指定的文件到指定的路径
     * @param _targetDir 指定路径
     * @param _filePath 指定的文件路径
     * @return 是否成功
     */
    bool copy_files_to(const std::string &_targetDir, const std::string &_filePath) const;

    /*
     * @brief 复制文件到指定的路径
     * @param _targetDir 指定路径
     * @return 复制到的文件的数量
     */
    size_t copy_files_to(const std::string &_targetDir) const;

    /*
     * @brief 返回所有的文件名（包含后缀）
     * @param _extension 指定的后缀
     * @return 返回的文件（包含后缀）
     */
    list< std::string > get_file_list( ) const;

    /*
     * @brief 返回特定后缀的文件名（包含后缀）
     * @param _extension 指定的后缀
     * @return 返回的文件（包含后缀）
     */
    list< std::string > get_file_list(const list< std::string > &_extension) const;

    /*
     * @brief 返回所有的文件名（包含后缀）
     * @param _extension 指定的后缀
     * @return 返回的文件（包含后缀）
     */
    list< std::string > get_u8file_list( ) const;

    /*
     * @brief 返回特定后缀的文件名（包含后缀）
     * @param _extension 指定的后缀
     * @return 返回的文件（包含后缀）
     */
    list< std::string > get_u8file_list(const list< std::string > &_extension) const;

    /*
     * @brief 返回所有的文件名（不包含后缀）
     * @param _extension 指定的后缀
     * @return 返回的文件名（不包含后缀）
     */
    list< std::string > get_filename_list( ) const;

    /*
     * @brief 返回特定后缀的文件名（不包含后缀）
     * @param _extension 指定的后缀
     * @return 返回的文件名（不包含后缀）
     */
    list< std::string > get_filename_list(const list< std::string > &_extension) const;

    /*
     * @brief 返回所有的文件名（不包含后缀）
     * @param _extension 指定的后缀
     * @return 返回的文件名（不包含后缀）
     */
    list< std::string > get_u8filename_list( ) const;

    /*
     * @brief 返回特定后缀的文件名（不包含后缀）
     * @param _extension 指定的后缀
     * @return 返回的文件名（不包含后缀）
     */
    list< std::string > get_u8filename_list(const list< std::string > &_extension) const;

    /*
     * @brief 检测此文件夹下是否有有被占用的文件
     * @param ifp 是否打印被占用的文件
     * @param progressBar 是否打开进度条，启用之后ifp不可用
     * @return _occu8PathList 输出被占用的文件路径(utf8)
     */
    list< std::string > check_occupied_utf8(bool ifp = false, bool progressBar = false) const;

    /*
     * @brief 检测此文件夹下是否有有被占用的文件
     * @param ifp 是否打印被占用的文件
     * @param progressBar 是否打开进度条，启用之后ifp不可用
     * @return 输出被占用的文件路径(sys)
     */
    list< std::string > check_occupied_sys(bool ifp = false, bool progressBar = false) const;

    /*
     * @brief 检查文件路径是否在此文件夹存在
     * @param _path 指定的文件路径
     * @return 是否存在
     */
    bool is_filepath_exist(const std::string &_path) const;

private:
    std::string         folderDir_;         // 文件夹的地址dir(文件夹的名称)
    list< std::string > filePathList_;      // 文件夹下的文件路径（按照此电脑编码）
    list< std::string > u8filePathList_;    // 文件夹下的文件路径（按照utf8编码）
};
/* ========================================================================================================================= */


/*
 * @brief 解析文件名字的后缀与文件名字（不含后缀）
 * @param _input 输入的文件名
 * @return 文件名字（不含后缀）与 后缀 的pair
 */
std::pair< std::string, std::string > split_filename_and_extension(const std::string &_input);

/*
 * @brief 从路径中提取文件名（包含后缀）
 * @param _path 输入的文件路径
 */
std::string split_file_from_path(const std::string &_path);

/*
 * @brief 组合文件夹路径和文件名，处理路径分隔符
 * @param _folder 文件夹路径
 * @param _filename 文件名字（带后缀）
 */
std::string combine_folderdir_and_filename(const std::string &_folder, const std::string &_filename);

/*
 * @brief 复制文件函数，目标为文件夹路径
 * @param _sourcePath 要复制的文件路径
 * @param _destFolder 要复制到的文件夹路径
 */
bool copy_file_to_folder(const std::string &_sourcePath, const std::string &_destFolder);

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
    const list< std::string > &_extension);

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
    const list< std::string > &_extension);

/*
 * @brief 用于读取表格（utf8编码）
 * @param _sheet 储存表格的二维数组（按照row，column的形式）
 * @param _path 文件的路径
 */
void load_sheet_from_xlsx(table< std::string > &_sheet, std::string _path);

/*
 * @brief 签到表表格的储存
 * @param _sheet 储存表格的二维数组
 * @param _path 文件的路径
 * @param _titleName 表格标题的名称
 */
void save_attSheet_to_xlsx(
    table< std::string > &_sheet,
    std::string           _path,
    std::string           _titleName);

/*
 * @brief 考勤表表格的储存
 * @param _sheet 储存表格的二维数组
 * @param _path 文件的路径
 * @param _titleName 表格标题的名称
 */
void save_sttSheet_to_xlsx(
    const table< std::string > &_sheet,
    std::string                &_path,
    std::string                &_titleName);

/*
 * @brief 保存报名信息到xlsx
 * @param _sheet 表格
 */
void save_storageSheet_to_xlsx(const table< std::string > &_sheet);

/*
 * @brief 加载缓存报名信息到xlsx
 * @param _sheet 表格
 */
void load_storageSheet_from_xlsx(table< std::string > &_sheet);

/*
 * @brief 保存尚未搜索到的成员到xlsx
 * @param _sheet 表格
 */
void save_unknownPerSheet_to_xlsx(table< std::string > &_sheet);

/*
 * @brief 保存青字班报名表
 * @param _sheet 表格
 */
void save_registrationSheet_to_xlsx(const table< std::string > &_sheet);

/**
 * 替换字符串中的所有指定字符
 *
 * 遍历输入字符串，将所有与oldChar匹配的字符替换为newChar
 * 不修改原字符串，返回替换后的新字符串副本
 *
 * @param str 要处理的原始字符串
 * @param oldChar 需要被替换的字符
 * @param newChar 用于替换的新字符
 * @return 替换完成后的字符串副本
 */
std::string replace_all(const std::string &str, char oldChar, char newChar);

// 分割路径为各个组件，同时支持正反斜杠
std::vector< std::string > split_path(const std::string &_path);

/**
 * 检查文件夹是否存在
 *
 * 使用系统相关函数检查指定路径是否存在且为一个目录
 * 支持绝对路径和相对路径，跨平台兼容Windows和类Unix系统
 *
 * @param _path 要检查的文件夹路径
 * @return 如果路径存在且是文件夹则返回true，否则返回false
 */
bool is_folder_exists(const std::string &_path);

// 检查文件夹是否为空
bool is_folder_empty(const std::string &folder_path);

// 检查文件是否存在（仅检测普通文件）
bool is_file_exists(const std::string &_path);

/**
 * 删除指定路径的文件
 * @param file_path 要删除的文件路径
 * @return 成功返回 true，失败返回 false
 */
bool delete_file(const std::string &file_path);

/**
 * 递归创建文件夹，支持Windows正反斜杠
 *
 * 解析输入路径，逐级检查并创建所有不存在的目录层级
 * 自动处理Windows系统下的正反斜杠混合使用情况，以及带盘符的路径
 * 类Unix系统下保持使用正斜杠作为路径分隔符
 *
 * @param path 要创建的文件夹路径（支持绝对路径、相对路径、嵌套路径）
 * @return 如果全部目录创建成功或路径已存在则返回true，创建失败则返回false
 */
bool create_folder_recursive(const std::string &path);

/**
 * @brief 检查文件是否被其他程序占用（支持 std::string 路径）
 * @param file_path 目标文件的路径（如 "D:/test.txt"，支持相对/绝对路径）,系统编码
 * @return true：文件被占用；false：文件未被占用或其他错误（如文件不存在）
 */
bool is_file_inuse(const std::string &file_path);

}    // namespace file

#endif    // !FILES_H
