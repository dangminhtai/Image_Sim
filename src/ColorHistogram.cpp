/**
 * @file ColorHistogram.cpp
 * @brief Cài đặt phương thức rút trích đặc trưng Color Histogram trong không gian HSV.
 *
 * Quy trình:
 *  1. Chuyển ảnh BGR sang HSV.
 *  2. Tính histogram cho từng kênh H, S, V.
 *  3. Chuẩn hóa từng histogram theo L2.
 *  4. Nối 3 histogram thành 1 vector đặc trưng.
 */

#include "ColorHistogram.h"
#include <opencv2/imgproc.hpp>
#include <stdexcept>

// ──────────────────────────────────────────────
// Constructor
// ──────────────────────────────────────────────

/**
 * @brief Khởi tạo với số bin cho mỗi kênh màu.
 */
ColorHistogram::ColorHistogram(int binsH, int binsS, int binsV)
    : m_binsH(binsH), m_binsS(binsS), m_binsV(binsV)
{
    if (binsH <= 0 || binsS <= 0 || binsV <= 0) {
        throw std::invalid_argument("Số bin phải lớn hơn 0.");
    }
}

// ──────────────────────────────────────────────
// extract()
// ──────────────────────────────────────────────

/**
 * @brief Rút trích đặc trưng Color Histogram từ ảnh BGR.
 *
 * Chuỗi xử lý:
 *  BGR → HSV → calcHist(H) + calcHist(S) + calcHist(V) → normalize → concat
 */
cv::Mat ColorHistogram::extract(const cv::Mat& image) const
{
    // Kiểm tra đầu vào
    if (image.empty()) {
        return cv::Mat();
    }

    // Bước 1: Chuyển sang không gian màu HSV
    cv::Mat hsvImage;
    cv::cvtColor(image, hsvImage, cv::COLOR_BGR2HSV);

    // Tách 3 kênh H, S, V
    std::vector<cv::Mat> channels;
    cv::split(hsvImage, channels);

    // Bước 2: Tính histogram cho từng kênh
    // Kênh H: phạm vi [0, 180) trong OpenCV
    // Kênh S, V: phạm vi [0, 256)
    cv::Mat histH, histS, histV;

    float rangeH[] = {0.0f, 180.0f};
    float rangeSV[] = {0.0f, 256.0f};
    const float* rangeH_ptr = rangeH;
    const float* rangeSV_ptr = rangeSV;

    int binsH = m_binsH, binsS = m_binsS, binsV = m_binsV;

    // Tính histogram kênh H
    cv::calcHist(&channels[0], 1, nullptr, cv::Mat(), histH,
                 1, &binsH, &rangeH_ptr);

    // Tính histogram kênh S
    cv::calcHist(&channels[1], 1, nullptr, cv::Mat(), histS,
                 1, &binsS, &rangeSV_ptr);

    // Tính histogram kênh V
    cv::calcHist(&channels[2], 1, nullptr, cv::Mat(), histV,
                 1, &binsV, &rangeSV_ptr);

    // Bước 3: Chuẩn hóa L2 từng histogram
    cv::normalize(histH, histH, 1.0, 0.0, cv::NORM_L2);
    cv::normalize(histS, histS, 1.0, 0.0, cv::NORM_L2);
    cv::normalize(histV, histV, 1.0, 0.0, cv::NORM_L2);

    // Bước 4: Reshape về dạng 1 chiều và nối lại
    histH = histH.reshape(1, 1); // shape: (1, binsH)
    histS = histS.reshape(1, 1); // shape: (1, binsS)
    histV = histV.reshape(1, 1); // shape: (1, binsV)

    cv::Mat featureVec;
    cv::hconcat(std::vector<cv::Mat>{histH, histS, histV}, featureVec);

    return featureVec; // shape: (1, binsH + binsS + binsV), CV_32F
}

// ──────────────────────────────────────────────
// getName() / getDimension()
// ──────────────────────────────────────────────

/**
 * @brief Trả về tên phương pháp.
 */
std::string ColorHistogram::getName() const
{
    return "ColorHistogram";
}

/**
 * @brief Trả về số chiều vector đặc trưng.
 */
int ColorHistogram::getDimension() const
{
    return m_binsH + m_binsS + m_binsV;
}
