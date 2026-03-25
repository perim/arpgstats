#pragma once

#include "base.h"
#include "mods.h"
#include "external/libdicey/dice.h"

#include <stdint.h>
#include <vector>

enum class base_item_type_t
{
	equippable,
	consumable, // has charges/stacks
	placeable,
	unlockable, // quest or collectable vanity item, ie no relevant function here
	any
};

enum class item_drop_type_t
{
	unique,
	normal,
	any
};

struct mod_roll_t
{
	int permanent_mods = 0;
	int spawn_mods = 0;
	int crafted_mods = 0;
};

struct restrict_drop_t
{
	base_item_type_t base_item_type; // if keystone, must be `any`
	item_drop_type_t drop_type; // if keystone, must be `unique`
	int32_t item_type; // keystone: other than -1 means override generated value (should stay same item classification)
	// note that the roll of permanent mods cannot be influenced, it is deterministic
	mod_roll_t roll_rarity_weightings; // added to general rarity
	mod_roll_t roll_quantity_weightings; // added to general quantity
};

struct item_t
{
	uint32_t item_type;
	std::vector<mod> mods; // ordered implicit -> permanent -> spawn -> crafted
	position found; // where you found it
	index_t crafted; // who made it
};

struct item_luck_t
{
	int drop_rarity_luck = 0;
	int drop_quantity_luck = 0;
	int rarity = 0;
	int quantity = 0;
	int currency_duplication_chance = 0;
	int consumable_duplication_chance = 0;
	uint64_t spawn_roll = 0; // if non-zero, the specific spawn mod(s) to roll
	// note that the roll of permanent mods cannot be influenced, it is deterministic
	mod_roll_t roll_rarity_weightings; // added to general rarity
	mod_roll_t roll_quantity_weightings; // added to general quantity
};

struct level_loot_context_t
{
	std::vector<tag_t> tags; // any area tags
	int depth = 1; // our current depth, starting from 1
	int difficulty = 0; // our current difficulty
	int base_quantity = 3; // for any fractional quantity, roll on it to see if we add 1 more
	int level_quantity_bonus = 0; // any challenge based quantity % being awarded
	int level_rarity_bonus = 0; // any challenge based rarity % being awarded
	int item_distribution = 50; // each item has 50% chance of being an item rather than currency
	int consumable_distribution = 50; // each item has 50% chance of being a consumable
	int unique_chance = 25; // each chest has this chance of containing a unique, and this chance is non-modifiable
};

struct item_cache;

struct loot_context_t
{
	loot_context_t(const seed& s) : rand(s) {}
	seed rand; // when level is created, determine how many chests are there, and generate a seed for each ahead of time
	const level_loot_context_t* level_modifiers = nullptr;
	const item_luck_t* player_modifiers = nullptr;
	item_cache* cache = nullptr; // private data, initialize with init_item_cache()
};

struct currency_t
{
	uint16_t type;
	uint16_t amount;
};

struct drops_t
{
	std::vector<item_t> items;
	std::vector<currency_t> currencies;
};

/// Initialize an item cache for a level
void init_item_cache(loot_context_t& ctx);

/// Free an item cache
void free_item_cache(loot_context_t& ctx);

/// Read item definitions from CSV
bool read_items(const char* path);

/// Read currencies from CSV
bool read_currencies(const char* path);

/// Get all item types.
const std::vector<std::string>& get_item_types();

/// Get all currency types.
const std::vector<std::string>& get_currency_types();

/// Generate an item. You may influence what type, rarity and quantity is generated through `context`.
/// If `keystone` is not null, then we generate a keystone item with more restrictions and
/// influences.
item_t create_item(const loot_context_t& context, const restrict_drop_t* keystone = nullptr);

/// Generate drops from a chest (or similar). Pass a `keystone` parameter if the chest
/// should contain a keystone item and can be used to override its modifiers and rolls.
drops_t generate_drops(const loot_context_t& context, int items, int currency, const restrict_drop_t* keystone = nullptr);
