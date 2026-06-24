/**
 * @file LBPExtractor.h
 * @brief Rút trích đặc trưng vân bề mặt (Texture) bằng Local Binary Pattern (LBP).
 */
#pragma once

#include "FeatureExtractor.h"

class LBPExtractor : public FeatureExtractor {
public:
    LBPExtractor() = default;

    cv::Mat extract(const cv::Mat& image) const override;
    std::string getName() const override { return "LBP"; }
    int getDimension() const override { return 256; }
};
