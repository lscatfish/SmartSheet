#pragma once

#ifndef EXCEL_H
#define EXCEL_H

#include <basic.hpp>
#include <chstring.hpp>
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
    const bool                        _bold            = false,
    const bool                        _italic          = false,
    const xlnt::color                &_color           = xlnt::color::black( ),
    const xlnt::font::underline_style _underline_style = xlnt::font::underline_style::none,
    const bool                        _strikethrough   = true);

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
    const xlnt::border::border_property _start  = set_border_property( ),
    const xlnt::border::border_property _end    = set_border_property( ),
    const xlnt::border::border_property _top    = set_border_property( ),
    const xlnt::border::border_property _bottom = set_border_property( ));

/* ================================================================================================================ */
/* ================================================================================================================ */

// 写入xlsx
class XlsxWrite {
public:
    XlsxWrite(
        const myTable< chstring > &_sh,
        const xlnt::border        &_border      = set_bolder( ),
        const xlnt::font          &_fontRegular = set_font(U8C(u8"宋体"), 14),
        const xlnt::alignment     &_align       = set_alignment( ),
        bool                       _hasTitle    = false,
        const chstring            &_title       = "",
        const xlnt::font          &_fontTitle   = set_font(U8C(u8"方正小标宋简体"), 24),
        bool                       _hasHeader   = false,
        const xlnt::font          &_fontHeader  = set_font(U8C(u8"宋体"), 24, true));
    XlsxWrite( );
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
    bool hasTitlt( ) const;

    // 设置是否有表头
    void hasHeader(const bool _h);
    // 返回是否有表头
    bool hasHeader( ) const;

    // 设置标题
    void title(const chstring &_t);
    // 获取标题
    chstring title( ) const;

    //设置表格
    void sheet(const myTable< chstring > &_s);
    //返回表格
    myTable< chstring > sheet( ) const;



private:
    myTable< chstring > sheet_;    // 正式的表格
    chstring            title_;

    xlnt::font      fontRegular_;    // 正文字体
    xlnt::font      fontTitle_;      // 标题字体
    xlnt::font      fontHeader_;     // 表头字体
    xlnt::border    borderCell_;     // 一个单元格的边框
    xlnt::alignment alignment_;      // 对齐方式
    bool            hasTitle_;       // 是否有标题
    bool            hasHeader_;      // 是否有表头
};

/* ================================================================================================================ */
/* ================================================================================================================ */

// 解析xlsx
class XlsxLoad {
public:
    XlsxLoad( )  = default;
    ~XlsxLoad( ) = default;

private:
};


}    // namespace xlsx


#endif    // !EXCEL_H
