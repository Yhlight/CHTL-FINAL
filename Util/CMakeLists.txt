# Create a static library for the utility functions
add_library(UtilLib STATIC
    StringUtil/StringUtil.cpp
    FileSystem/FileSystem.cpp
)

# Publicly expose the header files of the UtilLib library
target_include_directories(UtilLib PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)