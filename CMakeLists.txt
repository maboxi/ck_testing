cmake_minimum_required(VERSION 3.15)
project(ck_testing C)

# Store CPM downloads in the repository-local .packages directory.
set(CPM_SOURCE_CACHE "${CMAKE_SOURCE_DIR}/.packages" CACHE PATH "CPM package cache directory")

# Add CPM
include(cmake/CPM.cmake)

# Add packages helper
add_subdirectory(packages)

# Add example applications
add_subdirectory(example/ck_testing_app)