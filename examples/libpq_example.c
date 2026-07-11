/*-------------------------------------------------------------------------
 *
 * libpq_example.c
 *	  示例程序：演示 libpq 库的基本用法
 *
 * 编译方式（在项目构建目录下）:
 *   cmake .. -DENABLE_EXAMPLES=ON
 *   make
 *   ./example/libpq_example "host=localhost dbname=postgres user=postgres"
 *
 * 如果没有传连接参数，则默认连接:
 *   host=localhost dbname=postgres user=postgres password=secret
 *
 * Portions Copyright (c) 1996-2025, PostgreSQL Global Development Group
 * Portions Copyright (c) 1994, Regents of the University of California
 *
 *-------------------------------------------------------------------------
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "libpq-fe.h"

static void
check_connection(PGconn *conn)
{
	if (PQstatus(conn) != CONNECTION_OK)
	{
		fprintf(stderr, "Connection failed: %s\n", PQerrorMessage(conn));
		PQfinish(conn);
		exit(1);
	}
	printf("Connected to: %s/%s as %s\n",
		   PQhost(conn), PQdb(conn), PQuser(conn));
	printf("Server version: %d\n", PQserverVersion(conn));
	printf("Protocol version: %d\n", PQprotocolVersion(conn));
}

static void
exec_simple_query(PGconn *conn, const char *query)
{
	PGresult *res;

	res = PQexec(conn, query);
	if (PQresultStatus(res) != PGRES_TUPLES_OK &&
		PQresultStatus(res) != PGRES_COMMAND_OK)
	{
		fprintf(stderr, "Query failed: %s\n", PQerrorMessage(conn));
		PQclear(res);
		return;
	}

	printf("\n--- Query: %s ---\n", query);

	if (PQresultStatus(res) == PGRES_TUPLES_OK)
	{
		int ntuples = PQntuples(res);
		int nfields = PQnfields(res);
		int r, c;

		/* 打印列名 */
		for (c = 0; c < nfields; c++)
		{
			if (c > 0) printf(" | ");
			printf("%s", PQfname(res, c));
		}
		printf("\n");

		/* 打印结果行 */
		for (r = 0; r < ntuples; r++)
		{
			for (c = 0; c < nfields; c++)
			{
				if (c > 0) printf(" | ");
				if (PQgetisnull(res, r, c))
					printf("NULL");
				else
					printf("%s", PQgetvalue(res, r, c));
			}
			printf("\n");
		}

		printf("(%d row(s) returned)\n", ntuples);
	}
	else
	{
		/* PGRES_COMMAND_OK — 非查询命令 */
		const char *cmdstatus = PQcmdStatus(res);
		const char *tuples = PQcmdTuples(res);
		printf("%s", cmdstatus ? cmdstatus : "");
		if (tuples && tuples[0] != '\0')
			printf(" — %s row(s) affected\n", tuples);
		else
			printf("\n");
	}

	PQclear(res);
}

int
main(int argc, char *argv[])
{
	const char *conninfo;
	PGconn *conn;

	/*
	 * 从命令行参数获取连接信息，或使用默认值
	 */
	if (argc > 1)
		conninfo = argv[1];
	else
		conninfo = "host=localhost dbname=postgres user=postgres password=secret";

	/* 建立数据库连接 */
	conn = PQconnectdb(conninfo);
	check_connection(conn);

	/* ---- 示例 1: 执行 SELECT 查询 ---- */
	exec_simple_query(conn, "SELECT version() AS server_version");

	/* ---- 示例 2: 查询当前数据库的表信息 ---- */
	exec_simple_query(conn,
					  "SELECT schemaname, tablename, tableowner "
					  "FROM pg_catalog.pg_tables "
					  "WHERE schemaname NOT IN ('pg_catalog', 'information_schema') "
					  "LIMIT 10");

	/* ---- 示例 3: 执行 DDL / DML ---- */
	exec_simple_query(conn,
					  "CREATE TABLE IF NOT EXISTS public.libpq_example ("
					  "  id SERIAL PRIMARY KEY,"
					  "  name TEXT NOT NULL,"
					  "  created_at TIMESTAMPTZ DEFAULT now()"
					  ")");

	exec_simple_query(conn,
					  "INSERT INTO public.libpq_example (name) VALUES "
					  "('Alice'), ('Bob'), ('Charlie')");

	exec_simple_query(conn,
					  "SELECT * FROM public.libpq_example ORDER BY id");

	/* ---- 示例 4: 使用参数化查询（防 SQL 注入） ---- */
	{
		PGresult *res;
		const char *paramValues[1];
		paramValues[0] = "Bob";

		res = PQexecParams(conn,
						   "SELECT * FROM public.libpq_example WHERE name = $1",
						   1,		/* nParams */
						   NULL,	/* paramTypes (NULL => 让服务器推断) */
						   paramValues,
						   NULL,	/* paramLengths (文本格式不需要) */
						   NULL,	/* paramFormats (文本格式) */
						   0);		/* resultFormat (文本格式) */

		if (PQresultStatus(res) == PGRES_TUPLES_OK)
		{
			printf("\n--- Parameterized query: name = 'Bob' ---\n");
			if (PQntuples(res) > 0)
				printf("Found: id=%s, name=%s\n",
					   PQgetvalue(res, 0, 0),
					   PQgetvalue(res, 0, 1));
			else
				printf("No matching row found.\n");
		}
		else
		{
			fprintf(stderr, "Parameterized query failed: %s\n",
					PQerrorMessage(conn));
		}
		PQclear(res);
	}

	/* ---- 示例 5: 检查连接信息 ---- */
	printf("\n--- Connection info ---\n");
	printf("  host     : %s\n", PQhost(conn));
	printf("  port     : %s\n", PQport(conn));
	printf("  dbname   : %s\n", PQdb(conn));
	printf("  user     : %s\n", PQuser(conn));
	printf("  password : %s\n", PQpass(conn) ? "***" : "(none)");

	/* 清理并关闭连接 */
	PQfinish(conn);
	printf("\nDone.\n");

	return 0;
}
