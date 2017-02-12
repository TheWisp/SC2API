#include <Windows.h>
#include "SC2API/include/SC2APIGame.h"
#include "ExampleAI.h"
using namespace SC2API;

extern "C"
{
	__declspec(dllexport) void __cdecl InitAI()
	{
		SignalMatchStarted().connect(ExampleAI::GameInstance::instance);
		SignalMatchEnded().connect(ExampleAI::GameInstance::freeInstance);
	}

	__declspec(dllexport) void __cdecl CleanupAI()
	{
		ExampleAI::GameInstance::freeInstance();
	}
}