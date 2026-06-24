/**
 * @file ColorHistogram.h
 * @brief Rút trích đặc trưng màu sắc bằng phương pháp Color Histogram.
 *
 * Ảnh được chuyển sang không gian màu HSV, sau đó tính histogram
 * cho từng kênh H, S, V. Ba histogram được chuẩn hóa và nối lại
 * thành một vector đặc trưng duy nhất.
 *
 * Tham khảo: https://docs.opencv.org/4.x/d8/dbc/tutorial_histogram_calculation.html
 */
#pragma once

#include "FeatureExtractor.h"

/**
 * @class ColorHistogram
 * @brief Rút trích đặc trưng Color Histogram trong không gian màu HSV.
 *
 * Số bin (bins) cho mỗi kênh có thể cấu hình khi khởi tạo.
 * Kích thước vector đặc trưng = binsH + binsS + binsV.
 */
class ColorHistogram : public FeatureExtractor {
public:
    /**
     * @brief Khởi tạo ColorHistogram với số bin cho từng kênh màu.
     * @param binsH Số bin kênh Hue (mặc định 32).
     * @param binsS Số bin kênh Saturation (mặc định 32).
     * @param binsV Số bin kênh Value/Brightness (mặc định 32).
     */
    explicit ColorHistogram(int binsH = 32, int binsS = 32, int binsV = 32);

    /**
     * @brief Rút trích đặc trưng histogram màu HSV.
     * @param image Ảnh BGR đầu vào.
     * @return Vector đặc trưng kích thước (binsH + binsS + binsV), kiểu CV_32F, đã chuẩn hóa.
     */
    cv::Mat extract(const cv::Mat& image) const override;

    /**
     * @brief Trả về tên phương pháp.
     * @return "ColorHistogram"
     */
    std::string getName() const override;

    /**
     * @brief Trả về số chiều vector đặc trưng = binsH + binsS + binsV.
     */
    int getDimension() const override;

private:
    int m_binsH; ///< Số bin kênh Hue
    int m_binsS; ///< Số bin kênh Saturation
    int m_binsV; ///< Số bin kênh Value
};
