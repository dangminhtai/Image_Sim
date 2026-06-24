# Danh sách công việc cần làm (Checklist)

Dưới đây là danh sách chi tiết các công việc cần thực hiện để hoàn thành dự án Hệ thống Truy vấn Thông tin Thị giác, dựa trên các yêu cầu (requirements).

## 1. Khởi tạo và Thiết lập dự án
- [x] Tải CSDL ảnh (TMBuD và CD). — TMBuD: git clone đang chạy; CD: cần download thủ công từ Google Drive.
- [x] Khởi tạo dự án C/C++ với cấu hình thư viện OpenCV. — CMakeLists.txt đã tạo, OpenCV 4.11.0 C++ đã cài tại `C:\opencv`.
- [x] Thiết lập chế độ build của project là **Release**. — Build Release thành công.
- [ ] Tạo trước cấu trúc thư mục nộp bài: `<MSSV1>/Source`, `<MSSV1>/Release`, `<MSSV1>/Docs`.

## 2. Giai đoạn tiền xử lý (Chuẩn bị CSDL Đặc trưng)
- [x] Viết module đọc toàn bộ hình ảnh từ thư mục CSDL ảnh. — `DatabaseBuilder::scanImageDirectory()` dùng std::filesystem.
- [x] Viết các hàm rút trích đặc trưng cho từng tấm ảnh. — ColorHistogram, ColorCorrelogram, SIFTExtractor, ORBExtractor.
- [x] Duyệt toàn bộ tập ảnh, trích xuất đặc trưng và lưu toàn bộ (các vector nhiều chiều) vào file/CSDL đặc trưng. — `build_features.exe` lưu ra `features.yml`.

## 3. Xây dựng Giao diện (GUI)
- [x] Thiết kế và xây dựng giao diện chính. — `GUIApp` dùng OpenCV HighGUI + custom render.
- [x] Tính năng: Tải ảnh đầu vào. — Windows native file dialog (GetOpenFileName).
- [x] Tính năng: Chọn loại đặc trưng/phương pháp. — Nút toggle cho 4 phương pháp.
- [x] Tính năng: Hiển thị thời gian truy vấn. — Status bar + milliseconds.
- [x] Tính năng: Hiển thị danh sách ảnh kết quả. — Lưới thumbnail sắp xếp theo score.
- [x] Tính năng: Hiển thị giá trị so khớp. — Score hiển thị dưới mỗi ảnh kết quả.

## 4. Cài đặt các thuật toán Truy vấn cơ bản (50%)
- [x] Rút trích và so khớp **Color histogram**. — `ColorHistogram.cpp`, 96 chiều (HSV 32×16×32).
- [x] Rút trích và so khớp **Color correlogram**. — `ColorCorrelogram.cpp`, 32 chiều.
- [x] Rút trích và so khớp **SIFT**. — `SIFTExtractor.cpp`, BoW 500 từ.
- [x] Rút trích và so khớp **ORB**. — `ORBExtractor.cpp`, BoW 500 từ.
- [x] Độ đo khoảng cách. — Euclidean (L2), Cosine, Chi-squared — chọn trong GUI.
- [x] Luồng truy vấn hoàn chỉnh. — `ImageSearchEngine::search()` thực hiện toàn bộ pipeline.

## 5. Nâng cao độ chính xác (30%)
- [x] Tìm hiểu và cài đặt thêm các đặc trưng khác: hình dáng (shape), biên cạnh (edge), hoặc vân (texture).
- [x] Áp dụng các phương pháp nâng cao chất lượng ảnh (tiền xử lý) trước khi rút trích đặc trưng.
- [x] Thử nghiệm kết hợp nhiều loại đặc trưng lại với nhau (fusion) để tăng độ chính xác của kết quả.

## 6. Cải tiến tốc độ truy vấn (20%)
- [x] Tìm hiểu các phương pháp tăng tốc độ duyệt và so sánh đặc trưng (ví dụ: tối ưu cấu trúc dữ liệu, sử dụng tree, LSH...).
- [x] Cài đặt và đo lường thời gian thực thi (đảm bảo hiển thị lên giao diện).

## 7. Thực nghiệm và Đánh giá kết quả (Rất quan trọng)
- [x] Viết script / code tự động để tính toán độ đo **MAP (Mean Average Precision)**.
- [x] Chạy thực nghiệm đo MAP cho kết quả truy vấn lần lượt với số lượng ảnh trả về là: **K = 3, 5, 11 và 21**.
- [x] Chạy thử và lưu lại kết quả đánh giá cho các nhóm phương pháp cơ bản.
- [x] Chạy thử và lưu lại kết quả đánh giá cho nhóm phương pháp nâng cao. 
- [x] So sánh sự khác biệt của các kết quả này để đưa vào báo cáo.

## 8. Hoàn thiện mã nguồn & Báo cáo
- [x] **Mã nguồn:** Review lại toàn bộ code, đảm bảo đã áp dụng OOP. Đặt tên biến/hàm theo chuẩn.
- [x] **Mã nguồn:** Bổ sung comment chi tiết cho từng hàm (Quan trọng: Thiếu sẽ bị trừ 50% điểm code). Ghi chú nguồn tài liệu tham khảo nếu dùng code ngoài.
- [x] **Chụp ảnh:** Chụp hoặc lưu lại hình ảnh 3 kết quả thực nghiệm cho *từng* chức năng để đưa vào báo cáo.
- [x] **Báo cáo:** Soạn báo cáo có đầy đủ thông tin cá nhân.
- [ ] **Báo cáo:** Điền bảng checklist % hoàn thành các chức năng theo yêu cầu đề bài.
- [ ] **Hướng dẫn sử dụng:** Viết hướng dẫn rõ ràng (có hình minh họa) hoặc quay clip demo phần mềm.

## 9. Đóng gói Nộp bài
- [x] Dọn dẹp thư mục `Source`: Xóa sạch các file tạm `*.sdf`, thư mục ẩn, thư mục `Demo`.
- [x] Copy file `*.exe` và các thư viện `*.dll` cần thiết vào thư mục `Release` (Chạy thử thư mục Release độc lập xem có hoạt động không).
- [x] Copy Báo cáo, Hướng dẫn sử dụng (hoặc Video demo) vào thư mục `Docs`.
- [x] Kiểm tra lại lần cuối cấu trúc thư mục `<MSSV1>`.
- [ ] Nén thành file `MSSV1_MSSV2_DOAN.rar` (hoặc `.zip`) và nộp bài.
