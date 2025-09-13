
#include <basic.hpp>
#include <chstring.hpp>
#include <Encoding.h>
#include <excel.h>
#include <exception>
#include <high.h>
#include <iostream>
#include <string>
#include <xlnt/xlnt.hpp>


// 用于解析xlsx的类
namespace xlsx {
// 定义部分常用字体
xlnt::font                    stdfontRegular_attSheet = set_font(U8C(u8"宋体"), 14);
xlnt::font                    stdfontTitle_attSheet   = set_font(U8C(u8"宋体"), 24);
xlnt::font                    stdfontRegular_sttSheet = set_font(U8C(u8"宋体"), 16);
xlnt::font                    stdfontHeader_sttSheet  = set_font(U8C(u8"宋体"), 16, true);
xlnt::font                    stdfontTitle_sttSheet   = set_font(U8C(u8"方正小标宋简体"), 26);
xlnt::border::border_property stdBorderProperty       = set_border_property( );
xlnt::border                  stdBorder               = set_bolder( );
xlnt::alignment               autoAlignment           = set_alignment( );

/*
 * @brief 设置字体
 * @param _name 字体名称
 * @param _fsize 字体大小（磅）
 * @param _bold 是否为粗体
 * @param _italic 是否为斜体
 * @param _strikethrough 是否取消删除线
 * @param _color 字体颜色
 * @param _underline_style 是否取消删除线
 */
xlnt::font set_font(
    const chstring                   &_name,
    const size_t                     &_fsize,
    const bool                        _bold,
    const bool                        _italic,
    const xlnt::color                &_color,
    const xlnt::font::underline_style _underline_style,
    const bool                        _strikethrough) {
    xlnt::font f;
    // xlnt::format ff;
    f.name(_name.u8string( ));
    f.size(_fsize);
    f.bold(_bold);
    f.italic(_italic);
    f.color(_color);
    f.underline(_underline_style);
    f.strikethrough(_strikethrough);
    return f;
}

/*
 * @brief 设置对齐方式
 * @param _h 水平对齐方式
 * @param _v 垂直对齐方式
 * @param _wrap 是否自动换行
 * @param _rotation 文本旋转角度
 */
xlnt::alignment set_alignment(
    const xlnt::horizontal_alignment _h,
    const xlnt::vertical_alignment   _v,
    const bool                       _wrap,
    const int                        _rotation) {
    xlnt::alignment align;
    align.horizontal(_h);
    align.vertical(_v);
    align.wrap(_wrap);
    if (_rotation != 0)
        align.rotation(_rotation);
    return align;
}

/*
 * @brief 设置边框的属性
 * @param _style 边框的线性
 * @param _color 边框的颜色
 */
xlnt::border::border_property set_border_property(const xlnt::border_style _style, const xlnt::color _color) {
    xlnt::border::border_property prop;
    prop.color(_color);
    prop.style(_style);
    return prop;
}

/*
 * @brief 设置单元格边框
 * @param _start 左
 * @param _end 右
 * @param _top 上
 * @param _bottom 下
 */
xlnt::border set_bolder(
    const xlnt::border::border_property _start,
    const xlnt::border::border_property _end,
    const xlnt::border::border_property _top,
    const xlnt::border::border_property _bottom) {
    xlnt::border b;
    b.side(xlnt::border_side::start, _start);
    b.side(xlnt::border_side::end, _end);
    b.side(xlnt::border_side::top, _top);
    b.side(xlnt::border_side::bottom, _bottom);
    return b;
}

XlsxWrite::XlsxWrite(
    const myTable< chstring > &_sh,
    const chstring            &_path,
    const double               _heightRegular,
    const myList< double >    &_widths,
    const xlnt::border        &_border,
    const xlnt::font          &_fontRegular,
    const xlnt::alignment     &_align,
    bool                       _hasTitle,
    const chstring            &_title,
    const xlnt::font          &_fontTitle,
    const double               _heightTitle,
    bool                       _hasHeader,
    const xlnt::font          &_fontHeader,
    const double               _heightHeader)
    : sheet_(_sh),
      title_(_title),
      borderCell_(_border),
      fontRegular_(_fontRegular),
      alignment_(_align),
      fontTitle_(_fontTitle),
      fontHeader_(_fontHeader),
      hasTitle_(_hasTitle),
      hasHeader_(_hasHeader),
      path_(_path),
      widths_(_widths),
      heightRegular_(_heightRegular),
      heightTitle_(_heightTitle),
      heightHeader_(_heightHeader) {}

XlsxWrite::XlsxWrite( ) {
    borderCell_    = stdBorder;
    fontRegular_   = stdfontRegular_sttSheet;
    alignment_     = autoAlignment;
    hasTitle_      = false;
    title_         = "";
    fontTitle_     = stdfontTitle_sttSheet;
    hasHeader_     = false;
    fontHeader_    = stdfontHeader_sttSheet;
    path_          = "";
    widths_        = myList< double >{ };
    heightRegular_ = 24;
    heightHeader_  = 24;
    heightTitle_   = 40;
}

// 设置正文字体
void XlsxWrite::fontRegular(const xlnt::font &_f) {
    fontRegular_ = _f;
}
// 获取正文字体
xlnt::font XlsxWrite::fontRegular( ) const {
    return fontRegular_;
}

// 设置表头字体
void XlsxWrite::fontHeader(const xlnt::font &_f) {
    fontHeader_ = _f;
}
// 获取正文字体
xlnt::font XlsxWrite::fontHeader( ) const {
    return fontHeader_;
}

// 设置标题字体
void XlsxWrite::fontTitle(const xlnt::font &_f) {
    fontTitle_ = _f;
}
// 获取标题字体
xlnt::font XlsxWrite::fontTitle( ) const {
    return fontTitle_;
}

// 设置单元格边框格式
void XlsxWrite::borderCell(const xlnt::border &_b) {
    borderCell_ = _b;
}
// 获取单元格边框格式
xlnt::border XlsxWrite::borderCell( ) const {
    return borderCell_;
}

// 设置对齐方式
void XlsxWrite::alignment(const xlnt::alignment &_a) {
    alignment_ = _a;
}
// 获取对齐方式
xlnt::alignment XlsxWrite::alignment( ) const {
    return alignment_;
}

// 设置是否有标题
void XlsxWrite::hasTitle(const bool _t) {
    hasTitle_ = _t;
}
// 返回是否有标题
bool XlsxWrite::hasTitlt( ) const {
    return hasTitle_;
}

// 设置是否有表头
void XlsxWrite::hasHeader(const bool _h) {
    hasHeader_ = _h;
}
// 返回是否有表头
bool XlsxWrite::hasHeader( ) const {
    return hasHeader_;
}

// 设置标题
void XlsxWrite::title(const chstring &_t) {
    title_ = _t;
}
// 获取标题
chstring XlsxWrite::title( ) const {
    return title_;
}

// 设置表格
void XlsxWrite::sheet(const myTable< chstring > &_s) {
    sheet_ = _s;
}
// 返回表格
myTable< chstring > XlsxWrite::sheet( ) const {
    return sheet_;
}

// 设置写入的路径
void XlsxWrite::path(const chstring &_p) {
    path_ = _p;
}
// 获取写入路径
chstring XlsxWrite::path( ) const {
    return path_;
}

// 设置正文行高
void XlsxWrite::heightRegular(const double _h) {
    heightRegular_ = _h;
}
// 获取正文行高
double XlsxWrite::heightRegular( ) const {
    return heightRegular_;
}

// 设置标题行高
void XlsxWrite::heightTitle(const double _h) {
    heightTitle_ = _h;
}
// 获取标题行高
double XlsxWrite::heightTitle( ) const {
    return heightTitle_;
}

// 设置表头行高
void XlsxWrite::heightHeader(const double _h) {
    heightHeader_ = _h;
}
// 获取表头行高
double XlsxWrite::heightHeader( ) const {
    return heightHeader_;
}

// 检查是否可写
bool XlsxWrite::can_write( ) const {
    if (sheet_.empty( ) || path_.empty( )) return false;
    if (hasTitle_ && title_.empty( )) return false;
    return true;
}

// 写入xlsx
bool XlsxWrite::write( ) const {
    if (!can_write( )) return false;
    myTable< std::string > sh = chstring::get_u8table(sheet_);
    encoding::repair_sheet_utf8_invalidity(sh);

    xlnt::workbook wb;
    make_workbook(wb, sh);    // 制表

    // 保存
    try {
        wb.save(path_.u8string( ));
    } catch (const std::exception &e) {
        std::cerr << e.what( ) << std::endl;
    }
}

// 制作workbook
void XlsxWrite::make_workbook(xlnt::workbook &wb, const myTable< std::string > &sh) const {
    xlnt::worksheet ws = wb.active_sheet( );
    ws.title("Sheet1");

    size_t maxCol = 1;    // sheet中列的数量
    // 逐行逐列写入
    for (size_t r = 0; r < sh.size( ); r++) {
        for (size_t c = 0; c < sh[r].size( ); c++) {
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).value(sh[r][c]);
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).font(fontRegular_);
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).alignment(alignment_);
            ws.cell(xlnt::cell_reference(c + 1, r + 1)).border(borderCell_);
        }
        if (maxCol < sh[r].size( ))
            maxCol = sh[r].size( );
        ws.row_properties(r + 1).height        = heightRegular_;
        ws.row_properties(r + 1).custom_height = true;
    }
    // 调整列宽
    for (size_t i = 1; i <= maxCol && i <= widths_.size( ); i++) {
        ws.column_properties(i).width        = widths_[i - 1];
        ws.column_properties(i).custom_width = true;
    }
    if (hasHeader_) {
        for (size_t c = 0; c < sh[0].size( ); c++)
            ws.cell(xlnt::cell_reference(c + 1, 1)).font(fontHeader_);
        ws.row_properties(1).height        = heightHeader_;
        ws.row_properties(1).custom_height = true;
    }
    if (hasTitle_) {
        ws.insert_rows(1, 1);

        // 合并单元格
        std::string end_col;
        int         temp = maxCol;
        while (temp > 0) {
            int remainder = (temp - 1) % 26;
            end_col       = char('A' + remainder) + end_col;
            temp          = (temp - 1) / 26;
        }
        // 构建合并单元格的范围字符串
        std::string merRange = "A1:" + end_col + "1";
        ws.merge_cells(merRange);

        ws.row_properties(1).height        = heightTitle_;    // 40pt
        ws.row_properties(1).custom_height = true;
        ws.cell("A1").value(title_.u8string( ));
        ws.cell("A1").font(fontTitle_);
        ws.cell("A1").alignment(alignment_);
    }
}

/* ================================================================================================================ */
/* ================================================================================================================ */

XlsxLoad::XlsxLoad(const chstring &_p)
    : path_(_p) {
    std::cout << "load xlsx file: \"" << path_ << "\"";
    wb_.load(path_.u8string( ));
    auto ws = wb_.active_sheet( );

    // 按行遍历
    for (auto row : ws.rows(false)) {
        // 保存当前行所有单元格文本的临时向量
        myList< chstring > aSingleRow;
        // 遍历当前行的每个单元格
        for (auto cell : row) {
            // cell.to_string() 把数字、日期、公式等统一转为字符串
            aSingleRow.push_back(cell.to_string( ));
        }
        sheet_.push_back(aSingleRow);
    }

    std::cout << " - Done!" << std::endl;
}

void XlsxLoad::path(const chstring &_p) {
    path_ = _p;
}

chstring XlsxLoad::path( ) const {
    return path_;
}

// 获取解析得到的表格
myTable< chstring > XlsxLoad::get_sheet( ) const {
    return sheet_;
}

}    // namespace xlsx
