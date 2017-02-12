/* Cheats to help debugging / testing features. Only use in single player mode and never when more than one human player is in game! */
#pragma once
#include "SC2API.h"
#include "SC2APIUnit.h"
#include "SC2APIPoint.h"
#include <string>

namespace SC2API
{
	/// <summary>
	/// Sets game speed to desired factor. Only valid when the game is single player. Supports maximum factor = 256.
	/// </summary>
	SC2API_API void CheatSetGameSpeed(float factor);

	/// <summary>
	/// Creates a unit at given point. Only valid when the game is single player.
	/// </summary>
	SC2API_API Unit CheatCreateUnit(const std::string& inUnitType, Point inLocation);

	/// <summary>
	/// Removes a unit. Only valid when the game is single player.
	/// </summary>
	SC2API_API void CheatRemoveUnit(const Unit& inUnit);

	/// <summary>
	/// Adds given amount of supply limit. Only valid when the game is single player.
	/// </summary>
	SC2API_API void CheatSupply(int amount);
}