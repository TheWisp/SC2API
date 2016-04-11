#pragma once
#include "SC2API.h"

namespace SC2API
{
    class SC2API_API Order final
    {
        /// <summary>
        /// Returns an order with no target.
        /// </summary>
        /// <param name="command">Command of the order</param>
        /// <returns>order</returns>
        static Order OrderWithNoTarget(struct Command command);

        /// <summary>
        /// Returns an order targeting a point.
        /// </summary>
        /// <param name="command">Command of the order</param>
        /// <param name="targetPoint">Order target point</param>
        /// <returns>order</returns>
        static Order OrderTargetingPoint(struct Command command, struct Point targetPoint);

        /// <summary>
        /// Returns an order targeting a unit.
        /// </summary>
        /// <param name="command">Command of the order</param>
        /// <param name="targetUnit">Order target unit</param>
        /// <returns>order</returns>
        static Order OrderTargetingUnit(struct Command command, class Unit targetUnit);

        #pragma region Implementation
    private:
        friend class Order_Impl;
        class Order_Impl* _impl = nullptr;
    public:
        Order();
        ~Order();

        //copy constructor
        Order(const Order& other);

        //move constructor
        Order(Order&& src);

        //copy assign operator
        Order& operator= (const Order& other);

        //move assign operator
        Order& operator= (Order&& src);

        //compare operator
        friend bool operator== (const Order& lhs, const Order& rhs);

        #pragma endregion
    };
}