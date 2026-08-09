# Git LFS 移除记录

> 本文件由移除 Git LFS 的 PR 自动生成，用于归档仓库中曾经通过 Git LFS 存储的全部文件信息，
> 便于日后按版本重新获取这些第三方库。

## 背景

仓库 `.gitattributes` 中配置了 `*.dll filter=lfs diff=lfs merge=lfs -text`，
所有第三方库的 DLL 均通过 Git LFS 存储。这些文件本质上是公开第三方库的二进制产物，
可以随时按版本从官方渠道重新获取，没有必要占用仓库的 LFS 配额（GitHub 免费额度仅 1 GB），
因此本仓库决定彻底移除 LFS。

## 存储占用概况

- 当前 `master`（HEAD）中的 LFS 文件：**47 个，共 528.0 MB**
- 全部提交历史中出现过的 LFS 对象（按 SHA256 去重）：**50 个，共 533.8 MB**（即 GitHub 端实际占用的 LFS 存储）

## 当前 HEAD 中的 LFS 文件清单（按第三方库分组）

### PaddleOCR 推理库（6 个文件，共 252.09 MB）

| 文件路径 | SHA256 (前12位) | 大小 (MB) |
|---|---|---|
| `third_party/dll/OCRdlls-Release/common.dll` | `241495e49331` | 0.66 |
| `third_party/dll/OCRdlls-Release/libiomp5md.dll` | `bc9e17190505` | 1.65 |
| `third_party/dll/OCRdlls-Release/mkldnn.dll` | `a25df9358306` | 45.13 |
| `third_party/dll/OCRdlls-Release/mklml.dll` | `e2a7fd93e153` | 88.36 |
| `third_party/dll/OCRdlls-Release/paddle_inference.dll` | `4b1c820e7f46` | 115.55 |
| `third_party/dll/OCRdlls-Release/ppocr.dll` | `98b69e9e6ef1` | 0.76 |

重新获取方式：Paddle Inference 官方 C++ 预测库（paddle_inference）+ OpenMP/MKL 运行库，可从 PaddlePaddle 官网按版本重新下载。

### ICU 77（6 个文件，共 36.71 MB）

| 文件路径 | SHA256 (前12位) | 大小 (MB) |
|---|---|---|
| `third_party/dll/icu/Release/bin64/icudt77.dll` | `34802b95108b` | 30.42 |
| `third_party/dll/icu/Release/bin64/icuin77.dll` | `9c3773bbe7dd` | 3.42 |
| `third_party/dll/icu/Release/bin64/icuio77.dll` | `26273d102b14` | 0.06 |
| `third_party/dll/icu/Release/bin64/icutu77.dll` | `7a4ceba851ab` | 0.36 |
| `third_party/dll/icu/Release/bin64/icuuc77.dll` | `c32696ca01b3` | 2.43 |
| `third_party/dll/icu/Release/bin64/testplug.dll` | `1c02854ecb48` | 0.01 |

重新获取方式：icu4c 77.1 Windows 64位运行库，官方 release 可直接下载。

### OpenCV 4.10.0（5 个文件，共 208.16 MB）

| 文件路径 | SHA256 (前12位) | 大小 (MB) |
|---|---|---|
| `third_party/dll/opencv/bin/opencv_videoio_ffmpeg4100_64.dll` | `3de83b84588b` | 25.17 |
| `third_party/dll/opencv/bin/opencv_videoio_msmf4100_64.dll` | `b704a52411d2` | 0.17 |
| `third_party/dll/opencv/bin/opencv_videoio_msmf4100_64d.dll` | `1f7b802ab490` | 0.58 |
| `third_party/dll/opencv/bin/opencv_world4100.dll` | `e6e31e55b9b3` | 61.66 |
| `third_party/dll/opencv/bin/opencv_world4100d.dll` | `077e763a11f9` | 120.58 |

重新获取方式：OpenCV 4.10.0 Windows 官方预编译包（opencv-4.10.0-windows.exe）。

### Poppler 25.07.0（26 个文件，共 23.53 MB）

| 文件路径 | SHA256 (前12位) | 大小 (MB) |
|---|---|---|
| `third_party/dll/poppler-25.07.0/Library/bin/Lerc.dll` | `20a1654f3a98` | 0.50 |
| `third_party/dll/poppler-25.07.0/Library/bin/cairo.dll` | `2d6c526a13fd` | 0.98 |
| `third_party/dll/poppler-25.07.0/Library/bin/charset.dll` | `c2e0e0c3746c` | 0.01 |
| `third_party/dll/poppler-25.07.0/Library/bin/deflate.dll` | `8604e61b16e0` | 0.17 |
| `third_party/dll/poppler-25.07.0/Library/bin/expat.dll` | `ecdd32d69dc7` | 0.39 |
| `third_party/dll/poppler-25.07.0/Library/bin/fontconfig-1.dll` | `6c9df8a52924` | 0.27 |
| `third_party/dll/poppler-25.07.0/Library/bin/freetype.dll` | `ae926c2e515f` | 0.64 |
| `third_party/dll/poppler-25.07.0/Library/bin/iconv.dll` | `772ed247d3d4` | 1.04 |
| `third_party/dll/poppler-25.07.0/Library/bin/jpeg8.dll` | `dfb8b0ee1476` | 0.64 |
| `third_party/dll/poppler-25.07.0/Library/bin/lcms2.dll` | `1b528c598d71` | 0.54 |
| `third_party/dll/poppler-25.07.0/Library/bin/libcrypto-3-x64.dll` | `f8b6aeea92f9` | 6.98 |
| `third_party/dll/poppler-25.07.0/Library/bin/libcurl.dll` | `e218b04f6a84` | 0.65 |
| `third_party/dll/poppler-25.07.0/Library/bin/libexpat.dll` | `ecdd32d69dc7` | 0.39 |
| `third_party/dll/poppler-25.07.0/Library/bin/liblzma.dll` | `4826969d1704` | 0.18 |
| `third_party/dll/poppler-25.07.0/Library/bin/libpng16.dll` | `6764d45eec6a` | 0.19 |
| `third_party/dll/poppler-25.07.0/Library/bin/libssh2.dll` | `1563642bed85` | 0.25 |
| `third_party/dll/poppler-25.07.0/Library/bin/libtiff.dll` | `a04c460d2b6a` | 0.47 |
| `third_party/dll/poppler-25.07.0/Library/bin/libzstd.dll` | `166f5159df54` | 0.63 |
| `third_party/dll/poppler-25.07.0/Library/bin/openjp2.dll` | `d7647e16d6e7` | 0.34 |
| `third_party/dll/poppler-25.07.0/Library/bin/pixman-1-0.dll` | `242021042d4f` | 0.57 |
| `third_party/dll/poppler-25.07.0/Library/bin/poppler-cpp.dll` | `d2d3f4621336` | 0.17 |
| `third_party/dll/poppler-25.07.0/Library/bin/poppler-glib.dll` | `de7658192163` | 0.43 |
| `third_party/dll/poppler-25.07.0/Library/bin/poppler.dll` | `99e873ad35fd` | 5.92 |
| `third_party/dll/poppler-25.07.0/Library/bin/tiff.dll` | `a04c460d2b6a` | 0.47 |
| `third_party/dll/poppler-25.07.0/Library/bin/zlib.dll` | `7d86de8659d7` | 0.08 |
| `third_party/dll/poppler-25.07.0/Library/bin/zstd.dll` | `166f5159df54` | 0.63 |

重新获取方式：poppler-25.07.0 及其依赖，conda-forge `poppler` 包（Library/bin）。

### xlnt（2 个文件，共 7.15 MB）

| 文件路径 | SHA256 (前12位) | 大小 (MB) |
|---|---|---|
| `third_party/dll/xlnt/Debug/xlntd.dll` | `6df37ba134a5` | 5.46 |
| `third_party/dll/xlnt/Release/xlnt.dll` | `255b98b1edf8` | 1.69 |

重新获取方式：xlnt Excel 读写库，自行编译或 vcpkg 安装。

### zlib（2 个文件，共 0.34 MB）

| 文件路径 | SHA256 (前12位) | 大小 (MB) |
|---|---|---|
| `third_party/dll/zlib/Debug/zlibwapi.dll` | `7e97217c4d67` | 0.24 |
| `third_party/dll/zlib/Release/zlibwapi.dll` | `4101135a81ad` | 0.11 |

重新获取方式：zlibwapi，自行编译或 vcpkg 安装。

## 历史中曾经存在、当前已不在 HEAD 的 LFS 文件

以下为目录结构调整 / 库替换前的历史 LFS 记录，同样占用 LFS 存储，重写历史后将一并清除：

| 文件路径 | SHA256 (前12位) | 大小 (MB) |
|---|---|---|
| `third_party/OCRdlls-Release/common.dll` | `241495e49331` | 0.66 |
| `third_party/OCRdlls-Release/libiomp5md.dll` | `bc9e17190505` | 1.65 |
| `third_party/OCRdlls-Release/mkldnn.dll` | `a25df9358306` | 45.13 |
| `third_party/OCRdlls-Release/mklml.dll` | `e2a7fd93e153` | 88.36 |
| `third_party/OCRdlls-Release/paddle_inference.dll` | `4b1c820e7f46` | 115.55 |
| `third_party/OCRdlls-Release/ppocr.dll` | `33432ec7981f` | 0.76 |
| `third_party/OCRdlls-Release/ppocr.dll` | `98b69e9e6ef1` | 0.76 |
| `third_party/dll/paddle_inference/lib/common.dll` | `241495e49331` | 0.66 |
| `third_party/dll/paddle_inference/lib/paddle_inference.dll` | `4b1c820e7f46` | 115.55 |
| `third_party/dll/pdfium/bin/pdfium.dll` | `9c5d0706e650` | 5.50 |
| `third_party/dll/uchardet/Debug/uchardet.dll` | `7ba43f362135` | 0.46 |
| `third_party/dll/uchardet/Release/uchardet.dll` | `6f99f7ee1ec6` | 0.26 |
| `third_party/dll/zlib/Debug/zlibd.dll` | `30997d82329e` | 0.20 |
| `third_party/dll/zlib/Release/zlib.dll` | `18fcdb106783` | 0.09 |
| `third_party/opencv/bin/opencv_videoio_ffmpeg4100_64.dll` | `3de83b84588b` | 25.17 |
| `third_party/opencv/bin/opencv_videoio_msmf4100_64.dll` | `b704a52411d2` | 0.17 |
| `third_party/opencv/bin/opencv_videoio_msmf4100_64d.dll` | `1f7b802ab490` | 0.58 |
| `third_party/opencv/bin/opencv_world4100.dll` | `e6e31e55b9b3` | 61.66 |
| `third_party/opencv/bin/opencv_world4100d.dll` | `077e763a11f9` | 120.58 |
| `third_party/paddle_inference/lib/common.dll` | `241495e49331` | 0.66 |
| `third_party/paddle_inference/lib/paddle_inference.dll` | `4b1c820e7f46` | 115.55 |
| `third_party/uchardet/Debug/uchardet.dll` | `7ba43f362135` | 0.46 |
| `third_party/uchardet/Release/uchardet.dll` | `6f99f7ee1ec6` | 0.26 |
| `third_party/xlnt/Debug/xlntd.dll` | `6df37ba134a5` | 5.46 |
| `third_party/xlnt/Release/xlnt.dll` | `255b98b1edf8` | 1.69 |

## 本次移除操作与后续步骤

本 PR 完成：

1. 删除 `.gitattributes` 中的 `*.dll filter=lfs diff=lfs merge=lfs -text` 配置，仓库不再使用 LFS；
2. 删除 `third_party/dll/` 下全部 47 个 LFS 指针文件（失去 LFS 后它们只是无用的指针文本）；
3. `.gitignore` 增加 `*.dll`，防止 DLL 被再次提交进仓库；
4. 新增本记录文件归档全部 LFS 文件信息。

**合并本 PR 后，还需在本地执行一次历史重写，GitHub 才会真正回收 LFS 存储空间：**

```bash
# 1. 备份后，用 git filter-repo 从历史中彻底移除所有 dll
git clone --mirror https://github.com/lscatfish/SmartSheet.git
cd SmartSheet.git
git filter-repo --invert-paths --path-glob '*.dll' --force

# 2. 强推覆盖远端历史（会改写所有提交哈希，其他协作者需重新克隆）
git push --force --all
git push --force --tags
```

> 注意：LFS 对象在没有任何分支/标签引用后，GitHub 才会将其回收；
> 若强推后配额仍未释放，可在仓库 Settings → Billing 中确认，或联系 GitHub Support 手动清理。

## 本地开发如何补齐这些 DLL

你本地工作区中已有的真实 DLL 不受本 PR 影响（删除的只是远端指针）。
新的克隆环境可按上表“重新获取方式”下载对应版本的第三方库，
或将现有 DLL 保留在本地 `third_party/dll/` 目录（已被 .gitignore 忽略，不会再进入仓库）。
