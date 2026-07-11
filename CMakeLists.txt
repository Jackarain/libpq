cmake_minimum_required(VERSION 3.15)
project(libpq C)

set(CMAKE_C_STANDARD 99)

# ==============================================================================
# 1. 基础源码定义 (所有平台公用)
# ==============================================================================
set(LIBPQ_SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fe-auth-oauth.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fe-auth-scram.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fe-auth.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fe-cancel.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fe-connect.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fe-exec.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fe-lobj.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fe-misc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fe-print.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fe-protocol3.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fe-secure.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/fe-trace.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/legacy-pqsignal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/libpq-events.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/pqexpbuffer.c
)

# ==============================================================================
# 2. 平台特异性处理 (Windows / macOS / Linux)
# ==============================================================================
if(WIN32)
    # Windows 专属源码
    list(APPEND LIBPQ_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pthread-win32.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/win32.c
    )
endif()

# ==============================================================================
# 3. 可选依赖检测 (SSL / GSSAPI)
# ==============================================================================
# SSL 支持 (对应 if ssl.found())
option(ENABLE_SSL "Enable SSL support" ON)
if(ENABLE_SSL)
    find_package(OpenSSL REQUIRED)
    list(APPEND LIBPQ_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fe-secure-common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fe-secure-openssl.c
    )
endif()

# GSSAPI 支持 (对应 if gssapi.found())
option(ENABLE_GSSAPI "Enable GSSAPI support" OFF)
if(ENABLE_GSSAPI)
    find_package(GSSAPI REQUIRED) # 需要 CMake 带有 FindGSSAPI 模块，或手动指定路径
    list(APPEND LIBPQ_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fe-gssapi-common.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/fe-secure-gssapi.c
    )
endif()

# ==============================================================================
# 4. 包含路径与编译器参数
# ==============================================================================
set(LIBPQ_INC
    ${CMAKE_CURRENT_SOURCE_DIR}/include
    ${CMAKE_CURRENT_SOURCE_DIR}/port
    ${CMAKE_CURRENT_SOURCE_DIR}/interfaces/libpq
)

if(WIN32)
    list(APPEND LIBPQ_INC
        ${CMAKE_CURRENT_SOURCE_DIR}/win32_inc/include
        ${CMAKE_CURRENT_SOURCE_DIR}/include/port/win32
        ${CMAKE_CURRENT_SOURCE_DIR}/include/port/win32_msvc
    )
elseif(APPLE)
    list(APPEND LIBPQ_INC
        ${CMAKE_CURRENT_SOURCE_DIR}/macos_inc/include
    )
else()
    list(APPEND LIBPQ_INC
        ${CMAKE_CURRENT_SOURCE_DIR}/linux_inc/include
    )
endif()

# 全局公共宏定义 (对应 libpq_c_args)
add_compile_definitions(SO_MAJOR_VERSION=5)

# ==============================================================================
# 5. 目标构建：只构建静态库
# ==============================================================================
add_library(libpq_static STATIC ${LIBPQ_SOURCES})

# 设置输出的库名称 (在 Windows 上生成 pq.lib，在 Linux/macOS 上生成 libpq.a)
set_target_properties(libpq_static PROPERTIES OUTPUT_NAME "pq")

# 设置头文件包含路径
target_include_directories(libpq_static PRIVATE ${LIBPQ_INC})

# 链接第三方依赖库
if(ENABLE_SSL)
    target_compile_definitions(libpq_static PRIVATE
        -DUSE_OPENSSL
    )
    target_link_libraries(libpq_static PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()

if(ENABLE_GSSAPI)
    target_link_libraries(libpq_static PRIVATE ${GSSAPI_LIBRARIES})
endif()
