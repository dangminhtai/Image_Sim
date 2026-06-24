/**
 * @file ColorCorrelogram.h
 * @brief Rút trích đặc trưng Color Correlogram.
 *
 * Color Correlogram mô tả xác suất mà hai pixel cách nhau một khoảng d
 * có cùng màu. Đây là đặc trưng mạnh hơn Color Histogram vì nó nắm bắt
 * được thông tin về phân bố không gian của màu sắc.
 *
 * Tham khảo:
 * - Jing Huang et al., "Image Indexing Using Color Correlograms", CVPR 1997.
 * - https://docs.opencv.org/4.x/index.html
 */
#pragma once

#include "FeatureExtractor.h"
#include <vector>

/**
 * @class ColorCorrelogram
 * @brief Rút trích đặc trưng Color Correlogram.
 *
 * Thuật toán:
 *  1. Lượng hóa ảnh BGR về numColors màu đại diện.
 *  2. Với mỗi khoảng cách d trong danh sách distances[]:
 *     Với mỗi cặp màu (ci, cj): Tính P[ci][cj][d] = xác suất pixel màu ci
 *     cách pixel màu cj đúng d pixels.
 *  3. Vector đặc trưng = flatten của tensor P.
 *
 * Kích thước vector = numColors × numColors × len(distances).
 */
class ColorCorrelogram : public FeatureExtractor {
public:
    /**
     * @brief Khởi tạo ColorCorrelogram.
     * @param numColors Số màu sau khi lượng hóa (mặc định 8 → 8 màu đại diện).
     * @param distances Danh sách khoảng cách d cần tính (mặc định: {1, 3, 5, 7}).
     */
    explicit ColorCorrelogram(int numColors = 8,
                              std::vector<int> distances = {1, 3, 5, 7});

    /**
     * @brief Rút trích đặc trưng Color Correlogram từ ảnh BGR.
     * @param image Ảnh BGR đầu vào.
     * @return Vector đặc trưng flatten, kiểu CV_32F, đã chuẩn hóa L2.
     */
    cv::Mat extract(const cv::Mat& image) const override;

    /**
     * @brief Trả về tên phương pháp.
     * @return "ColorCorrelogram"
     */
    std::string getName() const override;

    /**
     * @brief Trả về số chiều = numColors * numColors * len(distances).
     */
    int getDimension() const override;

private:
    int m_numColors;              ///< Số màu lượng hóa
    std::vector<int> m_distances; ///< Danh sách khoảng cách d

    /**
     * @brief Lượng hóa ảnh BGR về m_numColors màu đại diện.
     * @param image Ảnh BGR gốc.
     * @return Ảnh chỉ số màu (kiểu CV_32S), mỗi pixel là chỉ số màu [0..numColors-1].
     */
    cv::Mat quantizeColor(const cv::Mat& image) const;
};
