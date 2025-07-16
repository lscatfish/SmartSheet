#pragma once

#ifndef FILES_H
#define FILES_H

/*
 * 此文件用于操作文件
 */
#include <string>
#include <vector>

/*
 * @brief 从一个文件下获取所有的文件
 * @param 返回的文件名
 * @param 返回的文件路径
 * @param 目标文件夹名称
 * @return 错误返回false，读取成功返回true
 */
bool get_filepath_from_folder(
    std::vector< std::string > &_name,
    std::vector< std::string > &_path,
    std::string                 _foldername);

/*
 * @brief 用于读取表格
 * @param 储存表格的二维数组
 * @param 文件的路径
 */
void get_sheet_from_file(
    std::vector< std::vector< std::string > > &aSheet,
    std::string                                pathAndName);

#endif    // !FILES_H
