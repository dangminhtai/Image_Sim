/**
 * @file HOGExtractor.h
 * @brief Rút trích đặc trưng hình dáng/biên cạnh bằng Histogram of Oriented Gradients (HOG).
 */
#pragma once

#include "FeatureExtractor.h"
#include <opencv2/objdetect.hpp>

/**
 * @class HOGExtractor
 * @brief Rút trích đặc trưng hình dáng/biên cạnh bằng Histogram of Oriented Gradients (HOG).
 *
 * Thuật toán HOG chuẩn hóa ảnh về một kích thước cố định (ví dụ 64x64),
 * chia ảnh thành các ô (cells) và khối (blocks), tính toán gradient định hướng
 * cho từng pixel và tổng hợp thành các histogram địa phương. Các histogram
 * được nối lại tạo thành một vector đặc trưng duy nhất, rất hiệu quả
 * cho việc nhận dạng hình dáng và đối tượng.
 */
class HOGExtractor : public FeatureExtractor {
public:
    /**
     * @brief Khởi tạo HOG Extractor với kích thước chuẩn hóa ảnh.
     * @param targetSize Kích thước ảnh đầu vào cho HOG (Mặc định: 64x64). Càng lớn vector càng dài.
     */
    HOGExtractor(cv::Size targetSize = cv::Size(64, 64));

    /**
     * @brief Rút trích đặc trưng HOG từ ảnh.
     * @param image Ảnh BGR đầu vào.
     * @return Vector đặc trưng 1 chiều kiểu CV_32F. Kích thước phụ thuộc vào targetSize.
     */
    cv::Mat extract(const cv::Mat& image) const override;

    /**
     * @brief Trả về tên phương pháp.
     * @return "HOG"
     */
    std::string getName() const override { return "HOG"; }

    /**
     * @brief Trả về số chiều của vector HOG.
     * @return -1 (Kích thước phụ thuộc vào cv::Size targetSize và cài đặt block/cell mặc định).
     */
    int getDimension() const override { return -1; }

private:
    cv::Size m_targetSize;               ///< Kích thước ảnh chuẩn hóa trước khi trích xuất.
    mutable cv::HOGDescriptor m_hog;     ///< Đối tượng HOG của OpenCV (mutable vì compute() không const).
};
