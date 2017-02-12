#pragma once
#include "SC2API.h"
#include <string>

namespace SC2API
{
	/// <summary>
	/// Outputs a simple message line to loader window
	/// </summary>
	SC2API_API void LogLoader(std::string message);
}

//Outputs a warning message to loader window if expression evaluates to false
#define SC2API_ASSERT(expr)												\
do 																		\
{																		\
	if ((expr) != true)													\
	{																	\
		SC2API::LogLoader("ASSERT failed: " #expr " at " __FUNCTION__);	\
	}																	\
} while (0)
