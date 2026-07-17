# Bài học rút ra từ dự án

## Bài học 2026-07-17

- **Vấn đề:** 
  Khi thực hiện truy vấn BoW histogram (SIFT, ORB) bằng brute-force Euclidean hay Cosine trên tập dữ liệu lớn, việc quét toàn bộ cơ sở dữ liệu và tính toán vector dense có chi phí thời gian cao.
  
- **Điều học được:**
  - Áp dụng cấu trúc **Inverted Index (visual word → posting list)** giúp chuyển bài toán so khớp ảnh BoW sang dạng bài toán truy vấn thông tin văn bản (Information Retrieval).
  - Sử dụng mô hình tích lũy trọng số **TF-IDF** cho phép lọc nhanh các ảnh ứng viên (candidate list) có chung visual word thay vì so sánh toàn bộ CSDL, đem lại tốc độ vượt trội trên các phân bố histogram sparse.
  - Sử dụng **Forward Index** tách rời dữ liệu metadata và feature vector giúp tăng tính module hóa, nâng tốc độ lấy feature của một ảnh cụ thể lên O(1).
  
- **Cách áp dụng sau này:**
  - Đối với các hệ thống truy vấn thông tin đa phương tiện sử dụng đặc trưng dạng túi (Bag of X): Luôn xây dựng inverted file/index đi kèm thay vì so khớp trực tiếp.
  - Khi thiết kế cấu trúc dữ liệu lưu trữ CSDL đặc trưng, cần thiết kế cấu trúc chỉ mục tra cứu hai chiều ngay từ đầu để đáp ứng cả yêu cầu cập nhật (forward) và tìm kiếm (inverted).
