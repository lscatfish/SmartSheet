#pragma once
#ifndef IMGS_H
#define IMGS_H

#include <basic.hpp>
#include <opencv2/opencv.hpp>
#include <ppocr_API.h>
#include <string>
#include <utility>
#include <vector>

// 操作照片的空间
namespace img {

// 定义由CELL生成的表格
using SHEET = std::vector< std::vector< CELL > >;

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
 * 作者：   lscatfish
 * 邮箱：   2561925435@qq.com
 *
 * v0.8.0 开放接口GridResult
 *
 **********************设计思路******************************************/

// 表格网格线的解析结果
class GridResult {
public:
    /*
     * @brief 输入图片，解析网格
     * @param _img 输入的图片
     */
    GridResult(const cv::Mat &img) {
        // 转为灰度图
        cv::Mat gray;
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

        // 二值化（反相，让线条为白色）
        cv::Mat thresh;
        cv::threshold(gray, thresh, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

        // 提取水平和垂直网格线
        cv::Mat horizontal = extract_horizontal_lines(thresh);
        cv::Mat vertical   = extract_vertical_lines(thresh);

        // 获取网格线坐标
        // 获取网格线坐标（设置边缘阈值为10像素，可根据实际情况调整）
        this->horizontalYs_ = get_horizontal_lineYs(horizontal, int(img.rows * 0.01));    // 水平线的y坐标
        this->verticalXs_   = get_vertical_lineXs(vertical, int(img.cols * 0.01));        // 竖线的x坐标

        // 将网格线解析到表格
        parse_grid_to_sheet( );
    };
    ~GridResult( ) = default;

    // 提取水平网格线
    static cv::Mat extract_horizontal_lines(cv::Mat &src);

    // 提取垂直网格线
    static cv::Mat extract_vertical_lines(cv::Mat &src);

    // 提取水平网格线的y坐标（带长度和边缘过滤）
    static std::vector< int > get_horizontal_lineYs(const cv::Mat &horizontalLines, int edgeThreshold = 10);

    // 提取垂直网格线的x坐标（带长度和边缘过滤）
    static std::vector< int > get_vertical_lineXs(const cv::Mat &verticalLines, int edgeThreshold = 10);

    // 填充sheet
    // 可以考虑放在构造函数里面自动解析
    void fill_sheet(const std::vector< CELL > &inCellLists);

    // 返回空列的索引(降序)
    std::vector< size_t > get_space_column_indices( );

    // 返回空行的索引（降序）
    std::vector< size_t > get_space_row_indices( );

    // 删除空行空列
    void remove_space_row_col( );

    // 返回构成的sheet
    SHEET get_sheet( ) const;

    // 返回只有string的表格
    table< std::string > get_stringSheet( ) const;

private:
    std::vector< int > horizontalYs_;    // 水平线的y坐标
    std::vector< int > verticalXs_;      // 竖线的x坐标
    SHEET              sheet_;           // 解析到的表格

    // 将网格线解析到表格
    void parse_grid_to_sheet( );
};

/* =============================================================================================================== */
/* =============================================================================================================== */
/* =============================================================================================================== */

/******************************************设计思路**********************************************
 * 文档扫描的思路
 *
 * 关键模块：
 * - 图片预处理
 * - 文档提取（最大四边形算法）(霍夫变换)（curve曲线边框）（反向演算）
 * - 透视矫正
 * - 图像增强
 *
 *
 *
 *******************************************设计思路**********************************************/


// 文档扫描（包含增强）
// @note 此类启用了opencv的后台并行加速，为此类进行多线程加速请注意线程安全
class DocumentScanner {
public:
    DocumentScanner(const cv::Mat &_inImg) {
        preprocess_= preprocess(_inImg);
    }
    ~DocumentScanner( ) = default;

    // 照片预处理
    // @return 返回一个二值化的边缘图
    cv::Mat preprocess(const cv::Mat &_img);

    // 获取经过校正之后的图片
    cv::Mat get_scanner_img( );

private:
    cv::Mat scanner_;       // 经过扫描之后的照片
    cv::Mat preprocess_;    // 预处理照片
};


/* =============================================================================================================== */
/* =============================================================================================================== */
/* =============================================================================================================== */

/*
 * @brief 用于读取图片的表格（utf8编码）
 * @param _sheet 储存表格的二维数组（按照row，column的形式）
 * @param _path 文件的路径(系统编码格式)
 */
void load_sheet_from_img(table< std::string > &_sheet, const std::string &_path);

}    // namespace img

#endif    // !IMGS_H
