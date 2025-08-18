#pragma once

/*
 * @brief 直接操作xlsx的文件
 */

#ifndef FILES_H
#define FILES_H

/*
 * 此文件用于操作文件
 */
#include <basic.hpp>
#include <Encoding.h>
#include <string>
#include <utility>
#include <vector>
#include <Windows.h>

namespace file {

// 此类用于获取文件夹中的所有文件
class DefFolder {
public:
    /*
     * @brief 标准构造
     * @param _folderDir 文件夹的地址dir（请按照工作电脑编码）
     */
    DefFolder(std::string _folderDir) {
        traverse_folder(_folderDir, this->filePathList_);
        // 输出u8的文件夹地址，用于在控制台输出
        for (const auto &fP : this->filePathList_) {
            u8filePathList_.push_back(encoding::chcode_to_utf8(fP));
        }
    };
    ~DefFolder( ) = default;

    /*
     * @brief 递归遍历文件夹，收集所有文件路径到 vector 中
     * @param folderPath 文件夹的地址dir(文件夹的名称)
     * @param filePaths 文件夹下的文件路径（按照此电脑编码）
     */
    static void traverse_folder(const std::string &folderPath, list< std::string > &filePaths);

    /*
     * @brief 输出文件夹下的各个文件路径(utf8编码)
     * @return list<string>类型一个列表
     */
    list< std::string > get_filePath_list( );

    /*
     * @brief 输出文件夹下的各个文件路径
     * @return list<string>类型一个列表
     */
    list< std::string > get_u8filePath_list( );

    /*
     * @brief 输出指定后缀的文件路径
     * @param _extension 指定的后缀
     * @return 输出指定后缀的文件路径
     */
    list< std::string > get_filePath_list(const list< std::string > &_extension);

    /*
     * @brief 输出指定后缀的文件路径(u8编码)
     * @param _extension 指定的后缀
     * @return 输出指定后缀的文件路径（u8编码）
     */
    list< std::string > get_u8filePath_list(const list< std::string > &_extension);

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

private:
    std::string         folderDir_;         // 文件夹的地址dir(文件夹的名称)
    list< std::string > filePathList_;      // 文件夹下的文件路径（按照此电脑编码）
    list< std::string > u8filePathList_;    // 文件夹下的文件路径（按照utf8编码）
};


/*
 * @brief 解析文件名字的后缀与文件名字（不含后缀）
 * @param _input 输入的文件名
 * @return 文件名字（不含后缀）与 后缀 的pair
 */
std::pair< std::string, std::string > split_filename_and_extension(const std::string &_input);

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
}    // namespace file

#endif    // !FILES_H
