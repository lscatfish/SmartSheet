#pragma once
#ifndef IMGS_H
#define IMGS_H


/**********************设计思路*******************************************
 *
 *@brief 将ocr的结果转化成sheet
 *
 * 图片表格解析功能实现
 * 整体思路：
 * 1. 图片加载：读取指定路径的图片文件到内存
 * 2. 表格网格线提取：
 *    - 将图片转为灰度图后进行二值化处理（反相使线条为白色）
 *    - 分别提取水平和垂直网格线（通过形态学腐蚀和膨胀操作突出线条）
 *    - 从提取的线条中获取有效坐标（过滤短线条和边缘线条，去重相近线条）
 * 3. 表格结构构建：
 *    - 根据网格线坐标生成单元格(CELL)表格(SHEET)，每个单元格由四个顶点坐标定义
 *    - 计算每个单元格的中心点用于后续内容匹配
 * 4. OCR文字识别：
 *    - 对图片进行OCR识别，获取文字内容及对应的位置信息（四边形边界框）
 *    - 将OCR结果转换为带坐标的CELL对象
 * 5. 文字填充到表格：
 *    - 判断OCR识别出的文字块属于哪个单元格（通过中心点是否包含在单元格内）
 *    - 将文字内容填充到对应的单元格中
 * 6. 结果输出：返回由文字组成的二维表格数据
 *
 *
 * @note  转换函数都是由 2561925435@qq.com 独自编写，若出现错误，还请修改
 * (+^-^+)
 *
 * 作者：   刘思成
 * 邮箱：   2561925435@qq.com
 *
 **********************设计思路******************************************/


#include <ppocr_API.h>
#include <string>
#include <utility>
#include <vector>

// 操作照片的空间
namespace img {

extern ppocr::DefDirs _ppocrDir_;

/*
 * @brief 用于读取图片的表格（utf8编码）
 * @param 储存表格的二维数组（按照row，column的形式）
 * @param 文件的路径
 */
void load_sheet_from_img(
    std::vector< std::vector< std::string > > &_aSheet,
    std::string                                _pathAndName);

}    // namespace img

#endif    // !IMGS_H
