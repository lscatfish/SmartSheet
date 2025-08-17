
#include <basic.hpp>
#include <cctype>    // 用于 std::isdigit
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

    std::streambuf *sb = std::cin.rdbuf( );
    while (sb->in_avail( ) > 0) {
        sb->sbumpc( );    // 读取并丢弃一个字符
    }

    std::cout << std::endl;
    std::cout << u8"请按 Enter 键继续..." << std::endl;
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
