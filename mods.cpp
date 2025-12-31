#include "mods.h"

#include <vector>

#include <stdio.h>

#define BUF_SIZE 256

static std::vector<mod_data> modlist;

const mod_data& get_mod_data(int index)
{
	return modlist.at(index);
}

bool read_mods(const char* filename)
{
	// Open file
	FILE* fp = fopen(filename, "r");
	if (!fp)
	{
		printf("Failed to open %s: %s\n", strerror(errno));
		exit(-1);
	}
	// Read and ignore CSV header
	fscanf(fp, "%*[^\\n]");
	// Read CSV contents
	// Type,Weighting,Subtype,Min ,Max,Description,Define + arg1 -> item mods
	// + Name and Requires -> class mods
	char buffer[BUF_SIZE];
	char type[BUF_SIZE];
	int weight;
	char subtype[BUF_SIZE];
	int min;
	int max;
	char descr[BUF_SIZE];
	char define[BUF_SIZE];
	while (fgets(buffer, sizeof(buffer), fp))
	{
		if (sscanf(fp, "%s,%d,%s,%d,%d,%s,%s", ) != )
		{
		}
	}
	// Done
	fclose(fp);
}
