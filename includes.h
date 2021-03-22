#pragma once
#define _CRT_SECURE_NO_WARNINGS
#define COBJMACROS
#define WIN32_LEAN_AND_MEAN
#define MAX_VERTEX_BUFFER 512 * 1024
#define MAX_INDEX_BUFFER 128 * 1024
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT
#define NK_D3D11_IMPLEMENTATION
#define NK_IMPLEMENTATION
#define skCrypt(str) _xor_(str).c_str()
#define BIT_CHECK(a,b) (!!((a) & (1ULL<<(b))))

#include <windows.h>
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <time.h>
#include "xorstr.hpp"
#include <math.h>
#include "Structs.h"
#include <D3D11.h>
#include <d3dx11.h>
#include "style.c"
#include <d3dx10.h>
#include <codecvt>
#include <Psapi.h>
#include <list>
#include "FW1FontWrapper.h"
#include "spoof_call.h"
#include "lazyimporter.h"
#include "memory.h"
#include "Nuklear/demo/d3d11/nuklear_d3d11.h"
#include "Draw.hpp"
#include "Offsets.h"
#include "MainOptions.h"
#include "FortUpdater.h"
#include "Helper.h"
#include "CheatHelper.h"
#include "Exploits.h"
#include "imgui/imgui.h"

#pragma comment(lib, "Psapi.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment (lib, "d3dx10.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "FW1FontWrapper.lib")

std::string Warn(std::string warningText) {
    Spoofer::SpoofCall(MessageBoxA, (HWND)0, (LPCSTR)(warningText.c_str()), (LPCSTR)skCrypt("Warning"), (UINT)0);
}

namespace Explotsw {
	typedef struct {
		float Pitch;
		float Yaw;
		float Roll;
	} FRotator2323;
	typedef struct {
		float X, Y, Z;
	} FVector2;
	typedef struct {
		FVector2 Location;
		FRotator2323 Rotation;
		float FOV;
		float OrthoWidth;
		float OrthoNearClipPlane;
		float OrthoFarClipPlane;
		float AspectRatio;
	} FMinimalViewInfo2323;

	struct {
		FMinimalViewInfo2323 Info;
		float ProjectionMatrix[4][4];
	} view = { 0 };

	FMinimalViewInfo2323& GetViewInfo() {
		return view.Info;
	}

	FVector2* GetPawnRootLocation(PVOID pawn) {
		auto root = ReadPointer(pawn, Offsets::Engine::Actor::RootComponent);
		if (!root) {
			return nullptr;
		}

		return reinterpret_cast<FVector2*>(reinterpret_cast<PBYTE>(root) + Offsets::Engine::SceneComponent::RelativeLocation);
	}
}