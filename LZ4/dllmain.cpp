// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "framework.h"
#include "lz4.h"
#include "lz4frame.h"
#include "lz4hc.h"
#include "xxhash.h"
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

