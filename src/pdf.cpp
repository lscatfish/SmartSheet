
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

    list< CELL >        textBoxeList;
    list< LineSegment > lineSegmentList;
    for (int ipage = 1; ipage <= num_pages_; ipage++) {
        textBoxeList = extract_textblocks(ipage);
        if (textBoxeList.size( ) == 0) continue;
        // 制作搜索库
        list< std::string > searchLib;
        for (const auto &textboxe : textBoxeList) {
            searchLib.push_back(textboxe.text);
        }

        if (fuzzy::search(searchLib, keyWordCom, fuzzy::LEVEL::High)) {
            sheetType_      = SheetType::Committee;
            lineSegmentList = extract_line_segments(ipage);
            break;
        } else if (fuzzy::search(searchLib, keyWordCla, fuzzy::LEVEL::High)) {
            sheetType_      = SheetType::Classmate;
            lineSegmentList = extract_line_segments(ipage);
            break;
        }
    }
    // 解析
    if (sheetType_ == SheetType::Others) {
        return;
    } else {
    }
}

/*
 * 解析表格线
 */
void DefPdf::parse_line_to_sheet( ) {

}




/* =============================================================================================================== */

// pdf启动函数
void Init( ) {
    // 初始化 Poppler
    globalParams = std::make_unique< GlobalParams >( );
}

}    // namespace pdf
