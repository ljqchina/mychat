#ifndef DB_H_
#define DB_H_

//#include <sqlite3.h>
#include <sqlcipher/sqlite3.h>
#include <string>


namespace db
{
	static sqlite3 *_db;
	int open(const char *dbFile);
	int close();

	namespace user
	{
		bool IsRegistered(const std::string &userId);
	};
};

#endif

