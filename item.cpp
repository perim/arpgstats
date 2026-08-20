#include "item.h"

#include "external/csv.hpp"

#include <algorithm>
#include <unordered_set>

struct currency_type_t
{
	std::string name;
	int weighting = -1;
	int min_level = -1;
	//int max_level = -1;
	int max_stack = -1;
	// TBD tags
	// TBD description
};

struct item_def_t
{
	std::string name;
	std::string classification;
	std::string location;
	int weighting = 100;
};

static std::vector<std::string> item_types;
static std::vector<item_def_t> item_defs;
static std::vector<std::string> currency_types;
static std::vector<currency_type_t> currencies;
static const_roll_table* currency_table = nullptr;
static std::vector<int> currency_weights;
static const_roll_table* item_table = nullptr;
static std::vector<int> item_weights;

struct item_mod_pool_t
{
	std::vector<int> permanent_mods;
	std::vector<int> permanent_weights;
	const_roll_table* permanent_table = nullptr;

	std::vector<int> spawn_mods;
	std::vector<int> spawn_weights;
	const_roll_table* spawn_table = nullptr;

	std::vector<int> crafted_mods;
	std::vector<int> crafted_weights;
	const_roll_table* crafted_table = nullptr;

	~item_mod_pool_t()
	{
		delete permanent_table;
		delete spawn_table;
		delete crafted_table;
	}
};

struct item_cache
{
	filtered_const_roll_table* currencies = nullptr;
	std::vector<item_mod_pool_t*> mod_pools;

	~item_cache()
	{
		delete currencies;
		for (auto* p : mod_pools) delete p;
		mod_pools.clear();
	}
};

static std::vector<std::string> get_applicable_mod_types(const std::string& item_name, const std::string& classification)
{
	std::vector<std::string> types;
	types.push_back(item_name);

	if (item_name == "Wand")
	{
		types.push_back("Weapon");
	}
	else if (classification == "Melee" || classification == "Ranged")
	{
		types.push_back("Weapon");
	}
	else if (classification == "Parrying")
	{
		if (item_name.find("shield") != std::string::npos || item_name.find("Shield") != std::string::npos)
		{
			types.push_back("Shield");
		}
		else
		{
			types.push_back("Weapon");
		}
	}
	else if (classification == "Headgear")
	{
		types.push_back("Helmet");
	}
	else if (classification == "Body")
	{
		types.push_back("Body");
	}
	else if (classification == "Gloves")
	{
		types.push_back("Gloves");
	}
	else if (classification == "Boots")
	{
		types.push_back("Boots");
	}

	return types;
}

static item_mod_pool_t* build_item_mod_pool(const std::string& item_name, const std::string& classification)
{
	item_mod_pool_t* pool = new item_mod_pool_t();
	std::vector<std::string> applicable = get_applicable_mod_types(item_name, classification);
	std::unordered_set<int> added;

	for (const auto& tname : applicable)
	{
		auto tidx = get_mod_type_index(tname);
		if (!tidx) continue;
		const auto& m_indices = get_mods_by_type(*tidx);
		for (int idx : m_indices)
		{
			if (added.find(idx) != added.end()) continue;
			added.insert(idx);
			const mod_data& d = get_mod_data(idx);
			int weight = d.weighting > 0 ? d.weighting : 100;

			if (d.category == mod_category::permanent)
			{
				pool->permanent_mods.push_back(idx);
				pool->permanent_weights.push_back(weight);
			}
			else if (d.category == mod_category::spawn)
			{
				pool->spawn_mods.push_back(idx);
				pool->spawn_weights.push_back(weight);
			}
			else if (d.category == mod_category::crafted)
			{
				pool->crafted_mods.push_back(idx);
				pool->crafted_weights.push_back(weight);
			}
		}
	}

	if (!pool->permanent_weights.empty())
	{
		pool->permanent_table = new const_roll_table(pool->permanent_weights);
	}
	if (!pool->spawn_weights.empty())
	{
		pool->spawn_table = new const_roll_table(pool->spawn_weights);
	}
	if (!pool->crafted_weights.empty())
	{
		pool->crafted_table = new const_roll_table(pool->crafted_weights);
	}

	return pool;
}

/// Initialize an item cache for a level
void init_item_cache(loot_context_t& ctx)
{
	assert(currency_table);
	free_item_cache(ctx);
	ctx.cache = new item_cache;
	std::vector<bool> mask(currency_weights.size());
	bool has_any = false;
	for (unsigned i = 0; i < currency_weights.size(); i++)
	{
		mask[i] = ctx.level_modifiers->depth >= currencies.at(i).min_level;
		has_any |= mask[i];
	}
	assert(has_any);
	if (has_any) ctx.cache->currencies = new filtered_const_roll_table(currency_weights, mask);

	// Build mod pools for each item type
	for (size_t i = 0; i < item_defs.size(); i++)
	{
		item_mod_pool_t* pool = build_item_mod_pool(item_defs[i].name, item_defs[i].classification);
		ctx.cache->mod_pools.push_back(pool);
	}
}

/// Free an item cache
void free_item_cache(loot_context_t& ctx)
{
	if (ctx.cache)
	{
		delete ctx.cache;
		ctx.cache = nullptr;
	}
}

static void roll_category_mods(seed& s,
                               int count,
                               const std::vector<int>& mod_indices,
                               const const_roll_table* table,
                               std::unordered_set<int>& chosen_indices,
                               std::vector<mod>& out_mods)
{
	if (count <= 0 || mod_indices.empty() || table == nullptr) return;

	int remaining = count;
	int attempts = 0;
	while (remaining > 0 && attempts < count * 20)
	{
		attempts++;
		int slot = table->roll(s);
		int mod_idx = mod_indices[slot];
		if (chosen_indices.find(mod_idx) != chosen_indices.end())
		{
			continue;
		}

		chosen_indices.insert(mod_idx);
		const mod_data& d = get_mod_data(mod_idx);

		mod m;
		m.type = (uint16_t)mod_idx;
		m.category = d.category;
		if (d.max > d.min)
		{
			m.roll = (uint16_t)s.roll(d.min, d.max);
		}
		else
		{
			m.roll = (uint16_t)d.min;
		}
		out_mods.push_back(m);
		remaining--;
	}

	// Fallback to pick unchosen if collisions prevented full roll
	if (remaining > 0)
	{
		for (int mod_idx : mod_indices)
		{
			if (remaining <= 0) break;
			if (chosen_indices.find(mod_idx) == chosen_indices.end())
			{
				chosen_indices.insert(mod_idx);
				const mod_data& d = get_mod_data(mod_idx);
				mod m;
				m.type = (uint16_t)mod_idx;
				m.category = d.category;
				m.roll = (uint16_t)(d.max > d.min ? s.roll(d.min, d.max) : d.min);
				out_mods.push_back(m);
				remaining--;
			}
		}
	}
}

item_t create_item(const loot_context_t& context, const restrict_drop_t* filter)
{
	assert(item_table);
	seed s = seed_random();
	item_t item{};
	if (filter != nullptr && filter->item_type >= 0)
	{
		item.item_type = (uint32_t)filter->item_type;
	}
	else
	{
		item.item_type = (uint32_t)item_table->roll(s);
	}

	level_loot_context_t default_lvl;
	const level_loot_context_t& lvl = context.level_modifiers ? *context.level_modifiers : default_lvl;

	item_mod_pool_t* pool = nullptr;
	bool free_pool_needed = false;
	if (context.cache && item.item_type < context.cache->mod_pools.size())
	{
		pool = context.cache->mod_pools[item.item_type];
	}
	else if (item.item_type < item_defs.size())
	{
		pool = build_item_mod_pool(item_defs[item.item_type].name, item_defs[item.item_type].classification);
		free_pool_needed = true;
	}

	if (pool == nullptr)
	{
		return item;
	}

	bool is_unique = (filter && filter->drop_type == item_drop_type_t::unique)
	                 || (s.roll(0, 99) < lvl.unique_chance);

	int perm_count = is_unique ? s.roll(lvl.min_permanent_mods, lvl.max_permanent_mods) : lvl.min_permanent_mods;
	int spawn_count = s.roll(lvl.min_spawn_mods, lvl.max_spawn_mods);
	int craft_count = s.roll(lvl.min_crafted_mods, lvl.max_crafted_mods);

	if (context.player_modifiers && context.player_modifiers->replace_permanent_with_spawn)
	{
		spawn_count += perm_count;
		perm_count = 0;
	}

	// Clamp by pool sizes
	perm_count = std::min(perm_count, (int)pool->permanent_mods.size());
	spawn_count = std::min(spawn_count, (int)pool->spawn_mods.size());
	craft_count = std::min(craft_count, (int)pool->crafted_mods.size());

	// Adjust total count within [min_mods, max_mods]
	int total = perm_count + spawn_count + craft_count;
	if (total < lvl.min_mods)
	{
		int needed = lvl.min_mods - total;
		int add_craft = std::min(needed, (int)pool->crafted_mods.size() - craft_count);
		craft_count += add_craft;
		needed -= add_craft;
		int add_spawn = std::min(needed, (int)pool->spawn_mods.size() - spawn_count);
		spawn_count += add_spawn;
	}
	else if (total > lvl.max_mods)
	{
		int excess = total - lvl.max_mods;
		int sub_craft = std::min(excess, craft_count);
		craft_count -= sub_craft;
		excess -= sub_craft;
		int sub_spawn = std::min(excess, spawn_count);
		spawn_count -= sub_spawn;
		excess -= sub_spawn;
		int sub_perm = std::min(excess, perm_count);
		perm_count -= sub_perm;
	}

	std::unordered_set<int> chosen_indices;
	// Order: implicit -> permanent -> spawn -> crafted
	roll_category_mods(s, perm_count, pool->permanent_mods, pool->permanent_table, chosen_indices, item.mods);
	roll_category_mods(s, spawn_count, pool->spawn_mods, pool->spawn_table, chosen_indices, item.mods);
	roll_category_mods(s, craft_count, pool->crafted_mods, pool->crafted_table, chosen_indices, item.mods);

	if (free_pool_needed)
	{
		delete pool;
	}

	return item;
}

drops_t generate_drops(const loot_context_t& context, int items, int currency_count, const restrict_drop_t* keystone)
{
	assert(context.cache);
	seed s = seed_random(); // for non-deterministic rolls
	drops_t drops;

	// Items
	drops.items.resize(items);
	for (int i = 0; i < items; i++)
	{
		const restrict_drop_t* filter = (keystone != nullptr && i == 0) ? keystone : nullptr;
		drops.items.at(i) = create_item(context, filter);
	}

	// Currencies
	if (context.cache->currencies == nullptr) return drops; // no valid currencies
	drops.currencies.resize(currency_count);
	while (currency_count)
	{
		const int index = context.cache->currencies->roll(s);
		const int max_stack = currencies.at(index).max_stack;
		const int amount = s.quadratic_weighted_roll(max_stack - 1) + 1;
		drops.currencies[currency_count - 1] = { (uint16_t)index, (uint16_t)amount };
		currency_count--;
	}
	return drops;
}

bool read_items(const char* path)
{
	if (item_table)
	{
		delete item_table;
		item_table = nullptr;
	}
	item_types.clear();
	item_defs.clear();
	item_weights.clear();

	csv::CSVReader reader(path);

	for (auto& row : reader)
	{
		std::string type = row["Type"].get<>();
		if (type.empty()) continue;
		if (std::find(item_types.begin(), item_types.end(), type) == item_types.end())
		{
			item_def_t def;
			def.name = type;
			def.classification = row["Classification"].get<>();
			def.location = row["Location"].get<>();
			def.weighting = 100;
			if (row["Weighting"].is_int()) def.weighting = row["Weighting"].get<int>();

			item_types.push_back(type);
			item_defs.push_back(def);
			item_weights.push_back(def.weighting);
		}
	}

	if (!item_weights.empty())
	{
		item_table = new const_roll_table(item_weights);
	}

	return (item_table != nullptr);
}

bool read_currencies(const char* path)
{
	// This really shouldn't happen, but for completeness and testing clear all the things:
	if (currency_table) delete currency_table;
	if (currency_weights.size() > 0) currency_weights.clear();
	if (currencies.size() > 0) currencies.clear();
	if (currency_types.size() > 0) currency_types.clear();

	csv::CSVReader reader(path);
	for (auto& row : reader)
	{
		currency_type_t v;
		std::string type = row["Type"].get<>();
		if (type != "Currency") continue;
		v.name = row["Name"].get<>();
		v.weighting = row["Weighting"].get<int>();
		if (row["Min abyss level"].is_int()) v.min_level = row["Min abyss level"].get<int>();
		if (row["Max stack size"].is_int()) v.max_stack = row["Max stack size"].get<int>();
		// TBD Handle Tags
		// TBD Handle Description
		currencies.push_back(v);
		currency_types.push_back(v.name);
		currency_weights.push_back(v.weighting);
	}
	currency_table = new const_roll_table(currency_weights);
	return true;
}

const std::vector<std::string>& get_item_types()
{
	return item_types;
}

const std::vector<std::string>& get_currency_types()
{
	return currency_types;
}
