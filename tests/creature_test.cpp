#include "creature.h"

#include <assert.h>

int main(int argc, char** argv)
{
	seed s(0);
	const char* creaturecsv = "data/test/monsters.csv";

	bool r = read_creatures(creaturecsv);
	assert(r);

	struct creature_context_t ctx(s);
	init_creature_cache(ctx);

	creature_t mob = create_creature(ctx);
	(void)mob;

	free_creature_cache(ctx);
	return 0;
}
