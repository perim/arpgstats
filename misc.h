#pragma once

#include <string>

enum class damage_t
{
	Physical,
	Fire,
	Cold,
	Lightning,
	Poison,
};

damage_t to_damage_type(const std::string& str);
