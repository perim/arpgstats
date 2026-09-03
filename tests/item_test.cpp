#include "item.h"

#include "util.h"

#include <assert.h>
#include <stdio.h>

static bool verbose = false;
static int depth = 1; // current depth level, starting from 1
static uint64_t seed_value = 0;
static bool has_seed = false; // if false, generate a random seed with seed_random()

static void print_drops(const drops_t& drops)
{
	const std::vector<std::string>& item_types = get_item_types();
	assert(item_types.size() > 0);
	const std::vector<std::string>& currency_types = get_currency_types();
	assert(currency_types.size() > 0);
	printf("Dropped:\n");
	for (unsigned i = 0; i < drops.items.size(); i++)
	{
		const item_t& item = drops.items[i];
		assert(item.item_type < item_types.size());
		printf("\tItem: %s (%u mods)\n", item_types[item.item_type].c_str(), (unsigned)item.mods.size());
		for (unsigned j = 0; j < item.mods.size(); j++)
		{
			const mod& m = item.mods[j];
			printf("\t\t[%s] %s\n", get_mod_category_name(m.category), format_mod_text(m).c_str());
		}
	}
	for (unsigned i = 0; i < drops.currencies.size(); i++)
	{
		assert(drops.currencies[i].amount > 0);
		assert(drops.currencies[i].type < currency_types.size());
		printf("\tCurrency: %d %s\n", (int)drops.currencies[i].amount, currency_types[drops.currencies[i].type].c_str());
	}
}

void usage()
{
	printf("item_test [options]\n");
	printf("\n");
	printf("Options:\n");
	printf("    -h/--help                This help\n");
	printf("    -v/--verbose             Verbose output\n");
	printf("    -d/--depth <level>       Current depth, filters loot (default 1)\n");
	printf("    -s/--seed <value>        Seed for deterministic rolls (live random if not given)\n");
	exit(-1);
}

int main(int argc, char** argv)
{
	int remaining = argc - 1; // zeroth is name of program

	for (int i = 1; i < argc; i++)
	{
		if (match(argv[i], "-h", "--help", remaining))
		{
			usage();
		}
		else if (match(argv[i], "-v", "--verbose", remaining))
		{
			verbose = true;
		}
		else if (match(argv[i], "-d", "--depth", remaining))
		{
			depth = get_int(argv[i + 1], remaining);
			i++; // skip the value
			if (depth < 1)
			{
				printf("Depth must be at least 1\n\n");
				usage();
			}
		}
		else if (match(argv[i], "-s", "--seed", remaining))
		{
			seed_value = get_uint64(argv[i + 1], remaining);
			has_seed = true;
			i++; // skip the value
		}
		else if (remaining > 0)
		{
			printf("Invalid option\n\n");
			usage();
		}
	}

	seed s = has_seed ? seed(seed_value) : seed_random();
	const char *modscsv = "data/test/combined.csv";
	const char *itemcsv = "data/test/items.csv";
	const char *currencycsv = "data/test/currencies.csv";

	bool r = read_mods(modscsv);
	assert(r);
	assert(get_mod_count() > 0);

	r = read_items(itemcsv);
	assert(r);
	const std::vector<std::string>& item_types = get_item_types();
	assert(item_types.size() > 0);

	r = read_currencies(currencycsv);
	assert(r);
	const std::vector<std::string>& currency_types = get_currency_types();
	assert(currency_types.size() > 0);

	level_loot_context_t level_modifiers;
	level_modifiers.depth = depth;
	item_luck_t player_modifiers;
	struct loot_context_t ctx(s);
	ctx.level_modifiers = &level_modifiers;
	ctx.player_modifiers = &player_modifiers;
	init_item_cache(ctx);

	// Test single item generation
	item_t item = create_item(ctx, nullptr);
	assert(item.mods.size() > 0);
	assert(item.item_type < item_types.size());
	for (const auto& m : item.mods)
	{
		const mod_data& md = get_mod_data(m.type);
		if (md.max >= md.min && md.max > 0)
		{
			assert(m.roll >= md.min && m.roll <= md.max);
		}
		std::string formatted = format_mod_text(m);
		assert(!formatted.empty());
	}

	// Test drops generation with items and currencies
	drops_t drops = generate_drops(ctx, 3, 3, nullptr);
	assert(drops.items.size() == 3);
	assert(drops.currencies.size() == 3);
	print_drops(drops);

	// Test unique drop via restrict_drop_t
	restrict_drop_t keystone_filter{};
	keystone_filter.drop_type = item_drop_type_t::unique;
	keystone_filter.item_type = 0; // Sword
	drops_t unique_drops = generate_drops(ctx, 1, 1, &keystone_filter);
	assert(unique_drops.items.size() == 1);
	assert(unique_drops.items[0].item_type == 0);
	print_drops(unique_drops);

	free_item_cache(ctx);
	return 0;
}
