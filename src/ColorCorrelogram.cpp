/**
 * @file ColorCorrelogram.cpp
 * @brief Cài đặt đặc trưng Color Correlogram.
 *
 * Thuật toán dựa trên paper:
 * Jing Huang et al., "Image Indexing Using Color Correlograms", CVPR 1997.
 *
 * Quy trình đơn giản hóa (Auto-correlogram):
 *  1. Lượng hóa ảnh BGR về numColors màu đại diện bằng k-means.
 *  2. Với mỗi khoảng cách d và mỗi màu c:
 *     P(c, d) = số cặp pixel (p1, p2) cách nhau d và cùng màu c
 *              / tổng số cặp có thể (xấp xỉ bằng số pixel màu c × số pixel lân cận).
 *  3. Flatten P thành vector đặc trưng và chuẩn hóa L2.
 *
 * Ghi chú: Phiên bản đầy đủ là cross-correlogram (mọi cặp màu), nhưng
 * auto-correlogram (chỉ cùng màu) đã cho kết quả tốt và nhẹ hơn nhiều.
 */

#include "ColorCorrelogram.h"
#include <opencv2/imgproc.hpp>
#include <cmath>
#include <algorithm>

// ──────────────────────────────────────────────
// Constructor
// ──────────────────────────────────────────────

/**
 * @brief Khởi tạo ColorCorrelogram.
 */
ColorCorrelogram::ColorCorrelogram(int numColors, std::vector<int> distances)
    : m_numColors(numColors), m_distances(std::move(distances))
{
}

// ──────────────────────────────────────────────
// quantizeColor() — lượng hóa ảnh về numColors màu
// ──────────────────────────────────────────────

/**
 * @brief Lượng hóa ảnh BGR về m_numColors màu đại diện.
 *
 * Dùng k-means clustering trên các giá trị pixel.
 *
 * @return Ảnh chỉ số màu (CV_32S), mỗi pixel là chỉ số [0..numColors-1].
 */
cv::Mat ColorCorrelogram::quantizeColor(const cv::Mat& image) const
{
    // Reshape ảnh thành danh sách pixel (N × 3 float)
    cv::Mat samples = image.reshape(1, image.rows * image.cols);
    samples.convertTo(samples, CV_32F);

    // K-means clustering
    cv::Mat labels, centers;
    cv::kmeans(samples, m_numColors, labels,
               cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 10, 1.0),
               3, cv::KMEANS_PP_CENTERS, centers);

    // Reshape labels về kích thước ảnh (H × W, CV_32S)
    labels = labels.reshape(1, image.rows);
    labels.convertTo(labels, CV_32S);
    return labels;
}

// ──────────────────────────────────────────────
// extract()
// ──────────────────────────────────────────────

/**
 * @brief Rút trích đặc trưng Color Correlogram.
 *
 * Dùng auto-correlogram: chỉ tính xác suất cùng màu cách nhau d pixel.
 * Vector kết quả kích thước = numColors × len(distances).
 */
cv::Mat ColorCorrelogram::extract(const cv::Mat& image) const
{
    if (image.empty()) {
        return cv::Mat();
    }

    // Bước 1: Lượng hóa màu
    cv::Mat colorMap = quantizeColor(image);
    int rows = colorMap.rows;
    int cols = colorMap.cols;

    // Bước 2: Tính auto-correlogram
    // correlo[d_idx][color] = số lần pixel màu c có hàng xóm cách d cùng màu c
    int D = static_cast<int>(m_distances.size());
    std::vector<std::vector<double>> correlo(D, std::vector<double>(m_numColors, 0.0));
    std::vector<double> colorCount(m_numColors, 0.0);

    // Đếm số pixel của mỗi màu
    for (int r = 0; r < rows; r++) {
        const int* rowPtr = colorMap.ptr<int>(r);
        for (int c = 0; c < cols; c++) {
            colorCount[rowPtr[c]]++;
        }
    }

    // Với mỗi khoảng cách d, kiểm tra 4 hướng (ngang, dọc, 2 chéo)
    // để đảm bảo hiệu quả — chỉ kiểm tra hướng ngang và dọc (+d)
    for (int di = 0; di < D; di++) {
        int d = m_distances[di];
        // Hướng ngang (right)
        for (int r = 0; r < rows; r++) {
            const int* rowPtr = colorMap.ptr<int>(r);
            for (int c = 0; c < cols - d; c++) {
                if (rowPtr[c] == rowPtr[c + d]) {
                    correlo[di][rowPtr[c]]++;
                }
            }
        }
        // Hướng dọc (down)
        for (int r = 0; r < rows - d; r++) {
            const int* rowPtr = colorMap.ptr<int>(r);
            const int* rowPtrD = colorMap.ptr<int>(r + d);
            for (int c = 0; c < cols; c++) {
                if (rowPtr[c] == rowPtrD[c]) {
                    correlo[di][rowPtr[c]]++;
                }
            }
        }
    }

    // Bước 3: Chuẩn hóa và tạo vector đặc trưng
    // Tổng số cặp pixel kiểm tra cho khoảng cách d
    // (xấp xỉ: rows * (cols - d) + (rows - d) * cols cho 2 hướng)
    cv::Mat featureVec = cv::Mat::zeros(1, m_numColors * D, CV_32F);
    float* ptr = featureVec.ptr<float>(0);

    for (int di = 0; di < D; di++) {
        for (int c = 0; c < m_numColors; c++) {
            double denom = (colorCount[c] > 0) ? colorCount[c] : 1.0;
            ptr[di * m_numColors + c] = static_cast<float>(correlo[di][c] / denom);
        }
    }

    // Chuẩn hóa L2 toàn bộ vector
    cv::normalize(featureVec, featureVec, 1.0, 0.0, cv::NORM_L2);

    return featureVec;
}

// ──────────────────────────────────────────────
// getName() / getDimension()
// ──────────────────────────────────────────────

/**
 * @brief Trả về tên phương pháp.
 */
std::string ColorCorrelogram::getName() const
{
    return "ColorCorrelogram";
}

/**
 * @brief Trả về số chiều vector đặc trưng.
 */
int ColorCorrelogram::getDimension() const
{
    return m_numColors * static_cast<int>(m_distances.size());
}
