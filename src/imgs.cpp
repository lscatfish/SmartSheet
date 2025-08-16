
#include <algorithm>
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


// 模型地址
ppocr::DefDirs _ppocrDir_ = {
    "./models/ppocr_keys.txt",    // 字典库
    "./models/ch_PP-OCRv4_det_infer",
    "./models/ch_PP-OCRv4_rec_infer",
    "./models/ch_ppocr_mobile_v2.0_cls_infer"
};

// 定义二维点类型
struct GridPoint {
    double x;    // x坐标
    double y;    // y坐标
    GridPoint( ) {
        y = 0;
        x = 0;
    }
    GridPoint(double _x, double _y) {
        x = _x;
        y = _y;
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
    }

    /*
     * @brief 检查对象是否被包含在cell中
     * @param bigger 输入的可以包含对象的一个cell
     * @return 对象被包含在bigger中返回true，否则返回false
     */
    bool is_contained(const CELL &bigger) const {
        return (bigger.top_left.x < this->corePoint.x
                && bigger.bottom_right.x > this->corePoint.x
                && bigger.top_left.y < this->corePoint.y
                && bigger.bottom_right.y > this->corePoint.y);
    };
};

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

    // 返回构成的sheet
    SHEET get_sheet( );

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

// 返回构成的sheet
SHEET GridResult::get_sheet( ) {
    return this->sheet_;
}

/*
 * @brief 加载图片
 * @param _pathAndName 图片的路径
 * @param _img 加载出来的图片
 * @return 成功返回true
 * @return 失败返回false
 */
static bool _read_img(const std::string _pathAndName, cv::Mat &_img) {
    std::cout << u8"加载图片: " << encoding::chcode_to_utf8(_pathAndName) << u8" 中" << std::endl;

    // 打开图片
    _img = cv::imread(_pathAndName);
    if (_img.empty( )) {
        std::cout << u8"图片 " << encoding::chcode_to_utf8(_pathAndName) << u8" 打开失败" << std::endl;
        return false;
    }
    return true;
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////关键函数//////////////////////////////////////////////////////////
///////////////////////////////////////////关键函数//////////////////////////////////////////////////////////
/*
 * @brief 用于读取图片的表格（utf8编码）
 * @param 储存表格的二维数组（按照row，column的形式）
 * @param 文件的路径
 */
void load_sheet_from_img(
    std::vector< std::vector< std::string > > &_aSheet,
    std::string                                _pathAndName) {

    // 读取图片
    cv::Mat img;
    if (!_read_img(_pathAndName, img)) return;    // 读取失败

    // ocr操作
    std::vector< std::vector< ppocr::OCRPredictResult > > ocrPR;    //[页][文字块]
    std::cout << u8"ppocr工作" << std::endl;
    ppocr::ocr(ocrPR, img.clone( ), _ppocrDir_);    // 这里返回的text为utf8编码
    std::cout << std::endl
              << u8"图片: " << encoding::chcode_to_utf8(_pathAndName) << u8" 加载结束..." << std::endl
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
}
///////////////////////////////////////////关键函数//////////////////////////////////////////////////////////
///////////////////////////////////////////关键函数//////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
