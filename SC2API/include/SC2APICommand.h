#pragma once
#include "SC2API.h"

namespace SC2API
{
    /// <summary>
    /// A command is the combination of an ability and an index for a command within that ability.
    /// Command indexes are 0 based.
    /// </summary>
    struct SC2API_API Command final
    {
        const char* Ability;
        int CommandIndex;
    };
}