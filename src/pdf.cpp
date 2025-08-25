
#include <algorithm>
#include <basic.hpp>
#include <basic.hpp>
#include <cstdlib>
#include <Fuzzy.h>
#include <helper.h>
#include <iostream>
#include <memory>
#include <pdf.h>
#include <PersonnelInformation.h>
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
#include<QingziClass.h>
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

    auto   page       = document_->create_page(pageNum_ - 1);
    auto   textList   = page->text_list( );
    double pageHeight = page->page_rect( ).height( );

    for (const auto &textBlock : textList) {
        CELL aaa(textBlock, pageHeight, -13.5);    // 降低-13.5个像素的y坐标误差
        textBoxes.push_back(aaa);
    }

    return textBoxes;
}

/*
 * @brief 解析pdf
 */
bool DefPdf::parse( ) {
    // 关键词(班委报名)
    const std::string keyWordCom = U8C(u8"应聘岗位");
    const std::string keyWordCla = U8C(u8"所任职务");

    list< CELL >        textBoxList;
    list< LineSegment > lineSegmentList;
    // 遍历每一页
    for (int ipage = 1; ipage <= num_pages_; ipage++) {
        textBoxList = extract_textblocks(ipage);
        if (textBoxList.size( ) == 0) continue;
        bool ifBreak = false;
        // 采用字串匹配到keyWord
        for (const auto &t : textBoxList) {
            if (fuzzy::search_substring(keyWordCom, t.text)) {
                sheetType_      = SheetType::Committee;
                lineSegmentList = extract_linesegments(ipage);
                ifBreak         = true;
                break;
            } else if (fuzzy::search_substring(keyWordCla, t.text)) {
                sheetType_      = SheetType::Classmate;
                lineSegmentList = extract_linesegments(ipage);
                ifBreak         = true;
                break;
            }
        }
        if (ifBreak) break;
    }

    // 解析
    if (sheetType_ == SheetType::Others) {
        return false;
    } else {
        if (lineSegmentList.size( ) == 0) return false;
        if (textBoxList.size( ) == 0) return false;

        sheet_ = parse_line_to_sheet(lineSegmentList);    // 先解析直线sheet
        fill_sheet(textBoxList);                          // 填充sheet_
        return true;
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
#if true

    for (const auto &a : horizontalLines) {
        std::cout << "h:y=" << (a.y1 + a.y2) / 2 << ")\n";
    }
    for (const auto &a : verticalLines) {
        std::cout << "v:x=" << (a.x1 + a.x2) / 2 << ")\n";
    }

#endif    // true



    /******************************************构造sheet***********************************************
     * 构造思路：
     * 从上部水平线开始遍历，只向下找下一条线
     * 从左部竖线开始遍历，只向右找下一条线
     * 构成的格子由四条线构成
     * 合并的单元格按照左上角所对应的行列进行储存
     ******************************************构造sheet***********************************************/
    for (size_t h1 = 0; h1 < horizontalLines.size( ) - 1; h1++) {
        list< CELL > row;
        for (size_t v1 = 0; v1 < verticalLines.size( ) - 1; v1++) {
            bool ifnext = false;
            for (size_t h2 = h1 + 1; h2 < horizontalLines.size( ); h2++) {
                for (size_t v2 = v1 + 1; v2 < verticalLines.size( ); v2++) {
                    // 相交判断
                    if (is_linesegments_intersect(horizontalLines[h1], verticalLines[v1])
                        && is_linesegments_intersect(horizontalLines[h1], verticalLines[v2])
                        && is_linesegments_intersect(horizontalLines[h2], verticalLines[v1])
                        && is_linesegments_intersect(horizontalLines[h2], verticalLines[v2])) {
                        // 此时是一个单元格
                        CELL c(horizontalLines[h1], horizontalLines[h2], verticalLines[v1], verticalLines[v2]);
                        row.push_back(c);
                        ifnext = true;
                        break;
                    }
                }
                if (ifnext) break;
            }
        }
        outSheet.push_back(row);
    }
    return outSheet;
}

/*
 * @brief 填充解析出的表格
 * @param _textBoxList 解析出的文字块
 */
void DefPdf::fill_sheet(const list< CELL > &_textBoxList) {
    for (const auto &t : _textBoxList) {
        for (auto &r : sheet_) {
            for (auto &c : r) {
                if (t.is_contained_for_pdf(c)) {
                    c.text = c.text + t.text;
                }
            }
        }
    }
}

// 返回解析出的表格
table< std::string > DefPdf::get_sheet( ) {
    if (sheet_.size( ) == 0) {
        return table< std::string >{ };
    }
    table< std::string > outSheet;
    for (const auto &row : sheet_) {
        list< std::string > r;
        for (const auto &cell : row) {
            r.push_back(cell.text);
        }
        outSheet.push_back(r);
    }
    return outSheet;
}

// 返回是否解析成功
bool DefPdf::isOKed( ) const {
    return isOK;
}

// 返回解析出来的表格的类型
DefPdf::SheetType DefPdf::get_sheet_type( ) const {
    return sheetType_;
}

// 获取人员的信息
DefPerson DefPdf::get_person( ) const {
    if (sheet_.size( ) == 0) {
        return DefPerson( );
    }
    DefPerson per;
    DefLine   perLine;
    // 定义关键的词
    const list< std::string > headerLib{
        U8C(u8"姓名"),
        U8C(u8"性别"), U8C(u8"年级"),
        U8C(u8"学号"), U8C(u8"政治面貌"),
        U8C(u8"学院"), U8C(u8"专业"),
        U8C(u8"电话"), U8C(u8"联系方式"),
        U8C(u8"联系电话"), U8C(u8"电话号码"),
        U8C(u8"QQ号"), U8C(u8"qq号"),
        U8C(u8"qq"), U8C(u8"QQ"),
        U8C(u8"所任职务"), U8C(u8"职务"),
        U8C(u8"学生职务"),
        U8C(u8"邮箱"), U8C(u8"民族"),
        U8C(u8"社团"), U8C(u8"报名青字班"),
        U8C(u8"青字班")
    };
    if (sheetType_ == SheetType::Committee) {
        // 班委，只用记录报了哪些名
        for (size_t r = 0; r < sheet_.size( ); r++) {
            for (size_t c = 0; c < sheet_[r].size( ); c++) {
                if (sheet_[r][c].text.size( ) != 0) {
                    if (fuzzy::search(headerLib, trim_whitespace(sheet_[r][c].text), fuzzy::LEVEL::High)) {
                        std::string key = trim_whitespace(sheet_[r][c].text);
                        if (c + 1 < sheet_[r].size( )) {
                            perLine.information[key] = trim_whitespace(sheet_[r][c + 1].text);
                        } else {
                            perLine.information[key] = "";
                        }
                    } else if (fuzzy::search_substring(U8C(u8"应聘岗位"), trim_whitespace(sheet_[r][c].text))) {
                        if (c + 1 < sheet_[r].size( )) {
                            perLine.information[U8C(u8"应聘岗位")] = trim_whitespace(sheet_[r][c + 1].text);
                        } else {
                            perLine.information[U8C(u8"应聘岗位")] = "UNKNOWN";
                        }
                    }
                }
            }
        }
        DoQingziClass::trans_line_to_person(perLine, per);
        per.ifsign = true;
        return per;
    } else if (sheetType_ == SheetType::Classmate) {
        // 普通同学
        return per;
    }

    return DefPerson( );
}

// 打印表格
void DefPdf::print_sheet( ) const {
    if (sheet_.size( ) == 0) {
        std::cout << "Empty sheet.\n";
        return;
    }
    std::cout << "Extracted sheet with " << sheet_.size( ) << " rows:\n";
    for (const auto &row : sheet_) {
        std::cout << "size: " << row.size( ) << " | ";
        for (const auto &cell : row) {
            std::cout << "core(" << cell.corePoint.x << "," << cell.corePoint.y << ")" << cell.text << "\t";
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
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
 * 思路：(^-^)
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
