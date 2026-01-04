#include "mods.h"
#include "external/csv.hpp"

#include <unordered_map>

#include <stdio.h>
#include <string.h>

#define BUF_SIZE 256

static std::vector<mod_data> modlist;
static std::vector<std::string> modtypes;
static std::vector<std::string> classifications;
static bool debug = false;

const mod_data& get_mod_data(int index)
{
	return modlist.at(index);
}

const std::vector<std::string>& get_mod_types()
{
	return modtypes;
}

const std::vector<std::string>& get_mod_classifications()
{
	return classifications;
}

int get_mod_index_by_name(const std::string& name)
{
	for (unsigned i = 0; i < modlist.size(); i++)
	{
		if (modlist.at(i).name == name) return i;
	}
	return -1;
}

static std::optional<unsigned> get_type_index(const std::string& v)
{
	for (unsigned i = 0; i < modtypes.size(); i++)
	{
		if (modtypes.at(i) == v) { return i; } // was found
	}
	return std::nullopt; // was not
}

std::optional<unsigned> get_classification_index(const std::string& v)
{
	for (unsigned i = 0; i < classifications.size(); i++)
	{
		if (classifications.at(i) == v) { return i; } // was found
	}
	return std::nullopt; // was not
}

bool read_mods(const char* filename)
{
	csv::CSVReader reader(filename);
	std::vector<std::string> require_list;

	for (auto& row : reader)
	{
		// First find type index
		int type_index = modtypes.size();
		std::string type = row["Type"].get<>();
		auto type_found = get_type_index(type);
		if (type_found) type_index = *type_found; // found, reuse index
		else modtypes.push_back(type); // was not found, so add it

		// Now store the data
		struct mod_data data;
		data.index = type_index;
		data.name = row["Name"].get<>();
		data.description = row["Description"].get<>();
		if (row["Min"].is_int()) data.min = row["Min"].get<int>();
		if (row["Max"].is_int()) data.max = row["Max"].get<int>();
		if (row["Weighting"].is_int()) data.weighting = row["Weighting"].get<int>();
		if (row["Cap"].is_int()) data.max_count = row["Cap"].get<int>();

		std::string category = row["Subtype"].get<>(); // TBD may want to rename in csv
		if (category.empty()) data.category = mod_category::none;
		else if (category == "Craft") data.category = mod_category::crafted;
		else if (category == "Drop") data.category = mod_category::permanent; // TBD align csv and code naming here
		else if (category == "Spawn") data.category = mod_category::spawn;
		else if (category == "Implicit") data.category = mod_category::implicit;
		else { printf("Bad mod category found: %s\n", category.c_str()); abort(); }

		// Requires temporary storage to set up with index
		std::string requires = row["Requires"].get<>();
		require_list.push_back(requires);

		// Requires temporary storage to set up with index
		std::string classification = row["Classification"].get<>();
		if (!classification.empty())
		{
			auto found = get_classification_index(classification);
			if (!found) { data.classification = classifications.size(); classifications.push_back(classification); }
			else data.classification = *found;
		}

		if (debug) printf("Found type=%s descr=%s\n", row["Type"].get<>().c_str(), row["Description"].get<>().c_str());
		modlist.push_back(data);
	}
	for (unsigned i = 0; i < require_list.size(); i++)
	{
		if (!require_list.at(i).empty())
		{
			int idx = get_mod_index_by_name(require_list.at(i));
			if (idx == -1) { printf("Bad mod requires found: %s\n", require_list.at(i).c_str()); abort(); }
			modlist.at(i).requires = idx;
		}
	}
	return true;
}
