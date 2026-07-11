# libpq — PostgreSQL 客户端 C 库

[![License](https://img.shields.io/badge/license-PostgreSQL-blue.svg)](https://www.postgresql.org/about/licence/)

## 简介

**libpq** 是 [PostgreSQL](https://www.postgresql.org/) 的官方 C 语言客户端库，为应用程序提供了连接、查询和管理 PostgreSQL 数据库的完整 API。本仓库是基于 CMake 构建系统重构的独立 libpq 版本，去除了对 PostgreSQL 完整源码树的依赖，可方便地集成到任意 C/C++ 项目中使用。

## 快速开始

### 构建要求

- CMake >= 3.15
- C99 兼容编译器 (GCC / Clang / MSVC)
- OpenSSL（可选，默认启用）
- GSSAPI（可选，默认禁用）

### 构建步骤

```bash
# 配置项目
cmake -B build

# 编译静态库
cmake --build build

# 产物位于 build/libpq.a (Unix) 或 build/pq.lib (Windows)
```

#### 可选构建选项

```bash
# 禁用 SSL 支持
cmake -B build -DENABLE_SSL=OFF

# 启用 GSSAPI 支持
cmake -B build -DENABLE_GSSAPI=ON
```

### 集成到你的项目

```cmake
# 在你的 CMakeLists.txt 中添加
add_subdirectory(path/to/libpq)
target_link_libraries(your_target PRIVATE libpq_static)
target_include_directories(your_target PRIVATE
    path/to/libpq/include
    path/to/libpq/interfaces/libpq
)
```

## 使用示例

```c
#include <stdio.h>
#include "libpq-fe.h"

int main() {
    PGconn *conn = PQconnectdb("host=localhost dbname=mydb user=myuser");
    if (PQstatus(conn) != CONNECTION_OK) {
        fprintf(stderr, "连接失败: %s", PQerrorMessage(conn));
        PQfinish(conn);
        return 1;
    }

    PGresult *res = PQexec(conn, "SELECT version()");
    if (PQresultStatus(res) == PGRES_TUPLES_OK)
        printf("%s\n", PQgetvalue(res, 0, 0));

    PQclear(res);
    PQfinish(conn);
    return 0;
}
```

## 支持的平台

- **Linux** (GCC / Clang)
- **macOS** (Clang)
- **Windows** (MSVC / MinGW)

## 许可证

本项目源自 PostgreSQL 官方源码，使用 **PostgreSQL License** — 一种与 BSD 类似的宽松开源许可证。详细信息请参阅 `COPYRIGHT` 文件（如包含）或访问 [PostgreSQL 许可页面](https://www.postgresql.org/about/licence/)。

## 相关资源

- [PostgreSQL 官方文档 — libpq](https://www.postgresql.org/docs/current/libpq.html)
- [PostgreSQL 源码](https://git.postgresql.org/gitweb/?p=postgresql.git)
- [PostgreSQL 社区](https://www.postgresql.org/community/)
