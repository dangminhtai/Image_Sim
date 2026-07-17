/**
 * @file ImageSearchEngine.h
 * @brief Engine truy vấn ảnh: nạp CSDL đặc trưng và tìm K ảnh gần nhất.
 *
 * ImageSearchEngine là trung gian giữa GUI và DatabaseBuilder.
 * Nó nạp features.yml, sau đó với mỗi ảnh truy vấn:
 *  1. Rút trích đặc trưng của ảnh query bằng extractor đã chọn.
 *  2. Tính khoảng cách giữa query và mỗi ảnh trong CSDL.
 *  3. Sắp xếp kết quả và trả về K ảnh gần nhất.
 *
 * Hỗ trợ 3 cơ chế index:
 *  - Forward Index: O(1) lookup feature theo imageID (tất cả method)
 *  - FLANN KD-Tree: tìm kiếm xấp xỉ O(log N) (HOG, Color_HOG)
 *  - Inverted Index: TF-IDF scoring qua posting lists (SIFT, ORB BoW)
 *
 * Hỗ trợ 3 độ đo so sánh:
 *  - Euclidean distance (L2)
 *  - Cosine similarity
 *  - Chi-squared distance (hiệu quả cho histogram)
 */
#pragma once

#include "FeatureExtractor.h"
#include "DatabaseBuilder.h"
#include "ForwardIndex.h"
#include "InvertedIndex.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <opencv2/flann.hpp>

/**
 * @enum DistanceMetric
 * @brief Các độ đo so sánh vector đặc trưng được hỗ trợ.
 */
enum class DistanceMetric {
    EUCLIDEAN,   ///< Khoảng cách Euclidean (L2) — tổng quát
    COSINE,      ///< Độ tương đồng Cosine — tốt cho vector chuẩn hóa
    CHI_SQUARED  ///< Khoảng cách Chi-squared — hiệu quả cho histogram màu
};

/**
 * @struct SearchResult
 * @brief Kết quả một ảnh trả về từ truy vấn.
 */
struct SearchResult {
    std::string imagePath; ///< Đường dẫn ảnh trong CSDL
    std::string label;     ///< Nhãn lớp của ảnh
    float score;           ///< Điểm tương đồng (nhỏ hơn = giống hơn với Euclidean/Chi2)
};

/**
 * @class ImageSearchEngine
 * @brief Engine thực hiện truy vấn ảnh theo nội dung (CBIR).
 */
class ImageSearchEngine {
public:
    /**
     * @brief Constructor mặc định.
     */
    ImageSearchEngine() = default;

    /**
     * @brief Nạp CSDL đặc trưng từ file YAML.
     * @param featuresFile Đường dẫn file features.yml.
     * @return true nếu nạp thành công.
     */
    bool loadDatabase(const std::string& featuresFile);

    /**
     * @brief Chuyển đổi CSDL và nạp lại toàn bộ từ vựng cho các Extractor.
     * @param featuresFile Đường dẫn file features.yml mới.
     * @param vocabDir Đường dẫn thư mục chứa từ vựng tương ứng.
     * @return true nếu chuyển đổi thành công.
     */
    bool switchDatabase(const std::string& featuresFile, const std::string& vocabDir);

    /**
     * @brief Đăng ký extractor sẽ dùng khi query (phải trùng tên với key trong YAML).
     * @param extractor Shared pointer đến extractor.
     */
    void addExtractor(std::shared_ptr<FeatureExtractor> extractor);

    /**
     * @brief Thực hiện truy vấn ảnh.
     *
     * @param queryImage  Ảnh truy vấn (cv::Mat BGR).
     * @param method      Tên phương pháp rút trích ("ColorHistogram", "SIFT", ...).
     * @param metric      Độ đo so sánh muốn dùng.
     * @param K           Số kết quả trả về.
     * @param elapsedMs   [out] Thời gian truy vấn (milliseconds).
     * @return Danh sách K kết quả, sắp xếp từ giống nhất đến ít giống nhất.
     */
    std::vector<SearchResult> search(const cv::Mat& queryImage,
                                     const std::string& method,
                                     DistanceMetric metric,
                                     int K,
                                     double& elapsedMs) const;

    /**
     * @brief Trả về danh sách tên các phương pháp có trong CSDL.
     */
    std::vector<std::string> getAvailableMethods() const;

    /**
     * @brief Trả về số ảnh trong CSDL.
     */
    int getDatabaseSize() const;

    /**
     * @brief Kiểm tra CSDL đã được nạp chưa.
     */
    bool isReady() const;

private:
    DatabaseBuilder m_db;                                           ///< CSDL đặc trưng
    std::vector<std::shared_ptr<FeatureExtractor>> m_extractors;    ///< Danh sách extractor
    std::vector<std::string> m_availableMethods;                    ///< Tên các method có sẵn

    /**
     * @brief Tính khoảng cách giữa 2 vector đặc trưng theo độ đo được chọn.
     * @param a Vector đặc trưng ảnh truy vấn.
     * @param b Vector đặc trưng ảnh CSDL.
     * @param metric Độ đo.
     * @return Giá trị khoảng cách (nhỏ hơn = giống hơn với L2/Chi2; lớn hơn = giống hơn với Cosine).
     */
    float computeDistance(const cv::Mat& a, const cv::Mat& b, DistanceMetric metric) const;

    /**
     * @brief Xây dựng FLANN KD-Tree index cho tất cả các phương pháp trong CSDL
     */
    void buildFlannIndices();

    /**
     * @brief Xây dựng Forward Index và Inverted Index sau khi load CSDL.
     *
     * Forward Index: O(1) lookup cho tất cả methods.
     * Inverted Index: chỉ build cho SIFT và ORB (BoW methods).
     */
    void buildSearchIndices();

    std::unordered_map<std::string, cv::Mat> m_flannFeatures;
    std::unordered_map<std::string, std::shared_ptr<cv::flann::Index>> m_flannIndices;

    ForwardIndex m_forwardIndex;   ///< Forward Index: imageID → features

    /// Inverted Index cho các method BoW (key = method name)
    std::unordered_map<std::string, InvertedIndex> m_invertedIndices;
};
