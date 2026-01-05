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
	assert(!d.description.empty()); // the only required field

	const std::vector<std::string>& types = get_mod_types();
	assert(types.size() > 0);

	const std::vector<std::string>& classifications = get_mod_classifications();
	assert(classifications.size() > 0);

	auto idx = get_classification_index("Craft");
	assert(idx);

	return 0;
}
