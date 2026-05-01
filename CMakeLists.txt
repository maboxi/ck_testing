cmake_minimum_required(VERSION 3.15)
project(ck_testing C)

# Store CPM downloads in the repository-local .packages directory.
set(CPM_SOURCE_CACHE "${CMAKE_SOURCE_DIR}/.packages" CACHE PATH "CPM package cache directory")

# Add CPM
include(cmake/CPM.cmake)

# Add packages helper
add_subdirectory(packages)

# Basic application that demonstrates the CK ring buffer.
add_executable(ck_testing_app
	src/main.c
	src/ringbuffer_app.c
)

target_compile_features(ck_testing_app PRIVATE c_std_99)
target_link_libraries(ck_testing_app PRIVATE ck::ck)