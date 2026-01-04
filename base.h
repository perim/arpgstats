#pragma once

#include <stdint.h>
#include <tuple>

using index_t = int16_t;
using level_t = int16_t;
using tile_t = uint16_t;
using tag_t = uint16_t;

/// A position is one unique coordinate
struct position
{
	tile_t x;
	tile_t y;
	level_t z;
	index_t w; // instance number of zone
	bool operator==(const position& rhs) { return std::tie(x, y, z, w) == std::tie(rhs.x, rhs.y, rhs.z, rhs.w); }
	bool operator<(const position& rhs) { return std::tie(x, y, z, w) < std::tie(rhs.x, rhs.y, rhs.z, rhs.w); }
};
