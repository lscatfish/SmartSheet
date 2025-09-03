#pragma once
#ifndef IMGS_H
#define IMGS_H

#include <algorithm>
#include <basic.hpp>
#include <Encoding.h>
#include <opencv2/opencv.hpp>
#include <ppocr_API.h>
#include <string>
#include <utility>
#include <vector>

// 操作照片的空间
namespace img {

extern bool enable_ManualDocPerspectiveCorrector;

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

// 手动的透视矫正
class ManualDocPerspectiveCorrector {
public:
    ManualDocPerspectiveCorrector(const cv::Mat _inImg, const std::string &_SYSprompt)
        : srcPts_(4), dispPts_(4) {
        src_        = _inImg.clone( );
        windowName_ = "PerspectiveCorrector - " + encoding::utf8_to_sysdcode(_SYSprompt);

        // 初始化原始图像中的四边形顶点（默认在原图内偏移10%位置，避免贴边）
        float margin = ((std::min)(src_.cols, src_.rows)) * 0.01f;    // 偏移量 = 原图宽高的最小值 * 1%
        srcPts_[0]   = { margin, margin };                            // 顶点0：左上（x=margin, y=margin）
        srcPts_[1]   = { src_.cols - margin, margin };                // 顶点1：右上（x=原图宽-margin, y=margin）
        srcPts_[2]   = { src_.cols - margin, src_.rows - margin };    // 顶点2：右下（x=原图宽-margin, y=原图高-margin）
        srcPts_[3]   = { margin, src_.rows - margin };                // 顶点3：左下（x=margin, y=原图高-margin）

        // 创建主窗口并初始化
        cv::namedWindow(windowName_, cv::WINDOW_NORMAL);
        cv::resizeWindow(windowName_, 1000, 750);                  // 设置窗口初始大小（宽1000px，高750px）
        cv::setMouseCallback(windowName_, staticOnMouse, this);    // 绑定鼠标回调函数，处理用户交互

        cv::Rect last_roi;
        for (;;) {
            cv::Rect img_roi;
            double   scale;
            cv::Size sz = cv::getWindowImageRect(windowName_).size( );
            layout(sz, img_roi, scale);

            if (img_roi != last_roi || !done_) {
                redraw(img_roi);
                cv::imshow(windowName_, displayImg_);
                last_roi = img_roi;
            }
            cv::waitKey(10);
            if (done_) break;
        }
        cv::destroyWindow(windowName_);
    };
    ~ManualDocPerspectiveCorrector( ) = default;

    /*
     * @brief 计算两点间距离
     * @param 两个2D点a和b
     * @param 两点间的欧氏距离（基于OpenCV的norm函数，简化计算）
     */
    static double dist(const cv::Point2f &a, const cv::Point2f &b);

    /*
     * @brief 原始图像坐标转显示图像坐标
     * @param p 原始图像中的点坐标
     * @param scale 原始图像到显示图像的缩放比例
     * @param r 显示图像中缩放后原图的矩形区域（img_roi）
     * @return 该点在显示图像中的对应坐标（考虑按钮栏偏移和缩放）
     */
    static cv::Point2f src2disp(const cv::Point2f &p, double scale, const cv::Rect &r);

    /*
     * @brief 显示图像坐标转原始图像坐标
     * @param p 显示图像中的点坐标
     * @param scale 原始图像到显示图像的缩放比例
     * @param r 显示图像中缩放后原图的矩形区域（img_roi）
     * @return 该点在原始图像中的对应坐标（用于矫正计算，确保精度）
     */
    static cv::Point2f disp2src(const cv::Point2f &p, double scale, const cv::Rect &r);

    // 获取校正之后的图像
    cv::Mat get_corrected_img( );

private:
    cv::Mat src_;           // 原始输入图像
    cv::Mat displayImg_;    // 显示用图像（包含按钮栏和缩放后的原图，用于界面渲染）
    cv::Mat corrected_;     // 透视矫正后的输出图像

    std::vector< cv::Point2f > srcPts_;     // 原始图像中四边形顶点坐标（对应需要矫正的区域，用于计算透视矩阵）
    std::vector< cv::Point2f > dispPts_;    // 显示图像中四边形顶点坐标（随窗口缩放动态变化，用于界面拖拽交互）

    bool isDragging_ = false;    // 标记是否处于顶点拖拽状态
    int  dragIdx_    = -1;       // 被拖拽顶点的索引（0-3，对应左上、右上、右下、左下）
    bool done_       = false;    // 标记是否完成矫正操作（点击done按钮后置为true）

    const int BAR_H = 60;     // 窗口顶部按钮栏的高度（单位：像素）
    const int BTN_W = 100;    // "done"按钮的宽度（单位：像素）
    const int BTN_H = 40;     // "done"按钮的高度（单位：像素）
    // 目标A4竖版纸张的像素尺寸（基于300 DPI打印精度）
    // 换算公式：像素 = 物理尺寸(mm) * DPI / 25.4(mm/inch)
    const int a4w = 2480;    // A4宽度：210mm * 300 / 25.4 ≈ 2480像素
    const int a4h = 3508;    // A4高度：297mm * 300 / 25.4 ≈ 3508像素

    cv::Rect    btnOK_;         // "done"按钮的矩形区域（存储位置和大小，用于判断点击）
    std::string windowName_;    // 主窗口名称


    /*
     * @brief 布局计算函数：动态调整界面元素位置
     * @param win_size 当前主窗口的尺寸（宽x高）
     * @return img_roi 缩放后原图在显示图像中的矩形区域
     * @return scale 原始图像到显示图像的缩放比例（确保原图完整显示且不拉伸）
     */
    void layout(const cv::Size &win_size, cv::Rect &img_roi, double &scale);

    /*
     * @brief 绘制函数：渲染整个显示界面
     * @brief 绘制按钮栏、缩放后的原图、可拖拽的四边形顶点和边
     * @param img_roi 缩放后原图在显示图像中的矩形区域
     */
    void redraw(const cv::Rect &img_roi);

    /*
     * @brief 鼠标回调函数：处理用户交互
     * @brief 响应鼠标点击、拖拽事件，实现顶点调整和矫正触发
     * @param evt 鼠标事件类型（按下、移动、释放等）
     * @param x,y 鼠标在窗口中的坐标
     * @param void* 用户数据（此处未使用）
     */
    void onMouse(int evt, int x, int y, int, void *);

    //// 静态成员函数：作为 setMouseCallback 的回调（无 this 指针）
    static void staticOnMouse(int event, int x, int y, int flags, void *userdata);
};


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
        preprocess_ = preprocess(_inImg);
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

/*
 * @brief 用于读取图片的表格（utf8编码）
 * @param _sheet 储存表格的二维数组（按照row，column的形式）
 * @param _path 文件的路径(系统编码格式)
 */
void load_sheet_from_img(table< std::string > &_sheet, const std::string &_path);

}    // namespace img

#endif    // !IMGS_H
