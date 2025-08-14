#pragma once

#ifndef TEST_H
#define TEST_H


//打开，开启各子函数的测试输出功能
#if flase
#define DO_TEST
#endif    // true

void test_main( );

//测试imgs的网格生成模型
bool test_for_grid( );

// 测试load_sheet_from_img
void test_for__load_sheet_from_img( );

//测试uchardet
void test_for_uchardet( );


#endif    // !TEST_H
