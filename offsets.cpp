
#include "stdafx.h"
#include "Helper.h"

namespace Offsets {
	PVOID* uWorld = 0;
	PVOID LaunchCharacter = 0;
	uintptr_t fnGetBounds = 0;
	uintptr_t fnLineOfSightTo = 0;
	uintptr_t fnGetBoneMatrix = 0;
	uintptr_t nObjectsCount = 0;
	uintptr_t uObjects = 0;
	uintptr_t fnGetWeaponStats = 0;

	namespace Engine {
		namespace World {
			DWORD OwningGameInstance = 0x190;
			DWORD Levels = 0x148;
			DWORD PersistentLevel = 0x30;
		}
		namespace SceneComponent {
			DWORD RelativeLocation = 0x11C;
			DWORD ComponentVelocity = 0x140;
		}

		namespace Level {
			DWORD AActors = 0x98;
		}

		namespace GameInstance {
			DWORD LocalPlayers = 0x38;
		}

		namespace Player {
			DWORD PlayerController = 0x30;
		}

		namespace Controller {
			DWORD ControlRotation = 0x288;
			DWORD RemoteViewPitch = 0x232;
			PVOID SetControlRotation = 0;
			PVOID SetActorHiddenInGame = 0;
		}

		namespace PlayerController {
			DWORD AcknowledgedPawn = 0x2A0;
			DWORD PlayerCameraManager = 0x2B8;
		}

		namespace Pawn {
			DWORD PlayerState = 0x240;
		}

		namespace PlayerState {
			PVOID GetPlayerName = 0;
		}

		namespace Actor {
			PVOID SetActorHiddenInGame = 0;
			DWORD CustomTimeDilation = 0x98;
			DWORD RootComponent = 0x130;
		}

		namespace Character {
			DWORD Mesh = 0x280;
		}

		namespace StaticMeshComponent {
			DWORD ComponentToWorld = 0x1C0;
			DWORD StaticMesh = 0x468;
		}

		namespace SkinnedMeshComponent {
			DWORD CachedWorldSpaceBounds = 0x600;
		}
	}

	namespace FortniteGame {
		namespace FortPawn {
			DWORD bIsDBNO = 0x552;
			DWORD bIsDying = 0x538;
			DWORD CurrentWeapon = 0x5A0;
		}
		namespace FortMeatballVehicle {
			DWORD BoostMinPushForce = 0x0478;
			DWORD BoostTopSpeedForceMultiplier = 0x047C;
			DWORD BoostTopSpeedMultiplier = 0x0480;
			DWORD LandTopSpeedMultiplier = 0x0488;
			DWORD LandPushForceMultiplier = 0x048C;
			DWORD BoostSteeringMultiplier = 0x04CC;
			DWORD LandSteeringMultiplier = 0x04D4;
			DWORD LandMinSpeedSteeringAngle = 0x04D8;
			DWORD LandMaxSpeedSteeringAngle = 0x04DC;
		}
		namespace FortPickup {
			DWORD PrimaryPickupItemEntry = 0x298;
		}

		namespace FortItemEntry {
			DWORD ItemDefinition = 0x18;
		}

		namespace FortItemDefinition {
			DWORD DisplayName = 0x70;
			DWORD Tier = 0x54;
		}

		namespace FortPlayerStateAthena {
			DWORD TeamIndex = 0xE68;
		}

		namespace FortWeapon {
			DWORD LastFireTime = 0x8BC;
			DWORD LastFireTimeVerified = 0x8C0;
			DWORD WeaponData = 0x378;
			DWORD LastFireAbilityTime = 0xAB8;
		}

		namespace FortWeaponItemDefinition {
			DWORD WeaponStatHandle = 0x7B8;
		}

		namespace FortProjectileAthena {
			DWORD FireStartLoc = 0x868;
		}

		namespace FortBaseWeaponStats {
			//Reload
			DWORD ReloadTime = 0xFC;
			DWORD ReloadScale = 0x100;
			DWORD ChargeDownTime = 0x12C;
		}

		namespace FortRangedWeaponStats {
			//No Recoil
			DWORD RecoilHoriz = 0x200;
			DWORD RecoilVert = 0x1F0;
			DWORD RecoilDownsightsMultiplier = 0x21C;
			//No Spread
			DWORD Spread = 0x15C;
			DWORD SpreadDownsights = 0x160;
			DWORD StandingStillSpreadMultiplier = 0x164;
			DWORD AthenaJumpingFallingSpreadMultiplier = 0x16C;
			DWORD AthenaCrouchingSpreadMultiplier = 0x168;
			DWORD AthenaSprintingSpreadMultiplier = 0x170;
			DWORD MinSpeedForSpreadMultiplier = 0x174;
			DWORD MaxSpeedForSpreadMultiplier = 0x178;
		}

		namespace BuildingContainer {
			DWORD bAlreadySearched = 0xC71;
		}
	}

	namespace UI {
		namespace ItemCount {
			DWORD ItemDefinition = 0x18;
		}
	}

}