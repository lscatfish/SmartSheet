#pragma once

/*
 * @brief 直接操作xlsx的文件
 */

#ifndef FILES_H
#define FILES_H

/*
 * 此文件用于操作文件
 */
#include <string>
#include <utility>
#include <vector>


namespace file {

/*
 * @brief 解析文件名字的后缀与文件名字（不含后缀）
 * @param _input 输入的文件名
 * @return 文件名字（不含后缀）与 后缀 的pair
 */
std::pair< std::string, std::string > separate_filename_and_extension(const std::string &_input);

/*
 * @brief 从一个文件下获取所有符合后缀条件的文件
 * @param _name 返回的文件名（无格式后缀）
 * @param _path 返回的文件路径
 * @param _foldername 目标文件夹名称
 * @param _extension 文件的后缀集合
 * @return 错误返回false，读取成功返回true
 */
bool get_filepath_from_folder(
    std::vector< std::string >       &_name,
    std::vector< std::string >       &_path,
    std::string                       _foldername,
    const std::vector< std::string > &_extension);

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
    std::vector< std::string >       &_path,
    std::vector< std::string >       &_u8name,
    std::vector< std::string >       &_u8path,
    std::string                       _foldername,
    const std::vector< std::string > &_extension);

/*
 * @brief 用于读取表格（utf8编码）
 * @param _sheet 储存表格的二维数组（按照row，column的形式）
 * @param _path 文件的路径
 */
void load_sheet_from_xlsx(std::vector< std::vector< std::string > > &_sheet, std::string _path);

/*
 * @brief 签到表表格的储存
 * @param _sheet 储存表格的二维数组
 * @param _path 文件的路径
 * @param _titleName 表格标题的名称
 */
void save_attSheet_to_xlsx(
    std::vector< std::vector< std::string > > &_sheet,
    std::string                                _path,
    std::string                                _titleName);

/*
 * @brief 考勤表表格的储存
 * @param _sheet 储存表格的二维数组
 * @param _path 文件的路径
 * @param _titleName 表格标题的名称
 */
void save_sttSheet_to_xlsx(
    const std::vector< std::vector< std::string > > &_sheet,
    std::string                                     &_path,
    std::string                                     &_titleName);

/*
 * @brief 保存报名信息到xlsx
 * @param _sheet 表格
 */
void save_signSheet_to_xlsx(const std::vector< std::vector< std::string > > &_sheet);

/*
 * @brief 加载缓存报名信息到xlsx
 * @param _sheet 表格
 */
void load_signSheet_from_xlsx(std::vector< std::vector< std::string > > &_sheet);

/*
 * @brief 保存尚未搜索到的成员到xlsx
 * @param _sheet 表格
 */
void save_unknownPerSheet_to_xlsx(std::vector< std::vector< std::string > > &_sheet);
}    // namespace file

#endif    // !FILES_H
