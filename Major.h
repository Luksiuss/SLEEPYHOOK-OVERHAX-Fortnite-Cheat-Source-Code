#pragma once
#include "includes.h"

Vector3 GetBoneLocByIdx(uintptr_t mesh, int id)
{
	if (!mesh) return { 0,0,0 };

	auto fGetBoneMatrix = ((FMatrix * (__fastcall*)(uintptr_t, FMatrix*, int))(Offsets::fnGetBoneMatrix));
	spoof_call(game_rbx_jmp, fGetBoneMatrix, mesh, myMatrix, id);

	return Vector3(myMatrix->M[3][0], myMatrix->M[3][1], myMatrix->M[3][2]);
}

Vector3 ProjectWorldToScreen(Vector3 WorldLocation, Vector3 camrot)
{
	Vector3 Screenlocation = Vector3(0, 0, 0);
	Vector3 Rotation = camrot;

	D3DMATRIX tempMatrix = Matrix(Rotation);

	Vector3 vAxisX, vAxisY, vAxisZ;

	vAxisX = Vector3(tempMatrix.m[0][0], tempMatrix.m[0][1], tempMatrix.m[0][2]);
	vAxisY = Vector3(tempMatrix.m[1][0], tempMatrix.m[1][1], tempMatrix.m[1][2]);
	vAxisZ = Vector3(tempMatrix.m[2][0], tempMatrix.m[2][1], tempMatrix.m[2][2]);

	Vector3 vDelta = WorldLocation - CamLoc;
	Vector3 vTransformed = Vector3(vDelta.Dot(vAxisY), vDelta.Dot(vAxisZ), vDelta.Dot(vAxisX));

	float FovAngle = GlobalFOV;
	float ScreenCenterX = X / 2.0f;
	float ScreenCenterY = Y / 2.0f;

	if (vTransformed.z < 1.f || tanf(FovAngle * (float)M_PI / 360.f) == 0.f) return Vector3(0, 0, 0);

	Screenlocation.x = ScreenCenterX + vTransformed.x * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;
	Screenlocation.y = ScreenCenterY - vTransformed.y * (ScreenCenterX / tanf(FovAngle * (float)M_PI / 360.f)) / vTransformed.z;

	return Screenlocation;
}

uintptr_t hkGetCameraInfo(uintptr_t a1, FMinimalViewInfo* a2)
{
	static float last_fire_ability_time = 0.f;

	uintptr_t ret = spoof_call(game_rbx_jmp, o_GetCameraInfo, a1, a2);

	CamLoc = a2->Loc;
	CamRot = a2->Rot;

	if (FOVSlider)
	{
		a2->FOV = (float)FOVMoment;
	}

	GlobalFOV = a2->FOV;

	if (bSilent)
	{
		if (!IsValid(LocalPawn) || !IsValid(entityx)) return ret;

		LocalWeapon = read<uintptr_t>(LocalPawn + Offsets::FortniteGame::FortPawn::CurrentWeapon);

		if (LocalWeapon)
		{
			if (*(float*)(LocalWeapon + Offsets::FortniteGame::FortWeapon::LastFireAbilityTime) != last_fire_ability_time)
			{
				Vector3 SilentAimAngle = CalcAim();

				if (SilentAimAngle.x != 0 || SilentAimAngle.y != 0)
					a2->Rot = SilentAimAngle;

				last_fire_ability_time = *(float*)(LocalWeapon + Offsets::FortniteGame::FortWeapon::LastFireAbilityTime);
				if (bRapidFire)
				{
					*(float*)(LocalWeapon + Offsets::FortniteGame::FortWeapon::LastFireAbilityTime) = 0.1;
				}

			}
		}
	}

	return ret;
}

bool InstallCameraHook(uintptr_t PlayerCameraManager)
{
	uintptr_t vTable_address = *(uintptr_t*)(PlayerCameraManager);
	if ((uintptr_t)hkGetCameraInfo == read<uintptr_t>(vTable_address + (0xCC * 0x8))) return true;
	if (!vTable_address) return false;
	int num_virtual_methods = 0;
	int z = 0;
	while (read<uintptr_t>(vTable_address + (z * 0x8))) {
		num_virtual_methods += 1;
		z++;
	}
	for (int i = 0; i < num_virtual_methods; i++)
	{
		Dummy_vTable[i] = read<uintptr_t>(vTable_address + (i * 0x8));
	}
	o_GetCameraInfo = (Cam)(read<uintptr_t>(vTable_address + (0xCC * 8)));
	Dummy_vTable[0xCC] = (uintptr_t)hkGetCameraInfo;
	*(uintptr_t**)(PlayerCameraManager) = Dummy_vTable;
	return true;
}

void* SetHook_1(void* pSource, void* pDestination, int dwLen)
{
	DWORD MinLen = 14;

	if (dwLen < MinLen) return NULL;

	BYTE stub[] = {
	0xFF, 0x25, 0x00, 0x00, 0x00, 0x00, // jmp qword ptr [$+6]
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 // ptr
	};

	static const void* pTrampoline = nullptr;
	if (!pTrampoline) {
		const auto ntdll = reinterpret_cast<const unsigned char*>(spoof_call(game_rbx_jmp, GetModuleHandleW, (LPCWSTR)NULL));
		const auto dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(ntdll);
		const auto nt = reinterpret_cast<const IMAGE_NT_HEADERS*>(ntdll + dos->e_lfanew);
		const auto sections = IMAGE_FIRST_SECTION(nt);
		const auto num_sections = nt->FileHeader.NumberOfSections;

		constexpr char section_name[5]{ '.', 't', 'e', 'x', 't' };
		const auto     section = std::find_if(sections, sections + num_sections, [&](const auto& s) {
			return std::equal(s.Name, s.Name + 5, section_name);
			});

		constexpr unsigned char instr_bytes[2]{ 0xFF, 0x25 };
		const auto              va = ntdll + section->VirtualAddress;
		pTrampoline = std::search(va, va + section->Misc.VirtualSize, instr_bytes, instr_bytes + 2);
	}

	DWORD64 retto = (DWORD64)pSource + dwLen;

	// trampoline
	memcpy(stub + 6, &retto, 8);
	memcpy((void*)((DWORD_PTR)pTrampoline), pSource, dwLen);
	memcpy((void*)((DWORD_PTR)pTrampoline + dwLen), stub, sizeof(stub));

	// orig
	memcpy(stub + 6, &pDestination, 8);
	memcpy(pSource, stub, sizeof(stub));

	for (int i = MinLen; i < dwLen; i++)
	{
		*(BYTE*)((DWORD_PTR)pSource + i) = 0x90;
	}

	return (void*)((DWORD_PTR)pTrampoline);
}

FBox GetFBox(uintptr_t Actor)
{
	if (!Actor) return {};

	Vector3 Origin, BoxExtend;

	auto fGetActorBounds = reinterpret_cast<void(__fastcall*)(__int64, char, Vector3*, Vector3*)>(Offsets::fnGetBounds);

	spoof_call(game_rbx_jmp, fGetActorBounds, (__int64)Actor, (char)true, &Origin, &BoxExtend);

	FBox NewBox;
	NewBox.IsValid = 1;
	NewBox.Min = Origin - BoxExtend;
	NewBox.Max = Origin + BoxExtend;

	return NewBox;
}

BOOLEAN GetTargetHead(GetStructs::FVector& out) {
	if (!TargetPawn) {
		return FALSE;
	}

	auto mesh = ReadPointer(TargetPawn, 0x278);
	if (!mesh) {
		return FALSE;
	}

	auto bones = ReadPointer(mesh, 0x420);
	if (!bones) bones = ReadPointer(mesh, 0x420 + 0x10);
	if (!bones) {
		return FALSE;
	}

	float compMatrix[4][4] = { 0 };
	ToMatrixWithScale(reinterpret_cast<float*>(reinterpret_cast<PBYTE>(mesh) + 0x1C0), compMatrix);

	GetBoneLocation(compMatrix, bones, 66, &out.X);
	return TRUE;
}

Vector3 CalcAim()
{
	Vector3 RetVector = { 0,0,0 };

	if (!entityx) return { 0,0,0 };

	uint64_t currentactormesh = read<uint64_t>(entityx + Offsets::Engine::Character::Mesh);
	if (!currentactormesh) return { 0,0,0 };

	Vector3 rootHead = GetBoneLocByIdx(currentactormesh, AimBoneInt);
	if (rootHead.x == 0 && rootHead.y == 0) return Vector3(0, 0, 0);

	Vector3 VectorPos = rootHead - CamLoc;

	float distance = VectorPos.Length();
	RetVector.x = -(((float)acos(VectorPos.z / distance) * (float)(180.0f / M_PI)) - 90.f);
	RetVector.y = (float)atan2(VectorPos.y, VectorPos.x) * (float)(180.0f / M_PI);

	return RetVector;
}

double GetCrossDistance(double x1, double y1, double x2, double y2)
{
	return spoof_call(game_rbx_jmp, sqrtf, spoof_call(game_rbx_jmp, powf, (float)(x1 - x2), (float)2) + spoof_call(game_rbx_jmp, powf, (float)(y1 - y2), (float)2));
}

bool GetClosestPlayerToCrossHair(Vector3 Pos, float& max, DWORD_PTR entity)
{
	if (entity)
	{
		float Dist = GetCrossDistance(Pos.x, Pos.y, (X / 2), (Y / 2));

		if (Dist < max)
		{

			//GlobalFOV : X = NewFOV : NewSize
			//NewSize = NewFOV * X / GlobalFOV

			float Radius = (AimFov * X / GlobalFOV) / 2;

			if (Pos.x <= ((X / 2) + Radius) &&
				Pos.x >= ((X / 2) - Radius) &&
				Pos.y <= ((Y / 2) + Radius) &&
				Pos.y >= ((Y / 2) - Radius))
			{
				max = Dist;
				entityx = entity;
				return true;
			}

			return false;
		}
	}

	return false;
}

void CheckClosestFOVEntity(DWORD_PTR entity, Vector3 Localcam, float& max)
{
	if (!entity || !LocalPawn || !LocalWeapon) return;

	uint64_t currentactormesh = read<uint64_t>(entity + Offsets::Engine::Character::Mesh);
	if (!currentactormesh) return;

	Vector3 rootHead = GetBoneLocByIdx(currentactormesh, 66);
	Vector3 rootHeadOut = ProjectWorldToScreen(rootHead, CamRot);

	if (rootHeadOut.x <= 0 || rootHeadOut.y <= 0) return;
	if (rootHeadOut.x >= X || rootHeadOut.y >= Y) return;

	if (bVisible && !IsTargetVisible(entity)) return;

	if (GetClosestPlayerToCrossHair(rootHeadOut, max, entity))
		entityx = entity;
}

void CheckClosestDistEntity(DWORD_PTR entity, Vector3 Localcam, float& max)
{
	if (!entity) return;

	uint64_t currentactormesh = read<uint64_t>(entity + Offsets::Engine::Character::Mesh);
	if (!currentactormesh) return;

	Vector3 rootHead = GetBoneLocByIdx(currentactormesh, 66);

	if (rootHead.x == 0 && rootHead.y == 0) return;

	if (bVisible && !IsTargetVisible(entity)) return;

	Vector3 Delta = rootHead - CamLoc;

	float Dist = spoof_call(game_rbx_jmp, sqrtf, spoof_call(game_rbx_jmp, powf, (Delta.x), (float)2) + spoof_call(game_rbx_jmp, powf, (Delta.y), (float)2) + spoof_call(game_rbx_jmp, powf, (Delta.z), (float)2));

	if (AimDistance != 300)
	{
		if ((Dist * 0.01f) > (float)AimDistance) return;
	}

	if ((Dist * 0.01f) < max)
	{
		max = (Dist * 0.01f);
		entityx = entity;
		return;
	}
	return;
}

void Draw3DBoundingBox(uintptr_t mesh, uint64_t CurrentActor, nk_color color)
{
	if (!CurrentActor) return;

	Vector3 min, max, vec1, vec2, vec3, vec4, vec5, vec6, vec7, vec8;

	FBox box = GetFBox(CurrentActor);

	if (!box.IsValid) return;

	min = box.Min;
	max = box.Max;

	vec3 = min;
	vec3.x = max.x;
	vec4 = min;
	vec4.y = max.y;
	vec5 = min;
	vec5.z = max.z;
	vec6 = max;
	vec6.x = min.x;
	vec7 = max;
	vec7.y = min.y;
	vec8 = max;
	vec8.z = min.z;
	vec1 = ProjectWorldToScreen(min, CamRot);
	vec2 = ProjectWorldToScreen(max, CamRot);
	vec3 = ProjectWorldToScreen(vec3, CamRot);
	vec4 = ProjectWorldToScreen(vec4, CamRot);
	vec5 = ProjectWorldToScreen(vec5, CamRot);
	vec6 = ProjectWorldToScreen(vec6, CamRot);
	vec7 = ProjectWorldToScreen(vec7, CamRot);
	vec8 = ProjectWorldToScreen(vec8, CamRot);

	if (vec1.x == 0 && vec1.y == 0) return;
	if (vec2.x == 0 && vec2.y == 0) return;
	if (vec3.x == 0 && vec3.y == 0) return;
	if (vec4.x == 0 && vec4.y == 0) return;
	if (vec5.x == 0 && vec5.y == 0) return;
	if (vec6.x == 0 && vec6.y == 0) return;
	if (vec7.x == 0 && vec7.y == 0) return;
	if (vec8.x == 0 && vec8.y == 0) return;


	Draw->DrawLine(vec1.x, vec1.y, vec5.x, vec5.y, 1, color);
	Draw->DrawLine(vec2.x, vec2.y, vec8.x, vec8.y, 1, color);
	Draw->DrawLine(vec3.x, vec3.y, vec7.x, vec7.y, 1, color);
	Draw->DrawLine(vec4.x, vec4.y, vec6.x, vec6.y, 1, color);
	Draw->DrawLine(vec1.x, vec1.y, vec3.x, vec3.y, 1, color);
	Draw->DrawLine(vec1.x, vec1.y, vec4.x, vec4.y, 1, color);
	Draw->DrawLine(vec8.x, vec8.y, vec3.x, vec3.y, 1, color);
	Draw->DrawLine(vec8.x, vec8.y, vec4.x, vec4.y, 1, color);
	Draw->DrawLine(vec2.x, vec2.y, vec6.x, vec6.y, 1, color);
	Draw->DrawLine(vec2.x, vec2.y, vec7.x, vec7.y, 1, color);
	Draw->DrawLine(vec5.x, vec5.y, vec6.x, vec6.y, 1, color);
	Draw->DrawLine(vec5.x, vec5.y, vec7.x, vec7.y, 1, color);
}

void Draw2DBoundingBox(Vector3 StartBoxLoc, float flWidth, float Height, nk_color color)
{
	StartBoxLoc.x = StartBoxLoc.x - (flWidth / 2);

	Draw->DrawLine(StartBoxLoc.x, StartBoxLoc.y, StartBoxLoc.x + flWidth, StartBoxLoc.y, 1, color); //bottom
	Draw->DrawLine(StartBoxLoc.x, StartBoxLoc.y, StartBoxLoc.x, StartBoxLoc.y + Height, 1, color); //left
	Draw->DrawLine(StartBoxLoc.x + flWidth, StartBoxLoc.y, StartBoxLoc.x + flWidth, StartBoxLoc.y + Height, 1, color); //right
	Draw->DrawLine(StartBoxLoc.x, StartBoxLoc.y + Height, StartBoxLoc.x + flWidth, StartBoxLoc.y + Height, 1, color); //up
}

void DrawSkeleton(DWORD_PTR mesh)
{
	std::list<int> upper_part = { 65,66 };
	std::list<int> right_arm = { 65, BONE_R_ARM_TOP, BONE_R_ARM_LOWER, BONE_MISC_R_HAND_1 };
	std::list<int> left_arm = { 65, BONE_L_ARM_TOP, BONE_L_ARM_LOWER, BONE_MISC_L_HAND };
	std::list<int> spine = { 65, BONE_PELVIS_1 };
	std::list<int> lower_right = { BONE_PELVIS_2, BONE_R_THIGH ,76 };
	std::list<int> lower_left = { BONE_PELVIS_2, BONE_L_THIGH ,69 };
	std::list<std::list<int>> Skeleton = { upper_part, right_arm, left_arm, spine, lower_right, lower_left };

	if (!mesh) return;

	nk_color col = { 102, 255, 204,255 };

	Vector3 neckpos = GetBoneLocByIdx(mesh, 65);
	Vector3 pelvispos = GetBoneLocByIdx(mesh, BONE_PELVIS_2);

	Vector3 previous(0, 0, 0);
	Vector3 current, p1, c1;

	for (auto a : Skeleton)
	{
		previous = Vector3(0, 0, 0);
		for (int bone : a)
		{
			current = bone == 65 ? neckpos : (bone == BONE_PELVIS_2 ? pelvispos : GetBoneLocByIdx(mesh, bone));
			if (previous.x == 0.f)
			{
				previous = current;
				continue;
			}

			p1 = ProjectWorldToScreen(previous, CamRot);
			c1 = ProjectWorldToScreen(current, CamRot);


			if (p1.x != 0 && p1.y != 0 && c1.x != 0 && c1.y != 0)
				Draw->DrawLine(p1.x, p1.y, c1.x, c1.y, 1, col);

			previous = current;
		}
	}
}


void inline DrawLaser(uintptr_t target, uintptr_t currentactormesh, nk_color color)
{
	if (target)
	{
		Vector3 target_rot = GetPawnEyeViewRot(target);
		Vector3 target_loc = GetBoneLocByIdx(currentactormesh, 66);

		if (target_rot.x == 0 && target_rot.y == 0) return;
		if (target_loc.x == 0 && target_loc.y == 0) return;

		double radiantsYAW = ((target_rot.y * M_PI) / 180);

		float RemotePitch = *(float*)(target + Offsets::Engine::Controller::RemoteViewPitch);
		double radiantsPITCH = (((float)RemotePitch * M_PI) / 180);
		double hyp = 250;
		double X = (hyp * cos(radiantsYAW));
		double Y = (hyp * sin(radiantsYAW));
		double Z = (hyp * sin(radiantsPITCH));
		float PlX = target_loc.x + X;
		float PlZ = target_loc.y + Y;
		float PlY = target_loc.z + Z;

		Vector3 Laser, Laser_out, HeadLoc_out;
		Laser.x = PlX;
		Laser.z = PlY;
		Laser.y = PlZ;

		Laser_out = ProjectWorldToScreen(Laser, CamRot);
		HeadLoc_out = ProjectWorldToScreen(target_loc, CamRot);

		if (Laser_out.x == 0 && Laser_out.y == 0) return;
		if (HeadLoc_out.x == 0 && HeadLoc_out.y == 0) return;

		Draw->DrawLine(HeadLoc_out.x, HeadLoc_out.y, Laser_out.x, Laser_out.y, 1, color);
	}
}

void inline DrawChest(uintptr_t chest)
{
	if (!chest) return;

	uintptr_t RootComponent = read<uintptr_t>(chest + Offsets::Engine::Actor::RootComponent);
	if (!RootComponent) return;

	Vector3 CenterLocation = *(Vector3*)(RootComponent + Offsets::Engine::SceneComponent::RelativeLocation);

	Vector3 Out_Screen = ProjectWorldToScreen(CenterLocation, CamRot);

	if (Out_Screen.x == 0 && Out_Screen.y == 0) return;

	Draw->nkDrawText(skCrypt("[CHEST]"), Out_Screen.x, Out_Screen.y, 7, nk_rgb(255, 255, 255));
}

void inline DrawAmmoBox(uintptr_t ammo)
{
	if (!ammo) return;

	uintptr_t RootComponent = read<uintptr_t>(ammo + Offsets::Engine::Actor::RootComponent);
	if (!RootComponent) return;

	Vector3 CenterLocation = *(Vector3*)(RootComponent + Offsets::Engine::SceneComponent::RelativeLocation);

	Vector3 Out_Screen = ProjectWorldToScreen(CenterLocation, CamRot);

	if (Out_Screen.x == 0 && Out_Screen.y == 0) return;

	Draw->nkDrawText(skCrypt("[AMMO BOX]"), Out_Screen.x, Out_Screen.y, 10, nk_rgb(255, 255, 255));
}

void inline DrawLlama(uintptr_t llama)
{
	if (!llama) return;

	uintptr_t RootComponent = read<uintptr_t>(llama + Offsets::Engine::Actor::RootComponent);
	if (!RootComponent) return;

	Vector3 CenterLocation = *(Vector3*)(RootComponent + Offsets::Engine::SceneComponent::RelativeLocation);

	Vector3 Out_Screen = ProjectWorldToScreen(CenterLocation, CamRot);

	if (Out_Screen.x == 0 && Out_Screen.y == 0) return;

	Draw->nkDrawText(skCrypt("[LLAMA]"), Out_Screen.x, Out_Screen.y, 7, nk_rgb(255, 0, 255));
}

Vector3 Normalize(Vector3 vec)
{
	float a1 = vec.x;
	float a2 = vec.y;
	float a3 = vec.z;
	double length = vec.Length();

	if (length == 0.0) return { 0,0,0 };

	return Vector3(a1 / length, a2 / length, a3 / length);
}

Vector3 inline LimitRotation(Vector3 startRotation, Vector3 endRotation)
{
	Vector3 ret;
	auto scale = AimSmooth;
	auto currentRotation = startRotation;

	ret.x = (endRotation.x - startRotation.x) / scale + startRotation.x;
	ret.y = (endRotation.y - startRotation.y) / scale + startRotation.y;

	return ret;
}

void DoAimbot(Vector3 Localcam)
{

	if (!read<uint64_t>(entityx + Offsets::Engine::Actor::RootComponent))
	{
		entityx = 0;
		return;
	}
	if (!read<uint64_t>(entityx + Offsets::Engine::Pawn::PlayerState))
	{
		entityx = 0;
		return;
	}
	if (!read<uint64_t>(entityx + Offsets::Engine::Character::Mesh))
	{
		entityx = 0;
		return;
	}
	uint8_t bIsDying = *(uint8_t*)(entityx + Offsets::FortniteGame::FortPawn::bIsDying);
	if (BIT_CHECK(bIsDying, 3))
	{
		entityx = 0;
		return;
	}

	if (!PlayerController || !read<uintptr_t>(PlayerController))
	{
		entityx = 0;
		return;
	}

	Vector3 NewAngle = CalcAim();

	if (NewAngle.x == 0 && NewAngle.y == 0) return;

	if (AimSmooth > 0)
		NewAngle = LimitRotation(CamRot, NewAngle);

	NewAngle.z = 0;

	if (aimhelper)
	{
		auto SetIgnoreLookInput = (*(void(__fastcall**)(uint64_t, char bNewLookInput))(*(uint64_t*)PlayerController + 0x728));
		spoof_call(game_rbx_jmp, SetIgnoreLookInput, PlayerController, (char)1);
	}

	auto ClientSetRotation = (*(void(__fastcall**)(uint64_t, Vector3, char))(*(uint64_t*)PlayerController + 0x628));
	spoof_call(game_rbx_jmp, ClientSetRotation, PlayerController, NewAngle, (char)0);
}


void DrawAimingEnemy()
{
	if (!entityx) return;

	uint64_t currentactormesh = read<uint64_t>(entityx + Offsets::Engine::Character::Mesh);
	if (!currentactormesh) return;

	nk_color col = { 0,255,0,255 };

	Vector3 target_loc = GetBoneLocByIdx(currentactormesh, 66);
	if (target_loc.x == 0 && target_loc.y == 0) return;

	Vector3 HeadLoc_out = ProjectWorldToScreen(target_loc, CamRot);

	if (HeadLoc_out.x == 0 && HeadLoc_out.y == 0) return;

	Draw->DrawLine(HeadLoc_out.x, HeadLoc_out.y, X / 2, Y / 2, 1, col);
}

Vector3 WorldToRadar(Vector3 Location, INT RadarX, INT RadarY, int size)
{
	Vector3 Return;

	FLOAT CosYaw = spoof_call(game_rbx_jmp, cosf, (float)((CamRot.y) * M_PI / 180.f));
	FLOAT SinYaw = spoof_call(game_rbx_jmp, sinf, (float)((CamRot.y) * M_PI / 180.f));

	FLOAT DeltaX = Location.x - CamLoc.x;
	FLOAT DeltaY = Location.y - CamLoc.y;

	FLOAT LocationX = (DeltaY * CosYaw - DeltaX * SinYaw) / (200);
	FLOAT LocationY = (DeltaX * CosYaw + DeltaY * SinYaw) / (200);

	if (LocationX > ((size / 2) - 5.0f) - 2.5f)
		LocationX = ((size / 2) - 5.0f) - 2.5f;
	else if (LocationX < -(((size / 2) - 5.0f) - 2.5f))
		LocationX = -(((size / 2) - 5.0f) - 2.5f);

	if (LocationY > ((size / 2) - 5.0f) - 2.5f)
		LocationY = ((size / 2) - 5.0f) - 2.5f;
	else if (LocationY < -(((size / 2) - 5.0f) - 2.5f))
		LocationY = -(((size / 2) - 5.0f) - 2.5f);

	Return.x = LocationX + RadarX;
	Return.y = -LocationY + RadarY;

	return Return;
}

void RadarDraw(int Size)
{
	nk_color blue;
	blue.a = 127; blue.b = 0; blue.g = 0; blue.r = 0;
	nk_color red;
	red.a = 127; red.b = 80; red.g = 80; red.r = 80;
	//BOX
	Draw->DrawFilledRect(1200, 10, Size, Size, 0.f, blue);
	Draw->DrawFilledRect(1201, 11, Size - 2, Size - 2, 0.f, red);

	//CROSS
	Draw->DrawLine(1200 + (Size / 2), 10, 1200 + (Size / 2), 10 + Size, 1, blue);
	Draw->DrawLine(1200, 10 + (Size / 2), 1200 + Size, 10 + (Size / 2), 1, blue);

	if (!PlayerController
		|| !PlayerCameraManager) return;

	//DefFOV : Size = NewFOV : NewSize -> NewSize = NewFOV * Size / DefFOV
	float NewFOV = (GlobalFOV * Size) / 90.f;

	//Draw FOV
	if (NewFOV <= Size)
	{
		Draw->DrawLine(1200 + (Size / 2), 10 + (Size / 2), 1200 + (Size / 2) + (NewFOV / 2), 10, 1, blue);
		Draw->DrawLine(1200 + (Size / 2), 10 + (Size / 2), 1200 + (Size / 2) - (NewFOV / 2), 10, 1, blue);
	}
	else
	{
		float NewHeight = (Size / 2) * (NewFOV - (Size / 2)) / NewFOV;


		Draw->DrawLine(1200 + (Size / 2), 10 + (Size / 2), 1200, 10 + NewHeight, 1, blue);
		Draw->DrawLine(1200 + (Size / 2), 10 + (Size / 2), 1200 + Size, 10 + NewHeight, 1, blue);
	}

	nk_color black;
	black.a = 255; black.b = 0; black.g = 0; black.r = 0;
	nk_color white;
	white.a = 255; white.b = 255; white.g = 255; white.r = 255;

	//My Player
	Draw->DrawFilledRect(1200 + (Size / 2) - 2, 10 + (Size / 2) - 2, 4, 4, 0.f, black);
	Draw->DrawFilledRect(1200 + (Size / 2) - 2, 10 + (Size / 2) - 2, 2, 2, 0.f, white);
}

void AddTargetToRadar(Vector3 ActorLoc, int RadarSize, nk_color col)
{
	Vector3 RadarCoords = WorldToRadar(ActorLoc, 1200 + (RadarSize / 2), 10 + (RadarSize / 2), RadarSize - 2);

	nk_color orange;
	orange.a = 255; orange.b = 0; orange.g = 0; orange.r = 0;

	Draw->DrawFilledRect(RadarCoords.x - 2, RadarCoords.y - 2, 4, 4, 0.f, orange);
	Draw->DrawFilledRect(RadarCoords.x - 1, RadarCoords.y - 1, 2, 2, 0.f, col);
}

void RadarLoop()
{
	if (bESP2D)
	{
		nk_color BOSScol;
		BOSScol.r = 255; BOSScol.g = 0; BOSScol.b = 255; BOSScol.a = 255;

		nk_color BOTcol;
		BOTcol.r = 255; BOTcol.g = 165; BOTcol.b = 0; BOTcol.a = 255;

		nk_color col;
		col.r = 255; col.g = 0; col.b = 0; col.a = 255;

		nk_color TEAMcol;
		TEAMcol.r = 0; TEAMcol.g = 255; TEAMcol.b = 0; TEAMcol.a = 255;

		int MyTeamID = 0;
		int TeamID = 1;

		if (!LocalPawn) return;

		uintptr_t MyPlayerState = read<uint64_t>(LocalPawn + Offsets::Engine::Pawn::PlayerState);
		if (MyPlayerState) MyTeamID = *(int*)(MyPlayerState + Offsets::FortniteGame::FortPlayerStateAthena::TeamIndex);

		RadarDraw(300);

		for (int i = 0; i < actor_count; i++)
		{
			uint64_t CurrentActor = read<uint64_t>(AActors + i * 0x8);
			if (CurrentActor == (uint64_t)nullptr || CurrentActor == -1 || CurrentActor == NULL) continue;

			int curactorid = *(int*)(CurrentActor + 0x18);

			if (curactorid == 0) continue;

			if (CurrentActor != LocalPawn && CurrentActor != Localplayer && CurrentActor != PlayerController)
			{
				if (!CurrentActor) continue;

				if (curactorid == Actors[0] || curactorid == Actors[1])
				{
					uint8_t bIsDying = *(uint8_t*)(CurrentActor + Offsets::FortniteGame::FortPawn::bIsDying);
					if (BIT_CHECK(bIsDying, 3)) continue;

					uintptr_t PlayerState = read<uint64_t>(CurrentActor + Offsets::Engine::Pawn::PlayerState);
					if (PlayerState) TeamID = *(int*)(PlayerState + Offsets::FortniteGame::FortPlayerStateAthena::TeamIndex);

					uint64_t RootComponents = read<uint64_t>(CurrentActor + Offsets::Engine::Actor::RootComponent);
					if (!RootComponents) continue;

					Vector3 Headpos = *(Vector3*)(RootComponents + Offsets::Engine::SceneComponent::RelativeLocation);

					if (TeamID != MyTeamID)
						AddTargetToRadar(Headpos, 300, col);
					else
						AddTargetToRadar(Headpos, 300, TEAMcol);
				}
				else
				{
					if (curactorid == Bots[0] || (curactorid >= (Bots[1] - 5) && curactorid <= (Bots[1] + 5)))
					{
						uint8_t bIsDying = *(uint8_t*)(CurrentActor + Offsets::FortniteGame::FortPawn::bIsDying);
						if (BIT_CHECK(bIsDying, 3)) continue;

						uint64_t RootComponents = read<uint64_t>(CurrentActor + Offsets::Engine::Actor::RootComponent);
						if (!RootComponents) continue;

						Vector3 Headpos = *(Vector3*)(RootComponents + Offsets::Engine::SceneComponent::RelativeLocation);

						if (curactorid == Bots[0])
							AddTargetToRadar(Headpos, 300, BOTcol);
						else
							AddTargetToRadar(Headpos, 300, BOSScol);
					}
				}
			}
		}
	}
}

void inline DrawObjects(std::string name, uintptr_t object)
{
	if (!object) return;

	uintptr_t RootComponent = read<uintptr_t>(object + Offsets::Engine::Actor::RootComponent);
	if (!RootComponent) return;

	Vector3 CenterLocation = *(Vector3*)(RootComponent + Offsets::Engine::SceneComponent::RelativeLocation);

	Vector3 Out_Screen = ProjectWorldToScreen(CenterLocation, CamRot);

	if (Out_Screen.x == 0 && Out_Screen.y == 0) return;

	Draw->nkDrawText(name.c_str(), Out_Screen.x, Out_Screen.y, 7, nk_rgb(255, 255, 255));
}

void OnlyActorsLoop()
{
	if (!IsValid(LocalPawn) || !IsValid(LocalWeapon)) return;

	float FOVmax = 9999.f;
	float DistanceMax = 9999999999.f;

	int MyTeamID = 0;
	int TeamID = 1;

	for (int i = 0; i < actor_count; i++)
	{
		uint64_t CurrentActor = read<uint64_t>(AActors + i * 0x8);
		if (!CurrentActor) continue;

		int curactorid = *(int*)(CurrentActor + 0x18);
		if (curactorid == 0) continue;

		//Enemy
		if (CurrentActor != LocalPawn && CurrentActor != Localplayer && CurrentActor != PlayerController)
		{
			if (Actors[0] == 0 || Actors[1] == 0 || Bots[0] == 0 || Bots[1] == 0)
			{
				if (curactorid != Actors[0] &&
					curactorid != Actors[1] &&
					curactorid != Bots[0] &&
					curactorid != Bots[1])
				{
					char* ObjName = (char*)malloc(200);
					strcpy(ObjName, (char*)GetObjectName(CurrentActor).c_str());
					if (strstr((const char*)ObjName, (const char*)skCrypt_("PlayerPawn_Athena_C")))
						Actors[0] = curactorid;
					if (strstr((const char*)ObjName, (const char*)skCrypt_("BP_PlayerPawn_Athena_Phoebe_C")))
						Actors[1] = curactorid;
					if (strstr((const char*)ObjName, (const char*)skCrypt_("BP_MangPlayerPawn_Default_C")))
						Bots[0] = curactorid;
					if (strstr((const char*)ObjName, (const char*)skCrypt_("BP_MangPlayerPawn_Boss_")))
						Bots[1] = curactorid;
					free(ObjName);
				}
			}

			if ((curactorid == Actors[0] || curactorid == Actors[1]) && curactorid != 0)
			{
				uintptr_t MyPlayerState = read<uint64_t>(LocalPawn + Offsets::Engine::Pawn::PlayerState);
				if (MyPlayerState) MyTeamID = *(int*)(MyPlayerState + Offsets::FortniteGame::FortPlayerStateAthena::TeamIndex);

				uint8_t bIsDying = *(uint8_t*)(CurrentActor + Offsets::FortniteGame::FortPawn::bIsDying);
				if (BIT_CHECK(bIsDying, 3)) continue;

				uintptr_t PlayerState = read<uint64_t>(CurrentActor + Offsets::Engine::Pawn::PlayerState);
				if (PlayerState) TeamID = *(int*)(PlayerState + Offsets::FortniteGame::FortPlayerStateAthena::TeamIndex);

				if (bESP && (bShowEnemies || bShowFriends || PlayerNames))
				{
					uint64_t currentactormesh = read<uint64_t>(CurrentActor + Offsets::Engine::Character::Mesh);
					if (!currentactormesh) continue;

					Vector3 Headpos = GetBoneLocByIdx(currentactormesh, 66);

					if (Headpos.x == 0 && Headpos.y == 0) continue;

					Vector3 bone0 = GetBoneLocByIdx(currentactormesh, 0);

					Vector3 HeadposW2s = ProjectWorldToScreen(Headpos, CamRot);
					Vector3 bottom = ProjectWorldToScreen(bone0, CamRot);
					Vector3 Headbox = ProjectWorldToScreen(Vector3(Headpos.x, Headpos.y, Headpos.z + 15), CamRot);

					if (HeadposW2s.x == 0 && HeadposW2s.y == 0) continue;
					if (bottom.x == 0 && bottom.y == 0) continue;
					if (Headbox.x == 0 && Headbox.y == 0) continue;

					if (bSkeleton) DrawSkeleton(currentactormesh);

					if (TeamID != MyTeamID && bShowEnemies)
					{
						if (b3DBox)
						{
							if (BoxType == 0);// Draw3DBoundingBox(currentactormesh, CurrentActor, Enemy_color);
							else
							{
								float Height1 = Headbox.y - bottom.y;

								if (Height1 < 0)
									Height1 = Height1 * (-1.f);
								float Width1 = Height1 * 0.65;

								Draw2DBoundingBox(Headbox, Width1, Height1, Enemy_color);
							}

						}

						if (bLaser) DrawLaser(CurrentActor, currentactormesh, Enemy_color);

						if (bSnapLine) Draw->DrawLine(X / 2, Y, bottom.x, bottom.y, 1, Enemy_color);
					}

					if (TeamID == MyTeamID && bShowFriends)
					{
						if (b3DBox)
						{
							Draw3DBoundingBox(currentactormesh, CurrentActor, Team_color);
						}
						else if (b2DBox)
						{

							float Height1 = Headbox.y - bottom.y;

							if (Height1 < 0)
								Height1 = Height1 * (-1.f);

							float Width1 = Height1 * 0.65;

							Draw2DBoundingBox(Headbox, Width1, Height1, Team_color);
						}

						if (bLaser) DrawLaser(CurrentActor, currentactormesh, Team_color);

						if (bSnapLine) Draw->DrawLine(X / 2, Y, bottom.x, bottom.y, 1, Team_color);
					}
				}
				if ((bAimbot || bSilent) && (TeamID != MyTeamID))
				{
					if (AimType == 0) CheckClosestFOVEntity(CurrentActor, CamRot, FOVmax);
					else CheckClosestDistEntity(CurrentActor, CamRot, DistanceMax);
				}
			}
			else
			{
				if ((curactorid == Bots[0] || (curactorid >= (Bots[1] - 5) && curactorid <= (Bots[1] + 5))) && curactorid != 0)
				{
					uint8_t bIsDying = *(uint8_t*)(CurrentActor + Offsets::FortniteGame::FortPawn::bIsDying);
					if (BIT_CHECK(bIsDying, 3)) continue;
					if (bESP && (bShowBot || bShowBoss))
					{
						uint64_t currentactormesh = read<uint64_t>(CurrentActor + Offsets::Engine::Character::Mesh);
						if (!currentactormesh) continue;

						Vector3 Headpos = GetBoneLocByIdx(currentactormesh, 66);

						if (Headpos.x == 0 && Headpos.y == 0) continue;
						Vector3 HeadposW2s = ProjectWorldToScreen(Headpos, CamRot);
						Vector3 bone0 = GetBoneLocByIdx(currentactormesh, 0);
						Vector3 bottom = ProjectWorldToScreen(bone0, CamRot);
						Vector3 Headbox = ProjectWorldToScreen(Vector3(Headpos.x, Headpos.y, Headpos.z + 15), CamRot);

						if (HeadposW2s.x == 0 && HeadposW2s.y == 0) continue;
						if (bottom.x == 0 && bottom.y == 0) continue;
						if (Headbox.x == 0 && Headbox.y == 0) continue;

						if (bSkeleton) DrawSkeleton(currentactormesh);
						if (curactorid == Bots[0] && bShowBot)
						{
							if (b3DBox)
							{
								float Height1 = Headbox.y - bottom.y;

								if (Height1 < 0)
									Height1 = Height1 * (-1.f);
								float Width1 = Height1 * 0.65;

								Draw2DBoundingBox(Headbox, Width1, Height1, Bot_color);
							}

							if (bLaser) DrawLaser(CurrentActor, currentactormesh, Bot_color);
							if (bCheckIfReal) Draw->nkDrawText(skCrypt_("Bot / AI"), bottom.x, bottom.y, 7, nk_rgb(128, 0, 128));
							if (bSnapLine) Draw->DrawLine(X / 2, Y, bottom.x, bottom.y, 1, Bot_color);
						}
						else
						{
							if (bShowBoss)
							{
								if (b3DBox)
								{
									float Height1 = Headbox.y - bottom.y;
									if (Height1 < 0)
										Height1 = Height1 * (-1.f);
									float Width1 = Height1 * 0.65;

									Draw2DBoundingBox(Headbox, Width1, Height1, Boss_color);
								}
								if (bLaser) DrawLaser(CurrentActor, currentactormesh, Boss_color);
								if (bSnapLine) Draw->DrawLine(X / 2, Y, bottom.x, bottom.y, 1, Boss_color);
							}
						}
					}
					if ((bAimbot || bSilent))
					{
						if (AimType == 0) CheckClosestFOVEntity(CurrentActor, CamRot, FOVmax);
						else CheckClosestDistEntity(CurrentActor, CamRot, DistanceMax);
					}
				}
			}
		}
	}
	if (!OldAimingActor && IsValid(entityx))
		if (GetAimKey())
			OldAimingActor = entityx;

	if (IsValid(OldAimingActor))
	{
		if (!read<uint64_t>(OldAimingActor + Offsets::Engine::Actor::RootComponent))
		{
			OldAimingActor = 0;
			return;
		}
		if (!read<uint64_t>(OldAimingActor + Offsets::Engine::Pawn::PlayerState))
		{
			OldAimingActor = 0;
			return;
		}
		if (!read<uint64_t>(OldAimingActor + Offsets::Engine::Character::Mesh))
		{
			OldAimingActor = 0;
			return;
		}
		uint8_t bIsDying = *(uint8_t*)(OldAimingActor + Offsets::FortniteGame::FortPawn::bIsDying);
		if (BIT_CHECK(bIsDying, 3))
		{
			OldAimingActor = 0;
			return;
		}

		if (GetAimKey())
			entityx = OldAimingActor;
		else
			OldAimingActor = NULL;
	}
}


bool MainAddress() {

	if (!IsValid(OFFSET_UWORLD)) return false;

	Uworld = read<DWORD_PTR>(OFFSET_UWORLD);
	if (!Uworld) return false;

	DWORD_PTR Gameinstance = read<DWORD_PTR>(Uworld + Offsets::Engine::World::OwningGameInstance);
	if (!Gameinstance) return false;

	DWORD_PTR LocalPlayers = read<DWORD_PTR>(Gameinstance + Offsets::Engine::GameInstance::LocalPlayers);
	if (!LocalPlayers) return false;

	Localplayer = read<DWORD_PTR>(LocalPlayers);
	if (!Localplayer) return false;

	PlayerController = read<DWORD_PTR>(Localplayer + Offsets::Engine::Player::PlayerController);
	if (!PlayerController) return false;

	LocalPawn = read<uint64_t>(PlayerController + Offsets::Engine::PlayerController::AcknowledgedPawn);
	if (!LocalPawn)
	{
		return false;
	}

	LocalWeapon = read<uint64_t>(LocalPawn + Offsets::FortniteGame::FortPawn::CurrentWeapon);
	if (!LocalWeapon) return false;

	PlayerCameraManager = read<uint64_t>(PlayerController + Offsets::Engine::PlayerController::PlayerCameraManager);
	if (!PlayerCameraManager) return false;

	Rootcomp = read<uint64_t>(LocalPawn + Offsets::Engine::Actor::RootComponent);
	if (!Rootcomp) return false;

	PawnMesh = read<uint64_t>(LocalPawn + Offsets::Engine::Character::Mesh);
	if (!PawnMesh) return false;

	Levels = read<DWORD_PTR>(Uworld + Offsets::Engine::World::Levels);
	if (!Levels) return false;

	LevelsCount = read<DWORD_PTR>(Uworld + Offsets::Engine::World::Levels + 0x8);
	if (!LevelsCount) return false;

	Ulevel = read<DWORD_PTR>(Uworld + Offsets::Engine::World::PersistentLevel);
	if (!Ulevel) return false;

	AActors = read<DWORD_PTR>(Ulevel + 0x98);
	actor_count = *(int*)(Ulevel + 0xA0);

	if (!AActors || !actor_count) return false;
	if (PlayerController && read<uintptr_t>(PlayerController))
	{
		auto SetIgnoreLookInput = (*(void(__fastcall**)(uint64_t, char bNewLookInput))(*(uint64_t*)PlayerController + 0x728));
		spoof_call(game_rbx_jmp, SetIgnoreLookInput, PlayerController, (char)0);

		if (bHidden && !bDone)
		{
			auto SetIgnoreLookInput = (*(void(__fastcall**)(uint64_t, char bNewLookInput))(*(uint64_t*)PlayerController + 0x728));
			spoof_call(game_rbx_jmp, SetIgnoreLookInput, PlayerController, (char)0);
			bDone = true;
		}
	}
	uint8_t bIsDying = *(uint8_t*)(LocalPawn + Offsets::FortniteGame::FortPawn::bIsDying);
	if (BIT_CHECK(bIsDying, 3))
	{
		LocalPawn = NULL;
		return false;
	}
	return InstallCameraHook(PlayerCameraManager);
}

void Crosshair(float X, float Y)
{
	nk_color color = { 255,255,255,255 };

	Draw->DrawLine((X / 2) - CrosshairSize, Y / 2, (X / 2) + CrosshairSize, Y / 2, 1, color);
	Draw->DrawLine(X / 2, (Y / 2) - CrosshairSize, X / 2, (Y / 2) + CrosshairSize, 1, color);
}

void DrawCursor()
{
	POINT Mouse;
	spoof_call(game_rbx_jmp, GetCursorPos, &Mouse);
	spoof_call(game_rbx_jmp, ScreenToClient, GameWindow, &Mouse);

	nk_color white = { 255,255,255,255 };
	nk_color black = { 0,0,0,255 };
	Draw->DrawLine(Mouse.x - 11, Mouse.y, Mouse.x + 11, Mouse.y, 3, black);
	Draw->DrawLine(Mouse.x, Mouse.y - 11, Mouse.x, Mouse.y + 11, 3, black);

	Draw->DrawLine(Mouse.x - 10, Mouse.y, Mouse.x + 10, Mouse.y, 1, white);
	Draw->DrawLine(Mouse.x, Mouse.y - 10, Mouse.x, Mouse.y + 10, 1, white);
}

void RandomiseHit() {
	AimBoneInt == 0;
	AimBoneInt = 0;
	srand((unsigned)time(0));

	int ran = 1 + (rand() % 15);

	switch (ran) {
	case 1:
		//Aimbone head
		AimBoneInt == 66;
		AimBoneInt = 66;
		break;
	case 2:
		//aimbone body
		AimBoneInt == 7;
		AimBoneInt = 7;
		break;
	case 3:
		//aimbone legs
		AimBoneInt == 67;
		AimBoneInt = 67;
		break;
	case 4:
		//aimbone right hand
		AimBoneInt == 85;
		AimBoneInt = 85;
		break;
	case 5:
		//aimbone left hand
		AimBoneInt == 86;
		AimBoneInt = 86;
		break;
	case 6:
		//aimbone left KNEE 
		AimBoneInt == 68;
		AimBoneInt = 68;
		break;
	case 7:
		//aimbone left SHIN 
		AimBoneInt == 70;
		AimBoneInt = 70;
		break;
	case 8:
		//aimbone left THIGH 
		AimBoneInt == 73;
		AimBoneInt = 73;
		break;

	case 9:
		//aimbone left THUMB 
		AimBoneInt == 58;
		AimBoneInt = 58;
		break;
	case 10:
		//aimbone RIGHT FOOT 
		AimBoneInt == 83;
		AimBoneInt = 83;
		break;
	case 11:
		//aimbone LEFT FOOT 
		AimBoneInt == 82;
		AimBoneInt = 82;
		break;
	}
}