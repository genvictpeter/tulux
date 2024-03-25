# set target name
set(TARGET_NAME rilutils)

include_directories(BEFORE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

# generate c bindings from proto files
find_package(Nanopb REQUIRED)
nanopb_generate_cpp(PROTO_SRCS PROTO_HDRS proto/sap-api.proto)
set_source_files_properties(${PROTO_SRCS} ${PROTO_HDRS} PROPERTIES GENERATED TRUE)

set(SOURCE_FILES ${PROTO_SRCS} ${PROTO_HDRS} librilutils.c record_stream.c)

add_library (${TARGET_NAME} SHARED ${SOURCE_FILES})
set_target_properties(${TARGET_NAME} PROPERTIES
                      VERSION 1.0.0
                      SOVERSION 1)

install ( TARGETS ${TARGET_NAME}
          RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
          ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
          LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} )

install( FILES
        ${PROTO_HDRS}
        DESTINATION include/telephony/ril/librilutils/proto )