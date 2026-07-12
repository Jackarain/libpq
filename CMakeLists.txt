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

    ${CMAKE_CURRENT_SOURCE_DIR}/port/chklocale.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/explicit_bzero.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/getpeereid.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/inet_net_ntop.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/noblock.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/pgstrcasecmp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/pg_strong_random.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/snprintf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/strerror.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/timingsafe_bcmp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/bsearch_arg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/path.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/pg_bitutils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/pgmkdirp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/pgsleep.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/pgstrsignal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/pqsignal.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/qsort_arg.c
    ${CMAKE_CURRENT_SOURCE_DIR}/port/quotes.c

    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/base64.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/cryptohash.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/encnames.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/fe_memutils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/hmac.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/ip.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/jsonapi.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/link-canary.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/md5.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/md5_common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/pg_prng.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/psprintf.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/saslprep.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/scram-common.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/sha1.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/sha2.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/string.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/stringinfo.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/unicode_norm.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common/wchar.c
)

# ==============================================================================
# 2. 平台特异性处理 (Windows / macOS / Linux)
# ==============================================================================
if(WIN32)
    # Windows 专属源码
    list(APPEND LIBPQ_SOURCES
        ${CMAKE_CURRENT_SOURCE_DIR}/src/pthread-win32.c
        ${CMAKE_CURRENT_SOURCE_DIR}/src/win32.c
        ${CMAKE_CURRENT_SOURCE_DIR}/port/open.c
        ${CMAKE_CURRENT_SOURCE_DIR}/port/strlcpy.c
        ${CMAKE_CURRENT_SOURCE_DIR}/port/strlcat.c
        ${CMAKE_CURRENT_SOURCE_DIR}/port/win32error.c
        ${CMAKE_CURRENT_SOURCE_DIR}/port/win32gai_strerror.c
        ${CMAKE_CURRENT_SOURCE_DIR}/port/win32gettimeofday.c
        ${CMAKE_CURRENT_SOURCE_DIR}/port/win32ntdll.c
        ${CMAKE_CURRENT_SOURCE_DIR}/port/win32setlocale.c
        ${CMAKE_CURRENT_SOURCE_DIR}/port/win32stat.c
        ${CMAKE_CURRENT_SOURCE_DIR}/port/dirmod.c
        ${CMAKE_CURRENT_SOURCE_DIR}/port/win32common.c
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
# 示例程序 (默认关闭)
# ==============================================================================
option(ENABLE_EXAMPLES "Build example programs" OFF)

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
    )
    if(NOT MINGW)
        list(APPEND LIBPQ_INC
            ${CMAKE_CURRENT_SOURCE_DIR}/include/port/win32_msvc
        )
    endif()
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
add_compile_definitions(SO_MAJOR_VERSION=5 FRONTEND _GNU_SOURCE JSONAPI_USE_PQEXPBUFFER)

# ==============================================================================
# 5. 目标构建：只构建静态库
# ==============================================================================
add_library(pq STATIC ${LIBPQ_SOURCES})

# 设置输出的库名称 (在 Windows 上生成 pq.lib，在 Linux/macOS 上生成 libpq.a)
set_target_properties(pq PROPERTIES OUTPUT_NAME "pq")

if(WIN32)
    target_compile_definitions(pq PRIVATE -DWIN32 -D_CRT_SECURE_NO_WARNINGS)
    target_link_libraries(pq INTERFACE ws2_32 wldap32)
    if(MINGW)
        target_compile_definitions(pq PRIVATE -D_POSIX_C_SOURCE)
    endif()
endif()

# 设置头文件包含路径
target_include_directories(pq PRIVATE ${LIBPQ_INC})

# 链接第三方依赖库
if(ENABLE_SSL)
    target_compile_definitions(pq PRIVATE -DUSE_OPENSSL)
    target_link_libraries(pq PRIVATE OpenSSL::SSL OpenSSL::Crypto)
endif()

if(ENABLE_GSSAPI)
    target_link_libraries(pq PRIVATE ${GSSAPI_LIBRARIES})
endif()

# 数学库 (用于 pg_prng.c 中的 log())
find_library(MATH_LIBRARY m)
if(MATH_LIBRARY)
    target_link_libraries(pq PRIVATE ${MATH_LIBRARY})
endif()

# 线程库 (用于多线程支持)
find_package(Threads REQUIRED)
if(Threads_FOUND)
    target_link_libraries(pq PRIVATE Threads::Threads)
endif()

# 设置别名库，方便外部使用
target_include_directories(pq SYSTEM INTERFACE interfaces/libpq include)

add_library(PostgreSQL::pq ALIAS pq)
add_library(PostgreSQL::PostgreSQL ALIAS pq)

# ==============================================================================
# 6. 示例程序
# ==============================================================================
if(ENABLE_EXAMPLES)
    add_subdirectory(examples)
endif()
