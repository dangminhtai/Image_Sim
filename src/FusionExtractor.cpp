#include "FusionExtractor.h"
#include <opencv2/core.hpp>

FusionExtractor::FusionExtractor(std::shared_ptr<FeatureExtractor> ext1, 
                                 std::shared_ptr<FeatureExtractor> ext2, 
                                 const std::string& name)
    : m_ext1(ext1), m_ext2(ext2), m_name(name) {}

cv::Mat FusionExtractor::extract(const cv::Mat& image) const {
    if (!m_ext1 || !m_ext2) return cv::Mat();

    cv::Mat feat1 = m_ext1->extract(image);
    cv::Mat feat2 = m_ext2->extract(image);

    if (feat1.empty() || feat2.empty()) return cv::Mat();

    // Định dạng lại thành 1 hàng để nối nhau
    cv::Mat f1 = feat1.reshape(1, 1);
    cv::Mat f2 = feat2.reshape(1, 1);

    // Chuyển cùng kiểu dữ liệu nếu khác nhau (đảm bảo đều là CV_32F)
    if (f1.type() != CV_32F) f1.convertTo(f1, CV_32F);
    if (f2.type() != CV_32F) f2.convertTo(f2, CV_32F);

    // Nối 2 vector đặc trưng
    cv::Mat fused;
    cv::hconcat(f1, f2, fused);

    // Chuẩn hóa lại toàn bộ vector bằng L2 norm 
    // Điều này giúp tỷ trọng của 2 đặc trưng được cân bằng hơn
    cv::normalize(fused, fused, 1.0, 0.0, cv::NORM_L2);

    return fused;
}
