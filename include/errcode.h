#ifndef ERRCODE_H_
#define ERRCODE_H_

#include <map>
#include <string>

enum
{
	ERR_SUCCESS = 0,
	ERR_SYS_ERR = 100,
	ERR_SYS_UNKNOWN,
	ERR_SYS_PACKAGE,

	ERR_USER_EXISTS = 200,
	ERR_USER_NOT_EXISTS,
	ERR_USER_ACOUNT_PASS,
	ERR_USER_LOGIN_OTHER,

	ERR_DB_ERROR = 300,
	ERR_DB_FILE,
	ERR_DB_NOT_FOUND 
};

static const std::map<int, std::string> errors = 
{
	{ERR_SUCCESS, "成功"},
	{ERR_SYS_ERR, "系统错误"},
	{ERR_SYS_UNKNOWN, "未知错误"},
	{ERR_SYS_PACKAGE, "数据包解析错误"},

	{ERR_USER_EXISTS, "用户已存在"},
	{ERR_USER_NOT_EXISTS, "用户不存在"},
	{ERR_USER_ACOUNT_PASS, "用户名或密码错误"},
	{ERR_USER_LOGIN_OTHER, "用户在其他机器登录"},

	{ERR_DB_ERROR, "数据库错误"},
	{ERR_DB_FILE, "数据库文件错误"},
	{ERR_DB_NOT_FOUND, "数据未发现"}
};

std::string errstr(int err)
{
	auto ite = errors.find(err);
	if(ite == errors.end())
		return std::string();
	return ite->second;
}
#endif

