/**
 * @file ForwardIndex.cpp
 * @brief Cài đặt Forward Index cho hệ thống CBIR.
 *
 * Forward Index lưu trữ ánh xạ imageID → feature vectors dưới dạng
 * hash map lồng nhau (unordered_map), cho phép tra cứu O(1).
 *
 * Quy trình build:
 *  1. Duyệt từng ImageRecord trong vector (index = imageID).
 *  2. Copy metadata (path, label) vào m_metadata.
 *  3. Copy tất cả feature vectors vào m_index[imageID][methodName].
 *
 * Tham khảo:
 * - Christopher D. Manning et al., "Introduction to Information Retrieval",
 *   Cambridge University Press, 2008, Chapter 1.
 */

#include "ForwardIndex.h"
#include "DatabaseBuilder.h"  // Cần để dùng struct ImageRecord
#include <iostream>
#include <stdexcept>

/// Feature rỗng dùng làm giá trị mặc định khi tra cứu thất bại
const cv::Mat ForwardIndex::s_emptyMat = cv::Mat();

// ──────────────────────────────────────────────
// build()
// ──────────────────────────────────────────────

/**
 * @brief Xây dựng Forward Index từ danh sách ImageRecord.
 *
 * Duyệt vector records, gán imageID = index trong vector (0, 1, 2, ...),
 * lưu metadata và feature vectors vào hash map.
 *
 * Độ phức tạp: O(N × M) với N = số ảnh, M = số phương pháp.
 */
void ForwardIndex::build(const std::vector<ImageRecord>& records)
{
    m_index.clear();
    m_metadata.clear();

    // Reserve capacity để giảm rehash
    m_index.reserve(records.size());
    m_metadata.reserve(records.size());

    for (int i = 0; i < static_cast<int>(records.size()); ++i) {
        const auto& record = records[i];

        // Lưu metadata
        ImageMetadata meta;
        meta.imagePath = record.imagePath;
        meta.label     = record.label;
        m_metadata[i]  = std::move(meta);

        // Lưu tất cả feature vectors
        std::unordered_map<std::string, cv::Mat> featureMap;
        featureMap.reserve(record.features.size());
        for (const auto& [methodName, featureVec] : record.features) {
            featureMap[methodName] = featureVec;  // Shallow copy (cv::Mat ref count)
        }
        m_index[i] = std::move(featureMap);
    }

    std::cout << "[ForwardIndex] Da build index cho "
              << m_index.size() << " anh.\n";
}

// ──────────────────────────────────────────────
// getFeature()
// ──────────────────────────────────────────────

/**
 * @brief Tra cứu feature vector theo imageID và method.
 *
 * Trả về tham chiếu hằng để tránh copy cv::Mat.
 * Nếu không tìm thấy imageID hoặc method, trả về cv::Mat rỗng.
 */
const cv::Mat& ForwardIndex::getFeature(int imageID, const std::string& method) const
{
    auto imgIt = m_index.find(imageID);
    if (imgIt == m_index.end()) {
        return s_emptyMat;
    }

    auto featIt = imgIt->second.find(method);
    if (featIt == imgIt->second.end()) {
        return s_emptyMat;
    }

    return featIt->second;
}

// ──────────────────────────────────────────────
// getMetadata()
// ──────────────────────────────────────────────

/**
 * @brief Tra cứu metadata của ảnh theo ID.
 *
 * @throws std::out_of_range nếu imageID không tồn tại trong index.
 */
const ImageMetadata& ForwardIndex::getMetadata(int imageID) const
{
    auto it = m_metadata.find(imageID);
    if (it == m_metadata.end()) {
        throw std::out_of_range(
            "[ForwardIndex] imageID " + std::to_string(imageID) + " khong ton tai.");
    }
    return it->second;
}

// ──────────────────────────────────────────────
// Utility methods
// ──────────────────────────────────────────────

/** @brief Trả về số ảnh trong index. */
int ForwardIndex::size() const
{
    return static_cast<int>(m_metadata.size());
}

/** @brief Kiểm tra index đã được build. */
bool ForwardIndex::isBuilt() const
{
    return !m_metadata.empty();
}

/** @brief Trả về danh sách tất cả imageID. */
std::vector<int> ForwardIndex::getAllImageIDs() const
{
    std::vector<int> ids;
    ids.reserve(m_metadata.size());
    for (const auto& [id, _] : m_metadata) {
        ids.push_back(id);
    }
    return ids;
}
