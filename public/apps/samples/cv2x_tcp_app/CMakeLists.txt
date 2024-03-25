cmake_minimum_required(VERSION 2.8.9)
#配置C++编译器
set(CMAKE_CXX_COMPILER /usr/local/oecore-x86_64/sysroots/x86_64-oesdk-linux/usr/bin/aarch64-oe-linux/aarch64-oe-linux-g++)
#配置C编译器
set(CMAKE_C_COMPILER /usr/local/oecore-x86_64/sysroots/x86_64-oesdk-linux/usr/bin/aarch64-oe-linux/aarch64-oe-linux-gcc)
#配置系统路径
set(CMAKE_SYSROOT /usr/local/oecore-x86_64/sysroots/aarch64-oe-linux)
#配置库文件头文件搜索路径
set(CMAKE_FIND_ROOT_PATH  /usr/local/oecore-x86_64/sysroots/aarch64-oe-linux)
SET(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
SET(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
SET(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(TARGET_CV2X_TCP_APP cv2x_tcp_app)

set(CV2X_TCP_SOURCES
    Cv2xTcpApp.cpp
)

# set global variables
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable (${TARGET_CV2X_TCP_APP} ${CV2X_TCP_SOURCES})
target_link_libraries(${TARGET_CV2X_TCP_APP} telux_cv2x pthread)

# install to target
install ( TARGETS ${TARGET_CV2X_TCP_APP}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_BINDIR} )
