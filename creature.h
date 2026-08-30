#pragma once

#include "base.h"
#include "mods.h"
#include "external/libdicey/dice.h"
#include "external/libdicey/perten.h"

#include <stdint.h>
#include <string>
#include <vector>

struct creature_cache;

struct creature_context_t
{
	creature_context_t(const seed& s) : rand(s) {}

	seed rand; // derive from position from level seed to make both deterministic and parallelizable
	std::vector<tag_t> tags; // any area tags
	int depth = 1; // our current depth, starting from 1
	int difficulty = 0; // our current difficulty scaling

	creature_cache* cache = nullptr; // private data, initialize with init_creature_cache()
};

enum class role_type
{
	invalid, // for reporting errors
	leader,
	damage,
	tank,
	support,
	boss, // will not spawn normally
};
using role_t = role_type;

// Do not duplicate anything in arpgstats entity here.
struct creature_t
{
	uint16_t creature_type = 0;
	perten scaling = perten_full;
	role_t role = role_type::damage;
};

/// One role in a tribe's composition, loaded from monster_roles.csv.
struct creature_role_t
{
	uint16_t creature_type = 0; // the creature that fills the role
	perten scaling = perten_full; // powers %
};

/// A tribe is a collection of creatures that may share mods and be the target of similar abilities.
/// They would usually spawn together, and have different roles. The role of each member is
/// deterministically derived from the tribe's creature type, count and member index, so the
/// members themselves are not stored.
struct tribe_t
{
	int16_t leader = -1; // index of the leader member, -1 if none
	uint16_t creature_type = 0;
	seed s;
	position pos;
	uint16_t count; // number of creatures in the tribe, up to 32
};

/// Memory to keep around even for as long as the level is actual, even if the creatures themselves are unloaded.
/// Other creature data is deterministically regenerated when needed.
struct tribe_short_memory_t
{
	uint32_t dead = 0; // bitset, set to 1 if dead
};

/// Initialize a create cache for a level
bool init_creature_cache(creature_context_t& ctx);

/// Free a create cache
void free_creature_cache(creature_context_t& ctx);

/// Read creature definitions from CSV
bool read_creatures(const char* path);

/// Read tribe role definitions from CSV. Must be called after read_creatures().
bool read_creature_roles(const char* path);

/// Create a creature tribe
tribe_t create_tribe(const creature_context_t& context, position pos);

/// Create the creatures of a tribe, in member order
std::vector<creature_t> create_creature(const creature_context_t& context, const tribe_t& tribe);

/// Get the role definition for a creature type and role, or null if the role is not defined.
const creature_role_t* get_creature_role(int index, role_t role);

/// Get all creature types.
const std::string& get_creature_name(int index);

/// Get the number of creature types.
int get_creature_count();

/// Get a creature type index by name, or -1 if not found.
int get_creature_index(const std::string& name);

const char* role_to_string(role_type t);
