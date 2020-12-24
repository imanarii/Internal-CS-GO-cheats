#include <iostream>
#include <cmath>
#include <Windows.h>
#include <TlHelp32.h>
#include <vector>
#include <algorithm>
#include "csgo.hpp"


struct vector
{
    float x, y, z;
};


int sniper[4] = {9,40,38,11};
int team;
uintptr_t localEnt, moduleBase;


void trigg()
{
    int crosshairId = *(int*)(localEnt + hazedumper::netvars::m_iCrosshairId);
    uintptr_t crossHairEnt = *(uintptr_t*)(moduleBase + hazedumper::signatures::dwEntityList + (crosshairId - 1) * 0x10);
    int ads = *(int*)(localEnt + hazedumper::netvars::m_bIsScoped);
    int weapon = *(int*)(localEnt + hazedumper::netvars::m_hActiveWeapon);
    int weaponEnt = *(int*)(moduleBase + hazedumper::signatures::dwEntityList + ((weapon & 0xFFF) - 1) * 0x10);

   

    if (weaponEnt)
    {
        int myWeapon = *(int*)(weaponEnt + hazedumper::netvars::m_iItemDefinitionIndex);
        //std::cout << myWeapon << std::endl;
        bool isSniper;
        int* found = std::find(sniper, std::end(sniper), myWeapon);
        if (found != std::end(sniper))
        {
            isSniper = true;
        }
        else
            isSniper = false;

        if (crosshairId <= 64 && crosshairId != 0)
        {

            if (crossHairEnt)
            {
                int crosshairTeam = *(int*)(crossHairEnt + hazedumper::netvars::m_iTeamNum);
                // int crosshairLifeState = *(int*)(crossHairEnt + hazedumper::netvars::m_lifeState);

                if (team != crosshairTeam && (ads || !isSniper))
                {
                    vector myLocation = *(vector*)(localEnt + hazedumper::netvars::m_vecOrigin);
                    vector enemyLocation = *(vector*)(crossHairEnt + hazedumper::netvars::m_vecOrigin);

                    int dist = sqrt(pow(myLocation.x - enemyLocation.x, 2) + pow(myLocation.y - enemyLocation.y, 2) + pow(myLocation.z - enemyLocation.z, 2)) * 0.0254;
                    //std::cout << dist << std::endl;
                    float Delay = dist * 3.9;

                    // std::cout << "Crosshair Id: " << crosshairId << std::endl;


                    *(int*)(moduleBase + hazedumper::signatures::dwForceAttack) = 5;
                    Sleep(30);
                    *(int*)(moduleBase + hazedumper::signatures::dwForceAttack) = 4;
                    Sleep(Delay);
                }
            }
        }
    }
}

void glowFunc()
{
    uintptr_t glowObject = *(uintptr_t*)(moduleBase + hazedumper::signatures::dwGlowObjectManager);
    int EntityList = *(int*)(moduleBase + hazedumper::signatures::dwEntityList);
    //std::cout << EntityList << std::endl;
    //std::cout << "My Team: " << team << std::endl;
    for (short int i = 0; i < 30; i++)
    {

        uintptr_t* entityPtr = (uintptr_t*)(moduleBase + hazedumper::signatures::dwEntityList + i * 0x10);
        uintptr_t entity = *entityPtr;

        if (entity)
        {
            //uintptr_t entity = *entityPtr;

            int glowindx = *(int*)(entity + hazedumper::netvars::m_iGlowIndex);
            int entityTeam = *(int*)(entity + hazedumper::netvars::m_iTeamNum);


            if (entityTeam == team)
            {
                *(float*)(glowObject + (glowindx * 0x38) + 0x4) = 255;
                *(float*)(glowObject + (glowindx * 0x38) + 0x8) = 255;
                *(float*)(glowObject + (glowindx * 0x38) + 0xC) = 255;
                *(float*)(glowObject + (glowindx * 0x38) + 0x10) = 1;

            }
            else
            {
                *(float*)(glowObject + (glowindx * 0x38) + 0x4) = 255;
                *(float*)(glowObject + (glowindx * 0x38) + 0x8) = 0;
                *(float*)(glowObject + (glowindx * 0x38) + 0xC) = 0;
                *(float*)(glowObject + (glowindx * 0x38) + 0x10) = 1;
            }
            *(bool*)(glowObject + (glowindx * 0x38) + 0x24) = true;
            *(bool*)(glowObject + (glowindx * 0x38) + 0x25) = false;
            //std::cout << "Entity Team: " << entityTeam << std::endl;

            
        }

    }
}

void radarFunc()
{
    for (int i = 0; i < 30; i++)
    {
        uintptr_t ent = *(uintptr_t*)(moduleBase + hazedumper::signatures::dwEntityList + i * 0x10);
        if (ent)
        {
            *(uintptr_t*)(ent + hazedumper::netvars::m_bSpotted) = true;
        }
    }
}

void noFlash()
{
    if (localEnt)
    {
        *(int*)(localEnt + hazedumper::netvars::m_flFlashMaxAlpha) = 0;
    }
}

DWORD WINAPI HackThread(HMODULE hModule)
{
    /*
    AllocConsole();
    FILE* f;
    freopen_s(&f, "CONOUT$", "w", stdout);
    std::cout << "Greetings Nerds and virgins" << std::endl;
    */

    moduleBase = (uintptr_t)GetModuleHandle(L"client.dll");


    bool triggerBot = false, glow = false, radar = false;


    while (true)
    {
        if (GetAsyncKeyState(VK_END) & 1)
        {
            break;
        }
        if (GetAsyncKeyState(VK_F1) & 1)
        {
            triggerBot = !triggerBot;
        }
        if (GetAsyncKeyState(VK_F2) & 1)
        {
            glow = !glow;
        }
        if (GetAsyncKeyState(VK_INSERT) & 1)
        {
            radar = !radar;
        }

        
        if (moduleBase)
        {
            uintptr_t* localEntPtr = (uintptr_t*)(moduleBase + hazedumper::signatures::dwLocalPlayer);

            if (localEntPtr)
            {
                localEnt = *localEntPtr;

                team = *(int*)(localEnt + hazedumper::netvars::m_iTeamNum);
            

                if (triggerBot)
                {
                    trigg();
                }

                if (glow)
                {
                    glowFunc();
                }

                if (radar)
                {
                    radarFunc();  
                }

                noFlash();
            }
        }

        Sleep(10);

    }

    //fclose(f);
    //FreeConsole();
    FreeLibraryAndExitThread(hModule, 0);
    return 0;

}

    


BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)HackThread, hModule, 0, nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}