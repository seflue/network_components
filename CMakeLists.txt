cmake_minimum_required(VERSION 3.22.1)

project(network_components C CXX)

list(APPEND CMAKE_MODULE_PATH ${CMAKE_BINARY_DIR})
list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR})

set(CMAKE_CXX_STANDARD 20)
set(USER_CONAN_CMAKE false)

if(${USE_CONAN_CMAKE})
  include(CMake/Conan.cmake)
  run_conan()
endif()

# GRPC Setup
find_package(Protobuf REQUIRED)

if(Protobuf_FOUND)
    message(STATUS "..... protobuf found ......")
    include_directories(${Protobuf_INCLUDE_DIRS})
endif()
find_package(gRPC REQUIRED)
if(gRPC_FOUND)
    message(STATUS "..... grpc found ......")
    include_directories(${gRPC_INCLUDE_DIRS})
endif()
find_package(absl REQUIRED)
if(absl_FOUND)
    message(STATUS "..... grpc found ......")
    include_directories(${absl_INCLUDE_DIRS})
endif()

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
if(Poco_FOUND)
    include_directories(${Poco_INCLUDE_DIRS})
endif()

set(poco-libs Poco::Foundation Poco::Util Poco::Net)

set(grpc-libs
  proto-objects
  absl::flags
  absl::flags_parse
  ${_GRPC_GRPCPP}
  ${_GRPC_REFLECTION}
  ${_PROTOBUF_LIBPROTOBUF}
)

set(UE_SOURCE_DIR user_equipment)
file(GLOB UE_FILES ${SOURCE_DIR}/*.h ${SOURCE_DIR}/*.cpp)

add_executable(user_equipment user_equipment/main.cpp
  user_equipment/UserEquipment.h user_equipment/UserEquipment.cpp
  user_equipment/ControlClient.h user_equipment/ControlClient.cpp
        user_equipment/DataHandler.h user_equipment/DataHandler.cpp
  user_equipment/DataSender.h user_equipment/DataSender.cpp
        utils/CoinOrDice.h user_equipment/DataObjects.h)
add_executable(base_station base_station/main.cpp utils/CoinOrDice.h)
add_executable(home_subscriber_server home_subscriber_server/main.cpp)
file(GENERATE OUTPUT debug_genexpr CONTENT "$<JOIN:${Poco_INCLUDE_DIRS}, >")

#include_directories(${Poco_INCLUDES})
target_include_directories(user_equipment PUBLIC ${Poco_INCLUDE_DIRS})
target_include_directories(base_station PUBLIC ${Poco_INCLUDE_DIRS})

get_target_property(poco_dirs user_equipment INCLUDE_DIRS)

foreach(_target
  user_equipment
  base_station)
target_link_libraries(${_target} ${poco-libs} ${grpc-libs})
endforeach()

#target_link_libraries(user_equipment
#  Poco::Foundation Poco::Util Poco::Net
#  $<JOIN:${grpc-libs}, >
#)