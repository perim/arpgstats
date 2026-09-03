#pragma once

#include <stdarg.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

void usage(); // forward declared only, implemented separately in each executable

static inline bool match(const char* in, const char* short_form, const char* long_form, int& remaining)
{
	if ((short_form && strcmp(in, short_form) == 0) || (long_form && strcmp(in, long_form) == 0))
	{
		remaining--;
		return true;
	}
	return false;
}

static inline int get_int(const char* in, int& remaining)
{
	if (remaining == 0)
	{
		usage();
	}
	remaining--;
	return atoi(in);
}

static inline std::string get_str(const char* in, int& remaining)
{
	if (remaining == 0)
	{
		usage();
	}
	remaining--;
	return in;
}

static inline uint64_t get_uint64(const char* in, int& remaining)
{
	if (remaining == 0)
	{
		usage();
	}
	remaining--;
	return strtoull(in, nullptr, 0); // base 0: decimal, 0x hex, 0 octal
}
