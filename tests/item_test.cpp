#include "item.h"

#include <assert.h>

int main(int argc, char** argv)
{
	seed s(0);
	struct loot_context_t ctx(s);
	item_t item = create_item(ctx, nullptr);
	assert(item.mods.size() > 0);
	(void)item;

	drops_t drops = generate_drops(ctx, 1, 1, nullptr);
	assert(drops.items.size() == 1);
	assert(drops.currencies.size() == 1);
	(void)drops;
	return 0;
}
