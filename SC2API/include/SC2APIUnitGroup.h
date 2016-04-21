#pragma once
#include "SC2API.h"
#include "SC2APIUnit.h"
#include "SC2APIUnitFilterFlag.h"
#include <set>

namespace SC2API
{
	/// <summary>
	/// SC2API UnitGroup is a collection of units. Note the content is copied when assigned to another variable.
	/// </summary>
	class SC2API_API UnitGroup final
	{
	public:

        /// <summary>
        /// Gets string representation of unit group for debugging purpose.
        /// </summary>
        /// <returns>String representation of units in the group</returns>
        std::string ToString() const;

		/// <summary>
		/// Adds a unit to the group
		/// </summary>
		/// <param name="inUnit">The unit to add</param>
		void Add(Unit inUnit);

		/// <summary>
		/// Adds all the units in the source group to the group
		/// </summary>
		/// <param name="inUnitGroup">The source group of units</param>
		void Add(const UnitGroup& inUnitGroup);

		/// <summary>
		/// Removes a unit from the group
		/// </summary>
		/// <param name="inUnit">The unit to remove</param>
		void Remove(Unit inUnit);

		/// <summary>
		/// Removes all the units in the source group from the group
		/// </summary>
		/// <param name="inUnitGroup">The source group of units</param>
		void Remove(const UnitGroup& inUnitGroup);

		/// <summary>
		/// Returns true if the unit is contained by the group
		/// </summary>
		/// <param name="inUnit">The unit to test</param>
		/// <returns>Whether the unit is contained by the group</returns>
		bool Has(Unit inUnit) const;

		/// <summary>
		/// Returns count of units in the group
		/// </summary>
		/// <returns>Count of units</returns>
		int Count() const;

        /// <summary>
        /// Gets first unit in the group, if any.
        /// </summary>
        /// <returns>Unit or empty value.</returns>
        Optional<Unit> First() const;

		/// <summary>
		/// Sends an order to all units in the group.
		/// </summary>
		/// <param name="order">Order to send</param>
		void SendOrder(class Order& order) const;

		/// <summary>
		/// Adds an order to end of order queue for all units in the group.
		/// </summary>
		/// <param name="order">Order to send</param>
		void QueueOrder(class Order& order) const;

		/// <summary>
		/// Filters the units in the group according to the required and excluded flags.
		/// </summary>
		/// <param name="requiredFlags">The required flags</param>
		/// <param name="excludedFlags">The excluded flags, empty by default</param>
		void Filter(UnitFilterFlag requiredFlags, UnitFilterFlag excludedFlags = UnitFilterFlag::Null);

		/// <summary>
		/// Filters the units in the group according to the allowed unit types
		/// </summary>
		/// <param name="possibleUnitTypes">The set of unit types, see SC2API::Units</param>
		void Filter(const std::set<std::string>& possibleUnitTypes);

		/// <summary>
		/// Gets all accessible units in the game.
		/// </summary>
		/// <returns>Group of all accessible units</returns>
		static UnitGroup GetAccessibleUnits();

		/// <summary>
		/// Gets all accessible units in the game with filter conditions.
		/// </summary>
		/// <param name="requiredFlags">The required flags</param>
		/// <param name="excludedFlags">The excluded flags, empty by default</param>
		static UnitGroup GetAccessibleUnits(UnitFilterFlag requiredFlags, UnitFilterFlag excludedFlags = UnitFilterFlag::Null);

		/// <summary>
		/// Gets all accessible units of specified unit type in the game.
		/// </summary>
		/// <param name="unitType">The required unit type, see SC2API::Units</param>
		/// <returns>Group of units</returns>
		static UnitGroup GetUnitsOfType(std::string unitType);

		/// <summary>
		/// Gets all accessible units of specified unit type in the game with filter conditions.
		/// </summary>
		/// <param name="unitType">The required unit type, see SC2API::Units</param>
		/// <param name="requiredFlags">The required flags</param>
		/// <param name="excludedFlags">The excluded flags, empty by default</param>
		/// <returns>Group of units</returns>
		static UnitGroup GetUnitsOfType(
			std::string unitType, 
			UnitFilterFlag requiredFlags, 
			UnitFilterFlag excludedFlags = UnitFilterFlag::Null);

		/// <summary>
		/// Gets all accessible units of unit types in possibleUnitTypes.
		/// </summary>
		/// <param name="possibleUnitTypes">The set of unit types, see SC2API::Units</param>
		/// <returns>Group of units</returns>
		static UnitGroup GetUnitsOfType(std::set<std::string> possibleUnitTypes);

		/// <summary>
		/// Gets all accessible units of unit types in possibleUnitTypes with filter conditions.
		/// </summary>
		/// <param name="possibleUnitTypes">The set of unit types, see SC2API::Units</param>
		/// <param name="requiredFlags">The required flags</param>
		/// <param name="excludedFlags">The excluded flags, empty by default</param>
		/// <returns>Group of units</returns>
		static UnitGroup GetUnitsOfType(
			const std::set<std::string>& possibleUnitTypes, 
			UnitFilterFlag requiredFlags, 
			UnitFilterFlag excludedFlags = UnitFilterFlag::Null);

		#pragma region Implementations
	private:
		std::set<Unit> Container;
	public:
		auto begin() const
		{
			return Container.begin();
		}
		auto end() const
		{
			return Container.end();
		}
		#pragma endregion
	};
}
