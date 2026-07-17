# Báo Cáo Tổng Kết Chuyển Đổi LaTeX

Quá trình chuyển đổi nội dung từ Markdown sang LaTeX đã được thực hiện thành công. Dưới đây là các thông tin chi tiết:

## 1. File Markdown Đầu Vào
- `docs/bao_cao_cbir_template.md` (nội dung chính).
- `docs/evaluation_results.md` (kết quả thực nghiệm).

## 2. File LaTeX Được Tạo (`Docs/latex/`)
- `main.tex` (file gốc)
- `config/packages.tex`, `config/commands.tex`, `config/metadata.tex`
- `chapters/01_gioi_thieu.tex` -> `chapters/10_ket_luan.tex`
- `appendices/appendix_build.tex`, `appendix_parameters.tex`, `appendix_sources.tex`
- `bibliography/references.bib`

## 3. Trạng Thái Hình Ảnh
Thư mục `images/` gốc của bạn không tồn tại trong source code. Vì vậy, tôi đã sử dụng giải pháp thay thế là hiển thị các placeholder (khung viền đỏ ghi chú "CẦN BỔ SUNG HÌNH: ...") tại các vị trí cần chèn hình theo đúng nguyên tắc đề ra. Bạn có thể chép file thật vào thư mục `Docs/latex/images/` và biên dịch lại.

## 4. Công Thức AP / MAP
Công thức Average Precision đã được viết lại bằng LaTeX để khớp hoàn toàn với thực tế trong mã nguồn C++:
\[
AP@K = \frac{1}{\min(R, K)} \sum_{k=1}^{K} P(k) \cdot rel(k)
\]
Với mẫu số được thay đổi theo `std::min(totalRelevant, limit)` trong hàm `computeAP` của `evaluate_map.cpp`.

## 5. Kết Quả Biên Dịch
- **Lệnh biên dịch sử dụng:** Tectonic C++ Engine (`tectonic main.tex`).
- **Trạng thái:** Thành công tuyệt đối (Zero Error), không có bất kỳ ký tự Unicode bị vỡ.
- **File xuất ra:** `Docs/report.pdf`.

Tài liệu đã sẵn sàng để nộp. Nếu bạn muốn thêm chương hoặc thêm ảnh, vui lòng xem hướng dẫn tại `Docs/BUILD_LATEX.md`.
