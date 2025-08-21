#include <algorithm>
#include <cmath>
#include <consoleapi2.h>
#include <fstream>
#include <iostream>
#include <poppler-document.h>
#include <poppler-global.h>
#include <poppler-page.h>
#include <poppler-rectangle.h>
#include <string>
#include <vector>
#include <WinNls.h>

using namespace std;
using namespace poppler;

// 确保中文正常显示，设置输出为UTF-8
void setupConsole( ) {
#ifdef _WIN32
    // Windows系统设置控制台编码为UTF-8
    SetConsoleOutputCP(CP_UTF8);
#endif
}

// 提取PDF中的所有文本，返回UTF-8格式
string extractPdfText(const string &pdfPath) {
    document *doc = document::load_from_file(pdfPath);
    if (!doc || doc->is_locked( )) {
        cerr << u8"无法打开PDF文件: " << pdfPath << endl;
        delete doc;
        return "";
    }

    string allText;
    int    numPages = doc->pages( );

    for (int i = 0; i < numPages; ++i) {
        page *pg = doc->create_page(i);
        if (!pg) {
            cerr << u8"无法获取第 " << i + 1 << u8" 页" << endl;
            continue;
        }

        ustring    pageUText = pg->text( );
        byte_array utf8Bytes = pageUText.to_utf8( );
        string     pageText(utf8Bytes.data( ), utf8Bytes.size( ));

        allText += u8"===== 第 " + to_string(i + 1) + u8" 页 =====" + "\n";
        allText += pageText + "\n\n";

        delete pg;
    }

    delete doc;
    return allText;
}

// 文本元素结构，包含文本内容和位置信息
struct TextElement {
    string text;
    double x, y;
    double width, height;

    TextElement(const string &t, double x_, double y_, double w_, double h_)
        : text(t), x(x_), y(y_), width(w_), height(h_) {}
};

// 排序函数
bool compareY(const TextElement &a, const TextElement &b) {
    return a.y < b.y;    // PDF坐标系中，Y轴向下递增
}

bool compareX(const TextElement &a, const TextElement &b) {
    return a.x < b.x;
}

// 提取页面中的文本元素（带位置信息）
vector< TextElement > getTextElements(page *pg) {
    vector< TextElement > elements;

    // 获取文本框列表（vector<text_box>）
    vector< text_box > text_boxes = pg->text_list( );

    for (const auto &tb : text_boxes) {
        // 1. 获取文本内容（使用text()方法，返回ustring）
        ustring    uText     = tb.text( );
        byte_array utf8Bytes = uText.to_utf8( );
        string     text(utf8Bytes.data( ), utf8Bytes.size( ));

        if (!text.empty( )) {
            // 2. 获取边界框（根据实际定义使用bbox()方法，返回rectf）
            rectf bbox = tb.bbox( );

            // 3. 从rectf中提取坐标信息
            double x      = bbox.x( );
            double y      = bbox.y( );
            double width  = bbox.width( );
            double height = bbox.height( );

            elements.emplace_back(text, x, y, width, height);
        }
    }

    return elements;
}

// 尝试从页面中提取表格
vector< vector< string > > extractTableFromPage(page *pg) {
    vector< vector< string > > table;
    vector< TextElement >      elements = getTextElements(pg);

    if (elements.empty( )) {
        return table;
    }

    // 按Y坐标排序，将文本元素分组为行
    sort(elements.begin( ), elements.end( ), compareY);

    const double                    Y_TOLERANCE = 2.0;    // 垂直方向容差，用于判断是否为同一行
    vector< vector< TextElement > > rows;
    vector< TextElement >           currentRow;

    currentRow.push_back(elements[0]);

    for (size_t i = 1; i < elements.size( ); ++i) {
        double yDiff = abs(elements[i].y - currentRow[0].y);
        if (yDiff < Y_TOLERANCE) {
            currentRow.push_back(elements[i]);
        } else {
            // 对当前行按X坐标排序
            sort(currentRow.begin( ), currentRow.end( ), compareX);
            rows.push_back(currentRow);
            currentRow.clear( );
            currentRow.push_back(elements[i]);
        }
    }

    // 添加最后一行
    if (!currentRow.empty( )) {
        sort(currentRow.begin( ), currentRow.end( ), compareX);
        rows.push_back(currentRow);
    }

    // 转换为字符串表格
    for (const auto &row : rows) {
        vector< string > tableRow;
        for (const auto &elem : row) {
            tableRow.push_back(elem.text);
        }
        table.push_back(tableRow);
    }

    return table;
}

// 提取PDF中的所有表格
vector< pair< int, vector< vector< string > > > > extractPdfTables(const string &pdfPath) {
    vector< pair< int, vector< vector< string > > > > allTables;

    document *doc = document::load_from_file(pdfPath);
    if (!doc || doc->is_locked( )) {
        cerr << u8"无法打开PDF文件: " << pdfPath << endl;
        delete doc;
        return allTables;
    }

    int numPages = doc->pages( );

    for (int i = 0; i < numPages; ++i) {
        page *pg = doc->create_page(i);
        if (!pg) {
            cerr << u8"无法获取第 " << i + 1 << u8" 页" << endl;
            continue;
        }

        vector< vector< string > > table = extractTableFromPage(pg);
        if (!table.empty( )) {
            allTables.emplace_back(i + 1, table);
        }

        delete pg;
    }

    delete doc;
    return allTables;
}

// 保存表格到CSV
void saveTablesToCsv(const vector< pair< int, vector< vector< string > > > > &tables, const string &csvPath) {
    ofstream outFile(csvPath, ios::out | ios::binary);
    if (!outFile.is_open( )) {
        cerr << u8"无法打开CSV输出文件: " << csvPath << endl;
        return;
    }

    for (const auto &[pageNum, table] : tables) {
        outFile << u8"===== 第 " << pageNum << u8" 页 表格 =====" << endl;
        for (const auto &row : table) {
            for (size_t i = 0; i < row.size( ); ++i) {
                outFile << row[i];
                if (i < row.size( ) - 1) {
                    outFile << ",";
                }
            }
            outFile << endl;
        }
        outFile << endl;
    }

    outFile.close( );
    cout << u8"表格已保存至: " << csvPath << endl;
}

void tmain( ) {
    setupConsole( );

    string pdfPath         = u8"./测.pdf";
    string textOutputPath  = u8"./000.txt";
    string tableOutputPath = u8"./000.csv";

    // 提取文本内容
    string textContent = extractPdfText(pdfPath);
    if (!textContent.empty( )) {
        ofstream textFile(textOutputPath, ios::out | ios::binary);
        if (textFile.is_open( )) {
            textFile << textContent;
            textFile.close( );
            cout << u8"文本内容已保存至: " << textOutputPath << endl;
        } else {
            cerr << u8"无法打开文本输出文件: " << textOutputPath << endl;
        }
    }

    // 提取表格内容
    auto tables = extractPdfTables(pdfPath);
    if (!tables.empty( )) {
        saveTablesToCsv(tables, tableOutputPath);
    } else {
        cout << u8"未在PDF中发现表格" << endl;
    }

    return;
}
