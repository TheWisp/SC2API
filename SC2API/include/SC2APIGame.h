#pragma once
#include "SC2API.h"
#include <zycore/Signal.hpp>

namespace SC2API
{
	/// <summary>
	/// Determines whether the game is in replay mode.
	/// </summary>
	SC2API_API bool GameIsReplay();

	/// <summary>
	/// Determines whether the game is single player (Only one client is running the game, so cheats and debugging are possible)
	/// </summary>
	SC2API_API bool GameIsSinglePlayer();

    #pragma region Utils

    /// <summary>
    /// Displays message only visible to local player in chat area of the screen.
    /// </summary>
    /// <param name="Message">String message to display</param>
    SC2API_API void OutputScreen(std::string Message);
    #pragma endregion

    #pragma region Events
    /// <summary>
    /// Fires when a match or replay has started.
    /// Connect: void OnMatchStart();
    /// </summary>
    /// <returns>Signal for binding</returns>
    SC2API_API Signal<>& SignalMatchStarted();

    /// <summary>
    /// Fires when a match or replay has ended.
    /// Connect: void OnMatchEnded();
    /// </summary>
    /// <returns>Signal for binding</returns>
    SC2API_API Signal<>& SignalMatchEnded();

    /// <summary>
    /// Sets a game timer to fire when time has passed.
    /// Connect: void OnTimer();
    /// </summary>
    SC2API_API Signal<>& SignalTimer(double timeOut, bool loop);
    #pragma endregion
}
