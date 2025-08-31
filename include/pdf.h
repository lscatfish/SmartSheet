#pragma once

/*******************************************************************************
 * @file pdf.h
 * 作者：lscatfish、KIMI
 *******************************************************************************/

#ifndef PDF_H
#define PDF_H

#include <basic.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <PersonnelInformation.h>
#include <poppler/cpp/poppler-document.h>
#include <poppler/GfxState.h>
#include <poppler/goo/GooString.h>
#include <poppler/OutputDev.h>
#include <poppler/PDFDoc.h>
#include <string>
#include <vector>

// 此空间用于解析pdf文件
// 输入文件名请采用 UTF-8 编码
namespace pdf {

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
    // @todo 按理来说这里应该先检测文件是否存在
    DefPdf(const std::string &_u8path)
        : pdfdoc_(std::make_unique< GooString >(_u8path.c_str( ))) {
        u8path_     = _u8path;
        document_ = poppler::document::load_from_file(u8path_);
        if (!document_) {
            std::cout << "Error: Could not open PDF file: " << u8path_ << std::endl;
            isOK = false;
            return;
        }
        if (!pdfdoc_.isOk( )) {
            std::cout << "Error: PDFDoc is not OK for file: " << u8path_ << std::endl;
            isOK = false;
            return;
        }
        std::cout << "Parse PDF file: \"" << _u8path << "\"";
        num_pages_ = pdfdoc_.getNumPages( );
        sheetType_ = SheetType::Others;
        isOK       = parse( );    // 解析
        //判断是否合理

        std::cout << " Done!" << std::endl;
    };

    /*
     * @brief 为searchingTool设计的构造函数
     * @param _u8path u8地址
     * @param out 输出的解析结果
     */
    DefPdf(const std::string &_u8path, list< list< CELL > > &out)
        : pdfdoc_(std::make_unique< GooString >(_u8path.c_str( ))) {
        u8path_     = _u8path;
        document_ = poppler::document::load_from_file(u8path_);
        std::cout << "Parse PDF file: \"" << _u8path << "\"";
        if (!document_) {
            std::cout << "Error: Could not open PDF file: " << u8path_ << std::endl;
            isOK = false;
            return;
        }
        if (!pdfdoc_.isOk( )) {
            std::cout << "Error: PDFDoc is not OK for file: " << u8path_ << std::endl;
            isOK = false;
            return;
        }
        sheetType_ = SheetType::Others;
        num_pages_ = pdfdoc_.getNumPages( );
        list< CELL > aP;    // 一页
        for (int i = 1; i <= num_pages_; i++) {
            aP = extract_textblocks(i);
            out.push_back(aP);
        }
        std::cout << " Done! " << std::endl;
    };

    /// 析构 DefPdf 对象，释放其占用的资源（如有）。
    ~DefPdf( ) = default;

    // 返回解析出的表格
    table< std::string > get_sheet( );

    // 返回是否解析成功
    bool isOKed( ) const;

    // 返回解析出来的表格的类型
    SheetType get_sheet_type( ) const;

    // 获取人员的信息
    DefPerson get_person( ) const;

    // 打印表格
    void print_sheet( ) const;

private:
    std::string        u8path_;    // 文件所在的路径(utf8)
    poppler::document *document_ = nullptr;
    PDFDoc             pdfdoc_;
    table< CELL >      sheet_;        // 提取出的表格
    SheetType          sheetType_;    // 表格的类型
    int                num_pages_;
    bool               isOK;    // 是否解析成功

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
    bool parse( );

    /*
     * 解析表格线
     * @param _lineSegmentList 解析出的线
     */
    table< CELL > parse_line_to_sheet(const list< LineSegment > &_lineSegmentList);

    /*
     * @brief 直接按照文本框解析表格
     * @param _textBoxList 解析出的内容
     */
    table< CELL > parse_textbox_to_sheet(const list< CELL > &_textBoxList);
    /* ===================================为parse_textbox_to_sheet解析============================================== */
    /*
     * @brief 按行聚类，聚类条件是从中心开始最小行高的1.5倍内的所有水平中心线
     * @param _textBoxList 解析出的内容
     * @return 返回聚类之后的行
     */
    table< CELL > cluster_rows(list< CELL > _textBoxList);
    /* ===================================为parse_textbox_to_sheet解析============================================== */

    /*
     * @brief 填充解析出的表格
     * @param _textBoxList 解析出的文字块
     */
    void fill_sheet(const list< CELL > &_textBoxList);
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
