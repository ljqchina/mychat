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

			int col = 1;
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

		bool CheckPassword(const LoginInfo &info)
		{
			std::string sql;
			const char *zTail;
			sqlite3_stmt *stmt = NULL; 

			sql = "select count(*) from userinfo where userid='" + info.userId + "'";
			sql += " and password='" + info.password + "'";
			int ret = sqlite3_prepare_v2(_db, sql.c_str(), -1, &stmt, &zTail);
			if(ret != SQLITE_OK)
			{
				printf("%s error:%d\n", __func__, ret);
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

		int QueryOfflineMsg(const std::string &userId, std::vector<OfflineInfo> &v)
		{
			std::string sql;
			const char *zTail;
			sqlite3_stmt *stmt = NULL; 

			sql = "select content, datetime, status from offlinemsg where userid='" + userId + "'";
			int ret = sqlite3_prepare_v2(_db, sql.c_str(), -1, &stmt, &zTail);
			if(ret != SQLITE_OK)
			{
				printf("%s error:%d\n", __func__, ret);
				return -1;
			}
			
			while(sqlite3_step(stmt) == SQLITE_ROW)
			{
				int col = 0;
				const unsigned char *p;
				OfflineInfo oi;
				oi.userId = userId;
			   	p = sqlite3_column_text(stmt, col++);
				oi.content = (char *)p;
			   	p = sqlite3_column_text(stmt, col++);
				oi.datetime = (char *)p;
			   	oi.status = sqlite3_column_int(stmt, col++);
				v.push_back(std::move(oi));
			}
			sqlite3_finalize(stmt);
			return 0;
		}

		int SaveOfflineMsg(const OfflineInfo &info)
		{
			int ret = 0;
			const char *zTail = nullptr;
			std::string sql;
			sql = "insert into offlinemsg(userid, content, datetime, status)\
			values(?, ?, ?, ?)";
			sqlite3_stmt * stmt = nullptr; 
			ret = sqlite3_prepare_v2(_db, sql.c_str(), sql.length(), &stmt, &zTail);
			if(ret != SQLITE_OK)
				return -1;

			int col = 1;
			sqlite3_bind_text(stmt, col++, info.userId.c_str(), info.userId.length(), NULL);
			sqlite3_bind_text(stmt, col++, info.content.c_str(), info.content.length(), NULL);
			sqlite3_bind_text(stmt, col++, info.datetime.c_str(), info.datetime.length(), NULL);
			sqlite3_bind_int(stmt, col++, info.status);

			sqlite3_step(stmt);
			sqlite3_reset(stmt);
			sqlite3_finalize(stmt);
			return 0;
		}

		int RemoveOfflineMsg(const std::string &userId)
		{
			int ret = 0;
			char *zErrMsg = NULL;
			std::string sql;
			sql = "delete from offlinemsg where userid='" + userId + "'";
			ret = sqlite3_exec(_db, sql.data(), 0, 0, &zErrMsg);
			if(ret == SQLITE_OK)
				return 0;
			else
				return -1;
		}

		int MakeFriend(const std::string &userId, const std::string &friendId)
		{
			int ret = 0;
			const char *zTail = nullptr;
			std::string sql;
			sql = "insert into friend(userid, friend) values(?, ?)";
			sqlite3_stmt * stmt = nullptr; 
			ret = sqlite3_prepare_v2(_db, sql.c_str(), sql.length(), &stmt, &zTail);
			if(ret != SQLITE_OK)
				return -1;

			int col = 1;
			sqlite3_bind_text(stmt, col++, userId.c_str(), userId.length(), NULL);
			sqlite3_bind_text(stmt, col++, friendId.c_str(), friendId.length(), NULL);
			sqlite3_step(stmt);
			sqlite3_reset(stmt);
			sqlite3_finalize(stmt);
			return 0;

		}

		bool IsFriend(const std::string &userId, const std::string &friendId)
		{
			std::string sql;
			const char *zTail;
			sqlite3_stmt *stmt = NULL; 
			sql = "select count(*) from friend where userid='" + userId + "' and friend='" + friendId + "'";
			int ret = sqlite3_prepare_v2(_db, sql.c_str(), -1, &stmt, &zTail);
			if(ret != SQLITE_OK)
			{
				printf("%s error:%d\n", __func__, ret);
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

		int RemoveFriend(const std::string &userId, const std::string &friendId)
		{
			int ret = 0;
			char *zErrMsg = NULL;
			std::string sql;
			sql = "delete from friend where userid='" + userId + "' and friend='" + friendId + "'";
			ret = sqlite3_exec(_db, sql.data(), 0, 0, &zErrMsg);
			if(ret == SQLITE_OK)
				return 0;
			else
				return -1;

		}
	};
};

