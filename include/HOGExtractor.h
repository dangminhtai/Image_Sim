/**
 * @file HOGExtractor.h
 * @brief Rút trích đặc trưng hình dáng/biên cạnh bằng Histogram of Oriented Gradients (HOG).
 */
#pragma once

#include "FeatureExtractor.h"
#include <opencv2/objdetect.hpp>

class HOGExtractor : public FeatureExtractor {
public:
    /**
     * @brief Khởi tạo HOG Extractor với kích thước chuẩn hóa.
     * @param targetSize Kích thước ảnh đầu vào cho HOG (VD: 64x64). Càng lớn vector càng dài.
     */
    HOGExtractor(cv::Size targetSize = cv::Size(64, 64));

    cv::Mat extract(const cv::Mat& image) const override;
    std::string getName() const override { return "HOG"; }
    int getDimension() const override { return -1; }

private:
    cv::Size m_targetSize;
    mutable cv::HOGDescriptor m_hog;
};
