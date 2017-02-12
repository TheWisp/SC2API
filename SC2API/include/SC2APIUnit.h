#pragma once
#include "SC2API.h"
#include "HandleId.h"

namespace SC2API
{
    struct Point;

    /// <summary>
    /// SC2API Unit is an unreliable shell of the actual unit, representing the observable aspect of unit.
    /// </summary>
    struct SC2API_API Unit final
    {
    public:
        #pragma region Basics
        /// <summary>
        /// Determines whether the unit can be observed by local player.
        /// An unit is accessible when it's alive and is not in fog of war or hidden.
        /// </summary>
        bool IsAccessible() const;

        /// <summary>
        /// Gets the unit type for the specified unit.
        /// </summary>
        /// <returns>If the unit is currently accessible, returns unit type, otherwise returns empty value.</returns>
        Optional<std::string> GetType() const;

        /// <summary>
        /// Gets unit's current position on the map.
        /// </summary>
        /// <returns>
        /// If the unit is currently accessible, returns current position.
        /// Otherwise returns an empty point.
        /// </returns>
        Optional<Point> GetPosition() const;

        /// <summary>
        /// Gets player owning the unit.
        /// </summary>
        /// <returns>
        /// If the unit is currently accessible, returns owning player.
        /// Returns -1 otherwise.
        /// </returns>
        int GetOwner() const;

        /// <summary>
        /// Determines whether unit is owned by the local player.
        /// </summary>
        /// <returns>
        /// Returns true when the unit is accessible and belongs to local player.
        /// Returns false otherwise.
        /// </returns>
        bool IsOwnedByLocalPlayer() const;

        /// <summary>
        /// Determines whether unit is owned by an enemy of local player.
        /// Note it would return true for an enemy unit in fog of war, despite not knowing its existence.
        /// </summary>
        /// <returns>
        /// True when the unit is known to not owned by any friendly or neutral player.
        /// False when the unit is owned by friendly or neutral player, or was confirmed dead.
        /// </returns>
        bool IsOwnedByEnemyPlayer() const;
        #pragma endregion

        #pragma region Properties and States
        /// <summary>
        /// Gets current life of unit.
        /// </summary>
        /// <returns>
        /// The life point if the unit is accessible.
        /// Empty value otherwise.
        /// </returns>
        Optional<double> GetLife() const;

        /// <summary>
        /// Gets current shield of unit.
        /// </summary>
        /// <returns>
        /// The shield point if the unit has shield and is accessible.
        /// 0 if the unit can not have shield.
        /// Empty value if the unit is inaccessible.
        /// </returns>
        Optional<double> GetShield() const;

        /// <summary>
        /// Gets current energy of unit.
        /// </summary>
        /// <returns>
        /// The energy point if the unit has energy and is accessible.
        /// 0 if the unit can not have energy.
        /// Empty value if the unit is inaccessible.
        /// </returns>
        Optional<double> GetEnergy() const;
        #pragma endregion

        #pragma region Orders

        /// <summary>
        /// Sends an order to a unit.
        /// </summary>
        /// <param name="order">Order to send</param>
        void SendOrder(class Order& order);

        /// <summary>
        /// Adds an order to end of order queue for a unit.
        /// </summary>
        /// <param name="order">Order to send</param>
        void QueueOrder(class Order& order);

        /// <summary>
        /// Gets current order for a unit.
        /// </summary>
        /// <returns>Returns current order if exists, returns empty value otherwise.</returns>
        Optional<Order> GetCurrentOrder() const;

        #pragma endregion

        #pragma region Miscellaneous
        /// <summary>
        /// Gets string representation of unit for debugging purpose.
        /// </summary>
        /// <returns>String representation of unit</returns>
        std::string ToString() const;
        #pragma endregion

        #pragma region Events
        /// <summary>
        /// Fires when seeing any unit being created. Does not fire if the unit is inaccessible.
        /// Connect: void OnUnitCreated(Unit eventUnit, int eventPlayerId);
        /// </summary>
        /// <returns>Signal for binding</returns>
        static Signal<Unit, int>& SignalUnitCreated();

        /// <summary>
        /// Fires when seeing any unit being killed / destroyed. Does not fire if the unit is inaccessible.
        /// killerUnit is the unit who killed this unit, or empty value if doesn't exist or when it is inaccessible.
        /// Connect: void OnUnitDestroyed(Unit eventUnit, Optional<Unit> killerUnit);
        /// </summary>
        /// <returns>Signal for binding</returns>
        static Signal<Unit, Optional<Unit>>& SignalUnitDestroyed();

        /// <summary>
        /// Fires when any unit becomes visible to local player. Also fires when a visible unit is created.
        /// Connect: void OnUnitEnterVision(Unit eventUnit);
        /// </summary>
        /// <returns>Signal for binding</returns>
        static Signal<Unit>& SignalUnitEnterVision();

        /// <summary>
        /// Fires when any unit becomes not visible to local player.
        /// Connect: void OnUnitEnterVision(Unit eventUnit);
        /// </summary>
        /// <returns>Signal for binding</returns>
        static Signal<Unit>& SignalUnitLeaveVision();

        /// <summary>
        /// Fires when local player's unit started training a unit.
        /// Connect: void OnUnitTrainingStarted(Unit trainerUnit, std::string trainedUnitType);
        /// </summary>
        /// <returns>Signal for binding</returns>
        static Signal<Unit, std::string>& SignalUnitTrainingStarted();

        /// <summary>
        /// Fires when local player's unit training is paused.
        /// Connect: void OnUnitTrainingPaused(Unit trainerUnit, std::string trainedUnitType);
        /// </summary>
        /// <returns>Signal for binding</returns>
        static Signal<Unit, std::string>& SignalUnitTrainingPaused();

        /// <summary>
        /// Fires when local player's unit training is resumed.
        /// Connect: void OnUnitTrainingResumed(Unit trainerUnit, std::string trainedUnitType);
        /// </summary>
        /// <returns>Signal for binding</returns>
        static Signal<Unit, std::string>& SignalUnitTrainingResumed();

        /// <summary>
        /// Fires when local player's unit training is canceled.
        /// Connect: void OnUnitTrainingCanceled(Unit trainerUnit, std::string trainedUnitType);
        /// </summary>
        /// <returns>Signal for binding</returns>
        static Signal<Unit, std::string>& SignalUnitTrainingCanceled();

        /// <summary>
        /// Fires when local player's unit training is complete.
        /// Connect: void OnUnitTrainingComplete(Unit trainerUnit, Unit completedUnit);
        /// </summary>
        /// <returns>Signal for binding</returns>
        static Signal<Unit, Unit>& SignalUnitTrainingCompleted();

        #pragma endregion 

        #pragma region Implementations
        /// <summary>
        /// Unique integer associated with this unit.
        /// </summary>
        HandleId id = 0;
        #pragma endregion
    };

	#pragma region Comparison and hashing
	SC2API_API bool operator < (const Unit& lhs, const Unit& rhs);
	SC2API_API bool operator > (const Unit& lhs, const Unit& rhs);
	SC2API_API bool operator == (const Unit& lhs, const Unit& rhs);
	SC2API_API bool operator != (const Unit& lhs, const Unit& rhs);
	#pragma endregion
}