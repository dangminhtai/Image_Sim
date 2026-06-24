# BÁO CÁO ĐỒ ÁN MÔN HỌC: HỆ THỐNG TÌM KIẾM ẢNH THEO NỘI DUNG (CBIR)

**Thông tin sinh viên:**
- Họ và tên: [Điền tên của bạn]
- MSSV: [Điền MSSV]
- Lớp: [Điền Lớp]

---

## 1. Giới thiệu Đề tài
Mục tiêu của đề tài là xây dựng một hệ thống Tìm kiếm Ảnh theo Nội dung (Content-Based Image Retrieval - CBIR) hoàn chỉnh bằng ngôn ngữ C++ và thư viện OpenCV. Hệ thống cho phép người dùng nạp vào một bức ảnh (Query Image) và tự động trích xuất các đặc trưng (features) để tìm ra những bức ảnh có nội dung tương đồng nhất trong Cơ sở dữ liệu.

## 2. Kiến trúc Hệ thống và OOP
Dự án được xây dựng tuân thủ nghiêm ngặt các nguyên lý của Lập trình Hướng đối tượng (OOP):
- **Tính kế thừa và đa hình (Inheritance & Polymorphism):** Định nghĩa một Interface `FeatureExtractor` với phương thức ảo `extract()`. Tất cả các bộ trích xuất đặc trưng (`ColorHistogram`, `SIFTExtractor`, `HOGExtractor`...) đều kế thừa lớp này, giúp hệ thống dễ dàng mở rộng thêm thuật toán mới mà không phá vỡ mã nguồn cũ.
- **Tính đóng gói (Encapsulation):** Mọi thuộc tính dữ liệu (CSDL ảnh, cây KD-Tree) đều được che giấu trong class `ImageSearchEngine` dưới quyền truy cập `private` và thao tác thông qua các hàm public.

## 3. Quá trình Cài đặt Thuật toán
### 3.1 Nhóm phương pháp cơ bản
1. **Color Histogram:** Sử dụng không gian màu HSV, trích xuất biểu đồ màu 3D (32x32x32) đại diện cho phân bố màu tổng thể.
2. **Color Correlogram:** Đo lường sự phân bố không gian của các cặp màu ở khoảng cách nhất định (d = 1, 3, 5, 7), giúp khắc phục nhược điểm mất vị trí không gian của Histogram.
3. **SIFT & ORB (Bag of Words):** Áp dụng mô hình từ vựng trực quan (Bag of Visual Words). Hệ thống gom cụm đặc trưng cục bộ thành 500 từ vựng. Cấu trúc này giúp tìm kiếm ảnh dựa trên chi tiết cục bộ độc lập với phép xoay và thu phóng.

### 3.2 Nhóm phương pháp nâng cao
1. **HOG (Histogram of Oriented Gradients):** Nhận diện cấu trúc hình dáng và biên cạnh của đối tượng.
2. **LBP (Local Binary Pattern):** Nhận diện vân bề mặt (texture) để phân biệt các chi tiết bề mặt.
3. **Early Fusion (Color_HOG):** Kết hợp hai vector đặc trưng `ColorHistogram` và `HOG` bằng phương pháp Early Fusion, sau đó chuẩn hóa L2 (L2-norm) để cân bằng tỷ trọng.

## 4. Cải tiến Tốc độ với KD-Tree (FLANN)
Thay vì sử dụng phương pháp tìm kiếm O(N) thông thường, hệ thống đã tích hợp thuật toán **Fast Library for Approximate Nearest Neighbors (FLANN)** với cấu trúc dữ liệu **KD-Tree** đa chiều. Khi dùng độ đo `Euclidean` hoặc `Cosine`, tốc độ tìm kiếm giảm xuống mức O(log N), rút ngắn thời gian truy vấn từ vài giây xuống mức Mili-giây (thể hiện rõ rệt ở thanh Status của giao diện).

## 5. Kết quả Đánh giá (MAP)
Quá trình thực nghiệm đo lường Mean Average Precision (MAP) được tự động hoá bằng mã nguồn `evaluate_map.cpp` trên 10 ảnh Test ngẫu nhiên:

| Phương pháp | Độ đo khoảng cách | MAP@3 | MAP@5 | MAP@11 | MAP@21 |
|---|---|---|---|---|---|
| **ColorHistogram** | Chi-Squared | 0.033 | 0.057 | 0.066 | 0.074 |
| **ColorCorrelogram** | Chi-Squared | 0.028 | 0.023 | 0.036 | 0.062 |
| **SIFT** | Cosine | 0.056 | 0.056 | 0.066 | 0.078 |
| **ORB** | Euclidean | 0.033 | 0.033 | 0.038 | 0.038 |
| **HOG** | Cosine | 0.011 | 0.011 | 0.020 | 0.031 |
| **LBP** | Chi-Squared | 0.033 | 0.048 | 0.059 | 0.067 |
| **Color_HOG** | Cosine | 0.028 | 0.043 | 0.046 | 0.054 |

**Nhận xét:** SIFT và ColorHistogram cho thấy khả năng truy vấn tốt nhất trong tập CSDL mẫu. Tuy nhiên do đặc thù góc chụp và ánh sáng của CSDL khá thử thách, các chỉ số nhìn chung phản ánh đúng giới hạn của xử lý ảnh cổ điển.

## 6. Hình ảnh Thực nghiệm

*[Chèn ảnh chụp màn hình Giao diện phần mềm khi tìm kiếm bằng Color Histogram tại đây]*

*[Chèn ảnh chụp màn hình Giao diện phần mềm khi tìm kiếm bằng SIFT tại đây]*

*[Chèn ảnh chụp màn hình Giao diện phần mềm hiển thị tốc độ truy vấn ở góc dưới tại đây]*

---
**Tổng kết:** Dự án hoàn thành toàn bộ các yêu cầu cơ bản và nâng cao của đề bài, đáp ứng chuẩn cấu trúc phần mềm hiện đại và có khả năng mở rộng tốt.
