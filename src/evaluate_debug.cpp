#include "ImageSearchEngine.h"
#include "SIFTExtractor.h"
#include <opencv2/imgcodecs.hpp>
#include <iostream>
#include <filesystem>

int main(int argc, char** argv) {
    if (argc < 2) return 1;
    ImageSearchEngine engine;
    auto sift = std::make_shared<SIFTExtractor>();
    sift->loadVocabulary("data/vocab_CD/sift_vocab.yml");
    engine.addExtractor(sift);
    engine.loadDatabase("data/features_CD.yml");
    cv::Mat img = cv::imread(argv[1], cv::IMREAD_COLOR);
    double elapsed;
    auto res = engine.search(img, "SIFT", DistanceMetric::COSINE, 5, elapsed);
    for (int i=0; i<res.size(); i++) {
        std::cout << res[i].label << " ";
    }
    std::cout << "\n";
    return 0;
}
