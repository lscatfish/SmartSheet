#pragma once

#ifndef HELPER_H

/*
 * @brief 按回车键继续
 */
void pause( );

// 分离字符串，返回一个 pair，第一个元素是中文部分，第二个元素是数字部分
std::pair< std::string, std::string > split_chinese_and_number(const std::string &input);

#endif    // !HELPER_H