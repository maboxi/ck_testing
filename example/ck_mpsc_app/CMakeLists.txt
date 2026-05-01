find_package(Threads REQUIRED)

add_executable(ck_mpsc_app
    src/mpsc_main.c
)

target_compile_features(ck_mpsc_app PRIVATE c_std_99)
target_link_libraries(ck_mpsc_app PRIVATE ck::ck Threads::Threads)
target_include_directories(ck_mpsc_app PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/.packages/ck/e2a3/include
)
