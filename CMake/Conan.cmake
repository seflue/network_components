macro(run_conan)
# Download automatically, you can also just copy the conan.cmake file
if(NOT EXISTS "${CMAKE_BINARY_DIR}/conan.cmake")
message(
STATUS
"Downloading conan.cmake from https://github.com/conan-io/cmake-conan")
file(DOWNLOAD "https://raw.githubusercontent.com/conan-io/cmake-conan/0.18.1/conan.cmake"
"${CMAKE_BINARY_DIR}/conan.cmake")
endif()

include(${CMAKE_BINARY_DIR}/conan.cmake)

conan_cmake_configure(
  REQUIRES poco/1.12.4 grpc/1.50.1 abseil/20220623.0 openssl/3.1.0
  GENERATORS cmake_find_package cmake_paths
)
conan_cmake_autodetect(settings)
conan_cmake_install(
  PATH_OR_REFERENCE .
  BUILD missing
  SETTINGS ${settings}
)
endmacro()