cmake_minimum_required(VERSION 2.8)
project(BIGEUlA)
#set(CMAKE_CXX_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE ON)
set(CMAKE_CXX_FLAGS "$ENV{CXXFALGS} -rdynamic  -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/build)
set(LIB_SRC
    code/log.cpp
    code/unti.cpp
)

add_library(BIGEULA SHARED ${LIB_SRC})
#add_library(BIGEULA_static STATIC ${LIB_SRC})
#SET_TARGET_PROPERTIES (hello_static PROPERTIES OUTPUT_NAME "hello")
add_executable(test_log test/test_log.cpp)
add_dependencies(test_log BIGEULA)
target_link_libraries(test_log BIGEULA)
add_executable(test_config test/test_config.cpp)
add_dependencies(test_config BIGEULA)
target_link_libraries(test_config BIGEULA)
SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)