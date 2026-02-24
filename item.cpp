#include "item.h"

#include "external/csv.hpp"

struct currency_type_t
{
	std::string name;
	int weighting = -1;
	int min_level = -1;
	int max_stack = -1;
	// TBD tags
	// TBD description
};

static std::vector<std::string> item_types;
static std::vector<std::string> currency_types;
static std::vector<currency_type_t> currencies;
static const_roll_table* currency_table = nullptr;

item_t create_item(const loot_context_t& context, const restrict_drop_t* filter)
{
	item_t item;
	item.mods.resize(1);
	return item;
}

drops_t generate_drops(const loot_context_t& context, int items, int currency_count, const restrict_drop_t* keystone)
{
	seed s = seed_random(); // only keystone drop will use deterministic rolls
	drops_t drops;
	drops.items.resize(items);
	for (int i = 0; i < items; i++)
	{
		const restrict_drop_t* filter = (keystone != nullptr && i == 0) ? keystone : nullptr;
		drops.items.at(i) = create_item(context, filter);
	}
	drops.currencies.resize(currency_count);
	while (currency_count)
	{
		const int index = currency_table->roll(s);
		const int min_level = currencies.at(index).min_level;
		if (context.level_modifiers && context.level_modifiers->depth < min_level) continue; // try again
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
	csv::CSVReader reader(path);
	if (currency_table) delete currency_table;
	std::vector<int> weights;

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
		weights.push_back(v.weighting);
	}
	currency_table = new const_roll_table(weights);
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
