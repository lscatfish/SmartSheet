
#include <algorithm>
#include <ChineseEncoding.h>
#include <cmath>
#include <imgs.h>
#include <iostream>
#include <iostream>
#include <map>
#include <numeric>
#include <ppocr_API.h>
#include <string>
#include <utility>
#include <vector>
#include <vector>

// 模型地址
ppocr::DefDirs _ppocrDir_ = {
    "./models/ppocr_keys.txt",    // 字典库
    "./models/ch_PP-OCRv4_det_infer",
    "./models/ch_PP-OCRv4_rec_infer",
    "./models/ch_ppocr_mobile_v2.0_cls_infer"
};


// 定义二维点类型，first为x坐标，second为y坐标
using Point = std::pair< double, double >;

/**
 * @brief 对排序后的坐标值进行聚类（将相近的值归为一组）
 *
 * @param sorted_vals 已排序的坐标值集合（可以是x坐标或y坐标）
 * @param epsilon 聚类阈值，小于此值的坐标被视为同一组（同一网格线）
 * @return 每个聚类组的平均值，代表该组对应的网格线坐标
 */
static std::vector< double > cluster_coords(const std::vector< double > &sorted_vals, double epsilon) {
    // 如果输入为空，直接返回空集合
    if (sorted_vals.empty( )) return { };

    // 第一步：将相近的坐标值分组
    std::vector< std::vector< double > > groups;
    // 初始化第一组，包含第一个坐标值
    groups.push_back({ sorted_vals[0] });

    // 遍历剩余坐标值，进行分组
    for (size_t i = 1; i < sorted_vals.size( ); ++i) {
        // 与当前组的最后一个元素比较，若差值小于阈值则加入当前组
        if (std::abs(sorted_vals[i] - groups.back( ).back( )) < epsilon) {
            groups.back( ).push_back(sorted_vals[i]);
        } else {
            // 否则创建新组
            groups.push_back({ sorted_vals[i] });
        }
    }

    // 第二步：计算每组的平均值作为网格线坐标（平均值可降低误差影响）
    std::vector< double > cluster_means;
    for (const auto &group : groups) {
        // 计算组内所有元素的平均值
        double mean = std::accumulate(group.begin( ), group.end( ), 0.0) / group.size( );
        cluster_means.push_back(mean);
    }

    return cluster_means;
}

/**
 * @brief 从排序后的网格线坐标中检测基础间距（适用于等间距网格）
 *
 * @param sorted_coords 已排序的网格线坐标集合
 * @param epsilon 误差阈值，用于判断两个差值是否属于同一间距
 * @return 检测到的基础间距，若无法检测则返回0.0
 */
static double detect_spacing(const std::vector< double > &sorted_coords, double epsilon) {
    // 如果网格线数量少于2条，无法计算间距
    if (sorted_coords.size( ) < 2) {
        return 0.0;
    }

    // 计算相邻网格线之间的差值
    std::vector< double > diffs;
    for (size_t i = 1; i < sorted_coords.size( ); ++i) {
        diffs.push_back(sorted_coords[i] - sorted_coords[i - 1]);
    }

    // 对差值进行聚类，找到最可能的基础间距（出现最频繁的差值）
    std::vector< std::vector< double > > diff_groups;
    if (!diffs.empty( )) {
        diff_groups.push_back({ diffs[0] });

        for (size_t i = 1; i < diffs.size( ); ++i) {
            bool merged = false;
            // 尝试将当前差值合并到已有组中
            for (auto &group : diff_groups) {
                // 计算当前组的平均值
                double group_mean = std::accumulate(group.begin( ), group.end( ), 0.0) / group.size( );
                // 如果当前差值与组平均值的差小于阈值，则合并
                if (std::abs(diffs[i] - group_mean) < epsilon) {
                    group.push_back(diffs[i]);
                    merged = true;
                    break;
                }
            }
            // 如果无法合并到任何组，则创建新组
            if (!merged) {
                diff_groups.push_back({ diffs[i] });
            }
        }
    }

    // 选择最大的聚类组的平均值作为基础间距
    size_t max_group_size = 0;
    double best_spacing   = 0.0;
    for (const auto &group : diff_groups) {
        if (group.size( ) > max_group_size) {
            max_group_size = group.size( );
            best_spacing   = std::accumulate(group.begin( ), group.end( ), 0.0) / group.size( );
        }
    }

    return best_spacing;
}

/**
 * @brief 根据基础间距和范围生成完整的网格线集合
 *
 * @param initial_grid 初步聚类得到的网格线坐标
 * @param spacing 检测到的基础间距
 * @param epsilon 误差阈值，用于处理浮点数精度问题
 * @return 完整的网格线坐标集合
 */
static std::vector< double > generate_complete_grid(const std::vector< double > &initial_grid,
                                                    double spacing, double epsilon) {
    // 如果初始网格为空，返回空集合
    if (initial_grid.empty( )) return { };

    // 对初始网格线进行排序
    std::vector< double > sorted_grid = initial_grid;
    std::sort(sorted_grid.begin( ), sorted_grid.end( ));

    // 确定网格线的范围（最小值和最大值）
    double min_val = sorted_grid.front( );
    double max_val = sorted_grid.back( );

    // 如果间距无效（过小或无法检测），直接返回排序后的初始网格
    if (spacing <= epsilon) {
        return sorted_grid;
    }

    // 生成完整的网格线：从最小值开始，按间距递增，直到超过最大值
    std::vector< double > complete_grid;
    double                current = min_val;

    // 循环生成网格线，添加一个epsilon的容差避免因浮点数精度问题漏掉最后一个点
    while (current <= max_val + epsilon) {
        complete_grid.push_back(current);
        current += spacing;
    }

    return complete_grid;
}

/**
 * @brief 从稀疏的坐标点中提取完整的网格交点
 *
 * @param points 输入的稀疏坐标点集合
 * @param epsilon_x x方向的误差阈值
 * @param epsilon_y y方向的误差阈值
 * @return 完整的网格交点集合
 */
static std::vector< Point > extract_complete_grid(const std::vector< Point > &points,
                                                  double epsilon_x, double epsilon_y) {
    // 1. 从输入点中提取所有x坐标和y坐标
    std::vector< double > x_vals, y_vals;
    for (const auto &point : points) {
        x_vals.push_back(point.first);
        y_vals.push_back(point.second);
    }

    // 对x和y坐标分别排序（聚类前必须排序）
    std::sort(x_vals.begin( ), x_vals.end( ));
    std::sort(y_vals.begin( ), y_vals.end( ));

    // 2. 对x和y坐标进行聚类，得到初步的网格线
    // 使用各自的误差阈值进行聚类
    std::vector< double > initial_x_grid = cluster_coords(x_vals, epsilon_x);
    std::vector< double > initial_y_grid = cluster_coords(y_vals, epsilon_y);

    // 3. 检测x和y方向的基础间距
    // 使用各自的误差阈值检测间距
    double spacing_x = detect_spacing(initial_x_grid, epsilon_x);
    double spacing_y = detect_spacing(initial_y_grid, epsilon_y);

    // 4. 根据基础间距生成完整的x和y网格线
    std::vector< double > complete_x_grid = generate_complete_grid(initial_x_grid, spacing_x, epsilon_x);
    std::vector< double > complete_y_grid = generate_complete_grid(initial_y_grid, spacing_y, epsilon_y);

    // 5. 生成所有网格交点（x网格线和y网格线的笛卡尔积）
    std::vector< Point > grid_points;
    for (double x : complete_x_grid) {
        for (double y : complete_y_grid) {
            grid_points.emplace_back(x, y);
        }
    }

    return grid_points;
}

/*
 * @brief 用于读取图片的表格（utf8编码）
 * @param 储存表格的二维数组（按照row，column的形式）
 * @param 文件的路径
 */
void load_sheet_from_img(
    std::vector< std::vector< std::string > > &_aSheet,
    std::string                                _pathAndName) {

    // 打开图片
    cv::Mat img = cv::imread(_pathAndName);
    if (img.empty( )) {
        std::cout << anycode_to_utf8("图片 ") << _pathAndName << anycode_to_utf8(" 打开失败") << std::endl;
        return;
    }
    // ocr操作,读取img
    std::vector< std::vector< ppocr::OCRPredictResult > > ocrPR;    //[页][文字line]
    ppocr::ocr(ocrPR, img, _ppocrDir_);                             // 这里返回的text为utf8编码

    // 这里只有一页，所以只有文字
    std::vector< DefSolveOCR > sovocr;    // solveOCR
    // 将 OCRPredictResult 转化为 DefSolveOCR
    for (const auto &l : ocrPR[0]) {
        DefSolveOCR temp(l);
        sovocr.push_back(temp);
    }

    // ----------------关键算法----------------------------------------------
    // **********************************************************************

    // ----------------关键算法----------------------------------------------
}