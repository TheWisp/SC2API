# SC2API

SC2API is a free C++ framework intended for AI projects to interact with Blizzard's RTS game, Starcraft 2. The library handles game state in a similar way to [BWAPI](https://github.com/bwapi/bwapi) that information hidden to local player remain unexposed to the framework.

SC2API is a third party "hack" of the game based on reverse engineering, thus violates EULA of the game. In order to protect the game from potential reuse SC2API as maphack, the internal code of the library is kept not open sourced, instead, binary versions of the library (dll) are provided, and kept up to date with game version. 

## Supported game client
StarCraft II 3.2.1 (41973), 32 bit Windows
64 bit and OSX support are still work in progress.

## How to use

ExampleAI solution can be built with VS2015, by default the outputs files are at Binaries/$(Configuration), that's also where other necessary SC2API files are located. Currently, SC2API only supports SC2 32 bit, windows client. Run SC2APILoader.exe and then run Starcraft 2 **as 32 bit**[How to launch game as 32 bit](https://eu.battle.net/support/en/article/6680), the loader program would load SC2API.dll into the game, which then loads ExampleAI.dll.

If you create your own project using SC2API, please make sure the result dll is put along with other necessary files. You might need to open DreamSC2.ini and DreamSC2Default.ini to change the following section,

[SC2API]
AIModule=ExampleAI

where the value is name of the dll to be loaded.