#include "creature.h"

#include <assert.h>

int main(int argc, char** argv)
{
	seed s = seed_random();
	const char* creaturecsv = "data/test/monsters.csv";

	bool r = read_creatures(creaturecsv);
	assert(r);

	struct creature_context_t ctx(s);
	r = init_creature_cache(ctx);
	assert(r);

	creature_t c = create_creature(ctx);
	printf("Created %s\n", get_creature_name(c.creature_type).c_str());

	free_creature_cache(ctx);
	return 0;
}
