/**
 * @file InvertedIndex.h
 * @brief Inverted Index: ánh xạ visual word → danh sách ảnh chứa word đó.
 *
 * Inverted Index là cấu trúc dữ liệu cốt lõi trong Information Retrieval,
 * cho phép tra cứu ngược từ một "từ" (visual word) ra danh sách "tài liệu"
 * (ảnh) chứa từ đó, kèm theo trọng số TF-IDF.
 *
 * Cấu trúc logic (ví dụ với SIFT BoW, vocab_size = 500):
 *   word_0   → [(img_3, tf=0.12), (img_17, tf=0.08), ...]
 *   word_1   → [(img_1, tf=0.15), (img_3, tf=0.02)]
 *   ...
 *   word_499 → [(img_7, tf=0.10)]
 *
 * Chỉ áp dụng cho các phương pháp tạo ra BoW histogram
 * (SIFT, ORB) vì chúng có vocabulary rời rạc.
 *
 * Tham khảo:
 * - Christopher D. Manning et al., "Introduction to Information Retrieval",
 *   Cambridge University Press, 2008, Chapters 1-6.
 * - J. Sivic and A. Zisserman, "Video Google: A Text Retrieval Approach
 *   to Object Matching in Videos", ICCV 2003.
 */
#pragma once

#include <opencv2/core.hpp>
#include <string>
#include <vector>
#include <utility>

/**
 * @struct PostingEntry
 * @brief Một entry trong posting list: lưu ID ảnh và tần suất term.
 *
 * Tương tự khái niệm posting entry trong Information Retrieval:
 * mỗi entry ghi lại một "document" (ảnh) có chứa "term" (visual word)
 * cùng với trọng số tần suất.
 */
struct PostingEntry {
    int imageID;      ///< ID của ảnh trong CSDL
    float termFreq;   ///< Term Frequency: tần suất visual word trong ảnh (đã chuẩn hóa L1)
};

/**
 * @class InvertedIndex
 * @brief Cấu trúc Inverted Index cho truy vấn BoW trong CBIR.
 *
 * Quy trình hoạt động:
 *  1. **Build**: Từ BoW histograms trong ForwardIndex, xây dựng posting lists
 *     và tính trọng số IDF cho mỗi visual word.
 *  2. **Search**: Với query histogram, chỉ duyệt các posting lists
 *     tương ứng với visual words có trong query (sparse lookup),
 *     tích lũy score TF-IDF cho mỗi ảnh ứng viên.
 *
 * Công thức TF-IDF scoring:
 *   score(query, doc) = Σ_w [ tf(w, query) × idf(w) × tf(w, doc) ]
 *
 * Trong đó:
 *   - tf(w, query) = giá trị histogram query tại bin w
 *   - idf(w) = log(N / df(w)), N = tổng số ảnh, df(w) = số ảnh chứa word w
 *   - tf(w, doc) = giá trị histogram ảnh trong CSDL tại bin w
 *
 * Sử dụng:
 * @code
 *   InvertedIndex invIdx;
 *   invIdx.build(imageRecords, "SIFT");  // Build từ BoW histograms
 *
 *   // Truy vấn: queryHist là BoW histogram của ảnh query
 *   auto results = invIdx.search(queryHist, 10);
 *   // results = vector<(imageID, score)>, sắp xếp giảm dần theo score
 * @endcode
 */
class InvertedIndex {
public:
    /**
     * @brief Constructor mặc định.
     */
    InvertedIndex() = default;

    /**
     * @brief Xây dựng Inverted Index từ danh sách ImageRecord.
     *
     * Quy trình:
     *  1. Duyệt tất cả ảnh, lấy BoW histogram của phương pháp chỉ định.
     *  2. Với mỗi bin có giá trị > 0 → thêm posting entry (imageID, tf).
     *  3. Tính IDF = log(N / df) cho mỗi visual word.
     *
     * @param records Danh sách bản ghi ảnh từ DatabaseBuilder.
     * @param method  Tên phương pháp BoW (vd: "SIFT", "ORB").
     */
    void build(const std::vector<struct ImageRecord>& records,
               const std::string& method);

    /**
     * @brief Truy vấn top-K ảnh tương đồng nhất bằng TF-IDF scoring.
     *
     * Chỉ duyệt posting lists của các visual words xuất hiện trong query
     * (sparse lookup), không cần duyệt toàn bộ CSDL.
     *
     * @param queryHist BoW histogram của ảnh truy vấn (1×vocabSize, CV_32F).
     * @param topK      Số lượng kết quả trả về.
     * @return Danh sách (imageID, score) sắp xếp theo score giảm dần.
     *         Score cao hơn = tương đồng hơn.
     */
    std::vector<std::pair<int, float>> search(const cv::Mat& queryHist,
                                               int topK) const;

    /**
     * @brief Trả về kích thước vocabulary (số visual words).
     */
    int getVocabSize() const;

    /**
     * @brief Kiểm tra index đã được build chưa.
     */
    bool isBuilt() const;

    /**
     * @brief Trả về tổng số ảnh đã index.
     */
    int getTotalDocs() const;

private:
    /// Posting lists: m_postings[wordID] = danh sách (imageID, tf) chứa word đó
    std::vector<std::vector<PostingEntry>> m_postings;

    /// IDF weights: m_idf[wordID] = log(N / df(wordID))
    std::vector<float> m_idf;

    int m_vocabSize = 0;   ///< Kích thước vocabulary
    int m_totalDocs = 0;   ///< Tổng số ảnh đã index
};
