/**
 * @file main_gui.cpp
 * @brief Điểm vào của ứng dụng GUI CBIR.
 *
 * Cách dùng:
 *   cbir_gui.exe <features.yml> [vocab_dir]
 *
 * Tham số:
 *   features.yml : File CSDL đặc trưng đã xây bằng build_features.exe (bắt buộc)
 *   vocab_dir    : Thư mục chứa vocabulary BoW (tùy chọn, mặc định: vocab/)
 *
 * Ví dụ:
 *   cbir_gui.exe data/features_CD.yml data/vocab_CD/
 *   cbir_gui.exe data/features_TMBuD.yml data/vocab_TMBuD/
 */

#include "GUIApp.h"
#include "ImageSearchEngine.h"
#include "ColorHistogram.h"
#include "ColorCorrelogram.h"
#include "SIFTExtractor.h"
#include "ORBExtractor.h"
#include "HOGExtractor.h"
#include "LBPExtractor.h"
#include "FusionExtractor.h"

#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

static std::string getBasePath() {
    if (fs::exists("data")) return "";
    if (fs::exists("../data")) return "../";
    if (fs::exists("../../data")) return "../../";
    if (fs::exists("../../../data")) return "../../../";
    return "";
}

/**
 * @brief Hàm main — khởi tạo engine và chạy GUI.
 */
int main(int argc, char* argv[])
{
    // ── Xử lý tham số ────────────────────────────────
    // Đặt đường dẫn mặc định để có thể click chạy trực tiếp từ Visual Studio
    // mà không cần truyền Command Arguments.
    std::string base = getBasePath();
    std::string featuresFile = base + "data/features_TMBuD.yml";
    std::string vocabDir     = base + "data/vocab_TMBuD/";

    if (argc >= 2) {
        featuresFile = argv[1];
    }
    if (argc >= 3) {
        vocabDir = argv[2];
    }

    // Đường dẫn vocabulary
    std::string siftVocabPath = vocabDir + "sift_vocab.yml";
    std::string orbVocabPath  = vocabDir + "orb_vocab.yml";

    // ── Khởi tạo các extractor ───────────────────────
    std::cout << "[Init] Khoi tao extractors...\n";

    auto colorHist = std::make_shared<ColorHistogram>(32, 32, 32);
    auto colorCorr = std::make_shared<ColorCorrelogram>(8, std::vector<int>{1, 3, 5, 7});
    auto siftExt   = std::make_shared<SIFTExtractor>(500, 0);
    auto orbExt    = std::make_shared<ORBExtractor>(500, 500);
    auto hogExt    = std::make_shared<HOGExtractor>(cv::Size(64, 64));
    auto lbpExt    = std::make_shared<LBPExtractor>();
    auto fusionExt = std::make_shared<FusionExtractor>(colorHist, hogExt, "Color_HOG");

    // Nạp vocabulary BoW (cần thiết cho SIFT/ORB)
    bool siftOK = siftExt->loadVocabulary(siftVocabPath);
    bool orbOK  = orbExt->loadVocabulary(orbVocabPath);

    if (!siftOK) {
        std::cerr << "[Canh bao] Khong the nap SIFT vocabulary tu: " << siftVocabPath
                  << "\n           SIFT se khong hoat dong.\n";
    }
    if (!orbOK) {
        std::cerr << "[Canh bao] Khong the nap ORB vocabulary tu: " << orbVocabPath
                  << "\n           ORB se khong hoat dong.\n";
    }

    // ── Khởi tạo Search Engine ───────────────────────
    std::cout << "[Init] Nap CSDL dac trung: " << featuresFile << "\n";

    ImageSearchEngine engine;
    engine.addExtractor(colorHist);
    engine.addExtractor(colorCorr);
    engine.addExtractor(siftExt);
    engine.addExtractor(orbExt);
    engine.addExtractor(hogExt);
    engine.addExtractor(lbpExt);
    engine.addExtractor(fusionExt);

    if (!engine.loadDatabase(featuresFile)) {
        std::cerr << "[LOI] Khong the nap CSDL tu: " << featuresFile << "\n";
        std::cerr << "      Hay chay build_features.exe truoc.\n";
        return 1;
    }

    std::cout << "[Init] CSDL san sang: " << engine.getDatabaseSize() << " anh\n";
    std::cout << "[Init] Phuong phap: ";
    for (const auto& m : engine.getAvailableMethods()) std::cout << m << " ";
    std::cout << "\n";

    // ── Khởi chạy GUI ────────────────────────────────
    std::cout << "[Init] Khoi chay GUI...\n";
    std::cout << "       Nhan 'q' hoac ESC de thoat.\n";

    GUIApp app(engine);
    app.run();

    std::cout << "[Info] Da thoat.\n";
    return 0;
}
