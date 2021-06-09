#ifndef UTIL_H_
#define UTIL_H_

#include <openssl/md5.h>
#include <string>

namespace util
{
	std::string md5(const std::string &str);
};

#endif

