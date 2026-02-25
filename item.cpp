#include "item.h"

#include "external/csv.hpp"

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

static std::vector<std::string> item_types;
static std::vector<std::string> currency_types;
static std::vector<currency_type_t> currencies;
static const_roll_table* currency_table = nullptr;
static std::vector<int> currency_weights;

struct item_cache
{
	filtered_const_roll_table* currencies = nullptr;
};

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
}

/// Free an item cache
void free_item_cache(loot_context_t& ctx)
{
	if (ctx.cache)
	{
		delete ctx.cache->currencies;
		delete ctx.cache;
		ctx.cache = nullptr;
	}
}

item_t create_item(const loot_context_t& context, const restrict_drop_t* filter)
{
	item_t item;
	item.mods.resize(1);
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
	csv::CSVReader reader(path);

	for (auto& row : reader)
	{
		// TBD
	}

	return true;
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
