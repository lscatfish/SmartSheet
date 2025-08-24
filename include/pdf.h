#pragma once

#ifndef PDF_H
#define PDF_H

#include <basic.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
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

// 此空间用于解析pdf文件
// 输入文件名请采用 UTF-8 编码
namespace pdf {


// 定义线段结构体
struct LineSegment {
public:
    // 线段的类型
    enum class Type {
        Others = 0,    // 其他类型
        Horizontal,    // 水平线
        Vertical       // 竖线
    };
    double x1, y1, x2, y2;
    Type   t;

    LineSegment(double _x1, double _y1, double _x2, double _y2) {
        x1 = _x1;
        x2 = _x2;
        y1 = _y1;
        y2 = _y2;
        if (std::abs(y1 - y2) < 1.0) {
            t = Type::Horizontal;
            // 向两端延长
            if (x1 < x2) {
                x1 = x1 - 1;
                x2 = x2 + 1;
                if (x1 < 0) x1 = 0;
            } else if (x1 > x2) {
                x1 = x1 + 1;
                x2 = x2 - 1;
                if (x2 < 0) x2 = 0;
            }
        } else if (std::abs(x1 - x2) < 1.0) {
            t = Type::Vertical;
            if (y1 < y2) {
                y1 = y1 - 1;
                y2 = y2 + 1;
                if (y1 < 0) y1 = 0;
            } else if (y1 > y2) {
                y1 = y1 + 1;
                y2 = y2 - 1;
                if (y2 < 0) y2 = 0;
            }
        } else {
            t = Type::Others;
        }
    };
};

// 定义线段提取类，继承自 OutputDev
class LineExtractor : public OutputDev {
public:
    std::vector< LineSegment > lines;

    bool upsideDown( ) override {
        return false;
    }
    bool useDrawChar( ) override {
        return false;
    }
    bool interpretType3Chars( ) override {
        return false;
    }

    void stroke(GfxState *state) override {
        const GfxPath *path = state->getPath( );
        if (!path) return;

        for (int i = 0; i < path->getNumSubpaths( ); ++i) {
            const GfxSubpath *sub = path->getSubpath(i);
            if (!sub || sub->getNumPoints( ) < 2) continue;

            for (int j = 1; j < sub->getNumPoints( ); ++j) {
                if (sub->getCurve(j)) continue;    // 跳过贝塞尔曲线控制点

                double x0 = sub->getX(j - 1);
                double y0 = sub->getY(j - 1);
                double x1 = sub->getX(j);
                double y1 = sub->getY(j);

                double tx0, ty0, tx1, ty1;
                state->transform(x0, y0, &tx0, &ty0);
                state->transform(x1, y1, &tx1, &ty1);

                lines.push_back(LineSegment(tx0, ty0, tx1, ty1));
            }
        }
    }
};

// 此类用于处理pdf文件
class DefPdf {
public:
    // pdf中储存表格的类型
    enum class SheetType {
        Others = 0,    // 其他表
        Committee,     // 班委应聘表
        Classmate      // 普通报名表
    };

    // 以文件地址进行构造
    DefPdf(std::string _u8path)
        : pdfdoc_(std::make_unique< GooString >(path_.c_str( ))) {

        path_     = _u8path;
        document_ = poppler::document::load_from_file(path_);
        if (!document_) {
            std::cout << "Error: Could not open PDF file: " << path_ << std::endl;
            return;
        }
        if (!pdfdoc_.isOk( )) {
            std::cout << "Error: PDFDoc is not OK for file: " << path_ << std::endl;
            return;
        }
        num_pages_ = pdfdoc_.getNumPages( );
        sheetType_ = SheetType::Others;
        parse( );
    };

    ~DefPdf( ) = default;


private:
    std::string path_;    // 文件所在的路径

    // list< TextBox >     textboxes_;    // 提取的文本框
    //  list< LineSegment > lines_;        // 提取的线段

    poppler::document   *document_ = nullptr;
    PDFDoc               pdfdoc_;
    table< std::string > sheet_;        // 提取出的表格
    SheetType            sheetType_;    // 表格的类型
    int                  num_pages_;

    /**
     * 提取 PDF 页面中的所有线段（直线）
     * @param pageNum_  页码（从 1 开始）[一般是1]
     * @return  所有线段的列表
     */
    list< LineSegment > extract_linesegments(int pageNum_);

    /*
     * 解析pdf文件的所有文本框
     * @param pageNum_ 页码（从1开始）[一般是1]
     */
    list< CELL > extract_textblocks(int pageNum_);

    /*
     * @brief 解析的控制函数
     */
    void parse( );

    /*
     * 解析表格线
     * @param _lineSegmentList 解析出的线
     */
    table< CELL > parse_line_to_sheet(const list< LineSegment > &_lineSegmentList);
};

/*
 * @brief 判断两线段是否相交
 * @param a 线段a
 * @param b 线段b
 * @return 是否相交
 */
bool is_linesegments_intersect(const LineSegment &a, const LineSegment &b);

// pdf启动函数
void Init( );

}    // namespace pdf

#endif    // PDF_H
