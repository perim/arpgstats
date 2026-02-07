ARGP Item Generation
====================

Generate item drops based on CSV definitions. Different modifier types
on these items depend on how pseudorandom generation impacts them. The
idea is that if you replay a level multiple times, some modifiers will
drop the same, others will vary.

## Basic item types

An item can be `equippable`, `placeable`, `unlockable`, or `consumable`.
Each category may have different drop rates. `consumable` also has a
quantity, ie it drops with a number of uses or charges. All other item
types are singular.

An item can also be either `unique` or `normal`. A `unique` item will
be pseudorandomly generated with some pseudorandom modifiers and some
random modifiers. A `normal` item will be randomly generated and only
have random modifiers.

Pseudorandom modifiers are called `permanent` modifiers while random
modifiers are called either `crafted` or `spawn` modifiers. If it is
`crafted` then these are modifiers that can also be added by the
player (e.g. through crafting), while `spawn` modifiers can only be
found in item drops.

## Keystone items

We have a special drop type called a keystone item which the player
can modify more than other drops. The item generation for these are
not supposed to be entirely pseudo-random or random, but rather
influenced by player actions.
