add_subdirectory(src)

add_custom_target(nuttx_post_build
COMMAND ${CMAKE_COMMAND} -E echo "Post-build step: Generating nuttx.elf"
COMMAND ${CMAKE_COMMAND} -E rename
        nuttx
        nuttx.elf
WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
COMMENT "Generating nuttx.elf after nuttx build"
)
add_dependencies(nuttx_post_build nuttx)
