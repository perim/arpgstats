#include "item.h"

#include <assert.h>

int main(int argc, char** argv)
{
	seed s(0);

	if (argc < 3)
	{
		printf("Usage: %s <csv item file> <csv currency file\n", argv[0]);
		exit(-1);
	}

	bool r = read_items(argv[1]);
	assert(r);

	r = read_currencies(argv[2]);
	assert(r);

	const std::vector<std::string>& item_types = get_item_types();
	//assert(types.size() > 0);

	const std::vector<std::string>& currency_types = get_currency_types();
	assert(currency_types.size() > 0);

	level_loot_context_t level_modifiers;
	item_luck_t player_modifiers;
	struct loot_context_t ctx(s);
	ctx.level_modifiers = &level_modifiers;
	ctx.player_modifiers = &player_modifiers;
	init_item_cache(ctx);

	item_t item = create_item(ctx, nullptr);
	assert(item.mods.size() > 0);
	(void)item;

	drops_t drops = generate_drops(ctx, 1, 3, nullptr);
	assert(drops.items.size() == 1);
	assert(drops.currencies.size() == 3);
	printf("Dropped:\n");
	for (unsigned i = 0; i < 3; i++)
	{
		assert(drops.currencies[i].amount > 0);
		assert(drops.currencies[i].type < currency_types.size());
		printf("\tCurrency: %d %s\n", (int)drops.currencies[i].amount, currency_types[drops.currencies[i].type].c_str());
	}
	(void)drops;

	free_item_cache(ctx);
	return 0;
}
