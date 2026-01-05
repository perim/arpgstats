#include "item.h"

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
