#	SmartSheet-v0.8.0
###	仓库地址：
GitHub标准仓库地址： [GitHub](https://github.com/lscatfish/SmartSheet.git)  
Gitee国内镜像仓库地址： [Gitee](https://gitee.com/sicheng-liu/SmartSheet.git)    
Gitcode国内镜像仓库地址：  [Gitcode](https://gitcode.com/lscatfish/SmartSheet.git)

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
	请使用temp分支进行开发，开发前请联系管理员（2561925435@qq.com）

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

---

## 测试数据集
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;考虑率到测试数据包含极为敏感的个人信息，若实在需要测试数据，请联系[lscatfish](https://github.com/lscatfish)  

---

## 如何解析docx、pdf、xlsx等文件   
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;在此项目中，我们解析了docx、pdf、xlsx等文件，下面将讲解解析此类文件的思路。

### DOCX
&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;`.docx` 文件并非单一文本文件，而是基于 **Office Open XML (OOXML)** 标准的**压缩包**，核心由 XML 格式文件、媒体资源和配置文件按固定目录结构组织而成，具体构成规则如下：


#### 一、核心本质：压缩包格式
将任意 `.docx` 文件的后缀名改为 `.zip` 并解压，可直接查看其内部所有文件，这是理解其构成的关键前提。


#### 二、解压后的核心目录结构
解压后会生成多个文件夹和 XML 文件，其中 **3个核心文件夹** 决定了文档的内容、格式和资源：

| 目录/文件          | 核心作用                                                                 |
|---------------------|--------------------------------------------------------------------------|
| `word/`             | 文档核心内容存储区，包含文本、段落、样式、表格、图片引用等（最重要目录） |
| `docProps/`         | 文档属性信息，如标题、作者、修改时间、字数统计等                         |
| `_rels/`            | 关系文件目录，定义各部分（如文档内容与媒体资源）的关联关系               |
| `[Content_Types].xml` | 根目录唯一关键文件，定义整个压缩包内所有文件的类型（如 XML、图片格式）   |


#### 三、关键子目录/文件详解（以 `word/` 为例）
`word/` 目录是 `.docx` 的核心，内部文件直接决定文档内容和格式，主要包含：

| 子文件/子目录       | 作用说明                                                                 |
|---------------------|--------------------------------------------------------------------------|
| `document.xml`      | **文档正文核心**：存储所有文本内容、段落结构、字体样式、表格、列表等（XML 格式） |
| `styles.xml`        | 存储文档的样式定义（如“标题1”“正文”样式的字体、行距、缩进等）             |
| `media/`            | 存储文档中插入的所有媒体资源（如图片、音频、视频，格式多为 jpg/png/mp4） |
| `footnotes.xml`     | 存储文档的脚注内容                                                       |
| `endnotes.xml`      | 存储文档的尾注内容                                                       |
| `_rels/document.xml.rels` | 定义 `document.xml` 与外部资源的关联（如文本中某张图片对应 `media/` 下的哪个文件） |


#### 四、核心构成规则总结
1. **压缩包封装**：所有内容通过 ZIP 压缩格式打包，后缀名改为 `.docx` 标识为 Word 文档。
2. **XML 文本存储**：文档的文本、结构、样式均以 **XML 标记语言** 存储（而非二进制），可直接用文本编辑器打开 XML 文件查看。
3. **资源分离存储**：媒体资源（图片、音视频）单独放在 `media/` 目录，正文通过“关系文件”（`*.rels`）引用资源路径，避免正文文件过大。
4. **标准化结构**：目录和文件命名严格遵循 OOXML 国际标准（ISO/IEC 29500），确保不同软件（如 Word、WPS）可兼容解析。

&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;例如：当你在 `.docx` 中插入一张图片时，实际发生的是：
- 图片文件被保存到 `word/media/` 目录（如 `image1.jpg`）；
- `word/_rels/document.xml.rels` 中添加一条记录，说明“`document.xml` 中的某段内容引用了 `media/image1.jpg`”；
- `document.xml` 中用 XML 标签标记图片的位置和显示属性。

#### 五、表格构成规则 
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

#### 六、解析方法 


