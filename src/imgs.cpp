
#include <algorithm>
#include <Encoding.h>
#include <cmath>
#include <cstdlib>
#include <exception>
#include <helper.h>
#include <imgs.h>
#include <iostream>
#include <map>
#include <numeric>
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


/**
 * @brief 对排序后的坐标值进行聚类（将相近的值归为一组）
 *
 * @param sorted_vals 已排序的坐标值集合（可以是x坐标或y坐标）
 * @param epsilon 聚类阈值，小于此值的坐标被视为同一组（同一网格线）
 * @return 每个聚类组的平均值，代表该组对应的网格线坐标
 */
std::vector< double > cluster_coords(const std::vector< double > &sorted_vals, double epsilon) {
    if (sorted_vals.empty( )) return { };

    std::vector< std::vector< double > > groups;
    groups.push_back({ sorted_vals[0] });

    for (size_t i = 1; i < sorted_vals.size( ); ++i) {
        if (std::abs(sorted_vals[i] - groups.back( ).back( )) < epsilon) {
            groups.back( ).push_back(sorted_vals[i]);
        } else {
            groups.push_back({ sorted_vals[i] });
        }
    }

    std::vector< double > cluster_means;
    for (const auto &group : groups) {
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
double detect_spacing(const std::vector< double > &sorted_coords, double epsilon) {
    if (sorted_coords.size( ) < 2) {
        return 0.0;
    }

    std::vector< double > diffs;
    for (size_t i = 1; i < sorted_coords.size( ); ++i) {
        diffs.push_back(sorted_coords[i] - sorted_coords[i - 1]);
    }

    std::vector< std::vector< double > > diff_groups;
    if (!diffs.empty( )) {
        diff_groups.push_back({ diffs[0] });

        for (size_t i = 1; i < diffs.size( ); ++i) {
            bool merged = false;
            for (auto &group : diff_groups) {
                double group_mean = std::accumulate(group.begin( ), group.end( ), 0.0) / group.size( );
                if (std::abs(diffs[i] - group_mean) < epsilon) {
                    group.push_back(diffs[i]);
                    merged = true;
                    break;
                }
            }
            if (!merged) {
                diff_groups.push_back({ diffs[i] });
            }
        }
    }

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
 * @param min_valid_spacing 生成网格线时过滤过小间距
 * @return 完整的网格线坐标集合
 */
std::vector< double > generate_complete_grid(
    const std::vector< double > &initial_grid,
    double spacing, double epsilon, double min_valid_spacing) {
    if (initial_grid.empty( )) return { };

    std::vector< double > sorted_grid = initial_grid;
    std::sort(sorted_grid.begin( ), sorted_grid.end( ));

    double min_val = sorted_grid.front( );
    double max_val = sorted_grid.back( );

    // 若间距过小或无效，则使用初始网格线（不扩展）
    if (spacing <= min_valid_spacing || spacing <= epsilon) {
        return sorted_grid;
    }

    // 生成完整网格线
    std::vector< double > complete_grid;
    double                current = min_val;

    // 避免生成过多网格线：限制最大数量（根据区域大小动态调整）
    size_t max_grid_lines = static_cast< size_t >((max_val - min_val) / min_valid_spacing) + 2;
    size_t count          = 0;

    while (current <= max_val + epsilon && count < max_grid_lines) {
        complete_grid.push_back(current);
        current += spacing;
        count++;
    }

    return complete_grid;
}

/**
 * @brief 从稀疏的坐标点中提取完整的网格信息（包含行坐标、列坐标和交点）
 *
 * @param points 输入的稀疏坐标点集合
 * @param epsilon_x x方向的误差阈值
 * @param epsilon_y y方向的误差阈值
 * @param min_valid_spacing_x 提取网格信息时传入最小有效间距参数x
 * @param min_valid_spacing_y 提取网格信息时传入最小有效间距参数y
 * @return GridResult 包含行坐标、列坐标和所有交点的结构体
 */
GridResult extract_grid_info(
    const std::vector< GridPoint > &points,
    double epsilon_x, double epsilon_y,
    double min_valid_spacing_x, double min_valid_spacing_y) {
    GridResult result;

    std::vector< double > x_vals, y_vals;
    for (const auto &point : points) {
        x_vals.push_back(point.first);
        y_vals.push_back(point.second);
    }

    std::sort(x_vals.begin( ), x_vals.end( ));
    std::sort(y_vals.begin( ), y_vals.end( ));

    // 聚类得到初步网格线
    std::vector< double > initial_x_grid = cluster_coords(x_vals, epsilon_x);
    std::vector< double > initial_y_grid = cluster_coords(y_vals, epsilon_y);

    // 检测基础间距
    double spacing_x = detect_spacing(initial_x_grid, epsilon_x);
    double spacing_y = detect_spacing(initial_y_grid, epsilon_y);

    // 生成完整网格线（传入最小有效间距）
    result.col_coords = generate_complete_grid(initial_x_grid, spacing_x, epsilon_x, min_valid_spacing_x);
    result.row_coords = generate_complete_grid(initial_y_grid, spacing_y, epsilon_y, min_valid_spacing_y);

    // 生成所有交点
    for (double x : result.col_coords) {
        for (double y : result.row_coords) {
            result.intersections.emplace_back(x, y);
        }
    }

    return result;
}


/**
 * @brief 新增函数：根据点列密度自动计算x和y方向的误差阈值
 *
 * @param points 输入的坐标点集合
 * @param max_x 区域x方向最大值（点的x坐标≤x0）
 * @param max_y 区域y方向最大值（点的y坐标≤y0）
 * @param ratio 阈值与中位数距离的比例（建议0.2~0.5，值越小阈值越严格）
 * @param min_epsilon_ratio 最小阈值区域的比例
 * @return 包含epsilon_x和epsilon_y的 pair （first为x方向阈值，second为y方向阈值）
 */
std::pair< double, double > auto_calculate_epsilon(
    const std::vector< GridPoint > &points,
    double max_x, double max_y,
    double ratio,                  // 增大比例系数（原 0.3→0.5）
    double min_epsilon_ratio) {    // 最小阈值为区域的 1%
    if (max_x <= 0 || max_y <= 0) {
        throw std::invalid_argument(encoding::anycode_to_utf8("区域大小 max_x 和 y0 必须为正数"));
    }
    if (ratio <= 0 || ratio >= 1) {
        throw std::invalid_argument(encoding::anycode_to_utf8("比例参数 ratio 必须在 (0,1) 范围内"));
    }

    std::vector< double > x_vals, y_vals;
    for (const auto &point : points) {
        if (point.first < 0 || point.first > max_x || point.second < 0 || point.second > max_y) {
            throw std::invalid_argument(encoding::anycode_to_utf8("点坐标超出指定区域范围或为负数"));
        }
        x_vals.push_back(point.first);
        y_vals.push_back(point.second);
    }
    std::sort(x_vals.begin( ), x_vals.end( ));
    std::sort(y_vals.begin( ), y_vals.end( ));

    // 计算单个方向的阈值（增强版）
    auto calculate_single_epsilon = [&](const std::vector< double > &coords, double range) {
        // 最小阈值限制：区域范围的一定比例（避免阈值过小）
        double min_epsilon = range * min_epsilon_ratio;

        if (coords.size( ) <= 1) {
            return min_epsilon;    // 点太少时使用最小阈值
        }

        // 计算相邻点的距离（过滤零距离）
        std::vector< double > distances;
        for (size_t i = 1; i < coords.size( ); ++i) {
            double dist = coords[i] - coords[i - 1];
            if (dist > 1e-9) {
                distances.push_back(dist);
            }
        }

        if (distances.empty( )) {
            return min_epsilon;    // 所有点重合时使用最小阈值
        }

        // 计算距离的中位数（忽略过小距离，避免受密集噪声点影响）
        std::sort(distances.begin( ), distances.end( ));
        // 过滤前 20% 的过小距离（鲁棒性优化）
        size_t                start_idx = std::max(1, static_cast< int >(distances.size( ) * 0.2));
        std::vector< double > filtered_dist(distances.begin( ) + start_idx, distances.end( ));

        size_t n           = filtered_dist.size( );
        double median_dist = (n % 2 == 1) ? filtered_dist[n / 2] : (filtered_dist[n / 2 - 1] + filtered_dist[n / 2]) / 2.0;

        // 阈值 = max (中位数距离 × 比例，最小阈值)
        return std::max(median_dist * ratio, min_epsilon);
    };

    // 计算 x 和 y 方向阈值
    double epsilon_x = calculate_single_epsilon(x_vals, max_x);
    double epsilon_y = calculate_single_epsilon(y_vals, max_y);

    return { epsilon_x, epsilon_y };
}

// 判断a（x,y）是否存在
static bool exists(const std::vector< std::vector< std::string > > &a, int x, int y) {
    return (x >= 0 && x < a.size( ) && y >= 0 && y < a[x].size( ));
}

/*
 * @brief 计算到给定目标点距离最近的网格点
 * @param 目标点
 * @param 网格点构成的点列
 */
static std::pair< size_t, size_t > get_closest_grid_point(
    const GridPoint  &_targetPoint,
    const GridResult &_gridStd) {

    // 其实只用找离x0最近grid与y0最近的grid构成的网格点(_gridStd.col_coords[col],_gridStd.row_coords[row])就可以

    size_t col = 0, row = 0;    // 最近网格点的（row，col）值
    double distance = 999999;
    for (; col < _gridStd.col_coords.size( ); col++) {
        double temp = std::min(distance, std::abs(_gridStd.col_coords[col] - _targetPoint.first));
        if (distance > temp) {
            distance = temp;
        } else {
            col--;
            break;
        }
    }
    if (col == _gridStd.col_coords.size( )) col--;
    distance = 999999;
    for (; row < _gridStd.row_coords.size( ); row++) {
        double temp = std::min(distance, std::abs(_gridStd.row_coords[row] - _targetPoint.second));
        if (distance > temp) {
            distance = temp;
        } else {
            row--;
            break;
        }
    }
    if (row == _gridStd.row_coords.size( )) row--;
    return { row, col };
}


/*
 * @brief 用于读取图片的表格（utf8编码）
 * @param 储存表格的二维数组（按照row，column的形式）
 * @param 文件的路径
 */
void load_sheet_from_img(
    std::vector< std::vector< std::string > > &_aSheet,
    std::string                                _pathAndName) {
    std::cout << u8"加载图片: " << _pathAndName << u8" 中" << std::endl
              << std::endl;

    // 打开图片
    cv::Mat img = cv::imread(_pathAndName);
    if (img.empty( )) {
        std::cout << u8"图片 " << _pathAndName << u8" 打开失败" << std::endl;
        return;
    }
    // ocr操作,读取img
    std::vector< std::vector< ppocr::OCRPredictResult > > ocrPR;    //[页][文字line]
    ppocr::ocr(ocrPR, img.clone( ), _ppocrDir_);                    // 这里返回的text为utf8编码

    std::cout << std::endl
              << u8"图片: " << _pathAndName << u8" 加载结束..." << std::endl
              << std::endl;

    // 这里只有一页，所以只有文字
    std::vector< DefSolveOCRResult > sovocr;    // solveOCR
    // 将 OCRPredictResult 转化为 DefSolveOCR
    for (const auto &l : ocrPR[0]) {
        DefSolveOCRResult temp(l);
        sovocr.push_back(temp);
    }

    // 提取点列
    std::vector< GridPoint > corePoints;
    for (auto &it_sovocr : sovocr) {
        corePoints.push_back(it_sovocr.corePoint);
    }

    try {
        // 自动计算阈值
        auto [epsilon_x, epsilon_y] = auto_calculate_epsilon(corePoints, img.cols, img.rows, 0.4, 0.009);
        // 计算最小有效间距（避免生成过密网格线）
        double min_spacing_x;
        double min_spacing_y;
        if (img.cols < img.rows) {
            min_spacing_x = std::max(epsilon_x * 2, img.cols / 7.0);
            min_spacing_y = std::max(epsilon_y * 2, img.rows / 30.0);
        } else {
            min_spacing_x = std::max(epsilon_x * 2, img.cols / 30.0);
            min_spacing_y = std::max(epsilon_y * 2, img.rows / 7.0);
        }

        // 提取网格（传入最小有效间距）
        GridResult gridResult = extract_grid_info(corePoints, epsilon_x, epsilon_y, min_spacing_x, min_spacing_y);

        // 构造表格,按照空string "" 初始化
        std::vector< std::vector< std::string > > sh;
        for (size_t row = 0; row < gridResult.row_coords.size( ); row++) {
            std::vector< std::string > line(gridResult.col_coords.size( ), "");
            sh.push_back(line);
        }

        std::cout << u8"总行数：" << sh.size( ) << u8"   总列数：" << gridResult.col_coords.size( ) << std::endl;

        // 给表格加入值
        for (const auto &cellOCR : sovocr) {
            auto [x, y] = get_closest_grid_point(cellOCR.corePoint, gridResult);
            if (!exists(sh, x, y)) {
                std::cout << u8"错误(" << x << "," << y << "):" << "unExist" << std::endl;
                pause( );
                continue;
            }
            if (sh[x][y].size( ) == 0) {
                // std::cout << "in(" << x << "," << y << ")" << std::endl;
                sh[x][y] = cellOCR.text;
            } else {
                std::cout << u8"错误(" << x << "," << y << "):" << sh[x][y] << std::endl;
                pause( );
            }
        }
        _aSheet = sh;
    } catch (const std::exception &e) {
        std::cerr << u8"错误：" << e.what( ) << std::endl;
        return;
    }
}