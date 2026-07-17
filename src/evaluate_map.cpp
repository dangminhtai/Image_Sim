/**
 * @file evaluate_map.cpp
 * @brief Kịch bản đánh giá Mean Average Precision (MAP) tự động.
 *
 * Đoạn mã này thực thi việc tính toán AP (Average Precision) cho 10 ảnh Test
 * đối chiếu với 99 ảnh trong CSDL thông qua 7 phương pháp truy vấn khác nhau.
 *
 * Tham khảo: 
 * - https://en.wikipedia.org/wiki/Evaluation_measures_(information_retrieval)#Mean_average_precision
 */
#include "ImageSearchEngine.h"
#include "ColorHistogram.h"
#include "ColorCorrelogram.h"
#include "SIFTExtractor.h"
#include "ORBExtractor.h"
#include "HOGExtractor.h"
#include "LBPExtractor.h"
#include "FusionExtractor.h"

#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <filesystem>
#include <fstream>
#include <algorithm>

namespace fs = std::filesystem;

static std::string getBasePath() {
    if (fs::exists("data")) return "";
    if (fs::exists("../data")) return "../";
    if (fs::exists("../../data")) return "../../";
    if (fs::exists("../../../data")) return "../../../";
    return "";
}

/**
 * @brief Khởi tạo và đăng ký tất cả các bộ trích xuất đặc trưng.
 * @param engine Tham chiếu đến engine tìm kiếm cần đăng ký.
 */
void setupExtractors(ImageSearchEngine& engine) {
    engine.addExtractor(std::make_shared<ColorHistogram>());
    engine.addExtractor(std::make_shared<ColorCorrelogram>());
    
    auto sift = std::make_shared<SIFTExtractor>();
    sift->loadVocabulary(getBasePath() + "data/vocab_CD/sift_vocab.yml");
    engine.addExtractor(sift);
    
    auto orb = std::make_shared<ORBExtractor>();
    orb->loadVocabulary(getBasePath() + "data/vocab_CD/orb_vocab.yml");
    engine.addExtractor(orb);
    
    engine.addExtractor(std::make_shared<HOGExtractor>());
    engine.addExtractor(std::make_shared<LBPExtractor>());
    
    auto colorHist = std::make_shared<ColorHistogram>();
    auto hog = std::make_shared<HOGExtractor>();
    engine.addExtractor(std::make_shared<FusionExtractor>(colorHist, hog, "Color_HOG"));
}

/**
 * @brief Lấy nhãn của ảnh Test từ tên file.
 * @param path Đường dẫn ảnh Test.
 * @return Chuỗi nhãn (VD: "01").
 */
std::string getTestLabel(const std::string& path) {
    std::string stem = fs::path(path).stem().string();
    size_t pos = stem.find('_');
    if (pos != std::string::npos) {
        return stem.substr(0, pos);
    }
    return stem;
}

/**
 * @brief Tính toán Average Precision (AP) cho một truy vấn.
 * @param results Danh sách kết quả trả về từ engine.
 * @param queryLabel Nhãn Ground Truth của ảnh truy vấn.
 * @param K Số lượng kết quả lấy ra để xét duyệt.
 * @param totalRelevant Tổng số ảnh có chung nhãn tồn tại trong CSDL.
 * @return Điểm AP (Average Precision) [0.0 - 1.0].
 */
float computeAP(const std::vector<SearchResult>& results, const std::string& queryLabel, int K, int totalRelevant = 3) {
    int hits = 0;
    float sumPrecisions = 0.0f;
    
    // Chỉ xét tới K kết quả
    int limit = std::min(K, static_cast<int>(results.size()));
    for (int i = 0; i < limit; ++i) {
        if (results[i].label == queryLabel) {
            hits++;
            sumPrecisions += static_cast<float>(hits) / (i + 1);
        }
    }
    
    if (hits == 0) return 0.0f;
    
    // Mẫu số: min(tổng số ảnh đúng trong CSDL, K)
    int denom = std::min(totalRelevant, limit);
    if (denom == 0) return 0.0f;
    
    return sumPrecisions / denom;
}

int main() {
    ImageSearchEngine engine;
    setupExtractors(engine);
    
    std::string base = getBasePath();
    std::string dbPath = base + "data/features_CD.yml";
    std::cout << "Loading Database...\n";
    if (!engine.loadDatabase(dbPath)) {
        std::cerr << "Failed to load DB: " << dbPath << "\n";
        return 1;
    }
    
    std::string testDir = base + "data/CD/TestImages";
    std::vector<std::string> testImages;
    for (const auto& entry : fs::directory_iterator(testDir)) {
        if (entry.is_regular_file()) {
            std::string ext = entry.path().extension().string();
            std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);
            if (ext == ".jpg" || ext == ".png") {
                testImages.push_back(entry.path().string());
            }
        }
    }
    std::sort(testImages.begin(), testImages.end());
    
    std::cout << "Found " << testImages.size() << " test images.\n\n";
    
    std::vector<std::string> methods = {
        "ColorHistogram", "ColorCorrelogram", "SIFT", "ORB", "HOG", "LBP", "Color_HOG"
    };
    std::vector<int> Ks = {3, 5, 11, 21};
    
    std::ofstream out("docs/evaluation_results.md");
    out << "# Báo cáo Đánh giá MAP (Mean Average Precision)\n\n";
    out << "Báo cáo dưới đây được tự động tạo bởi script `evaluate_map`. \n";
    out << "Độ đo MAP (càng cao càng tốt) được tính bằng cách trung bình Average Precision (AP) của " << testImages.size() << " ảnh Test.\n\n";
    
    out << "| Phương pháp | Độ đo khoảng cách | MAP@3 | MAP@5 | MAP@11 | MAP@21 |\n";
    out << "|---|---|---|---|---|---|\n";
    
    for (const auto& method : methods) {
        DistanceMetric metric = DistanceMetric::EUCLIDEAN;
        // Gán cứng logic chạy chuẩn nhất cho từng method
        if (method == "ColorHistogram" || method == "ColorCorrelogram") {
            metric = DistanceMetric::CHI_SQUARED; // Histogram tốt nhất với Chi-Sq
        } else if (method == "HOG" || method == "Color_HOG" || method == "SIFT") {
            metric = DistanceMetric::COSINE; // Đặc trưng vector L2-normalized tốt nhất với Cosine/L2
        } else if (method == "ORB") {
            metric = DistanceMetric::EUCLIDEAN; // ORB BoW dùng L2 cũng tốt
        } else if (method == "LBP") {
            metric = DistanceMetric::CHI_SQUARED; // LBP bản chất cũng là histogram
        }
        
        std::string metricName = (metric == DistanceMetric::EUCLIDEAN) ? "Euclidean" :
                                 (metric == DistanceMetric::COSINE) ? "Cosine" : "Chi-Squared";
        
        out << "| **" << method << "** | " << metricName << " |";
        std::cout << "Evaluating " << method << " (Metric: " << metricName << ")...\n";
        
        for (int K : Ks) {
            float totalAP = 0.0f;
            for (const auto& testPath : testImages) {
                cv::Mat img = cv::imread(testPath, cv::IMREAD_COLOR);
                std::string label = getTestLabel(testPath);
                
                double elapsed;
                // Truy vấn với số lượng K kết quả
                auto results = engine.search(img, method, metric, K, elapsed);
                float ap = computeAP(results, label, K, 3);
                totalAP += ap;
            }
            float map = totalAP / testImages.size();
            out << " " << std::fixed << std::setprecision(3) << map << " |";
        }
        out << "\n";
    }
    
    out.close();
    std::cout << "\nHoan thanh! Ket qua duoc luu vao docs/evaluation_results.md\n";
    return 0;
}
