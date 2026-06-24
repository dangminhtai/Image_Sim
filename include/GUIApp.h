/**
 * @file GUIApp.h
 * @brief Lớp ứng dụng GUI chính cho hệ thống truy vấn ảnh CBIR.
 *
 * GUIApp sử dụng OpenCV HighGUI làm nền tảng hiển thị.
 * Toàn bộ UI được vẽ thủ công lên cv::Mat canvas và hiển thị qua imshow().
 *
 * Layout giao diện:
 * ┌─────────────────────────────────────────────────────┐
 * │  TITLE BAR                                          │
 * ├──────────────┬──────────────────────────────────────┤
 * │  LEFT PANEL  │  RIGHT PANEL (Results Grid)          │
 * │  [Query Img] │  [img1][img2][img3][img4][img5]      │
 * │  [Controls]  │  [img6][img7]...                     │
 * │  [Load Btn]  │                                      │
 * │  [Search Btn]│                                      │
 * ├──────────────┴──────────────────────────────────────┤
 * │  STATUS BAR: method | metric | time | DB size       │
 * └─────────────────────────────────────────────────────┘
 */
#pragma once

#include "ImageSearchEngine.h"
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <vector>
#include <functional>

/**
 * @struct Button
 * @brief Nút bấm đơn giản được vẽ trên canvas.
 */
struct Button {
    cv::Rect rect;          ///< Vị trí và kích thước nút
    std::string label;      ///< Nhãn hiển thị
    bool isSelected;        ///< Trạng thái được chọn (cho toggle button)
    bool isHovered;         ///< Trạng thái khi chuột hover

    /**
     * @brief Kiểm tra một điểm có nằm trong nút không.
     * @param pt Tọa độ điểm cần kiểm tra.
     * @return true nếu điểm nằm trong rect.
     */
    bool contains(cv::Point pt) const { return rect.contains(pt); }
};

/**
 * @class GUIApp
 * @brief Ứng dụng GUI CBIR dùng OpenCV HighGUI.
 *
 * Toàn bộ UI được render lên cv::Mat, mouse callback xử lý tương tác.
 * Không cần thư viện GUI ngoài (Qt, wxWidgets...).
 */
class GUIApp {
public:
    // ── Kích thước cửa sổ ─────────────────────────────
    static constexpr int WIN_W      = 1280; ///< Chiều rộng cửa sổ
    static constexpr int WIN_H      = 820;  ///< Chiều cao cửa sổ
    static constexpr int LEFT_W     = 280;  ///< Chiều rộng panel trái
    static constexpr int STATUS_H   = 40;   ///< Chiều cao thanh trạng thái
    static constexpr int TITLE_H    = 50;   ///< Chiều cao tiêu đề
    static constexpr int THUMB_SIZE = 110;  ///< Kích thước ảnh kết quả (pixel)
    static constexpr int THUMB_GAP  = 12;   ///< Khoảng cách giữa ảnh kết quả

    /**
     * @brief Constructor.
     * @param engine Tham chiếu đến ImageSearchEngine đã nạp CSDL.
     */
    explicit GUIApp(ImageSearchEngine& engine);

    /**
     * @brief Khởi động vòng lặp GUI chính (blocking cho đến khi đóng cửa sổ).
     */
    void run();

private:
    ImageSearchEngine& m_engine;            ///< Engine truy vấn
    cv::Mat m_canvas;                       ///< Canvas chính để vẽ UI
    cv::Mat m_queryImage;                   ///< Ảnh truy vấn hiện tại
    std::string m_queryPath;                ///< Đường dẫn ảnh truy vấn
    std::vector<SearchResult> m_results;    ///< Kết quả truy vấn hiện tại
    double m_lastQueryTime = 0.0;           ///< Thời gian truy vấn gần nhất (ms)

    // ── State giao diện ───────────────────────────────
    int m_selectedDbIdx      = 0;           ///< Index database đang chọn (0 = TMBuD, 1 = CD)
    int m_selectedMethodIdx  = 0;           ///< Index phương pháp đang chọn
    int m_selectedMetricIdx  = 0;           ///< Index độ đo đang chọn
    int m_selectedK          = 15;          ///< Số kết quả hiển thị
    int m_hoveredResult      = -1;          ///< Index kết quả đang hover
    std::string m_statusMsg;                ///< Thông báo trên thanh status

    // ── Buttons ───────────────────────────────────────
    Button m_btnLoad;                       ///< Nút mở ảnh
    Button m_btnSearch;                     ///< Nút tìm kiếm
    std::vector<Button> m_dbBtns;           ///< Nút chọn Database
    std::vector<Button> m_methodBtns;       ///< Nút chọn phương pháp
    std::vector<Button> m_metricBtns;       ///< Nút chọn độ đo
    std::vector<Button> m_kBtns;            ///< Nút chọn K

    // ── Màu sắc theme ─────────────────────────────────
    const cv::Scalar COLOR_BG        {30, 30, 30};       ///< Nền tối
    const cv::Scalar COLOR_PANEL     {45, 45, 45};       ///< Nền panel
    const cv::Scalar COLOR_ACCENT    {0, 165, 255};      ///< Màu nhấn (cam)
    const cv::Scalar COLOR_ACCENT2   {0, 200, 100};      ///< Màu nhấn 2 (xanh lá)
    const cv::Scalar COLOR_BTN       {70, 70, 70};       ///< Nền nút thường
    const cv::Scalar COLOR_BTN_SEL   {0, 130, 200};      ///< Nền nút được chọn
    const cv::Scalar COLOR_BTN_HOVER {90, 90, 90};       ///< Nền nút hover
    const cv::Scalar COLOR_TEXT      {220, 220, 220};    ///< Màu chữ chính
    const cv::Scalar COLOR_TEXT_DIM  {140, 140, 140};    ///< Màu chữ mờ
    const cv::Scalar COLOR_SUCCESS   {0, 200, 100};      ///< Màu thành công
    const cv::Scalar COLOR_WARNING   {0, 165, 255};      ///< Màu cảnh báo
    const cv::Scalar COLOR_RESULT_BG {55, 55, 55};       ///< Nền ảnh kết quả
    const cv::Scalar COLOR_BORDER    {80, 80, 80};       ///< Màu viền

    // ── Phương thức nội bộ ────────────────────────────

    /** @brief Khởi tạo layout nút bấm. */
    void initButtons();

    /** @brief Render toàn bộ giao diện lên canvas. */
    void render();

    /** @brief Vẽ thanh tiêu đề. */
    void drawTitle();

    /** @brief Vẽ panel trái (query image + controls). */
    void drawLeftPanel();

    /** @brief Vẽ panel phải (kết quả). */
    void drawResultsPanel();

    /** @brief Vẽ thanh trạng thái phía dưới. */
    void drawStatusBar();

    /** @brief Vẽ một nút bấm lên canvas. */
    void drawButton(const Button& btn,
                    const cv::Scalar& normalColor,
                    const cv::Scalar& selectedColor,
                    int textScale = 1);

    /** @brief Vẽ ảnh thumbnail vào vùng rect, giữ tỷ lệ. */
    void drawThumbnail(const cv::Mat& img, const cv::Rect& rect);

    /** @brief Vẽ văn bản căn giữa theo chiều ngang trong vùng rect. */
    void drawTextCentered(const std::string& text, const cv::Rect& rect,
                          const cv::Scalar& color, double scale = 0.45, int thickness = 1);

    /**
     * @brief Xử lý sự kiện chuột từ OpenCV callback.
     * @param event Loại sự kiện (EVENT_LBUTTONDOWN, EVENT_MOUSEMOVE, ...).
     * @param x, y Tọa độ chuột.
     */
    void onMouse(int event, int x, int y);

    /** @brief Mở hộp thoại chọn file ảnh (Windows native dialog). */
    bool openFileDialog(std::string& outPath);

    /** @brief Thực hiện truy vấn với cài đặt hiện tại. */
    void doSearch();

    /** @brief Trả về tên độ đo đang chọn. */
    DistanceMetric currentMetric() const;

    /** @brief Static callback cho OpenCV mouse event. */
    static void mouseCallback(int event, int x, int y, int flags, void* userdata);
};
