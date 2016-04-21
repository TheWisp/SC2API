#pragma once
#include "SC2APIUnitFilterFlag.dependency.h"
namespace SC2API
{
    enum class UnitFilterFlag : int64_t
    {
        Null                = 0,
        Self                = 1ULL << 0, //Self
        Player              = 1ULL << 1, //Player
        Ally                = 1ULL << 2, //Ally
        Neutral             = 1ULL << 3, //Neutral
        Enemy               = 1ULL << 4, //Enemy
        Air                 = 1ULL << 5, //Air
        Ground              = 1ULL << 6, //Ground
        Light               = 1ULL << 7, //Light
        Armored             = 1ULL << 8, //Armored
        Biological          = 1ULL << 9, //Biological
        Robotic             = 1ULL << 10, //Robotic
        Mechanical          = 1ULL << 11, //Mechanical
        Psionic             = 1ULL << 12, //Psionic
        Massive             = 1ULL << 13, //Massive
        Structure           = 1ULL << 14, //Structure
        Hover               = 1ULL << 15, //Hover
        Heroic              = 1ULL << 16, //Heroic
        MapObject           = 1ULL << 17, //Map Object
        Worker              = 1ULL << 18, //Worker
        RawResource         = 1ULL << 19, //Raw Resource
        HarvestableResource = 1ULL << 20, //Harvestable Resource
        Missile             = 1ULL << 21, //Missile
        Destructible        = 1ULL << 22, //Destructible
        Item                = 1ULL << 23, //Item
        Uncommandable       = 1ULL << 24, //Uncommandable
        CanHaveEnergy       = 1ULL << 25, //Can Have Energy
        CanHaveShields      = 1ULL << 26, //Can Have Shields
        PreventDefeat       = 1ULL << 27, //Prevent Defeat
        PreventReveal       = 1ULL << 28, //Prevent Reveal
        Buried              = 1ULL << 29, //Buried
        Cloaked             = 1ULL << 30, //Cloaked
        Visible             = 1ULL << 31, //Visible
        Stasis              = 1ULL << 32, //Stasis
        UnderConstruction   = 1ULL << 33, //Under Construction
        Dead                = 1ULL << 34, //Dead
        Revivable           = 1ULL << 35, //Revivable
        Hidden              = 1ULL << 36, //Hidden
        Hallucination       = 1ULL << 37, //Hallucination
        Invulnerable        = 1ULL << 38, //Invulnerable
        HasEnergy           = 1ULL << 39, //Has Energy
        HasShields          = 1ULL << 40, //Has Shields
        Benign              = 1ULL << 41, //Benign
        Passive             = 1ULL << 42, //Passive
        Detector            = 1ULL << 43, //Detector
        Radar               = 1ULL << 44, //Radar
        Stunned             = 1ULL << 45, //Stunned
        Summoned            = 1ULL << 46, //Summoned
        Unstoppable         = 1ULL << 47, //Unstoppable
        Resistant           = 1ULL << 49, //Resistant
    };
    ENUM_CLASS_FLAGS(UnitFilterFlag)
}
