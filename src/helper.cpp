
/*
 * @brief 一些辅助功能的函数
 *
 * 作者：lscatfish
 * 邮箱：2561925435@qq.com
 */

#include <algorithm>
#include <basic.hpp>
#include <cctype>    // 用于 std::isdigit
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
#include<console.h>

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
    list< std::string > mergedList = list1;    // 复制第一个列表
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




// 开始前警告
bool start_warning( ) {
    console::clearConsole( );
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
