# ==============================================================================
# 示例程序
# ==============================================================================

# 示例: libpq_example
add_executable(libpq_example
    ${CMAKE_CURRENT_SOURCE_DIR}/libpq_example.c
)

target_link_libraries(libpq_example PRIVATE
    PostgreSQL::PostgreSQL
)

if(WIN32)
    target_link_libraries(libpq_example PRIVATE
        ws2_32.lib
        secur32.lib
    )
endif()
