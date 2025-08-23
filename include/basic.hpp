
#pragma once

#ifndef BASIC_HPP
#define BASIC_HPP

#include <ppocr_API.h>
#include <string>
#include <type_traits>
#include <vector>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-global.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-rectangle.h>
#include <poppler/cpp/poppler-version.h>

#define U8C(s) reinterpret_cast< const char * >(s)

// 表格
template < typename _T >
using table = std::vector< std::vector< _T > >;

// 一行（列）
template < typename _T >
using list = std::vector< _T >;


// 定义二维点类型
struct GridPoint {
    double x;    // x坐标
    double y;    // y坐标
    GridPoint( ) {
        y = 0;
        x = 0;
    };
    GridPoint(double _x, double _y) {
        x = _x;
        y = _y;
    };
    GridPoint(const GridPoint &p) {
        *this = p;
    };
};

// 一个表格的单元格，包含在图片中的四个顶点的坐标
struct CELL {
    GridPoint   top_left;
    GridPoint   top_right;
    GridPoint   bottom_right;
    GridPoint   bottom_left;
    GridPoint   corePoint;    // box的中心点坐标(x,y)
    std::string text;         // 储存的文字
    bool        ifSelect;     // 是否已被选中

    // 基于 OCRPredictResult 构造
    CELL(ppocr::OCRPredictResult _ocrPR) {
        this->top_left     = GridPoint(_ocrPR.box[0][0], _ocrPR.box[0][1]);
        this->top_right    = GridPoint(_ocrPR.box[1][0], _ocrPR.box[1][1]);
        this->bottom_right = GridPoint(_ocrPR.box[2][0], _ocrPR.box[2][1]);
        this->bottom_left  = GridPoint(_ocrPR.box[3][0], _ocrPR.box[3][1]);
        this->text         = _ocrPR.text;
        this->ifSelect     = false;
        this->corePoint.x  = (_ocrPR.box[0][0] + _ocrPR.box[1][0] + _ocrPR.box[2][0] + _ocrPR.box[3][0]) / 4.0;
        this->corePoint.y  = (_ocrPR.box[0][1] + _ocrPR.box[1][1] + _ocrPR.box[2][1] + _ocrPR.box[3][1]) / 4.0;
    };

    /*
     * @brief 基于顶点构造
     * @param _top_left
     * @param _top_right
     * @param _bottom_right
     * @param _bottom_left
     */
    CELL(GridPoint _top_left, GridPoint _top_right, GridPoint _bottom_right, GridPoint _bottom_left) {
        this->top_left     = _top_left;
        this->top_right    = _top_right;
        this->bottom_left  = _bottom_left;
        this->bottom_right = _bottom_right;
        this->text         = "";
        this->ifSelect     = false;
        this->corePoint.x  = (this->top_left.x + this->top_right.x + this->bottom_left.x + this->bottom_right.x) / 4.0;
        this->corePoint.y  = (this->top_left.y + this->top_right.y + this->bottom_left.y + this->bottom_right.y) / 4.0;
    };

    /*
     * @brief 基于两点构造
     * @param _top_left
     * @param _bottom_right
     */
    CELL(GridPoint _top_left, GridPoint _bottom_right) {
        this->bottom_right = _bottom_right;
        this->top_left     = _top_left;
        this->top_right    = GridPoint(_bottom_right.x, _top_left.y);
        this->bottom_left  = GridPoint(_top_left.x, _bottom_right.y);
        this->text         = "";
        this->ifSelect     = false;
        this->corePoint.x  = (this->top_left.x + this->top_right.x + this->bottom_left.x + this->bottom_right.x) / 4.0;
        this->corePoint.y  = (this->top_left.y + this->top_right.y + this->bottom_left.y + this->bottom_right.y) / 4.0;
    };

    /*
    * @brief 基于poppler::text_box构造
    */
    CELL(const poppler::text_box& tb) {
        *this = CELL(GridPoint(tb.bbox( ).left( ), tb.bbox( ).top( )), GridPoint(tb.bbox( ).right( ), tb.bbox( ).bottom( )));
        for (const auto &c : tb.text( ).to_utf8( )) {
            this->text.push_back(c);
        }
    }

    /*
     * @brief 检查对象是否被包含在cell中(为img设计，y轴向下)
     * @param bigger 输入的可以包含对象的一个cell
     * @return 对象被包含在bigger中返回true，否则返回false
     */
    bool is_contained_for_img(const CELL &bigger) const {
        return (bigger.top_left.x < this->corePoint.x
                && bigger.bottom_right.x > this->corePoint.x
                && bigger.top_left.y < this->corePoint.y
                && bigger.bottom_right.y > this->corePoint.y);
    };
};




#endif    // !BASIC_H
