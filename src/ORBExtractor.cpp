/**
 * @file ORBExtractor.cpp
 * @brief Cài đặt ORB + Bag of Visual Words (BoW).
 *
 * ORB sử dụng binary descriptor (32 bytes = 256 bits).
 * Do descriptor kiểu CV_8U (nhị phân), ta dùng FLANN với LSH index
 * thay vì KD-Tree như SIFT.
 *
 * Quy trình tương tự SIFTExtractor nhưng descriptor phải convert sang
 * CV_32F trước khi đưa vào BOWKMeansTrainer.
 */

#include "ORBExtractor.h"
#include <opencv2/imgproc.hpp>
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <stdexcept>

// ──────────────────────────────────────────────
// Constructor
// ──────────────────────────────────────────────

/**
 * @brief Khởi tạo ORBExtractor.
 */
ORBExtractor::ORBExtractor(int vocabSize, int nFeatures)
    : m_vocabSize(vocabSize), m_nFeatures(nFeatures)
{
    m_orb = cv::ORB::create(m_nFeatures);
}

// ──────────────────────────────────────────────
// initBowExtractor()
// ──────────────────────────────────────────────

/**
 * @brief Khởi tạo BFMatcher để dùng trong extract() thủ công.
 *
 * ORB vocabulary sau k-means là CV_32F.
 * Dùng BFMatcher(NORM_L2) thay vì FLANN LSH để tránh lỗi type mismatch.
 * BOWImgDescriptorExtractor không được dùng cho ORB vì nó không tự convert
 * descriptor từ CV_8U sang CV_32F trước khi match với vocabulary float.
 */
void ORBExtractor::initBowExtractor()
{
    // BFMatcher với NORM_L2 cho vocabulary CV_32F
    // (ORB descriptor sẽ được convert sang float trước khi match)
    m_matcher = cv::BFMatcher::create(cv::NORM_L2);
}

// ──────────────────────────────────────────────
// buildVocabulary()
// ──────────────────────────────────────────────

/**
 * @brief Xây dựng từ điển BoW từ danh sách ảnh.
 */
void ORBExtractor::buildVocabulary(const std::vector<std::string>& imagePaths)
{
    if (imagePaths.empty()) {
        throw std::runtime_error("ORBExtractor: Danh sách ảnh rỗng.");
    }

    // Chỉ dùng tối đa 150 ảnh để train vocabulary nhằm tránh quá tải RAM (Crash/Hang K-Means)
    int maxImages = std::min(150, static_cast<int>(imagePaths.size()));
    std::cout << "[ORB] Bat dau xay vocabulary voi " << m_vocabSize
              << " tu, dung " << maxImages << " anh dai dien...\n";

    cv::BOWKMeansTrainer bowTrainer(m_vocabSize,
        cv::TermCriteria(cv::TermCriteria::EPS | cv::TermCriteria::MAX_ITER, 10, 0.001),
        3, cv::KMEANS_PP_CENTERS);

    int barWidth = 30;
    for (int i = 0; i < maxImages; i++) {
        // In Progress bar phong cách tqdm
        float progress = static_cast<float>(i + 1) / maxImages;
        int filled = static_cast<int>(barWidth * progress);
        std::cout << "\r[ORB]  Extracting: [";
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
        m_orb->detectAndCompute(img, cv::noArray(), keypoints, descriptors);

        if (!descriptors.empty()) {
            cv::Mat desc32f;
            descriptors.convertTo(desc32f, CV_32F);
            bowTrainer.add(desc32f);
        }
    }
    std::cout << "\n";

    std::cout << "[ORB] Chay k-means...\n";
    m_vocabulary = bowTrainer.cluster();
    std::cout << "[ORB] Vocabulary hoan thanh: " << m_vocabulary.rows
              << " tu, " << m_vocabulary.cols << " chieu\n";

    initBowExtractor();
}

// ──────────────────────────────────────────────
// saveVocabulary() / loadVocabulary()
// ──────────────────────────────────────────────

/**
 * @brief Lưu vocabulary ra file YAML.
 */
void ORBExtractor::saveVocabulary(const std::string& filePath) const
{
    if (m_vocabulary.empty()) {
        std::cerr << "[ORB] Vocabulary rong.\n";
        return;
    }
    cv::FileStorage fs(filePath, cv::FileStorage::WRITE);
    fs << "vocabulary" << m_vocabulary;
    fs << "vocabSize" << m_vocabSize;
    std::cout << "[ORB] Da luu vocabulary: " << filePath << "\n";
}

/**
 * @brief Nạp vocabulary từ file YAML.
 */
bool ORBExtractor::loadVocabulary(const std::string& filePath)
{
    cv::FileStorage fs(filePath, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "[ORB] Khong mo duoc file: " << filePath << "\n";
        return false;
    }
    fs["vocabulary"] >> m_vocabulary;
    fs["vocabSize"] >> m_vocabSize;
    if (m_vocabulary.empty()) return false;
    initBowExtractor();
    std::cout << "[ORB] Da nap vocabulary: " << filePath << "\n";
    return true;
}

/**
 * @brief Kiểm tra vocabulary sẵn sàng.
 */
bool ORBExtractor::isVocabularyReady() const
{
    return !m_vocabulary.empty();
}

/**
 * @brief Nạp lại từ vựng khi chuyển đổi CSDL.
 */
bool ORBExtractor::reloadVocabulary(const std::string& vocabDir)
{
    std::string path = vocabDir;
    if (!path.empty() && path.back() != '/' && path.back() != '\\') {
        path += "/";
    }
    return loadVocabulary(path + "orb_vocab.yml");
}

// ──────────────────────────────────────────────
// extract()
// ──────────────────────────────────────────────

/**
 * @brief Rút trích đặc trưng BoW ORB cho một ảnh.
 *
 * Quy trình thủ công (không dùng BOWImgDescriptorExtractor):
 *  1. Phát hiện keypoints + tính ORB descriptors (CV_8U).
 *  2. Convert descriptors sang CV_32F để match với vocabulary.
 *  3. Dùng BFMatcher tìm visual word gần nhất cho mỗi descriptor.
 *  4. Đếm tần suất → histogram BoW → chuẩn hóa L2.
 */
cv::Mat ORBExtractor::extract(const cv::Mat& image) const
{
    if (image.empty()) return cv::Mat();
    if (!isVocabularyReady()) {
        std::cerr << "[ORB] Vocabulary chua san sang.\n";
        return cv::Mat();
    }

    cv::Mat gray;
    if (image.channels() == 3) {
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = image.clone();
    }

    // Bước 1: Phát hiện keypoints và tính descriptors
    std::vector<cv::KeyPoint> keypoints;
    cv::Mat descriptors;
    m_orb->detectAndCompute(gray, cv::noArray(), keypoints, descriptors);

    if (keypoints.empty() || descriptors.empty()) {
        return cv::Mat::zeros(1, m_vocabSize, CV_32F);
    }

    // Bước 2: Convert ORB descriptor (CV_8U) → CV_32F để match với vocabulary
    cv::Mat descFloat;
    descriptors.convertTo(descFloat, CV_32F);

    // Bước 3: Match mỗi descriptor với visual word gần nhất trong vocabulary
    std::vector<cv::DMatch> matches;
    m_matcher->match(descFloat, m_vocabulary, matches);

    // Bước 4: Xây histogram BoW
    cv::Mat bowDesc = cv::Mat::zeros(1, m_vocabSize, CV_32F);
    for (const auto& m : matches) {
        if (m.trainIdx >= 0 && m.trainIdx < m_vocabSize) {
            bowDesc.at<float>(0, m.trainIdx) += 1.0f;
        }
    }

    // Chuẩn hóa L2
    cv::normalize(bowDesc, bowDesc, 1.0, 0.0, cv::NORM_L2);
    return bowDesc;
}

// ──────────────────────────────────────────────
// getName() / getDimension()
// ──────────────────────────────────────────────

/** @brief Trả về tên phương pháp. */
std::string ORBExtractor::getName() const { return "ORB"; }

/** @brief Trả về số chiều = vocabSize. */
int ORBExtractor::getDimension() const { return m_vocabSize; }
