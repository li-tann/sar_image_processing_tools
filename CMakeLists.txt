cmake_minimum_required(VERSION 3.0.0)
project(SAR_TOOLS VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS off)
set(CMAKE_CXX_STANDARD_REQUIRED on)

include(CTest)
enable_testing()

find_package(GDAL CONFIG REQUIRED)
find_package(spdlog CONFIG REQUIRED)
find_package(fmt CONFIG REQUIRED)
find_package(OpenMP REQUIRED)
find_package(FFTW3f CONFIG REQUIRED)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

set(EXE_LIST)

#debug config 测试
add_executable(debug_config_test debug_config_test.cpp)

# float格式（不止）的二进制影像转换为tif并输出
add_executable(binary_to_tif binary_img_to_tif.cpp template_binary_img_io.h)
target_link_libraries(binary_to_tif PRIVATE GDAL::GDAL)
target_link_libraries(binary_to_tif PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
set(EXE_LIST ${EXE_LIST} binary_to_tif)

# tif转换为二进制格式
add_executable(tif_to_binary tif_to_binary.cpp template_binary_img_io.h)
target_link_libraries(tif_to_binary PRIVATE GDAL::GDAL)
target_link_libraries(tif_to_binary PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
set(EXE_LIST ${EXE_LIST} tif_to_binary)

# 打印sar影像
add_executable(print_sar_image print_sar_image.cpp template_sarImage_process.h datatype.cpp)
target_include_directories(print_sar_image 
							INTERFACE 
							${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(print_sar_image PRIVATE GDAL::GDAL)
target_link_libraries(print_sar_image PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(print_sar_image PRIVATE fmt::fmt)
set(EXE_LIST ${EXE_LIST} print_sar_image)

# 融合sar影像
add_executable(merge_sar_image merge_sar_image.cpp template_sarImage_process.h)
target_link_libraries(merge_sar_image PRIVATE GDAL::GDAL)
target_link_libraries(merge_sar_image PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
set(EXE_LIST ${EXE_LIST} merge_sar_image)

# 从数据B中复制一块区域填充到A数据中(几何关系一致)
add_executable(geoimage_copy_ref_paste_to_target copy_Geoimage_paste.cpp datatype.h datatype.cpp)
target_link_libraries(geoimage_copy_ref_paste_to_target PRIVATE GDAL::GDAL)
target_link_libraries(geoimage_copy_ref_paste_to_target PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(geoimage_copy_ref_paste_to_target PRIVATE fmt::fmt)
set(EXE_LIST ${EXE_LIST} geoimage_copy_ref_paste_to_target)


# rolling guidance 滤波
add_executable(rolling_guidance_filter rolling_guidance.cpp datatype.h datatype.cpp)
target_link_libraries(rolling_guidance_filter PRIVATE GDAL::GDAL)
target_link_libraries(rolling_guidance_filter PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(rolling_guidance_filter PRIVATE fmt::fmt)
set(EXE_LIST ${EXE_LIST} rolling_guidance_filter)
if(OpenMP_FOUND)
    target_link_libraries(rolling_guidance_filter PRIVATE OpenMP::OpenMP_CXX)
else()
    message(FATAL_ERROR "openmp not found!")
endif()

#复数影像格式转换
add_executable(complex_transto complex_transto.cpp datatype.h datatype.cpp)
target_link_libraries(complex_transto PRIVATE GDAL::GDAL)
target_link_libraries(complex_transto PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(complex_transto PRIVATE fmt::fmt)
set(EXE_LIST ${EXE_LIST} complex_transto)

#滤波评价
add_executable(filter_evaluation filter_evaluation.cpp datatype.h datatype.cpp)
target_link_libraries(filter_evaluation PRIVATE GDAL::GDAL)
target_link_libraries(filter_evaluation PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(filter_evaluation PRIVATE fmt::fmt)
set(EXE_LIST ${EXE_LIST} filter_evaluation)


#goldstein滤波
add_executable(goldstein goldstein.cpp datatype.h datatype.cpp)
target_link_libraries(goldstein PRIVATE GDAL::GDAL)
target_link_libraries(goldstein PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(goldstein PRIVATE fmt::fmt)
target_link_libraries(goldstein PRIVATE FFTW3::fftw3f)
set(EXE_LIST ${EXE_LIST} goldstein)
if(OpenMP_FOUND)
    target_link_libraries(goldstein PRIVATE OpenMP::OpenMP_CXX)
else()
    message(FATAL_ERROR "openmp not found!")
endif()

#pseudo-coherence 伪相干性计算
add_executable(pseudo_correlation pseudo_correlation.cpp datatype.h datatype.cpp)
target_link_libraries(pseudo_correlation PRIVATE GDAL::GDAL)
target_link_libraries(pseudo_correlation PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(pseudo_correlation PRIVATE fmt::fmt)
set(EXE_LIST ${EXE_LIST} pseudo_correlation)
if(OpenMP_FOUND)
    target_link_libraries(pseudo_correlation PRIVATE OpenMP::OpenMP_CXX)
else()
    message(FATAL_ERROR "openmp not found!")
endif()

#goldstein-baran滤波
add_executable(baran goldstein_baran.cpp datatype.h datatype.cpp)
target_link_libraries(baran PRIVATE GDAL::GDAL)
target_link_libraries(baran PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(baran PRIVATE fmt::fmt)
target_link_libraries(baran PRIVATE FFTW3::fftw3f)
set(EXE_LIST ${EXE_LIST} baran)
if(OpenMP_FOUND)
    target_link_libraries(baran PRIVATE OpenMP::OpenMP_CXX)
else()
    message(FATAL_ERROR "openmp not found!")
endif()

#goldstein-zhao滤波
add_executable(zhao goldstein_zhao.cpp datatype.h datatype.cpp)
target_link_libraries(zhao PRIVATE GDAL::GDAL)
target_link_libraries(zhao PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(zhao PRIVATE fmt::fmt)
target_link_libraries(zhao PRIVATE FFTW3::fftw3f)
set(EXE_LIST ${EXE_LIST} zhao)
if(OpenMP_FOUND)
    target_link_libraries(zhao PRIVATE OpenMP::OpenMP_CXX)
else()
    message(FATAL_ERROR "openmp not found!")
endif()

#interf_spectrum_filter滤波
add_executable(interf_spectrum_filter interf_spectrum_filter.cpp datatype.h datatype.cpp)
target_link_libraries(interf_spectrum_filter PRIVATE GDAL::GDAL)
target_link_libraries(interf_spectrum_filter PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(interf_spectrum_filter PRIVATE fmt::fmt)
target_link_libraries(interf_spectrum_filter PRIVATE FFTW3::fftw3f)
set(EXE_LIST ${EXE_LIST} interf_spectrum_filter)
if(OpenMP_FOUND)
    target_link_libraries(interf_spectrum_filter PRIVATE OpenMP::OpenMP_CXX)
else()
    message(FATAL_ERROR "openmp not found!")
endif()

#interf 很简单的干涉
add_executable(interf interf.cpp datatype.h datatype.cpp)
target_link_libraries(interf PRIVATE GDAL::GDAL)
target_link_libraries(interf PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(interf PRIVATE fmt::fmt)
set(EXE_LIST ${EXE_LIST} interf)
if(OpenMP_FOUND)
    target_link_libraries(interf PRIVATE OpenMP::OpenMP_CXX)
else()
    message(FATAL_ERROR "openmp not found!")
endif()

add_executable(phase_simulation phase_simulation.cpp datatype.h datatype.cpp)
target_link_libraries(phase_simulation PRIVATE GDAL::GDAL)
target_link_libraries(phase_simulation PRIVATE spdlog::spdlog spdlog::spdlog_header_only)
target_link_libraries(phase_simulation PRIVATE fmt::fmt)
set(EXE_LIST ${EXE_LIST} phase_simulation)

# add "cmake.configureSettings": {"CMAKE_BUILD_TYPE":"${buildType}"} in setting.json, when ${CMAKE_BUILD_TYPE} is ""
message(STATUS "cmake build type: " ${CMAKE_BUILD_TYPE})
file(MAKE_DIRECTORY ${CMAKE_CURRENT_LIST_DIR}/build/${CMAKE_BUILD_TYPE})
file(COPY_FILE ${CMAKE_CURRENT_LIST_DIR}/proj.db ${CMAKE_CURRENT_LIST_DIR}/build/${CMAKE_BUILD_TYPE}/proj.db ONLY_IF_DIFFERENT)


# INSTALL
set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_LIST_DIR}/install)

# message("exe_list:" "${EXE_LIST}")
install(TARGETS ${EXE_LIST}
        CONFIGURATIONS Release
        RUNTIME DESTINATION binary
        LIBRARY DESTINATION library
        ARCHIVE DESTINATION library
)

# GET_RUNTIME_DEPENDENCIES 可以导入第三方库, 但没有摸清楚怎么使用, 从网上找了个临时替代的方法, 如下所示
file(GLOB DLLS "${CMAKE_BINARY_DIR}/${CMAKE_BUILD_TYPE}/*.dll") # 假设DLLs在构建目录中
install(FILES ${DLLS} DESTINATION binary CONFIGURATIONS Release)
# message("DLLS: ${DLLS}")

file(GLOB scripts "scripts/*.ps1" "scripts/*.bat") # 假设DLLs在构建目录中
install(FILES ${scripts} DESTINATION binary CONFIGURATIONS Release)
# message("scripts: ${scripts}")

install(FILES ${CMAKE_CURRENT_LIST_DIR}/proj.db DESTINATION binary CONFIGURATIONS Release)
install(FILES ${CMAKE_CURRENT_LIST_DIR}/vcruntime140d.dll ${CMAKE_CURRENT_LIST_DIR}/vcruntime140_1d.dll DESTINATION binary/vc CONFIGURATIONS Release)