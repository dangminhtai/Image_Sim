/**
 * @file FusionExtractor.h
 * @brief Early Fusion: Kết hợp 2 đặc trưng lại với nhau để tạo thành một vector đặc trưng chung.
 */
#pragma once

#include "FeatureExtractor.h"
#include <memory>

/**
 * @class FusionExtractor
 * @brief Áp dụng kỹ thuật Early Fusion để ghép nối hai vector đặc trưng.
 *
 * Cho phép kết hợp sức mạnh của nhiều phương pháp, ví dụ như kết hợp
 * đặc trưng màu sắc (ColorHistogram) và đặc trưng hình dáng (HOG)
 * để tạo ra một vector đặc trưng chung mạnh mẽ hơn. Các vector con
 * sẽ được ghép nối (concatenate) lại với nhau.
 */
class FusionExtractor : public FeatureExtractor {
public:
    /**
     * @brief Khởi tạo FusionExtractor từ 2 extractor khác.
     * @param ext1 Con trỏ chia sẻ tới Extractor 1 (VD: ColorHistogram).
     * @param ext2 Con trỏ chia sẻ tới Extractor 2 (VD: HOGExtractor).
     * @param name Tên định danh của phương pháp lai (VD: "Color_HOG").
     */
    FusionExtractor(std::shared_ptr<FeatureExtractor> ext1, 
                    std::shared_ptr<FeatureExtractor> ext2, 
                    const std::string& name = "Fusion");

    /**
     * @brief Rút trích và ghép nối đặc trưng.
     * @param image Ảnh đầu vào.
     * @return Vector đặc trưng mới (chiều dài = ext1_dim + ext2_dim), kiểu CV_32F.
     */
    cv::Mat extract(const cv::Mat& image) const override;

    /**
     * @brief Trả về tên phương pháp.
     * @return Chuỗi tên định danh của phương pháp lai này.
     */
    std::string getName() const override { return m_name; }

    /**
     * @brief Trả về tổng số chiều của vector lai.
     * @return -1 (Bởi vì phụ thuộc vào kích thước của các extractor con).
     */
    int getDimension() const override { return -1; }

private:
    std::shared_ptr<FeatureExtractor> m_ext1;
    std::shared_ptr<FeatureExtractor> m_ext2;
    std::string m_name;
};
