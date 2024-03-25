include_directories(BEFORE
        ${CMAKE_SOURCE_DIR}/mocks
        )

install( FILES
        Parcel2.h
        DESTINATION include/binder
        )

set( TARGET_PARCEL parcel )

set( PARCEL_SOURCES
        Parcel2.cpp)

# build library
add_library ( ${TARGET_PARCEL} SHARED
        ${PARCEL_SOURCES}
        )

set_target_properties(${TARGET_PARCEL} PROPERTIES
        VERSION 1.0.0
        SOVERSION 1)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# install to target
install ( TARGETS ${TARGET_PARCEL}
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )
