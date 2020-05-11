cmake_minimum_required(VERSION 3.16)

option(EXTERNAL_PICOJSON "Use find_package() to locate the picojson header" OFF)

project(json2pb)

set(BUILD_SHARED_LIBS ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)

set(JSON2PB_INCLUDE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/include)

if (EXTERNAL_PICOJSON)
    find_package(picojson REQUIRED)
    set(PICO_HEADER ${JSON2PB_INCLUDE_PATH}/picojson/picojson.h)
endif ()

find_package(Protobuf REQUIRED)

set(JSON2PB_HEADERS ${JSON2PB_INCLUDE_PATH}/json2pb/json2pb.h)

set(JSON2PB_SOURCE ${JSON2PB_INCLUDE_PATH}/json2pb/json2pb.cpp)

include_directories(${JSON2PB_INCLUDE_PATH})

add_library(
        json2pb
        ${PROTO_SRCS}
        ${PROTO_HDRS}
        ${PICO_HEADER}
        ${JSON2PB_HEADERS}
        ${JSON2PB_SOURCE}
)

target_link_libraries(
        json2pb
        ${PROTOBUF_LIBRARIES}
        #protobuf
)


add_subdirectory(examples)
