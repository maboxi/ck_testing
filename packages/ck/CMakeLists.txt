# Concurrency Kit library
CPMAddPackage(
  NAME ck
  GITHUB_REPOSITORY concurrencykit/ck
  GIT_TAG master
)

# Header-only interface target for CK and the local demo headers.
add_library(ck_headers INTERFACE)
add_library(ck::ck ALIAS ck_headers)

target_include_directories(ck_headers INTERFACE
	${CMAKE_CURRENT_SOURCE_DIR}/port/include
	${CPM_PACKAGE_ck_SOURCE_DIR}/include
)
