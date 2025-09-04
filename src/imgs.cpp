
#include <algorithm>
#include <basic.hpp>
#include <cmath>
#include <cstdlib>
#include <Encoding.h>
#include <exception>
#include <Files.h>
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

bool enable_ManualDocPerspectiveCorrector = false;    // 是否启用手动透视校正
bool enable_ImageEnhancementRemoveShadow  = false;    // 去除阴影
bool enable_ImageEnhancementLightSharp    = false;    // 轻度锐化
bool enable_ImageEnhancementAuto          = false;    // 自动图像增强

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

/*
 * @brief 基于 HSV 色彩空间的阴影去除（简单高效，适合彩色图）
 * @brief 阴影会降低图像局部的明度（V 通道），但对色调（H） 和饱和度（S） 影响较小。通过单独增强阴影区域的 V 通道，可快速消除阴影。
 * @param _inImg 输入的图片
 * @param _light 亮度提升系数
 * @param _sh 阴影阈值缩放器
 * @return 返回一个深拷贝
 */
cv::Mat ImageEnhancement::remove_shadow_HSV(const cv::Mat &_inImg, double _light, double _sh) {

    cv::Mat hsv;
    cv::cvtColor(_inImg, hsv, cv::COLOR_BGR2HSV);    // BGR转HSV

    // 分离H、S、V通道
    std::vector< cv::Mat > channels;
    split(hsv, channels);
    cv::Mat V = channels[2];    // 明度通道（阴影区域值较低）

    // 1. 计算V通道的均值，作为阴影区域的亮度阈值参考
    cv::Scalar meanVal      = mean(V);
    int        shadowThresh = static_cast< int >(meanVal[0] * _sh);    // 阴影阈值（可根据图像调整）

    // 2. 增强阴影区域的亮度（非阴影区域保持不变）
    cv::Mat enhancedV;
    V.copyTo(enhancedV);
    for (int i = 0; i < V.rows; i++) {
        uchar *pV        = V.ptr< uchar >(i);
        uchar *pEnhanced = enhancedV.ptr< uchar >(i);
        for (int j = 0; j < V.cols; j++) {
            if (pV[j] < shadowThresh) {                                       // 判定为阴影区域
                pEnhanced[j] = cv::saturate_cast< uchar >(pV[j] * _light);    // 亮度提升（系数可调整）
            }
        }
    }

    // 3. 合并通道并转回BGR
    channels[2] = enhancedV;
    merge(channels, hsv);
    cv::Mat dst;
    cvtColor(hsv, dst, cv::COLOR_HSV2BGR);
    return dst;
}

/*
 * @brief 基于形态学操作去除阴影（返回一个灰度图）
 * @param _inImg 输入的图片
 * @return 返回一个深拷贝的灰度图
 */
cv::Mat ImageEnhancement::remove_shadow_toGray_mophology(const cv::Mat &_inImg) {

    // 1.将图像转为灰度图
    cv::Mat gray;
    cv::cvtColor(_inImg, gray, cv::COLOR_BGR2GRAY);

    // 定义腐蚀和膨胀的结构化元素和迭代次数
    cv::Mat element   = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    int     iteration = 9;

    // 2.将灰度图进行膨胀操作
    cv::Mat dilateMat;
    cv::morphologyEx(gray, dilateMat, cv::MORPH_DILATE, element, cv::Point(-1, -1), iteration);

    // 3.将膨胀后的图再进行腐蚀
    cv::Mat erodeMat;
    cv::morphologyEx(dilateMat, erodeMat, cv::MORPH_ERODE, element, cv::Point(-1, -1), iteration);

    // 4.膨胀再腐蚀后的图减去原灰度图再进行取反操作
    cv::Mat calcMat = ~(erodeMat - gray);

    // 5.使用规一化将原来背景白色的改了和原来灰度图差不多的灰色
    cv::Mat removeShadowMat;
    cv::normalize(calcMat, removeShadowMat, 0, 200, cv::NORM_MINMAX);

    return removeShadowMat;
}

/*
 * @brief 基于Gamma校正与白平衡去除阴影
 * @param _inImg 输入的图片
 * @param sigma 背景高斯核大小（越大越平滑）
 * @param gamma  Gamma 校正系数
 * @return 返回一个深拷贝的灰度图
 */
cv::Mat ImageEnhancement::remove_shadow_Gamma(
    const cv::Mat &_inImg,
    double         sigma,
    double         gamma) {
    CV_Assert(!_inImg.empty( ) && _inImg.type( ) == CV_8UC3);

    // 1. 转 float，范围 0~1
    cv::Mat img32f;
    _inImg.convertTo(img32f, CV_32FC3, 1.0 / 255.0);

    // 2. 估计背景光照：对灰度做超大高斯
    cv::Mat gray, bg;
    cv::cvtColor(img32f, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, bg, cv::Size(0, 0), sigma);
    cv::cvtColor(bg, bg, cv::COLOR_GRAY2BGR);

    // 3. 背景除法：抵消阴影
    cv::Mat flat = img32f / bg;

    // 4. Gamma 校正
    cv::Mat lut(1, 256, CV_8UC1);
    for (int i = 0; i < 256; ++i)
        lut.at< uchar >(i) = cv::saturate_cast< uchar >(255 * pow(i / 255.0, 1.0 / gamma));

    cv::Mat result8u;
    flat.convertTo(result8u, CV_8UC3, 255.0);
    cv::LUT(result8u, lut, result8u);

    return result8u;
}

/*
 * @brief 基于自适应阈值 + 背景减除的去阴影
 * @param _inImg 输入的图片
 * @param blockSize 自适应阈值邻域大小，必须是奇数
 * @param C 阈值偏移，越大阴影区越少
 * @param sigma 背景估计高斯核大小
 * @param lighten 阴影区加亮的灰度值
 */
cv::Mat ImageEnhancement::remove_shadow_adaptiveThreshold(
    const cv::Mat &_inImg,
    int            blockSize,
    double         C,
    double         sigma,
    double         lighten) {

    CV_Assert(!_inImg.empty( ) && _inImg.type( ) == CV_8UC3);

    // ---------- 1. 灰度 + 背景估计 ----------
    cv::Mat gray, bgIllum32f;
    cv::cvtColor(_inImg, gray, cv::COLOR_BGR2GRAY);
    cv::GaussianBlur(gray, bgIllum32f, cv::Size(0, 0), sigma);

    // ---------- 2. 自适应阈值：得到阴影掩膜 ----------
    // thresholdType = THRESH_BINARY_INV 这样暗区(阴影)为 255，亮区为 0
    cv::Mat shadowMask;
    cv::adaptiveThreshold(gray, shadowMask, 255,
                          cv::ADAPTIVE_THRESH_MEAN_C,
                          cv::THRESH_BINARY_INV,
                          blockSize, C);

    // 可选：形态学开运算去掉孤立噪点
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));
    cv::morphologyEx(shadowMask, shadowMask, cv::MORPH_OPEN, kernel);

    // ---------- 3. 背景减除 ----------
    // 把光照不均的部分除掉：原图 / 背景光照
    cv::Mat img32f, bg32f3;
    _inImg.convertTo(img32f, CV_32FC3, 1.0 / 255.0);
    cv::cvtColor(bgIllum32f, bg32f3, cv::COLOR_GRAY2BGR);
    bg32f3.convertTo(bg32f3, CV_32FC3, 1.0 / 255.0);

    cv::Mat flat32f = img32f / bg32f3;

    // ---------- 4. 按掩膜提亮 ----------
    cv::Mat result8u;
    flat32f.convertTo(result8u, CV_8UC3, 255.0);

    // 只把阴影区加亮
    cv::Mat channels[3];
    cv::split(result8u, channels);
    for (int c = 0; c < 3; ++c) {
        channels[c].convertTo(channels[c], CV_32F);
        channels[c] += shadowMask * (lighten / 255.0);    // shadowMask==1 的地方加亮
        channels[c].convertTo(channels[c], CV_8U);
    }
    cv::merge(channels, 3, result8u);

    return result8u;
}

/*
 * @brief 基于像素的线性增益操作调整阴影（灰度阈值+Gamma提亮）
 * @param _inImg 输入的图片
 * @param light 暗部亮度加强度（ >0 亮度加强；<0 亮度减弱 ）
 * @return 返回一个深拷贝的灰度图
 */
cv::Mat ImageEnhancement::remove_shadow_pixel_linear(const cv::Mat &_inImg, int light) {
    cv::Mat _inputImg = _inImg.clone( );
    // 生成灰度图
    cv::Mat gray = cv::Mat::zeros(_inputImg.size( ), CV_32FC1);
    cv::Mat f    = _inputImg.clone( );
    f.convertTo(f, CV_32FC3);
    std::vector< cv::Mat > pics;
    split(f, pics);
    gray = 0.299f * pics[2] + 0.587 * pics[1] + 0.114 * pics[0];
    gray = gray / 255.f;

    // 确定阴影区
    cv::Mat thresh = cv::Mat::zeros(gray.size( ), gray.type( ));
    thresh         = (1.0f - gray).mul(1.0f - gray);
    // 取平均值作为阈值
    cv::Scalar t    = cv::mean(thresh);
    cv::Mat    mask = cv::Mat::zeros(gray.size( ), CV_8UC1);
    mask.setTo(255, thresh >= t[0]);

    // 参数设置
    int   max    = 4;
    float bright = light / 100.0f / max;
    float mid    = 1.0f + max * bright;

    // 边缘平滑过渡
    cv::Mat midrate    = cv::Mat::zeros(_inputImg.size( ), CV_32FC1);
    cv::Mat brightrate = cv::Mat::zeros(_inputImg.size( ), CV_32FC1);
    for (int i = 0; i < _inputImg.rows; ++i) {
        uchar *m  = mask.ptr< uchar >(i);
        float *th = thresh.ptr< float >(i);
        float *mi = midrate.ptr< float >(i);
        float *br = brightrate.ptr< float >(i);
        for (int j = 0; j < _inputImg.cols; ++j) {
            if (m[j] == 255) {
                mi[j] = mid;
                br[j] = bright;
            } else {
                mi[j] = (mid - 1.0f) / t[0] * th[j] + 1.0f;
                br[j] = (1.0f / t[0] * th[j]) * bright;
            }
        }
    }

    // 阴影提亮，获取结果图
    cv::Mat result = cv::Mat::zeros(_inputImg.size( ), _inputImg.type( ));
    for (int i = 0; i < _inputImg.rows; ++i) {
        float *mi = midrate.ptr< float >(i);
        float *br = brightrate.ptr< float >(i);
        uchar *in = _inputImg.ptr< uchar >(i);
        uchar *r  = result.ptr< uchar >(i);
        for (int j = 0; j < _inputImg.cols; ++j) {
            for (int k = 0; k < 3; ++k) {
                float temp = pow(float(in[3 * j + k]) / 255.f, 1.0f / mi[j]) * (1.0 / (1 - br[j]));
                if (temp > 1.0f)
                    temp = 1.0f;
                if (temp < 0.0f)
                    temp = 0.0f;
                uchar utemp  = uchar(255 * temp);
                r[3 * j + k] = utemp;
            }
        }
    }
    return result;
}

/*
 * @brief 基于形态学思想 + 自适应阈值 + 软阈值 + HSV 色彩保护 去除阴影
 * 这个方法一坨屎，不推荐
 * @param _inImg 输入的图片
 * @param light 暗部亮度加强度（ >0 亮度加强；<0 亮度减弱 ）
 * @param alpha Sigmoid 斜率，可调
 */
cv::Mat ImageEnhancement::remove_shadow_pixel_AT(const cv::Mat &_inImg, int light, float alpha) {
    /* ---------- 1. 深拷贝 + BGR→HSV ---------- */
    cv::Mat _inputImg = _inImg.clone( );
    cv::Mat hsv;
    cv::cvtColor(_inputImg, hsv, cv::COLOR_BGR2HSV);
    std::vector< cv::Mat > chn;
    cv::split(hsv, chn);    // chn[0]=H, chn[1]=S, chn[2]=V

    /* ---------- 2. 计算灰度亮度图 (0-1) ---------- */
    // 直接复用你原来的加权公式
    cv::Mat gray32f = 0.299f * chn[2] + 0.587f * chn[1] + 0.114f * chn[2];
    gray32f.convertTo(gray32f, CV_32FC1, 1.0f / 255.0f);

    /* ---------- 3. 阴影概率图 & Otsu 阈值 ---------- */
    cv::Mat prob = (1.0f - gray32f).mul(1.0f - gray32f);    // (1-gray)^2
    cv::Mat prob8u;
    prob.convertTo(prob8u, CV_8UC1, 255.0);
    cv::Mat temp;
    double  otsuTh = cv::threshold(prob8u, temp, 0, 255,
                                   cv::THRESH_BINARY | cv::THRESH_OTSU);
    otsuTh /= 255.0;    // 归一化到 0-1

    /* ---------- 4. 参数（保持原含义） ---------- */
    const int   max    = 4;
    const float bright = light / 100.0f / max;
    const float mid    = 1.0f + max * bright;

    /* ---------- 5. Sigmoid 软掩膜 ---------- */
    cv::Mat midrate    = cv::Mat::zeros(_inputImg.size( ), CV_32FC1);
    cv::Mat brightrate = cv::Mat::zeros(_inputImg.size( ), CV_32FC1);

    for (int i = 0; i < _inputImg.rows; ++i) {
        const float *pProb   = prob.ptr< float >(i);
        float       *pMid    = midrate.ptr< float >(i);
        float       *pBright = brightrate.ptr< float >(i);
        for (int j = 0; j < _inputImg.cols; ++j) {
            float x    = pProb[j] / otsuTh;    // 归一化
            float s    = 1.0f / (1.0f + std::exp(-alpha * (x - 0.5f)));
            pMid[j]    = 1.0f + (mid - 1.0f) * s;
            pBright[j] = bright * s;
        }
    }

    /* ---------- 6. 仅对 V 通道做 Gamma 提亮 ---------- */
    for (int i = 0; i < _inputImg.rows; ++i) {
        const float *pMid    = midrate.ptr< float >(i);
        const float *pBright = brightrate.ptr< float >(i);
        uchar       *pV      = chn[2].ptr< uchar >(i);
        for (int j = 0; j < _inputImg.cols; ++j) {
            float v = pV[j] / 255.0f;
            v       = pow(v, 1.0f / pMid[j]) * (1.0f / (1.0f - pBright[j]));
            v       = cv::max(0.0f, cv::min(1.0f, v));
            pV[j]   = cv::saturate_cast< uchar >(v * 255.0f);
        }
    }

    /* ---------- 7. 合并通道并转回 BGR ---------- */
    cv::Mat result;
    cv::merge(chn, hsv);
    cv::cvtColor(hsv, result, cv::COLOR_HSV2BGR);
    return result;
}

/*
 *brief 对 8 位 3 通道图做轻量锐化
 * @param _inImg 输入的图片
 * @param strength 锐化强度，默认 1.0（标准核），>1 更锐，<1 更弱
 */
cv::Mat ImageEnhancement::light_sharpen(const cv::Mat &_inImg, double strength) {
    CV_Assert(!_inImg.empty( ) && _inImg.type( ) == CV_8UC3);

    // 标准拉普拉斯锐化核
    cv::Mat kernel = (cv::Mat_< float >(3, 3) << 0, -1, 0,
                      -1, 5.0, -1,
                      0, -1, 0);
    kernel *= strength;    // 调节强度

    cv::Mat dst;
    cv::filter2D(_inImg, dst, -1, kernel);
    return dst;
}

/*
 * @brief 推荐的阴影去除函数
 * @param _inImg 输入的图片
 */
cv::Mat ImageEnhancement::remove_shandow(const cv::Mat &_inImg) {
    cv::Mat dis = remove_shadow_pixel_linear(_inImg, 45);
    dis         = light_sharpen(dis, 0.9);
    return dis;
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
    std::cout << U8C(u8"加载图片\"")
              << encoding::sysdcode_to_utf8(_pathAndName)
              << U8C(u8"\"中") << std::endl;

    // 打开图片
    _img = cv::imread(_pathAndName);
    if (_img.empty( )) {
        std::cout << U8C(u8"图片\"")
                  << encoding::sysdcode_to_utf8(_pathAndName)
                  << U8C(u8"\"打开失败") << std::endl;
        return false;
    }
    return true;
}

/*
 * @brief 用于读取图片的表格（utf8编码）
 * @param _sheet 储存表格的二维数组（按照row，column的形式）
 * @param _path 文件的路径(系统编码格式)
 */
void load_sheet_from_img(table< std::string > &_sheet, const std::string &_path) {

    // 读取图片
    cv::Mat img;
    if (!_read_img(_path, img)) return;    // 读取失败

    // @todo 这里应该全部裁剪之后再进入，需要重构母函数
    if (enable_ManualDocPerspectiveCorrector) {
        ManualDocPerspectiveCorrector perspectiveCorrector(img, file::split_file_from_path(_path));
        img = perspectiveCorrector.get_corrected_img( );
    }

    if (enable_ImageEnhancementAuto)
        img = ImageEnhancement::remove_shandow(img);
    else {
        if (enable_ImageEnhancementRemoveShadow)
            img = ImageEnhancement::remove_shadow_pixel_linear(img, 45);
        if (enable_ImageEnhancementLightSharp)
            img = ImageEnhancement::light_sharpen(img, 1);
    }

    // ocr操作
    std::vector< std::vector< ppocr::OCRPredictResult > > ocrPR;    //[页][文字块]
    std::cout << U8C(u8"ppocr工作") << std::endl;
    ppocr::ocr(ocrPR, img.clone( ));    // 这里返回的text为utf8编码
    std::cout << std::endl
              << U8C(u8"图片:\"") << encoding::sysdcode_to_utf8(_path) << U8C(u8"\"加载结束...") << std::endl
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