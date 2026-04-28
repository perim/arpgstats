#include "creature.h"
#include "misc.h"

#include "external/csv.hpp"

struct creature_type_t
{
	std::string name;
	std::string species;
	int weighting = -1;
	int min = -1;
	int max = -1;
	int move = 100;
	int physical = 100;
	damage_t element = damage_t::Physical;
	int elemental = 0;
	int dot_duration = 100;
	int attack_speed = 100;
	int range = 10;
	int ammo = -1;
	int health = 100;
	int energy = 100;
	int stamina = 100;
	int block_speed = 100;
	int reload = 100;
	int max_level = -1;
	int min_level = -1;
	std::string tags; // TBD list
};

static std::vector<creature_type_t> creatures;

void init_creature_cache(creature_context_t& ctx)
{
}

void free_creature_cache(creature_context_t& ctx)
{
}

bool read_creatures(const char* path)
{
	csv::CSVReader reader(path);
	for (auto& row : reader)
	{
		creature_type_t v;
		v.name = row["Type"].get<>();
		v.species = row["Species"].get<>();
		v.weighting = row["Weighting"].get<int>();
		v.min = row["Min"].get<int>();
		v.max = row["Max"].get<int>();
		v.move = row["Move"].get<int>();
		v.physical = row["Phys"].get<int>();
		std::string elem = row["Element"].get<>();
		v.element = to_damage_type(elem);
		v.elemental = row["Res+Dam"].get<int>(); // damage + resist
		v.dot_duration = row["DoT secs"].get<int>();
		v.attack_speed = row["Attack speed"].get<int>();
		v.range = row["Range"].get<int>();
		if (row["Ammo"].is_int()) v.ammo = row["Ammo"].get<int>();
		if (row["Health"].is_int()) v.health = row["Health"].get<int>();
		if (row["Energy"].is_int()) v.energy = row["Energy"].get<int>();
		if (row["Stamina"].is_int()) v.stamina = row["Stamina"].get<int>();
		if (row["Block speed"].is_int()) v.block_speed = row["Block speed"].get<int>();
		if (row["Reload"].is_int()) v.reload = row["Reload"].get<int>();
		if (row["Min level"].is_int()) v.min_level = row["Min level"].get<int>();
		if (row["Max level"].is_int()) v.max_level = row["Max level"].get<int>();
		v.tags = row["Tags"].get<>(); // TBD decompose into a list
		creatures.push_back(v);
	}
	return true;
}

creature_t create_creature(const creature_context_t& context)
{
	return creature_t{};
}
