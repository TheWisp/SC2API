#pragma once
#include <SC2API/include/SC2API.h>
#include <SC2API/include/SC2APIGame.h>
#include <SC2API/include/SC2APIUnit.h>
#include <SC2API/include/SC2APIPlayer.h>
#include <SC2API/include/Utils.h>
using namespace SC2API;

namespace ExampleAI
{
    class GameInstance
        : public Singleton<GameInstance>
        , public SignalObject
    {
    public:

        void OnUnitCreated(Unit eventUnit, int eventPlayerId)
        {
            LogLoader("Unit is created: " + eventUnit.ToString());
        }

        void OnUnitDestroyed(Unit eventUnit, Optional<Unit> killerUnit)
        {
            if (killerUnit.hasValue())
            {
                LogLoader("Unit " + eventUnit.ToString() + " is killed by " + killerUnit.value().ToString());
            }
            else
            {
                LogLoader("Unit " + eventUnit.ToString() + " is destroyed");
            }
        }

        void OnUnitEnterVision(Unit eventUnit)
        {
            LogLoader("Unit enters vision: " + eventUnit.ToString());
        }
        
        void OnUnitLeaveVision(Unit eventUnit)
        {
            LogLoader("Unit leaves vision: " + eventUnit.ToString());
        }

        GameInstance()
        {
            LogLoader("Game has started.");

            Unit::SignalUnitCreated().connect(this, &GameInstance::OnUnitCreated);
            Unit::SignalUnitDestroyed().connect(this, &GameInstance::OnUnitDestroyed);
            Unit::SignalUnitEnterVision().connect(this, &GameInstance::OnUnitEnterVision);
            Unit::SignalUnitLeaveVision().connect(this, &GameInstance::OnUnitLeaveVision);

            LogLoader("Player 1 race: " + PlayerLobbyRace(1));
            LogLoader("Player 2 race: " + PlayerLobbyRace(2));
            
        }

        ~GameInstance()
        {
            LogLoader("Game has ended.");
        }
    };
}
