cmake_minimum_required(VERSION 2.8)
project(yx_demo_main)

set(TARGET_YX_DEMO_MAIN yx_demo_main)

# Add the source in project root directory
aux_source_directory(. DIRSRCS)
# Add header file include directories
include_directories(./ ./can ./serial_link)
# Add block directories
add_subdirectory(can)
add_subdirectory(serial_link)
# set pthread lib
set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
# Target
#add_executable(yx_demo_main ${DIRSRCS})
add_executable(${TARGET_YX_DEMO_MAIN} ${DIRSRCS})
target_link_libraries(${TARGET_YX_DEMO_MAIN} can serial_link)
#target_link_libraries(yx_demo_main can serial_link PUBLIC Threads::Threads)

# install to target
install ( TARGETS ${TARGET_YX_DEMO_MAIN}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_BINDIR} )
