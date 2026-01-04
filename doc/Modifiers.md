ARGP Modifier Management
========================

This module takes care of the large amount of `modifiers` that a game can
have and the wide variety of different targets these may have.

Modifiers are defined in CSV files.

Each modifier must have
* A description text.
* One primary value that may have a randomized roll range. It is referenced
  in the modifier description by either a % (if it is a relative unit of
  change, eg a percentage), or a `#` (if it is an absolute unit of change).
* A weighting. Modifiers are usually added randomly, and this is the roll
  weighting used for this randomness roll. A low value means it is more rare.

Each modifier may have
* A name. This can be used to link modifiers or to name them in game.
* A variance. This is referenced in the description inside a `<` and `>` pair.
* A mod category. This determines how and if the player can modify the modifier.
  If it is `permanent` or `none` then the player cannot modify it. If it is
  `permanent` it is expected that it is also deterministically generated.
  If it is `spawn` then it can only be generated on initial object creation,
  and not later. If it is `crafted` it can be changed, added or removed by
  the player later. If it is `implicit` then it should follow the object type.
* The `Requires` column gives a requirement for another modifier to be present
  before it can be added, to add a dependency chain.
* The `Cap` column and `max_count` field gives the maximum amount of this type
  of modifier that can be added to any object. This is 1 by default.
