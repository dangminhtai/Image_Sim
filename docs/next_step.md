# Next Step

## Ưu tiên cao
- [ ] Thực hiện nén thư mục `23127523_DOAN_FINAL` thành file `.zip` hoặc `.rar` chuẩn bị nộp bài.
- [ ] Kiểm tra chạy thử file `23127523_DOAN_FINAL/Release/cbir_gui.exe` sau khi đặt thư mục `data` tải từ link Drive cùng cấp để đảm bảo không bị lỗi thiếu DLL hay đường dẫn tương đối.

## Ưu tiên trung bình
- [ ] Cập nhật báo cáo LaTeX bằng cách bổ sung nội dung giải thích cơ chế Forward Index & Inverted Index, phân tích kết quả MAP@K và so sánh sự khác biệt khi dùng TF-IDF scoring.
- [ ] Đo đạc thời gian truy vấn trung bình (Query Latency) trên tập dữ liệu lớn hơn (TMBuD) để làm rõ hiệu năng vượt trội của Inverted Index so với Brute-force.

## Ý tưởng sau này
- [ ] Tích hợp cơ chế phân cụm phân cấp (Hierarchical K-Means) để xây dựng Vocabulary Tree, nâng cao độ chính xác và khả năng mở rộng của BoW SIFT/ORB.
- [ ] Thử nghiệm phương pháp Query Expansion (Mở rộng truy vấn) dựa trên phản hồi của người dùng hoặc tự động (Pseudo-Relevance Feedback) để tăng độ đo MAP.
