
/*
 * @brief 一些辅助功能的函数
 *
 * 作者：lscatfish
 * 邮箱：2561925435@qq.com
 */

#include <algorithm>
#include <basic.hpp>
#include <cctype>    // 用于 std::isdigit
#include <console.h>
#include <cstdlib>
#include <Encoding.h>
#include <helper.h>
#include <ios>
#include <iostream>
#include <limits>
#include <streambuf>
#include <string>
#include <utility>
#include <vector>

/*
 * @brief 按回车键继续
 */
void pause( ) {
    console::clear_input_buffer( );
    std::cout << std::endl;
    std::cout << U8C(u8"请按 Enter 键继续...") << std::endl;
    std::cin.ignore((std::numeric_limits< std::streamsize >::max)( ), '\n');
    // 这里实际上是std::cin.ignore(std::numeric_limits< std::streamsize >::max( ), '\n');
    // 加一个括号防止windows.h的宏污染
}

// 分离字符串，返回一个 pair，第一个元素是中文部分，第二个元素是数字部分
std::pair< std::string, std::string > split_chinese_and_number(const std::string &input) {
    // 从后向前遍历字符串，找到第一个非数字字符的位置
    size_t pos = input.size( );
    while (pos > 0 && std::isdigit(input[pos - 1])) {
        --pos;
    }

    // 分离中文部分和数字部分
    std::string chinesePart = input.substr(0, pos);
    std::string numberPart  = input.substr(pos);

    return { chinesePart, numberPart };
}

// 递归终止函数
void mergeHelper(table< std::string > &result) {}

// 合并两个list<std::string>
list< std::string > merge_two_string_lists(const list< std::string > &list1, const list< std::string > &list2) {
    list< std::string > mergedList = list1;                                // 复制第一个列表
    mergedList.insert(mergedList.end( ), list2.begin( ), list2.end( ));    // 插入第二个列表的元素
    return mergedList;
}

// 清除字符串前后的所有空白字符（包括空格、\t、\n等）
std::string trim_whitespace(const std::string &str) {
    if (str.size( ) == 0) return "";
    // 找到第一个非空白字符的位置
    auto firstNonSpace = std::find_if(str.begin( ), str.end( ),
                                      [](unsigned char c) { return !std::isspace(c); });

    // 如果字符串全是空白字符，返回空字符串
    if (firstNonSpace == str.end( )) {
        return "";
    }

    // 找到最后一个非空白字符的位置
    auto lastNonSpace = std::find_if(str.rbegin( ), str.rend( ), [](unsigned char c) { return !std::isspace(c); }).base( );

    // 从第一个非空白字符截取到最后一个非空白字符
    return std::string(firstNonSpace, lastNonSpace);
}

// 分割字符串，获取'='前后的内容
// 返回值: pair的first是'='前的字符串，second是'='后的字符串
// 如果没有找到'='，则first为原字符串，second为空
std::pair< std::string, std::string > split_by_equal(const std::string &str) {
    if (str.empty( )) return { "", "" };
    // 查找'='的位置
    size_t equalPos = str.find('=');

    // 如果没有找到'='，返回原字符串和空字符串
    if (equalPos == std::string::npos) {
        return { str, "" };
    }

    // 提取'='前面的子字符串
    std::string before = str.substr(0, equalPos);

    // 提取'='后面的子字符串

    std::string after;
    if (equalPos + 1 < str.size( ))
        after = str.substr(equalPos + 1);
    else
        after = "";
    return { before, after };
}

/*
 * @brief 分割字符串，获取一个字符的前后的内容
 * @param str 输入的字符串
 * @param cutter 分割的字符
 */
std::pair< std::string, std::string > split_by(const std::string &str, const char cutter) {
    if (str.empty( )) return { "", "" };
    // 查找cutter的位置
    size_t equalPos = str.find(cutter);

    // 如果没有找到'='，返回原字符串和空字符串
    if (equalPos == std::string::npos) {
        return { str, "" };
    }

    // 提取'='前面的子字符串
    std::string before = str.substr(0, equalPos);

    // 提取'='后面的子字符串
    std::string after;
    if (equalPos + 1 < str.size( ))
        after = str.substr(equalPos + 1);
    else
        after = "";
    return { before, after };
}

// 判断一个字符串是否都是数字
bool is_all_digits(const std::string &s) {
    if (s.empty( )) return false;
    return std::all_of(s.begin( ), s.end( ), [](unsigned char c) { return std::isdigit(c); });
}

// 开始前警告
bool start_warning( ) {
    console::clear_console( );
    std::cout << U8C(u8"请确保已经关闭工作区（input、output与storage文件夹下所有文件都必须关闭）!!!") << std::endl;
    std::cout << std::endl
              << U8C(u8"-程序运行过程中会在output文件夹内生成结果，请勿删除output文件夹!!!") << std::endl;
    std::cout << U8C(u8"-如果output文件夹内有旧的结果，程序会自动覆盖，请注意备份重要数据!!!") << std::endl;
    std::cout << std::endl
              << U8C(u8"详细的使用教程请参看本程序同目录下的“教程”文件") << std::endl;
    std::cout << std::endl
              << U8C(u8"你是否已确认关闭工作区  [Y/n]  （请输入Y以开始程序）:") << std::endl;
    console::clear_input_buffer( );
    std::string yn;
    std::cin >> yn;
    if (yn != "Y" && yn != "y") {
        std::cout << U8C(u8"你未输入Y，程序终止...") << std::endl;
        pause( );
        return false;
    }
    console::clear_input_buffer( );
    return true;
}

/*
 * @brief 对table< string > 进行排序
 * @param _inTable 待排序的表格
 * @param _sortColIndex 依据哪一列进行排序（从0开始计数）
 * @param _ascending 是否升序排序，默认为true（升序）
 * @param _keepFirstCol 是否保持第一列不变，默认为true
 * @param _excludeHeader 是否排除表头（第一行）进行排序，默认为true
 * @return 排序是否成功
 */
bool sort_table_string_by(
    table< std::string > &_inTable,
    size_t                _sortColIndex,
    bool                  _ascending,
    bool                  _keepFirstCol,
    bool                  _excludeHeader) {
    if (_inTable.size( ) <= 1) return false;                   // 如果表格行数小于等于1，无需排序
    if (_sortColIndex >= _inTable[0].size( )) return false;    // 如果排序列索引超出范围，返回false

    /* ==============================lambda===================================== */
    // 比较两列,是否满足排序条件
    auto comparator = [_sortColIndex, _ascending](const std::vector< std::string > &a, const std::vector< std::string > &b) -> bool {
        if (_sortColIndex >= a.size( ) || _sortColIndex >= b.size( )) {
            return false;    // 如果索引超出范围，保持原有顺序
        }
        if (_ascending) {
            return a[_sortColIndex] < b[_sortColIndex];    // 升序
        } else {
            return a[_sortColIndex] > b[_sortColIndex];    // 降序
        }
    };
    // 交换两行
    auto swap_rows = [_keepFirstCol](std::vector< std::string > &a, std::vector< std::string > &b) {
        std::swap(a, b);
        if (_keepFirstCol && a.size( ) > 0 && b.size( ) > 0) {
            std::swap(a[0], b[0]);    // 保持第一列不变
        }
    };
    /* ==============================lambda===================================== */

    size_t startRow = _excludeHeader ? 1 : 0;    // 排除表头则从第二行开始排序
    // 冒泡排序（o2好像可以优化到冒泡）
    for (size_t i = startRow; i < _inTable.size( ); i++) {
        for (size_t j = startRow + 1; j < _inTable.size( ); j++) {
            if (comparator(_inTable[j - 1], _inTable[j])) {
                swap_rows(_inTable[j - 1], _inTable[j]);
                // std::cout << "c";
            }
        }
    }
    return true;
}
