#include "includes.h"

static ID3D11DeviceContext* m_pContext;
static ID3D11RenderTargetView* g_pRenderTargetView;
static IDXGISwapChain* g_pSwapChain;

static WNDPROC oWndProc;

typedef int (WINAPI* LPFN_MBA)(DWORD);
static LPFN_MBA o_getasynckeystate;

static HWND hWindow;
static HWND hWnd;

enum Keys {
	LButton,
	RButton,
	Alt_Button,
	Shift
};

enum Tabs {
	Aimbot,
	ESP,
	Objects,
	Misc
};

static int CurrentTab;

static bool ShowMenu = true;

static int AimBoneInt = 66;

static int AimBoneHead = 0;
static int AimBoneChest = 0;
static int AimBoneLeg = 0;

static int bAimbot = 0;
static int bAimLock = 0;
static int bNoSpreadAimbot = 0;
static int bRandomHit = 0;
static int bSilent = 0;
static int bStick = 0;
static int bBulletTP = 0;
static int bVisible = 0;
static int aimhelper = 0;
static int AimKey = Keys::RButton;
static int AimType = 0;
static int AimFov = 80;
static int AimDistance = 300;
static int AimSmooth = 0;
static int bSpinBot = 0;
static int bFirstPerson = 0;

static int bVis = 0;
static int bChest = 0;
static int bESP = 1;
static int bESP2D = 0;
static int bShowEnemies = 0;
static int bShowFriends = 0;
static int bShowSelf = 0;
static int bShowBot = 0;
static int bShowBoss = 0;
static int bSkeleton = 0;
static int b3DBox = 0;
static int b2DBox = 0;
static int BoxType = 0;
static int PlayerNames = 0;
static int bPlayerNames = 0;
static int bLaser = 0;
static int bSnapLine = 0;
static int bAimingLine = 0;

static int bObjectESP = 0;

static int bLootBoxAkaChest = 0;

static int bPickups = 0;
static int bBoatTP = 0;
static int bBoatSpeed = 0;
static int bBoatSpeedValue = 0;

static int bInstantReload = 0;
static int bFreeCamSpeed = 0;
static int bNoSpread = 0;
static int bNoReload = 0;
static int bNoRecoil = 0;

static int bRapidFire = 0;
static int bAirStruck = 0;
static int bSpeedHax = 0;
static int HoagieBooster = 0;
static int bCheckIfReal = 0;
static int bInvisblePlayer = 0;
static int bBoatFly = 0;
static int bDrawCircle = 0;
static int bkeybinds = 0;
static int FOVCircle = 150;
static int bCrosshair = 0;
static int CrosshairSize = 10;
static int FOVSlider = 0;
static int FOVMoment = 80;