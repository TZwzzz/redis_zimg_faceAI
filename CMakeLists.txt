cmake_minimum_required(VERSION 2.6)
#创建工程
project(redis_zimg_faceAI)
set(CMAKE_CXX_FLAGS "-std=c++11")

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(/usr/local/include/hiredis)
include_directories(/home/ubuntu/spdlog/include)
link_directories(/usr/local/lib)

add_executable(app main.cpp redis_tool.cpp getUrlPic.cpp faceDetect.cpp json.cpp logs.cpp) #将程序生成可执行文件
target_link_libraries(app ${OpenCV_LIBS} hiredis curl pthread) #链接lib文件到可执行文件中