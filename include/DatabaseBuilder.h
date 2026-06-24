/**
 * @file DatabaseBuilder.h
 * @brief Module xây dựng và quản lý CSDL đặc trưng ảnh.
 *
 * DatabaseBuilder đảm nhận vai trò:
 *  1. Quét và đọc toàn bộ ảnh từ thư mục CSDL.
 *  2. Gọi các FeatureExtractor đã đăng ký để rút trích đặc trưng.
 *  3. Lưu toàn bộ kết quả ra file CSDL đặc trưng (.yml) dùng OpenCV FileStorage.
 *  4. Nạp lại CSDL đặc trưng từ file khi cần truy vấn.
 *
 * Cấu trúc file CSDL (.yml):
 *   images:
 *     - path: "data/TMBuD/cat/001.jpg"
 *       label: "cat"
 *       ColorHistogram: [0.1, 0.2, ...]
 *       SIFT: [0.0, 0.3, ...]
 *       ORB: [...]
 *   ...
 */
#pragma once

#include "FeatureExtractor.h"
#include <memory>
#include <vector>
#include <string>
#include <map>
#include <functional>

/**
 * @struct ImageRecord
 * @brief Lưu thông tin và đặc trưng của một tấm ảnh trong CSDL.
 */
struct ImageRecord {
    std::string imagePath;                        ///< Đường dẫn file ảnh gốc
    std::string label;                            ///< Nhãn lớp ảnh (tên thư mục cha)
    std::map<std::string, cv::Mat> features;      ///< Map: tên extractor → vector đặc trưng
};

/**
 * @class DatabaseBuilder
 * @brief Xây dựng và quản lý CSDL đặc trưng ảnh.
 */
class DatabaseBuilder {
public:
    /**
     * @brief Constructor mặc định.
     */
    DatabaseBuilder() = default;

    /**
     * @brief Đăng ký một FeatureExtractor vào hệ thống.
     * @param extractor Con trỏ shared đến extractor cần thêm.
     */
    void addExtractor(std::shared_ptr<FeatureExtractor> extractor);

    /**
     * @brief Quét thư mục CSDL ảnh và tìm tất cả file ảnh hợp lệ.
     *
     * Hỗ trợ các định dạng: .jpg, .jpeg, .png, .bmp.
     * Nhãn lớp được lấy tự động từ tên thư mục cha của mỗi ảnh.
     *
     * @param dbPath Đường dẫn thư mục gốc của CSDL ảnh.
     * @return Danh sách đường dẫn các ảnh tìm được.
     */
    std::vector<std::string> scanImageDirectory(const std::string& dbPath);

    /**
     * @brief Xây dựng CSDL đặc trưng từ thư mục ảnh.
     *
     * Với mỗi ảnh tìm được, gọi tất cả extractor đã đăng ký để rút trích.
     * Kết quả lưu trong m_database. Tiến độ được in ra console.
     *
     * Với SIFTExtractor và ORBExtractor: tự động gọi buildVocabulary()
     * trước khi extract() nếu vocabulary chưa sẵn sàng.
     *
     * @param dbPath Đường dẫn thư mục gốc của CSDL ảnh.
     * @param progressCallback Hàm callback báo tiến độ (optional).
     *        Tham số: (ảnh hiện tại, tổng số ảnh).
     */
    void buildDatabase(const std::string& dbPath,
                       std::function<void(int, int)> progressCallback = nullptr);

    /**
     * @brief Lưu CSDL đặc trưng đã xây dựng ra file YAML.
     * @param outputPath Đường dẫn file đầu ra (ví dụ: "features.yml").
     * @return true nếu lưu thành công.
     */
    bool saveDatabase(const std::string& outputPath) const;

    /**
     * @brief Nạp CSDL đặc trưng từ file YAML đã lưu.
     * @param filePath Đường dẫn file cần nạp.
     * @return true nếu nạp thành công.
     */
    bool loadDatabase(const std::string& filePath);

    /**
     * @brief Trả về danh sách bản ghi đặc trưng trong CSDL.
     * @return Tham chiếu hằng đến vector ImageRecord.
     */
    const std::vector<ImageRecord>& getDatabase() const;

    /**
     * @brief Trả về số lượng ảnh trong CSDL.
     */
    int size() const;

private:
    std::vector<std::shared_ptr<FeatureExtractor>> m_extractors; ///< Danh sách extractor
    std::vector<ImageRecord> m_database;                          ///< CSDL đặc trưng trong RAM

    /**
     * @brief Lấy nhãn lớp từ đường dẫn ảnh (tên thư mục cha).
     * @param imagePath Đường dẫn đầy đủ của ảnh.
     * @return Tên thư mục cha làm nhãn.
     */
    std::string extractLabel(const std::string& imagePath) const;

    /**
     * @brief Kiểm tra một file có phải định dạng ảnh hỗ trợ không.
     * @param filename Tên file cần kiểm tra.
     * @return true nếu đuôi file là .jpg, .jpeg, .png, .bmp.
     */
    bool isImageFile(const std::string& filename) const;
};
