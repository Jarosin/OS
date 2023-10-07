cmake_minimum_required(VERSION 3.0.0)
project(Levenshtein VERSION 0.1.0 LANGUAGES C CXX)

include(CTest)
enable_testing()

file(GLOB SRCS *.c *.h )

add_executable(os_lab_2 ${SRCS})

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
