cmake_minimum_required(VERSION 2.8)
project (examples)

set(BASE_PROTO_DIR ${CMAKE_CURRENT_SOURCE_DIR}/proto)
file(GLOB PROTO_FILES ${BASE_PROTO_DIR}/*.proto)
FOREACH (FIL ${PROTO_FILES})
    get_filename_component(ABS_FIL ${FIL} ABSOLUTE)
    get_filename_component(FIL_WE ${FIL} NAME_WE)

    list(APPEND BAR_SRCS "${BASE_PROTO_DIR}/${FIL_WE}.pb.cc")
    list(APPEND BAR_HDRS "${BASE_PROTO_DIR}/${FIL_WE}.pb.h")

    execute_process(
            COMMAND ${PROTOBUF_PROTOC_EXECUTABLE} ${PROTO_FLAGS} --cpp_out=${BASE_PROTO_DIR} --proto_path=${BASE_PROTO_DIR} ${FIL}
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/proto
    )
ENDFOREACH ()

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR}/proto SOURCE_FILES_PROTO)

add_executable(examples example.cpp ${SOURCE_FILES_PROTO})

target_link_libraries(examples json2pb)


