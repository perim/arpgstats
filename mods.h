#pragma once

#include <stdint.h>

#include <string>
#include <vector>
#include <optional>

enum class mod_category
{
	none, // when category is irrelevant, ie not modifiable by player
	permanent, // deterministic drop modifier that cannot be altered
	spawn, // drop-only sticky modifier, can be removed but never added after the initial drop
	crafted, // random (re)craftable modifier
	implicit, // comes with item type
};

enum class value_type
{
	more,
	less,
	slower,
	faster,
};

// variance could be a conditional or a specific element of damage
struct variance_pair
{
	uint8_t type = 0; // type of variance
	uint8_t value = 0; // value of the variance
};

struct mod
{
	uint8_t type;
	uint8_t roll;
	mod_category category;
	variance_pair restriction;
	variance_pair condition;
};

// as read from file
struct mod_data
{
	uint16_t index; // generated on read, contiguous, may change
	uint32_t uuid; // stable unique id, not contiguous; 8 bits type, 8 bits include section, 16 bits index
	std::string name; // may be empty, if non-empty, must be unique
	std::string description;
//	uint16_t mod_effect; // some enum for the effect of a mod, TBD
	uint16_t classification; // generic subtype
	mod_category category;
//	value_type value_meaning; // TBD
	uint16_t min; // roll range minimum
	uint16_t max; // by default same as min
//	uint16_t min_depth = 0; // zero means no limit
//	uint16_t max_depth = 0; // zero means no limit
	uint16_t weighting = 0;
//	int16_t variance_type = -1; // the type of variance used in this modifier, or -1 if none
	// trees only:
	int16_t requires = -1; // if it requires another mod to be present first
	uint8_t max_count = 0; // only this many nodes of this type can be on the tree
};

/// Read mods from csv file and initialize. `main` is the full list of mod data, while `reverse` is the reverse lookup
/// file containing just type and GUID values. Both are created by the `combine.py` script. You must call this before
/// any other function here.
bool read_mods(const char* main);//, const char* reverse);

/// Get mod data for a given enumerated mod type by GUID index.
const mod_data& get_mod_data(int index);

/// Get a list of mods by GUID indicies that exist for a given type. TBD.
//const std::vector<int>& get_mods_by_type(int type);

/// Get a list of types that exist. This is mostly for debugging.
const std::vector<std::string>& get_mod_types();

/// Get a list of classifications that exist. This is mostly for debugging.
const std::vector<std::string>& get_mod_classifications();

/// Look-up classification index
std::optional<unsigned> get_classification_index(const std::string& v);
