#pragma once
#include "includes.h"
#include "RetSpoof.h"

namespace Hooking
{
	uintptr_t GetDiscordModuleBase();
	uintptr_t GetSceneAddress();

	short GetAsyncKeyState(const int vKey);

	bool InsertHook(uintptr_t pOriginal, uintptr_t pHookedFunction, uintptr_t pOriginalCall);
	bool RemoveHook(uintptr_t pOriginal);

	bool CreateHook(uintptr_t pOriginal, uintptr_t pHookedFunction, uintptr_t pOriginalCall);
	bool EnableHook(uintptr_t pTarget, bool bIsEnabled);
	bool EnableHookQue();

	std::vector<uintptr_t> pCreatedHooksArray;
}

uintptr_t Hooking::GetDiscordModuleBase()
{
	static uintptr_t discordModuleBase = 0;

	if (!discordModuleBase)
		discordModuleBase = (uintptr_t)GetModuleHandleA("DiscordHook64.dll");

	return discordModuleBase;
}

uintptr_t Hooking::GetSceneAddress()
{
	static uintptr_t presentSceneAdress = 0;

	if (!presentSceneAdress)
		presentSceneAdress = Helper::PatternScan(GetDiscordModuleBase(), ("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B D9 41 8B F8"));

	return presentSceneAdress;
}

short Hooking::GetAsyncKeyState(const int vKey)
{
	static uintptr_t addrGetAsyncKeyState = NULL;

	if (!addrGetAsyncKeyState)
		addrGetAsyncKeyState = Helper::PatternScan(GetDiscordModuleBase(), ("40 53 48 83 EC 20 8B D9 FF 15 ? ? ? ?"));

	if (!addrGetAsyncKeyState)
		return false;

	using GetAsyncKeyState_t = short(__fastcall*)(int);
	auto fnGetAyncKeyState = (GetAsyncKeyState_t)addrGetAsyncKeyState;

	return Spoofer::SpoofCall(fnGetAyncKeyState, vKey);
}

// Wrapper
bool Hooking::InsertHook(uintptr_t pOriginal, uintptr_t pHookedFunction, uintptr_t pOriginalCall)
{
	bool bAlreadyCreated = false;
	for (auto _Hook : pCreatedHooksArray)
	{
		if (_Hook == pOriginal)
		{
			bAlreadyCreated = true;
			break;
		}
	}

	if (!bAlreadyCreated)
		bAlreadyCreated = CreateHook(pOriginal, pHookedFunction, pOriginalCall);

	if (bAlreadyCreated)
		if (EnableHook(pOriginal, true))
			if (EnableHookQue())
				return true;

	return false;
}

bool Hooking::RemoveHook(uintptr_t pOriginal)
{
	bool bAlreadyCreated = false;
	for (auto _Hook : pCreatedHooksArray)
	{
		if (_Hook == pOriginal)
		{
			bAlreadyCreated = true;
			break;
		}
	}

	if (bAlreadyCreated)
		if (EnableHook(pOriginal, false))
			if (EnableHookQue())
				return true;

	return false;
}

// Internal Calls
bool Hooking::CreateHook(uintptr_t pOriginal, uintptr_t pHookedFunction, uintptr_t pOriginalCall)
{
	static uintptr_t addrCreateHook = NULL;

	if (!addrCreateHook)
		addrCreateHook = Helper::PatternScan(GetDiscordModuleBase(), ("40 53 55 56 57 41 54 41 56 41 57 48 83 EC 60"));

	if (!addrCreateHook)
		return false;

	using CreateHook_t = uint64_t(__fastcall*)(LPVOID, LPVOID, LPVOID*);
	auto fnCreateHook = (CreateHook_t)addrCreateHook;

	return Spoofer::SpoofCall(fnCreateHook, (void*)pOriginal, (void*)pHookedFunction, (void**)pOriginalCall) == 0 ? true : false;
}

bool Hooking::EnableHook(uintptr_t pTarget, bool bIsEnabled)
{
	static uintptr_t addrEnableHook = NULL;

	if (!addrEnableHook)
		addrEnableHook = Helper::PatternScan(GetDiscordModuleBase(), ("48 89 5C 24 ? 48 89 6C 24 ? 48 89 74 24 ? 57 41 56 41 57 48 83 EC 20 33 F6 8B FA"));

	if (!addrEnableHook)
		return false;

	using EnableHook_t = uint64_t(__fastcall*)(LPVOID, bool);
	auto fnEnableHook = (EnableHook_t)addrEnableHook;

	return Spoofer::SpoofCall(fnEnableHook, (void*)pTarget, bIsEnabled) == 0 ? true : false;
}

bool Hooking::EnableHookQue()
{
	static uintptr_t addrEnableHookQueu = NULL;

	if (!addrEnableHookQueu)
		addrEnableHookQueu = Helper::PatternScan(GetDiscordModuleBase(), ("48 89 5C 24 ? 48 89 6C 24 ? 48 89 7C 24 ? 41 57"));

	if (!addrEnableHookQueu)
		return false;

	using EnableHookQueu_t = uint64_t(__stdcall*)(VOID);
	auto fnEnableHookQueu = (EnableHookQueu_t)addrEnableHookQueu;

	return Spoofer::SpoofCall(fnEnableHookQueu) == 0 ? true : false;
}