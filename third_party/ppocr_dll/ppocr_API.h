#pragma once
#include "opencv2/core.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc.hpp"
#include <gflags/gflags.h>
#include<glog/logging.h>
#include <include/args.h>
#include <include/paddleocr.h>
#include <include/paddlestructure.h>
#include <iostream>
#include <opencv2/core/cvstd.hpp>
#include <Windows.h>

#define DLL_API_EXPORT _declspec(dllexport)

struct OCRPredictResult {
	std::vector<std::vector<int>> box;
	std::string text;
	float score = -1.0;
	float cls_score;
	int cls_label = -1;
};

static char buff[1024];
extern "C" DLL_API_EXPORT void SetImgDir(char* image_dir);
extern "C" DLL_API_EXPORT bool GetOcr(const cv::Mat& srcimg, std::vector<std::vector<PaddleOCR::OCRPredictResult>>& res);

extern "C" DLL_API_EXPORT void SetModelDir(
	const char* det_dir,
	const char* rec_dir,
	const char* cls_dir,
	bool use_cls
);

extern "C" DLL_API_EXPORT void SetRecDictPath(const char* dictPath);

// DllMain：DLL 被加载/卸载时自动调用
BOOL APIENTRY DllMain(HMODULE hModule,
	DWORD  ul_reason_for_call,
	LPVOID lpReserved);