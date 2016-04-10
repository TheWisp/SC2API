#include <Windows.h>
#include <SC2API/include/SC2APIGame.h>
#include "ExampleAI.h"

using namespace SC2API;

static void Init()
{
    Game::SignalMatchStarted().connect(ExampleAI::GameInstance::instance);
    Game::SignalMatchEnded().connect(ExampleAI::GameInstance::freeInstance);
}

static void Cleanup()
{
    ExampleAI::GameInstance::freeInstance();
}

BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
    )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        Init();
        break;
    case DLL_THREAD_ATTACH:
        break;
    case DLL_THREAD_DETACH:
        break;
    case DLL_PROCESS_DETACH:
        Cleanup();
        break;
    }
    return TRUE;
}

