/**
 * @file FeatureExtractor.h
 * @brief Lớp trừu tượng (abstract class) định nghĩa giao diện chung
 *        cho tất cả các phương pháp rút trích đặc trưng ảnh.
 *
 * Tất cả extractor cụ thể (ColorHistogram, SIFT, ORB, ...) phải kế thừa
 * lớp này và cài đặt phương thức extract().
 *
 * Tham khảo: https://docs.opencv.org/4.x/index.html
 */
#pragma once

#include <opencv2/core.hpp>
#include <string>

/**
 * @class FeatureExtractor
 * @brief Giao diện chung cho các phương pháp rút trích đặc trưng ảnh.
 *
 * Thiết kế theo mẫu Strategy Pattern: cho phép hoán đổi linh hoạt
 * giữa các thuật toán rút trích khác nhau mà không thay đổi code client.
 */
class FeatureExtractor {
public:
    /**
     * @brief Destructor ảo để đảm bảo destructor của lớp con được gọi đúng.
     */
    virtual ~FeatureExtractor() = default;

    /**
     * @brief Rút trích đặc trưng từ một tấm ảnh.
     * @param image Ảnh đầu vào (cv::Mat, BGR hoặc Grayscale).
     * @return cv::Mat 1 chiều (1×N, kiểu CV_32F) chứa vector đặc trưng.
     *         Trả về cv::Mat rỗng nếu rút trích thất bại.
     */
    virtual cv::Mat extract(const cv::Mat& image) const = 0;

    /**
     * @brief Trả về tên định danh của phương pháp rút trích.
     * @return Chuỗi tên, ví dụ: "ColorHistogram", "SIFT", "ORB".
     */
    virtual std::string getName() const = 0;

    /**
     * @brief Trả về số chiều của vector đặc trưng được tạo ra.
     * @return Số chiều (int). -1 nếu chiều có thể thay đổi (ví dụ BoW chưa build).
     */
    virtual int getDimension() const = 0;

    /**
     * @brief Tải lại từ vựng (Vocabulary) cho các phương pháp dùng BoW (SIFT, ORB).
     * Mặc định không làm gì đối với các phương pháp không dùng BoW.
     * @param vocabDir Đường dẫn thư mục chứa từ vựng.
     * @return true nếu tải thành công hoặc không cần tải.
     */
    virtual bool reloadVocabulary(const std::string& vocabDir) {
        return true; 
    }
};
