
#include <basic.hpp>
#include <cstdlib>
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
list< LineSegment > DefPdf::extract_line_segments(int pageNum_) {

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
list< TextBox > DefPdf::extract_textblocks(int pageNum_) {
    std::vector< TextBox > textBoxes;

    auto page     = document_->create_page(pageNum_ - 1);
    auto textList = page->text_list( );

    for (auto &textBlock : textList) {
        poppler::rectf bbox = textBlock.bbox( );
        TextBox        aaa(bbox.left( ),
                           bbox.top( ),
                           bbox.right( ),
                           bbox.bottom( ),
                           textBlock.text( ).to_utf8( ));
        textBoxes.push_back(aaa);
    }

    return textBoxes;
}




/* ============================================================================================================= */

// pdf启动函数
void Init( ) {
    // 初始化 Poppler
    globalParams = std::make_unique< GlobalParams >( );
}

}    // namespace pdf
