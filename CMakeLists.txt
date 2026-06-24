cmake_minimum_required(VERSION 3.16)
project(ImageRetrieval VERSION 1.0 LANGUAGES CXX)

# ── Chuẩn C++17 (cần cho std::filesystem) ─────────────────────────────────────
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ── Tìm OpenCV ────────────────────────────────────────────────────────────────
# Nếu OpenCV không tự phát hiện, set OpenCV_DIR thủ công:
#   cmake -DOpenCV_DIR="C:/opencv/build" ..
if(NOT DEFINED OpenCV_DIR)
    set(OpenCV_DIR "C:/opencv/build" CACHE PATH "Path to OpenCV build directory")
endif()

find_package(OpenCV REQUIRED
    COMPONENTS core imgproc imgcodecs features2d flann ml calib3d)

message(STATUS "OpenCV version: ${OpenCV_VERSION}")
message(STATUS "OpenCV libraries: ${OpenCV_LIBS}")
message(STATUS "OpenCV include: ${OpenCV_INCLUDE_DIRS}")

# ── Include paths ─────────────────────────────────────────────────────────────
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${OpenCV_INCLUDE_DIRS}
)

# ── Source files cho thư viện lõi ─────────────────────────────────────────────
set(LIB_SOURCES
    src/ColorHistogram.cpp
    src/ColorCorrelogram.cpp
    src/SIFTExtractor.cpp
    src/ORBExtractor.cpp
    src/DatabaseBuilder.cpp
    src/ImageSearchEngine.cpp
    src/GUIApp.cpp
    src/HOGExtractor.cpp
    src/LBPExtractor.cpp
    src/FusionExtractor.cpp
)

# ── Thư viện tĩnh chứa các extractor và builder ───────────────────────────────
add_library(retrieval_lib STATIC ${LIB_SOURCES})
target_link_libraries(retrieval_lib ${OpenCV_LIBS})

# ── Executable: build_features (công cụ dòng lệnh xây CSDL) ──────────────────
add_executable(build_features src/build_features.cpp)
target_link_libraries(build_features retrieval_lib ${OpenCV_LIBS})

# ── Executable: evaluate_map (công cụ đo đạc tự động) ────────────────────────
add_executable(evaluate_map src/evaluate_map.cpp)
target_link_libraries(evaluate_map retrieval_lib ${OpenCV_LIBS})

# ── Executable: evaluate_debug (công cụ debug) ────────────────────────
add_executable(evaluate_debug src/evaluate_debug.cpp)
target_link_libraries(evaluate_debug retrieval_lib ${OpenCV_LIBS})

# ── Executable: cbir_gui (GUI ứng dụng truy vấn ảnh) ──────────────────────
add_executable(cbir_gui src/main_gui.cpp)
target_link_libraries(cbir_gui retrieval_lib ${OpenCV_LIBS})

# Trên Windows: link thêm comdlg32 cho native file dialog (GetOpenFileName)
if(WIN32)
    target_link_libraries(cbir_gui comdlg32)
endif()

# ── Cài đặt Debug/Release ─────────────────────────────────────────────────────
if(MSVC)
    # Tối ưu tốc độ cho Release
    target_compile_options(retrieval_lib PRIVATE
        $<$<CONFIG:Release>:/O2 /W3>
        $<$<CONFIG:Debug>:/Od /W4>
    )
    target_compile_options(build_features PRIVATE
        $<$<CONFIG:Release>:/O2 /W3>
        $<$<CONFIG:Debug>:/Od /W4>
    )
    target_compile_options(evaluate_map PRIVATE
        $<$<CONFIG:Release>:/O2 /W3>
        $<$<CONFIG:Debug>:/Od /W4>
    )
    # Tắt cảnh báo về fopen_s / strcpy_s (không cần thiết cho project này)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
endif()

# ── Thông tin sau khi configure ───────────────────────────────────────────────
message(STATUS "")
message(STATUS "===== ImageRetrieval Configuration =====")
message(STATUS "  Build type   : ${CMAKE_BUILD_TYPE}")
message(STATUS "  C++ standard : C++${CMAKE_CXX_STANDARD}")
message(STATUS "  OpenCV dir   : ${OpenCV_DIR}")
message(STATUS "  Install dir  : ${CMAKE_INSTALL_PREFIX}")
message(STATUS "========================================")
