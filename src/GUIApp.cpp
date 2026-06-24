/**
 * @file GUIApp.cpp
 * @brief Cài đặt GUI ứng dụng CBIR dùng OpenCV HighGUI.
 *
 * Toàn bộ UI được render bằng cách vẽ lên cv::Mat.
 * Tương tác chuột qua OpenCV mouse callback.
 * File dialog dùng Windows API (GetOpenFileNameA).
 */

#include "GUIApp.h"
#include <opencv2/imgcodecs.hpp>
#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <iomanip>
#include <filesystem>

namespace fs = std::filesystem;

static std::string getBasePath() {
    if (fs::exists("data")) return "";
    if (fs::exists("../data")) return "../";
    if (fs::exists("../../data")) return "../../";
    if (fs::exists("../../../data")) return "../../../";
    return "";
}

// Windows native file dialog
#ifdef _WIN32
#define NOMINMAX          // Ngăn Windows define macro min/max (conflict với std::min/max)
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <commdlg.h>
#pragma comment(lib, "comdlg32.lib")
#endif

static constexpr char WIN_NAME[] = "CBIR - Content-Based Image Retrieval";

// ──────────────────────────────────────────────
// Constructor
// ──────────────────────────────────────────────

/**
 * @brief Khởi tạo GUIApp với engine đã nạp CSDL.
 */
GUIApp::GUIApp(ImageSearchEngine& engine)
    : m_engine(engine)
{
    m_canvas = cv::Mat(WIN_H, WIN_W, CV_8UC3);
    m_statusMsg = "San sang. Bam [Mo anh] de chon anh truy van.";
    initButtons();
}

// ──────────────────────────────────────────────
// initButtons()
// ──────────────────────────────────────────────

/**
 * @brief Khởi tạo layout tất cả nút bấm trong panel trái.
 *
 * Các nút được sắp xếp theo thứ tự từ trên xuống:
 *  - Nút Load ảnh
 *  - Nhóm chọn phương pháp (method)
 *  - Nhóm chọn độ đo (metric)
 *  - Nhóm chọn K
 *  - Nút Tìm kiếm
 */
void GUIApp::initButtons()
{
    int px = 10;      // Padding trái
    int py = TITLE_H + 10; // Vị trí bắt đầu (dưới title)
    int bw = LEFT_W - 20; // Chiều rộng nút
    int bh = 32;          // Chiều cao nút thường

    // ── Nút Load ảnh ──────────────────────────
    m_btnLoad = {{px, py, bw, bh + 4}, "Mo anh truy van...", false, false};
    py += bh + 4 + 20; // Dịch xuống cho label "Anh truy van:"

    // ── Dành không gian cho khung ảnh ─────────
    int imgSize = bw;
    py += imgSize + 20; // Dịch qua vùng vẽ ảnh (260x260) + khoảng trống dưới ảnh

    // ── Nhóm method ───────────────────────────
    m_methodBtns.clear();
    auto methods = m_engine.getAvailableMethods();
    if (methods.empty()) {
        methods = {"ColorHistogram", "ColorCorrelogram", "SIFT", "ORB"};
    }

    int halfW = (bw - 4) / 2;

    // ── Nhóm Database ─────────────────────────────
    m_dbBtns.clear();
    m_dbBtns.push_back({{px, py, halfW, bh}, "TMBuD (Bld)", m_selectedDbIdx == 0, false});
    m_dbBtns.push_back({{px + halfW + 4, py, halfW, bh}, "CD Covers", m_selectedDbIdx == 1, false});
    py += bh + 15;

    for (int i = 0; i < (int)methods.size(); i++) {
        int col = i % 2, row = i / 2;
        int bx = px + col * (halfW + 4);
        int by = py + row * (bh + 4);
        m_methodBtns.push_back({{bx, by, halfW, bh}, methods[i], i == m_selectedMethodIdx, false});
    }
    int methodRows = ((int)methods.size() + 1) / 2;
    py += methodRows * (bh + 4) + 15;

    // ── Nhóm metric ───────────────────────────
    m_metricBtns.clear();
    std::vector<std::string> metrics = {"Euclidean", "Cosine", "Chi-Sq"};
    int mw = (bw - 8) / 3;
    for (int i = 0; i < 3; i++) {
        m_metricBtns.push_back({{px + i * (mw + 4), py, mw, bh},
                                 metrics[i], i == 0, false});
    }
    py += bh + 15;

    // ── Nhóm K ────────────────────────────────
    m_kBtns.clear();
    std::vector<int> kValues = {3, 5, 11, 15};
    int kw = (bw - 12) / 4;
    for (int i = 0; i < 4; i++) {
        std::string label = "K=" + std::to_string(kValues[i]);
        bool sel = (kValues[i] == m_selectedK);
        m_kBtns.push_back({{px + i * (kw + 4), py, kw, bh}, label, sel, false});
    }
    py += bh + 15;

    // ── Nút Search ────────────────────────────
    m_btnSearch = {{px, py, bw, bh + 8}, "Tim kiem", false, false};
}

// ──────────────────────────────────────────────
// run()
// ──────────────────────────────────────────────

/**
 * @brief Vòng lặp chính của GUI.
 *
 * Tạo cửa sổ, đăng ký mouse callback, render liên tục cho đến khi
 * người dùng đóng cửa sổ hoặc nhấn phím 'q'/ESC.
 */
void GUIApp::run()
{
    cv::namedWindow(WIN_NAME, cv::WINDOW_AUTOSIZE);
    cv::setMouseCallback(WIN_NAME, mouseCallback, this);

    while (true) {
        render();
        cv::imshow(WIN_NAME, m_canvas);

        int key = cv::waitKey(30);
        if (key == 27 || key == 'q') break;          // ESC hoặc q
        if (cv::getWindowProperty(WIN_NAME, cv::WND_PROP_VISIBLE) < 1) break;
    }

    cv::destroyAllWindows();
}

// ──────────────────────────────────────────────
// render()
// ──────────────────────────────────────────────

/**
 * @brief Render toàn bộ UI lên canvas.
 */
void GUIApp::render()
{
    m_canvas.setTo(COLOR_BG);
    drawTitle();
    drawLeftPanel();
    drawResultsPanel();
    drawStatusBar();
}

// ──────────────────────────────────────────────
// drawTitle()
// ──────────────────────────────────────────────

/**
 * @brief Vẽ thanh tiêu đề với gradient đơn giản.
 */
void GUIApp::drawTitle()
{
    // Nền tiêu đề
    cv::rectangle(m_canvas, {0, 0, WIN_W, TITLE_H},
                  cv::Scalar(25, 25, 35), cv::FILLED);

    // Đường viền dưới title
    cv::line(m_canvas, {0, TITLE_H}, {WIN_W, TITLE_H}, COLOR_ACCENT, 2);

    // Tiêu đề chính
    cv::putText(m_canvas, "CBIR - Content-Based Image Retrieval System",
                {15, TITLE_H - 16}, cv::FONT_HERSHEY_DUPLEX, 0.7,
                COLOR_ACCENT, 1, cv::LINE_AA);

    // Thông tin DB ở góc phải
    std::string dbInfo = "DB: " + std::to_string(m_engine.getDatabaseSize()) + " anh";
    int baseline = 0;
    auto sz = cv::getTextSize(dbInfo, cv::FONT_HERSHEY_SIMPLEX, 0.4, 1, &baseline);
    cv::putText(m_canvas, dbInfo,
                {WIN_W - sz.width - 15, TITLE_H - 16},
                cv::FONT_HERSHEY_SIMPLEX, 0.4, COLOR_TEXT_DIM, 1, cv::LINE_AA);
}

// ──────────────────────────────────────────────
// drawLeftPanel()
// ──────────────────────────────────────────────

/**
 * @brief Vẽ panel trái: ảnh query + tất cả controls.
 */
void GUIApp::drawLeftPanel()
{
    int panelH = WIN_H - TITLE_H - STATUS_H;

    // Nền panel trái
    cv::rectangle(m_canvas, {0, TITLE_H, LEFT_W, panelH},
                  COLOR_PANEL, cv::FILLED);
    cv::line(m_canvas, {LEFT_W, TITLE_H}, {LEFT_W, WIN_H - STATUS_H},
             COLOR_BORDER, 1);

    // ── Nút Load ──────────────────────────────
    drawButton(m_btnLoad, COLOR_BTN_SEL, COLOR_ACCENT);

    // ── Ảnh query ─────────────────────────────
    int imgY   = m_btnLoad.rect.y + m_btnLoad.rect.height + 10;
    int imgSize = LEFT_W - 20;
    cv::Rect imgRect = {10, imgY, imgSize, imgSize};

    // Khung ảnh
    cv::rectangle(m_canvas, imgRect, COLOR_BORDER, 1);

    if (!m_queryImage.empty()) {
        drawThumbnail(m_queryImage, imgRect);
    } else {
        // Placeholder khi chưa có ảnh
        cv::rectangle(m_canvas, imgRect, cv::Scalar(40, 40, 40), cv::FILLED);
        drawTextCentered("Chua co anh", imgRect, COLOR_TEXT_DIM, 0.4);
    }

    // Label "Anh truy van"
    cv::putText(m_canvas, "Anh truy van:",
                {10, imgY - 5}, cv::FONT_HERSHEY_SIMPLEX, 0.38,
                COLOR_TEXT_DIM, 1, cv::LINE_AA);

    // Draw db btns
    cv::putText(m_canvas, "Chon Database:", cv::Point(20, m_dbBtns[0].rect.y - 8),
                cv::FONT_HERSHEY_SIMPLEX, 0.45, COLOR_TEXT_DIM, 1);
    for (const auto& btn : m_dbBtns) {
        drawButton(btn, COLOR_BTN, COLOR_BTN_SEL);
    }

    // Draw method btns
    cv::putText(m_canvas, "Phuong phap:", cv::Point(20, m_methodBtns[0].rect.y - 8),
                cv::FONT_HERSHEY_SIMPLEX, 0.45, COLOR_TEXT_DIM, 1);
    for (const auto& btn : m_methodBtns) {
        drawButton(btn, COLOR_BTN, COLOR_BTN_SEL);
    }

    // ── Section: Độ đo ───────────────────────
    if (!m_metricBtns.empty()) {
        int metricLabelY = m_metricBtns[0].rect.y - 6;
        cv::putText(m_canvas, "Do do khoang cach:",
                    {10, metricLabelY}, cv::FONT_HERSHEY_SIMPLEX, 0.38,
                    COLOR_TEXT_DIM, 1, cv::LINE_AA);
    }

    for (auto& btn : m_metricBtns) {
        drawButton(btn, COLOR_BTN, COLOR_BTN_SEL);
    }

    // ── Section: K ───────────────────────────
    if (!m_kBtns.empty()) {
        int kLabelY = m_kBtns[0].rect.y - 6;
        cv::putText(m_canvas, "So luong ket qua K:",
                    {10, kLabelY}, cv::FONT_HERSHEY_SIMPLEX, 0.38,
                    COLOR_TEXT_DIM, 1, cv::LINE_AA);
    }

    for (auto& btn : m_kBtns) {
        drawButton(btn, COLOR_BTN, COLOR_BTN_SEL);
    }

    // ── Nút Search ───────────────────────────
    cv::Scalar searchColor = m_queryImage.empty() ? COLOR_BTN : COLOR_ACCENT2;
    drawButton(m_btnSearch, searchColor, COLOR_ACCENT2);
}

// ──────────────────────────────────────────────
// drawResultsPanel()
// ──────────────────────────────────────────────

/**
 * @brief Vẽ panel kết quả bên phải dưới dạng lưới ảnh thumbnail.
 *
 * Mỗi ô kết quả gồm: thumbnail ảnh + tên label + điểm similarity.
 * Ảnh được highlight khi chuột hover.
 */
void GUIApp::drawResultsPanel()
{
    int panelX = LEFT_W + 1;
    int panelY = TITLE_H;
    int panelW = WIN_W - LEFT_W - 1;
    int panelH = WIN_H - TITLE_H - STATUS_H;

    // Nền panel phải
    cv::rectangle(m_canvas, {panelX, panelY, panelW, panelH},
                  COLOR_BG, cv::FILLED);

    if (m_results.empty()) {
        // Placeholder khi chưa có kết quả
        std::string msg = m_queryImage.empty()
                          ? "Mo anh va bam Tim kiem"
                          : "Bam [Tim kiem] de bat dau truy van";
        cv::Rect area = {panelX, panelY, panelW, panelH};
        drawTextCentered(msg, area, COLOR_TEXT_DIM, 0.5);
        return;
    }

    // Tiêu đề panel kết quả
    std::string title = "Ket qua: " + std::to_string(m_results.size()) + " anh";
    cv::putText(m_canvas, title,
                {panelX + 10, panelY + 22}, cv::FONT_HERSHEY_SIMPLEX, 0.5,
                COLOR_TEXT, 1, cv::LINE_AA);
    cv::line(m_canvas, {panelX, panelY + 30}, {WIN_W, panelY + 30},
             COLOR_BORDER, 1);

    // Tính số cột
    int startX = panelX + THUMB_GAP;
    int startY = panelY + 35;
    int cols   = std::max(1, (panelW - THUMB_GAP) / (THUMB_SIZE + THUMB_GAP));

    for (int i = 0; i < (int)m_results.size(); i++) {
        int col = i % cols;
        int row = i / cols;
        int x   = startX + col * (THUMB_SIZE + THUMB_GAP);
        int y   = startY + row * (THUMB_SIZE + THUMB_GAP + 35);

        if (y + THUMB_SIZE + 35 > WIN_H - STATUS_H) break; // Overflow

        bool hovered = (i == m_hoveredResult);
        cv::Rect cellRect = {x - 4, y - 4, THUMB_SIZE + 8, THUMB_SIZE + 40};

        // Nền cell
        cv::rectangle(m_canvas, cellRect,
                      hovered ? cv::Scalar(65, 65, 75) : COLOR_RESULT_BG,
                      cv::FILLED);
        cv::rectangle(m_canvas, cellRect,
                      hovered ? COLOR_ACCENT : COLOR_BORDER, 1);

        // Thumbnail ảnh
        cv::Rect thumbRect = {x, y, THUMB_SIZE, THUMB_SIZE};
        std::string fullPath = getBasePath() + m_results[i].imagePath;
        cv::Mat img = cv::imread(fullPath);
        if (!img.empty()) {
            drawThumbnail(img, thumbRect);
        } else {
            cv::rectangle(m_canvas, thumbRect, cv::Scalar(60, 60, 60), cv::FILLED);
            drawTextCentered("?", thumbRect, COLOR_TEXT_DIM);
        }

        // Label (tên lớp)
        cv::Rect labelRect = {x, y + THUMB_SIZE + 2, THUMB_SIZE, 16};
        std::string labelStr = "#" + std::to_string(i + 1) + " " + m_results[i].label;
        drawTextCentered(labelStr, labelRect,
                         hovered ? COLOR_TEXT : COLOR_TEXT_DIM, 0.38);

        // Score (điểm tương đồng)
        cv::Rect scoreRect = {x, y + THUMB_SIZE + 18, THUMB_SIZE, 16};
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(4) << m_results[i].score;
        drawTextCentered("score: " + oss.str(), scoreRect, COLOR_ACCENT2, 0.35);
    }
}

// ──────────────────────────────────────────────
// drawStatusBar()
// ──────────────────────────────────────────────

/**
 * @brief Vẽ thanh trạng thái ở cuối cửa sổ.
 *
 * Hiển thị: thông báo | phương pháp | độ đo | thời gian truy vấn
 */
void GUIApp::drawStatusBar()
{
    int barY = WIN_H - STATUS_H;
    cv::rectangle(m_canvas, {0, barY, WIN_W, STATUS_H},
                  cv::Scalar(20, 20, 30), cv::FILLED);
    cv::line(m_canvas, {0, barY}, {WIN_W, barY}, COLOR_ACCENT, 1);

    // Thông báo trái
    cv::putText(m_canvas, m_statusMsg,
                {10, barY + STATUS_H / 2 + 5},
                cv::FONT_HERSHEY_SIMPLEX, 0.38, COLOR_TEXT, 1, cv::LINE_AA);

    // Thời gian truy vấn (phải)
    if (m_lastQueryTime > 0) {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(1) << m_lastQueryTime << " ms";
        std::string timeStr = "Thoi gian: " + oss.str();
        int baseline = 0;
        auto sz = cv::getTextSize(timeStr, cv::FONT_HERSHEY_SIMPLEX, 0.38, 1, &baseline);
        cv::putText(m_canvas, timeStr,
                    {WIN_W - sz.width - 10, barY + STATUS_H / 2 + 5},
                    cv::FONT_HERSHEY_SIMPLEX, 0.38, COLOR_ACCENT, 1, cv::LINE_AA);
    }
}

// ──────────────────────────────────────────────
// drawButton()
// ──────────────────────────────────────────────

/**
 * @brief Vẽ một nút bấm lên canvas với màu tùy theo trạng thái.
 */
void GUIApp::drawButton(const Button& btn,
                         const cv::Scalar& normalColor,
                         const cv::Scalar& selectedColor,
                         int textScale)
{
    cv::Scalar bgColor = btn.isSelected ? selectedColor
                       : btn.isHovered  ? COLOR_BTN_HOVER
                       : normalColor;

    // Bo góc giả lập bằng vẽ đè
    cv::rectangle(m_canvas, btn.rect, bgColor, cv::FILLED);
    cv::rectangle(m_canvas, btn.rect,
                  btn.isSelected ? COLOR_ACCENT : COLOR_BORDER, 1);

    // Nhãn căn giữa
    drawTextCentered(btn.label, btn.rect, COLOR_TEXT, 0.38 + textScale * 0.04);
}

// ──────────────────────────────────────────────
// drawThumbnail()
// ──────────────────────────────────────────────

/**
 * @brief Vẽ ảnh vào vùng rect, giữ tỷ lệ khung hình (letterbox).
 */
void GUIApp::drawThumbnail(const cv::Mat& img, const cv::Rect& rect)
{
    if (img.empty()) return;

    // Tính kích thước giữ tỷ lệ
    float scaleX = static_cast<float>(rect.width)  / img.cols;
    float scaleY = static_cast<float>(rect.height) / img.rows;
    float scale  = std::min(scaleX, scaleY);
    int newW = static_cast<int>(img.cols * scale);
    int newH = static_cast<int>(img.rows * scale);

    // Resize ảnh
    cv::Mat resized;
    cv::resize(img, resized, {newW, newH}, 0, 0, cv::INTER_LINEAR);

    // Tính offset để căn giữa
    int offX = rect.x + (rect.width  - newW) / 2;
    int offY = rect.y + (rect.height - newH) / 2;

    // Copy vào canvas
    cv::Rect dstRect = {offX, offY, newW, newH};
    if (dstRect.x >= 0 && dstRect.y >= 0 &&
        dstRect.x + dstRect.width  <= m_canvas.cols &&
        dstRect.y + dstRect.height <= m_canvas.rows) {
        resized.copyTo(m_canvas(dstRect));
    }
}

// ──────────────────────────────────────────────
// drawTextCentered()
// ──────────────────────────────────────────────

/**
 * @brief Vẽ text căn giữa theo chiều ngang và dọc trong rect.
 */
void GUIApp::drawTextCentered(const std::string& text, const cv::Rect& rect,
                               const cv::Scalar& color, double scale, int thickness)
{
    int baseline = 0;
    auto sz = cv::getTextSize(text, cv::FONT_HERSHEY_SIMPLEX, scale, thickness, &baseline);
    int x = rect.x + (rect.width  - sz.width)  / 2;
    int y = rect.y + (rect.height + sz.height) / 2;
    cv::putText(m_canvas, text, {x, y},
                cv::FONT_HERSHEY_SIMPLEX, scale, color, thickness, cv::LINE_AA);
}

// ──────────────────────────────────────────────
// Mouse handling
// ──────────────────────────────────────────────

/**
 * @brief Static callback OpenCV, chuyển hướng sang instance method.
 */
void GUIApp::mouseCallback(int event, int x, int y, int /*flags*/, void* userdata)
{
    reinterpret_cast<GUIApp*>(userdata)->onMouse(event, x, y);
}

/**
 * @brief Xử lý sự kiện chuột: hover, click nút.
 */
void GUIApp::onMouse(int event, int x, int y)
{
    cv::Point pt = {x, y};

    // ── Hover update ──────────────────────────
    for (auto& btn : m_methodBtns) btn.isHovered = btn.contains(pt);
    for (auto& btn : m_metricBtns) btn.isHovered = btn.contains(pt);
    for (auto& btn : m_kBtns)      btn.isHovered = btn.contains(pt);
    for (auto& btn : m_dbBtns)     btn.isHovered = btn.contains(pt);
    m_btnLoad.isHovered   = m_btnLoad.contains(pt);
    m_btnSearch.isHovered = m_btnSearch.contains(pt);

    // Hover trên kết quả
    m_hoveredResult = -1;
    if (!m_results.empty() && x > LEFT_W) {
        int panelX = LEFT_W + 1;
        int startX = panelX + THUMB_GAP;
        int startY = TITLE_H + 35;
        int cols   = std::max(1, (WIN_W - LEFT_W - 1 - THUMB_GAP) / (THUMB_SIZE + THUMB_GAP));
        for (int i = 0; i < (int)m_results.size(); i++) {
            int col = i % cols, row = i / cols;
            int bx = startX + col * (THUMB_SIZE + THUMB_GAP) - 4;
            int by = startY + row * (THUMB_SIZE + THUMB_GAP + 35) - 4;
            cv::Rect cell = {bx, by, THUMB_SIZE + 8, THUMB_SIZE + 40};
            if (cell.contains(pt)) { m_hoveredResult = i; break; }
        }
    }

    if (event != cv::EVENT_LBUTTONDOWN) return;

    // ── Click handling ────────────────────────
    
    // Click nut Database
    for (size_t i = 0; i < m_dbBtns.size(); i++) {
        if (m_dbBtns[i].contains(pt) && m_selectedDbIdx != (int)i) {
            m_selectedDbIdx = (int)i;
            m_dbBtns[0].isSelected = (i == 0);
            m_dbBtns[1].isSelected = (i == 1);

            std::string base = getBasePath();
            std::string featuresFile = (i == 0) ? base + "data/features_TMBuD.yml" 
                                                : base + "data/features_CD.yml";
            std::string vocabDir     = (i == 0) ? base + "data/vocab_TMBuD/" 
                                                : base + "data/vocab_CD/";
            
            if (m_engine.switchDatabase(featuresFile, vocabDir)) {
                m_statusMsg = "Da chuyen sang Database: " + m_dbBtns[i].label;
                initButtons(); // cap nhat UI
                m_results.clear(); // Xoa ket qua cu
            } else {
                m_statusMsg = "Loi: Khong nap duoc " + featuresFile;
            }
            render();
            return;
        }
    }

    if (m_btnLoad.contains(pt)) {
        std::string path;
        if (openFileDialog(path)) {
            m_queryImage = cv::imread(path);
            m_queryPath  = path;
            m_results.clear();
            m_lastQueryTime = 0;
            m_statusMsg = "Da tai: " + path.substr(path.find_last_of("\\/") + 1);
        }
        return;
    }

    if (m_btnSearch.contains(pt)) {
        if (!m_queryImage.empty()) doSearch();
        else m_statusMsg = "Vui long tai anh truoc!";
        return;
    }

    // Method buttons
    for (int i = 0; i < (int)m_methodBtns.size(); i++) {
        if (m_methodBtns[i].contains(pt)) {
            for (auto& b : m_methodBtns) b.isSelected = false;
            m_methodBtns[i].isSelected = true;
            m_selectedMethodIdx = i;
            m_results.clear();
            return;
        }
    }

    // Metric buttons
    for (int i = 0; i < (int)m_metricBtns.size(); i++) {
        if (m_metricBtns[i].contains(pt)) {
            for (auto& b : m_metricBtns) b.isSelected = false;
            m_metricBtns[i].isSelected = true;
            m_selectedMetricIdx = i;
            m_results.clear();
            return;
        }
    }

    // K buttons
    std::vector<int> kValues = {3, 5, 11, 21};
    for (int i = 0; i < (int)m_kBtns.size(); i++) {
        if (m_kBtns[i].contains(pt)) {
            for (auto& b : m_kBtns) b.isSelected = false;
            m_kBtns[i].isSelected = true;
            m_selectedK = kValues[i];
            m_results.clear();
            return;
        }
    }
}

// ──────────────────────────────────────────────
// openFileDialog()
// ──────────────────────────────────────────────

/**
 * @brief Mở hộp thoại chọn file ảnh.
 *
 * Trên Windows dùng GetOpenFileNameA (Windows API native dialog).
 * Trên các hệ điều hành khác, có thể dùng zenity hoặc stdin.
 */
bool GUIApp::openFileDialog(std::string& outPath)
{
#ifdef _WIN32
    OPENFILENAMEA ofn;
    char szFile[512] = {0};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize  = sizeof(ofn);
    ofn.hwndOwner    = nullptr;
    ofn.lpstrFile    = szFile;
    ofn.nMaxFile     = sizeof(szFile);
    ofn.lpstrFilter  = "Image Files\0*.jpg;*.jpeg;*.png;*.bmp;*.tif\0All Files\0*.*\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrTitle   = "Chon anh truy van";
    ofn.Flags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_NOCHANGEDIR;

    if (GetOpenFileNameA(&ofn)) {
        outPath = szFile;
        return true;
    }
    return false;
#else
    // Fallback: nhập đường dẫn qua console
    std::cout << "Nhap duong dan anh: ";
    std::getline(std::cin, outPath);
    return !outPath.empty();
#endif
}

// ──────────────────────────────────────────────
// doSearch()
// ──────────────────────────────────────────────

/**
 * @brief Thực hiện truy vấn ảnh với cài đặt hiện tại.
 *
 * Cập nhật m_results, m_lastQueryTime, và m_statusMsg.
 */
void GUIApp::doSearch()
{
    // Lấy tên method đang chọn
    auto methods = m_engine.getAvailableMethods();
    if (methods.empty() || m_selectedMethodIdx >= (int)methods.size()) {
        m_statusMsg = "Loi: Khong co phuong phap nao.";
        return;
    }
    std::string method = methods[m_selectedMethodIdx];
    DistanceMetric metric = currentMetric();

    m_statusMsg = "Dang tim kiem...";
    render();
    cv::imshow(WIN_NAME, m_canvas);
    cv::waitKey(1);

    double elapsed = 0;
    m_results = m_engine.search(m_queryImage, method, metric, m_selectedK, elapsed);
    m_lastQueryTime = elapsed;

    std::ostringstream oss;
    oss << "Tim thay " << m_results.size() << " ket qua | "
        << method << " | "
        << m_metricBtns[m_selectedMetricIdx].label << " | "
        << std::fixed << std::setprecision(1) << elapsed << " ms";
    m_statusMsg = oss.str();
}

// ──────────────────────────────────────────────
// currentMetric()
// ──────────────────────────────────────────────

/**
 * @brief Trả về DistanceMetric tương ứng với nút đang chọn.
 */
DistanceMetric GUIApp::currentMetric() const
{
    switch (m_selectedMetricIdx) {
    case 0: return DistanceMetric::EUCLIDEAN;
    case 1: return DistanceMetric::COSINE;
    case 2: return DistanceMetric::CHI_SQUARED;
    default: return DistanceMetric::EUCLIDEAN;
    }
}
