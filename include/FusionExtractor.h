/**
 * @file FusionExtractor.h
 * @brief Early Fusion: Kết hợp 2 đặc trưng lại với nhau để tạo thành một vector đặc trưng chung.
 */
#pragma once

#include "FeatureExtractor.h"
#include <memory>

class FusionExtractor : public FeatureExtractor {
public:
    /**
     * @brief Khởi tạo FusionExtractor từ 2 extractor khác.
     * @param ext1 Extractor 1 (VD: ColorHistogram)
     * @param ext2 Extractor 2 (VD: HOGExtractor)
     * @param name Tên của phương pháp lai này (VD: "Color+HOG")
     */
    FusionExtractor(std::shared_ptr<FeatureExtractor> ext1, 
                    std::shared_ptr<FeatureExtractor> ext2, 
                    const std::string& name = "Fusion");

    cv::Mat extract(const cv::Mat& image) const override;
    std::string getName() const override { return m_name; }
    int getDimension() const override { return -1; }

private:
    std::shared_ptr<FeatureExtractor> m_ext1;
    std::shared_ptr<FeatureExtractor> m_ext2;
    std::string m_name;
};
