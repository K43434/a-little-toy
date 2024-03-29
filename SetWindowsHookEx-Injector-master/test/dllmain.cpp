#include "pch.h"
#include "Windows.h"
#include <stdlib.h>
#include <cstdio>
#include <iostream>
#include "Memory.h"
#include "GameData.h"
#include <conio.h> 
#include "xorstr.hpp"
#include <string>
#include <locale>
#include <codecvt>
#include <strsafe.h>
#include <sstream>

static HMODULE dllModule = NULL; // DLL模块句柄
static bool hookSet = false;

// 检查当前区域设置是否为简体中文或繁体中文
bool IsChineseLocale() {
    WCHAR localeName[LOCALE_NAME_MAX_LENGTH] = { 0 };

    // 获取系统默认的区域设置名称
    if (GetSystemDefaultLocaleName(localeName, LOCALE_NAME_MAX_LENGTH) == 0)
        return false; // 如果获取失败，则返回 false

    std::wstring locale(localeName);

    // 检查区域设置是否为简体中文（"zh-CN"）或繁体中文（"zh-TW"）
    return (locale == L"zh-CN" || locale == L"zh-TW" || locale == L"zh-HK" || locale == L"zh-SG");
}

// 用于转换 std::string 到 std::wstring
std::wstring stringToWString(const std::string& str) {
    std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
    return converter.from_bytes(str);
}
//Console Menu using checkbox
struct Checkbox {
    std::wstring title;
    bool checked;
};
void GetIniFilePath(TCHAR* iniPath, DWORD sizeOfIniPath) {
    // 获取DLL模块的完整路径
    GetModuleFileName(dllModule, iniPath, sizeOfIniPath);

    // 查找最后一个反斜杠的位置，这代表了目录路径的结束
    TCHAR* lastSlash = wcsrchr(iniPath, '\\');
    if (lastSlash != nullptr) {
        // 在最后一个反斜杠之后写入setting.ini的文件名
        // 确保有足够的空间来添加文件名和终结字符
        size_t remainingSize = sizeOfIniPath - (lastSlash - iniPath + 1);
        wcscpy_s(lastSlash + 1, remainingSize, TEXT("setting.ini"));
        OutputDebugString(iniPath);
    }
}
void InitializeFeaturesFromIni(std::vector<Checkbox>& checkboxes) {
    TCHAR iniPath[MAX_PATH];
    GetIniFilePath(iniPath, MAX_PATH);

    std::wstringstream enabledFeaturesStream;  // 使用宽字符版本的stringstream
    bool anyFeatureEnabled = false;  // 跟踪是否有任何功能被启用

    for (auto& checkbox : checkboxes) {
        checkbox.checked = GetPrivateProfileIntW(TEXT("Features"), checkbox.title.c_str(), 0, iniPath) == 1;
        if (checkbox.checked) {
            if (anyFeatureEnabled) {
                enabledFeaturesStream << L"\n";  // 在条目之间添加换行符
            }
            enabledFeaturesStream << checkbox.title;  // 将启用的功能添加到流中
            anyFeatureEnabled = true;
        }
    }

    if (anyFeatureEnabled) {
        // 有功能被启用，显示它们
        std::wstring enabledFeaturesMessage = L"以下功能已启用:\n" + enabledFeaturesStream.str();
        enabledFeaturesMessage += L"\n\n修改器由KAIN43和全知全能的CHATGPT4.0制作";
        MessageBoxW(NULL, enabledFeaturesMessage.c_str(), L"已启用的功能", MB_ICONINFORMATION | MB_OK);
    }
    else {
        // 没有功能被启用
        MessageBoxW(NULL, L"没有功能被启用。请检查setting.ini文件。", L"警告", MB_ICONWARNING | MB_OK);
    }
}


extern "C" __declspec(dllexport) LRESULT CALLBACK NextHook(int code, WPARAM wParam, LPARAM lParam) {
    return CallNextHookEx(NULL, code, wParam, lParam);
}

void setupHook() {
    if (!hookSet) {
        HHOOK hookHandle = SetWindowsHookEx(WH_GETMESSAGE, NextHook, dllModule, 0);
        if (hookHandle == NULL) {
            std::cout << "启动失败" << std::endl;
        }
        else {
            std::cout << "原神启动" << std::endl;
            hookSet = true; // 标记钩子已设置
        }
    }
    else {
        std::cout << "[ INFO ] Hook already set." << std::endl;
    }
}


/*void InitializeConsole() {
    FILE* pFile = nullptr;
    AllocConsole();
    SetConsoleTitleW(L"绝地潜兵2高端修改器");
    freopen_s(&pFile, _XOR_("CONOUT$"), _XOR_("w"), stdout);
}


void DestroyConsole()
{
    DestroyWindow(GetConsoleWindow());
    FreeConsole();
}

//Check Key Pressed
bool keyPressed(int vKey)
{
    return (GetAsyncKeyState(vKey) & 1);
}

// Function to display checkboxes
void displayCheckboxes(const std::vector<Checkbox>& checkboxes, size_t selectedCheckbox) {
    system("cls"); // Clear the console (Windows specific)

    printf(_XOR_("方向键上下移动，空格确定，最后用回车启用\n"));
    printf(_XOR_("遇到问题请联系你的咸鱼客服，问他内部挂怎么还被检测，然后来3DM联系我安慰一下你\n"));
    printf(_XOR_("本修改器改编由海外修改器emoisback，注入和bypaa由KAIN43和全知全能的chatgpt4.0制作。倒狗死妈\n"));

    std::cout << _XOR_("选单\n");

}*/


DWORD WINAPI Payload(LPVOID lpParam)
{


    //Console Menu
    std::vector<Checkbox> checkboxes = {
          {L"无限血", false}
        , {L"无限雷", false}
        , {L"演员无限雷", false}
        , {L"无限子弹", false}
        , {L"演员无限子弹", false}
        , {L"无限针", false}
        , {L"演员无限针", false}
        , {L"无限体力", false}
        , {L"无限支援", false}
        , {L"两倍移速", false}
        , {L"六倍移速", false}
        , {L"无限任务时间", false}
        , {L"无需换弹", false}
        , {L"满样本", false}
        , {L"捡样本X5", false}
        , {L"无后座", false}
       // , {L"增伤", false}
        //, {L"无限背包弹药", false}
        , {L"无限特殊武器弹药", false}
        , {L"激光不过热", false}
        , {L"轨道枪无蓄力", false}
        , {L"去除地图迷雾", false}
        , {L"支援无CD", false}
        , {L"背包护盾无CD", false}
        , {L"无限喷气背包", false}
        , {L"左按钮开金库门", false}
        //, {L"秒撤离", false}
        , {L"全变傻子", false}
        , {L"全部解锁", false}

    }; // Initialize all checkboxes to unchecked
    const int numCheckboxes = checkboxes.size();
    size_t selectedCheckbox = 0;
    char userInput;

    struct Checkbox {
        std::wstring title; // 使用宽字符串以支持Unicode
        bool checked;
    };
    
    InitializeFeaturesFromIni(checkboxes);

    HMODULE moduleHandle = nullptr;
    GameData gData;

    do
    {
        moduleHandle = GetModuleHandleW(_XOR_(L"game.dll")); // Use GetModuleHandleW for wide strings
        Sleep(1000);
    } while (!moduleHandle);
    Sleep(100);




    for (size_t i = 0; i < checkboxes.size(); ++i) 
    {
        if (checkboxes[i].checked)
        {

            if (checkboxes[i].title == L"无限血")
            {
                if (!gData.InfHealth) // no need but its old code when activate using hotkey, but need to much hotkey for all feature
                {
                    BYTE WriteHealthBytes[] =
                    {
                        0x48, 0x85, 0xDB,                               // test rbx,rbx
                        0x74, 0x03,                                     // jz short @f
                        0x45, 0x89, 0x38,                               //  mov [r8],r15d
                        // @@:
0x49, 0x8B, 0x84, 0xDE, 0x28, 0x04, 0x00, 0x00, // mov rax,[r14+rbx*8+00000428]
0x8B, 0x48, 0x10,                               // mov ecx,[rax+10]
0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 //JMP return_WriteHealth
                    };

                    BYTE SetHealthBytes[] =
                    {
                        0x48, 0x85, 0xD2,                                                       // test rdx,rdx
                        0x75, 0x0C,                                                             // jnz short @f
                        0x41, 0xC7, 0x84, 0x8B, 0x28, 0x4C, 0x00, 0x00, 0x0F, 0x27, 0x00, 0x00, //   mov [r11+rcx*4+00004C28],#9999
                        // @@:
0x41, 0x8B, 0x84, 0x8B, 0x28, 0x4C, 0x00, 0x00,                         // mov eax,[r11+rcx*4+00004C28]
0x48, 0x8B, 0x5C, 0x24, 0x20,                                           // mov rbx,[rsp+20]
0x48, 0x8B, 0x74, 0x24, 0x28,                                           // mov rsi,[rsp+28]
0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 //JMP return_SetHealth
                    };

                    uintptr_t InfHealth = Memory::FindPattern(_XOR_("game.dll"), _XOR_("45 89 38 49 8B 84 DE 28 04 00 00"));
                    LPVOID memory = Memory::AllocateMemory(InfHealth, sizeof(WriteHealthBytes));
                    Memory::CreateTrampoline(InfHealth, memory);
                    Memory::WriteAssemblyInstructions((uintptr_t)memory, InfHealth + 14, WriteHealthBytes, Memory::ArrayLength(WriteHealthBytes));

                    uintptr_t InfHealth1 = Memory::FindPattern(_XOR_("game.dll"), _XOR_("41 8B 84 8B 28 4C 00 00"));
                    memory = Memory::AllocateMemory(InfHealth1, sizeof(SetHealthBytes));
                    Memory::CreateTrampoline(InfHealth1, memory);
                    Memory::WriteAssemblyInstructions((uintptr_t)memory, InfHealth1 + 18, SetHealthBytes, Memory::ArrayLength(SetHealthBytes));

                    gData.InfHealth = !gData.InfHealth;
                    //create trampolin
                    //printf(_XOR_("1\n"));
                }
            }

            if (checkboxes[i].title == L"无限雷")
            {
                if (!gData.InfGrenades)
                {
                    uintptr_t GrenadesAddress = Memory::FindPattern(_XOR_("game.dll"), _XOR_("41 FF 08 4A 8B 84 ED"));
                    Memory::Nop((LPVOID)(GrenadesAddress), 3);
                    gData.InfGrenades = !gData.InfGrenades;
                    printf(_XOR_("2\n"));
                }
            }

            if (checkboxes[i].title == L"演员无限雷")
            {
                if (!gData.InfGrenadesLegit && !gData.InfGrenades)
                {
                    uintptr_t GrenadesAddress = Memory::FindPattern(_XOR_("game.dll"), _XOR_("4D 03 C6 41 FF 08"));
                    BYTE GrenadeBytes[] =
                    {
                        0x4D, 0x01, 0xF0,                               //add r8,r14
                        0x41, 0x83, 0x38, 0x01,                         //cmp dword ptr [r8],01
                        0x74, 0x03,                                     //je short @f
                        0x41, 0xFF, 0x08,                               // dec [r8]
                        0x4A, 0x8B, 0x84, 0xED, 0x30, 0x20, 0x00, 0x00, // mov rax,[rbp+r13*8+00002030]
                        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 //JMP return_Grenades
                    };
                    LPVOID memory = Memory::AllocateMemory(GrenadesAddress, sizeof(GrenadeBytes));
                    Memory::CreateTrampoline(GrenadesAddress, memory);
                    Memory::WriteAssemblyInstructions((uintptr_t)memory, GrenadesAddress + 14, GrenadeBytes, Memory::ArrayLength(GrenadeBytes));
                    gData.InfGrenadesLegit = !gData.InfGrenadesLegit;
                    printf(_XOR_("3\n"));
                }
            }

            if (checkboxes[i].title == L"无限子弹")
            {
                if (!gData.InfAmmo)
                {
                    uintptr_t AmmoAddress = Memory::FindPattern(_XOR_("game.dll"), _XOR_("41 83 2C C2 01 4D 8D 04 C2 49 8B 84 CA"));
                    BYTE AmmoPatch[] = { 0x00 };
                    Memory::Patch((LPVOID)(AmmoAddress + 4), AmmoPatch, 1);
                    gData.InfAmmo = !gData.InfAmmo;
                    printf(_XOR_("4\n"));
                }
            }

            if (checkboxes[i].title == L"演员无限子弹")
            {
                if (!gData.InfAmmoLegit && !gData.InfAmmo)
                {
                    uintptr_t AmmoAddress = Memory::FindPattern(_XOR_("game.dll"), _XOR_("41 83 2C C2 01"));
                    BYTE AmmoBytes[] =
                    {
                        0x41, 0x83, 0x3C, 0xC2, 0x02,                    // cmp dword ptr [r10+rax*8],#2
                        0x7E, 0x05,                                      // jle short @f
                        0x41, 0x83, 0x2C, 0xC2, 0x01,                    // sub dword ptr [r10+rax*8],01
                        0x4D, 0x8D, 0x04, 0xC2,			                 // lea r8,[r10+rax*8]
                        0x49, 0x8B, 0x84, 0xCA, 0x28, 0x20, 0x00, 0x00,  // mov rax,[r10+rcx*8+00002028]
                        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 //JMP return_SubtractAmmo
                    };
                    LPVOID memory = Memory::AllocateMemory(AmmoAddress, sizeof(AmmoBytes));
                    Memory::CreateTrampoline(AmmoAddress, memory);
                    Memory::WriteAssemblyInstructions((uintptr_t)memory, AmmoAddress + 17, AmmoBytes, Memory::ArrayLength(AmmoBytes));
                    gData.InfAmmoLegit = !gData.InfAmmoLegit;
                    printf(_XOR_("5\n"));
                }
            }

            if (checkboxes[i].title == L"无限针")
            {
                if (!gData.InfSyringes)
                {
                    uintptr_t Syringes = Memory::FindPattern(_XOR_("game.dll"), _XOR_("41 FF CF 3B C2 74 61"));
                    Memory::Nop((LPVOID)(Syringes), 3);
                    gData.InfSyringes = !gData.InfSyringes;
                    printf(_XOR_("6\n"));
                }
            }

            if (checkboxes[i].title == L"演员无限针")
            {
                if (!gData.InfSyringesLegit && !gData.InfSyringes)
                {
                    uintptr_t SyringesAddress = Memory::FindPattern(_XOR_("game.dll"), _XOR_("48 03 C9 45 8B BC CA C8 75 00 00"));
                    BYTE SyringesBytes[] =
                    {
                        0x48, 0x01, 0xC9,                                     // add rcx,rcx
                        0x45, 0x8B, 0xBC, 0xCA, 0xC8, 0x75, 0x00, 0x00,       // mov r15d,[r10+rcx*8+000075C8]
                        0x41, 0x83, 0xBC, 0xCA, 0xC8, 0x75, 0x00, 0x00, 0x01, // cmp dword ptr [r10+rcx*8+000075C8],01 { 1 }
                        0x74, 0x03,                                           // je short @f
                        0x41, 0xFF, 0xCF,                                     // dec r15d
                        0x39, 0xD0,                                           // cmp eax,edx
                        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 //JMP return_Syringes
                    };
                    LPVOID memory = Memory::AllocateMemory(SyringesAddress, sizeof(SyringesBytes));
                    Memory::CreateTrampoline(SyringesAddress, memory);
                    Memory::WriteAssemblyInstructions((uintptr_t)memory, SyringesAddress + 14, SyringesBytes, Memory::ArrayLength(SyringesBytes));
                    gData.InfSyringesLegit = !gData.InfSyringesLegit;
                    printf(_XOR_("7\n"));
                }
            }

            if (checkboxes[i].title == L"无限体力")
            {
                if (!gData.InfStamina)
                {
                    uintptr_t Stamina = Memory::FindPattern(_XOR_("game.dll"), _XOR_("F3 41 0F 11 08 8B 48 10 E8 ?? ?? ?? ?? 41 8B 47 48"));
                    BYTE StaminaPatch[] = { 0xF3, 0x41, 0x0F, 0x11, 0x30 };
                    Memory::Patch((LPVOID)(Stamina), StaminaPatch, 5);
                    gData.InfStamina = !gData.InfStamina;
                    printf(_XOR_("8\n"));
                }
            }

            if (checkboxes[i].title == L"无限支援")
            {
                if (!gData.InfStratagems)
                {
                    uintptr_t Stratagems = Memory::FindPattern(_XOR_("game.dll"), _XOR_("0F 86 BF 01 00 00 0F"));
                    BYTE StratagemsPatch1[] = { 0x90, 0xE9 };
                    Memory::Patch((LPVOID)(Stratagems), StratagemsPatch1, 2);
                    gData.InfStratagems = !gData.InfStratagems;
                    printf(_XOR_("9\n"));
                }
            }

            if (checkboxes[i].title == L"无限任务时间")
            {
                if (!gData.InfMissionTime)
                {
                    uintptr_t MissionTime = Memory::FindPattern(_XOR_("game.dll"), _XOR_("F3 0F 5C C7 F3 41 0F 5F C5"));
                    Memory::Nop((LPVOID)(MissionTime), 4);
                    gData.InfMissionTime = !gData.InfMissionTime;
                    printf(_XOR_("10\n"));
                }
            }

            if (checkboxes[i].title == L"无需换弹")
            {
                if (!gData.NoReload)
                {
                    BYTE NoReloadByte[] =
                    {
                        0x90,
                        0x49, 0x8B, 0x84, 0xCA, 0x28, 0x20, 0x00, 0x00,
                        0x8B, 0x48, 0x10,
                        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,  // JMP [rip+6]
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Placeholder for the target address
                    };

                    uintptr_t NoReload = Memory::FindPattern(_XOR_("game.dll"), _XOR_("41 89 28 49 8B 84 CA 28 20 00 00 8B 48 10"));

                    LPVOID memory = Memory::AllocateMemory(NoReload, 0x100);
                    Memory::CreateTrampoline(NoReload, memory);
                    Memory::WriteAssemblyInstructions((uintptr_t)memory, NoReload + 14, NoReloadByte, Memory::ArrayLength(NoReloadByte));
                    gData.NoReload = !gData.NoReload;
                    printf(_XOR_("11\n"));
                }
            }
            
            if (checkboxes[i].title == L"无需换弹")
            {
                if (!gData.NoReload_v2)
                {
                    uintptr_t NoReload_v2 = Memory::FindPattern(_XOR_("game.dll"), _XOR_("FF 4C 87 04 83 FE FF 74 52 49 8B 00 F6 40 14 01"));
                    Memory::Nop((LPVOID)(NoReload_v2), 4);
                    gData.NoReload_v2 = !gData.NoReload_v2;
                    //printf(_XOR_("[Active] No Reload V2\n"));
                }
            }

            if (checkboxes[i].title == L"满样本")
            {
                if (!gData.MaxResources)
                {
                    BYTE MaxResourcesByte[] =
                    {
                        0x41, 0x81, 0x84, 0x8A, 0xEC, 0x17, 0x00, 0x00, 0xF4, 0x01, 0x00, 0x00,
                        0x41, 0x81, 0x84, 0x8A, 0xF0, 0x17, 0x00, 0x00, 0xF4, 0x01, 0x00, 0x00,
                        0x41, 0x81, 0x84, 0x8A, 0xF4, 0x17, 0x00, 0x00, 0xF4, 0x01, 0x00, 0x00,
                        0x48, 0x8D, 0x82, 0x99, 0x00, 0x00, 0x00,
                        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00,  // JMP [rip+6]
                        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, // Placeholder for the target address
                    };

                    uintptr_t MaxResources = Memory::FindPattern(_XOR_("game.dll"), _XOR_("45 01 B4 8A EC 17 00 00"));

                    LPVOID memory = Memory::AllocateMemory(MaxResources, 0x100);
                    Memory::CreateTrampoline(MaxResources, memory);
                    Memory::WriteAssemblyInstructions((uintptr_t)memory, MaxResources + 17, MaxResourcesByte, Memory::ArrayLength(MaxResourcesByte));
                    gData.MaxResources = !gData.MaxResources;
                    printf(_XOR_("12\n"));
                }
            }

            if (checkboxes[i].title == L"捡样本X5")
            {
                if (!gData.Add5Resources && !gData.MaxResources)
                {
                    uintptr_t ResourcesAddress = Memory::FindPattern(_XOR_("game.dll"), _XOR_("45 01 B4 8A EC 17 00 00"));
                    BYTE ResourcesBytes[] =
                    {
                        0x41, 0xBE, 0x05, 0x00, 0x00, 0x00,              // mov r14d, #5
                        0x45, 0x01, 0xB4, 0x8A, 0xEC, 0x17, 0x00, 0x00,  // add [r10+rcx*4+000017EC],r14d
                        0x48, 0x8D, 0x82, 0x99, 0x00, 0x00, 0x00,        // lea rax,[rdx+00000099]
                        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 //JMP return_Add5Resources
                    };
                    LPVOID memory = Memory::AllocateMemory(ResourcesAddress, sizeof(ResourcesBytes));
                    Memory::CreateTrampoline(ResourcesAddress, memory);
                    Memory::WriteAssemblyInstructions((uintptr_t)memory, ResourcesAddress + 15, ResourcesBytes, Memory::ArrayLength(ResourcesBytes));
                    gData.Add5Resources = !gData.Add5Resources;
                    printf(_XOR_("13\n"));
                }
            }
            /*if (checkboxes[i].title == L"增伤")
            {
                if (!gData.IncreasedDamage)
                {// IncreasedDamage 实现
                    // 查找需要修改的内存地址
                    uintptr_t ohkAddress = Memory::FindPattern("game.dll", "89 87 44 64 00 00");

                    // 分配新内存用于我们的代码
                    LPVOID newmem = Memory::AllocateMemory(ohkAddress, 0x100);

                    // JMP 到 newmem 的汇编指令
                    BYTE jmpInstruction[] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };
                    int offset = ((int)newmem - (int)ohkAddress - 5); // 计算偏移量
                    memcpy(&jmpInstruction[1], &offset, sizeof(offset));

                    // 在原地址写入 JMP 指令
                    Memory::Patch((LPVOID)ohkAddress, jmpInstruction, sizeof(jmpInstruction));

                    // 写入我们的代码到新内存
                    BYTE customCode[] = {
                        0x48, 0x83, 0xEC, 0x28,                   // sub rsp, 0x28
                        0x48, 0x8B, 0xC4,                         // mov rax, rsp
                        0x48, 0x89, 0x58, 0x10,                   // mov [rax+10h], rbx
                        0x48, 0x89, 0x70, 0x18,                   // mov [rax+18h], rsi
                        // 你的自定义代码来实现 IncreasedDamage
                        0xC3                                      // ret
                    };

                    // 使用 Memory::WriteAssemblyInstructions 写入自定义代码
                    Memory::WriteAssemblyInstructions((uintptr_t)newmem, ohkAddress + 5, customCode, sizeof(customCode));
                }


            }*/

            if (checkboxes[i].title == L"轨道枪无蓄力")
            {
                if (!gData.InstantRailGun)
                {
                    BYTE InstantRailGunByte[] =
                    {
                        0x43, 0xC7, 0x84, 0x3E, 0x2C, 0x2C, 0x00, 0x00, 0xFC, 0xFF, 0x3F, 0x40, // mov [r14+r15+00002C2C],403FFFFC { 3.00 }
                        0xF3, 0x43, 0x0F, 0x10, 0x84, 0x3E, 0x2C, 0x2C, 0x00, 0x00,             // movss xmm0,[r14+r15+00002C2C]
                        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 //JMP return_Railgun
                    };

                    uintptr_t InstantRailGun = Memory::FindPattern(_XOR_("game.dll"), _XOR_("F3 43 0F 11 84 37 2C 2C 00 00"));

                    LPVOID memory = Memory::AllocateMemory(InstantRailGun, sizeof(InstantRailGunByte));
                    Memory::CreateTrampoline(InstantRailGun, memory);
                    Memory::WriteAssemblyInstructions((uintptr_t)memory, InstantRailGun + 14, InstantRailGunByte, Memory::ArrayLength(InstantRailGunByte));
                    gData.InstantRailGun = !gData.InstantRailGun;
                    printf(_XOR_("14\n"));
                }
            }


            if (checkboxes[i].title == L"六倍移速")
            {
                if (!gData.Speedhack)
                {
                    BYTE SpeedhackByte[] =
                    {
                        0x41, 0xC7, 0x46, 0x0C, 0x00, 0x00, 0xC0, 0x40,
                        0xF3, 0x41, 0x0F, 0x59, 0x56, 0x0C,
                        0xF3, 0x41, 0x0F, 0x59, 0x56, 0x10,
                        0x0F, 0x28, 0xE2,
                        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 //JMP return_Railgun
                    };

                    uintptr_t Speedhack = Memory::FindPattern(_XOR_("game.dll"), _XOR_("F3 41 ?? ?? ?? ?? F3 41 ?? ?? ?? ?? 0F 28 ?? F3 0F ?? ?? 0F 5A"));

                    LPVOID memory = Memory::AllocateMemory(Speedhack, sizeof(SpeedhackByte));
                    Memory::CreateTrampoline(Speedhack, memory);
                    Memory::WriteAssemblyInstructions((uintptr_t)memory, Speedhack + 15, SpeedhackByte, Memory::ArrayLength(SpeedhackByte));
                    gData.Speedhack = !gData.Speedhack;
                    printf(_XOR_("15\n"));
                }
            }

            if (checkboxes[i].title == L"两倍移速")
            {
                if (!gData.Speedhack)
                {
                    BYTE SpeedhackByte[] =
                    {
                        0x41, 0xC7, 0x46, 0x0C, 0x00, 0x00, 0x00, 0x40,
                        0xF3, 0x41, 0x0F, 0x59, 0x56, 0x0C,
                        0xF3, 0x41, 0x0F, 0x59, 0x56, 0x10,
                        0x0F, 0x28, 0xE2,
                        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 //JMP return_Railgun
                    };

                    uintptr_t Speedhack = Memory::FindPattern(_XOR_("game.dll"), _XOR_("F3 41 ?? ?? ?? ?? F3 41 ?? ?? ?? ?? 0F 28 ?? F3 0F ?? ?? 0F 5A"));

                    LPVOID memory = Memory::AllocateMemory(Speedhack, sizeof(SpeedhackByte));
                    Memory::CreateTrampoline(Speedhack, memory);
                    Memory::WriteAssemblyInstructions((uintptr_t)memory, Speedhack + 15, SpeedhackByte, Memory::ArrayLength(SpeedhackByte));
                    gData.Speedhack = !gData.Speedhack;
                    printf(_XOR_("15\n"));
                }
            }
            if (checkboxes[i].title == L"无后座")
            {
                if (!gData.Recoil) // 假设gData中有一个InfNoRecoil的标志位
                {
                    uintptr_t RecoilAddress = Memory::FindPattern(_XOR_("game.dll"), _XOR_("48 83 EC 18 48 8B 01"));
                    if (RecoilAddress != 0)
                    {
                        BYTE NoRecoilPatch[] = { 0xC3 }; // 用于替换的字节，C3是ret指令
                        Memory::Patch((LPVOID)RecoilAddress, NoRecoilPatch, sizeof(NoRecoilPatch));
                        gData.Recoil = true; // 标记无后座已激活
                        printf(_XOR_("16\n"));
                    }
                }
            }

            if (checkboxes[i].title == L"激光不过热")
            {
                if (!gData.NoCannonOverheat)
                {
                    uintptr_t CannonOverheat = Memory::FindPattern(_XOR_("game.dll"), _XOR_("F3 0F 11 09 4C 8B C1 49"));
                    Memory::Nop((LPVOID)(CannonOverheat), 4);
                    gData.NoCannonOverheat = !gData.NoCannonOverheat;
                    printf(_XOR_("17\n"));
                }
            }

            if (checkboxes[i].title == L"无限特殊武器弹药")
            {
                if (!gData.InfSpecWeapon)
                {
                    uintptr_t SpecWeapon = Memory::FindPattern(_XOR_("game.dll"), _XOR_("44 89 7F 08 41 80 BC 24"));
                    Memory::Nop((LPVOID)(SpecWeapon), 4);
                    gData.InfSpecWeapon = !gData.InfSpecWeapon;
                    printf(_XOR_("18\n"));
                }
            }

            if (checkboxes[i].title == L"支援无CD")
            {
                if (!gData.NoStasTurretOverHeat)
                {
                    uintptr_t NoStasTurretOverHeat = Memory::FindPattern(_XOR_("game.dll"), _XOR_("F3 42 0F 11 84 32 ?? ?? ?? ?? 8B 55 38 43 89 94 35"));
                    Memory::Nop((LPVOID)(NoStasTurretOverHeat), 10);
                    gData.NoStasTurretOverHeat = !gData.NoStasTurretOverHeat;
                    printf(_XOR_("19\n"));
                }
            }

            if (checkboxes[i].title == L"无限喷气背包")
            {
                if (!gData.JetpackNoCD)
                {
                    uintptr_t JetpackNoCD = Memory::FindPattern(_XOR_("game.dll"), _XOR_("8B 08 89 8C BE ?? ?? 00 00"));
                    Memory::Nop((LPVOID)(JetpackNoCD + 2), 7);
                    gData.JetpackNoCD = !gData.JetpackNoCD;
                    printf(_XOR_("20\n"));
                }
            }

            if (checkboxes[i].title == L"背包护盾无CD")
            {
                if (!gData.ShieldNoCD)
                {
                    BYTE ShieldNoCDByte[] =
                    {
                        0xF3, 0x0F, 0x5C, 0xC9, 0x90
                    };

                    uintptr_t ShieldNoCD = Memory::FindPattern(_XOR_("game.dll"), _XOR_("F3 41 0F 5C CA F3 0F 11 8C EE"));
                    Memory::Patch((LPVOID)(ShieldNoCD), ShieldNoCDByte, 5);
                    gData.ShieldNoCD = !gData.ShieldNoCD;
                    printf(_XOR_("21\n"));
                }
            }


            /*if (checkboxes[i].title == L"无限背包弹药")
            {
                if (!gData.InfBackpack)
                {
                    uintptr_t Backpack = Memory::FindPattern(_XOR_("game.dll"), _XOR_("49 ? ? 45 ? ? ? e8 * * * * e9 ? ? ? ?"));
                    Memory::Nop((LPVOID)(Backpack), 0);
                    gData.InfBackpack = !gData.InfBackpack;
                    printf(_XOR_("22\n"));
                }
            }*/

            if (checkboxes[i].title == L"去除地图迷雾")
            {
                if (!gData.ShowAllMapIcons)
                {
                    BYTE ShowAllMapIconsByte[] =
                    {
                        0xB8, 0x01, 0x00, 0x00, 0x00, 0x90
                    };

                    BYTE ShowAllMapIconsByte1[] =
                    {
                        0xF8
                    };

                    BYTE ShowAllMapIconsByte2n4[] =
                    {
                        0x90, 0xE9
                    };

                    BYTE ShowAllMapIconsByte3[] =
                    {
                        0xEB, 0x04
                    };

                    uintptr_t ShowAllMapIconsAddr = Memory::FindPattern(_XOR_("game.dll"), _XOR_("41 0F B6 44 97 23"));
                    uintptr_t aob_CheckIfAlienHivesAreObstructed = Memory::FindPattern(_XOR_("game.dll"), _XOR_("41 80 BE 3C BA 07 00 00"));
                    uintptr_t aob_CheckIfMinorInterestBlipIsDiscovered = Memory::FindPattern(_XOR_("game.dll"), _XOR_("0F 85 ?? ?? ?? ?? 48 8B 44 24 ?? 80 78 29 00"));
                    uintptr_t aob_GetMinorInterestBlipIcon = Memory::FindPattern(_XOR_("game.dll"), _XOR_("0F 84 ?? ?? ?? ?? 48 8B 4C 24 ?? F3 41 0F 10 4F"));
                    uintptr_t aob_CheckMissionBlip = Memory::FindPattern(_XOR_("game.dll"), _XOR_("0F 85 59 02 00 00 49 8D"));

                    Memory::Patch((LPVOID)(ShowAllMapIconsAddr), ShowAllMapIconsByte, 6);
                    Memory::Patch((LPVOID)(aob_CheckIfAlienHivesAreObstructed), ShowAllMapIconsByte1, 1);
                    Memory::Nop((LPVOID)(aob_CheckIfAlienHivesAreObstructed + 1), 7);
                    Memory::Patch((LPVOID)(aob_CheckIfMinorInterestBlipIsDiscovered), ShowAllMapIconsByte2n4, 2);
                    Memory::Patch((LPVOID)(aob_GetMinorInterestBlipIcon), ShowAllMapIconsByte3, 2);
                    Memory::Patch((LPVOID)(aob_CheckMissionBlip), ShowAllMapIconsByte2n4, 2);
                    gData.ShowAllMapIcons = !gData.ShowAllMapIcons;
                    printf(_XOR_("23\n"));
                }
            }
            
            if (checkboxes[i].title == L"左按钮开金库门") // Only use the left button to open vault
            {
                if (!gData.FuckVaults)
                {
                    BYTE FuckVaultByte[] =
                    {
                    0x90, 0xE9
                    };

                    uintptr_t aob_VaultCheck = Memory::FindPattern(_XOR_("game.dll"), _XOR_("0F 84 BD 00 00 00 81 FA E0"));
                    Memory::Patch((LPVOID)(aob_VaultCheck), FuckVaultByte, sizeof(FuckVaultByte));
                    gData.FuckVaults = !gData.FuckVaults;
                    printf(_XOR_("[Active] Fuck Vaults\n"));
                }
            }

            /*if (checkboxes[i].title == L"秒撤离")
            {
                if (!gData.instantleave)
                {
                    uintptr_t patternAddress = Memory::FindPattern("game.dll", "61 F3 41 0F 11 00");

                    LPVOID newMem = Memory::AllocateMemory(patternAddress, 1024);

                    // Assume compare_float_check is placed right after the custom code.
                    float compareValue = 1.0f; // The value to compare with xmm0
                    uintptr_t compareAddress = (uintptr_t)newMem + 100;

                    // 自定义汇编代码
                    BYTE customCode[] = 
                    {
    0x0F, 0x2E, 0x05, 0xF9, 0x03, 0x00, 0x00, // ucomiss xmm0, [compare_float_check]
    0x76, 0x08, // jbe short @f
    0xF3, 0x0F, 0x10, 0x05, 0xEB, 0x03, 0x00, 0x00, // movss xmm0, [compare_float_check]
    0xF3, 0x41, 0x0F, 0x11, 0x00, // movss [r8], xmm0
    0x4A, 0x8B, 0x84, 0xF6, 0x50, 0x04, 0x00, 0x00, // mov rax, [rsi+r14*8+00000450]
    0x8B, 0x48, 0x10, // mov ecx, [rax+10]
    0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, // jmp [address] （假定地址将被后续代码正确计算和填充）
    0x00, 0x00, 0x00, 0x00 // 为 jmp 指令的目标地址预留空间
                    };

                    // Patch the actual compareAddress into the ucomiss instruction.
                        // Calculate the relative address and patch it similarly for movss.

                    uintptr_t jmpBackOffset = patternAddress + 5 - ((uintptr_t)newMem + sizeof(customCode)); // +5 for the size of replaced instructions
                    memcpy(&customCode[sizeof(customCode) - 4], &jmpBackOffset, 4); // Patch the jmp offset directly into the code

                    void WriteAssemblyInstructions(uintptr_t allocatedMemory, BYTE * assemblyBytes, size_t assemblyBytesSize);

                    // Patch the original code to jump to your custom code.
                    BYTE jmpToNewMem[5] = { 0xE9, 0x00, 0x00, 0x00, 0x00 };
                    DWORD jmpOffset = (DWORD)((uintptr_t)newMem - patternAddress - 5);
                    memcpy(&jmpToNewMem[1], &jmpOffset, sizeof(jmpOffset));
                    Memory::Patch((LPVOID)patternAddress, jmpToNewMem, sizeof(jmpToNewMem));

                }
            }*/

            if (checkboxes[i].title == L"全变傻子")
                if (!gData.alldumb)
                {
                  
                        // 查找游戏内特定的代码模式
                        uintptr_t aggroPatternAddr = Memory::FindPattern("game.dll", "74 ?? 45 8b 81 ?? ?? ?? ?? 45 85 c0");
                        // 将目标地址的指令替换为 NOP（No Operation），以禁用原有逻辑
                        // 根据提供的 AOBScan 结果，我们需要在地址处注入两个 NOP 指令来覆盖原有的跳转逻辑
                        BYTE AttackPatch[] = { 0xeb };
                        Memory::Patch((LPVOID)aggroPatternAddr, AttackPatch, 1);

                        std::cout << "NO AGGRO 功能已启用" << std::endl;
                    
                }

            if (checkboxes[i].title == L"全部解锁")
            {
                if (!gData.AllStratagems)
                {
                    BYTE AllStratagemsByte[] =
                    {
                        0xB0, 0x01, 0xC3
                    };

                    uintptr_t AllStratagems = Memory::FindPattern(_XOR_("game.dll"), _XOR_("48 89 5C 24 ?? 48 8B D9 85 D2 75 09"));
                    Memory::Patch((LPVOID)(AllStratagems), AllStratagemsByte, 3);
                    gData.AllStratagems = !gData.AllStratagems;
                    printf(_XOR_("24\n"));
                }
            }

            if (checkboxes[i].title == L"全部解锁")
            {
                if (!gData.AllEquipment)
                {
                    BYTE AllEquipmentByte[] =
                    {
                        0xB0, 0x01, 0xC3
                    };

                    uintptr_t AllEquipment = Memory::FindPattern(_XOR_("game.dll"), _XOR_("83 B9 ?? ?? ?? ?? ?? 75 ?? 85 D2 74 ?? 44 8B 89 ?? ?? ?? ?? 45 33 C0 45 85 C9 74 ?? 48 8D 81 ?? ?? ?? ?? 39 50 ?? 74 ?? 41 FF C0 48 83 C0 ?? 45 3B C1 72 ?? 32 C0 C3 8B 00 48 69 C8"));
                    Memory::Patch((LPVOID)(AllEquipment + 11), AllEquipmentByte, 3);
                    gData.AllEquipment = !gData.AllEquipment;
                    printf(_XOR_("25\n"));
                }
            }

            if (checkboxes[i].title == L"全部解锁")
            {
                if (!gData.AllArmor)
                {
                    BYTE AllArmorByte[] =
                    {
                        0xB0, 0x01, 0xC3
                    };

                    uintptr_t AllArmor = Memory::FindPattern(_XOR_("game.dll"), _XOR_("48 83 EC ?? 44 8B 49 ?? 45 33 C0"));
                    Memory::Patch((LPVOID)(AllArmor), AllArmorByte, 3);
                    gData.AllArmor = !gData.AllArmor;
                    printf(_XOR_("26\n"));
                }
            }

            /*if (checkboxes[i].title == "One / Two Hit Kill ( Bile Titan Bug, Aim Only Head )")
            {
                if (!gData.OHK)
                {
                    BYTE OHKByte[] =
                    {
                        0x83, 0xBF, 0x38, 0x0B, 0x00, 0x00, 0x0A,
                        0x0F, 0x85, 0x05, 0x00, 0x00, 0x00,
                        0xE9, 0x18, 0x00, 0x00, 0x00,
                        0xC7, 0x87, 0x44, 0x64, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x66, 0x4A, 0x6B, 0x80, 0x01, 0x00, 0x00, 0x00,
                        0x89, 0x87, 0x44, 0x64, 0x00, 0x00,
                        0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
                    };

                    uintptr_t OHK = Memory::FindPattern("game.dll", "89 87 44 64 00 00");
                    LPVOID memory = Memory::AllocateMemory(OHK, 0x100);
                    Memory::CreateTrampoline(OHK, memory);
                    Memory::WriteAssemblyInstructions((uintptr_t)memory, OHK + 15, OHKByte, Memory::ArrayLength(OHKByte));
                    gData.OHK = !gData.OHK;
                    printf("[Active] 轨道枪无蓄力\n");
                }
            }*/





        }
    }
    //printf(_XOR_("原神关闭\n"));
    //FreeConsole();
    FreeLibraryAndExitThread(dllModule, 0);
    return 0;
}


BOOL APIENTRY DllMain(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) 
{
    switch (ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        if (!IsChineseLocale()) {
            MessageBox(NULL, L"海外用户不得参加淫趴", L"原神不欢迎洋鬼子", MB_ICONERROR);
            return FALSE; // 阻止 DLL 加载
        }
        DisableThreadLibraryCalls(hModule); // 推荐做法，提高性能
        CreateThread(NULL, 0, Payload, NULL, 0, NULL);
        break;
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}