#pragma once

#ifndef TEST_H
#define TEST_H


// 打开，开启各子函数的测试输出功能
#if flase
#define DO_TEST
#endif    // true

void test_main( );

// 测试ENCODING
void test_for_ENCODING( );

// 测试chstring
void test_for_chstring( );

// 测试opencv的imread
void test_for_cv_imread( );

// 测试融合sheet函数
void test_for_mergeMultipleSheets( );

// 测试ppocr
void test_for_ppocr( );

// 测试解析docx文件的minizip与pugixml
int test_for_docx( );

// 测试DefFolder
void test_for_DefFolder( );

// 测试文件夹的检测
void test_for_check_and_create_folder( );

// 测试icu_encoding_handler
void test_for_icu_encoding_handler( );


#endif    // !TEST_H
