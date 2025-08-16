
#include <iostream>
#include <opencv2/opencv.hpp>
#include <test_for_cv.h>

using namespace cv;
using namespace std;

// 提取水平网格线
Mat extractHorizontalLines(Mat &src) {
    // 创建水平结构元素（水平线检测）
    int horizontal_size   = src.cols / 30;
    Mat horizontal_kernel = getStructuringElement(MORPH_RECT, Size(horizontal_size, 1));

    // 腐蚀和膨胀操作突出水平线
    Mat horizontal;
    erode(src, horizontal, horizontal_kernel);
    dilate(horizontal, horizontal, horizontal_kernel);

    return horizontal;
}

// 提取垂直网格线
Mat extractVerticalLines(Mat &src) {
    // 创建垂直结构元素（垂直线检测）
    int vertical_size   = src.rows / 30;
    Mat vertical_kernel = getStructuringElement(MORPH_RECT, Size(1, vertical_size));

    // 腐蚀和膨胀操作突出垂直线
    Mat vertical;
    erode(src, vertical, vertical_kernel);
    dilate(vertical, vertical, vertical_kernel);

    return vertical;
}

// 提取水平网格线的y坐标（带长度和边缘过滤）
std::vector< int > getHorizontalLineYs(const Mat &horizontalLines, int edgeThreshold = 10) {
    std::vector< int >                  ys;
    std::vector< std::vector< Point > > contours;

    // 查找轮廓
    findContours(horizontalLines, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // 图像宽度
    int imageWidth = horizontalLines.cols;
    // 横线长度阈值（图片宽度的1/3）
    double minLineLength = imageWidth / 3.0;

    for (const auto &contour : contours) {
        // 计算轮廓的边界框
        Rect rect = boundingRect(contour);

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
std::vector< int > getVerticalLineXs(const Mat &verticalLines, int edgeThreshold = 10) {
    std::vector< int >                  xs;
    std::vector< std::vector< Point > > contours;

    // 查找轮廓
    findContours(verticalLines, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // 图像高度
    int imageHeight = verticalLines.rows;
    // 竖线长度阈值（图片高度的1/5）
    double minLineLength = imageHeight / 5.0;

    for (const auto &contour : contours) {
        // 计算轮廓的边界框
        Rect rect = boundingRect(contour);

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

// 在测试函数中使用
void test_for_cv1( ) {
    // 读取图像（假设是表格图片）
    Mat img = imread("1.jpeg");
    if (img.empty( )) {
        cerr << "无法读取图像！" << endl;
        return;
    }

    // 转为灰度图
    Mat gray;
    cvtColor(img, gray, COLOR_BGR2GRAY);

    // 二值化（反相，让线条为白色）
    Mat thresh;
    threshold(gray, thresh, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);

    // 提取水平和垂直网格线
    Mat horizontal = extractHorizontalLines(thresh);
    Mat vertical   = extractVerticalLines(thresh);

    // 获取网格线坐标
    // 获取网格线坐标（设置边缘阈值为10像素，可根据实际情况调整）
    std::vector< int > horizontalYs = getHorizontalLineYs(horizontal,int(img.rows * 0.005));//（水平线的y坐标）
    std::vector< int > verticalXs   = getVerticalLineXs(vertical, int(img.cols * 0.005));

    // 打印结果
    cout << "水平线y坐标: ";
    for (int y : horizontalYs) {
        cout << y << " ";
    }
    cout << endl
         << horizontalYs.size( ) << endl;

    cout << "垂直线x坐标: ";
    for (int x : verticalXs) {
        cout << x << " ";
    }
    cout << endl;
    cout << verticalXs.size( ) << endl;

    // 合并水平线和垂直线
    Mat grid_lines;
    addWeighted(horizontal, 0.5, vertical, 0.5, 0, grid_lines);

    // 创建可调整大小的窗口
    namedWindow("原图", WINDOW_NORMAL);
    namedWindow("水平网格线", WINDOW_NORMAL);
    namedWindow("垂直网格线", WINDOW_NORMAL);
    namedWindow("所有网格线", WINDOW_NORMAL);

    // 显示结果
    imshow("原图", img);
    imshow("水平网格线", horizontal);
    imshow("垂直网格线", vertical);
    imshow("所有网格线", grid_lines);

    waitKey(0);
    destroyAllWindows( );

    // 保存结果
    imwrite("horizontal_lines.jpg", horizontal);
    imwrite("vertical_lines.jpg", vertical);
    imwrite("grid_lines.jpg", grid_lines);

    return;
}
