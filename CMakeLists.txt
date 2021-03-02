cmake_minimum_required(VERSION 3.0.0)
project(flv_parser VERSION 0.1.0)

include(CTest)
enable_testing()

include_directories(/usr/local/include)
include_directories(${CMAKE_SOURCE_DIR}/glad/include)
link_directories(/usr/local/lib)
link_directories(${CMAKE_SOURCE_DIR}/glad/src)


add_executable(flv_parser main.cpp ${CMAKE_SOURCE_DIR}/glad/src/glad.c)

target_link_libraries(flv_parser avformat avcodec avutil glfw)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
