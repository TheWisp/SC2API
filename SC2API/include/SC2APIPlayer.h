#pragma once
#include "SC2API.h"

namespace SC2API
{
	/// <summary>
	/// Gets the local player's id.
	/// </summary>
	SC2API_API int PlayerLocal();

    /// <summary>
    /// Returns the actual race of local player. The race is known even if random was chosen from the lobby.
    /// </summary>
    /// <returns>Terran, Zerg or Protoss, see SC2API::Races</returns>
    SC2API_API std::string PlayerLocalRace();

    /// <summary>
    /// Returns the race of the player chosen from lobby.
    /// </summary>
    /// <returns>Terran, Zerg, Protoss or Random, see SC2API::Races</returns>
    SC2API_API std::string PlayerLobbyRace(int inPlayer);
}
