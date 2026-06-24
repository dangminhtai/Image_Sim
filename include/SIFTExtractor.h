/**
 * @file SIFTExtractor.h
 * @brief Rút trích đặc trưng SIFT (Scale-Invariant Feature Transform).
 *
 * SIFT phát hiện keypoints bất biến với tỷ lệ và góc quay, sau đó mô tả
 * mỗi keypoint bằng vector 128 chiều. Để tạo vector đặc trưng cố định chiều
 * cho toàn bộ ảnh (dùng so sánh ảnh vs ảnh), ta dùng phương pháp
 * Bag of Visual Words (BoW) với k-means clustering.
 *
 * Lưu ý: SIFT có trong opencv2/features2d.hpp từ OpenCV 4.4+.
 *
 * Tham khảo:
 * - D.G. Lowe, "Distinctive Image Features from Scale-Invariant Keypoints", IJCV 2004.
 * - https://docs.opencv.org/4.x/d7/d60/classcv_1_1SIFT.html
 * - https://docs.opencv.org/4.x/d2/d94/tutorial_table_of_content_ml.html (BOWKMeansTrainer)
 */
#pragma once

#include "FeatureExtractor.h"
#include <opencv2/features2d.hpp>

/**
 * @class SIFTExtractor
 * @brief Rút trích đặc trưng SIFT + Bag of Visual Words (BoW).
 *
 * Quy trình 2 bước:
 *  Bước 1 (offline): buildVocabulary() — tạo vocabulary từ tập ảnh CSDL.
 *  Bước 2 (online):  extract() — chiếu descriptor của 1 ảnh lên vocabulary → histogram BoW.
 */
class SIFTExtractor : public FeatureExtractor {
public:
    /**
     * @brief Khởi tạo SIFTExtractor.
     * @param vocabSize Số từ (cụm k-means) trong từ điển BoW (mặc định 500).
     * @param nFeatures Số keypoints tối đa SIFT phát hiện mỗi ảnh (0 = không giới hạn).
     */
    explicit SIFTExtractor(int vocabSize = 500, int nFeatures = 0);

    /**
     * @brief Rút trích đặc trưng BoW cho một ảnh.
     * @param image Ảnh BGR đầu vào.
     * @return Histogram BoW (1 × vocabSize, kiểu CV_32F), đã chuẩn hóa L2.
     *         Trả về cv::Mat rỗng nếu vocabulary chưa được xây dựng.
     */
    cv::Mat extract(const cv::Mat& image) const override;

    /**
     * @brief Trả về tên phương pháp.
     * @return "SIFT"
     */
    std::string getName() const override;

    /**
     * @brief Trả về số chiều = kích thước vocabulary (vocabSize).
     */
    int getDimension() const override;

    /**
     * @brief Xây dựng từ điển BoW từ danh sách ảnh CSDL.
     *
     * Hàm này cần được gọi 1 lần trước khi extract().
     * Thực hiện k-means clustering trên toàn bộ SIFT descriptors
     * từ tất cả ảnh CSDL.
     *
     * @param imagePaths Danh sách đường dẫn các ảnh CSDL.
     */
    void buildVocabulary(const std::vector<std::string>& imagePaths);

    /**
     * @brief Lưu từ điển BoW đã xây dựng ra file.
     * @param filePath Đường dẫn file lưu (ví dụ: "sift_vocab.yml").
     */
    void saveVocabulary(const std::string& filePath) const;

    /**
     * @brief Nạp từ điển BoW từ file đã lưu sẵn.
     * @param filePath Đường dẫn file đã lưu.
     * @return true nếu nạp thành công.
     */
    bool loadVocabulary(const std::string& filePath);

    /**
     * @brief Kiểm tra vocabulary đã sẵn sàng chưa.
     */
    bool isVocabularyReady() const;

    /**
     * @brief Nạp lại từ vựng khi chuyển đổi CSDL.
     */
    bool reloadVocabulary(const std::string& vocabDir) override;

private:
    int m_vocabSize;                     ///< Số từ trong từ điển
    int m_nFeatures;                     ///< Số keypoints tối đa
    cv::Mat m_vocabulary;                ///< Ma trận từ điển (vocabSize × 128)
    cv::Ptr<cv::SIFT> m_sift;            ///< Đối tượng SIFT detector
    cv::Ptr<cv::BOWImgDescriptorExtractor> m_bowExtractor; ///< Bộ trích xuất BoW

    /**
     * @brief Khởi tạo lại bow extractor sau khi có vocabulary.
     */
    void initBowExtractor();
};
