#include <assert.h>
#include <stdio.h>

#include "mods.h"

int main(int argc, char** argv)
{
	if (argc < 2)
	{
		printf("Usage: %s <csv filename>\n", argv[0]);
		exit(-1);
	}

	bool r = read_mods(argv[1]);
	assert(r);
	const mod_data& d = get_mod_data(0);
	assert(!d.description.empty()); // the only require field
	return 0;
}
