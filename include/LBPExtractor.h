/**
 * @file LBPExtractor.h
 * @brief Rút trích đặc trưng vân bề mặt (Texture) bằng Local Binary Pattern (LBP).
 */
#pragma once

#include "FeatureExtractor.h"

/**
 * @class LBPExtractor
 * @brief Rút trích đặc trưng vân bề mặt (Texture) bằng Local Binary Pattern (LBP).
 *
 * Thuật toán LBP cơ bản quét qua từng pixel, so sánh giá trị cường độ sáng
 * của nó với 8 pixel xung quanh để tạo ra một chuỗi nhị phân 8-bit (giá trị 0-255).
 * Kết quả là một histogram tần suất xuất hiện của 256 mẫu LBP này.
 */
class LBPExtractor : public FeatureExtractor {
public:
    /**
     * @brief Khởi tạo LBPExtractor mặc định.
     */
    LBPExtractor() = default;

    /**
     * @brief Rút trích đặc trưng LBP histogram từ ảnh.
     * @param image Ảnh đầu vào (sẽ được chuyển sang Grayscale nếu là ảnh màu).
     * @return Vector đặc trưng kích thước 256 chiều, kiểu CV_32F, đã chuẩn hóa L1.
     */
    cv::Mat extract(const cv::Mat& image) const override;

    /**
     * @brief Trả về tên phương pháp.
     * @return "LBP"
     */
    std::string getName() const override { return "LBP"; }

    /**
     * @brief Trả về số chiều vector đặc trưng LBP = 256.
     * @return 256
     */
    int getDimension() const override { return 256; }
};
