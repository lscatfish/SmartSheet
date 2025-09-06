
#include <algorithm>
#include <basic.hpp>
#include <console.h>
#include <cstdlib>
#include <Fuzzy.h>
#include <helper.h>
#include <iostream>
#include <iterator>
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
#include <QingziClass.h>
#include <set>
#include <string>
#include <vector>

// pdf标准坐标系是左下角为原点，y轴向上
namespace pdf {

// 以文件地址进行构造
// @todo 按理来说这里应该先检测文件是否存在
DefPdf::DefPdf(const chstring &_path)
    : pdfdoc_(std::make_unique< GooString >(_path.u8string( ).c_str( ))) {
    path_     = _path;
    document_ = poppler::document::load_from_file(path_.u8string( ));
    if (!document_) {
        std::cout << "Error: Could not open PDF file: " << path_ << std::endl;
        isOK = false;
        return;
    }
    if (!pdfdoc_.isOk( )) {
        std::cout << "Error: PDFDoc is not OK for file: " << path_ << std::endl;
        isOK = false;
        return;
    }
    std::cout << "Parse PDF file: \"" << path_ << "\"";
    num_pages_ = pdfdoc_.getNumPages( );
    sheetType_ = SheetType::Others;
    isOK       = parse( );    // 解析

    std::cout << " Done!" << std::endl;
}

/*
 * @brief 为searchingTool设计的构造函数
 * @param _u8path u8地址
 * @param out 输出的解析结果
 */
DefPdf::DefPdf(const chstring &_path, list< list< CELL > > &out)
    : pdfdoc_(std::make_unique< GooString >(_path.u8string( ).c_str( ))) {
    path_     = _path;
    document_ = poppler::document::load_from_file(path_.u8string( ));
    std::cout << "Parse PDF file: \"" << path_ << "\"";
    if (!document_) {
        std::cout << "Error: Could not open PDF file: " << path_ << std::endl;
        isOK = false;
        return;
    }
    if (!pdfdoc_.isOk( )) {
        std::cout << "Error: PDFDoc is not OK for file: " << path_ << std::endl;
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
    page->display(&extractor, 72.0, 72.0, 0, false, false, false);    // 默认解析的dpi

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
        CELL aaa(textBlock, pageHeight);    // 降低-13.5个像素的y坐标误差，不降低
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
    } else if (sheetType_ == SheetType::Committee) {
        if (lineSegmentList.size( ) == 0) return false;
        if (textBoxList.size( ) == 0) return false;
        sheet_ = parse_line_to_sheet(lineSegmentList);    // 先解析直线sheet
        fill_sheet(textBoxList);                          // 填充sheet_
        return true;
    } else if (sheetType_ == SheetType::Classmate) {
        if (textBoxList.size( ) == 0) return false;
        if (lineSegmentList.size( ) != 0) {
            sheet_ = parse_line_to_sheet(lineSegmentList);    // 先解析直线sheet
            fill_sheet(textBoxList);                          // 填充sheet_
            // 填充之后要判断是否正确
            DefPerson per = get_person( );
            if (per.name.empty( ) || per.studentID.empty( )) {    // 这是必备的
                sheet_.clear( );
                sheet_ = parse_textbox_to_sheet(textBoxList);    // 采用聚类解析表格
                per    = get_person( );
                if (per.name.empty( ) || per.studentID.empty( ))
                    return false;
                else
                    return true;
            }
            return true;
        } else {
            // 其他解析方式
            sheet_        = parse_textbox_to_sheet(textBoxList);
            DefPerson per = get_person( );
            if (per.name.empty( ) || per.studentID.empty( ))
                return false;
            else
                return true;
        }
    }
    return false;    // 免得报warning
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
 * @brief 直接按照文本框解析表格
 * @param _textBoxList 解析出的内容
 */
table< CELL > DefPdf::parse_textbox_to_sheet(const list< CELL > &_textBoxList) {
    table< CELL > out;                   // 输出
    out = cluster_rows(_textBoxList);    // 聚类
    // 按行排序
    sort_my_list(out, [](const list< CELL > &a, const list< CELL > &b) { return a[0].corePoint.y > b[0].corePoint.y; });
    return out;
}

/*
 * @brief 按行聚类，聚类条件是从中心开始最小行高内的所有水平中心线
 * @param _textBoxList 解析出的内容
 * @return 返回聚类之后的行
 */
table< CELL > DefPdf::cluster_rows(list< CELL > _textBoxList) {
    for (auto &t : _textBoxList) t.ifSelect = false;    // 这行可以不要，不过o2应该优化掉了
    table< CELL > out;
    // 输出中位数
    auto get_medianY = [](const std::multiset< CELL, CELL::CompareByCorePointYDesc > &rowHeight) -> double {
        auto it = rowHeight.begin( );
        std::advance(it, rowHeight.size( ) / 2);

        if (rowHeight.size( ) % 2 == 0) {    // 偶数
            // 偶数个元素：返回中间两个元素的平均值
            auto it_prev = std::prev(it);    // 获取前一个元素的迭代器
            return (it->corePoint.y + it_prev->corePoint.y) / 2;
        } else if (rowHeight.size( ) % 2 == 1) {    // 奇数
            return it->corePoint.y;
        }
    };

    // 设置阈值
    double threshold = 100;
    for (size_t i = 0; i < _textBoxList.size( ); i++) {
        threshold = (std::min)(threshold, std::abs(_textBoxList[i].top_right.y - _textBoxList[i].bottom_left.y));
        if (_textBoxList[i].text == U8C(u8"姓名")) {
            threshold = std::abs(_textBoxList[i].top_left.y - _textBoxList[i].bottom_left.y);
            break;
        }
    }
    // threshold *= 1.4;    // 1.4倍高度，这是之前的一个设想

    // 解析
    for (size_t i = 0; i < _textBoxList.size( ); i++) {
        // 为每一个cell进行解析
        std::multiset< CELL, CELL::CompareByCorePointXAsc >  row;          // 每行中的内容要升序排列(从左到右)
        std::multiset< CELL, CELL::CompareByCorePointYDesc > rowHeight;    // 判断是否在同一行的set（从上到下）
        list< CELL >                                         r;            // 标准行
        if (!_textBoxList[i].ifSelect) {
            row.insert(_textBoxList[i]);
            rowHeight.insert(_textBoxList[i]);
            _textBoxList[i].ifSelect = true;
            // 同行判断
            for (size_t j = 0; j < _textBoxList.size( ); j++) {
                if (!_textBoxList[j].ifSelect) {    // 没有被选的框进行判断
                    auto medianY = get_medianY(rowHeight);
                    if ((std::abs)(_textBoxList[j].corePoint.y - medianY) < threshold) {
                        row.insert(_textBoxList[j]);
                        rowHeight.insert(_textBoxList[j]);
                        _textBoxList[j].ifSelect = true;
                    }
                }
            }
        }
        for (const auto &a : row) r.push_back(a);
        if (r.size( ) != 0) out.push_back(r);
    }

    return out;
}

/*
 * @brief 填充解析出的表格
 * @param _textBoxList 解析出的文字块
 */
void DefPdf::fill_sheet(const list< CELL > &_textBoxList) {
    // 填充之前对位置进行修正
    double deltaH = 18000;    // 修正参数
    for (const auto &c : _textBoxList) {
        if (c.text == U8C(u8"姓名")) {
            deltaH = sheet_[0][0].corePoint.y - c.corePoint.y;
            break;
        }
    }

    // 重构_textBoxList
    list< CELL > thisBox;
    for (const auto &c : _textBoxList) {
        CELL thisCc(c, deltaH);
        thisBox.push_back(thisCc);
    }

    for (const auto &t : thisBox) {
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
    perLine.information[U8C(u8"备注")]     = "";
    perLine.information[U8C(u8"个人特长")] = "";
    perLine.information[U8C(u8"工作经历")] = "";
    perLine.information[U8C(u8"获奖情况")] = "";
    perLine.information[U8C(u8"个人简介")] = "";

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
                        c++;
                    } else if (fuzzy::search_substring(U8C(u8"应聘岗位"), trim_whitespace(sheet_[r][c].text))) {
                        if (c + 1 < sheet_[r].size( )) {
                            perLine.information[U8C(u8"应聘岗位")] = trim_whitespace(sheet_[r][c + 1].text);
                        } else {
                            perLine.information[U8C(u8"应聘岗位")] = "UNKNOWN";
                        }
                        c++;
                    }
                }
            }
        }
        DoQingziClass::trans_personline_to_person(perLine, per);
        per.ifsign = true;
        per.optimize( );
        return per;
    } else if (sheetType_ == SheetType::Classmate) {    // 普通同学
        for (size_t row = 0; row < sheet_.size( ); row++) {
            for (size_t col = 0; col < sheet_[row].size( ); col++) {
                if (sheet_[row][col].text.size( ) != 0) {
                    if (fuzzy::search(headerLib, trim_whitespace(sheet_[row][col].text), fuzzy::LEVEL::High)) {
                        if (col + 1 < sheet_[row].size( )) {
                            perLine.information[trim_whitespace(sheet_[row][col].text)] = trim_whitespace(sheet_[row][col + 1].text);
                            col++;
                        }
                    } else if (trim_whitespace(sheet_[row][col].text) == U8C(u8"个人简介")) {
                        if (col + 1 < sheet_[row].size( )) {
                            if (sheet_[row][col + 1].text.size( ) != 0)
                                perLine.information[U8C(u8"个人简介")] =
                                    trim_whitespace(sheet_[row][col + 1].text);
                            if (trim_whitespace(sheet_[row][col + 1].text).size( ) < 60) {    // 20字
                                perLine.information[U8C(u8"备注")] =
                                    perLine.information[U8C(u8"备注")]
                                    + U8C(u8"个人简介极少；");
                                col++;
                            } else if (trim_whitespace(sheet_[row][col + 1].text).size( ) < 150) {    // 50字
                                perLine.information[U8C(u8"备注")] =
                                    perLine.information[U8C(u8"备注")]
                                    + U8C(u8"个人简介较少；");
                                col++;
                            }
                        }
                    } else if (trim_whitespace(sheet_[row][col].text) == U8C(u8"个人特长")) {
                        if (col + 1 < sheet_[row].size( )) {
                            if (sheet_[row][col + 1].text.size( ) != 0)
                                perLine.information[U8C(u8"个人特长")] =
                                    trim_whitespace(sheet_[row][col + 1].text);
                            if (trim_whitespace(sheet_[row][col + 1].text).size( ) < 30) {    // 10字
                                perLine.information[U8C(u8"备注")] =
                                    perLine.information[U8C(u8"备注")]
                                    + U8C(u8"个人特长极少；");
                                col++;
                            } else if (trim_whitespace(sheet_[row][col + 1].text).size( ) < 60) {    // 20字
                                perLine.information[U8C(u8"备注")] =
                                    perLine.information[U8C(u8"备注")]
                                    + U8C(u8"个人特长较少；");
                                col++;
                            }
                        }
                    } else if (trim_whitespace(sheet_[row][col].text) == U8C(u8"工作经历")) {
                        if (col + 1 < sheet_[row].size( )) {
                            if (sheet_[row][col + 1].text.size( ) != 0)
                                perLine.information[U8C(u8"工作经历")] = trim_whitespace(sheet_[row][col + 1].text);
                            if (trim_whitespace(sheet_[row][col + 1].text).size( ) < 30) {    // 10字
                                perLine.information[U8C(u8"备注")] =
                                    perLine.information[U8C(u8"备注")]
                                    + U8C(u8"工作经历极少；");
                                col++;
                            } else if (trim_whitespace(sheet_[row][col + 1].text).size( ) < 60) {    // 20字
                                perLine.information[U8C(u8"备注")] =
                                    perLine.information[U8C(u8"备注")]
                                    + U8C(u8"工作经历较少；");
                                col++;
                            }
                        }
                    } else if (trim_whitespace(sheet_[row][col].text) == U8C(u8"获奖情况")) {
                        if (col + 1 < sheet_[row].size( )) {
                            if (sheet_[row][col + 1].text.size( ) != 0)
                                perLine.information[U8C(u8"获奖情况")] =
                                    trim_whitespace(sheet_[row][col + 1].text);
                            col++;
                        }
                    }
                }
            }
        }
        DoQingziClass::trans_personline_to_person(perLine, per);
        if (fuzzy::search_substring(path_.u8string( ), U8C(u8"自主报名")))
            per.otherInformation[U8C(u8"报名方式")] = U8C(u8"自主报名");
        else if (fuzzy::search_substring(path_.u8string( ), U8C(u8"重庆大学团校")))
            per.otherInformation[U8C(u8"报名方式")] = U8C(u8"自主报名");
        else
            per.otherInformation[U8C(u8"报名方式")] = U8C(u8"组织推荐");
        per.otherInformation[U8C(u8"文件地址")] = path_.u8string( );
        per.optimize( );
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
            std::cout << "(" << cell.top_left.x << "," << cell.top_left.y << ")-("
                      << cell.bottom_right.x << "," << cell.bottom_right.y << ")"
                      << cell.text << "\t";
        }
        std::cout << "\n";
    }
    std::cout << std::endl;
}

/* =============================================================================================================== */
/* =============================================================================================================== */
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
