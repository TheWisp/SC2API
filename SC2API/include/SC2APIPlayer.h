#pragma once
#include "SC2API.h"

namespace SC2API
{
    /// <summary>
    /// Returns the race of the player chosen from lobby.
    /// </summary>
    /// <returns>Terran, Zerg, Protoss or Random, see SC2API::Races</returns>
    SC2API_API std::string PlayerLobbyRace(int inPlayer);
}
