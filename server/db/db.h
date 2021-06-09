#ifndef DB_H_
#define DB_H_

//#include <sqlite3.h>
#include <sqlcipher/sqlite3.h>
#include <string>
#include <vector>
#include "Protocol.h"

namespace db
{
	static sqlite3 *_db;
	int open(const char *dbFile);
	int close();

	namespace user
	{
		bool IsRegistered(const std::string &userId);
		int RegisterUser(const RegisterInfo &info);
		bool CheckPassword(const LoginInfo &pwd);
		int QueryOfflineMsg(const std::string &userId, std::vector<OfflineInfo> &v);
	};
};

#endif

