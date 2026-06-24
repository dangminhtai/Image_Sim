/**
 * @file build_features.cpp
 * @brief Công cụ dòng lệnh xây dựng CSDL đặc trưng từ thư mục ảnh.
 *
 * Cách dùng:
 *   build_features.exe <db_path> [output.yml] [vocab_dir]
 *
 * Tham số:
 *   db_path    : Đường dẫn thư mục CSDL ảnh (bắt buộc)
 *   output.yml : Tên file đầu ra (tùy chọn, mặc định: features.yml)
 *   vocab_dir  : Thư mục lưu vocabulary BoW (tùy chọn, mặc định: vocab/)
 *
 * Ví dụ:
 *   build_features.exe data/TMBuD features_tmbud.yml vocab/tmbud/
 *   build_features.exe data/CD    features_cd.yml    vocab/cd/
 */

#include "DatabaseBuilder.h"
#include "ColorHistogram.h"
#include "ColorCorrelogram.h"
#include "SIFTExtractor.h"
#include "ORBExtractor.h"
#include "HOGExtractor.h"
#include "LBPExtractor.h"
#include "FusionExtractor.h"

#include <iostream>
#include <filesystem>
#include <chrono>
#include <iomanip>

namespace fs = std::filesystem;

/**
 * @brief In thanh tiến độ (Progress Bar) dạng console.
 *
 * Hàm này dùng để vẽ một thanh tiến độ đẹp mắt dưới console
 * giúp theo dõi quá trình duyệt và rút trích đặc trưng của Database.
 * 
 * @param current Số lượng ảnh hiện tại đã xử lý.
 * @param total Tổng số lượng ảnh trong tập CSDL.
 */
void printProgress(int current, int total, const std::string& prefix = "Processing")
{
    int barWidth = 30;
    float progress = (total > 0) ? static_cast<float>(current) / total : 1.0f;
    int filled = static_cast<int>(barWidth * progress);

    std::cout << "\r" << prefix << ": [" ;
    for (int i = 0; i < barWidth; i++) {
        if (i < filled) std::cout << "=";
        else if (i == filled) std::cout << ">";
        else std::cout << "-";
    }
    std::cout << "| " << current << "/" << total << " ";
    std::cout.flush();
}

static std::string getBasePath() {
    if (fs::exists("data")) return "";
    if (fs::exists("../data")) return "../";
    if (fs::exists("../../data")) return "../../";
    if (fs::exists("../../../data")) return "../../../";
    return "";
}

/**
 * @brief Hàm main — điểm vào của chương trình build_features.
 */
int main(int argc, char* argv[])
{
    // ── Xử lý tham số dòng lệnh ──────────────────────
    std::string base = getBasePath();
    std::string dbPath     = base + "data/TMBuD/images";
    std::string outputFile = base + "data/features_TMBuD.yml";
    std::string vocabDir   = base + "data/vocab_TMBuD/";

    if (argc >= 2) dbPath = argv[1];
    if (argc >= 3) outputFile = argv[2];
    if (argc >= 4) vocabDir = argv[3];

    // Tạo thư mục vocab nếu chưa có
    fs::create_directories(vocabDir);

    std::string siftVocabPath = vocabDir + "sift_vocab.yml";
    std::string orbVocabPath  = vocabDir + "orb_vocab.yml";

    // ── Khởi tạo các extractor ───────────────────────
    std::cout << "=== BUILD FEATURES ===\n";
    std::cout << "Thu muc CSDL : " << dbPath     << "\n";
    std::cout << "File dau ra  : " << outputFile  << "\n";
    std::cout << "Thu muc vocab: " << vocabDir    << "\n\n";

    // ColorHistogram: 32 bins mỗi kênh H, S, V → vector 96 chiều
    auto colorHist = std::make_shared<ColorHistogram>(32, 32, 32);

    // ColorCorrelogram: 8 màu, khoảng cách d = {1,3,5,7} → vector 32 chiều
    auto colorCorr = std::make_shared<ColorCorrelogram>(8, std::vector<int>{1, 3, 5, 7});

    // SIFT BoW: 500 từ → vector 500 chiều
    auto siftExt = std::make_shared<SIFTExtractor>(500, 0);

    // ORB BoW: 500 từ, 500 keypoints/ảnh → vector 500 chiều
    auto orbExt  = std::make_shared<ORBExtractor>(500, 500);

    // HOG: 64x64 ảnh đầu vào
    auto hogExt  = std::make_shared<HOGExtractor>(cv::Size(64, 64));

    // LBP: Local Binary Pattern
    auto lbpExt  = std::make_shared<LBPExtractor>();

    // Fusion: Nối ColorHistogram và HOG
    auto fusionExt = std::make_shared<FusionExtractor>(colorHist, hogExt, "Color_HOG");

    // Thử nạp vocabulary đã lưu từ lần trước (tránh tính lại)
    bool siftLoaded = siftExt->loadVocabulary(siftVocabPath);
    bool orbLoaded  = orbExt->loadVocabulary(orbVocabPath);

    if (!siftLoaded) std::cout << "[Info] SIFT vocabulary chua co, se tu dong xay.\n";
    if (!orbLoaded)  std::cout << "[Info] ORB vocabulary chua co, se tu dong xay.\n";

    // ── Khởi tạo DatabaseBuilder ─────────────────────
    DatabaseBuilder builder;
    builder.addExtractor(colorHist);
    builder.addExtractor(colorCorr);
    builder.addExtractor(siftExt);
    builder.addExtractor(orbExt);
    builder.addExtractor(hogExt);
    builder.addExtractor(lbpExt);
    builder.addExtractor(fusionExt);

    // ── Xây dựng CSDL đặc trưng ──────────────────────
    auto startTime = std::chrono::high_resolution_clock::now();

    try {
        builder.buildDatabase(dbPath, [](int current, int total) { 
            printProgress(current, total, "Extracting features"); 
        });
        std::cout << "\n"; // Newline sau thanh tiến độ
    }
    catch (const std::exception& e) {
        std::cerr << "\n[LOI] " << e.what() << "\n";
        return 1;
    }

    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsed = std::chrono::duration<double>(endTime - startTime).count();
    std::cout << "Thoi gian rut trich: " << std::fixed << std::setprecision(2)
              << elapsed << " giay\n\n";

    // ── Lưu vocabulary BoW ───────────────────────────
    if (!siftLoaded) {
        siftExt->saveVocabulary(siftVocabPath);
    }
    if (!orbLoaded) {
        orbExt->saveVocabulary(orbVocabPath);
    }

    // ── Lưu CSDL đặc trưng ──────────────────────────
    bool saved = builder.saveDatabase(outputFile);
    if (!saved) {
        std::cerr << "[LOI] Khong the luu CSDL dac trung.\n";
        return 1;
    }

    // ── Tổng kết ─────────────────────────────────────
    std::cout << "\n=== HOAN THANH ===\n";
    std::cout << "  Tong so anh     : " << builder.size() << "\n";
    std::cout << "  Thoi gian       : " << elapsed << "s\n";
    std::cout << "  File dac trung  : " << outputFile << "\n";
    std::cout << "  SIFT vocab      : " << siftVocabPath << "\n";
    std::cout << "  ORB  vocab      : " << orbVocabPath  << "\n";

    return 0;
}
