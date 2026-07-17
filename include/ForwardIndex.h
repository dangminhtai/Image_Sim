/**
 * @file ForwardIndex.h
 * @brief Forward Index: ánh xạ imageID → tập hợp feature vectors.
 *
 * Forward Index là cấu trúc dữ liệu cho phép tra cứu nhanh O(1)
 * feature vector của một ảnh theo ID và tên phương pháp trích xuất.
 *
 * Cấu trúc logic:
 *   imageID_0 → { "ColorHistogram": vec[96], "SIFT": vec[500], ... }
 *   imageID_1 → { "ColorHistogram": vec[96], "SIFT": vec[500], ... }
 *   ...
 *
 * Khác với cách lưu tuần tự trong vector<ImageRecord>, ForwardIndex
 * dùng hash map để truy xuất O(1) thay vì duyệt tuyến tính O(N).
 *
 * Tham khảo:
 * - Christopher D. Manning et al., "Introduction to Information Retrieval",
 *   Cambridge University Press, 2008, Chapter 1: Boolean retrieval.
 */
#pragma once

#include <opencv2/core.hpp>
#include <string>
#include <unordered_map>
#include <vector>

/**
 * @struct ImageMetadata
 * @brief Thông tin metadata của một ảnh trong CSDL.
 *
 * Tách riêng metadata (path, label) khỏi feature data
 * để giảm coupling và tiết kiệm bộ nhớ khi chỉ cần tra cứu thông tin ảnh.
 */
struct ImageMetadata {
    std::string imagePath;  ///< Đường dẫn file ảnh gốc
    std::string label;      ///< Nhãn lớp (ground truth)
};

/**
 * @class ForwardIndex
 * @brief Cấu trúc Forward Index cho hệ thống CBIR.
 *
 * Cho phép tra cứu nhanh:
 *  - Feature vector theo (imageID, methodName) → O(1)
 *  - Metadata theo imageID → O(1)
 *  - Tất cả features của một method → O(N) nhưng truy cập liên tục
 *
 * Sử dụng:
 * @code
 *   ForwardIndex fwdIdx;
 *   fwdIdx.build(imageRecords);  // Xây dựng từ vector<ImageRecord>
 *
 *   // Tra cứu feature của ảnh ID=5, phương pháp "SIFT"
 *   cv::Mat feat = fwdIdx.getFeature(5, "SIFT");
 *
 *   // Lấy metadata
 *   const auto& meta = fwdIdx.getMetadata(5);
 *   std::cout << meta.imagePath << ", " << meta.label;
 * @endcode
 */
class ForwardIndex {
public:
    /**
     * @brief Constructor mặc định.
     */
    ForwardIndex() = default;

    /**
     * @brief Xây dựng Forward Index từ danh sách ImageRecord.
     *
     * Mỗi ImageRecord trong vector được gán một imageID tăng dần (0, 1, 2, ...),
     * trùng với index trong vector gốc để đảm bảo tương thích ngược.
     *
     * @param records Danh sách bản ghi ảnh từ DatabaseBuilder.
     */
    void build(const std::vector<struct ImageRecord>& records);

    /**
     * @brief Tra cứu feature vector của một ảnh theo ID và tên phương pháp.
     *
     * @param imageID ID của ảnh (index trong CSDL).
     * @param method  Tên phương pháp trích xuất (vd: "ColorHistogram", "SIFT").
     * @return Tham chiếu hằng đến cv::Mat chứa feature vector.
     *         Trả về cv::Mat rỗng nếu không tìm thấy.
     */
    const cv::Mat& getFeature(int imageID, const std::string& method) const;

    /**
     * @brief Tra cứu metadata (path, label) của một ảnh theo ID.
     *
     * @param imageID ID của ảnh.
     * @return Tham chiếu hằng đến ImageMetadata.
     * @throws std::out_of_range nếu imageID không tồn tại.
     */
    const ImageMetadata& getMetadata(int imageID) const;

    /**
     * @brief Trả về tổng số ảnh trong index.
     */
    int size() const;

    /**
     * @brief Kiểm tra index đã được build chưa.
     */
    bool isBuilt() const;

    /**
     * @brief Trả về danh sách tất cả imageID có trong index.
     */
    std::vector<int> getAllImageIDs() const;

private:
    /// imageID → (methodName → feature vector)
    std::unordered_map<int, std::unordered_map<std::string, cv::Mat>> m_index;

    /// imageID → metadata (path, label)
    std::unordered_map<int, ImageMetadata> m_metadata;

    /// Feature rỗng trả về khi không tìm thấy
    static const cv::Mat s_emptyMat;
};
