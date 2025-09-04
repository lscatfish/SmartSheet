
/* ====================================================================================================== *
 *
 * 考虑到存在头文件的互相调用的问题，这个头文件的设计是不安全的
 *
 * 此文件的设计初衷是为了存放一些基本的类型定义和常用的模板函数，以供其他文件调用
 *
 * 作者：lscatfish
 *
 * ======================================================================================================= */

#pragma once

#ifndef BASIC_HPP
#define BASIC_HPP

#include <cstdlib>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-global.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-rectangle.h>
#include <poppler/cpp/poppler-version.h>
#include <ppocr_API.h>
#include <string>
#include <type_traits>
#include <vector>

#define U8C(s) reinterpret_cast< const char * >(s)

// 表格
template < typename _T >
using table = std::vector< std::vector< _T > >;    // 如果冲突，请封装到namespace中，或者改名为 MyTable

// 一行（列）
template < typename _T >
using list = std::vector< _T >;    // 如果冲突，请封装到namespace中，或者改名为 MyList

// 模板函数：对 vector<T>按指定成员变量排序
// 参数：容器引用、比较函数（决定排序规则和依据的成员）
// @note [@lscatfish] ***多此一举***却似用筷子夹勺子喝粥***
template < typename _T, typename Compare >
void sort_my_list(list< _T > &vec, Compare comp) {
    std::sort(vec.begin( ), vec.end( ), comp);
}
// @note [@lscatfish] ***多此一举***却似用筷子夹勺子喝粥***

namespace pdf {
// 定义线段结构体
struct LineSegment {
public:
    // 线段的类型
    enum class Type {
        Others = 0,    // 其他类型
        Horizontal,    // 水平线
        Vertical       // 竖线
    };
    double x1, y1, x2, y2;
    Type   t;

    LineSegment(double _x1, double _y1, double _x2, double _y2) {
        x1 = _x1;
        x2 = _x2;
        y1 = _y1;
        y2 = _y2;
        if (std::abs(y1 - y2) < 1.0) {
            t = Type::Horizontal;
            // 向两端延长
            if (x1 < x2) {
                x1 = x1 - 1;
                x2 = x2 + 1;
                if (x1 < 0) x1 = 0;
            } else if (x1 > x2) {
                x1 = x1 + 1;
                x2 = x2 - 1;
                if (x2 < 0) x2 = 0;
            }
        } else if (std::abs(x1 - x2) < 1.0) {
            t = Type::Vertical;
            if (y1 < y2) {
                y1 = y1 - 1;
                y2 = y2 + 1;
                if (y1 < 0) y1 = 0;
            } else if (y1 > y2) {
                y1 = y1 + 1;
                y2 = y2 - 1;
                if (y2 < 0) y2 = 0;
            }
        } else {
            t = Type::Others;
        }
    };
};
}    // namespace pdf

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
        if (x < 0) x = 0;
        if (y < 0) y = 0;
    };
    GridPoint(const GridPoint &p) {
        *this = p;
    };

    bool operator==(const GridPoint &b) const {
        return this->x == b.x && this->y == b.y;
    }
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
    CELL(const poppler::text_box &tb, double _h) {
        GridPoint p1(tb.bbox( ).x( ), _h - tb.bbox( ).y( ));
        GridPoint p2(tb.bbox( ).x( ) + tb.bbox( ).width( ), _h - (tb.bbox( ).y( ) - tb.bbox( ).height( )));
        *this = CELL(p1, p2);
        for (const auto &c : tb.text( ).to_utf8( )) {
            this->text.push_back(c);
        }
    }

    /*
     * @brief 基于CELL构造
     * @param CELL的引用
     * @param deltaH 增加的高度
     */
    CELL(const CELL &a, double deltaH) {
        GridPoint p1(a.top_left.x, a.top_left.y + deltaH);
        GridPoint p2(a.bottom_right.x, a.bottom_right.y + deltaH);
        *this      = CELL(p1, p2);
        this->text = a.text;
    }

    /*
     * @brief 基于pdf::LineSegments构造
     */
    CELL(const pdf::LineSegment &_top, const pdf::LineSegment &_bottom, const pdf::LineSegment &_left, const pdf::LineSegment &_right) {
        this->top_left     = GridPoint((_left.x1 + _left.x2) / 2, (_top.y1 + _top.y2) / 2);
        this->top_right    = GridPoint((_right.x1 + _right.x2) / 2, (_top.y1 + _top.y2) / 2);
        this->bottom_left  = GridPoint((_left.x1 + _left.x2) / 2, (_bottom.y1 + _bottom.y2) / 2);
        this->bottom_right = GridPoint((_right.x1 + _right.x2) / 2, (_bottom.y1 + _bottom.y2) / 2);
        this->corePoint.x  = (this->top_left.x + this->top_right.x + this->bottom_left.x + this->bottom_right.x) / 4.0;
        this->corePoint.y  = (this->top_left.y + this->top_right.y + this->bottom_left.y + this->bottom_right.y) / 4.0;
        this->text         = "";
        this->ifSelect     = false;
    }

    /*
     * @brief 默认构造函数
     */
    CELL( ) {
        this->bottom_left  = GridPoint(0, 0);
        this->bottom_right = GridPoint(0, 0);
        this->top_left     = GridPoint(0, 0);
        this->top_right    = GridPoint(0, 0);
        this->text         = "";
        this->ifSelect     = false;
        this->corePoint    = GridPoint(0, 0);
    };

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

    /*
     * @brief 检查对象是否被包含在cell中(为pdf设计，y轴向上)
     * @param bigger 输入的可以包含对象的一个cell
     * @return 对象被包含在bigger中返回true，否则返回false
     */
    bool is_contained_for_pdf(const CELL &bigger) const {
        return (bigger.top_left.x < this->corePoint.x
                && bigger.bottom_right.x > this->corePoint.x
                && bigger.top_left.y > this->corePoint.y
                && bigger.bottom_right.y < this->corePoint.y);
    }

    /*
     * @brief 检测对象是否与目标下方有重合（为pdf设计，y轴向上）
     * @param stdcell 输入的标准CELL
     * @return 有重合返回true，否则返回false
     */
    /*bool is_graphics_coincide_bottom_for_pdf(const CELL &stdcell) const {
        return this->top_left.y >= stdcell.bottom_right.y;
    }*/

    /* ================================适配STL======================================== */
    bool operator==(const CELL &b) const {
        return this->bottom_left == b.bottom_left
               && this->bottom_right == b.bottom_right
               && this->corePoint == b.corePoint
               && this->ifSelect == b.ifSelect
               && this->text == b.text
               && this->top_left == b.top_left
               && this->top_right == b.top_right;
    }

    // 升序排序y
    struct CompareByCorePointYAsc {
        bool operator( )(const CELL &a, const CELL &b) const {
            return a.corePoint.y < b.corePoint.y;
        }
    };
    // 降序排序y
    struct CompareByCorePointYDesc {
        bool operator( )(const CELL &a, const CELL &b) const {
            return a.corePoint.y > b.corePoint.y;
        }
    };
    // 升序排序y
    struct CompareByCorePointXAsc {
        bool operator( )(const CELL &a, const CELL &b) const {
            return a.corePoint.x < b.corePoint.x;
        }
    };
    // 降序排序y
    struct CompareByCorePointXDesc {
        bool operator( )(const CELL &a, const CELL &b) const {
            return a.corePoint.x > b.corePoint.x;
        }
    };
    /* ================================适配STL======================================== */
};


#endif    // !BASIC_H
