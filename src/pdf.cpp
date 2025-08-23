
#include <basic.hpp>
#include <cstdlib>
#include <iostream>
#include <memory>
#include <pdf.h>
#include <poppler/cpp/poppler-document.h>
#include <poppler/cpp/poppler-global.h>
#include <poppler/cpp/poppler-page.h>
#include <poppler/cpp/poppler-rectangle.h>
#include <poppler/cpp/poppler-version.h>
#include <poppler/Gfx.h>
#include <poppler/GfxState.h>
#include <poppler/GlobalParams.h>
#include <poppler/goo/GooString.h>
#include <poppler/OutputDev.h>
#include <poppler/Page.h>
#include <poppler/PDFDoc.h>
#include <string>
#include <vector>

// pdf标准坐标系是左下角为原点，y轴向上
namespace pdf {

// pdf启动函数
void Init( ) {
    // 初始化 Poppler
    globalParams = std::make_unique< GlobalParams >( );
}

// 定义线段结构体
struct LineSegment {
    double x1, y1, x2, y2;
};


// 此类用于处理pdf文件
class DefPdf {
public:
    DefPdf( );
    ~DefPdf( );

private:
};

DefPdf::DefPdf( ) {
}

DefPdf::~DefPdf( ) {
}



}    // namespace pdf
