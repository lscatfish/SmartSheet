
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

/* =============================================================================================================== */
/* =============================================================================================================== */

/*
 * @brief 计算两点间距离
 * @param 两个2D点a和b
 * @param 两点间的欧氏距离（基于OpenCV的norm函数，简化计算）
 */
double ManualDocPerspectiveCorrector::dist(const cv::Point2f &a, const cv::Point2f &b) {
    return cv::norm(a - b);    // norm函数计算向量（a-b）的L2范数，即欧氏距离
}

/*
 * @brief 原始图像坐标转显示图像坐标
 * @param p 原始图像中的点坐标
 * @param scale 原始图像到显示图像的缩放比例
 * @param r 显示图像中缩放后原图的矩形区域（img_roi）
 * @return 该点在显示图像中的对应坐标（考虑按钮栏偏移和缩放）
 */
cv::Point2f ManualDocPerspectiveCorrector::src2disp(const cv::Point2f &p, double scale, const cv::Rect &r) {
    // 计算逻辑：显示x = 缩放后原图的x偏移 + 原始x * 缩放比例
    //          显示y = 缩放后原图的y偏移 + 原始y * 缩放比例
    return cv::Point2f((float)(r.x + p.x * scale), (float)(r.y + p.y * scale));
}

/*
 * @brief 显示图像坐标转原始图像坐标
 * @param p 显示图像中的点坐标
 * @param scale 原始图像到显示图像的缩放比例
 * @param r 显示图像中缩放后原图的矩形区域（img_roi）
 * @return 该点在原始图像中的对应坐标（用于矫正计算，确保精度）
 */
cv::Point2f ManualDocPerspectiveCorrector::disp2src(const cv::Point2f &p, double scale, const cv::Rect &r) {
    // 计算逻辑：原始x = (显示x - 缩放后原图的x偏移) / 缩放比例
    //          原始y = (显示y - 缩放后原图的y偏移) / 缩放比例
    return cv::Point2f((float)((p.x - r.x) / scale), (float)((p.y - r.y) / scale));
}

// 获取校正之后的图像
cv::Mat ManualDocPerspectiveCorrector::get_corrected_img( ) {
    return corrected_.clone( );
}

/*
 * @brief 布局计算函数：动态调整界面元素位置
 * @param win_size 当前主窗口的尺寸（宽x高）
 * @return img_roi 缩放后原图在显示图像中的矩形区域
 * @return scale 原始图像到显示图像的缩放比例（确保原图完整显示且不拉伸）
 */
void ManualDocPerspectiveCorrector::layout(const cv::Size &win_size, cv::Rect &img_roi, double &scale) {
    // 异常处理：窗口尺寸过小（宽<=0或高<=按钮栏高度），无法显示图像
    if (win_size.width <= 0 || win_size.height <= BAR_H) {
        img_roi = cv::Rect( );    // 空矩形，表示无有效图像区域
        scale   = 1.0;            // 默认缩放比例1.0
        return;
    }

    // 可用显示区域：窗口总区域减去顶部按钮栏区域
    int usable_w = win_size.width;             // 可用宽度 = 窗口宽度（按钮栏占满宽度）
    int usable_h = win_size.height - BAR_H;    // 可用高度 = 窗口高度 - 按钮栏高度

    // 计算缩放比例：取“宽度适配比例”和“高度适配比例”的最小值，避免图像拉伸
    // 宽度适配比例 = 可用宽度 / 原始图像宽度
    // 高度适配比例 = 可用高度 / 原始图像高度
    scale = (std::min)((double)usable_w / src_.cols, (double)usable_h / src_.rows);

    // 计算缩放后原图的实际显示尺寸
    int drawn_w = (int)(src_.cols * scale);    // 缩放后宽度 = 原始宽度 * 缩放比例
    int drawn_h = (int)(src_.rows * scale);    // 缩放后高度 = 原始高度 * 缩放比例

    // 计算缩放后原图的显示偏移（使其在可用区域内居中）
    int off_x = (usable_w - drawn_w) / 2;            // x方向偏移 = (可用宽度 - 缩放后宽度)/2
    int off_y = BAR_H + (usable_h - drawn_h) / 2;    // y方向偏移 = 按钮栏高度 + (可用高度 - 缩放后高度)/2

    // 确定缩放后原图的矩形区域（img_roi）
    img_roi = cv::Rect(off_x, off_y, drawn_w, drawn_h);

    // 更新显示图像中的四边形顶点坐标（根据缩放比例和偏移）
    for (int i = 0; i < 4; ++i)
        dispPts_[i] = src2disp(srcPts_[i], scale, img_roi);

    // 确定“done”按钮的位置（窗口右上角，右偏移20px，垂直居中于按钮栏）
    btnOK_ = cv::Rect(
        win_size.width - BTN_W - 20,    // 按钮x坐标 = 窗口宽度 - 按钮宽度 - 右偏移20px
        (BAR_H - BTN_H) / 2,            // 按钮y坐标 = (按钮栏高度 - 按钮高度)/2（垂直居中）
        BTN_W, BTN_H                    // 按钮宽高
    );
}

/*
 * @brief 绘制函数：渲染整个显示界面
 * @brief 绘制按钮栏、缩放后的原图、可拖拽的四边形顶点和边
 * @param img_roi 缩放后原图在显示图像中的矩形区域
 */
void ManualDocPerspectiveCorrector::redraw(const cv::Rect &img_roi) {
    // 1. 创建显示画布：尺寸与当前窗口一致，背景色为浅灰色（RGB：240,240,240）
    cv::Size sz = cv::getWindowImageRect(windowName_).size( );    // 获取当前窗口的实际尺寸
    displayImg_ = cv::Mat(sz, CV_8UC3, cv::Scalar(240, 240, 240));

    // 2. 绘制“done”按钮：绿色填充，白色文字
    cv::rectangle(displayImg_, btnOK_, cv::Scalar(50, 180, 50), cv::FILLED);    // 绘制绿色填充矩形
    cv::putText(displayImg_, "Done",                                            // 绘制按钮文字
                cv::Point(btnOK_.x + 20, btnOK_.y + BTN_H - 10),                // 文字位置（按钮内居中）
                cv::FONT_HERSHEY_SIMPLEX, 0.7,                                  // 字体类型、大小
                cv::Scalar(255, 255, 255), 2);                                  // 文字颜色（白色）、粗细

    // 异常处理：无有效图像区域（窗口过小），仅绘制按钮栏
    if (img_roi.empty( )) return;

    // 3. 绘制缩放后的原始图像：将原图缩放到img_roi区域
    cv::Mat roi = displayImg_(img_roi);                          // 获取显示画布中的图像区域
    cv::resize(src_, roi, roi.size( ), 0, 0, cv::INTER_AREA);    // 缩放原图到roi尺寸，INTER_AREA适合缩小

    // 4. 绘制可拖拽的四边形：红色顶点（填充圆）、绿色边（直线）
    for (int i = 0; i < 4; ++i) {
        // 绘制顶点：红色填充圆，半径8px
        cv::circle(displayImg_, dispPts_[i], 8, cv::Scalar(0, 0, 255), cv::FILLED);
        // 绘制边：绿色直线，粗细2px，连接当前顶点与下一个顶点（第4个顶点连回第1个）
        cv::line(displayImg_, dispPts_[i], dispPts_[(i + 1) % 4], cv::Scalar(0, 255, 0), 2);
    }
}

/*
 * @brief 鼠标回调函数：处理用户交互
 * @brief 响应鼠标点击、拖拽事件，实现顶点调整和矫正触发
 * @param evt 鼠标事件类型（按下、移动、释放等）
 * @param x,y 鼠标在窗口中的坐标
 * @param void* 用户数据（此处未使用）
 */
void ManualDocPerspectiveCorrector::onMouse(int evt, int x, int y, int, void *) {
    // 矫正完成后，不再响应鼠标事件
    if (done_) return;

    cv::Point pt(x, y);    // 存储当前鼠标坐标

    // 临时变量：存储当前图像布局信息
    cv::Rect img_roi;
    double   scale;
    // 计算当前窗口的布局（图像区域、缩放比例、按钮位置）
    layout(cv::getWindowImageRect(windowName_).size( ), img_roi, scale);

    // 1. 鼠标左键按下事件：判断是点击顶点还是点击按钮
    if (evt == cv::EVENT_LBUTTONDOWN) {
        // 1.1 检查是否点击了四边形顶点（遍历4个顶点）
        for (int i = 0; i < 4; ++i) {
            // 鼠标与顶点距离小于12px，视为选中该顶点
            if (dist(pt, dispPts_[i]) < 12) {
                isDragging_ = true;    // 标记进入拖拽状态
                dragIdx_    = i;       // 记录被拖拽的顶点索引
                return;                // 跳出函数，避免后续按钮判断
            }
        }

        // 1.2 检查是否点击了“done”按钮（判断鼠标是否在按钮矩形内）
        if (btnOK_.contains(pt)) {
            // 定义A4竖版的目标矩形顶点（顺时针：左上、右上、右下、左下）
            cv::Point2f dst[4] = { { 0, 0 }, { (float)a4w, 0 }, { (float)a4w, (float)a4h }, { 0, (float)a4h } };
            // 计算透视变换矩阵：从原始图像的四边形（src_pts）映射到A4矩形（dst）
            cv::Mat M = getPerspectiveTransform(srcPts_.data( ), dst);
            // 执行透视矫正：使用高质量插值INTER_LANCZOS4，背景默认黑色
            warpPerspective(src_, corrected_, M, cv::Size(a4w, a4h), cv::INTER_LANCZOS4);
            // imshow("矫正结果", corrected);  // 注释：如需实时显示矫正结果，可取消注释
            done_ = true;    // 标记矫正完成，退出交互循环
            return;
        }
    }

    // 2. 鼠标移动事件：仅在拖拽状态下响应，更新顶点位置
    if (evt == cv::EVENT_MOUSEMOVE && isDragging_) {
        // 无有效图像区域，不处理拖拽
        if (img_roi.empty( )) return;

        // 限制顶点拖拽范围：只能在缩放后的原图区域内（避免拖出图像外）
        pt.x = (std::max)(img_roi.x, (std::min)(pt.x, img_roi.x + img_roi.width));     // x方向限制在图像左右边界
        pt.y = (std::max)(img_roi.y, (std::min)(pt.y, img_roi.y + img_roi.height));    // y方向限制在图像上下边界

        // 更新原始图像中的顶点坐标（从显示坐标转回原始坐标，确保矫正精度）
        srcPts_[dragIdx_] = disp2src(pt, scale, img_roi);
        // 更新显示图像中的顶点坐标（直接使用限制后的鼠标坐标）
        dispPts_[dragIdx_] = pt;

        // 重新绘制界面，实时更新顶点位置
        redraw(img_roi);
        cv::imshow(windowName_, displayImg_);
    }

    // 3. 鼠标左键释放事件：结束拖拽状态
    if (evt == cv::EVENT_LBUTTONUP)
        isDragging_ = false;    // 重置拖拽标记
}

//// 静态成员函数：作为 setMouseCallback 的回调（无 this 指针）
void ManualDocPerspectiveCorrector::staticOnMouse(int event, int x, int y, int flags, void *userdata) {
    // 将 userdata 转换为类实例指针，再调用非静态 onMouse
    ManualDocPerspectiveCorrector *instance =
        static_cast< ManualDocPerspectiveCorrector * >(userdata);
    if (instance != nullptr) {
        // 调用实例的非静态 onMouse，传递事件参数
        instance->onMouse(event, x, y, flags, userdata);
    }
}

/* =============================================================================================================== */
/* =============================================================================================================== */

// 照片预处理
//@return 返回一个二值化的边缘图
cv::Mat DocumentScanner::preprocess(const cv::Mat &_img) {
    cv::Mat gray, binary, gauss;
    cv::GaussianBlur(_img, gauss, cv::Size(5, 5), 0);
    cv::cvtColor(gauss, gray, cv::COLOR_BGR2GRAY);

    // clahe
    cv::Mat enhance;
    auto    clahe = cv::createCLAHE(1.5, cv::Size(8, 8));
    clahe->apply(gray, enhance);

    // 自适应阈值
    cv::adaptiveThreshold(enhance, binary, 255, cv::ADAPTIVE_THRESH_GAUSSIAN_C, cv::THRESH_BINARY, 31, 5);

    // 形态学操作去除噪点
    cv::Mat morph;
    // 先腐蚀去除小点，再膨胀恢复形状
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_ELLIPSE, cv::Size(3, 3));
    cv::erode(binary, morph, kernel, cv::Point(-1, -1), 1);
    cv::dilate(morph, morph, kernel, cv::Point(-1, -1), 1);

    // 中值滤波
    cv::Mat median;
    cv::medianBlur(morph, median, 3);

    // 反转图像（因为connectedComponentsWithStats默认前景为白色）
    // 反转后：黑色小点变为白色，背景变为黑色
    cv::Mat inverted;
    cv::bitwise_not(median, inverted);

    // 方法3：去除小面积连通区域
    cv::Mat labels, stats, centroids;
    int     num_labels = cv::connectedComponentsWithStats(inverted, labels, stats, centroids, 8, CV_32S);
    // 创建结果图像，初始化为全白（255）
    cv::Mat filtered = cv::Mat::ones(inverted.size( ), CV_8UC1) * 255;

    // 遍历所有连通区域（黑色区域）
    for (int i = 1; i < num_labels; i++) {    // i=0是白色背景，跳过
        int area = stats.at< int >(i, cv::CC_STAT_AREA);
        // 如果是小面积区域（原黑色小点），保留为白色（不处理）
        // 大面积黑色区域（需要保留的），在结果中设为黑色
        if (area > 200) {
            filtered.setTo(0, labels == i);
        }
    }

    return filtered.clone( );    // 避免悬空指针和空引用
}

// 获取经过校正之后的图片
cv::Mat DocumentScanner::get_scanner_img( ) {

    return cv::Mat( );
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////关键函数//////////////////////////////////////////////////////////

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
/////////////////////////////////////////////////////////////////////////////////////////////////////////////


}    // namespace img