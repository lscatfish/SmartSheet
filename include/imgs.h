#pragma once
#ifndef IMGS_H
#define IMGS_H

#include <ppocr_API.h>
#include <vector>
#include <string>

extern ppocr::DefDirs _ppocrDir_;

// 派生自 OCRPredictResult
struct DefSolveOCR : public ppocr::OCRPredictResult {
public:
    double x;      // box的中心点x坐标
    double y;      // box的中心点y坐标
    size_t row;    // 行数，第i行


    // 基于 OCRPredictResult 构造
    DefSolveOCR(ppocr::OCRPredictResult _ocrPR) {
        this->box       = _ocrPR.box;
        this->cls_label = _ocrPR.cls_label;
        this->cls_score = _ocrPR.cls_score;
        this->score     = _ocrPR.score;
        this->text      = _ocrPR.text;
        this->x         = (_ocrPR.box[0][0] + _ocrPR.box[1][0] + _ocrPR.box[2][0] + _ocrPR.box[3][0]) / 4.0;
        this->y         = (_ocrPR.box[0][1] + _ocrPR.box[1][1] + _ocrPR.box[2][1] + _ocrPR.box[3][1]) / 4.0;
        row             = 0;    // 默认为第0行
    }
};

/*
 * @brief 用于读取图片的表格（utf8编码）
 * @param 储存表格的二维数组（按照row，column的形式）
 * @param 文件的路径
 */
void load_sheet_from_img(
    std::vector< std::vector< std::string > > &_aSheet,
    std::string                                _pathAndName);


#endif    // !IMGS_H
