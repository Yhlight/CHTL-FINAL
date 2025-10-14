cmake_minimum_required(VERSION 3.20)

# 创建Catch2接口库
add_library(Catch2::Catch2WithMain INTERFACE IMPORTED)

# 设置包含目录
target_include_directories(Catch2::Catch2WithMain INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# 设置C++20标准
target_compile_features(Catch2::Catch2WithMain INTERFACE cxx_std_20)

# 设置编译定义
target_compile_definitions(Catch2::Catch2WithMain INTERFACE CATCH_CONFIG_FAST_COMPILE)