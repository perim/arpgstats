#include "creature.h"

#include <assert.h>

static void test_read_roles()
{
	assert(get_creature_count() > 0);
	assert(get_creature_index("Firewraith") >= 0);
	assert(get_creature_index("No Such Creature") < 0);

	const int firewraith = get_creature_index("Firewraith");
	const int burningzombie = get_creature_index("Burning Zombie");
	const creature_role_t* leader = get_creature_role(firewraith, role_type::leader);
	assert(leader);
	assert(leader->scaling == perten_from_percent(750));
	assert(leader->creature_type == (uint16_t)firewraith); // no "Filled by" for this row

	const creature_role_t* damage = get_creature_role(firewraith, role_type::damage);
	assert(damage);
	assert(damage->scaling == perten_from_percent(150));
	assert(damage->creature_type == (uint16_t)burningzombie);

	assert(get_creature_role(firewraith, role_type::tank));
	assert(get_creature_role(firewraith, role_type::support));
	assert(get_creature_role(firewraith, role_type::boss)); // defined, but must never appear in a tribe
	assert(!get_creature_role(firewraith, role_type::invalid));

	// Bones defines no leader role
	const int bones = get_creature_index("Bones");
	assert(bones >= 0);
	assert(!get_creature_role(bones, role_type::leader));
}

/// Damage is the filler role, specialist roles max one each, and a leader needs followers.
static void test_tribe_role_assignment()
{
	struct creature_context_t ctx(seed_random());
	const bool cache_ok = init_creature_cache(ctx);
	assert(cache_ok);

	for (int t = 0; t < get_creature_count(); t++)
	{
		for (int count = 1; count <= 32; count++)
		{
			tribe_t tribe{ -1, (uint16_t)t, seed(1), position{ 0, 0, 0, 0 }, (uint16_t)count };
			const std::vector<creature_t> members = create_creature(ctx, tribe);
			assert(members.size() == (size_t)count);
			int leaders = 0, tanks = 0, supports = 0, damages = 0;
			for (const creature_t& m : members)
			{
				switch (m.role)
				{
					case role_type::leader: leaders++; break;
					case role_type::tank: tanks++; break;
					case role_type::support: supports++; break;
					case role_type::damage: damages++; break;
					default: assert(!"bosses and invalid roles never appear in tribes");
				}
				// creature and scaling come from the role definition, or from the default
				const creature_role_t* def = get_creature_role(t, m.role);
				if (def)
				{
					assert(m.creature_type == def->creature_type);
					assert(m.scaling == def->scaling);
				}
				else
				{
					assert(m.creature_type == (uint16_t)t);
					assert(m.scaling == perten_full);
				}
			}
			assert(leaders <= 1); // at most one of each specialist role
			assert(tanks <= 1);
			assert(supports <= 1);
			assert(damages >= 1); // damage always fills the rest
			if (leaders) assert(members.at(0).role == role_type::leader); // the leader is member 0
			if (!get_creature_role(t, role_type::leader) || count < 2) assert(leaders == 0); // a leader needs followers
		}
	}
	free_creature_cache(ctx);
}

static void test_create_tribe_deterministic()
{
	seed s = seed_random();
	struct creature_context_t ctx(s);
	const bool cache_ok = init_creature_cache(ctx);
	assert(cache_ok);

	const position pos{ 7, 9, 12, 3 };
	const tribe_t a = create_tribe(ctx, pos);
	const tribe_t b = create_tribe(ctx, pos);
	assert(a.creature_type == b.creature_type); // same position, same tribe
	assert(a.count == b.count);
	assert(a.leader == b.leader);
	assert(a.pos == b.pos);
	assert(a.count >= 1 && a.count <= 32);
	assert(a.creature_type < (uint16_t)get_creature_count());

	// the leader field agrees with the composition
	if (a.leader == 0)
		assert(create_creature(ctx, a).at(0).role == role_type::leader);
	else
		assert(create_creature(ctx, a).at(0).role != role_type::leader);

	// all members can be regenerated, with the same result each time
	const std::vector<creature_t> members = create_creature(ctx, a);
	const std::vector<creature_t> members2 = create_creature(ctx, a);
	assert(members.size() == members2.size());
	printf("Tribe %s x%d at %d,%d,%d,%d:\n", get_creature_name(a.creature_type).c_str(),
	       a.count, a.pos.x, a.pos.y, a.pos.z, a.pos.w);
	for (size_t i = 0; i < members.size(); i++)
	{
		assert(members.at(i).creature_type == members2.at(i).creature_type);
		assert(members.at(i).role == members2.at(i).role);
		assert(members.at(i).scaling == members2.at(i).scaling);
		printf("\t[%d] %s role=%s scaling=%d%%\n", (int)i, get_creature_name(members.at(i).creature_type).c_str(),
		       role_to_string(members.at(i).role), perten_to_percent(members.at(i).scaling));
	}

	free_creature_cache(ctx);
}

int main(int argc, char** argv)
{
	bool r = read_creatures("data/test/monsters.csv");
	assert(r);
	r = read_creature_roles("data/test/monster_roles.csv");
	assert(r);

	test_read_roles();
	test_tribe_role_assignment();
	test_create_tribe_deterministic();
	return 0;
}
