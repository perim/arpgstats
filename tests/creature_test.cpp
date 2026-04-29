#include "creature.h"

#include <assert.h>

int main(int argc, char** argv)
{
	seed s(0);

	if (argc < 2)
	{
		printf("Usage: %s <csv creature file>\n", argv[0]);
		exit(-1);
	}

	bool r = read_creatures(argv[1]);
	assert(r);

	struct creature_context_t ctx(s);
	init_creature_cache(ctx);

	creature_t mob = create_creature(ctx);
	(void)mob;

	free_creature_cache(ctx);
	return 0;
}
