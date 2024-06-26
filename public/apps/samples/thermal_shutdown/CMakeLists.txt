cmake_minimum_required(VERSION 2.8.9)

set(TARGET_THERMALSHUDOWN_SAMPLE_APP thermal_shutdown_sample_app)

set(THERMALSHUDOWN_SAMPLE_APP_SOURCES
    ThermalShutdownSampleApp.cpp
)
macro(SYSR_INCLUDE_DIR subdir)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -I =/usr/include/${subdir}")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I =/usr/include/${subdir}")
endmacro()

# add these sub-folders from /usr/include/<subdir>
SYSR_INCLUDE_DIR(telux)

include_directories(BEFORE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# set global variables
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror")

add_executable (${TARGET_THERMALSHUDOWN_SAMPLE_APP} ${THERMALSHUDOWN_SAMPLE_APP_SOURCES})
target_link_libraries(${TARGET_THERMALSHUDOWN_SAMPLE_APP} telux_therm)

# install to target
install ( TARGETS ${TARGET_THERMALSHUDOWN_SAMPLE_APP}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_BINDIR} )
