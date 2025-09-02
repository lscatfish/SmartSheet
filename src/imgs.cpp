
#include <algorithm>
#include <basic.hpp>
#include <cmath>
#include <cstdlib>
#include <Encoding.h>
#include <exception>
#include <functional>
#include <helper.h>
#include <imgs.h>
#include <iostream>
#include <map>
#include <numeric>
#include <opencv2/opencv.hpp>
#include <ppocr_API.h>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>


// 解析照片的空间
namespace img {

// 提取水平网格线
cv::Mat GridResult::extract_horizontal_lines(cv::Mat &src) {
    // 创建水平结构元素（水平线检测）
    int     horizontal_size   = src.cols / 30;
    cv::Mat horizontal_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(horizontal_size, 1));

    // 腐蚀和膨胀操作突出水平线
    cv::Mat horizontal;
    cv::erode(src, horizontal, horizontal_kernel);
    cv::dilate(horizontal, horizontal, horizontal_kernel);

    return horizontal;
}

// 提取垂直网格线
cv::Mat GridResult::extract_vertical_lines(cv::Mat &src) {
    // 创建垂直结构元素（垂直线检测）
    int     vertical_size   = src.rows / 30;
    cv::Mat vertical_kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(1, vertical_size));

    // 腐蚀和膨胀操作突出垂直线
    cv::Mat vertical;
    cv::erode(src, vertical, vertical_kernel);
    cv::dilate(vertical, vertical, vertical_kernel);

    return vertical;
}

// 提取水平网格线的y坐标（带长度和边缘过滤）
std::vector< int > GridResult::get_horizontal_lineYs(const cv::Mat &horizontalLines, int edgeThreshold) {
    std::vector< int >                      ys;
    std::vector< std::vector< cv::Point > > contours;

    // 查找轮廓
    findContours(horizontalLines, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // 图像宽度
    int imageWidth = horizontalLines.cols;
    // 横线长度阈值（图片宽度的1/3）
    double minLineLength = imageWidth / 3.0;

    for (const auto &contour : contours) {
        // 计算轮廓的边界框
        cv::Rect rect = cv::boundingRect(contour);

        // 检查线条长度是否达标（横线看宽度）
        if (rect.width > minLineLength) {
            // 取边界框的中间y坐标作为线的y坐标
            int y = rect.y + rect.height / 2;
            ys.push_back(y);
        }
    }

    // 排序并去重
    std::sort(ys.begin( ), ys.end( ));
    auto last = std::unique(ys.begin( ), ys.end( ), [](int a, int b) {
        return std::abs(a - b) < 5;    // 小于5像素的视为同一条线
    });
    ys.erase(last, ys.end( ));

    // 过滤边缘线条（顶部和底部边缘）
    std::vector< int > filteredYs;
    int                imageHeight = horizontalLines.rows;
    for (int y : ys) {
        if (y > edgeThreshold && y < (imageHeight - edgeThreshold)) {
            filteredYs.push_back(y);
        }
    }

    return filteredYs;
}

// 提取垂直网格线的x坐标（带长度和边缘过滤）
std::vector< int > GridResult::get_vertical_lineXs(const cv::Mat &verticalLines, int edgeThreshold) {
    std::vector< int >                      xs;
    std::vector< std::vector< cv::Point > > contours;

    // 查找轮廓
    findContours(verticalLines, contours, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    // 图像高度
    int imageHeight = verticalLines.rows;
    // 竖线长度阈值（图片高度的1/5）
    double minLineLength = imageHeight / 5.0;

    for (const auto &contour : contours) {
        // 计算轮廓的边界框
        cv::Rect rect = cv::boundingRect(contour);

        // 检查线条长度是否达标（竖线看高度）
        if (rect.height > minLineLength) {
            // 取边界框的中间x坐标作为线的x坐标
            int x = rect.x + rect.width / 2;
            xs.push_back(x);
        }
    }

    // 排序并去重
    std::sort(xs.begin( ), xs.end( ));
    auto last = std::unique(xs.begin( ), xs.end( ), [](int a, int b) {
        return std::abs(a - b) < 5;    // 小于5像素的视为同一条线
    });
    xs.erase(last, xs.end( ));

    // 过滤边缘线条（左侧和右侧边缘）
    std::vector< int > filteredXs;
    int                imageWidth = verticalLines.cols;
    for (int x : xs) {
        if (x > edgeThreshold && x < (imageWidth - edgeThreshold)) {
            filteredXs.push_back(x);
        }
    }

    return filteredXs;
}

// 将网格线解析到表格
void GridResult::parse_grid_to_sheet( ) {
    // 按照从左上角的点到右下角的点来遍历

    // 水平线的y坐标（多个有效的y坐标）
    for (size_t j = 0; j < this->horizontalYs_.size( ) - 1; j++) {
        std::vector< CELL > line;
        // 竖线的x坐标(多个有效的x坐标)
        for (size_t i = 0; i < this->verticalXs_.size( ) - 1; i++) {
            // 此cell的坐标
            GridPoint top_left     = GridPoint(this->verticalXs_[i], this->horizontalYs_[j]);
            GridPoint top_right    = GridPoint(this->verticalXs_[i + 1], this->horizontalYs_[j]);
            GridPoint bottom_right = GridPoint(this->verticalXs_[i + 1], this->horizontalYs_[j + 1]);
            GridPoint bottom_left  = GridPoint(this->verticalXs_[i], this->horizontalYs_[j + 1]);
            CELL      cell(top_left, top_right, bottom_right, bottom_left);
            line.push_back(cell);
        }
        this->sheet_.push_back(line);
    }
}

// 填充sheet，填充的时候删除多余的列
void GridResult::fill_sheet(const std::vector< CELL > &inCellLists) {
    if (inCellLists.empty( )) return;
    for (const auto &inCell : inCellLists) {
        for (auto &line : this->sheet_) {
            for (auto &cell : line) {
                if (inCell.is_contained_for_img(cell)) {
                    if (cell.text.size( ) == 0) {
                        cell.text = inCell.text;
                    } else {
                        cell.text = cell.text + inCell.text;
                    }
                }
            }
        }
    }
    // 检测并删除空列、空行
    remove_space_row_col( );
    // 删多了，最后再加一行
    if (sheet_.size( ) != 0 && sheet_[0].size( ) == 3) {
        for (auto &r : sheet_) {
            CELL a;
            r.push_back(a);
        }
    }
}

// 返回空列的索引(降序)
std::vector< size_t > GridResult::get_space_column_indices( ) {
    std::vector< size_t > indices;
    for (size_t col = 0; col < sheet_[0].size( ); col++) {
        bool colSpace = true;    // 此列为空
        for (size_t row = 0; row < sheet_.size( ); row++) {
            if (!sheet_[row][col].text.empty( )) {
                colSpace = false;
                break;
            }
        }
        if (colSpace) {
            indices.push_back(col);
        }
    }
    if (indices.empty( )) {
        return indices;
    } else {
        std::sort(indices.begin( ), indices.end( ), std::greater< size_t >( ));
        return indices;
    }
    return indices;
}

// 返回空行的索引（降序）
std::vector< size_t > GridResult::get_space_row_indices( ) {
    std::vector< size_t > indices;
    for (size_t row = 0; row < sheet_.size( ); row++) {
        bool rowSpace = true;    // 此行为空
        for (size_t col = 0; col < sheet_[row].size( ); col++) {
            if (!sheet_[row][col].text.empty( )) {
                rowSpace = false;
                break;
            }
        }
        if (rowSpace) {
            indices.push_back(row);
        }
    }
    if (indices.empty( )) {
        return indices;
    } else {
        std::sort(indices.begin( ), indices.end( ), std::greater< size_t >( ));
        return indices;
    }
    return indices;
}

// 删除空行空列
void GridResult::remove_space_row_col( ) {
    std::vector< size_t > cols = get_space_column_indices( );
    if (!cols.empty( )) {
        for (auto &aRow : sheet_) {
            for (const auto &index : cols) {
                aRow.erase(aRow.begin( ) + index);
            }
        }
    }

    std::vector< size_t > rows = get_space_row_indices( );
    if (!rows.empty( )) {
        for (const auto &index : rows) {
            sheet_.erase(sheet_.begin( ) + index);
        }
    }
}

// 返回构成的sheet
SHEET GridResult::get_sheet( ) const {
    return this->sheet_;
}

// 返回只有string的表格
table< std::string > GridResult::get_stringSheet( ) const {
    table< std::string > sh;
    for (auto &line : this->sheet_) {
        list< std::string > l;
        for (auto &cell : line) {
            l.push_back(cell.text);
        }
        sh.push_back(l);
    }
    return sh;
}

/*
 * @brief 加载图片
 * @param _pathAndName 图片的路径
 * @param _img 加载出来的图片
 * @return 成功返回true
 * @return 失败返回false
 */
static bool _read_img(const std::string _pathAndName, cv::Mat &_img) {
    std::cout << U8C(u8"加载图片: ")
              << encoding::sysdcode_to_utf8(_pathAndName)
              << U8C(u8" 中") << std::endl;

    // 打开图片
    _img = cv::imread(_pathAndName);
    if (_img.empty( )) {
        std::cout << U8C(u8"图片 ")
                  << encoding::sysdcode_to_utf8(_pathAndName)
                  << U8C(u8" 打开失败") << std::endl;
        return false;
    }
    return true;
}

/* =============================================================================================================== */
/* =============================================================================================================== */
/* =============================================================================================================== */

// 获取经过校正之后的图片
cv::Mat DocumentScanner::get_scanner_img( ) {

    return cv::Mat( );
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////关键函数//////////////////////////////////////////////////////////
///////////////////////////////////////////关键函数//////////////////////////////////////////////////////////
/*
 * @brief 用于读取图片的表格（utf8编码）
 * @param _sheet 储存表格的二维数组（按照row，column的形式）
 * @param _path 文件的路径(系统编码格式)
 */
void load_sheet_from_img(
    table< std::string > &_sheet,
    const std::string    &_path) {

    // 读取图片
    cv::Mat img;
    if (!_read_img(_path, img)) return;    // 读取失败

    // ocr操作
    std::vector< std::vector< ppocr::OCRPredictResult > > ocrPR;    //[页][文字块]
    std::cout << U8C(u8"ppocr工作") << std::endl;
    ppocr::ocr(ocrPR, img.clone( ));    // 这里返回的text为utf8编码
    std::cout << std::endl
              << U8C(u8"图片: ")
              << encoding::sysdcode_to_utf8(_path)
              << U8C(u8" 加载结束...") << std::endl
              << std::endl;

    // 解析照片中表格的网格线
    GridResult grid(img);

    // 处理ocr的结果到cell的向量中
    //  这里只有一页，所以只有文字
    std::vector< CELL > textBox;    // solveOCR
    // 将 OCRPredictResult 转化为 DefSolveOCR
    if (!ocrPR.empty( )) {
        for (const auto &l : ocrPR[0]) {
            CELL temp(l);
            textBox.push_back(temp);
        }
    }

    // 还原表格
    grid.fill_sheet(textBox);
    _sheet = grid.get_stringSheet( );
}
///////////////////////////////////////////关键函数//////////////////////////////////////////////////////////
///////////////////////////////////////////关键函数//////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

}    // namespace img