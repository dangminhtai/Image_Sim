/**
 * @file ImageSearchEngine.cpp
 * @brief Cài đặt engine truy vấn ảnh CBIR.
 *
 * Độ đo khoảng cách được sử dụng:
 *  - Euclidean (L2): d = ||a - b||₂ — chuẩn, tổng quát nhất
 *  - Cosine: sim = (a·b)/(||a||·||b||) — 1.0 = giống hệt, 0 = khác nhau
 *  - Chi-squared: d = Σ (aᵢ - bᵢ)²/(aᵢ + bᵢ) — tốt nhất cho histogram màu
 *
 * Tham khảo:
 * - https://docs.opencv.org/4.x/dc/dc3/tutorial_py_matcher.html
 * - Ritendra Datta et al., "Image Retrieval: Ideas, Influences, and Trends", ACM 2008
 */

#include "ImageSearchEngine.h"
#include <opencv2/imgproc.hpp>
#include <algorithm>
#include <chrono>
#include <cmath>
#include <iostream>

// ──────────────────────────────────────────────
// loadDatabase()
// ──────────────────────────────────────────────

/**
 * @brief Nạp CSDL đặc trưng từ file YAML.
 */
bool ImageSearchEngine::loadDatabase(const std::string& featuresFile)
{
    bool ok = m_db.loadDatabase(featuresFile);
    if (!ok || m_db.size() == 0) return false;

    // Tự động phát hiện các phương pháp có trong CSDL
    m_availableMethods.clear();
    const auto& records = m_db.getDatabase();
    if (!records.empty()) {
        for (const auto& [name, _] : records[0].features) {
            m_availableMethods.push_back(name);
        }
    }

    std::cout << "[SearchEngine] CSDL da nap: " << m_db.size() << " anh, "
              << m_availableMethods.size() << " phuong phap\n";

    // Xây dựng FLANN indices để tìm kiếm nhanh
    buildFlannIndices();
    
    return true;
}

/**
 * @brief Chuyển đổi CSDL và nạp lại toàn bộ từ vựng cho các Extractor.
 */
bool ImageSearchEngine::switchDatabase(const std::string& featuresFile, const std::string& vocabDir)
{
    if (!loadDatabase(featuresFile)) {
        return false;
    }
    // Nạp lại từ vựng cho các phương pháp BoW
    for (auto& ext : m_extractors) {
        ext->reloadVocabulary(vocabDir);
    }
    return true;
}

/**
 * @brief Đăng ký extractor cho việc rút trích query.
 */
void ImageSearchEngine::addExtractor(std::shared_ptr<FeatureExtractor> extractor)
{
    if (extractor) m_extractors.push_back(extractor);
}

// ──────────────────────────────────────────────
// computeDistance()
// ──────────────────────────────────────────────

/**
 * @brief Tính khoảng cách giữa 2 vector đặc trưng.
 *
 * Lưu ý: Với Cosine similarity, giá trị trả về được đổi dấu
 * để đồng nhất với "nhỏ hơn = giống hơn" cho mọi độ đo.
 * Cụ thể: score = 1.0 - cosine_similarity.
 */
float ImageSearchEngine::computeDistance(const cv::Mat& a, const cv::Mat& b,
                                         DistanceMetric metric) const
{
    if (a.empty() || b.empty()) return std::numeric_limits<float>::max();

    // Đảm bảo cùng kiểu và chiều
    cv::Mat af = a.reshape(1, 1), bf = b.reshape(1, 1);
    if (af.cols != bf.cols) return std::numeric_limits<float>::max();

    switch (metric) {
    case DistanceMetric::EUCLIDEAN:
        // L2 norm của (a - b)
        return static_cast<float>(cv::norm(af - bf, cv::NORM_L2));

    case DistanceMetric::COSINE: {
        // 1 - cos(a, b) = 1 - (a·b)/(||a||·||b||)
        double dot  = af.dot(bf);
        double normA = cv::norm(af, cv::NORM_L2);
        double normB = cv::norm(bf, cv::NORM_L2);
        if (normA < 1e-9 || normB < 1e-9) return 1.0f;
        float cosine = static_cast<float>(dot / (normA * normB));
        return 1.0f - cosine; // Nhỏ hơn = giống hơn
    }

    case DistanceMetric::CHI_SQUARED: {
        // d = Σ (aᵢ - bᵢ)² / (aᵢ + bᵢ + ε)
        float dist = 0.0f;
        const float* pa = af.ptr<float>();
        const float* pb = bf.ptr<float>();
        for (int i = 0; i < af.cols; i++) {
            float sum = pa[i] + pb[i];
            if (sum > 1e-9f) {
                float diff = pa[i] - pb[i];
                dist += (diff * diff) / sum;
            }
        }
        return dist;
    }
    }
    return std::numeric_limits<float>::max();
}

// ──────────────────────────────────────────────
// search()
// ──────────────────────────────────────────────

/**
 * @brief Thực hiện truy vấn ảnh và trả về K kết quả tốt nhất.
 *
 * Quy trình:
 *  1. Tìm extractor tương ứng với method được chọn.
 *  2. Rút trích đặc trưng của ảnh truy vấn.
 *  3. Với mỗi ảnh trong CSDL: tính khoảng cách.
 *  4. Sắp xếp theo khoảng cách tăng dần.
 *  5. Trả về K kết quả đầu tiên.
 */
std::vector<SearchResult> ImageSearchEngine::search(const cv::Mat& queryImage,
                                                     const std::string& method,
                                                     DistanceMetric metric,
                                                     int K,
                                                     double& elapsedMs) const
{
    auto t0 = std::chrono::high_resolution_clock::now();
    std::vector<SearchResult> results;

    if (!isReady()) {
        std::cerr << "[SearchEngine] CSDL chua duoc nap.\n";
        return results;
    }

    // Bước 1: Tìm extractor cho method được chọn
    std::shared_ptr<FeatureExtractor> extractor;
    for (const auto& ext : m_extractors) {
        if (ext->getName() == method) {
            extractor = ext;
            break;
        }
    }
    if (!extractor) {
        std::cerr << "[SearchEngine] Khong tim thay extractor cho: " << method << "\n";
        return results;
    }

    // Bước 2: Rút trích đặc trưng query
    cv::Mat queryFeat = extractor->extract(queryImage);
    if (queryFeat.empty()) {
        std::cerr << "[SearchEngine] Rut trich dac trung that bai.\n";
        return results;
    }

    // Bước 3: So sánh với toàn bộ CSDL
    const auto& db = m_db.getDatabase();
    results.reserve(db.size());

    if ((metric == DistanceMetric::EUCLIDEAN || metric == DistanceMetric::COSINE) &&
        m_flannIndices.find(method) != m_flannIndices.end()) {
        
        // --- TÌM KIẾM NHANH VỚI FLANN (O(log N)) ---
        auto flannIndex = m_flannIndices.at(method);
        cv::Mat queryF32;
        queryFeat.convertTo(queryF32, CV_32F);

        // Do ta cần trả về K kết quả NHƯNG sau này ta có sắp xếp lại toàn bộ,
        // để mô phỏng đúng hệt code cũ (tính khoảng cách với toàn bộ),
        // ta có thể cho FLANN trả về số lượng K_search = N hoặc giới hạn lại.
        // Tuy nhiên hàm search() yêu cầu giữ nguyên luồng logic lấy top K.
        // Tối ưu nhất là nhờ FLANN tìm k_neighbors = toàn bộ (do db nhỏ = 99)
        // hoặc chí ít là K kết quả gần nhất. Nhưng vì kiến trúc cũ push toàn bộ
        // rồi mới sort. Ta cứ lấy toàn bộ N kết quả từ FLANN cho an toàn.
        int searchK = static_cast<int>(db.size());
        
        cv::Mat indices(1, searchK, CV_32S);
        cv::Mat dists(1, searchK, CV_32F);
        
        // Tham số checks=32 (số lần duyệt lá trong KD-tree, càng cao càng chính xác)
        flannIndex->knnSearch(queryF32, indices, dists, searchK, cv::flann::SearchParams(32));

        for (int i = 0; i < searchK; i++) {
            int idx = indices.at<int>(0, i);
            float dist = dists.at<float>(0, i);
            
            // FLANN KD-Tree trả về bình phương khoảng cách L2 (Squared L2)
            float finalScore = 0.0f;
            if (metric == DistanceMetric::EUCLIDEAN) {
                finalScore = std::sqrt(dist);
            } else if (metric == DistanceMetric::COSINE) {
                // Euclidean distance squared = 2 - 2*cos
                // cos = 1 - dist/2
                // score = 1 - cos = dist / 2
                finalScore = dist / 2.0f; 
            }
            results.push_back({db[idx].imagePath, db[idx].label, finalScore});
        }
    } else {
        // --- TÌM KIẾM TUYẾN TÍNH O(N) (Dành cho Chi-Sq) ---
        for (const auto& record : db) {
            auto it = record.features.find(method);
            if (it == record.features.end()) continue;

            float score = computeDistance(queryFeat, it->second, metric);
            results.push_back({record.imagePath, record.label, score});
        }
    }

    // Bước 4: Sắp xếp tăng dần theo score (score nhỏ = giống nhất)
    std::sort(results.begin(), results.end(),
              [](const SearchResult& a, const SearchResult& b) {
                  return a.score < b.score;
              });

    // Bước 5: Giữ K kết quả đầu
    if (static_cast<int>(results.size()) > K) {
        results.resize(K);
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    elapsedMs = std::chrono::duration<double, std::milli>(t1 - t0).count();
    return results;
}

// ──────────────────────────────────────────────
// Getters
// ──────────────────────────────────────────────

/** @brief Trả về danh sách tên phương pháp. */
std::vector<std::string> ImageSearchEngine::getAvailableMethods() const
{
    return m_availableMethods;
}

/** @brief Trả về số ảnh trong CSDL. */
int ImageSearchEngine::getDatabaseSize() const
{
    return m_db.size();
}

/** @brief Kiểm tra CSDL đã sẵn sàng. */
bool ImageSearchEngine::isReady() const
{
    return m_db.size() > 0;
}

// ──────────────────────────────────────────────
// FLANN Indices
// ──────────────────────────────────────────────

/** @brief Tự động gom ma trận đặc trưng và build FLANN KD-Tree */
void ImageSearchEngine::buildFlannIndices()
{
    m_flannFeatures.clear();
    m_flannIndices.clear();

    const auto& db = m_db.getDatabase();
    if (db.empty()) return;

    for (const auto& method : m_availableMethods) {
        std::vector<cv::Mat> features;
        for (const auto& record : db) {
            auto it = record.features.find(method);
            if (it != record.features.end()) {
                features.push_back(it->second.reshape(1, 1));
            }
        }

        if (features.empty()) continue;

        cv::Mat featuresMat;
        cv::vconcat(features, featuresMat);

        if (featuresMat.type() != CV_32F) {
            featuresMat.convertTo(featuresMat, CV_32F);
        }

        m_flannFeatures[method] = featuresMat;

        // Xây dựng 4 cây KD-Tree để tìm kiếm xấp xỉ song song
        m_flannIndices[method] = std::make_shared<cv::flann::Index>(
            featuresMat, cv::flann::KDTreeIndexParams(4));
    }
    std::cout << "[SearchEngine] FLANN (KD-Tree) indices da duoc build thanh cong.\n";
}
