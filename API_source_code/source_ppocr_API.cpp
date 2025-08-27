#include "source_ppocr_API.h"
using namespace PaddleOCR;
using namespace std;

bool GetOcr(const cv::Mat& srcimg, std::vector<std::vector<PaddleOCR::OCRPredictResult>>& res) {
	vector<cv::String> cv_all_img_names;
	cv::glob(FLAGS_image_dir, cv_all_img_names);
	// 创建对象
	PPOCR ocr;
	// 是否启用基准测试
	if (FLAGS_benchmark) {
		ocr.reset_timer();
	}
	vector<cv::Mat> img_list;
	std::vector<cv::String> img_names;
	cv::Mat img = srcimg.clone();
	img_list.push_back(img);

	// 执行OCR推理
	res = ocr.ocr(img_list, FLAGS_det, FLAGS_rec, FLAGS_cls);
	std::vector<PaddleOCR::OCRPredictResult> ocr_result;
	std::string info;

	// 处理OCR结果
	for (int i = 0; i < img_list.size(); ++i) {
		// 打印ocr结果
		// Utility::print_result(res[i]);
		// 保存文本检测可视化结果
		if (FLAGS_visualize && FLAGS_det) {
			// std::string file_name = Utility::basename(img_names[i]);
			std::string file_name = Utility::basename("temp.jpg");
			cv::Mat srcimg = img_list[i].clone();
			Utility::VisualizeBboxes(srcimg, res[i], FLAGS_output + "/" + file_name);
		}
	}
	// 启用基准测试
	if (FLAGS_benchmark) {
		ocr.benchmark_log(cv_all_img_names.size());
	}
	if (res.size() > 0) {
		return true;
	}
	return false;
}

extern "C" __declspec(dllexport)
void SetModelDir(const char* det_dir, const char* rec_dir, const char* cls_dir, bool use_cls) {



	FLAGS_det_model_dir = det_dir;
	FLAGS_rec_model_dir = rec_dir;
	FLAGS_cls_model_dir = cls_dir;
	FLAGS_use_angle_cls = use_cls;
}


extern "C" __declspec(dllexport)
void SetRecDictPath(const char* dictPath)
{
	FLAGS_rec_char_dict_path = dictPath;
}

void SetImgDir(char* image_dir) {
	FLAGS_image_dir = image_dir;
}
