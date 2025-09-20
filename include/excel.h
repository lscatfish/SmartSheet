#pragma once

#ifndef EXCEL_H
#define EXCEL_H

/*
 * @file
 * 处理xlsx的类
 * 为了适配xlnt
 */

#include <basic.hpp>
#include <chstring.hpp>
#include <high.h>
#include <iostream>
#include <string>
#include <xlnt/xlnt.hpp>

// 用于解析xlsx的类
namespace xlsx {

extern xlnt::font                    stdFontRegular_attSheet;
extern xlnt::font                    stdFontTitle_attSheet;
extern xlnt::font                    stdFontRegular_sttSheet;
extern xlnt::font                    stdFontHeader_sttSheet;
extern xlnt::font                    stdFontTitle_sttSheet;
extern xlnt::border::border_property stdBorderProperty;
extern xlnt::border                  stdBorder;
extern xlnt::alignment               autoAlignment;

/*
 * @brief 设置字体
 * @param _name 字体名称
 * @param _fsize 字体大小（磅）
 * @param _bold 是否为粗体
 * @param _italic 是否为斜体
 * @param _strikethrough 是否取消删除线
 * @param _color 字体颜色
 * @param _underline_style 是否有删除线
 */
xlnt::font set_font(
    const chstring                   &_name,
    const size_t                     &_fsize,
    const bool                        _bold            = false,
    const bool                        _italic          = false,
    const xlnt::color                &_color           = xlnt::color::black( ),
    const xlnt::font::underline_style _underline_style = xlnt::font::underline_style::none,
    const bool                        _strikethrough   = false);

/*
 * @brief 设置对齐方式
 * @param _h 水平对齐方式
 * @param _v 垂直对齐方式
 * @param _wrap 是否自动换行
 * @param _rotation 文本旋转角度
 */
xlnt::alignment set_alignment(
    const xlnt::horizontal_alignment _h        = xlnt::horizontal_alignment::center,
    const xlnt::vertical_alignment   _v        = xlnt::vertical_alignment::center,
    const bool                       _wrap     = false,
    const int                        _rotation = 0);

/*
 * @brief 设置边框的属性
 * @param _style 边框的线性
 * @param _color 边框的颜色
 */
xlnt::border::border_property set_border_property(
    const xlnt::border_style _style = xlnt::border_style::thin,
    const xlnt::color        _color = xlnt::color::black( ));

/*
 * @brief 设置单元格边框
 * @param _start 左
 * @param _end 右
 * @param _top 上
 * @param _bottom 下
 */
xlnt::border set_bolder(
    const xlnt::border::border_property _start  = stdBorderProperty,
    const xlnt::border::border_property _end    = stdBorderProperty,
    const xlnt::border::border_property _top    = stdBorderProperty,
    const xlnt::border::border_property _bottom = stdBorderProperty);

/* ================================================================================================================ */
/* ================================================================================================================ */

// 写入xlsx
class XlsxWrite {
public:
    XlsxWrite(
        const myTable< chstring > &_sh,
        const chstring            &_path,
        const double               _heightRegular = 24,
        const myList< double >    &_widths        = myList< double >{ },
        const xlnt::border        &_border        = stdBorder,
        const xlnt::font          &_fontRegular   = stdFontRegular_sttSheet,
        const xlnt::alignment     &_align         = autoAlignment,
        bool                       _hasTitle      = false,
        const chstring            &_title         = "",
        const xlnt::font          &_fontTitle     = stdFontTitle_sttSheet,
        const double               _heightTitle   = 40,
        bool                       _hasHeader     = false,
        const xlnt::font          &_fontHeader    = stdFontHeader_sttSheet,
        const double               _heightHeader  = 24);
    XlsxWrite( );
    XlsxWrite(const chstring &_p);
    ~XlsxWrite( ) = default;

    // 设置正文字体
    void fontRegular(const xlnt::font &_f);
    // 获取正文字体
    xlnt::font fontRegular( ) const;

    // 设置表头字体
    void fontHeader(const xlnt::font &_f);
    // 获取正文字体
    xlnt::font fontHeader( ) const;

    // 设置标题字体
    void fontTitle(const xlnt::font &_f);
    // 获取标题字体
    xlnt::font fontTitle( ) const;

    // 设置单元格边框格式
    void borderCell(const xlnt::border &_b);
    // 获取单元格边框格式
    xlnt::border borderCell( ) const;

    // 设置对齐方式
    void alignment(const xlnt::alignment &_a);
    // 获取对齐方式
    xlnt::alignment alignment( ) const;

    // 设置是否有标题
    void hasTitle(const bool _t);
    // 返回是否有标题
    bool hasTitle( ) const;

    // 设置是否有表头
    void hasHeader(const bool _h);
    // 返回是否有表头
    bool hasHeader( ) const;

    // 设置标题
    void title(const chstring &_t);
    // 获取标题
    chstring title( ) const;

    // 设置表格内容
    void sheet(const myTable< chstring > &_s);
    // 返回表格内容
    myTable< chstring > sheet( ) const;

    // 设置写入的路径
    void path(const chstring &_p);
    // 获取写入路径
    chstring path( ) const;

    // 设置正文行高
    void heightRegular(const double _h);
    // 获取正文行高
    double heightRegular( ) const;

    // 设置标题行高
    void heightTitle(const double _h);
    // 获取标题行高
    double heightTitle( ) const;

    // 设置表头行高
    void heightHeader(const double _h);
    // 获取表头行高
    double heightHeader( ) const;

    // 设置行宽
    void widths(const myList< double > &_w);
    // 获取行宽
    myList< double > widths( ) const;

    // 检查是否可写
    bool can_write( ) const;

    // 写入xlsx
    bool write( ) const;

private:
    myTable< chstring > sheet_;            // 正式的表格
    chstring            title_;            // 标题
    xlnt::font          fontRegular_;      // 正文字体
    xlnt::font          fontTitle_;        // 标题字体
    xlnt::font          fontHeader_;       // 表头字体
    xlnt::border        borderCell_;       // 一个单元格的边框
    chstring            path_;             // 路径
    xlnt::alignment     alignment_;        // 对齐方式
    bool                hasTitle_;         // 是否有标题
    bool                hasHeader_;        // 是否有表头
    double              heightRegular_;    // 正文行高
    double              heightHeader_;     // 表头的行高
    double              heightTitle_;      // 标题行高
    myList< double >    widths_;           // 列宽的列表

    // 制作workbook
    void make_workbook(xlnt::workbook &wb, const myTable< std::string > &sh) const;
};

/* ================================================================================================================ */
/* ================================================================================================================ */

// 解析xlsx
class XlsxLoad {
public:
    XlsxLoad(const chstring &_p);
    ~XlsxLoad( ) = default;

    void     path(const chstring &_p);
    chstring path( ) const;

    // 获取解析得到的表格
    myTable< chstring > get_sheet( ) const;

private:
    chstring            path_;     // 文件路径
    myTable< chstring > sheet_;    // 解析出的表格
    xlnt::workbook      wb_;       // 内置解析得到的表格
};

/* ================================================================================================================ */
/* ================================================================================================================ */
//
//// 使用请采用此类
//class DefXlsx {
//public:
//    // 调用方式
//    enum class xType {
//        write = 0,
//        load
//    };
//
//    /*
//     * @param _xt 调用方式
//     * @param _p 地址
//     */
//    DefXlsx(const xType _xt, const chstring &_p);
//
//    // @brief 默认采用写方式
//    DefXlsx(const myTable< chstring > &_sh,
//            const chstring            &_path,
//            const double               _heightRegular = 24,
//            const myList< double >    &_widths        = myList< double >{ },
//            const xlnt::border        &_border        = stdBorder,
//            const xlnt::font          &_fontRegular   = stdFontRegular_sttSheet,
//            const xlnt::alignment     &_align         = autoAlignment,
//            bool                       _hasTitle      = false,
//            const chstring            &_title         = "",
//            const xlnt::font          &_fontTitle     = stdFontTitle_sttSheet,
//            const double               _heightTitle   = 40,
//            bool                       _hasHeader     = false,
//            const xlnt::font          &_fontHeader    = stdFontHeader_sttSheet,
//            const double               _heightHeader  = 24);
//
//    ~DefXlsx( );
//
//    // 设置正文字体
//    void fontRegular(const xlnt::font &_f);
//    // 获取正文字体
//    xlnt::font fontRegular( ) const;
//
//    // 设置表头字体
//    void fontHeader(const xlnt::font &_f);
//    // 获取正文字体
//    xlnt::font fontHeader( ) const;
//
//    // 设置标题字体
//    void fontTitle(const xlnt::font &_f);
//    // 获取标题字体
//    xlnt::font fontTitle( ) const;
//
//    // 设置单元格边框格式
//    void borderCell(const xlnt::border &_b);
//    // 获取单元格边框格式
//    xlnt::border borderCell( ) const;
//
//    // 设置对齐方式
//    void alignment(const xlnt::alignment &_a);
//    // 获取对齐方式
//    xlnt::alignment alignment( ) const;
//
//    // 设置是否有标题
//    void hasTitle(const bool _t);
//    // 返回是否有标题
//    bool hasTitle( ) const;
//
//    // 设置是否有表头
//    void hasHeader(const bool _h);
//    // 返回是否有表头
//    bool hasHeader( ) const;
//
//    // 设置标题
//    void title(const chstring &_t);
//    // 获取标题
//    chstring title( ) const;
//
//    // 设置表格内容
//    void sheet(const myTable< chstring > &_s);
//    // 返回表格内容
//    myTable< chstring > sheet( ) const;
//
//    // 设置写入的路径
//    void path(const chstring &_p);
//    // 获取写入路径
//    chstring path( ) const;
//
//    // 设置正文行高
//    void heightRegular(const double _h);
//    // 获取正文行高
//    double heightRegular( ) const;
//
//    // 设置标题行高
//    void heightTitle(const double _h);
//    // 获取标题行高
//    double heightTitle( ) const;
//
//    // 设置表头行高
//    void heightHeader(const double _h);
//    // 获取表头行高
//    double heightHeader( ) const;
//
//    // 设置行宽
//    void widths(const myList< double > &_w);
//    // 获取行宽
//    myList< double > widths( ) const;
//
//    // 检查是否可写
//    bool can_write( ) const;
//
//    // 写入xlsx
//    bool write( ) const;
//
//private:
//    XlsxWrite *writter = nullptr;    // 写方法
//    XlsxLoad  *loader  = nullptr;    // 读取方法
//    chstring   path_;                // 路径
//    xType      xtype_;               // 调用defxlsx的方式
//};

/* ================================================================================================================ */
/* ================================================================================================================ */

}    // namespace xlsx


#endif    // !EXCEL_H
