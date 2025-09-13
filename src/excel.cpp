
#include <basic.hpp>
#include <chstring.hpp>
#include <excel.h>
#include <high.h>
#include <iostream>
#include <xlnt/xlnt.hpp>


// 用于解析xlsx的类
namespace xlsx {

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


// XlsxWrite::XlsxWrite(
//     const myTable< chstring > &_sh,
//     const chstring            &_title)
//     : sheet_(_sh), title_(_title) {
// }



XlsxWrite::XlsxWrite(
    const myTable< chstring > &_sh,
    const xlnt::border        &_border,
    const xlnt::font          &_fontRegular,
    const xlnt::alignment     &_align,
    bool                       _hasTitle,
    const chstring            &_title,
    const xlnt::font          &_fontTitle,
    bool                       _hasHeader,
    const xlnt::font          &_fontHeader)
    : sheet_(_sh),
      title_(_title),
      borderCell_(_border),
      fontRegular_(_fontRegular),
      alignment_(_align),
      fontTitle_(_fontTitle),
      fontHeader_(_fontHeader),
      hasTitle_(_hasTitle),
      hasHeader_(_hasHeader) {}

XlsxWrite::XlsxWrite( ) {
    borderCell_  = set_bolder( );
    fontRegular_ = set_font(U8C(u8"宋体"), 14);
    alignment_   = set_alignment( );
    hasTitle_    = false;
    title_       = "";
    fontTitle_   = set_font(U8C(u8"方正小标宋简体"), 24);
    hasHeader_   = false;
    fontHeader_  = set_font(U8C(u8"宋体"), 24, true);
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

}    // namespace xlsx
