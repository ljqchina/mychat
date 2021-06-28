#include "util.h"
#include <string.h>

namespace util
{
	std::string md5(const std::string &str)
	{
		MD5_CTX ctx;

		std::string md5_string;
		unsigned char md[20] = { 0 };
		char tmp[4] = { 0 };

		MD5_Init(&ctx);
		MD5_Update(&ctx, str.c_str(), str.size());
		MD5_Final(md, &ctx);

		for(int i = 0; i < 16; ++i)
		{   
			memset(tmp, 0x00, sizeof(tmp));
			sprintf(tmp, "%02X", md[i]);
			md5_string += tmp;
		}   
		return md5_string;
	}
	
	std::string DateTime()
	{
		time_t t;
		time(&t);
		struct tm *tt;
		tt = localtime(&t);
		char s[100];
		sprintf(s, "%d-%02d-%02d %02d:%02d:%02d", tt->tm_year+1900,
				tt->tm_mon+1, tt->tm_mday, tt->tm_hour, tt->tm_min, tt->tm_sec);
		return std::string(s);
	}
};

