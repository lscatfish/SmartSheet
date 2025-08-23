
#include <basic.hpp>
#include <iostream>
#include <memory>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-page.h>
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
struct LineSegment {
    double x1, y1, x2, y2;
};

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

/**
 * 提取 PDF 页面中的所有线段（直线）
 * @param pdfPath  PDF 文件路径
 * @param pageNum  页码（从 1 开始）
 * @return  所有线段的列表
 */
std::vector< LineSegment > extractLineSegments(const std::string &pdfPath, int pageNum) {
    // 初始化 Poppler
    globalParams = std::make_unique< GlobalParams >( );

    PDFDoc doc(std::make_unique< GooString >(pdfPath.c_str( )));
    if (!doc.isOk( ) || pageNum < 1 || pageNum > doc.getNumPages( )) {
        std::cerr << "Invalid PDF or page number.\n";
        return { };
    }

    Page *page = doc.getPage(pageNum);
    if (!page || !page->isOk( )) {
        std::cerr << "Failed to load page.\n";
        return { };
    }

    LineExtractor extractor;
    page->display(&extractor, 72.0, 72.0, 0, false, false, false);

    return extractor.lines;
}

// 示例用法
void tmain( ) {
    auto lines = extractLineSegments(U8C(u8"测.pdf"), 1);
    std::cout << "Found " << lines.size( ) << " line segments:\n";
    for (const auto &l : lines) {
        std::cout << "Line: (" << l.x1 << ", " << l.y1 << ") -> ("
                  << l.x2 << ", " << l.y2 << ")\n";
    }
    return;
}