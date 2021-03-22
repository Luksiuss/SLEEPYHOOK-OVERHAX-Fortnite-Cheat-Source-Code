#pragma once
#include "includes.h"

bool InitD3DHook()
{
	HWND GameWindowFortnite = FindWindowA(skCrypt_("UnrealWindow"), skCrypt_("Fortnite  "));
	if (!GameWindowFortnite) GameWindowFortnite = GetForegroundWindow();
	if (!GameWindowFortnite) return false;

	GameWindow = GameWindowFortnite;

	uint64_t uworld_address = (memory::occurence(skCrypt_("FortniteClient-Win64-Shipping.exe"), skCrypt_("48 8B 1D ? ? ? ? 48 85 DB 74 3B")));
	//if (!IsValid(uworld_address)) { Warn("uworld failed!"); return false; }

	uworld_address = reinterpret_cast<uintptr_t>(RELATIVE_ADDR(uworld_address, 7));
	//48 8B 05 00 00 00 00 4C 8D 3C CD
	uint64_t uobject_address = (memory::occurence(skCrypt_("FortniteClient-Win64-Shipping.exe"), skCrypt_("E8 ? ? ? ? 44 8B 4E 04 4C 8B C0")));
	//if (!IsValid(uobject_address)) { Warn("uobject failed!"); return false; }

	uobject_address = reinterpret_cast<uintptr_t>(RELATIVE_ADDR(uobject_address, 7));

	OFFSET_UWORLD = uworld_address;
	OFFSET_UOBJECT = uobject_address;

	OFFSET_GETOBJECTNAMES = (memory::occurence(skCrypt_("FortniteClient-Win64-Shipping.exe"), skCrypt_("40 53 48 83 EC 20 48 8B D9 48 85 D2 75 45 33 C0 48 89 01 48 89 41 08 8D 50 05 E8 ? ? ? ? 8B 53 08 8D 42 05 89 43 08 3B 43 0C 7E 08 48 8B CB E8 ? ? ? ? 48 8B 0B 48 8D 15 ? ? ? ? 41 B8 ? ? ? ? E8 ? ? ? ? 48 8B C3 48 83 C4 20 5B C3 48 8B 42 18")));
	OFFSET_GETNAMEBYINDEX = (memory::occurence(skCrypt_("FortniteClient-Win64-Shipping.exe"), skCrypt_("48 89 5C 24 ? 55 56 57 48 8B EC 48 83 EC 30 8B")));
	OFFSET_FNFREE = (memory::occurence(skCrypt_("FortniteClient-Win64-Shipping.exe"), skCrypt_("48 85 C9 74 2E 53 48 83 EC 20 48 8B D9 48 8B 0D ? ? ? ? 48 85 C9 75 0C")));

	// GetWeaponStats
	auto addr = Utilities::FindPattern(skCrypt_("\x48\x83\xEC\x58\x48\x8B\x91\x00\x00\x00\x00\x48\x85\xD2\x0F\x84\x00\x00\x00\x00\xF6\x81\x00\x00\x00\x00\x00\x74\x10\x48\x8B\x81\x00\x00\x00\x00\x48\x85\xC0\x0F\x85\x00\x00\x00\x00\x48\x8B\x8A\x00\x00\x00\x00\x48\x89\x5C\x24\x00\x48\x8D\x9A\x00\x00\x00\x00\x48\x85\xC9"), skCrypt_("xxxxxxx????xxxxx????xx?????xxxxx????xxxxx????xxx????xxxx?xxx????xxx"));
	if (!addr) {
		//Warn("GetWeaponStats failed!");
		return FALSE;
	}
	GetWeaponStats = reinterpret_cast<decltype(GetWeaponStats)>(addr);

	// Free
	addr = Utilities::FindPattern(skCrypt("\x48\x85\xC9\x74\x2E\x53\x48\x83\xEC\x20\x48\x8B\xD9\x48\x8B\x0D\x00\x00\x00\x00\x48\x85\xC9\x75\x0C"), skCrypt("xxxxxxxxxxxxxxxx????xxxxx"));
	if (!addr) {
		//Warn("Free failed!");
		return FALSE;
	}
	FreeInternal = reinterpret_cast<decltype(FreeInternal)>(addr);


	GetObjectNameInternal = reinterpret_cast<decltype(GetObjectNameInternal)>(OFFSET_GETOBJECTNAMES);
	objects = reinterpret_cast<decltype(objects)>(OFFSET_UOBJECT);

	//Offsets::Engine::Controller::SetActorHiddenInGame = util::FindObject(L"/Script/Engine.Actor.SetActorHiddenInGame");;
  //  Offsets::LaunchCharacter = util::FindObject(L"/Script/Engine.Character.LaunchCharacter");

	uint64_t ValidateFire_address = (memory::occurence(skCrypt_("FortniteClient-Win64-Shipping.exe"),
		skCrypt_("E8 ? ? ? ? 0F B6 D8 EB 9E")));
	//if (!IsValid(ValidateFire_address)) { Warn("ValidateFire_address failed!"); return false; }
	uintptr_t ValidateFire_add = (ValidateFire_address + *(DWORD*)(ValidateFire_address + 0x1) + 0x5 - 0x100000000);

	auto GetBounds_Addr = (memory::occurence(skCrypt_("FortniteClient-Win64-Shipping.exe"),
		skCrypt_("E8 ? ? ? ? 48 8B 4D 77 B3 01")));
	//if (!IsValid(GetBounds_Addr)) { Warn("GetBounds_Addr failed!"); return false; }
	Offsets::fnGetBounds = (GetBounds_Addr + *(DWORD*)(GetBounds_Addr + 0x1) + 0x5);

	auto GetBoneMatrix_add = (memory::occurence(skCrypt_("FortniteClient-Win64-Shipping.exe"),
		skCrypt_("E8 ? ? ? ? 48 8B 47 30 F3 0F 10 45 ?")));
	//if (!IsValid(GetBoneMatrix_add)) { Warn("GetBoneMatrix_add failed!"); return false; }
	Offsets::fnGetBoneMatrix = (GetBoneMatrix_add + *(DWORD*)(GetBoneMatrix_add + 0x1) + 0x5 - 0x100000000);

	Offsets::fnLineOfSightTo = (memory::occurence(skCrypt_("FortniteClient-Win64-Shipping.exe"),
		skCrypt_("40 55 53 56 57 48 8D 6C 24 ? 48 81 EC ? ? ? ? 48 8B 05 ? ? ? ? 48 33 C4 48 89 45 E0 49")));


	Offsets::fnGetWeaponStats = (memory::occurence(skCrypt_("FortniteClient-Win64-Shipping.exe"),
		skCrypt_("48 83 EC 58 48 8B 91 ? ? ? ? 48 85 D2 0F 84 ? ? ? ? F6 81 ? ? ? ? ? 74 10 48 8B 81 ? ? ? ? 48 85 C0 0F 85 ? ? ? ? 48 8B 8A ? ? ? ? 48 89 5C 24 ? 48 8D 9A ? ? ? ? 48 85 C9")));
	if (!ValidateFire_add || !Offsets::fnGetBounds || !IsValid(Offsets::fnGetBoneMatrix) || !IsValid(Offsets::fnLineOfSightTo) || !IsValid(Offsets::fnGetWeaponStats)) {
		//Warn("ValidateFire_add failed!");
	}
	oWndProc = (WNDPROC)SetWindowLongPtrW(GameWindowFortnite, GWLP_WNDPROC, (LONG_PTR)hkWndProc);
	oValidateLastFire = (LFAT)SetHook_1((void*)ValidateFire_add, (void*)hkValidateLastFire, 16);
	const auto pcall_present_discord = Helper::PatternScan(Hooking::GetDiscordModuleBase(),
		skCrypt_("FF 15 ? ? ? ? 8B D8 E8 ? ? ? ? E8 ? ? ? ? EB 10"));
	if (!pcall_present_discord)
		return FALSE;
	const auto poriginal_present = reinterpret_cast<f_present*>(pcall_present_discord +
		*reinterpret_cast<int32_t*>(pcall_present_discord + 0x2) + 0x6);
	if (!*poriginal_present)
		return FALSE;
	SwapChain = *poriginal_present;
	*poriginal_present = hk_present;
	auto presentSceneAdress = Helper::PatternScan(Hooking::GetDiscordModuleBase(),
		skCrypt_("48 89 5C 24 ? 48 89 74 24 ? 57 48 83 EC 20 48 8B D9 41 8B F8"));
	Hooking::InsertHook(presentSceneAdress, (uintptr_t)hk_present, (uintptr_t)&PresentOriginal);
	return true;
}

bool InitGetKeys()
{
	HMODULE API = GetModuleHandleW(skCrypt_(TEXT("win32u.dll")));
	if (API != NULL)
	{
		o_getasynckeystate = (LPFN_MBA)GetProcAddress(API, skCrypt_("NtUserGetAsyncKeyState"));
		if (o_getasynckeystate != NULL)
			return true;
		else
			return false;
	}
}

#define updatershow FALSE
VOID Main() {
	// Utilities::CreateConsole();
	extern uint64_t base_address;
	uintptr_t UObjectArray = (uintptr_t)Utilities::FindPattern(skCrypt("\x48\x8B\x05\x00\x00\x00\x00\x4C\x8D\x34\xCD"), skCrypt("xxx????xxxx"));
	UObjectArray = reinterpret_cast<uintptr_t>(RELATIVE_ADDR(UObjectArray, 7));
	uintptr_t GetNameByIndex = (uintptr_t)Utilities::FindPattern(skCrypt("\x48\x89\x5C\x24\x00\x55\x56\x57\x48\x8B\xEC\x48\x83\xEC\x30\x8B"), skCrypt("xxxx?xxxxxxxxxxx"));
	uintptr_t GetObjectName = (uintptr_t)Utilities::FindPattern(skCrypt("\x40\x53\x48\x83\xEC\x20\x48\x8B\xD9\x48\x85\xD2\x75\x45\x33\xC0\x48\x89\x01\x48\x89\x41\x08\x8D\x50\x05\xE8\x00\x00\x00\x00\x8B\x53\x08\x8D\x42\x05\x89\x43\x08\x3B\x43\x0C\x7E\x08\x48\x8B\xCB\xE8\x00\x00\x00\x00\x48\x8B\x0B\x48\x8D\x15\x00\x00\x00\x00\x41\xB8\x00\x00\x00\x00\xE8\x00\x00\x00\x00\x48\x8B\xC3\x48\x83\xC4\x20\x5B\xC3\x48\x8B\x42\x18"), skCrypt("xxxxxxxxxxxxxxxxxxxxxxxxxxx????xxxxxxxxxxxxxxxxxx????xxxxxx????xx????x????xxxxxxxxxxxxx"));
	uintptr_t FnFree = (uintptr_t)Utilities::FindPattern(skCrypt("\x48\x85\xC9\x74\x2E\x53\x48\x83\xEC\x20\x48\x8B\xD9\x48\x8B\x0D\x00\x00\x00\x00\x48\x85\xC9\x75\x0C"), skCrypt("xxxxxxxxxxxxxxxx????xxxxx"));
	FortUpdater* Updater = new FortUpdater();
	//48 8B 05 ? ? ? ? 4C 8D 34 CD
	if (Updater->Init(UObjectArray, GetObjectName, GetNameByIndex, FnFree))
	{
		Offsets::Engine::World::Levels = Updater->FindOffset("World", "Levels");
		Offsets::Engine::World::PersistentLevel = Updater->FindOffset(("World"), ("PersistentLevel"));
		Offsets::Engine::GameInstance::LocalPlayers = Updater->FindOffset("GameInstance", "LocalPlayers");
		Offsets::Engine::World::OwningGameInstance = Updater->FindOffset("World", "OwningGameInstance");
		Offsets::Engine::Controller::ControlRotation = Updater->FindOffset("Controller", "ControlRotation");
		Offsets::Engine::PlayerController::PlayerCameraManager = Updater->FindOffset("PlayerController", "PlayerCameraManager");
		Offsets::Engine::PlayerController::AcknowledgedPawn = Updater->FindOffset("PlayerController", "AcknowledgedPawn");
		Offsets::Engine::Pawn::PlayerState = Updater->FindOffset("Pawn", "PlayerState");
		Offsets::Engine::Actor::RootComponent = Updater->FindOffset("Actor", "RootComponent");
		Offsets::Engine::Character::Mesh = Updater->FindOffset("Character", "Mesh");
		Offsets::Engine::SceneComponent::RelativeLocation = Updater->FindOffset("SceneComponent", "RelativeLocation");
		Offsets::Engine::SceneComponent::ComponentVelocity = Updater->FindOffset("SceneComponent", "ComponentVelocity");
		Offsets::Engine::StaticMeshComponent::StaticMesh = Updater->FindOffset("StaticMeshComponent", "StaticMesh");
		Offsets::Engine::StaticMeshComponent::ComponentToWorld = Updater->FindOffset("StaticMeshComponent", "ComponentToWorld");
		Offsets::Engine::SkinnedMeshComponent::CachedWorldSpaceBounds = Updater->FindOffset("SkinnedMeshComponent", "CachedWorldSpaceBounds");
		Offsets::Engine::Actor::CustomTimeDilation = Updater->FindOffset("Actor", "CustomTimeDilation");
		Offsets::FortniteGame::FortWeapon::LastFireTimeVerified = Updater->FindOffset("FortWeapon", "LastFireTimeVerified");
		Offsets::FortniteGame::FortWeapon::LastFireTime = Updater->FindOffset("FortWeapon", "LastFireTime");
		Offsets::FortniteGame::FortPawn::bIsDBNO = Updater->FindOffset("FortPawn", "bIsDBNO");
		Offsets::FortniteGame::FortPawn::bIsDying = Updater->FindOffset("FortPawn", "bIsDying");
		Offsets::FortniteGame::FortPlayerStateAthena::TeamIndex = Updater->FindOffset("FortPlayerStateAthena", "TeamIndex");
		Offsets::FortniteGame::FortPickup::PrimaryPickupItemEntry = Updater->FindOffset("FortPickup", "PrimaryPickupItemEntry");
		Offsets::FortniteGame::FortItemDefinition::DisplayName = Updater->FindOffset("FortItemDefinition", "DisplayName");
		Offsets::FortniteGame::FortItemDefinition::Tier = Updater->FindOffset("FortItemDefinition", "Tier");
		Offsets::FortniteGame::FortItemEntry::ItemDefinition = Updater->FindOffset("FortItemEntry", "ItemDefinition");
		Offsets::FortniteGame::FortPawn::CurrentWeapon = Updater->FindOffset("FortPawn", "CurrentWeapon");
		Offsets::FortniteGame::FortWeapon::WeaponData = Updater->FindOffset("FortWeapon", "WeaponData");
		Offsets::Engine::Controller::RemoteViewPitch = Updater->FindOffset(("Pawn"), ("RemoteViewPitch"));
		Offsets::FortniteGame::FortWeapon::LastFireAbilityTime = Updater->FindOffset("FortWeapon", "LastFireAbilityTime");
		Offsets::FortniteGame::FortWeaponItemDefinition::WeaponStatHandle = Updater->FindOffset("FortWeaponItemDefinition", "WeaponStatHandle");
		Offsets::FortniteGame::FortProjectileAthena::FireStartLoc = Updater->FindOffset("FortProjectileAthena", "FireStartLoc");
		Offsets::FortniteGame::FortBaseWeaponStats::ReloadTime = Updater->FindOffset("FortBaseWeaponStats", "ReloadTime");
		Offsets::FortniteGame::FortBaseWeaponStats::ReloadScale = Updater->FindOffset("FortBaseWeaponStats", "ReloadScale");
		Offsets::FortniteGame::FortBaseWeaponStats::ChargeDownTime = Updater->FindOffset("FortBaseWeaponStats", "ChargeDownTime");
		Offsets::FortniteGame::FortRangedWeaponStats::RecoilHoriz = Updater->FindOffset("FortRangedWeaponStats", "RecoilHoriz");
		Offsets::FortniteGame::FortRangedWeaponStats::RecoilVert = Updater->FindOffset("FortRangedWeaponStats", "RecoilVert");
		Offsets::FortniteGame::FortRangedWeaponStats::RecoilDownsightsMultiplier = Updater->FindOffset("FortRangedWeaponStats", "RecoilDownsightsMultiplier");

		Offsets::FortniteGame::FortRangedWeaponStats::Spread = Updater->FindOffset("FortRangedWeaponStats", "Spread");
		Offsets::FortniteGame::FortRangedWeaponStats::SpreadDownsights = Updater->FindOffset("FortRangedWeaponStats", "SpreadDownsights");
		Offsets::FortniteGame::FortRangedWeaponStats::StandingStillSpreadMultiplier = Updater->FindOffset("FortRangedWeaponStats", "StandingStillSpreadMultiplier");
		Offsets::FortniteGame::FortRangedWeaponStats::AthenaJumpingFallingSpreadMultiplier = Updater->FindOffset("FortRangedWeaponStats", "AthenaJumpingFallingSpreadMultiplier");
		Offsets::FortniteGame::FortRangedWeaponStats::AthenaCrouchingSpreadMultiplier = Updater->FindOffset("FortRangedWeaponStats", "AthenaCrouchingSpreadMultiplier");
		Offsets::FortniteGame::FortRangedWeaponStats::AthenaSprintingSpreadMultiplier = Updater->FindOffset("FortRangedWeaponStats", "RecoilDownsightsMultiplier");
		Offsets::FortniteGame::FortRangedWeaponStats::MinSpeedForSpreadMultiplier = Updater->FindOffset("FortRangedWeaponStats", "MinSpeedForSpreadMultiplier");
		Offsets::FortniteGame::FortRangedWeaponStats::MaxSpeedForSpreadMultiplier = Updater->FindOffset("FortRangedWeaponStats", "MaxSpeedForSpreadMultiplier");

		Offsets::FortniteGame::BuildingContainer::bAlreadySearched = Updater->FindOffset("BuildingContainer", "bAlreadySearched");
	}
	else
	{
		Warn("Unknown Error!");
	}
#ifdef updatershow
	/* std::cout << "Engine::World::Levels: 0x" << std::hex << std::uppercase << Offsets::Engine::World::Levels << std::endl;
	 std::cout << "Engine::World::PersistentLevel: 0x" << std::hex << std::uppercase << Offsets::Engine::World::PersistentLevel << std::endl;
	 std::cout << "Engine::GameInstance::LocalPlayers: 0x" << std::hex << std::uppercase << Offsets::Engine::GameInstance::LocalPlayers << std::endl;
	 std::cout << "Engine::World::OwningGameInstance: 0x" << std::hex << std::uppercase << Offsets::Engine::World::OwningGameInstance << std::endl;
	 std::cout << "Engine::Controller::ControlRotation: 0x" << std::hex << std::uppercase << Offsets::Engine::Controller::ControlRotation << std::endl;
	 std::cout << "Engine::PlayerController::PlayerCameraManager: 0x" << std::hex << std::uppercase << Offsets::Engine::PlayerController::PlayerCameraManager << std::endl;
	 std::cout << "Engine::PlayerController::AcknowledgedPawn: 0x" << std::hex << std::uppercase << Offsets::Engine::PlayerController::AcknowledgedPawn << std::endl;
	 std::cout << "Engine::Pawn::PlayerState: 0x" << std::hex << std::uppercase << Offsets::Engine::Pawn::PlayerState << std::endl;
	 std::cout << "Engine::Actor::RootComponent: 0x" << std::hex << std::uppercase << Offsets::Engine::Actor::RootComponent << std::endl;
	 std::cout << "Engine::Character::Mesh: 0x" << std::hex << std::uppercase << Offsets::Engine::Character::Mesh << std::endl;
	 std::cout << "Engine::SceneComponent::RelativeLocation: 0x" << std::hex << std::uppercase << Offsets::Engine::SceneComponent::RelativeLocation << std::endl;
	 std::cout << "Engine::SceneComponent::ComponentVelocity: 0x" << std::hex << std::uppercase << Offsets::Engine::SceneComponent::ComponentVelocity << std::endl;
	 std::cout << "Engine::StaticMeshComponent::StaticMesh: 0x" << std::hex << std::uppercase << Offsets::Engine::StaticMeshComponent::StaticMesh << std::endl;
	 std::cout << "Engine::SkinnedMeshComponent::CachedWorldSpaceBounds: 0x" << std::hex << std::uppercase << Offsets::Engine::SkinnedMeshComponent::CachedWorldSpaceBounds << std::endl;
	 std::cout << "Engine::Actor::CustomTimeDilation: 0x" << std::hex << std::uppercase << Offsets::Engine::Actor::CustomTimeDilation << std::endl;

	 std::cout << "FortniteGame::Offsets::FortniteGame::FortWeapon::LastFireTimeVerified: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortWeapon::LastFireTimeVerified << std::endl;
	 std::cout << "FortniteGame::Offsets::FortniteGame::FortWeapon::LastFireTime: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortWeapon::LastFireTime << std::endl;
	 std::cout << "FortniteGame::FortPawn::bIsDBNO: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortPawn::bIsDBNO << std::endl;
	 std::cout << "FortniteGame::FortPawn::bIsDying: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortPawn::bIsDying << std::endl;
	 std::cout << "FortniteGame::FortPlayerStateAthena::TeamIndex: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortPlayerStateAthena::TeamIndex << std::endl;
	 std::cout << "FortniteGame::FortPickup::PrimaryPickupItemEntry: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortPickup::PrimaryPickupItemEntry << std::endl;
	 std::cout << "FortniteGame::FortItemDefinition::DisplayName: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortItemDefinition::DisplayName << std::endl;
	 std::cout << "FortniteGame::FortItemDefinition::Tier: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortItemDefinition::Tier << std::endl;
	 std::cout << "FortniteGame::FortItemEntry::ItemDefinition: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortItemEntry::ItemDefinition << std::endl;
	 std::cout << "FortniteGame::FortPawn::CurrentWeapon: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortPawn::CurrentWeapon << std::endl;
	 std::cout << "FortniteGame::FortWeapon::WeaponData: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortWeapon::WeaponData << std::endl;
	 std::cout << "FortniteGame::FortWeapon::LastFireAbilityTime: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortWeapon::LastFireAbilityTime << std::endl;
	 std::cout << "FortniteGame::FortWeaponItemDefinition::WeaponStatHandle: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortWeaponItemDefinition::WeaponStatHandle << std::endl;
	 std::cout << "FortniteGame::FortProjectileAthena::FireStartLoc: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortProjectileAthena::FireStartLoc << std::endl;
	 std::cout << "FortniteGame::FortBaseWeaponStats::ReloadTime: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortBaseWeaponStats::ReloadTime << std::endl;
	 std::cout << "FortniteGame::FortBaseWeaponStats::ReloadScale: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortBaseWeaponStats::ReloadScale << std::endl;
	 std::cout << "FortniteGame::FortBaseWeaponStats::ChargeDownTime: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortBaseWeaponStats::ChargeDownTime << std::endl;
	 std::cout << "FortniteGame::FortRangedWeaponStats::RecoilHoriz: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortRangedWeaponStats::RecoilHoriz << std::endl;
	 std::cout << "FortniteGame::FortRangedWeaponStats::RecoilVert: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortRangedWeaponStats::RecoilVert << std::endl;
	 std::cout << "FortniteGame::FortRangedWeaponStats::RecoilDownsightsMultiplier: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortRangedWeaponStats::RecoilDownsightsMultiplier << std::endl;

	 std::cout << "FortniteGame::FortRangedWeaponStats::Spread: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortRangedWeaponStats::Spread << std::endl;
	 std::cout << "FortniteGame::FortRangedWeaponStats::SpreadDownsights: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortRangedWeaponStats::SpreadDownsights << std::endl;
	 std::cout << "FortniteGame::FortRangedWeaponStats::StandingStillSpreadMultiplier: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortRangedWeaponStats::StandingStillSpreadMultiplier << std::endl;
	 std::cout << "FortniteGame::FortRangedWeaponStats::AthenaJumpingFallingSpreadMultiplier: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortRangedWeaponStats::AthenaJumpingFallingSpreadMultiplier << std::endl;
	 std::cout << "FortniteGame::FortRangedWeaponStats::AthenaCrouchingSpreadMultiplier: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortRangedWeaponStats::AthenaCrouchingSpreadMultiplier << std::endl;
	 std::cout << "FortniteGame::FortRangedWeaponStats::AthenaSprintingSpreadMultiplier: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortRangedWeaponStats::AthenaSprintingSpreadMultiplier << std::endl;
	 std::cout << "FortniteGame::FortRangedWeaponStats::MinSpeedForSpreadMultiplier: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortRangedWeaponStats::MinSpeedForSpreadMultiplier << std::endl;
	 std::cout << "FortniteGame::FortRangedWeaponStats::MaxSpeedForSpreadMultiplier: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::FortRangedWeaponStats::MaxSpeedForSpreadMultiplier << std::endl;

	 std::cout << "FortniteGame::BuildingContainer::bAlreadySearched: 0x" << std::hex << std::uppercase << Offsets::FortniteGame::BuildingContainer::bAlreadySearched << std::endl;
	 std::cout << "Offsets::Engine::Controller::RemoteViewPitch: 0x" << std::hex << std::uppercase << Offsets::Engine::Controller::RemoteViewPitch << std::endl;*/
#endif 
	game_rbx_jmp = gadget(NULL);
	if (!InitGetKeys()) { Warn("InitGetKeys failed"); return; }
	if (!InitD3DHook()) { Warn("d3dhook failed"); return; }
}