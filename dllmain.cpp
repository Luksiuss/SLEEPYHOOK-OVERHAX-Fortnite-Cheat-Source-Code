#include "includes.h"
#include "MatrixStructs.h"
#include "Major.h"
#include "DrawMenu.h"
#include "MainHooking.h"

void Tick(nk_command_buffer* b)
{

	if (!b) return;
	X = (float)spoof_call(game_rbx_jmp, GetSystemMetrics, SM_CXSCREEN);
	Y = (float)spoof_call(game_rbx_jmp, GetSystemMetrics, SM_CYSCREEN);
	Draw->SetBuffer(b);
	Draw->SetOverlaySize(X, Y);
	entityx = 0;
	bool bUpdated = MainAddress();
	if (bUpdated)
	{
		if (bRandomHit) { RandomiseHit(); }
		Exploits();
		if (!LocalPawn) return;
		if (bAimbot || bSilent || bESP) { OnlyActorsLoop(); }
		if (bAimbot) if (GetAimKey()) DoAimbot(CamRot);
		if (bAimingLine) DrawAimingEnemy();
	}

	if (bCrosshair) Crosshair(X, Y);
	if (bDrawCircle) { float  NewFovRadius = (AimFov * X / GlobalFOV) / 2; Draw->DrawFOV(NewFovRadius); }
	if (bUpdated) RadarLoop();
	if (bUpdated && entityx)
	{
		if (!bUpdated) { entityx = 0; return; }
		if (!read<uint64_t>(entityx + Offsets::Engine::Actor::RootComponent)) { entityx = 0; return; }
		if (!read<uint64_t>(entityx + Offsets::Engine::Pawn::PlayerState)) { entityx = 0; return; }
		if (!read<uint64_t>(entityx + Offsets::Engine::Character::Mesh)) { entityx = 0; return; }
		uint8_t bIsDying = *(uint8_t*)(entityx + Offsets::FortniteGame::FortPawn::bIsDying);
		if (BIT_CHECK(bIsDying, 3)) { entityx = 0; return; }
	} 
	if (ShowMenu) DrawCursor(); 
	if (ShowMenu && bUpdated)
	{
		if (PlayerController && read<uintptr_t>(PlayerController))
		{
			auto SetIgnoreLookInput = (*(void(__fastcall**)(uint64_t, char bNewLookInput))(*(uint64_t*)PlayerController + 0x728));
			spoof_call(game_rbx_jmp, SetIgnoreLookInput, PlayerController, (char)1);
		}
	}
	Draw->SetBuffer(NULL);
}

BOOL APIENTRY DllMain(HMODULE module, DWORD reason, LPVOID reserved) {
	if (reason == DLL_PROCESS_ATTACH) {
		DisableThreadLibraryCalls(module);
		Main();
	}

	return TRUE;
}