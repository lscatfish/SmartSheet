
#include <algorithm>
#include <basic.hpp>
#include <cstdlib>
#include <Fuzzy.h>
#include <iostream>
#include <memory>
#include <pdf.h>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-global.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-rectangle.h>
#include <poppler/cpp/poppler-version.h>
#include <poppler/Gfx.h>
#include <poppler/GfxState.h>
#include <poppler/GlobalParams.h>
#include <poppler/goo/GooString.h>
#include <poppler/OutputDev.h>
#include <poppler/Page.h>
#include <poppler/PDFDoc.h>
#include <string>
#include <vector>

// pdf标准坐标系是左下角为原点，y轴向上
namespace pdf {

/**
 * 提取 PDF 页面中的所有线段（直线）
 * @param pageNum_  页码（从 1 开始）
 * @return  所有线段的列表
 */
list< LineSegment > DefPdf::extract_linesegments(int pageNum_) {

    Page *page = pdfdoc_.getPage(pageNum_);
    if (!page || !page->isOk( )) {
        std::cerr << "Failed to load page.\n";
        return { };
    }

    LineExtractor extractor;
    page->display(&extractor, 72.0, 72.0, 0, false, false, false);

    return extractor.lines;
}

/*
 * 解析pdf文件的所有文本框
 * @param pageNum_ 页码（从1开始）
 */
list< CELL > DefPdf::extract_textblocks(int pageNum_) {
    list< CELL > textBoxes;

    auto page     = document_->create_page(pageNum_ - 1);
    auto textList = page->text_list( );

    for (const auto &textBlock : textList) {
        CELL aaa(textBlock);
        textBoxes.push_back(aaa);
    }

    return textBoxes;
}

/*
 * @brief 解析pdf
 */
void DefPdf::parse( ) {
    // 关键词(班委报名)
    const std::string keyWordCom = U8C(u8"应聘岗位");
    const std::string keyWordCla = U8C(u8"所任职务");

    list< CELL >        textBoxList;
    list< LineSegment > lineSegmentList;
    for (int ipage = 1; ipage <= num_pages_; ipage++) {
        textBoxList = extract_textblocks(ipage);
        if (textBoxList.size( ) == 0) continue;
        // 制作搜索库
        list< std::string > searchLib;
        for (const auto &textboxe : textBoxList) {
            searchLib.push_back(textboxe.text);
        }

        if (fuzzy::search(searchLib, keyWordCom, fuzzy::LEVEL::High)) {
            sheetType_      = SheetType::Committee;
            lineSegmentList = extract_linesegments(ipage);
            break;
        } else if (fuzzy::search(searchLib, keyWordCla, fuzzy::LEVEL::High)) {
            sheetType_      = SheetType::Classmate;
            lineSegmentList = extract_linesegments(ipage);
            break;
        }
    }
    // 解析
    if (sheetType_ == SheetType::Others) {
        return;
    } else {
        parse_line_to_sheet(lineSegmentList);    // 先解析直线sheet
    }
}

/*
 * 解析表格线到table中
 * @param _lineSegmentList 解析出的线
 */
table< CELL > DefPdf::parse_line_to_sheet(const list< LineSegment > &_lineSegmentList) {
    table< CELL > outSheet;    // 输出的表格
    // 对线段分类
    list< LineSegment > horizontalLines;    // 水平线
    list< LineSegment > verticalLines;      // 竖线
    for (const auto &line : _lineSegmentList) {
        if (line.t == LineSegment::Type::Horizontal) {
            horizontalLines.push_back(line);
        } else if (line.t == LineSegment::Type::Vertical) {
            verticalLines.push_back(line);
        }
    }
    // 对线段进行排序
    // 水平线按照y的降序
    sort_my_list(horizontalLines,
                 [](const LineSegment &a, const LineSegment &b) { return ((a.y1 + a.y2) / 2) > ((b.y1 + b.y2) / 2); });
    // 竖线按照x的升序
    sort_my_list(verticalLines,
                 [](const LineSegment &a, const LineSegment &b) { return ((a.x1 + a.x2) / 2) < ((b.x1 + b.x2) / 2); });
    /******************************************构造sheet***********************************************
     * 构造思路：
     * 从上部水平线开始遍历，只向下找下一条线
     * 从左部竖线开始遍历，只向右找下一条线
     * 构成的格子由四条线构成
     * 合并的单元格按照左上角所对应的行列进行储存
     ******************************************构造sheet***********************************************/
    for (size_t i = 0; i < horizontalLines.size( ) - 1; i++) {
        std::vector< CELL > row;    // 一行
        for (size_t j = 0; j < verticalLines.size( ) - 1; j++) {
            // 判断相交
            if (!is_linesegments_intersect(horizontalLines[i], verticalLines[j])) {
                continue;
            } else {
                // 相交，找下一条与竖线相交的水平线
                for (size_t k = i + 1; k < horizontalLines.size( ); k++) {
                    if (!is_linesegments_intersect(verticalLines[j], horizontalLines[k])) {
                        continue;
                    }
                }

            }
        }
        outSheet.push_back(row);
    }
}

/* =============================================================================================================== */

// pdf启动函数
void Init( ) {
    // 初始化 Poppler
    globalParams = std::make_unique< GlobalParams >( );
}

/*
 * @brief 判断两线段是否相交
 * @param a 线段a
 * @param b 线段b
 * @return 是否相交
 * 思路：
 */
bool is_linesegments_intersect(const LineSegment &a, const LineSegment &b) {
    /* -------------------------------lambda------------------------------------ */
    // 计算叉积 (p1 - p0) × (p2 - p0)
    // 结果>0：p2在p0p1的左侧；=0：共线；<0：右侧
    const auto crossProduct = [](const GridPoint &p0, const GridPoint &p1, const GridPoint &p2) -> double {
        return (p1.x - p0.x) * (p2.y - p0.y) - (p1.y - p0.y) * (p2.x - p0.x);
    };

    // 判断点p是否在线段ab上（需确保p在ab所在直线上）
    const auto isPointOnSegment = [](const GridPoint &p, const GridPoint &a, const GridPoint &b) -> bool {
        // 检查p的坐标是否在a和b的坐标范围内
        if (std::min(a.x, b.x) - 1e-8 <= p.x
            && p.x <= std::max(a.x, b.x) + 1e-8
            && std::min(a.y, b.y) - 1e-8 <= p.y
            && p.y <= std::max(a.y, b.y) + 1e-8) {
            return true;
        }
        return false;
    };
    /* -------------------------------lambda------------------------------------ */

    // 步骤1：快速排斥实验
    // 检查线段边界框是否重叠
    if ((std::max)(a.x1, a.x2) < (std::min)(b.x1, b.x2) - 1e-8
        || (std::max)(b.x1, b.x2) < (std::min)(a.x1, a.x2) - 1e-8
        || (std::max)(a.y1, a.y2) < (std::min)(b.y1, b.y2) - 1e-8
        || (std::max)(b.y1, b.y2) < (std::min)(a.y1, a.y2) - 1e-8) {
        return false;
    }

    GridPoint p1(a.x1, a.y1);
    GridPoint p2(a.x2, a.y2);
    GridPoint p3(b.x1, b.y1);
    GridPoint p4(b.x2, b.y2);

    // 步骤2：跨立实验
    double c1 = crossProduct(p1, p2, p3);
    double c2 = crossProduct(p1, p2, p4);
    double c3 = crossProduct(p3, p4, p1);
    double c4 = crossProduct(p3, p4, p2);
    // 情况1：线段端点在另一条线段的两侧（叉积异号）
    if ((c1 * c2 < -1e-8) && (c3 * c4 < -1e-8)) {
        return true;
    }
    // 情况2：线段端点在另一条线段上（叉积为0，需额外判断是否在线段上）
    if (c1 == 0 && isPointOnSegment(p3, p1, p2)) return true;
    if (c2 == 0 && isPointOnSegment(p4, p1, p2)) return true;
    if (c3 == 0 && isPointOnSegment(p1, p3, p4)) return true;
    if (c4 == 0 && isPointOnSegment(p2, p3, p4)) return true;

    // 其他情况：不相交
    return false;
}

}    // namespace pdf
