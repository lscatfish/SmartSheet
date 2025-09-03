#	SmartSheet-v0.8.0
###	仓库地址：
GitHub标准仓库地址： [GitHub](https://github.com/lscatfish/SmartSheet.git "访问 GitHub 官网")  
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
