/**
 * @file SIFTExtractor.cpp
 * @brief Cài đặt SIFT + Bag of Visual Words (BoW).
 *
 * Quy trình:
 *  Offline — buildVocabulary():
 *    1. Với mỗi ảnh trong CSDL: phát hiện keypoints bằng SIFT, lấy descriptors.
 *    2. Gộp tất cả descriptors lại → chạy k-means (BOWKMeansTrainer).
 *    3. Kết quả là vocabulary (ma trận vocabSize × 128 float).
 *
 *  Online — extract():
 *    1. Phát hiện keypoints của ảnh truy vấn.
 *    2. BOWImgDescriptorExtractor tính histogram tần suất từng visual word.
 *    3. Chuẩn hóa histogram và trả về.
 */

#include "SIFTExtractor.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/ml.hpp>
#include <iostream>
#include <stdexcept>

// ──────────────────────────────────────────────
// Constructor
// ──────────────────────────────────────────────

/**
 * @brief Khởi tạo SIFTExtractor với kích thước vocabulary và số keypoints.
 */
SIFTExtractor::SIFTExtractor(int vocabSize, int nFeatures)
    : m_vocabSize(vocabSize), m_nFeatures(nFeatures)
{
    // Khởi tạo SIFT detector
    m_sift = cv::SIFT::create(m_nFeatures);
}

// ──────────────────────────────────────────────
// initBowExtractor() — khởi tạo BOW extractor sau khi có vocabulary
// ──────────────────────────────────────────────

/**
 * @brief Khởi tạo BOWImgDescriptorExtractor với FLANN matcher.
 */
void SIFTExtractor::initBowExtractor()
{
    // SIFT dùng FLANN KD-Tree matcher (descriptor float)
    auto matcher = cv::FlannBasedMatcher::create();
    m_bowExtractor = cv::makePtr<cv::BOWImgDescriptorExtractor>(m_sift, matcher);
    m_bowExtractor->setVocabulary(m_vocabulary);
}

// ──────────────────────────────────────────────
// buildVocabulary()
// ──────────────────────────────────────────────

/**
 * @brief Xây dựng từ điển BoW từ danh sách ảnh.
 *
 * Giai đoạn offline — cần gọi một lần trước khi dùng extract().
 */
void SIFTExtractor::buildVocabulary(const std::vector<std::string>& imagePaths)
{
    if (imagePaths.empty()) {
        throw std::runtime_error("SIFTExtractor: Danh sách ảnh rỗng, không thể xây vocabulary.");
    }

    // Chỉ dùng tối đa 150 ảnh để train vocabulary nhằm tránh quá tải RAM (Crash/Hang K-Means)
    int maxImages = std::min(150, static_cast<int>(imagePaths.size()));
    std::cout << "[SIFT] Bat dau xay vocabulary voi " << m_vocabSize
              << " tu, dung " << maxImages << " anh dai dien (ngan tran RAM)...\n";

    // Tạo BOWKMeansTrainer để tập hợp descriptors và chạy k-means
    cv::BOWKMeansTrainer bowTrainer(m_vocabSize,
        cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 10, 0.001),
        3, cv::KMEANS_PP_CENTERS);

    int barWidth = 30;
    for (int i = 0; i < maxImages; i++) {
        // In Progress bar phong cách tqdm
        float progress = static_cast<float>(i + 1) / maxImages;
        int filled = static_cast<int>(barWidth * progress);
        std::cout << "\r[SIFT] Extracting: [";
        for (int j = 0; j < barWidth; j++) {
            if (j < filled) std::cout << "=";
            else if (j == filled) std::cout << ">";
            else std::cout << "-";
        }
        std::cout << "] " << (i + 1) << "/" << maxImages << " ";
        std::cout.flush();

        cv::Mat img = cv::imread(imagePaths[i], cv::IMREAD_GRAYSCALE);
        if (img.empty()) continue;

        std::vector<cv::KeyPoint> keypoints;
        cv::Mat descriptors;
        m_sift->detectAndCompute(img, cv::noArray(), keypoints, descriptors);

        if (!descriptors.empty() && descriptors.type() == CV_32F) {
            bowTrainer.add(descriptors);
        }
    }
    std::cout << "\n";

    std::cout << "[SIFT] Chay k-means de tao vocabulary...\n";
    m_vocabulary = bowTrainer.cluster();
    std::cout << "[SIFT] Vocabulary hoan thanh: " << m_vocabulary.rows
              << " tu, " << m_vocabulary.cols << " chieu\n";

    // Khởi tạo bow extractor với vocabulary vừa tạo
    initBowExtractor();
}

// ──────────────────────────────────────────────
// saveVocabulary() / loadVocabulary()
// ──────────────────────────────────────────────

/**
 * @brief Lưu vocabulary ra file YAML để tái sử dụng.
 */
void SIFTExtractor::saveVocabulary(const std::string& filePath) const
{
    if (m_vocabulary.empty()) {
        std::cerr << "[SIFT] Vocabulary rong, khong the luu.\n";
        return;
    }
    cv::FileStorage fs(filePath, cv::FileStorage::WRITE);
    fs << "vocabulary" << m_vocabulary;
    fs << "vocabSize" << m_vocabSize;
    std::cout << "[SIFT] Da luu vocabulary tai: " << filePath << "\n";
}

/**
 * @brief Nạp vocabulary từ file YAML.
 */
bool SIFTExtractor::loadVocabulary(const std::string& filePath)
{
    cv::FileStorage fs(filePath, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "[SIFT] Khong mo duoc file: " << filePath << "\n";
        return false;
    }
    fs["vocabulary"] >> m_vocabulary;
    fs["vocabSize"] >> m_vocabSize;
    if (m_vocabulary.empty()) {
        std::cerr << "[SIFT] Vocabulary trong file bi rong.\n";
        return false;
    }
    // Khởi tạo bow extractor sau khi nạp vocabulary
    initBowExtractor();
    std::cout << "[SIFT] Da nap vocabulary tu: " << filePath << "\n";
    return true;
}

/**
 * @brief Kiểm tra vocabulary đã sẵn sàng chưa.
 */
bool SIFTExtractor::isVocabularyReady() const
{
    return !m_vocabulary.empty();
}

/**
 * @brief Nạp lại từ vựng khi chuyển đổi CSDL.
 */
bool SIFTExtractor::reloadVocabulary(const std::string& vocabDir)
{
    std::string path = vocabDir;
    if (!path.empty() && path.back() != '/' && path.back() != '\\') {
        path += "/";
    }
    return loadVocabulary(path + "sift_vocab.yml");
}

// ──────────────────────────────────────────────
// extract()
// ──────────────────────────────────────────────

/**
 * @brief Rút trích đặc trưng BoW SIFT cho một ảnh.
 */
cv::Mat SIFTExtractor::extract(const cv::Mat& image) const
{
    if (image.empty()) {
        return cv::Mat();
    }
    if (!isVocabularyReady()) {
        std::cerr << "[SIFT] Vocabulary chua san sang. Goi buildVocabulary() truoc.\n";
        return cv::Mat();
    }

    // Chuyển sang grayscale
    cv::Mat gray;
    if (image.channels() == 3) {
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = image.clone();
    }

    // Phát hiện keypoints
    std::vector<cv::KeyPoint> keypoints;
    m_sift->detect(gray, keypoints);
    if (keypoints.empty()) {
        // Trả về vector 0 nếu không có keypoint
        return cv::Mat::zeros(1, m_vocabSize, CV_32F);
    }

    // Tính BoW histogram
    cv::Mat bowDescriptor;
    m_bowExtractor->compute(gray, keypoints, bowDescriptor);

    if (bowDescriptor.empty()) {
        return cv::Mat::zeros(1, m_vocabSize, CV_32F);
    }

    // Chuẩn hóa L2
    cv::normalize(bowDescriptor, bowDescriptor, 1.0, 0.0, cv::NORM_L2);
    return bowDescriptor; // shape: (1, vocabSize), CV_32F
}

// ──────────────────────────────────────────────
// getName() / getDimension()
// ──────────────────────────────────────────────

/**
 * @brief Trả về tên phương pháp.
 */
std::string SIFTExtractor::getName() const
{
    return "SIFT";
}

/**
 * @brief Trả về số chiều = vocabSize.
 */
int SIFTExtractor::getDimension() const
{
    return m_vocabSize;
}
