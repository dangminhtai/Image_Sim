/**
 * @file HOGExtractor.cpp
 * @brief Cài đặt đặc trưng Histogram of Oriented Gradients (HOG).
 *
 * Tiền xử lý ảnh (chuyển Grayscale, Gaussian Blur, Equalize Histogram),
 * thay đổi kích thước ảnh về m_targetSize, sau đó sử dụng cv::HOGDescriptor
 * để trích xuất đặc trưng và chuẩn hóa L2 vector kết quả.
 */

#include "HOGExtractor.h"
#include <opencv2/imgproc.hpp>
#include <vector>

// ──────────────────────────────────────────────
// Constructor
// ──────────────────────────────────────────────

/**
 * @brief Khởi tạo cấu hình HOG Descriptor.
 */
HOGExtractor::HOGExtractor(cv::Size targetSize) : m_targetSize(targetSize) {
    // Sử dụng cấu hình mặc định nhưng kích thước winSize phải khớp targetSize
    // winSize, blockSize, blockStride, cellSize, nbins
    m_hog = cv::HOGDescriptor(
        m_targetSize,       // winSize
        cv::Size(16, 16),   // blockSize
        cv::Size(8, 8),     // blockStride
        cv::Size(8, 8),     // cellSize
        9                   // nbins
    );
}

// ──────────────────────────────────────────────
// extract()
// ──────────────────────────────────────────────

/**
 * @brief Rút trích và chuẩn hóa đặc trưng HOG.
 */
cv::Mat HOGExtractor::extract(const cv::Mat& image) const {
    if (image.empty()) return cv::Mat();

    // Tiền xử lý 1: Đổi sang ảnh xám
    cv::Mat gray;
    if (image.channels() == 3) {
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = image.clone();
    }

    // Tiền xử lý 2: Làm mịn ảnh (Gaussian Blur) để khử nhiễu
    cv::GaussianBlur(gray, gray, cv::Size(3, 3), 0);

    // Tiền xử lý 3: Cân bằng Histogram (tùy chọn, giúp nổi bật biên)
    cv::equalizeHist(gray, gray);

    // Chuẩn hóa kích thước để HOG vector có độ dài cố định
    cv::Mat resized;
    cv::resize(gray, resized, m_targetSize);

    // Tính HOG
    std::vector<float> descriptors;
    m_hog.compute(resized, descriptors);

    // Chuyển sang cv::Mat (1 row) kiểu CV_32F
    cv::Mat feat(1, static_cast<int>(descriptors.size()), CV_32F);
    for (int i = 0; i < feat.cols; i++) {
        feat.at<float>(0, i) = descriptors[i];
    }

    // Chuẩn hóa L2 vector đặc trưng
    cv::normalize(feat, feat, 1.0, 0.0, cv::NORM_L2);

    return feat;
}
