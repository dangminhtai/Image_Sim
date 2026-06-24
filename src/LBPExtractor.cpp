#include "LBPExtractor.h"
#include <opencv2/imgproc.hpp>

cv::Mat LBPExtractor::extract(const cv::Mat& image) const {
    if (image.empty()) return cv::Mat();

    // Tiền xử lý 1: Đổi sang ảnh xám
    cv::Mat gray;
    if (image.channels() == 3) {
        cv::cvtColor(image, gray, cv::COLOR_BGR2GRAY);
    } else {
        gray = image.clone();
    }

    // Tiền xử lý 2: Làm mịn (Gaussian Blur) để tránh nhiễu điểm ảnh làm sai lệch vân LBP
    cv::GaussianBlur(gray, gray, cv::Size(3, 3), 0);

    // Tính toán ảnh LBP
    cv::Mat lbp = cv::Mat::zeros(gray.rows, gray.cols, CV_8UC1);

    for (int i = 1; i < gray.rows - 1; i++) {
        for (int j = 1; j < gray.cols - 1; j++) {
            uchar center = gray.at<uchar>(i, j);
            uchar code = 0;
            code |= (gray.at<uchar>(i-1, j-1) > center) << 7;
            code |= (gray.at<uchar>(i-1, j)   > center) << 6;
            code |= (gray.at<uchar>(i-1, j+1) > center) << 5;
            code |= (gray.at<uchar>(i, j+1)   > center) << 4;
            code |= (gray.at<uchar>(i+1, j+1) > center) << 3;
            code |= (gray.at<uchar>(i+1, j)   > center) << 2;
            code |= (gray.at<uchar>(i+1, j-1) > center) << 1;
            code |= (gray.at<uchar>(i, j-1)   > center) << 0;
            lbp.at<uchar>(i, j) = code;
        }
    }

    // Tính histogram của ảnh LBP (256 bins vì LBP code có 8 bit = 0-255)
    int histSize = 256;
    float range[] = {0, 256};
    const float* histRange = {range};
    cv::Mat hist;
    
    cv::calcHist(&lbp, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, true, false);

    // Chuyển sang Float và chuẩn hóa L2
    cv::Mat histFloat;
    hist.convertTo(histFloat, CV_32F);
    cv::normalize(histFloat, histFloat, 1.0, 0.0, cv::NORM_L2);
    
    return histFloat.reshape(1, 1);
}
