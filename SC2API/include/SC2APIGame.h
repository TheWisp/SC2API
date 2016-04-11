#pragma once
#include "SC2API.h"
#include <zycore/Signal.hpp>

namespace SC2API
{
    #pragma region Utils

    /// <summary>
    /// Displays message in chat area of the screen.
    /// Only the local player can see displayed message.
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
    #pragma endregion
}
