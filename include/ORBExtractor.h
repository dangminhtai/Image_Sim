/**
 * @file ORBExtractor.h
 * @brief Rút trích đặc trưng ORB (Oriented FAST and Rotated BRIEF).
 *
 * ORB là thuật toán phát hiện và mô tả đặc trưng cục bộ nhanh hơn SIFT,
 * sử dụng binary descriptor (256-bit). Phù hợp cho ứng dụng thời gian thực.
 * Tương tự SIFTExtractor, ta dùng Bag of Visual Words (BoW) để tạo
 * vector đặc trưng cố định chiều.
 *
 * Lưu ý: ORB dùng FLANN matcher với LSH index cho binary descriptor.
 *
 * Tham khảo:
 * - E. Rublee et al., "ORB: An efficient alternative to SIFT or SURF", ICCV 2011.
 * - https://docs.opencv.org/4.x/db/d95/classcv_1_1ORB.html
 */
#pragma once

#include "FeatureExtractor.h"
#include <opencv2/features2d.hpp>

/**
 * @class ORBExtractor
 * @brief Rút trích đặc trưng ORB + Bag of Visual Words (BoW).
 */
class ORBExtractor : public FeatureExtractor {
public:
    /**
     * @brief Khởi tạo ORBExtractor.
     * @param vocabSize Số từ trong từ điển BoW (mặc định 500).
     * @param nFeatures Số keypoints tối đa ORB phát hiện mỗi ảnh (mặc định 500).
     */
    explicit ORBExtractor(int vocabSize = 500, int nFeatures = 500);

    /**
     * @brief Rút trích đặc trưng BoW cho một ảnh.
     * @param image Ảnh BGR đầu vào.
     * @return Histogram BoW (1 × vocabSize, kiểu CV_32F), đã chuẩn hóa L2.
     *         Trả về cv::Mat rỗng nếu vocabulary chưa được xây dựng.
     */
    cv::Mat extract(const cv::Mat& image) const override;

    /**
     * @brief Trả về tên phương pháp.
     * @return "ORB"
     */
    std::string getName() const override;

    /**
     * @brief Trả về số chiều = kích thước vocabulary (vocabSize).
     */
    int getDimension() const override;

    /**
     * @brief Xây dựng từ điển BoW từ danh sách ảnh CSDL.
     * @param imagePaths Danh sách đường dẫn ảnh CSDL.
     */
    void buildVocabulary(const std::vector<std::string>& imagePaths);

    /**
     * @brief Lưu từ điển BoW ra file.
     * @param filePath Đường dẫn file lưu (ví dụ: "orb_vocab.yml").
     */
    void saveVocabulary(const std::string& filePath) const;

    /**
     * @brief Nạp từ điển BoW từ file đã lưu.
     * @param filePath Đường dẫn file.
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
    cv::Mat m_vocabulary;                ///< Ma trận từ điển (CV_32F)
    cv::Ptr<cv::ORB> m_orb;             ///< Đối tượng ORB detector
    cv::Ptr<cv::BFMatcher> m_matcher;    ///< BFMatcher(NORM_L2) để match float descriptor vs vocabulary

    /**
     * @brief Khởi tạo BFMatcher sau khi có vocabulary.
     * Dùng NORM_L2 vì vocabulary là CV_32F (không dùng FLANN LSH).
     */
    void initBowExtractor();
};
