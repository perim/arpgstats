ARPG Stats Library
==================

This is a library for doing some of the internal game state calculations for an action
role-playing game (ARPG). It has a number of different interconnected modules.

These are:
* [The character ability management module](doc/Abilities.md)
* [The modifiers management module](doc/Modifiers.md)
* Coming soon: Item generation module

The library is written to be fast and there are some sacrifices made to ensure that
this is the case. The game is assumed to require fully deterministic operation across
a variety of platforms, and as such avoids the use of floating point operations or any
other platform-dependent sources of non-determinism.
