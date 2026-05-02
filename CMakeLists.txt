cmake_minimum_required(VERSION 3.15)
project(ck_testing C)

# Store CPM downloads in the repository-local .packages directory.
set(CPM_SOURCE_CACHE "${CMAKE_SOURCE_DIR}/.packages" CACHE PATH "CPM package cache directory")

# Add CPM
include(cmake/CPM.cmake)

# Cache variable to also emit assembly (.s) files for example targets
set(OUTPUT_ASM OFF CACHE BOOL "Also generate assembly (.s) for example targets")

# Add packages helper
add_subdirectory(packages)

# Assembly output helper
include(cmake/AsmOutput.cmake)

# Add example applications
add_subdirectory(example/ck_testing_app)
add_subdirectory(example/ck_mpsc_app)