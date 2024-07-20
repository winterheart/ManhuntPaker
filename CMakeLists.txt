cmake_minimum_required(VERSION 3.23)
project(ManhuntPaker VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

configure_file(manhuntpaker_version.h.in manhuntpaker_version.h)

add_executable(manhuntpaker
  crc32.cpp
  main.cpp
)
target_include_directories(manhuntpaker PUBLIC "${PROJECT_BINARY_DIR}" "${PROJECT_SOURCE_DIR}")
target_link_options(manhuntpaker PRIVATE
  $<$<PLATFORM_ID:Windows>:-static>
  $<$<PLATFORM_ID:Windows>:-s>
)

install(TARGETS manhuntpaker DESTINATION .)
install(FILES LICENSE README.md DESTINATION .)

set(CPACK_GENERATOR "ZIP")
include(CPack)
