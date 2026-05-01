# Automatically add all subdirectories containing CMakeLists.txt
file(GLOB subdirs RELATIVE ${CMAKE_CURRENT_SOURCE_DIR} "${CMAKE_CURRENT_SOURCE_DIR}/*")

foreach(subdir ${subdirs})
  set(subdir_path "${CMAKE_CURRENT_SOURCE_DIR}/${subdir}")
  
  if(IS_DIRECTORY "${subdir_path}" AND EXISTS "${subdir_path}/CMakeLists.txt")
    add_subdirectory(${subdir})
  endif()
endforeach()
