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

item_t create_item(const loot_context_t& context, const restrict_drop_t* filter)
{
	item_t item;
	item.mods.resize(1);
	return item;
}

drops_t generate_drops(const loot_context_t& context, int items, int currencies, const restrict_drop_t* keystone)
{
	drops_t drops;
	drops.items.resize(items);
	drops.currencies.resize(currencies);
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
	}

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
