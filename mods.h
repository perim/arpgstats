#pragma once

#include <stdint.h>

#include "base.h"
#include "libdicey/dice.h"

enum class mod_class : uint8_t
{
	permanent, // deterministic drop modifier that cannot be altered
	spawn, // drop-only sticky modifier, can be removed but never added after the initial drop
	crafted, // random (re)craftable modifier
	implicit, // comes with item type
};

enum class mod_type : uint8_t
{
	more,
	less,
	slower,
	faster,
};

// variance could be a conditional or a specific element of damage
struct variance_pair
{
	uint8_t type; // type of variance
	uint8_t value; // value of the variance
};

struct item_mod
{
	uint8_t type;
	uint8_t roll;
	mod_class binding;
	std::array<2, variance_type> variance;
};

/// First 8 bits are type classification (item, class, skill...), second 8 bits is index of which one
using uint16_t object_type = uint16_t;

// as read from file
struct mod_data
{
	std::string name; // may be empty
	std::string description;
	uint16_t mod_type; // some enum for type of mod
	object_type applies; // to which object type it applies to (any enumerated item type, skill, class or device type)
	mod_class subtype;
	mod_type type;
	uint16_t min; // roll range minimum
	uint16_t max; // by default same as min
	level_t min_depth = 0; // zero means no limit
	level_t max_depth = 0; // zero means no limit
	uint16_t weighting = 0;
	// trees only:
	int16_t requires = -1; // if it requires another mod to be present first
	uint8_t max_count = 0; // only this many nodes of this type can be on the tree
};

/// Get mod data for a given enumerated mod type
const mod_data& get_mod_data(int index);

/// Read mods from csv file
bool read_mods(const char* filename);
