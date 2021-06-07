#include "db.h"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

namespace db
{
	int open(const char *dbFile)
	{
		int ret = sqlite3_open(dbFile, &_db);
		return ret;
	}

	int close()
	{
		int ret = sqlite3_close(_db);
		return ret;
	}

	namespace user
	{
		bool IsRegistered(const std::string &userId)
		{
			std::string sql;
			const char *zTail;
			sqlite3_stmt *stmt = NULL; 

			sql = "select count(*) from userinfo where userid = " + userId;
			int ret = sqlite3_prepare_v2(_db, sql.c_str(), -1, &stmt, &zTail);
			if(ret != SQLITE_OK)
			{
				printf("error:%d\n", ret);
				return false;
			}
			
			int col = 0;
			int count = 0;
			if(sqlite3_step(stmt) == SQLITE_ROW)
			{
				count = sqlite3_column_int(stmt, col);
			}
			sqlite3_finalize(stmt);
			return count ? true : false;
		}

		int RegisterUser(const RegisterInfo &info)
		{
			int ret = 0;
			const char *zTail = nullptr;
			std::string sql;
			sql = "insert into userinfo(userid, nickname, password,\
				   sex, age, email, mobile, address, type, status) \
				   values(?, ?, ?, ?, ?, ?, ?, ?, ?, ?)";
			sqlite3_stmt * stmt = nullptr; 
			ret = sqlite3_prepare_v2(_db, sql.c_str(), sql.length(), &stmt, &zTail);
			if(ret != SQLITE_OK)
				return -1;

			int col = 0;
			sqlite3_bind_text(stmt, col++, info.userId.c_str(), info.userId.length(), NULL);
			sqlite3_bind_text(stmt, col++, info.nickName.c_str(), info.nickName.length(), NULL);
			sqlite3_bind_text(stmt, col++, info.password.c_str(), info.password.length(), NULL);
			sqlite3_bind_int(stmt, col++, info.sex);
			sqlite3_bind_int(stmt, col++, info.age);
			sqlite3_bind_text(stmt, col++, info.email.c_str(), info.email.length(), NULL);
			sqlite3_bind_text(stmt, col++, info.mobile.c_str(), info.mobile.length(), NULL);
			sqlite3_bind_text(stmt, col++, info.address.c_str(), info.address.length(), NULL);
			sqlite3_bind_int(stmt, col++, info.type);
			sqlite3_bind_int(stmt, col++, info.status);

			sqlite3_step(stmt);
			sqlite3_reset(stmt);
			sqlite3_finalize(stmt);
			return 0;
		}
	};
};

/*
int main()
{
	const char *dbfile = "./mychat.db";
	int ret = db::open(dbfile);
	printf("db::open ret:%d\n", ret);

	string userId("123456");
	printf("db::IsRegistered:%d\n", db::user::IsRegistered(userId));

	ret = db::close();
	printf("db::close ret:%d\n", ret);
	return 0;
}
*/

