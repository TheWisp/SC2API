#pragma once
#include <type_traits>
#include "SC2APIPoint.h"
#include "Creator.h"

namespace SC2API
{
	class SC2API_API UnitTestBase
	{
	public:

		virtual ~UnitTestBase()
		{
		}
		UnitTestBase() = default;

		//Connect: void OnFinished(bool success)
		Signal<bool /*success*/> Finished;

		virtual const char* GetName() const = 0;
		virtual float       GetTimeOutDuration() const = 0;
		virtual void		SetupTest() = 0;
		virtual void        RunTest() = 0;
		virtual void		TeardownTest() = 0;

		void                ReportError(std::string const& errorMessage);

		template<typename A, typename B>
		void TestEqual(const A& valueA, const B& valueB)
		{
			if (!(valueA == valueB))
			{
				std::string thisName{ GetName() };
				ReportError("TestEqual failed in " + thisName + ": " + stringify(valueA) + " is not equal to " + stringify(valueB));
				Finished(false);
			}
		}

		template<typename A, typename B>
		void TestGreater(const A& valueA, const B& valueB)
		{
			if (!(valueA > valueB))
			{
				std::string thisName{ GetName() };
				ReportError("TestEqual failed in " + thisName + ": " + stringify(valueA) + " is not greater than " + stringify(valueB));
				Finished(false);
			}
		}

	protected:

		template<typename T>
		static std::enable_if_t<std::is_arithmetic_v<T>, std::string> stringify(T t)
		{
			return std::to_string(t);
		}

		template<typename T>
		static std::enable_if_t<!std::is_arithmetic_v<T>, std::string> stringify(T t)
		{
			return static_cast<std::ostringstream&>(std::ostringstream() << t).str();
		}

		template<>
		static std::string stringify<char>(char t)
		{
			return std::string(1, t);
		}
	};

	//Helper functions to interact with setups in the test map

	//Gets a normal candidate point for a town with resources
	SC2API_API Point GetTownCandidatePositionStandard();

	//TODO doc
	SC2API_API Signal<>& SignalRegisterUnitTests();

	//TODO doc
	SC2API_API void RegisterUnitTest(const char* name, Creator<UnitTestBase> unitTestCreator);
}
