function(RAFTCPP_DEFINE_TEST)
    set(src_count 0)
    set(link_count 0)
    set(name_arg 0)
    set(current_arg 0)
    while (current_arg LESS ${ARGC})
        
        math(EXPR current_arg "${current_arg} + 1")
        # list all source files
        if ("x${ARGV${current_arg}}" STREQUAL "xSRC_FILES")
            while (current_arg LESS ${ARGC})           
                math(EXPR current_arg "${current_arg} + 1")                  
                if ("x${ARGV${current_arg}}" STREQUAL "xLINKS")
                    break()
                endif()              
                if (NOT current_arg LESS ${ARGC})
                    break()
                endif()
                set(cur_src ${src_count})
                set(src_file_${cur_src}_list "${ARGV${current_arg}}")
                math(EXPR src_count "${src_count} + 1")          
            endwhile()
        endif()
        # list all target link libraries
        if ("x${ARGV${current_arg}}" STREQUAL "xLINKS")        
            while (current_arg LESS ${ARGC})            
                math(EXPR current_arg "${current_arg} + 1")            
                if (NOT current_arg LESS ${ARGC})
                    break()
                endif()
                set(cur_link ${link_count})
                set(link_lib_${cur_link}_list "${ARGV${current_arg}}")
                math(EXPR link_count "${link_count} + 1")
                if (NOT current_arg LESS ${ARGC})
                    break()
                endif()            
            endwhile()
        endif()
    endwhile()
    set(src_index 0)    
    while(src_index LESS ${src_count})
        list(APPEND src_files "${src_file_${src_index}_list}")
        math(EXPR src_index "${src_index} + 1")
    endwhile()

    set(link_index 0)    
    while(link_index LESS ${link_count})
        list(APPEND link_libs "${link_lib_${link_index}_list}" ${GTEST_STATIC_LIB} ${GTEST_MAIN_STATIC_LIB} ${GMOCK_MAIN_STATIC_LIB})
        math(EXPR link_index "${link_index} + 1")
    endwhile()

    list(APPEND default_deps node_lib gflags_ep doctest_ep rest_rpc_ep common_lib googletest_ep)
    add_executable(${ARGV${name_arg}} ${src_files})
    add_dependencies(${ARGV${name_arg}} ${default_deps})
    target_link_libraries(${ARGV${name_arg}} ${link_libs})
endfunction(RAFTCPP_DEFINE_TEST)
