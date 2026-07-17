# Hướng dẫn Biên dịch Báo cáo LaTeX sang PDF

Dự án này sử dụng LaTeX để tạo tài liệu báo cáo học thuật chuyên nghiệp. Các file mã nguồn LaTeX nằm trong thư mục `Docs/latex`.

## 1. Yêu cầu cài đặt

Để biên dịch, máy tính của bạn cần cài đặt một bản phân phối LaTeX hỗ trợ `XeLaTeX` hoặc `LuaLaTeX`:
- **Windows:** Cài đặt [MiKTeX](https://miktex.org/) hoặc [TeX Live](https://tug.org/texlive/).
- **Linux:** Cài đặt gói `texlive-full` (`sudo apt-get install texlive-full`).
- **macOS:** Cài đặt [MacTeX](https://www.tug.org/mactex/).

## 2. Cách biên dịch thành file PDF

Cách tốt nhất và tự động nhất là sử dụng công cụ `latexmk`. Mở Terminal/Command Prompt, trỏ vào thư mục `Docs/latex` và chạy lệnh sau:

```bash
cd Docs/latex
latexmk -xelatex -interaction=nonstopmode main.tex
```

Lệnh này sẽ tự động chạy XeLaTeX nhiều lần (để đánh số trang, tạo mục lục), chạy Biber để lấy danh sách tài liệu tham khảo, và cuối cùng xuất ra file `main.pdf`.

### Nếu không có `latexmk`
Bạn có thể chạy thủ công tuần tự các lệnh sau:

```bash
cd Docs/latex
xelatex main.tex
biber main
xelatex main.tex
xelatex main.tex
```

## 3. Cập nhật và chỉnh sửa nội dung

- **Thêm chương:** Sửa nội dung các file trong thư mục `Docs/latex/chapters/`. Nếu muốn thêm chương hoàn toàn mới, hãy tạo file `.tex` mới và thêm dòng `\input{chapters/ten_file_moi}` vào file `main.tex`.
- **Thêm hình ảnh:** Đặt file ảnh thật vào thư mục `Docs/latex/images/`. Sau đó thay thế lệnh `\missingfigure{ten_anh.png}{Chú thích}` bằng cấu trúc:
  ```latex
  \begin{figure}[H]
      \centering
      \includegraphics[width=0.8\textwidth]{images/ten_anh.png}
      \caption{Chú thích ảnh.}
      \label{fig:nhan_anh}
  \end{figure}
  ```
- **Thêm tài liệu tham khảo:** Thêm chuẩn trích dẫn BibTeX vào file `Docs/latex/bibliography/references.bib`, sau đó dùng lệnh `\cite{ten_khoa}` trong nội dung `.tex`.

## 4. Xử lý sự cố thường gặp
- **Lỗi font chữ:** Đảm bảo hệ điều hành của bạn có cài đặt font `Times New Roman`. Nếu không, LaTeX sẽ báo lỗi `Font Times New Roman not found`. Bạn có thể đổi sang font hệ thống khác trong file `Docs/latex/config/packages.tex`.
- **Mục lục không hiển thị hoặc hiện dấu `??`:** Bạn cần biên dịch file `.tex` ít nhất 2 lần để hệ thống LaTeX có thể thu thập đủ các nhãn (label) tham chiếu. Mặc định `latexmk` tự động lo việc này.
