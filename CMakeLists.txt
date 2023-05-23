cmake_minimum_required(VERSION 3.22.1)

project(network_components C CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 20)
set(USER_CONAN_CMAKE false)

if (${USE_CONAN_CMAKE})
    include(CMake/Conan.cmake)
    run_conan()
endif ()

# GRPC Setup
find_package(Protobuf REQUIRED)

if (Protobuf_FOUND)
    message(STATUS "..... protobuf found ......")
    include_directories(${Protobuf_INCLUDE_DIRS})
endif ()
find_package(gRPC REQUIRED)
if (gRPC_FOUND)
    message(STATUS "..... grpc found ......")
    include_directories(${gRPC_INCLUDE_DIRS})
endif ()
find_package(absl REQUIRED)
if (absl_FOUND)
    message(STATUS "..... absl found ......")
    include_directories(${absl_INCLUDE_DIRS})
endif ()
add_compile_definitions(SPDLOG_USE_STD_FORMAT)
find_package(spdlog REQUIRED)
if (spdlog_FOUND)
    message(STATUS "..... spdlog found ......")
    include_directories(${absl_INCLUDE_DIRS})
endif ()

set(_PROTOBUF_LIBPROTOBUF protobuf::libprotobuf)
set(_GRPC_REFLECTION gRPC::grpc++_reflection)
set(_GRPC_UNSECURE gRPC::grpc++_unsecure)
set(_GRPC_GRPCPP gRPC::grpc++)

add_library(proto-objects OBJECT "${CMAKE_CURRENT_LIST_DIR}/proto/controlplane.proto")

target_link_libraries(proto-objects PUBLIC ${_PROTOBUF_LIBPROTOBUF} ${_GRPC_UNSECURE} ${_GRPC_REFLECTION} ${_GRPC_GRPCCPP})

set(PROTO_BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/generated")
set(PROTO_IMPORT_DIRS "${CMAKE_CURRENT_LIST_DIR}/proto")

file(MAKE_DIRECTORY "${PROTO_BINARY_DIR}")

protobuf_generate(
        TARGET proto-objects
        IMPORT_DIRS ${PROTO_IMPORT_DIRS}
        PROTOC_OUT_DIR "${PROTO_BINARY_DIR}")

protobuf_generate(
        TARGET proto-objects
        LANGUAGE grpc
        GENERATE_EXTENSIONS .grpc.pb.h .grpc.pb.cc
        PLUGIN "protoc-gen-grpc=\$<TARGET_FILE:gRPC::grpc_cpp_plugin>"
        IMPORT_DIRS ${PROTO_IMPORT_DIRS}
        PROTOC_OUT_DIR "${PROTO_BINARY_DIR}")

target_include_directories(proto-objects PUBLIC "$<BUILD_INTERFACE:${PROTO_BINARY_DIR}>")

# Poco Setup
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/build")
list(APPEND CMAKE_PREFIX_PATH "${CMAKE_CURRENT_LIST_DIR}/build")

find_package(Poco REQUIRED COMPONENTS Foundation Util Net)
if (Poco_FOUND)
    include_directories(${Poco_INCLUDE_DIRS})
endif ()

set(poco-libs Poco::Foundation Poco::Util Poco::Net)

set(grpc-libs
        proto-objects
        absl::flags
        absl::flags_parse
        ${_GRPC_GRPCPP}
        ${_GRPC_REFLECTION}
        ${_PROTOBUF_LIBPROTOBUF}
        )

set(UE_SOURCE_DIR src/user_equipment)
set(BS_SOURCE_DIR src/base_station)
set(HSS_SOURCE_DIR src/home_subscriber_server)
set(UTILS_SOURCE_DIR src/utils)

set(TESTDATA_DIR ${CMAKE_CURRENT_SOURCE_DIR}/testdata)
set(UE_WORKING_DIR ${CMAKE_CURRENT_BINARY_DIR}/user_equipment_wd)
set(BS_WORKING_DIR ${CMAKE_CURRENT_BINARY_DIR}/base_station_wd)

file(MAKE_DIRECTORY ${UE_WORKING_DIR})
file(MAKE_DIRECTORY ${BS_WORKING_DIR})

file(GLOB UE_SOURCE_FILES ${UE_SOURCE_DIR}/*.h ${UE_SOURCE_DIR}/*.cpp)
file(GLOB BS_SOURCE_FILES ${BS_SOURCE_DIR}/*.h ${BS_SOURCE_DIR}/*.cpp)
file(GLOB HSS_SOURCE_FILES ${HSS_SOURCE_DIR}/*.h ${HSS_SOURCE_DIR}/*.cpp)
file(GLOB UTILS_SOURCE_FILES ${UTILS_SOURCE_DIR}/*.h ${UTILS_SOURCE_DIR}/*.cpp)

file(COPY ${TESTDATA_DIR} DESTINATION ${UE_WORKING_DIR})

add_executable(user_equipment ${UE_SOURCE_FILES})
set_target_properties(user_equipment PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${UE_WORKING_DIR})
add_executable(base_station ${BS_SOURCE_FILES})
set_target_properties(base_station PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${BS_WORKING_DIR})
add_executable(home_subscriber_server ${HSS_SOURCE_FILES})
add_library(utils ${UTILS_SOURCE_DIR})
set_target_properties(utils PROPERTIES LINKER_LANGUAGE CXX)


file(GENERATE OUTPUT debug_genexpr CONTENT "$<JOIN:${Poco_INCLUDE_DIRS}, >")

target_include_directories(user_equipment PRIVATE ${Poco_INCLUDE_DIRS} ${UTILS_SOURCE_DIR} ${spdlog_INCLUDE_DIRS})
target_include_directories(base_station PRIVATE ${Poco_INCLUDE_DIRS} ${UTILS_SOURCE_DIR} ${spdlog_INCLUDE_DIRS})
target_include_directories(utils PRIVATE ${spdlog_INCLUDE_DIRS})

foreach (_target
        user_equipment
        base_station)
    target_link_libraries(${_target} ${utils} ${poco-libs} ${grpc-libs})
endforeach ()

# Turn on some compiler warnings
foreach (_target
        user_equipment
        base_station
        utils
        )
    target_compile_options(${_target} PRIVATE
            $<$<OR:$<CXX_COMPILER_ID:Clang>,$<CXX_COMPILER_ID:AppleClang>,$<CXX_COMPILER_ID:GNU>>:
            -Wall -Werror -Wextra>
            $<$<CXX_COMPILER_ID:MSVC>:
            /W4>)
endforeach ()
