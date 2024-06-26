cmake_minimum_required(VERSION 3.16)
project(stickersolve)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})

set(CMAKE_CXX_FLAGS "-no-pie -Wall -Wextra")
# set(CMAKE_CXX_FLAGS_DEBUG "-O3 -pg -g") # gprof stickersolve gmon.out
set(CMAKE_CXX_FLAGS_RELEASE "-O3")
# set(CMAKE_CXX_FLAGS_RELEASE "-Ofast -zopt -flto -fremap-arrays -mllvm -reduce-array-computations=3 -faggressive-loop-transform") # for amd compiler

if(WIN32)
  # see https://blog.kitware.com/create-dlls-on-windows-without-declspec-using-new-cmake-export-all-feature/
  set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)
endif(WIN32)

# the -I flag in gcc
include_directories(
  "./include"
  "./vendor/include"
) 

set(CMAKE_BUILD_WITH_INSTALL_RPATH on)
set(CMAKE_INSTALL_RPATH "./vendor/lib" "./lib")
link_directories(${PROJECT_SOURCE_DIR}/vendor/lib)

file(GLOB_RECURSE APP_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/*.h" "${PROJECT_SOURCE_DIR}/include/*.hpp" "${PROJECT_SOURCE_DIR}/include/*.cpp" "${PROJECT_SOURCE_DIR}/include/*.tpp")
file(GLOB_RECURSE VENDOR_SOURCES CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/vendor/src/*.h" "${PROJECT_SOURCE_DIR}/vendor/src/*.hpp" "${PROJECT_SOURCE_DIR}/vendor/src/*.cpp")

# creates ./build/bin/runner
add_executable(${PROJECT_NAME} ${APP_SOURCES} ${VENDOR_SOURCES} frontend/main.cpp)
target_link_libraries(${PROJECT_NAME} pthread -l:libboost_system.a -l:libboost_iostreams.a -l:libboost_filesystem.a)

# shared library
# set(LIBRARY_NAME World)
# add_library(${LIBRARY_NAME} SHARED ${LIB_SOURCES})
# target_link_libraries(${PROJECT_NAME} ${LIBRARY_NAME})