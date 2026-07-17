# Báo cáo Đánh giá MAP (Mean Average Precision)

Báo cáo dưới đây được tự động tạo bởi script `evaluate_map`. 
Độ đo MAP (càng cao càng tốt) được tính bằng cách trung bình Average Precision (AP) của 10 ảnh Test.

| Phương pháp | Độ đo khoảng cách | MAP@3 | MAP@5 | MAP@11 | MAP@21 |
|---|---|---|---|---|---|
| **ColorHistogram** | Chi-Squared | 0.033 | 0.057 | 0.066 | 0.074 |
| **ColorCorrelogram** | Chi-Squared | 0.028 | 0.023 | 0.036 | 0.062 |
| **SIFT** | Cosine | 0.033 | 0.033 | 0.058 | 0.058 |
| **ORB** | Euclidean | 0.033 | 0.033 | 0.033 | 0.035 |
| **HOG** | Cosine | 0.011 | 0.011 | 0.020 | 0.031 |
| **LBP** | Chi-Squared | 0.033 | 0.048 | 0.059 | 0.067 |
| **Color_HOG** | Cosine | 0.028 | 0.043 | 0.046 | 0.054 |
