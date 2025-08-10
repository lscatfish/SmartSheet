#include <ChineseEncoding.h>
#include <helper.h>
#include <ios>
#include <iostream>
#include <limits>
#include <streambuf>

/*
 * @brief 按回车键继续
 */
void pause( ) {

    std::streambuf *sb    = std::cin.rdbuf( );
    while (sb->in_avail( ) > 0) {
        sb->sbumpc( );    // 读取并丢弃一个字符
    }

    std::cout << std::endl;
    std::cout << anycode_to_utf8("请按 Enter 键继续...") << std::endl;
    std::cin.ignore((std::numeric_limits< std::streamsize >::max)( ), '\n');
    // 这里实际上是std::cin.ignore(std::numeric_limits< std::streamsize >::max( ), '\n');
    // 加一个括号防止windows.h的宏污染
}
