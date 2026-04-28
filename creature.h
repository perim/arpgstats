#pragma once

#include "base.h"
#include "mods.h"
#include "external/libdicey/dice.h"

#include <stdint.h>
#include <vector>

struct creature_cache;

struct creature_context_t
{
	creature_context_t(const seed& s) : rand(s) {}
	seed rand; // when level is created

	std::vector<tag_t> tags; // any area tags
	int depth = 1; // our current depth, starting from 1
	int difficulty = 0; // our current difficulty scaling

	creature_cache* cache = nullptr; // private data, initialize with init_creature_cache()
};

struct creature_t
{
	// unique index value? for caller
	//const std::string& name(); // type name
	// powers vector/array
	// current action state
	// inactive vs active
};

/// A tribe is a collection of creatures that may share mods and be the target of similar abilities.
/// They would usually spawn together, and have different roles. Roles are referred to by their
/// index into the members vector.
struct tribe_t
{
	// unique index value? for caller
	int16_t leader = -1;
	std::vector<creature_t> members; // ordered by importance
};

/// Initialize a create cache for a level
void init_creature_cache(creature_context_t& ctx);

/// Free a create cache
void free_creature_cache(creature_context_t& ctx);

/// Read creature definitions from CSV
bool read_creatures(const char* path);

/// Create a creature instance
creature_t create_creature(const creature_context_t& context);
