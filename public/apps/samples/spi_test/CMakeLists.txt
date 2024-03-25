cmake_minimum_required(VERSION 2.8.9)

set(TARGET_SPI_TEST spi_test)

set(SPI_TEST_SOURCES
    main.c
)

# set global variables
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable (${TARGET_SPI_TEST} ${SPI_TEST_SOURCES})
#target_link_libraries(${TARGET_SPI_TEST} spi_test)

# install to target
install ( TARGETS ${TARGET_SPI_TEST}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_BINDIR} )
