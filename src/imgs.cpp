
#include <algorithm>
#include <basic.hpp>
#include <cmath>
#include <cstdlib>
#include <Encoding.h>
#include <exception>
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

// 定义由CELL生成的表格
using SHEET = std::vector< std::vector< CELL > >;

// 表格网格线的解析结果
class GridResult {
public:
    /*
     * @brief 输入图片，解析网格
     * @param _img 输入的图片
     */
    GridResult(const cv::Mat &img) {
        // 转为灰度图
        cv::Mat gray;
        cv::cvtColor(img, gray, cv::COLOR_BGR2GRAY);

        // 二值化（反相，让线条为白色）
        cv::Mat thresh;
        cv::threshold(gray, thresh, 0, 255, cv::THRESH_BINARY_INV | cv::THRESH_OTSU);

        // 提取水平和垂直网格线
        cv::Mat horizontal = extract_horizontal_lines(thresh);
        cv::Mat vertical   = extract_vertical_lines(thresh);

        // 获取网格线坐标
        // 获取网格线坐标（设置边缘阈值为10像素，可根据实际情况调整）
        this->horizontalYs_ = get_horizontal_lineYs(horizontal, int(img.rows * 0.005));    // 水平线的y坐标
        this->verticalXs_   = get_vertical_lineXs(vertical, int(img.cols * 0.005));        // 竖线的x坐标

        // 将网格线解析到表格
        parse_grid_to_sheet( );
    };
    ~GridResult( ) = default;

    // 提取水平网格线
    static cv::Mat extract_horizontal_lines(cv::Mat &src);

    // 提取垂直网格线
    static cv::Mat extract_vertical_lines(cv::Mat &src);

    // 提取水平网格线的y坐标（带长度和边缘过滤）
    static std::vector< int > get_horizontal_lineYs(const cv::Mat &horizontalLines, int edgeThreshold = 10);

    // 提取垂直网格线的x坐标（带长度和边缘过滤）
    static std::vector< int > get_vertical_lineXs(const cv::Mat &verticalLines, int edgeThreshold = 10);

    // 填充sheet
    // 可以考虑放在构造函数里面自动解析
    void fill_sheet(const std::vector< CELL > &inCellLists);

    // 返回构成的sheet
    SHEET get_sheet( );

    // 返回只有string的表格
    table< std::string > get_stringSheet( );

private:
    std::vector< int > horizontalYs_;    // 水平线的y坐标
    std::vector< int > verticalXs_;      // 竖线的x坐标
    SHEET              sheet_;           // 解析到的表格

    // 将网格线解析到表格
    void parse_grid_to_sheet( );
};


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

// 填充sheet
void GridResult::fill_sheet(const std::vector< CELL > &inCellLists) {
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
}

// 返回构成的sheet
SHEET GridResult::get_sheet( ) {
    return this->sheet_;
}

// 返回只有string的表格
table< std::string > GridResult::get_stringSheet( ) {
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


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////关键函数//////////////////////////////////////////////////////////
///////////////////////////////////////////关键函数//////////////////////////////////////////////////////////
/*
 * @brief 用于读取图片的表格（utf8编码）
 * @param _sheet 储存表格的二维数组（按照row，column的形式）
 * @param _path 文件的路径
 */
void load_sheet_from_img(
    table< std::string > &_sheet,
    std::string           _path) {

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
    std::vector< CELL > solveOCR;    // solveOCR
    // 将 OCRPredictResult 转化为 DefSolveOCR
    for (const auto &l : ocrPR[0]) {
        CELL temp(l);
        solveOCR.push_back(temp);
    }

    // 还原表格
    grid.fill_sheet(solveOCR);
    _sheet = grid.get_stringSheet( );
}
///////////////////////////////////////////关键函数//////////////////////////////////////////////////////////
///////////////////////////////////////////关键函数//////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
}    // namespace img