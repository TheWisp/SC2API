#pragma once
#include "SC2API.h"
#include <string>

namespace SC2API
{
	struct SC2API_API Point
	{
		double X;
		double Y;

		/// <summary>
		/// Gets string representation of point for debugging purpose.
		/// </summary>
		/// <returns>String representation of point</returns>
		std::string ToString() const;

		/// <summary>
		/// Returns distance between two points.
		/// </summary>
		static double Dist(const Point& pointA, const Point& pointB);
	};
}
