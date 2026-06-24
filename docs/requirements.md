# Yêu cầu dự án: Truy vấn ảnh dựa vào nội dung thị giác (Content-Based Image Retrieval)

Dựa trên tài liệu mô tả bài tập (lap1-truyvan.pdf), dưới đây là danh sách các yêu cầu chi tiết cho dự án Hệ thống Truy vấn Thông tin Thị giác.

## 1. Thông tin chung
- **Mục tiêu:**
  - Xây dựng hệ thống truy vấn ảnh dựa vào các kỹ thuật xử lý ảnh.
  - Đánh giá kết quả của hệ thống (tốc độ, độ chính xác).
  - Lập trình hướng đối tượng (OOP) và sử dụng thư viện OpenCV.
- **Môi trường:** C/C++, OpenCV (được phép dùng tất cả API). Có thể xây dựng cho desktop hoặc mobile.
- **Dữ liệu (CSDL):** CSDL ảnh TMBuD và CD.

## 2. Tính năng chính (Quy trình hệ thống)
Hệ thống cần có giao diện đồ họa (GUI) mô phỏng quá trình truy vấn ảnh với 2 giai đoạn chính:

### A. Giai đoạn chuẩn bị CSDL
- Rút trích đặc trưng cho từng tấm ảnh trong CSDL.
- Lưu các đặc trưng này (vector nhiều chiều) vào một tập tin hoặc CSDL đặc trưng.

### B. Giai đoạn truy vấn
- Nhận ảnh đầu vào (ảnh truy vấn).
- Rút trích đặc trưng cho ảnh truy vấn.
- So sánh (duyệt) đặc trưng của ảnh truy vấn với CSDL đặc trưng để tìm ra K tấm ảnh gần giống nhất.
- Hiển thị kết quả trên giao diện theo thứ tự từ giống nhất đến giống ít nhất, kèm theo mức độ giống/giá trị so khớp tương ứng.

## 3. Các yêu cầu chức năng và Đánh giá (Theo thang điểm)

### 3.1. Truy vấn cơ bản và Đánh giá (50%)
- **Chức năng:** Cho phép người dùng truy vấn ảnh sử dụng:
  - Đặc trưng màu: Color histogram, Color correlogram.
  - Đặc trưng cục bộ: SIFT, ORB.
- **Độ đo so sánh:** Tự chọn và giải thích các độ đo (ví dụ: Khoảng cách Euclidean, Hệ số Cosine, Hệ số Jaccard...).
- **Đánh giá & Báo cáo:**
  - So sánh kết quả khi sử dụng các đặc trưng khác nhau.
  - Sử dụng độ đo **MAP (Mean Average Precision)** để đánh giá độ chính xác kết quả truy vấn lần lượt theo số lượng kết quả trả về là K = 3, 5, 11 và 21.

### 3.2. Nâng cao độ chính xác (30%)
- **Chức năng:** 
  - Tìm hiểu và sử dụng thêm các đặc trưng liên quan đến hình dáng (shape), biên cạnh (edge), vân (texture).
  - Kết hợp các đặc trưng hoặc sử dụng các phương pháp tiền xử lý (nâng cao chất lượng ảnh trước khi rút trích) để tăng độ chính xác của hệ thống.
- **Đánh giá & Báo cáo:**
  - So sánh kết quả truy vấn của các phương pháp cải tiến.
  - Tiếp tục sử dụng độ đo **MAP** để đánh giá độ chính xác với K = 3, 5, 11, 21.

### 3.3. Cải tiến tốc độ tìm kiếm (20%)
- **Chức năng:** Tìm hiểu và cài đặt các phương pháp để cải tiến tốc độ truy vấn.
- **Yêu cầu giao diện:** Ghi rõ **thời gian truy vấn** trên giao diện của chương trình.

## 4. Yêu cầu về mã nguồn và Báo cáo

### 4.1. Mã nguồn (Code)
- Code trong sáng, rõ ràng, áp dụng OOP.
- **Bắt buộc:** Chú thích đầy đủ cho mỗi hàm (thiếu sẽ bị trừ 50% điểm). Ghi rõ nguồn tham khảo nếu sử dụng code có sẵn.
- Đặt tên biến, tên hàm có ý nghĩa và đúng quy định.
- Chương trình build thành công dưới dạng **Release**.

### 4.2. Báo cáo (Docs)
- File báo cáo (doc/docx, ppt) có đầy đủ thông tin cá nhân.
- Kèm bảng checklist đánh giá mức độ hoàn thành các yêu cầu (50%, 100%).
- Minh họa 3 kết quả thực nghiệm cho từng chức năng.
- Hướng dẫn sử dụng chương trình rõ ràng, có hình ảnh minh họa hoặc clip demo (nếu cần).

## 5. Cấu trúc nộp bài
Nén file dạng .RAR hoặc .ZIP với tên `<MSSV1>_<MSSV2>_DOAN.rar/.zip` bao gồm:
- **Thư mục `<MSSV1>`**
  - `Source/`: Mã nguồn (đã xóa các file tạm, file ẩn, file .sdf, thư mục Demo).
  - `Release/`: Chứa file `*.exe`, `*.dll` (chỉ các file cần thiết, nếu OpenCV > 3.0 thì không cần dll).
  - `Docs/`: Chứa file báo cáo và hướng dẫn.
