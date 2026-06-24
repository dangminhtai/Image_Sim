/**
 * @file DatabaseBuilder.cpp
 * @brief Cài đặt module xây dựng và quản lý CSDL đặc trưng ảnh.
 *
 * Dùng std::filesystem (C++17) để duyệt thư mục đệ quy.
 * Lưu/nạp CSDL dùng OpenCV FileStorage (định dạng YAML).
 */

#include "DatabaseBuilder.h"
#include "SIFTExtractor.h"
#include "ORBExtractor.h"

#include <opencv2/imgcodecs.hpp>
#include <filesystem>
#include <iostream>
#include <algorithm>
#include <stdexcept>

namespace fs = std::filesystem;

// ──────────────────────────────────────────────
// addExtractor()
// ──────────────────────────────────────────────

/**
 * @brief Đăng ký một FeatureExtractor vào hệ thống.
 */
void DatabaseBuilder::addExtractor(std::shared_ptr<FeatureExtractor> extractor)
{
    if (extractor) {
        m_extractors.push_back(extractor);
    }
}

// ──────────────────────────────────────────────
// isImageFile() — kiểm tra định dạng ảnh
// ──────────────────────────────────────────────

/**
 * @brief Kiểm tra file có phải định dạng ảnh hỗ trợ không.
 */
bool DatabaseBuilder::isImageFile(const std::string& filename) const
{
    // Lấy phần đuôi mở rộng, chuyển thành lowercase
    fs::path p(filename);
    std::string ext = p.extension().string();
    std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

    return (ext == ".jpg" || ext == ".jpeg" ||
            ext == ".png" || ext == ".bmp"  ||
            ext == ".tif" || ext == ".tiff");
}

// ──────────────────────────────────────────────
// extractLabel() — lấy nhãn từ tên thư mục cha
// ──────────────────────────────────────────────

/**
 * @brief Lấy nhãn lớp của ảnh.
 *
 * Hỗ trợ 2 kiểu cấu trúc dataset:
 *  1. Folder-based (TMBuD): "data/TMBuD/cat/001.jpg" → nhãn = "cat"
 *     (thư mục cha KHÔNG phải training_images/TestImages)
 *  2. Filename-prefix (CD): "data/training_images/01_1.jpg" → nhãn = "01"
 *     (tên file có dạng XX_Y.jpg hoặc XX.jpg, lấy phần trước "_" hoặc ".")
 *
 * Quy tắc: Nếu thư mục cha là một trong các tên tổng (training_images,
 * TestImages, TMBuD...) thì lấy prefix từ tên file; ngược lại lấy tên thư mục cha.
 */
std::string DatabaseBuilder::extractLabel(const std::string& imagePath) const
{
    fs::path p(imagePath);
    std::string parentName = p.parent_path().filename().string();

    // Danh sách các thư mục gốc dataset (không phải nhãn lớp)
    static const std::vector<std::string> rootDirNames = {
        "training_images", "TestImages", "TMBuD", "CD", "data", "images"
    };

    bool isRootDir = false;
    for (const auto& name : rootDirNames) {
        if (parentName == name) { isRootDir = true; break; }
    }

    if (isRootDir) {
        // Lấy prefix từ tên file (không có extension)
        std::string stem = p.stem().string();

        // Trường hợp TMBuD: tên file 5 chữ số "XXXYY" → lấy 3 chữ đầu làm lớp
        // Ví dụ: "00101" → "001", "00501" → "005"
        if (stem.size() == 5 && std::all_of(stem.begin(), stem.end(), ::isdigit)) {
            return stem.substr(0, 3);
        }

        // Trường hợp CD dataset: "01_1" → "01", "01" → "01"
        size_t pos = stem.find('_');
        if (pos != std::string::npos) {
            return stem.substr(0, pos);
        }
        return stem;
    }

    // Thư mục cha là tên lớp (kiểu TMBuD)
    return parentName;
}

// ──────────────────────────────────────────────
// scanImageDirectory()
// ──────────────────────────────────────────────

/**
 * @brief Quét thư mục CSDL ảnh và trả về danh sách đường dẫn.
 */
std::vector<std::string> DatabaseBuilder::scanImageDirectory(const std::string& dbPath)
{
    std::vector<std::string> imagePaths;

    if (!fs::exists(dbPath) || !fs::is_directory(dbPath)) {
        std::cerr << "[DatabaseBuilder] Thu muc khong ton tai: " << dbPath << "\n";
        return imagePaths;
    }

    // Duyệt đệ quy tất cả file trong thư mục
    for (const auto& entry : fs::recursive_directory_iterator(dbPath)) {
        if (entry.is_regular_file() && isImageFile(entry.path().string())) {
            imagePaths.push_back(entry.path().string());
        }
    }

    // Sắp xếp để kết quả nhất quán
    std::sort(imagePaths.begin(), imagePaths.end());

    std::cout << "[DatabaseBuilder] Tim thay " << imagePaths.size()
              << " anh trong: " << dbPath << "\n";
    return imagePaths;
}

// ──────────────────────────────────────────────
// buildDatabase()
// ──────────────────────────────────────────────

/**
 * @brief Xây dựng CSDL đặc trưng từ thư mục ảnh.
 *
 * Bước 1: Quét thư mục để lấy danh sách ảnh.
 * Bước 2: Build vocabulary cho SIFT/ORB nếu cần.
 * Bước 3: Với mỗi ảnh, gọi tất cả extractor và lưu kết quả.
 */
void DatabaseBuilder::buildDatabase(const std::string& dbPath,
                                    std::function<void(int, int)> progressCallback)
{
    m_database.clear();

    // Bước 1: Lấy danh sách ảnh
    std::vector<std::string> imagePaths = scanImageDirectory(dbPath);
    if (imagePaths.empty()) {
        throw std::runtime_error("Khong tim thay anh nao trong: " + dbPath);
    }

    int total = static_cast<int>(imagePaths.size());

    // Bước 2: Build vocabulary cho các extractor cần BoW
    for (auto& extractor : m_extractors) {
        // Dynamic cast để kiểm tra có phải SIFT/ORB không
        auto* siftExt = dynamic_cast<SIFTExtractor*>(extractor.get());
        if (siftExt && !siftExt->isVocabularyReady()) {
            std::cout << "[DatabaseBuilder] Xay SIFT vocabulary...\n";
            siftExt->buildVocabulary(imagePaths);
        }

        auto* orbExt = dynamic_cast<ORBExtractor*>(extractor.get());
        if (orbExt && !orbExt->isVocabularyReady()) {
            std::cout << "[DatabaseBuilder] Xay ORB vocabulary...\n";
            orbExt->buildVocabulary(imagePaths);
        }
    }

    // Bước 3: Rút trích đặc trưng từng ảnh
    std::cout << "[DatabaseBuilder] Bat dau rut trich dac trung cho "
              << total << " anh...\n";

    for (int i = 0; i < total; i++) {
        const std::string& path = imagePaths[i];

        // Đọc ảnh
        cv::Mat img = cv::imread(path, cv::IMREAD_COLOR);
        if (img.empty()) {
            std::cerr << "[DatabaseBuilder] Bo qua anh loi: " << path << "\n";
            continue;
        }

        // Tạo bản ghi
        ImageRecord record;
        record.imagePath = path;
        record.label = extractLabel(path);

        // Gọi từng extractor
        for (const auto& extractor : m_extractors) {
            cv::Mat feat = extractor->extract(img);
            if (!feat.empty()) {
                record.features[extractor->getName()] = feat.clone();
            }
        }

        m_database.push_back(std::move(record));

        // Báo tiến độ
        if (progressCallback) {
            progressCallback(i + 1, total);
        }

        // In tiến độ mỗi 50 ảnh
        if ((i + 1) % 50 == 0 || (i + 1) == total) {
            std::cout << "[DatabaseBuilder] " << (i + 1) << "/" << total
                      << " anh hoan thanh\n";
        }
    }

    std::cout << "[DatabaseBuilder] HOAN THANH xay CSDL voi "
              << m_database.size() << " ban ghi.\n";
}

// ──────────────────────────────────────────────
// saveDatabase()
// ──────────────────────────────────────────────

/**
 * @brief Lưu CSDL đặc trưng ra file YAML.
 *
 * Cấu trúc YAML:
 *   totalImages: N
 *   images:
 *     - path: "..."
 *       label: "..."
 *       ColorHistogram: [...]
 *       SIFT: [...]
 */
bool DatabaseBuilder::saveDatabase(const std::string& outputPath) const
{
    if (m_database.empty()) {
        std::cerr << "[DatabaseBuilder] CSDL rong, khong co gi de luu.\n";
        return false;
    }

    cv::FileStorage fs(outputPath, cv::FileStorage::WRITE);
    if (!fs.isOpened()) {
        std::cerr << "[DatabaseBuilder] Khong the tao file: " << outputPath << "\n";
        return false;
    }

    fs << "totalImages" << static_cast<int>(m_database.size());
    fs << "images" << "["; // Bắt đầu sequence

    for (const auto& record : m_database) {
        fs << "{"; // Bắt đầu mapping cho 1 ảnh
        fs << "path"  << record.imagePath;
        fs << "label" << record.label;

        // Lưu từng vector đặc trưng
        for (const auto& [name, feat] : record.features) {
            fs << name << feat;
        }

        fs << "}"; // Kết thúc mapping
    }

    fs << "]"; // Kết thúc sequence
    fs.release();

    std::cout << "[DatabaseBuilder] Da luu CSDL (" << m_database.size()
              << " anh) vao: " << outputPath << "\n";
    return true;
}

// ──────────────────────────────────────────────
// loadDatabase()
// ──────────────────────────────────────────────

/**
 * @brief Nạp CSDL đặc trưng từ file YAML.
 */
bool DatabaseBuilder::loadDatabase(const std::string& filePath)
{
    cv::FileStorage fs(filePath, cv::FileStorage::READ);
    if (!fs.isOpened()) {
        std::cerr << "[DatabaseBuilder] Khong mo duoc: " << filePath << "\n";
        return false;
    }

    m_database.clear();

    int total = 0;
    fs["totalImages"] >> total;

    cv::FileNode imagesNode = fs["images"];
    if (imagesNode.type() != cv::FileNode::SEQ) {
        std::cerr << "[DatabaseBuilder] Dinh dang file khong hop le.\n";
        return false;
    }

    for (const auto& node : imagesNode) {
        ImageRecord record;
        node["path"]  >> record.imagePath;
        node["label"] >> record.label;

        // Nạp tất cả feature vectors (mọi key trừ path và label)
        for (const auto& item : node) {
            std::string key = item.name();
            if (key == "path" || key == "label") continue;
            cv::Mat feat;
            item >> feat;
            if (!feat.empty()) {
                record.features[key] = feat;
            }
        }

        m_database.push_back(std::move(record));
    }

    std::cout << "[DatabaseBuilder] Da nap " << m_database.size()
              << " ban ghi tu: " << filePath << "\n";
    return true;
}

// ──────────────────────────────────────────────
// Getters
// ──────────────────────────────────────────────

/**
 * @brief Trả về danh sách bản ghi CSDL.
 */
const std::vector<ImageRecord>& DatabaseBuilder::getDatabase() const
{
    return m_database;
}

/**
 * @brief Trả về số lượng ảnh trong CSDL.
 */
int DatabaseBuilder::size() const
{
    return static_cast<int>(m_database.size());
}
