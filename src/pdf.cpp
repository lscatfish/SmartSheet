#include <vector>
#include <string>
#include <memory>
#include <fpdfview.h>
#include <fpdf_text.h>
#include <fpdf_edit.h>

// 确保链接PDFium库
#pragma comment(lib, "pdfium.lib")

namespace pdf {

/**
 * 坐标点结构
 */
struct Point {
    float x;    // X坐标
    float y;    // Y坐标
    Point(float x_, float y_)
        : x(x_), y(y_) {}
};

/**
 * 矩形区域结构
 */
struct Rect {
    float left;      // 左边界
    float top;       // 上边界
    float right;     // 右边界
    float bottom;    // 下边界
    Rect(float l, float t, float r, float b)
        : left(l), top(t), right(r), bottom(b) {}
};

/**
 * 文本块结构
 */
struct TextBlock {
    std::string content;    // 文本内容
    Rect        rect;       // 文本块位置
    TextBlock(const std::string &c, const Rect &r)
        : content(c), rect(r) {}
};

/**
 * 表格线结构
 */
struct TableLine {
    Point start;            // 起始点
    Point end;              // 结束点
    bool  is_horizontal;    // 是否水平线
    TableLine(const Point &s, const Point &e)
        : start(s), end(e), is_horizontal(s.y == e.y) {}
};

/**
 * PDF解析器类
 */
class Parser {
private:
    FPDF_DOCUMENT m_doc;         // PDF文档句柄
    bool          m_is_valid;    // 文档是否有效

    // 释放PDF文档资源的自定义删除器
    struct DocumentDeleter {
        void operator( )(FPDF_DOCUMENT doc) const {
            if (doc) FPDF_CloseDocument(doc);
        }
    };

    // 释放页面资源的自定义删除器
    struct PageDeleter {
        void operator( )(FPDF_PAGE page) const {
            if (page) FPDF_ClosePage(page);
        }
    };

public:
    /**
     * 构造函数
     * @param file_path PDF文件路径
     */
    Parser(const std::string &file_path)
        : m_doc(nullptr), m_is_valid(false) {
        // 初始化PDFium库
        FPDF_InitLibrary( );

        // 加载PDF文档
        m_doc      = FPDF_LoadDocument(file_path.c_str( ), nullptr);
        m_is_valid = (m_doc != nullptr);
    }

    /**
     * 析构函数
     */
    ~Parser( ) {
        if (m_doc) {
            FPDF_CloseDocument(m_doc);
        }
        FPDF_DestroyLibrary( );
    }

    /**
     * 检查文档是否有效
     * @return 有效返回true，否则返回false
     */
    bool is_valid( ) const {
        return m_is_valid;
    }

    /**
     * 获取PDF页数
     * @return 页数，失败返回0
     */
    int get_page_count( ) const {
        if (!m_is_valid) return 0;
        return FPDF_GetPageCount(m_doc);
    }

    /**
     * 提取指定页面的文本块
     * @param page_index 页面索引（从0开始）
     * @return 文本块列表
     */
    std::vector< TextBlock > extract_text_blocks(int page_index) {
        std::vector< TextBlock > blocks;
        if (!m_is_valid || page_index < 0 || page_index >= get_page_count( )) {
            return blocks;
        }
        // 加载页面
        std::unique_ptr< FPDF_PAGE__, PageDeleter > page(FPDF_LoadPage(m_doc, page_index));
        if (!page) return blocks;

        // 加载文本内容
        FPDF_TEXTPAGE text_page = FPDFText_LoadPage(page.get( ));
        if (!text_page) return blocks;

        // 获取字符总数
        int char_count = FPDFText_GetCharCount(text_page);
        if (char_count <= 0) {
            FPDFText_ClosePage(text_page);
            return blocks;
        }

        // 提取文本块（简单合并相邻字符）
        std::string current_text;
        Rect        current_rect(0, 0, 0, 0);
        bool        is_first_char = true;

        for (int i = 0; i < char_count; ++i) {
            // 获取字符信息
            wchar_t char_code;
            FPDFText_GetChar(text_page, i, &char_code);

            // 跳过控制字符
            if (char_code <= 32) {
                if (!current_text.empty( )) {
                    blocks.emplace_back(current_text, current_rect);
                    current_text.clear( );
                    is_first_char = true;
                }
                continue;
            }

            // 获取字符位置
            double left, top, right, bottom;
            if (!FPDFText_GetCharBox(text_page, i, &left, &top, &right, &bottom)) {
                continue;
            }

            // 转换为float坐标
            Rect char_rect((float)left, (float)top, (float)right, (float)bottom);

            // 初始化或更新当前文本块
            if (is_first_char) {
                current_text  = std::wstring(1, char_code);
                current_rect  = char_rect;
                is_first_char = false;
            } else {
                current_text += std::wstring(1, char_code);
                current_rect.left   = std::min(current_rect.left, char_rect.left);
                current_rect.top    = std::min(current_rect.top, char_rect.top);
                current_rect.right  = std::max(current_rect.right, char_rect.right);
                current_rect.bottom = std::max(current_rect.bottom, char_rect.bottom);
            }
        }

        // 添加最后一个文本块
        if (!current_text.empty( )) {
            blocks.emplace_back(current_text, current_rect);
        }

        // 清理资源
        FPDFText_ClosePage(text_page);
        return blocks;
    }

    /**
     * 提取指定页面的表格线
     * @param page_index 页面索引（从0开始）
     * @return 表格线列表
     */
    std::vector< TableLine > extract_table_lines(int page_index) {
        std::vector< TableLine > lines;
        if (!m_is_valid || page_index < 0 || page_index >= get_page_count( )) {
            return lines;
        }

        // 加载页面
        std::unique_ptr< FPDF_PAGE__, PageDeleter > page(FPDF_LoadPage(m_doc, page_index));
        if (!page) return lines;

        // 获取页面对象数量
        int obj_count = FPDFPage_GetObjectCount(page.get( ));
        for (int i = 0; i < obj_count; ++i) {
            // 获取页面对象
            FPDF_PAGEOBJ obj = FPDFPage_GetObject(page.get( ), i);
            if (!obj) continue;

            // 检查是否为路径对象（表格线通常是路径）
            if (FPDFPageObj_GetType(obj) != FPDF_PAGEOBJ_PATH) {
                continue;
            }

            // 解析路径数据
            int path_size = FPDFPath_GetPointCount(obj);
            if (path_size < 2) continue;

            // 获取路径点坐标
            std::vector< Point > points;
            for (int j = 0; j < path_size; ++j) {
                double x, y;
                if (FPDFPath_GetPoint(obj, j, &x, &y)) {
                    points.emplace_back((float)x, (float)y);
                }
            }

            // 提取线段（简单处理直线段）
            for (size_t j = 1; j < points.size( ); ++j) {
                lines.emplace_back(points[j - 1], points[j]);
            }
        }

        return lines;
    }

    /**
     * 计算表格线的交点
     * @param lines 表格线列表
     * @return 交点列表
     */
    std::vector< Point > calculate_intersections(const std::vector< TableLine > &lines) {
        std::vector< Point > intersections;

        // 分离水平线和垂直线
        std::vector< TableLine > horizontal, vertical;
        for (const auto &line : lines) {
            if (line.is_horizontal) {
                horizontal.push_back(line);
            } else {
                vertical.push_back(line);
            }
        }

        // 计算水平线与垂直线的交点
        for (const auto &hline : horizontal) {
            for (const auto &vline : vertical) {
                // 水平线：y = h_y，垂直线：x = v_x
                float h_y = hline.start.y;
                float v_x = vline.start.x;

                // 检查交点是否在线段范围内
                bool in_h = (v_x >= std::min(hline.start.x, hline.end.x) && v_x <= std::max(hline.start.x, hline.end.x));
                bool in_v = (h_y >= std::min(vline.start.y, vline.end.y) && h_y <= std::max(vline.start.y, vline.end.y));

                if (in_h && in_v) {
                    intersections.emplace_back(v_x, h_y);
                }
            }
        }

        return intersections;
    }
};
}    // namespace pdf