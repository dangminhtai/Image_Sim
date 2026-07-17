/**
 * @file InvertedIndex.cpp
 * @brief Cài đặt Inverted Index cho truy vấn BoW trong hệ thống CBIR.
 *
 * Inverted Index xây dựng posting lists từ BoW histograms:
 *  - Mỗi bin trong histogram BoW tương ứng với một visual word.
 *  - Nếu bin có giá trị > 0, ảnh đó được thêm vào posting list của word đó.
 *  - IDF được tính theo công thức chuẩn: idf(w) = log(N / df(w)).
 *
 * Truy vấn dùng TF-IDF accumulation:
 *  - Chỉ duyệt posting lists của các words có trong query histogram.
 *  - Tích lũy score = Σ queryTF[w] × idf[w] × docTF[w] cho mỗi ảnh.
 *
 * Tham khảo:
 * - J. Sivic and A. Zisserman, "Video Google", ICCV 2003.
 * - Christopher D. Manning et al., "Introduction to Information Retrieval",
 *   Cambridge University Press, 2008.
 */

#include "InvertedIndex.h"
#include "DatabaseBuilder.h"  // Cần struct ImageRecord
#include <algorithm>
#include <cmath>
#include <iostream>
#include <unordered_map>

// ──────────────────────────────────────────────
// build()
// ──────────────────────────────────────────────

/**
 * @brief Xây dựng Inverted Index từ BoW histograms.
 *
 * Quy trình chi tiết:
 *  1. Duyệt tất cả records, lấy histogram BoW của phương pháp chỉ định.
 *  2. Xác định vocab_size từ số cột của histogram đầu tiên.
 *  3. Khởi tạo posting lists rỗng (1 list / visual word).
 *  4. Với mỗi ảnh: duyệt histogram, nếu bin[w] > 0 → thêm posting entry.
 *  5. Tính IDF: đếm document frequency (df) cho mỗi word,
 *     rồi idf[w] = log(N / df[w]).
 *
 * Độ phức tạp: O(N × V) với N = số ảnh, V = vocab_size.
 */
void InvertedIndex::build(const std::vector<ImageRecord>& records,
                          const std::string& method)
{
    m_postings.clear();
    m_idf.clear();
    m_vocabSize = 0;
    m_totalDocs = 0;

    // Bước 1: Xác định vocab_size từ histogram đầu tiên
    for (const auto& record : records) {
        auto it = record.features.find(method);
        if (it != record.features.end() && !it->second.empty()) {
            cv::Mat hist = it->second.reshape(1, 1);  // Đảm bảo 1 hàng
            m_vocabSize = hist.cols;
            break;
        }
    }

    if (m_vocabSize == 0) {
        std::cerr << "[InvertedIndex] Khong tim thay histogram cho method: "
                  << method << "\n";
        return;
    }

    // Bước 2: Khởi tạo posting lists rỗng
    m_postings.resize(m_vocabSize);

    // Bước 3: Duyệt tất cả ảnh, build posting lists
    std::vector<int> docFreq(m_vocabSize, 0);  // df[w] = số ảnh chứa word w

    for (int imgID = 0; imgID < static_cast<int>(records.size()); ++imgID) {
        auto it = records[imgID].features.find(method);
        if (it == records[imgID].features.end()) continue;

        cv::Mat hist = it->second.reshape(1, 1);
        if (hist.type() != CV_32F) {
            hist.convertTo(hist, CV_32F);
        }

        const float* histData = hist.ptr<float>();
        m_totalDocs++;

        for (int w = 0; w < m_vocabSize; ++w) {
            if (histData[w] > 1e-9f) {
                // Thêm vào posting list của word w
                PostingEntry entry;
                entry.imageID = imgID;
                entry.termFreq = histData[w];
                m_postings[w].push_back(entry);

                // Đếm document frequency
                docFreq[w]++;
            }
        }
    }

    // Bước 4: Tính IDF weights
    // idf(w) = log(N / df(w)), với df(w) = 0 thì idf = 0
    m_idf.resize(m_vocabSize);
    for (int w = 0; w < m_vocabSize; ++w) {
        if (docFreq[w] > 0) {
            m_idf[w] = std::log(static_cast<float>(m_totalDocs) / docFreq[w]);
        } else {
            m_idf[w] = 0.0f;
        }
    }

    // Thống kê
    int totalPostings = 0;
    for (const auto& pl : m_postings) {
        totalPostings += static_cast<int>(pl.size());
    }

    std::cout << "[InvertedIndex] Build xong cho method \"" << method << "\": "
              << m_vocabSize << " words, "
              << m_totalDocs << " docs, "
              << totalPostings << " posting entries.\n";
}

// ──────────────────────────────────────────────
// search()
// ──────────────────────────────────────────────

/**
 * @brief Truy vấn top-K ảnh tương đồng bằng TF-IDF scoring.
 *
 * Quy trình:
 *  1. Duyệt query histogram, chỉ xét các bin có giá trị > 0 (sparse).
 *  2. Với mỗi visual word w có trong query:
 *     - Lấy posting list của w.
 *     - Với mỗi ảnh trong posting list:
 *       score[imgID] += queryTF[w] × idf[w] × docTF[w]
 *  3. Sort theo score giảm dần.
 *  4. Trả về top-K.
 *
 * Độ phức tạp: O(Q × L_avg + C × logC)
 *   Q = số words có trong query (sparse, thường << vocab_size)
 *   L_avg = độ dài trung bình posting list
 *   C = số ảnh ứng viên (candidate set)
 */
std::vector<std::pair<int, float>> InvertedIndex::search(
    const cv::Mat& queryHist, int topK) const
{
    std::vector<std::pair<int, float>> results;

    if (!isBuilt() || queryHist.empty()) {
        return results;
    }

    cv::Mat query = queryHist.reshape(1, 1);
    if (query.type() != CV_32F) {
        query.convertTo(query, CV_32F);
    }

    if (query.cols != m_vocabSize) {
        std::cerr << "[InvertedIndex] Query histogram size ("
                  << query.cols << ") != vocab size ("
                  << m_vocabSize << ")\n";
        return results;
    }

    // Accumulate TF-IDF scores
    // Dùng vector để có chỉ số truy cập O(1) trực tiếp thay vì hash map
    std::vector<float> scores(m_totalDocs, 0.0f);
    const float* queryData = query.ptr<float>();

    for (int w = 0; w < m_vocabSize; ++w) {
        float queryTF = queryData[w];
        if (queryTF < 1e-9f) continue;  // Skip zero bins (sparse)

        float idf = m_idf[w];
        if (idf < 1e-9f) continue;  // Skip words xuất hiện ở tất cả ảnh (vô nghĩa)

        // Duyệt posting list của word w
        for (const auto& entry : m_postings[w]) {
            scores[entry.imageID] += queryTF * idf * entry.termFreq;
        }
    }

    // Chuyển sang vector và sort giảm dần theo score
    results.reserve(m_totalDocs);
    for (int imgID = 0; imgID < m_totalDocs; ++imgID) {
        if (scores[imgID] > 0.0f) {
            results.emplace_back(imgID, scores[imgID]);
        }
    }

    std::sort(results.begin(), results.end(),
              [](const auto& a, const auto& b) {
                  return a.second > b.second;  // Score cao = tương đồng hơn
              });

    // Giữ top-K
    if (static_cast<int>(results.size()) > topK) {
        results.resize(topK);
    }

    return results;
}

// ──────────────────────────────────────────────
// Utility methods
// ──────────────────────────────────────────────

/** @brief Trả về kích thước vocabulary. */
int InvertedIndex::getVocabSize() const
{
    return m_vocabSize;
}

/** @brief Kiểm tra index đã build. */
bool InvertedIndex::isBuilt() const
{
    return m_vocabSize > 0 && m_totalDocs > 0;
}

/** @brief Trả về tổng số ảnh đã index. */
int InvertedIndex::getTotalDocs() const
{
    return m_totalDocs;
}
