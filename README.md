#	SmartSheet-v0.8.0
###	仓库地址：
GitHub标准仓库地址： [GitHub](https://github.com/lscatfish/SmartSheet.git)  
Gitee国内镜像仓库地址： [Gitee](https://gitee.com/sicheng-liu/SmartSheet.git)    
Gitcode国内镜像仓库地址（推荐使用此仓库，直接下载zip）： [Gitcode](https://gitcode.com/lscatfish/SmartSheet.git)

---

## 🚀 快速开始

### 环境要求
- **操作系统**：
	- windows10（CISC设计的架构），切勿使用ARM
- **依赖工具**：
	- cmake3.20
	- MSVC v143
	- Windows 11 SDK (10.0+)
	- （推荐）VS2022
- **其他依赖**：
	- PaddleOCR v3.1.0 &nbsp;&nbsp;&nbsp; 仓库地址：[Github](https://github.com/PaddlePaddle/PaddleOCR.git)&nbsp;&nbsp;&nbsp;[Gitee](https://gitee.com/paddlepaddle/PaddleOCR.git)
	- ICU v77.1 &nbsp;&nbsp;&nbsp; 仓库地址：[Github](https://github.com/unicode-org/icu.git)&nbsp;&nbsp;&nbsp;*最好下载官方的release版本* ( ^ - ^ ) &nbsp;&nbsp;&nbsp; *这个要配置很多其他文件，看官方文件*
	- opencv v4.1 &nbsp;&nbsp;&nbsp; 仓库地址：[Github](https://github.com/opencv/opencv.git)&nbsp;&nbsp;&nbsp;[Gitee](https://gitee.com/opencv/opencv.git) &nbsp;&nbsp;&nbsp; *最好下载官方的release版本*
	- xlnt v1.6.1 &nbsp;&nbsp;&nbsp; 仓库地址：[Github](https://github.com/xlnt-community/xlnt.git)
	- poppler v25.07.0 &nbsp;&nbsp;&nbsp; 仓库地址：[官方地址](https://poppler.freedesktop.org/)
	- zlib v1.3.1 &nbsp;&nbsp;&nbsp; 仓库地址：[官方地址](https://zlib.net/)
	- ftxui v6.1.9 &nbsp;&nbsp;&nbsp; 仓库地址：[Github](https://github.com/ArthurSonzogni/FTXUI.git) *请手动编译到Rlease静态库*
- **注意事项**：
	请使用temp分支进行开发；如果新增其他可执行文件或者封装层，请使用shared分支进行开发。开发前请联系管理员（2561925435@qq.com）

---

## 🛠️ 安装与配置

### 1.安装第三方依赖
- **PaddleOCR**：请按照官方的文档操作，最后加上自己写的一个API文件（你可以使用我的配置 [source_ppocr_API.h](./API_source_code/source_ppocr_API.h) [source_ppocr_API.cpp](./API_source_code/source_ppocr_API.cpp) ），编译生成ppocr.dll。 
当然，如果可以，请为此项目直接链接PaddleOCR的头文件以及动态链接库。 
*注意：在链接到本项目时需要ppocr.dll以及其附属的dll。* 
- **ftxui**：请下载源码，然后编译生成Release版本的静态链接库。 
- **其他库**：请下载源码，编译生成Release版本的动态链接库；或者直接下载官方的发布版。 
- *为什么要编译Release： 因为PaddleOCR强制要求Release才能正常运行* 

### 2.编译配置
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;如果可以，请使用cmake编译；编译时，需要启用c++20标准。然后确认cmake已经链接上了第三方依赖之后，能够复制相应的dll到exe文件目录  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;这里我使用的是单独链接到每一个第三方库，如果可以，你可以链接到自己的包。  
   
- **dll包含**：
	- PaddleOCR配置API文件后生成的ppocr.dll以及PaddleOCR的附属dll 
	- ICU的所有dll 
	- opencv的所有dll 
	- xlnt的所有dll 
	- poppler的所有dll及其依赖的dll 
	- zlib的所有dll   

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;编译生成Release版本的程序即可。  

## 测试数据集
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;考虑率到测试数据包含极为敏感的个人信息，若实在需要测试数据，请联系[lscatfish](https://github.com/lscatfish)  

## TODO 
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;ppocr希望可以使用动态链接库，而不是直接链接到`.dll`文件。 

---
---
---

# 如何解析docx、pdf、xlsx、图片等文件  
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;在此项目中，我们解析了docx、pdf、xlsx、图片等文件，下面将讲解解析此类文件的思路。 

---

# DOCX
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`.docx` 文件并非单一文本文件，而是基于 **Office Open XML (OOXML)** 标准的**压缩包**，核心由 XML 格式文件、媒体资源和配置文件按固定目录结构组织而成，具体构成规则如下：


## 一、核心本质：压缩包格式
将任意 `.docx` 文件的后缀名改为 `.zip` 并解压，可直接查看其内部所有文件，这是理解其构成的关键前提。


## 二、解压后的核心目录结构
解压后会生成多个文件夹和 XML 文件，其中 **3个核心文件夹** 决定了文档的内容、格式和资源：

| 目录/文件          | 核心作用                                                                 |
|---------------------|--------------------------------------------------------------------------|
| `word/`             | 文档核心内容存储区，包含文本、段落、样式、表格、图片引用等（最重要目录） |
| `docProps/`         | 文档属性信息，如标题、作者、修改时间、字数统计等                         |
| `_rels/`            | 关系文件目录，定义各部分（如文档内容与媒体资源）的关联关系               |
| `[Content_Types].xml` | 根目录唯一关键文件，定义整个压缩包内所有文件的类型（如 XML、图片格式）   |


## 三、关键子目录/文件详解（以 `word/` 为例）
`word/` 目录是 `.docx` 的核心，内部文件直接决定文档内容和格式，主要包含：

| 子文件/子目录       | 作用说明                                                                 |
|---------------------|--------------------------------------------------------------------------|
| `document.xml`      | **文档正文核心**：存储所有文本内容、段落结构、字体样式、表格、列表等（XML 格式） |
| `styles.xml`        | 存储文档的样式定义（如“标题1”“正文”样式的字体、行距、缩进等）             |
| `media/`            | 存储文档中插入的所有媒体资源（如图片、音频、视频，格式多为 jpg/png/mp4） |
| `footnotes.xml`     | 存储文档的脚注内容                                                       |
| `endnotes.xml`      | 存储文档的尾注内容                                                       |
| `_rels/document.xml.rels` | 定义 `document.xml` 与外部资源的关联（如文本中某张图片对应 `media/` 下的哪个文件） |


## 四、核心构成规则总结
1. **压缩包封装**：所有内容通过 ZIP 压缩格式打包，后缀名改为 `.docx` 标识为 Word 文档。
2. **XML 文本存储**：文档的文本、结构、样式均以 **XML 标记语言** 存储（而非二进制），可直接用文本编辑器打开 XML 文件查看。
3. **资源分离存储**：媒体资源（图片、音视频）单独放在 `media/` 目录，正文通过“关系文件”（`*.rels`）引用资源路径，避免正文文件过大。
4. **标准化结构**：目录和文件命名严格遵循 OOXML 国际标准（ISO/IEC 29500），确保不同软件（如 Word、WPS）可兼容解析。

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;例如：当你在 `.docx` 中插入一张图片时，实际发生的是：
- 图片文件被保存到 `word/media/` 目录（如 `image1.jpg`）；
- `word/_rels/document.xml.rels` 中添加一条记录，说明“`document.xml` 中的某段内容引用了 `media/image1.jpg`”；
- `document.xml` 中用 XML 标签标记图片的位置和显示属性。

## 五、表格构成规则 
表格采用**树形层级结构**，从整体到局部依次为：
1. 一个表格由若干行（`tr`）组成；
1. 一行由若干单元格（`tc`）组成；
1. 单元格是表格的最小单位，包含文本、段落、图片等内容。  

表格的结构在 `word/document.xml` 中通过以下核心标签定义：

| 标签               | 作用说明                                                                 |
|--------------------|--------------------------------------------------------------------------|
| `<w:tbl>`          | 表格的根标签，包裹整个表格内容                                           |
| `<w:tblPr>`        | 表格属性设置，如表格宽度、边框样式、对齐方式等                           |
| `<w:tblGrid>`      | 表格列宽定义，包含多个 `<w:gridCol>` 标签，每个标签对应一列的宽度         |
| `<w:tr>`           | 表格行标签，包裹一行中的所有单元格                                       |
| `<w:trPr>`         | 行属性设置，如行高、行对齐方式等                                         |
| `<w:tc>`           | 单元格标签，包裹单元格内的内容                                           |
| `<w:tcPr>`         | 单元格属性设置，如单元格宽度、合并（跨列/跨行）、背景色等                 |
| `<w:p>`            | 单元格内的段落标签（表格内容必须放在段落中，即使是纯文本）                 |

假如有一个表格 

| 姓名  | 年龄  | 职业   |
|-------|-------|--------|
| 张三  |    28 | 工程师   |
| 李四  |    34 |  设计师   |


````` xml
<w:tbl>
  <!-- 表格属性设置 -->
  <w:tblPr>
    <w:tblW w:w="5000" w:type="dxa"/> <!-- 表格总宽度为5000 twentieths of a point -->
    <w:tblBorders>
      <!-- 表格边框样式（简化版） -->
      <w:top w:val="single" w:sz="4" w:space="0" w:color="auto"/>
      <w:left w:val="single" w:sz="4" w:space="0" w:color="auto"/>
      <w:bottom w:val="single" w:sz="4" w:space="0" w:color="auto"/>
      <w:right w:val="single" w:sz="4" w:space="0" w:color="auto"/>
      <w:insideH w:val="single" w:sz="4" w:space="0" w:color="auto"/>
      <w:insideV w:val="single" w:sz="4" w:space="0" w:color="auto"/>
    </w:tblBorders>
  </w:tblPr>

  <!-- 列宽定义（3列） -->
  <w:tblGrid>
    <w:gridCol w:w="1500"/> <!-- 第1列宽度 -->
    <w:gridCol w:w="1000"/> <!-- 第2列宽度 -->
    <w:gridCol w:w="2500"/> <!-- 第3列宽度 -->
  </w:tblGrid>

  <!-- 第1行（表头） -->
  <w:tr>
    <!-- 单元格1：姓名 -->
    <w:tc>
      <w:tcPr><w:tcW w:w="1500" w:type="dxa"/></w:tcPr>
      <w:p>
        <w:r><w:t>姓名</w:t></w:r>
      </w:p>
    </w:tc>
    
    <!-- 单元格2：年龄 -->
    <w:tc>
      <w:tcPr><w:tcW w:w="1000" w:type="dxa"/></w:tcPr>
      <w:p>
        <w:r><w:t>年龄</w:t></w:r>
      </w:p>
    </w:tc>
    
    <!-- 单元格3：职业 -->
    <w:tc>
      <w:tcPr><w:tcW w:w="2500" w:type="dxa"/></w:tcPr>
      <w:p>
        <w:r><w:t>职业</w:t></w:r>
      </w:p>
    </w:tc>
  </w:tr>

  <!-- 第2行（数据行） -->
  <w:tr>
    <w:tc>
      <w:tcPr><w:tcW w:w="1500" w:type="dxa"/></w:tcPr>
      <w:p><w:r><w:t>张三</w:t></w:r></w:p>
    </w:tc>
    <w:tc>
      <w:tcPr><w:tcW w:w="1000" w:type="dxa"/></w:tcPr>
      <w:p><w:r><w:t>28</w:t></w:r></w:p>
    </w:tc>
    <w:tc>
      <w:tcPr><w:tcW w:w="2500" w:type="dxa"/></w:tcPr>
      <w:p><w:r><w:t>工程师</w:t></w:r></w:p>
    </w:tc>
  </w:tr>

  <!-- 第3行（数据行） -->
  <w:tr>
    <w:tc>
      <w:tcPr><w:tcW w:w="1500" w:type="dxa"/></w:tcPr>
      <w:p><w:r><w:t>李四</w:t></w:r></w:p>
    </w:tc>
    <w:tc>
      <w:tcPr><w:tcW w:w="1000" w:type="dxa"/></w:tcPr>
      <w:p><w:r><w:t>32</w:t></w:r></w:p>
    </w:tc>
    <w:tc>
      <w:tcPr><w:tcW w:w="2500" w:type="dxa"/></w:tcPr>
      <w:p><w:r><w:t>设计师</w:t></w:r></w:p>
    </w:tc>
  </w:tr>
</w:tbl>

`````

**对应关系说明**：   
1. 整个表格被 `<w:tbl>` 标签包裹； 
1. `<w:tblGrid>` 定义了 3 列的宽度，与表格的 3 列一一对应； 
1. 每一行对应一个 `<w:tr>` 标签； 
1. 每个单元格对应一个 `<w:tc>` 标签，内部的 `<w:p>` 是段落标签（Word 要求所有内容必须放在段落中）; 
1. 文本内容通过 `<w:t>` 标签存储（如 `<w:t>张三</w:t>` 对应单元格中的 “张三”）。 

## 六、解析方法 

### 核心功能
| 功能模块               | 描述                                                                 |
|------------------------|----------------------------------------------------------------------|
| Docx 文件读取          | 解压 Docx 文件并读取内部核心 XML（如 `word/document.xml`）            |
| 表格解析与位置记录     | 解析 XML 中的表格结构，记录每个单元格的内容、行号（0 开始）、列号（0 开始） |
| 关键表格筛选           | 根据关键词列表（如“姓名”“学号”）模糊匹配并提取目标表格               |
| 人员信息提取           | 从关键表格中自动提取姓名、学号、联系方式等人员信息，并生成标准格式数据 |
| 表格数据打印           | 打印解析出的所有表格及单元格位置信息，便于调试和验证                 |


### 依赖库
使用前需确保项目中已引入以下依赖：
- **XML 解析**：`pugixml`（用于解析 Docx 内部的 XML 结构）
- **压缩/解压**：`zip.h` / `unzip.h`（用于解压 Docx 压缩包）
- **编码转换**：自定义 `Encoding.h`（处理 UTF-8 与系统编码的转换）
- **模糊匹配**：自定义 `Fuzzy.h`（支持关键词模糊搜索表格）
- **基础工具**：`basic.hpp`、`helper.h`（提供字符串处理、数据结构等基础功能）
- **人员信息模型**：`PersonnelInformation.h`（定义 `DefPerson`、`DefLine` 等人员信息结构体）


### 类与数据结构说明

### 1. 核心数据结构
#### `TableCell`（单元格结构体）
存储单个表格单元格的内容与位置信息：  

| 成员变量   | 类型         | 描述                     |
|------------|--------------|--------------------------|
| `content`  | `std::string`| 单元格文本内容           |
| `row`      | `int`        | 单元格所在行号（从 0 开始）|
| `col`      | `int`        | 单元格所在列号（从 0 开始）|


### 2. 核心类 `DefDocx`
#### 构造函数
| 构造函数                          | 描述                                                                 |
|-----------------------------------|----------------------------------------------------------------------|
| `DefDocx(const std::string &_path)` | 核心构造函数：输入 Docx 文件路径（系统编码），自动完成文件读取与表格解析 |
| `DefDocx()`                       | 默认构造函数（无实际操作，需后续调用解析接口）                       |


#### 公共成员函数
| 函数声明                                                                 | 功能描述                                                                 |
|--------------------------------------------------------------------------|--------------------------------------------------------------------------|
| `static std::vector<char> read_docx_file(const std::string &_docx_path, const std::string &_inner_file_path)` | 静态方法：读取 Docx 压缩包内指定文件（如 `word/document.xml`），返回文件二进制数据 |
| `static list<table<TableCell>> parse_tables_with_position(const std::vector<char> &_xml_data)` | 静态方法：解析 XML 数据中的所有表格，返回带位置信息的表格列表             |
| `table<TableCell> get_table_with(const list<std::string> &_u8imp)`       | 根据 UTF-8 关键词列表（如 `{"姓名", "学号"}`）模糊筛选目标表格           |
| `DefPerson get_person()`                                                | 从关键表格中提取人员信息，返回标准 `DefPerson` 结构体                   |
| `list<table<TableCell>> get_table_list() const`                          | 获取解析出的所有表格列表                                                 |
| `void print_tables_with_position()`                                      | 打印所有表格的单元格内容及位置信息（格式：`[行,列]内容`）                |


#### 私有成员变量
| 变量名       | 类型                          | 描述                     |
|--------------|-------------------------------|--------------------------|
| `path_`      | `std::string`                 | Docx 文件路径（系统编码） |
| `u8path_`    | `std::string`                 | Docx 文件路径（UTF-8 编码）|
| `tableList_` | `list<table<TableCell>>`       | 解析出的所有表格列表     |
| `keyTable_`  | `table<TableCell>`             | 筛选出的关键表格（如人员信息表） |


### 使用示例：解析 Docx 并打印表格

```cpp
#include <iostream>
#include "word.h"

int main() {
    // 1. 初始化 DefDocx 对象（传入 Docx 文件路径）
    docx::DefDocx docx_parser("test.docx");

    // 2. 打印所有解析出的表格及单元格位置
    docx_parser.print_tables_with_position();

    return 0;
}
```

# XLSX 
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`.xlsx`文件使用xlnt库解析，详细的使用方式可以参考xlnt库的[官方使用手册](https://github.com/xlnt-community/xlnt) 

---

# PDF

## 一、概述
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;本项目提供了一套PDF解析工具，能够从PDF文件中提取表格结构及文本内容，并支持对特定类型表格（如班委应聘表、普通报名表）进行结构化信息提取，最终生成人员信息数据。
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`.pdf`文件使用采用poppler库的表层和其底层pdf渲染器共同完成。其中表层（cpp对外开放的类）用于文字块的识别，底层pdf渲染器用于识别表格的框线。    
### 存在问题 
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;在对pdf进行渲染的时候，会出现**非矢量线段无法识别**的问题，这点有待改进。此外，应当注意到底层与表层库的对pdf解析后得到的返回坐标存在数值上的误差（误差的来源未知，但是可以用修正量修正）。 
```cpp
/*
 * @brief 填充解析出的表格
 * @param _textBoxList 解析出的文字块
 */
void DefPdf::fill_sheet(const myList< CELL > &_textBoxList) {
    // 填充之前对位置进行修正
    double deltaH = 18000;    // 修正参数
    for (const auto &c : _textBoxList) {
        if (c.text == U8C(u8"姓名")) {
            deltaH = sheet_[0][0].corePoint.y - c.corePoint.y;
            break;
        }
    }

    // 重构_textBoxList
    myList< CELL > thisBox;
    for (const auto &c : _textBoxList) {
        CELL thisCc(c, deltaH);
        thisBox.push_back(thisCc);
    }
    for (const auto &t : thisBox) {
        for (auto &r : sheet_) {
            for (auto &c : r) {
                if (t.is_contained_for_pdf(c)) {
                    c.text = c.text + t.text;
                }
            }
        }
    }
}
```
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;这里我按照第一个框线中的文本应该是`姓名`来将文本块校正到单元格的中心。   

## 二、功能特点
- 提取PDF中的文本块信息
- 识别PDF中的表格线条并构建表格结构
- 支持无表格线时通过文本聚类构建表格
- 对特定类型表格进行结构化解析，提取人员信息（姓名、学号、联系方式等）
- 支持多种表格类型识别（班委应聘表、普通报名表）

## 三、核心组件

### 数据结构定义
- `LineSegment`：表示PDF中的线段，支持水平/垂直/其他类型判断
- `GridPoint`：二维坐标点结构
- `CELL`：表格单元格结构，包含顶点坐标、中心坐标、文本内容等信息
- `myTable`：表格数据结构（二维向量）
- `myList`：行/列数据结构（向量）

### 主要类
- `LineExtractor`：线段提取器，用于从PDF中提取线段信息
- `DefPdf`：PDF解析主类，提供完整的PDF解析功能

## 四、使用方法

### 基本用法
```cpp
// 初始化PDF解析器
pdf::Init();

// 解析PDF文件
chstring pdfPath = "path/to/your/file.pdf";
pdf::DefPdf pdfDoc(pdfPath);

// 检查解析是否成功
if (pdfDoc.isOKed()) {
    // 获取解析出的表格
    auto sheet = pdfDoc.get_sheet();
    
    // 打印表格内容
    pdfDoc.print_sheet();
    
    // 获取人员信息
    DefPerson person = pdfDoc.get_person();
}
```

### 批量提取文本块
```cpp
myList<myList<CELL>> textBlocks;
pdf::DefPdf pdfDoc(pdfPath, textBlocks);

// 处理提取到的文本块
for (const auto& pageBlocks : textBlocks) {
    for (const auto& cell : pageBlocks) {
        // 处理单元格内容
        std::cout << "Text: " << cell.text << std::endl;
    }
}
```

## 五、解析流程
1. **初始化**：调用`pdf::Init()`初始化PDF解析环境
2. **加载文件**：通过`DefPdf`构造函数加载PDF文件
3. **内容提取**：
   - 提取文本块（`extract_textblocks`）
   - 提取线段（`extract_linesegments`）
4. **表格构建**：
   - 基于线段构建表格（`parse_line_to_sheet`）
   - 或基于文本聚类构建表格（`parse_textbox_to_sheet`）
5. **内容填充**：将文本块内容填充到表格单元格（`fill_sheet`）
6. **信息提取**：从表格中提取结构化的人员信息（`get_person`）

## 六、依赖项
- Poppler：PDF渲染库，用于PDF内容提取
- C++标准库

## 七、注意事项
- 输入文件路径需使用UTF-8编码
- 目前主要支持两种表格类型：班委应聘表和普通报名表
- 解析效果受PDF文件质量影响，清晰的表格线和规范的排版能获得更好的解析结果
- 对于无表格线的PDF，通过文本聚类算法构建表格，可能存在一定误差


---
---
---
# 图片解析方式 
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;本项目使用飞桨开发的文字识别库PaddleOCR，编译了一个动态链接文件`ppocr.dll`，使用时直接通过API调用`ppocr.dll`来进行文字识别。 

## [PaddleOCR的使用](https://github.com/PaddlePaddle/PaddleOCR/blob/main/readme/README_cn.md) 
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp; 







---
---
---
---

## 作者累了，休息一下，有什么问题联系作者哦   







