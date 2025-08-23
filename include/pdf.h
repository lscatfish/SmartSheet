#pragma once

#ifndef PDF_H
#define PDF_H

#include <basic.hpp>
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

// pdf启动函数
void Init( );

// 定义线段结构体
struct LineSegment {
    double x1, y1, x2, y2;
};

// 提取 PDF 页面中的所有的文本框
struct TextBox {
    double      x1, y1;    // 左上角坐标
    double      x2, y2;    // 右下角坐标
    std::string text;      // 文本内容

    TextBox(double _x1, double _y1, double _x2, double _y2, poppler::byte_array _text) {
        x1 = _x1;
        y1 = _y1;
        x2 = _x2;
        y2 = _y2;
        for (auto &c : _text)
            text.push_back(c);
    };

    TextBox( ) {
        x1 = 0;
        x2 = 0;
        y2 = 0;
        y1 = 0;
        text = "";
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

                lines.push_back({ tx0, ty0, tx1, ty1 });
            }
        }
    }
};



// 此类用于处理pdf文件
class DefPdf {
public:

    /**
     * 提取 PDF 页面中的所有线段（直线）
     * @param pageNum_  页码（从 1 开始）[一般是1]
     * @return  所有线段的列表
     */
    list< LineSegment > extract_line_segments(int pageNum_);

    /*
     * 解析pdf文件的所有文本框
     * @param pageNum_ 页码（从1开始）[一般是1]
    */
    list< TextBox > extract_textblocks(int pageNum_);

    // 以文件地址进行构造
    DefPdf(std::string _u8path)
        : pdfdoc_(std::make_unique< GooString >(path_.c_str( ))) {

        path_     = _u8path;
        document_ = poppler::document::load_from_file(path_);
        if (!document_) {
            std::cout << "Error: Could not open PDF file: " << path_ << std::endl;
            return;
        }
        if ( !pdfdoc_.isOk( ) ) {
            std::cout << "Error: PDFDoc is not OK for file: " << path_ << std::endl;
            return;
        }
        num_pages_ = pdfdoc_.getNumPages( );


    };

    ~DefPdf( ) = default;


private:

    std::string         path_;         // 文件所在的路径
    list< TextBox >     textboxes_;    // 提取的文本框
    list< LineSegment > lines_;        // 提取的线段
    poppler::document  *document_ = nullptr;
    PDFDoc              pdfdoc_;
    int                 num_pages_;

};


}    // namespace pdf

#endif    // PDF_H
