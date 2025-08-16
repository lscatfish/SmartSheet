#pragma once
#ifndef IMGS_H
#define IMGS_H


/**********************设计思路*******************************************
 *@brief 将ocr的结果转化成sheet
 *
 * @note 1.自ocr结果类OCRPredictResult派生DefSolveOCRResult，用于计算存在文字的中心点坐标
 * @note 2.考虑所有中心点的坐标，可以构成点阵，此点阵看作网格的交点
 * @note 3.那么问题转化成知道部分网格交点求出网格的一个过程
 * @note 4.cluster_coords、detect_spacing、generate_complete_grid、extract_grid_info求网格
 * @note 5.网格交点自然可以求出，网格交点就是一个单元格，里面的内容就是单元格内容（可能为空）
 * @note 6.遍历每个DefSolveOCRResult结果，向周围求出最近的网格交点
 *
 *
 * 一、具体步骤
 * 1. 数据预处理：
 *    - 从输入点中分离x坐标和y坐标
 *    - 对x和y坐标分别排序（为后续聚类做准备）
 *
 * 2. 聚类分析（确定初始网格线）：
 *    - 对排序后的x坐标聚类，将误差范围内的点视为同一竖线
 *    - 对排序后的y坐标聚类，将误差范围内的点视为同一横线
 *    - 每组聚类取平均值作为初始网格线坐标（降低误差影响）
 *
 * 3. 间距检测（发现网格规律）：
 *    - 计算初始网格线间的距离，通过聚类找到最可能的基础间距
 *    - 分别处理x方向（列间距）和y方向（行间距）
 *
 * 4. 完整网格生成：
 *    - 根据初始网格线范围和检测到的间距，填补缺失的网格线
 *    - 生成完整的行坐标（y值）和列坐标（x值）
 *
 * 5. 结果组合：
 *    - 计算行坐标与列坐标的笛卡尔积，得到所有网格交点
 *    - 将行坐标、列坐标和交点坐标封装为统一结果
 *
 * 二、关键技术点
 * 1. 动态聚类：自适应将相近坐标分组，无需预先知道网格数量
 * 2. 间距检测：通过分析差值分布发现网格的潜在规律
 * 3. 误差隔离：x和y方向使用独立误差阈值，适应不同精度数据
 *
 * 三、使用说明
 * 调用extract_grid_info函数，传入：
 * - 输入坐标点集合
 * - x方向误差阈值epsilon_x
 * - y方向误差阈值epsilon_y
 * 返回GridResult结构体，包含完整的网格信息
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
