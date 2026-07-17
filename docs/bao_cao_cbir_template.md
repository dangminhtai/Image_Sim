# BÁO CÁO ĐỒ ÁN MÔN HỌC  
# HỆ THỐNG TRUY VẤN ẢNH DỰA VÀO NỘI DUNG THỊ GIÁC

---

## THÔNG TIN SINH VIÊN

| Nội dung | Thông tin |
|---|---|
| Họ và tên | Nguyễn Văn Vũ |
| MSSV | 23127523 |
| Lớp | 23TGMT |
| Email | nvvu23@clc.fitus.edu.vn |
| Số điện thoại | 0905450412|
| Môn học | CSC16105 – Truy vấn thông tin thị giác |
| Giảng viên | Võ Hoài Việt |

---

## LỜI CAM ĐOAN

Tôi cam đoan mã nguồn và nội dung báo cáo này do tôi tự thực hiện. Các tài liệu, thuật toán và đoạn mã được tham khảo từ nguồn bên ngoài đều được ghi rõ trong phần tài liệu tham khảo hoặc chú thích tại mã nguồn.

---

## MỤC LỤC

> Khi chuyển sang Word/PDF, tạo mục lục tự động từ các tiêu đề trong báo cáo.

---

# 1. GIỚI THIỆU

## 1.1. Bối cảnh

Truy vấn ảnh dựa vào nội dung thị giác, hay Content-Based Image Retrieval (CBIR), là bài toán tìm kiếm các ảnh trong cơ sở dữ liệu có nội dung tương tự với một ảnh truy vấn. Thay vì dựa trên tên tệp hoặc mô tả văn bản, hệ thống sử dụng các đặc trưng được trích xuất trực tiếp từ ảnh như màu sắc, hình dạng, biên cạnh, kết cấu và đặc trưng cục bộ.

## 1.2. Mục tiêu đề tài

- Xây dựng chương trình truy vấn ảnh có giao diện.
- Trích xuất và so sánh nhiều loại đặc trưng ảnh.
- Hiển thị các ảnh kết quả theo thứ tự tương đồng giảm dần.
- Đánh giá độ chính xác bằng AP và MAP.
- Đánh giá thời gian truy vấn.
- Áp dụng lập trình hướng đối tượng và thư viện OpenCV.

## 1.3. Phạm vi thực hiện

- Ngôn ngữ lập trình: C/C++.
- Thư viện chính: OpenCV.
- Cơ sở dữ liệu ảnh: [TMBuD/CD/Tên tập dữ liệu thực tế].
- Các phương pháp cơ bản:
  - Color Histogram.
  - Color Correlogram.
  - SIFT.
  - ORB.
- Các phương pháp mở rộng:
  - HOG.
  - LBP.
  - Color + HOG.
  - [Phương pháp khác nếu có].

## 1.4. Kết quả đạt được

Tóm tắt ngắn gọn những chức năng đã hoàn thành, phương pháp tốt nhất và thời gian truy vấn trung bình.

---

# 2. CƠ SỞ LÝ THUYẾT

## 2.1. Hệ thống truy vấn ảnh dựa vào nội dung

Một hệ thống CBIR gồm hai giai đoạn chính:

### 2.1.1. Giai đoạn xây dựng cơ sở dữ liệu đặc trưng

1. Đọc từng ảnh trong cơ sở dữ liệu.
2. Tiền xử lý ảnh.
3. Trích xuất vector đặc trưng.
4. Lưu vector đặc trưng cùng đường dẫn hoặc nhãn ảnh.

### 2.1.2. Giai đoạn truy vấn

1. Người dùng chọn ảnh truy vấn.
2. Hệ thống trích xuất đặc trưng của ảnh truy vấn.
3. Tính độ khác biệt hoặc độ tương đồng với các ảnh trong cơ sở dữ liệu.
4. Sắp xếp kết quả.
5. Trả về Top-K ảnh gần nhất.

![Quy trình tổng quát của hệ thống](images/system_pipeline.png) (sài mermaid cũng dc)

**Hình 2.1.** Quy trình tổng quát của hệ thống CBIR.

## 2.2. Color Histogram

Trình bày:

- Không gian màu sử dụng: [RGB/HSV/Lab].
- Số lượng bin: [Điền].
- Cách chuẩn hóa histogram.
- Ưu điểm và hạn chế.

Công thức histogram chuẩn hóa:

\[
\hat{h}_i=\frac{h_i}{\sum_{j=1}^{B}h_j}
\]

Trong đó:

- \(h_i\): số pixel thuộc bin thứ \(i\).
- \(B\): tổng số bin.
- \(\hat{h}_i\): giá trị histogram sau chuẩn hóa.

## 2.3. Color Correlogram

Trình bày:

- Ý nghĩa của correlogram.
- Số mức lượng tử hóa màu.
- Khoảng cách không gian sử dụng.
- Kích thước vector đặc trưng.
- Ưu điểm so với Color Histogram.

## 2.4. SIFT

Trình bày:

- Phát hiện keypoint.
- Mô tả đặc trưng cục bộ.
- Số chiều descriptor.
- Cách chuyển tập descriptor thành vector ảnh nếu có sử dụng Bag of Visual Words.
- Ưu điểm và hạn chế.

## 2.5. ORB

Trình bày:

- FAST dùng để phát hiện keypoint.
- BRIEF có định hướng dùng để tạo descriptor.
- Descriptor nhị phân.
- Ưu điểm về tốc độ.
- Hạn chế so với SIFT.

## 2.6. HOG

Trình bày:

- Tính gradient.
- Chia ảnh thành cell và block.
- Lập histogram hướng gradient.
- Chuẩn hóa block.
- Tham số sử dụng trong chương trình.

## 2.7. LBP

Trình bày:

- So sánh pixel trung tâm với các pixel lân cận.
- Tạo mã nhị phân cục bộ.
- Xây dựng histogram LBP.
- Ý nghĩa trong mô tả kết cấu.

Công thức LBP:

\[
LBP_{P,R}=\sum_{p=0}^{P-1}s(g_p-g_c)2^p
\]

với:

\[
s(x)=
\begin{cases}
1, & x\geq 0\\
0, & x<0
\end{cases}
\]

## 2.8. Kết hợp đặc trưng Color + HOG

Trình bày chính xác cách kết hợp đã cài đặt:

\[
f_{\text{fusion}}=[\alpha f_{\text{color}},\ \beta f_{\text{HOG}}]
\]

Trong đó:

- \(f_{\text{color}}\): vector đặc trưng màu.
- \(f_{\text{HOG}}\): vector HOG.
- \(\alpha,\beta\): trọng số nếu có.
- Nêu rõ phương pháp chuẩn hóa trước hoặc sau khi ghép vector.

## 2.9. Các độ đo so sánh

### 2.9.1. Khoảng cách Euclidean

\[
d(\mathbf{x},\mathbf{y})
=\sqrt{\sum_{i=1}^{n}(x_i-y_i)^2}
\]

Khoảng cách càng nhỏ thì hai ảnh càng tương đồng.

### 2.9.2. Cosine Similarity

\[
\operatorname{cos}(\mathbf{x},\mathbf{y})
=\frac{\mathbf{x}\cdot\mathbf{y}}
{\|\mathbf{x}\|_2\|\mathbf{y}\|_2}
\]

Nếu chương trình sử dụng Cosine Distance:

\[
d_{\text{cosine}}=1-\operatorname{cos}(\mathbf{x},\mathbf{y})
\]

### 2.9.3. Chi-Squared Distance

\[
d_{\chi^2}(\mathbf{x},\mathbf{y})
=\frac{1}{2}\sum_{i=1}^{n}
\frac{(x_i-y_i)^2}{x_i+y_i+\varepsilon}
\]

Trong đó \(\varepsilon\) dùng để tránh chia cho 0.

## 2.10. Average Precision và Mean Average Precision

Với kết quả Top-K, Precision tại vị trí \(k\):

\[
P(k)=\frac{\text{số ảnh liên quan trong }k\text{ kết quả đầu}}
{k}
\]

Average Precision tại Top-K:

\[
AP@K=
\frac{1}{\min(R,K)}
\sum_{k=1}^{K}P(k)\cdot rel(k)
\]

Trong đó:

- \(rel(k)=1\) nếu ảnh tại vị trí \(k\) liên quan đến ảnh truy vấn.
- \(rel(k)=0\) nếu ảnh không liên quan.
- \(R\): tổng số ảnh liên quan của truy vấn trong cơ sở dữ liệu.

Mean Average Precision:

\[
MAP@K=\frac{1}{Q}\sum_{q=1}^{Q}AP_q@K
\]

Trong đó \(Q\) là số ảnh truy vấn được dùng để đánh giá.

> Cần ghi rõ cách xác định hai ảnh “liên quan”, cách xử lý ảnh truy vấn nếu ảnh đó nằm trong cơ sở dữ liệu, và mẫu số AP thực tế được sử dụng trong mã nguồn.

---

# 3. PHÂN TÍCH VÀ THIẾT KẾ HỆ THỐNG

## 3.1. Yêu cầu chức năng

| Mã | Chức năng | Mô tả |
|---|---|---|
| F01 | Chọn ảnh truy vấn | Cho phép người dùng chọn một ảnh từ máy |
| F02 | Chọn phương pháp | Chọn bộ trích xuất đặc trưng |
| F03 | Chọn số lượng kết quả | Chọn Top-3, Top-5, Top-11 hoặc Top-21 |
| F04 | Thực hiện truy vấn | Tìm và xếp hạng các ảnh tương tự |
| F05 | Hiển thị kết quả | Hiển thị ảnh, thứ hạng và giá trị so khớp |
| F06 | Hiển thị thời gian | Hiển thị thời gian truy vấn |
| F07 | Đánh giá MAP | Tính MAP@3, MAP@5, MAP@11 và MAP@21 |

## 3.2. Yêu cầu phi chức năng

- Mã nguồn rõ ràng, có chú thích.
- Tên class, hàm và biến có ý nghĩa.
- Cấu trúc hướng đối tượng.
- Không hard-code đường dẫn không cần thiết.
- Có xử lý lỗi khi ảnh không đọc được.
- Kết quả có thể tái lập khi chạy cùng cấu hình.

## 3.3. Kiến trúc hệ thống

![Kiến trúc hệ thống](images/system_architecture.png)

**Hình 3.1.** Kiến trúc tổng quát của hệ thống.

Mô tả các thành phần:

| Thành phần | Trách nhiệm |
|---|---|
| Giao diện | Nhận thao tác và hiển thị kết quả |
| Feature Extractor | Trích xuất vector đặc trưng |
| Distance Calculator | Tính khoảng cách hoặc độ tương đồng |
| Search Engine | Xếp hạng và trả về Top-K |
| Feature Database | Lưu đặc trưng và thông tin ảnh |
| Evaluation Module | Tính AP, MAP và thời gian truy vấn |

## 3.4. Thiết kế hướng đối tượng

### 3.4.1. Sơ đồ lớp

![Sơ đồ lớp](images/class_diagram.png)

**Hình 3.2.** Sơ đồ lớp của chương trình.

### 3.4.2. Danh sách lớp chính

| Lớp | Trách nhiệm | Các hàm chính |
|---|---|---|
| `FeatureExtractor` | Giao diện chung cho các bộ trích xuất | `extract()` |
| `ColorHistogramExtractor` | Trích xuất histogram màu | `[Điền]` |
| `ColorCorrelogramExtractor` | Trích xuất correlogram | `[Điền]` |
| `SIFTExtractor` | Trích xuất đặc trưng SIFT | `[Điền]` |
| `ORBExtractor` | Trích xuất đặc trưng ORB | `[Điền]` |
| `HOGExtractor` | Trích xuất HOG | `[Điền]` |
| `LBPExtractor` | Trích xuất LBP | `[Điền]` |
| `ImageSearchEngine` | Thực hiện tìm kiếm và xếp hạng | `[Điền]` |
| `MapEvaluator` | Tính AP và MAP | `[Điền]` |
| `[Tên lớp giao diện]` | Điều khiển giao diện | `[Điền]` |

> Chỉ giữ những lớp thực sự tồn tại trong mã nguồn. Không mô tả kiến trúc chưa cài đặt.

## 3.5. Cấu trúc thư mục dự án

```text
<MSSV>/
├── Source/
│   ├── include/
│   ├── src/
│   ├── CMakeLists.txt
│   └── ...
├── Release/
│   ├── <chuong_trinh>.exe
│   └── ...
├── Docs/
│   ├── report.md
│   ├── report.pdf
│   └── images/
└── README.md
```

---

# 4. CÀI ĐẶT HỆ THỐNG

## 4.1. Môi trường phát triển

| Thành phần | Phiên bản |
|---|---|
| Hệ điều hành | [Điền] |
| Trình biên dịch | [Điền] |
| CMake | [Điền] |
| OpenCV | [Điền] |
| IDE | [Điền] |
| Ngôn ngữ | C++ [phiên bản] |

## 4.2. Tiền xử lý dữ liệu

Mô tả:

- Cách đọc ảnh.
- Cách thay đổi kích thước.
- Chuyển đổi không gian màu.
- Chuẩn hóa dữ liệu.
- Xử lý ảnh lỗi hoặc ảnh không hợp lệ.

## 4.3. Xây dựng cơ sở dữ liệu đặc trưng

Mô tả quy trình:

1. Duyệt toàn bộ ảnh.
2. Trích xuất đặc trưng.
3. Lưu đường dẫn ảnh và vector đặc trưng.
4. Lưu từ điển Bag of Visual Words nếu có.
5. Tái sử dụng đặc trưng đã lưu khi chương trình khởi động lại.

## 4.4. Quy trình truy vấn Top-K

```text
Ảnh truy vấn
    ↓
Tiền xử lý
    ↓
Trích xuất đặc trưng
    ↓
Tính khoảng cách với CSDL đặc trưng
    ↓
Sắp xếp tăng/giảm theo loại độ đo
    ↓
Loại ảnh truy vấn nếu cần
    ↓
Trả về Top-K kết quả
```

## 4.5. Cải tiến tốc độ

Trình bày đúng phương pháp thực tế đã cài đặt:

- Tiền tính đặc trưng.
- Lưu đặc trưng ra tệp.
- FLANN/KD-Tree/LSH nếu có.
- Đa luồng nếu có.
- Giảm chiều nếu có.
- Cache dữ liệu nếu có.

> Không ghi độ phức tạp hoặc mức tăng tốc nếu chưa được chứng minh bằng thực nghiệm.

## 4.6. Xử lý ngoại lệ

| Trường hợp | Cách xử lý |
|---|---|
| Không chọn ảnh truy vấn | [Điền] |
| Ảnh không đọc được | [Điền] |
| CSDL đặc trưng chưa tồn tại | [Điền] |
| Vector đặc trưng rỗng | [Điền] |
| Số lượng ảnh nhỏ hơn K | [Điền] |
| Sai kích thước vector | [Điền] |

---

# 5. HƯỚNG DẪN SỬ DỤNG

## 5.1. Khởi động chương trình

1. Mở thư mục `Release`.
2. Chạy tệp `[Tên chương trình].exe`.
3. Chọn thư mục cơ sở dữ liệu nếu chương trình yêu cầu.
4. Chờ hệ thống tải cơ sở dữ liệu đặc trưng.

## 5.2. Thực hiện truy vấn

1. Nhấn nút **Chọn ảnh**.
2. Chọn phương pháp trích xuất đặc trưng.
3. Chọn số lượng kết quả Top-K.
4. Nhấn nút **Truy vấn**.
5. Quan sát kết quả và thời gian truy vấn.

![Giao diện chính](images/main_ui.png)

**Hình 5.1.** Giao diện chính của chương trình.

## 5.3. Ý nghĩa thông tin trên giao diện

| Thành phần | Ý nghĩa |
|---|---|
| Ảnh truy vấn | Ảnh người dùng cần tìm |
| Tên phương pháp | Bộ đặc trưng đang được sử dụng |
| Giá trị so khớp | Khoảng cách hoặc độ tương đồng |
| Thứ hạng | Vị trí của ảnh trong danh sách kết quả |
| Thời gian truy vấn | Thời gian xử lý một truy vấn |
| Top-K | Số lượng ảnh được trả về |

---

# 6. THỰC NGHIỆM VÀ ĐÁNH GIÁ

## 6.1. Cấu hình thực nghiệm

| Thuộc tính | Giá trị |
|---|---|
| Tên tập dữ liệu | [Điền] |
| Tổng số ảnh | [Điền] |
| Số lớp | [Điền] |
| Số ảnh mỗi lớp | [Điền hoặc mô tả phân bố] |
| Số ảnh truy vấn | [Điền] |
| Giá trị K | 3, 5, 11, 21 |
| Phần cứng | [CPU, RAM, GPU nếu có] |
| Cách chọn ảnh truy vấn | [Ngẫu nhiên/cố định/toàn bộ tập test] |
| Cách xác định ảnh liên quan | [Cùng lớp/cùng thư mục/ground truth] |

## 6.2. Bộ tham số của các phương pháp

| Phương pháp | Tham số chính | Độ đo |
|---|---|---|
| Color Histogram | [Không gian màu, số bin] | Chi-Squared |
| Color Correlogram | [Số màu, khoảng cách] | Chi-Squared |
| SIFT | [Số keypoint, BoVW nếu có] | Cosine |
| ORB | [Số keypoint, BoVW nếu có] | Euclidean |
| HOG | [Cell, block, số bin] | Cosine |
| LBP | [P, R, kiểu histogram] | Chi-Squared |
| Color + HOG | [Cách chuẩn hóa, trọng số] | Cosine |

## 6.3. Kết quả MAP

| Phương pháp | Độ đo khoảng cách | MAP@3 | MAP@5 | MAP@11 | MAP@21 |
|---|---|---:|---:|---:|---:|
| Color Histogram | Chi-Squared | 0.033 | 0.057 | 0.066 | 0.074 |
| Color Correlogram | Chi-Squared | 0.028 | 0.023 | 0.036 | 0.062 |
| SIFT | Cosine | **0.056** | **0.056** | **0.066** | **0.078** |
| ORB | Euclidean | 0.033 | 0.033 | 0.038 | 0.038 |
| HOG | Cosine | 0.011 | 0.011 | 0.020 | 0.031 |
| LBP | Chi-Squared | 0.033 | 0.048 | 0.059 | 0.067 |
| Color + HOG | Cosine | 0.028 | 0.043 | 0.046 | 0.054 |

> Thay bảng trên nếu kết quả cuối cùng thay đổi sau khi kiểm tra lại mã đánh giá.

## 6.4. Phân tích kết quả MAP

Phân tích tối thiểu:

- Phương pháp tốt nhất tại từng giá trị K.
- Mức chênh lệch giữa các phương pháp.
- Lý do đặc trưng màu hoạt động tốt hoặc kém.
- Lý do SIFT/ORB hoạt động tốt hoặc kém.
- Tại sao HOG hoặc LBP phù hợp với một số nhóm ảnh.
- Tại sao phương pháp kết hợp chưa chắc tốt hơn từng đặc trưng riêng lẻ.
- Ảnh hưởng của số lượng ảnh truy vấn và phân bố lớp.
- Các giới hạn của thí nghiệm.

Ví dụ nhận xét:

SIFT đạt MAP@21 cao nhất với giá trị 0.078, trong khi Color Histogram đứng thứ hai với 0.074. Điều này cho thấy đặc trưng cục bộ của SIFT và thông tin phân bố màu đều có khả năng phân biệt tương đối tốt trên tập dữ liệu đang xét. HOG có kết quả thấp nhất, có thể do hình dạng và hướng biên giữa các lớp chưa đủ khác biệt hoặc ảnh có nhiều biến đổi về góc chụp và nền.

## 6.5. Kết quả thời gian truy vấn

| Phương pháp | Thời gian trung bình (ms) | Nhỏ nhất (ms) | Lớn nhất (ms) |
|---|---:|---:|---:|
| Color Histogram | [Điền] | [Điền] | [Điền] |
| Color Correlogram | [Điền] | [Điền] | [Điền] |
| SIFT | [Điền] | [Điền] | [Điền] |
| ORB | [Điền] | [Điền] | [Điền] |
| HOG | [Điền] | [Điền] | [Điền] |
| LBP | [Điền] | [Điền] | [Điền] |
| Color + HOG | [Điền] | [Điền] | [Điền] |

Nêu rõ thời gian trên bao gồm:

- Chỉ thời gian tìm kiếm; hoặc
- Cả trích xuất đặc trưng truy vấn và tìm kiếm.

## 6.6. Minh họa kết quả thực nghiệm

Theo yêu cầu, mỗi chức năng cần có tối thiểu ba kết quả minh họa.

### 6.6.1. Color Histogram

![Kết quả Color Histogram 1](images/color_histogram_01.png)

**Hình 6.1.** Kết quả truy vấn Color Histogram – trường hợp 1.

![Kết quả Color Histogram 2](images/color_histogram_02.png)

**Hình 6.2.** Kết quả truy vấn Color Histogram – trường hợp 2.

![Kết quả Color Histogram 3](images/color_histogram_03.png)

**Hình 6.3.** Kết quả truy vấn Color Histogram – trường hợp 3.

### 6.6.2. Color Correlogram

![Kết quả Color Correlogram 1](images/color_correlogram_01.png)

![Kết quả Color Correlogram 2](images/color_correlogram_02.png)

![Kết quả Color Correlogram 3](images/color_correlogram_03.png)

### 6.6.3. SIFT

![Kết quả SIFT 1](images/sift_01.png)

![Kết quả SIFT 2](images/sift_02.png)

![Kết quả SIFT 3](images/sift_03.png)

### 6.6.4. ORB

![Kết quả ORB 1](images/orb_01.png)

![Kết quả ORB 2](images/orb_02.png)

![Kết quả ORB 3](images/orb_03.png)

### 6.6.5. HOG

![Kết quả HOG 1](images/hog_01.png)

![Kết quả HOG 2](images/hog_02.png)

![Kết quả HOG 3](images/hog_03.png)

### 6.6.6. LBP

![Kết quả LBP 1](images/lbp_01.png)

![Kết quả LBP 2](images/lbp_02.png)

![Kết quả LBP 3](images/lbp_03.png)

### 6.6.7. Color + HOG

![Kết quả Color + HOG 1](images/color_hog_01.png)

![Kết quả Color + HOG 2](images/color_hog_02.png)

![Kết quả Color + HOG 3](images/color_hog_03.png)

## 6.7. So sánh tổng hợp

| Tiêu chí | Phương pháp tốt nhất | Nhận xét |
|---|---|---|
| MAP@3 | [Điền] | [Điền] |
| MAP@5 | [Điền] | [Điền] |
| MAP@11 | [Điền] | [Điền] |
| MAP@21 | [Điền] | [Điền] |
| Tốc độ truy vấn | [Điền] | [Điền] |
| Đặc trưng màu | [Điền] | [Điền] |
| Đặc trưng hình dạng | [Điền] | [Điền] |
| Đặc trưng kết cấu | [Điền] | [Điền] |

---

# 7. MỨC ĐỘ HOÀN THÀNH YÊU CẦU

| STT | Yêu cầu | Tỷ trọng | Mức độ hoàn thành | Minh chứng |
|---:|---|---:|---:|---|
| 1 | Truy vấn bằng Color Histogram, Color Correlogram, SIFT và ORB; đánh giá MAP@3, @5, @11, @21 | 50% | [100%/50%/Để trống] | [Mục, hình hoặc file minh chứng] |
| 2 | Cài đặt đặc trưng nâng cao hoặc kết hợp đặc trưng; so sánh kết quả MAP | 30% | [100%/50%/Để trống] | [Mục, hình hoặc file minh chứng] |
| 3 | Cải tiến tốc độ tìm kiếm; hiển thị thời gian truy vấn trên giao diện | 20% | [100%/50%/Để trống] | [Mục, hình hoặc file minh chứng] |

> Chỉ ghi `100%` nếu yêu cầu đã hoàn thành đầy đủ; ghi `50%` nếu mức hoàn thành lớn hơn 50% nhưng chưa đầy đủ; dưới 50% thì để trống theo quy định của đề bài.

---

# 8. KIỂM THỬ CHƯƠNG TRÌNH

## 8.1. Danh sách ca kiểm thử

| Mã | Trường hợp kiểm thử | Kết quả mong đợi | Kết quả thực tế | Trạng thái |
|---|---|---|---|---|
| TC01 | Chọn ảnh hợp lệ | Hiển thị ảnh truy vấn | [Điền] | Đạt/Không đạt |
| TC02 | Không chọn ảnh | Hiển thị thông báo lỗi | [Điền] | Đạt/Không đạt |
| TC03 | Truy vấn Top-3 | Trả đúng 3 ảnh | [Điền] | Đạt/Không đạt |
| TC04 | Truy vấn Top-21 | Trả tối đa 21 ảnh | [Điền] | Đạt/Không đạt |
| TC05 | Đổi phương pháp | Kết quả được tính lại | [Điền] | Đạt/Không đạt |
| TC06 | Ảnh hỏng | Không làm chương trình bị dừng | [Điền] | Đạt/Không đạt |
| TC07 | CSDL nhỏ hơn K | Trả số ảnh hiện có | [Điền] | Đạt/Không đạt |
| TC08 | Chạy đánh giá MAP | Sinh đúng bảng kết quả | [Điền] | Đạt/Không đạt |

## 8.2. Kiểm tra công thức AP/MAP

Ghi lại một ví dụ tính thủ công cho một ảnh truy vấn để đối chiếu với chương trình.

Ví dụ, danh sách liên quan trong Top-5:

```text
[1, 0, 1, 0, 1]
```

Khi đó:

\[
P(1)=1,\quad P(3)=\frac{2}{3},\quad P(5)=\frac{3}{5}
\]

\[
AP@5=
\frac{1+\frac{2}{3}+\frac{3}{5}}
{\min(R,5)}
\]

Thay \(R\) bằng số ảnh liên quan thực tế và đối chiếu với output chương trình.

---

# 9. HẠN CHẾ VÀ HƯỚNG PHÁT TRIỂN

## 9.1. Hạn chế

- Đặc trưng cổ điển nhạy với ánh sáng, góc chụp hoặc nền ảnh.
- Số lượng ảnh đánh giá còn hạn chế.
- Cách kết hợp đặc trưng chưa được tối ưu trọng số.
- Tốc độ phụ thuộc kích thước cơ sở dữ liệu.
- [Hạn chế thực tế khác].

## 9.2. Hướng phát triển

- Tối ưu tham số bằng tập validation.
- Áp dụng PCA để giảm chiều.
- Dùng FLANN hoặc chỉ mục tìm kiếm gần đúng.
- Kết hợp nhiều đặc trưng với trọng số học được.
- Sử dụng đặc trưng học sâu từ CNN.
- Mở rộng đánh giá trên toàn bộ tập dữ liệu.
- Bổ sung Precision–Recall và biểu đồ thời gian.

---

# 10. KẾT LUẬN

Tóm tắt:

- Những chức năng đã hoàn thành.
- Phương pháp đạt kết quả tốt nhất.
- Kết quả MAP nổi bật.
- Kết quả cải tiến tốc độ.
- Mức độ đáp ứng yêu cầu đề bài.
- Bài học rút ra trong quá trình thực hiện.

---

# TÀI LIỆU THAM KHẢO

[1] C. D. Manning, P. Raghavan và H. Schütze, *Introduction to Information Retrieval*, Cambridge University Press, 2008.

[2] OpenCV, *OpenCV Documentation*, phiên bản [Điền phiên bản], truy cập ngày [dd/mm/yyyy].

[3] R. Datta, D. Joshi, J. Li và J. Z. Wang, “Image Retrieval: Ideas, Influences, and Trends of the New Age,” *ACM Computing Surveys*, vol. 40, no. 2, 2008.

[4] D. G. Lowe, “Distinctive Image Features from Scale-Invariant Keypoints,” *International Journal of Computer Vision*, vol. 60, no. 2, pp. 91–110, 2004.

[5] E. Rublee, V. Rabaud, K. Konolige và G. Bradski, “ORB: An Efficient Alternative to SIFT or SURF,” *Proceedings of ICCV*, 2011.

[6] [Bổ sung nguồn Color Histogram, Color Correlogram, HOG, LBP và mã nguồn tham khảo thực tế].

---

# PHỤ LỤC A. HƯỚNG DẪN BIÊN DỊCH

```bash
# Ví dụ với CMake
cmake -S . -B build
cmake --build build --config Release
```

Điều chỉnh lệnh theo cấu trúc thực tế của dự án.

---

# PHỤ LỤC B. THAM SỐ CẤU HÌNH

| Tham số | Giá trị | Ý nghĩa |
|---|---|---|
| `TOP_K` | [Điền] | Số ảnh trả về |
| `HIST_BINS` | [Điền] | Số bin histogram |
| `CORRELOGRAM_DISTANCE` | [Điền] | Khoảng cách correlogram |
| `SIFT_FEATURES` | [Điền] | Số keypoint SIFT |
| `ORB_FEATURES` | [Điền] | Số keypoint ORB |
| `HOG_CELL_SIZE` | [Điền] | Kích thước cell HOG |
| `LBP_RADIUS` | [Điền] | Bán kính LBP |
| `LBP_POINTS` | [Điền] | Số điểm lân cận |

---

# PHỤ LỤC C. DANH SÁCH MÃ NGUỒN THAM KHẢO

| STT | Tệp/đoạn mã | Nội dung tham khảo | Nguồn | Phần đã chỉnh sửa |
|---:|---|---|---|---|
| 1 | [Điền] | [Điền] | [URL/tài liệu] | [Điền] |
| 2 | [Điền] | [Điền] | [URL/tài liệu] | [Điền] |

---

# PHỤ LỤC D. CHECKLIST TRƯỚC KHI NỘP

- [ ] Điền đầy đủ họ tên, MSSV, lớp, email và số điện thoại.
- [ ] Kiểm tra lại toàn bộ số liệu MAP.
- [ ] Kiểm tra ảnh truy vấn có bị tính vào kết quả hay không.
- [ ] Ghi rõ cách xác định ảnh liên quan.
- [ ] Ghi rõ đơn vị thời gian truy vấn.
- [ ] Có ít nhất ba hình minh họa cho từng chức năng.
- [ ] Mọi hình đều có số thứ tự và chú thích.
- [ ] Mọi bảng đều có tiêu đề hoặc phần giải thích.
- [ ] Mỗi hàm trong mã nguồn đều có chú thích.
- [ ] Tên biến và tên hàm rõ nghĩa.
- [ ] Mã nguồn tham khảo có ghi nguồn.
- [ ] Build thành công ở chế độ Release.
- [ ] Thư mục `Source`, `Release`, `Docs` đúng yêu cầu.
- [ ] Xóa file tạm, thư mục build không cần thiết và file ẩn.
- [ ] Kiểm tra tên file nén đúng quy định.
