# Utility to add assembly (.s) output for a target's source files
# Uses the target's own include directories and those from its linked libraries.
# Usage: add_assembly_output(<target>)
function(add_assembly_output target)
    if(NOT OUTPUT_ASM)
        return()
    endif()

    get_target_property(_SRCS ${target} SOURCES)
    if(NOT _SRCS)
        message(WARNING "Target ${target} has no SOURCES; skipping assembly output")
        return()
    endif()

    # Collect include directories from target and linked libraries
    set(_includes)
    
    # Get direct include directories of the target
    get_target_property(_target_includes ${target} INCLUDE_DIRECTORIES)
    if(_target_includes)
        list(APPEND _includes ${_target_includes})
    endif()
    
    # Get include directories from interface-linked libraries
    get_target_property(_link_libs ${target} LINK_LIBRARIES)
    if(_link_libs)
        foreach(_lib IN LISTS _link_libs)
            if(TARGET ${_lib})
                get_target_property(_lib_includes ${_lib} INTERFACE_INCLUDE_DIRECTORIES)
                if(_lib_includes)
                    list(APPEND _includes ${_lib_includes})
                endif()
            endif()
        endforeach()
    endif()
    
    # Convert includes to -I flags
    set(_include_flags)
    foreach(_inc IN LISTS _includes)
        list(APPEND _include_flags "-I${_inc}")
    endforeach()

    foreach(_src IN LISTS _SRCS)
        get_filename_component(_srcpath "${_src}" ABSOLUTE BASE_DIR ${CMAKE_CURRENT_SOURCE_DIR})
        get_filename_component(_name "${_srcpath}" NAME_WE)
        set(_asm_out "${CMAKE_CURRENT_BINARY_DIR}/${_name}.s")
        add_custom_command(
            TARGET ${target}
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E echo "Generating assembly: ${_asm_out}"
            COMMAND ${CMAKE_C_COMPILER} -S -o "${_asm_out}" "${_srcpath}" ${_include_flags} ${CMAKE_C_FLAGS}
            VERBATIM
        )
    endforeach()
endfunction()
