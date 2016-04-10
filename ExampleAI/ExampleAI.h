#pragma once
#include <SC2API/include/SC2API.h>
#include <SC2API/include/SC2APIGame.h>
#include <SC2API/include/SC2APIUnit.h>
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
            Game::OutputScreen("Unit is created: " + eventUnit.ToString());
        }

        void OnUnitDestroyed(Unit eventUnit, Optional<Unit> killerUnit)
        {
            if (killerUnit.hasValue())
            {
                Game::OutputScreen("Unit " + eventUnit.ToString() + " is killed by " + killerUnit.value().ToString());
            }
            else
            {
                Game::OutputScreen("Unit " + eventUnit.ToString() + " is destroyed");
            }
        }

        void OnUnitEnterVision(Unit eventUnit)
        {
            Game::OutputScreen("Unit enters vision: " + eventUnit.ToString());
        }
        
        void OnUnitLeaveVision(Unit eventUnit)
        {
            Game::OutputScreen("Unit leaves vision: " + eventUnit.ToString());
        }

        GameInstance()
        {
            Game::OutputScreen("entered game!!!");

            Unit::SignalUnitCreated().connect(this, &GameInstance::OnUnitCreated);
            Unit::SignalUnitDestroyed().connect(this, &GameInstance::OnUnitDestroyed);
            Unit::SignalUnitEnterVision().connect(this, &GameInstance::OnUnitEnterVision);
            Unit::SignalUnitLeaveVision().connect(this, &GameInstance::OnUnitLeaveVision);
        }

        ~GameInstance()
        {
            Game::OutputScreen("end game!!!");
        }
    };
}
