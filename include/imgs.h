#pragma once
#ifndef IMGS_H
#define IMGS_H


/**********************设计思路*******************************************
 *@brief 将ocr的结果转化成sheet
 *
 *
 * @
 * @attention
 * @attention test.h中有此文件的测试函数
 * @attention
 * @
 *
 * @todo [@刘思成] 可不可以封装一下
 *
 * @note  转换函数都是由 2561925435@qq.com 独自编写，若出现错误，还请修改
 * (+^-^+)
 *
 * 作者：   刘思成
 * 邮箱：   2561925435@qq.com
 *
 **********************设计思路****************************************** */


#include <ppocr_API.h>
#include <string>
#include <utility>
#include <vector>

extern ppocr::DefDirs _ppocrDir_;


/*
 * @brief 用于读取图片的表格（utf8编码）
 * @param 储存表格的二维数组（按照row，column的形式）
 * @param 文件的路径
 */
void load_sheet_from_img(
    std::vector< std::vector< std::string > > &_aSheet,
    std::string                                _pathAndName);



#endif    // !IMGS_H
