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
static std::vector<int> creature_weights;

const std::string& get_creature_name(int index)
{
	return creatures.at(index).name;
}

struct creature_cache
{
	filtered_const_roll_table* creatures = nullptr;

	~creature_cache()
	{
		delete creatures;
	}
};

bool init_creature_cache(creature_context_t& ctx)
{
	assert(!creatures.empty());
	free_creature_cache(ctx);
	ctx.cache = new creature_cache;
	std::vector<bool> mask(creatures.size());
	bool has_any = false;
	for (unsigned i = 0; i < creatures.size(); i++)
	{
		const creature_type_t& c = creatures.at(i);
		mask[i] = ctx.depth >= c.min_level && (c.max_level < 0 || ctx.depth <= c.max_level);
		has_any |= mask[i];
	}
	assert(has_any);
	if (has_any) ctx.cache->creatures = new filtered_const_roll_table(creature_weights, mask);
	return has_any;
}

void free_creature_cache(creature_context_t& ctx)
{
	if (ctx.cache)
	{
		delete ctx.cache;
		ctx.cache = nullptr;
	}
}

bool read_creatures(const char* path)
{
	creatures.clear();
	creature_weights.clear();
	csv::CSVReader reader(path);
	for (auto& row : reader)
	{
		creature_type_t v;
		v.name = row["Type"].get<>();
		if (v.name.empty()) continue; // skip sum rows
		v.species = row["Species"].get<>();
		if (row["Weighting"].is_int()) v.weighting = row["Weighting"].get<int>();
		if (row["Min"].is_int()) v.min = row["Min"].get<int>();
		if (row["Max"].is_int()) v.max = row["Max"].get<int>();
		if (row["Move"].is_int()) v.move = row["Move"].get<int>();
		if (row["Phys"].is_int()) v.physical = row["Phys"].get<int>();
		std::string elem = row["Element"].get<>();
		if (!elem.empty()) v.element = to_damage_type(elem);
		if (row["Res+Dam"].is_int()) v.elemental = row["Res+Dam"].get<int>(); // damage + resist
		if (row["DoT secs"].is_int()) v.dot_duration = row["DoT secs"].get<int>();
		if (row["Attack speed"].is_int()) v.attack_speed = row["Attack speed"].get<int>();
		if (row["Range"].is_int()) v.range = row["Range"].get<int>();
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
		creature_weights.push_back(v.weighting > 0 ? v.weighting : 100);
	}
	printf("Added %d creatures\n", (int)creatures.size());
	return creatures.size() > 0;
}

creature_t create_creature(const creature_context_t& context)
{
	assert(context.cache && context.cache->creatures);
	seed s = context.rand;
	const uint16_t which = context.cache->creatures->roll(s);
	creature_t c{ which };
	return c;
}
