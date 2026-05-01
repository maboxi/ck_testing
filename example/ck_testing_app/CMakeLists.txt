add_executable(ck_testing_app
    src/main.c
)

target_compile_features(ck_testing_app PRIVATE c_std_99)
target_link_libraries(ck_testing_app PRIVATE ck::ck)
target_include_directories(ck_testing_app PRIVATE
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/.packages/ck/e2a3/include
)
