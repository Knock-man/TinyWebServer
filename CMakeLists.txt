cmake_minimum_required(VERSION 3.16)
project(TinyWebServer)
set(CMAKE_CXX_STANDARD 11)
set(EXECUTABLE_OUTPUT_PATH ../bin)

#加载头文件
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/lock)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/mysql)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/log)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/threadpool)

#源文件路径
aux_source_directory(./mysql MYSQL_LIST)
aux_source_directory(./log LOG_LIST)
aux_source_directory(./threadpool THREADPOOL_LIST)

#生成可执行文件
add_executable(TinyServer main.cpp ${LOG_LIST} ${MYSQL_LIST} ${THREADPOOL_LIST})

#链接库文件
target_link_libraries(TinyServer pthread mysqlclient)

