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
 **********************设计思路*******************************************/


#include <ppocr_API.h>
#include <string>
#include <utility>
#include <vector>
#include "ChineseEncoding.h"

extern ppocr::DefDirs _ppocrDir_;

// 定义二维点类型，first为x坐标，second为y坐标
using GridPoint = std::pair< double, double >;

// 派生自 OCRPredictResult
struct DefSolveOCRResult : public ppocr::OCRPredictResult {
public:
    GridPoint corePoint;    // box的中心点坐标
    // size_t    row;          // 行数，第i行

    // 基于 OCRPredictResult 构造
    DefSolveOCRResult(ppocr::OCRPredictResult _ocrPR) {
        this->box              = _ocrPR.box;
        this->cls_label        = _ocrPR.cls_label;
        this->cls_score        = _ocrPR.cls_score;
        this->score            = _ocrPR.score;
        this->text             = anycode_to_utf8(_ocrPR.text);
        this->corePoint.first  = (_ocrPR.box[0][0] + _ocrPR.box[1][0] + _ocrPR.box[2][0] + _ocrPR.box[3][0]) / 4.0;
        this->corePoint.second = (_ocrPR.box[0][1] + _ocrPR.box[1][1] + _ocrPR.box[2][1] + _ocrPR.box[3][1]) / 4.0;
        // row                    = 0;    // 默认为第0行
    }
};

// 定义网格结果结构体，包含行坐标、列坐标和所有交点
struct GridResult {
    std::vector< double >    row_coords;       // 行坐标（横线的y值）
    std::vector< double >    col_coords;       // 列坐标（竖线的x值）
    std::vector< GridPoint > intersections;    // 所有网格交点
};

/*
 * @brief 用于读取图片的表格（utf8编码）
 * @param 储存表格的二维数组（按照row，column的形式）
 * @param 文件的路径
 */
void load_sheet_from_img(
    std::vector< std::vector< std::string > > &_aSheet,
    std::string                                _pathAndName);

/**
 * @brief 对排序后的坐标值进行聚类（将相近的值归为一组）
 *
 * @param sorted_vals 已排序的坐标值集合（可以是x坐标或y坐标）
 * @param epsilon 聚类阈值，小于此值的坐标被视为同一组（同一网格线）
 * @return 每个聚类组的平均值，代表该组对应的网格线坐标
 */
std::vector< double > cluster_coords(const std::vector< double > &sorted_vals, double epsilon);

/**
 * @brief 从排序后的网格线坐标中检测基础间距（适用于等间距网格）
 *
 * @param sorted_coords 已排序的网格线坐标集合
 * @param epsilon 误差阈值，用于判断两个差值是否属于同一间距
 * @return 检测到的基础间距，若无法检测则返回0.0
 */
double detect_spacing(const std::vector< double > &sorted_coords, double epsilon);

/**
 * @brief 根据基础间距和范围生成完整的网格线集合
 *
 * @param initial_grid 初步聚类得到的网格线坐标
 * @param spacing 检测到的基础间距
 * @param epsilon 误差阈值，用于处理浮点数精度问题
 * @param min_valid_spacing 生成网格线时过滤过小间距
 * @return 完整的网格线坐标集合
 */
std::vector< double > generate_complete_grid(
    const std::vector< double > &initial_grid,
    double spacing, double epsilon, double min_valid_spacing);

/**
 * @brief 从稀疏的坐标点中提取完整的网格信息（包含行坐标、列坐标和交点）
 *
 * @param points 输入的稀疏坐标点集合
 * @param epsilon_x x方向的误差阈值
 * @param epsilon_y y方向的误差阈值
 * @param min_valid_spacing_x 提取网格信息时传入最小有效间距参数x
 * @param min_valid_spacing_y 提取网格信息时传入最小有效间距参数y
 * @return GridResult 包含行坐标、列坐标和所有交点的结构体
 */
GridResult extract_grid_info(
    const std::vector< GridPoint > &points,
    double epsilon_x, double epsilon_y,
    double min_valid_spacing_x, double min_valid_spacing_y);

/**
 * @brief 新增函数：根据点列密度自动计算x和y方向的误差阈值
 *
 * @param points 输入的坐标点集合
 * @param x0 区域x方向最大值（点的x坐标≤x0）
 * @param y0 区域y方向最大值（点的y坐标≤y0）
 * @param ratio 阈值与中位数距离的比例（建议0.2~0.5，值越小阈值越严格）
 * @param min_epsilon_ratio 最小阈值区域的比例
 * @return 包含epsilon_x和epsilon_y的 pair （first为x方向阈值，second为y方向阈值）
 */
std::pair< double, double > auto_calculate_epsilon(
    const std::vector< GridPoint > &points,
    double x0, double y0,
    double ratio             = 0.5,
    double min_epsilon_ratio = 0.01);


#endif    // !IMGS_H
