#pragma once
#include "includes.h"
#include "Major.h"

void DrawMenu()
{
	bg.r = 0.10f, bg.g = 0.18f, bg.b = 0.24f, bg.a = 1.0f;

	if (o_getasynckeystate((DWORD)VK_INSERT) == -32767) ShowMenu = !ShowMenu;

	if (ShowMenu)
	{
		set_style(g_pNkContext, THEME_PURPLE);
		bHidden = false;
		bDone = false;
		if (nk_begin(g_pNkContext, skCrypt("Overgay BUILD : " __TIME__ " " __DATE__), nk_rect(50, 50, 480, 600), NK_WINDOW_BORDER | NK_WINDOW_MOVABLE
			| NK_WINDOW_TITLE | NK_WINDOW_NO_SCROLLBAR | NK_WINDOW_SCALABLE | NK_WINDOW_MINIMIZABLE))
		{
			CurrentTab = Tabs::Aimbot;

			nk_layout_row_dynamic(g_pNkContext, 600, 1);
			if (nk_group_begin(g_pNkContext, skCrypt("FeaturesTab"), NK_WINDOW_BORDER | NK_WINDOW_NO_SCROLLBAR)) {

				static int property = 20;
				static int switchTabs = 4;

				nk_layout_row_static(g_pNkContext, 30, 100, 30);
				if (nk_button_label(g_pNkContext, skCrypt("Aimbot")))
					switchTabs = 1;
				if (nk_button_label(g_pNkContext, skCrypt("Visuals")))
					switchTabs = 2;
				if (nk_button_label(g_pNkContext, skCrypt("Misc")))
					switchTabs = 3;
				if (nk_button_label(g_pNkContext, skCrypt("Credits")))
					switchTabs = 4;
				nk_layout_row_dynamic(g_pNkContext, 30, 2);

				switch (switchTabs) {
				case 1:
					nk_layout_row_dynamic(g_pNkContext, 23, 1);
					nk_label(g_pNkContext, skCrypt("Aimbot Type"), NK_TEXT_LEFT);
					nk_checkbox_label(g_pNkContext, skCrypt("Memory Aimbot"), &bAimbot);
					nk_checkbox_label(g_pNkContext, skCrypt("Silent Aimbot"), &bSilent);
					nk_label(g_pNkContext, skCrypt("Aimbot FOV"), NK_TEXT_LEFT);
					nk_checkbox_label(g_pNkContext, skCrypt("Aimbot FOV Circle"), &bDrawCircle);
					nk_label(g_pNkContext, skCrypt("Aimbot Bone"), NK_TEXT_LEFT);
					nk_checkbox_label(g_pNkContext, skCrypt("Random Aim Bone"), &bRandomHit);
					if (nk_option_label(g_pNkContext, skCrypt("Head"), AimBoneInt == 66)) AimBoneInt = 66;
					if (nk_option_label(g_pNkContext, skCrypt("Pelvis"), AimBoneInt == 7)) AimBoneInt = 7;
					if (nk_option_label(g_pNkContext, skCrypt("Leg"), AimBoneInt == 67)) AimBoneInt = 67;
					// AIMBOT KEY 0-3
					nk_label(g_pNkContext, skCrypt("Aimbot Keybind"), NK_TEXT_LEFT);
					if (nk_option_label(g_pNkContext, skCrypt("LMB"), AimKey == 0)) AimKey = 0;
					if (nk_option_label(g_pNkContext, skCrypt("RMB"), AimKey == 1)) AimKey = 1;
					// SMOOTH - DISTANCE SLIDER
					nk_label(g_pNkContext, skCrypt("Aimbot Configuration"), NK_TEXT_LEFT);
					nk_property_int(g_pNkContext, skCrypt("Aim Distance:"), 20, &AimDistance, 1000, 1, 1);
					nk_property_int(g_pNkContext, skCrypt("Aim Smoothness:"), 0, &AimSmooth, 30, 1, 1);
					nk_property_int(g_pNkContext, skCrypt("Aim FOV:"), 1, &AimFov, 1337, 1, 1);
				}

				switch (switchTabs) {
				case 2:
					nk_layout_row_dynamic(g_pNkContext, 23, 1);


					// ESP 
					nk_label(g_pNkContext, skCrypt("ESP Entity"), NK_TEXT_LEFT);
					nk_checkbox_label(g_pNkContext, skCrypt("Enemies"), &bShowEnemies);
					nk_checkbox_label(g_pNkContext, skCrypt("Henchmen"), &bShowBoss);
					nk_checkbox_label(g_pNkContext, skCrypt("Teammates"), &bShowFriends);
					nk_checkbox_label(g_pNkContext, skCrypt("Bots"), &bShowBot);
					//esp type
					nk_label(g_pNkContext, skCrypt("ESP Type"), NK_TEXT_LEFT);
					nk_checkbox_label(g_pNkContext, skCrypt("Box"), &b3DBox);
					nk_checkbox_label(g_pNkContext, skCrypt("Skeleton"), &bSkeleton);
					nk_checkbox_label(g_pNkContext, skCrypt("Line ESP"), &bSnapLine);
					//esp extra
					nk_label(g_pNkContext, skCrypt("ESP Misc"), NK_TEXT_LEFT);
					nk_checkbox_label(g_pNkContext, skCrypt("Radar"), &bESP2D);
					nk_checkbox_label(g_pNkContext, skCrypt("Visible Check"), &bVisible);
				}

				switch (switchTabs) {
				case 3:
					nk_layout_row_dynamic(g_pNkContext, 23, 1);

					nk_label(g_pNkContext, skCrypt("Exploits"), NK_TEXT_LEFT);
					//nk_layout_row_dynamic(g_pNkContext, 23, 3);
					nk_checkbox_label(g_pNkContext, skCrypt("Instant Reload"), &bNoReload);
					nk_checkbox_label(g_pNkContext, skCrypt("No Spread"), &bNoSpread);
					nk_checkbox_label(g_pNkContext, skCrypt("No Recoil"), &bNoRecoil);
					nk_checkbox_label(g_pNkContext, skCrypt("Rapid Fire"), &bRapidFire);
					nk_checkbox_label(g_pNkContext, skCrypt("Bullet TP"), &bBulletTP);
					nk_checkbox_label(g_pNkContext, skCrypt("Air Struck [Left Shift + Space]"), &bAirStruck);
					nk_label(g_pNkContext, skCrypt("Misc"), NK_TEXT_LEFT);
					nk_checkbox_label(g_pNkContext, skCrypt("Crosshair"), &bCrosshair);
					nk_property_int(g_pNkContext, skCrypt("Crosshair Size:"), 1, &CrosshairSize, 540, 1, 1);
					nk_property_int(g_pNkContext, skCrypt("FOV Changer:"), 20, &FOVMoment, 140, 1, 1);
				}
				switch (switchTabs) {
				case 4:
					nk_layout_row_dynamic(g_pNkContext, 23, 1);

					nk_label(g_pNkContext, skCrypt("-Official Links-"), NK_TEXT_LEFT);
					nk_label(g_pNkContext, skCrypt("Discord - https://discord.gg/mnCJDPU"), NK_TEXT_LEFT);
					nk_label(g_pNkContext, skCrypt("-Developers-"), NK_TEXT_LEFT);
					nk_label(g_pNkContext, skCrypt("Kesh"), NK_TEXT_LEFT);
					nk_label(g_pNkContext, skCrypt("CL"), NK_TEXT_LEFT);
				}
				nk_group_end(g_pNkContext);
			}

		}
		nk_end(g_pNkContext);
	}
	else
	{
		bHidden = true;

		if (nk_begin(g_pNkContext, skCrypt(" "), nk_rect(1, 1, 1, 1), NK_WINDOW_MINIMIZED))
		{
		}

		nk_end(g_pNkContext);
	}
}

HRESULT __stdcall hk_present(IDXGISwapChain* pthis, UINT sync_interval, UINT flags)
{
	ID3D11Texture2D* pRenderTargetTexture;
	g_pSwapChain = nullptr;

	g_pSwapChain = pthis;

	if (firstTime) {

		pthis->GetDevice(__uuidof(ID3D11Device), (void**)&uDevice);

		if (!uDevice) return false;

		uDevice->GetImmediateContext(&m_pContext);

		if (!m_pContext) return false;

		g_pNkContext = nk_d3d11_init((ID3D11Device*)uDevice, (int)spoof_call(game_rbx_jmp, GetSystemMetrics, SM_CXSCREEN), (int)spoof_call(game_rbx_jmp, GetSystemMetrics, SM_CYSCREEN), (unsigned int)MAX_VERTEX_BUFFER, (unsigned int)MAX_INDEX_BUFFER);
		{
			nk_d3d11_font_stash_begin(&Draw->atlas);
			nk_d3d11_font_stash_end();

			Draw->CopyContext = g_pNkContext;
		}

		if (SUCCEEDED(pthis->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pRenderTargetTexture)))
		{
			uDevice->CreateRenderTargetView(pRenderTargetTexture, NULL, &g_pRenderTargetView);
			pRenderTargetTexture->Release();
			uDevice->Release();
		}

		firstTime = false;
	}

	if (!g_pRenderTargetView)
	{
		pthis->GetDevice(__uuidof(ID3D11Device), (void**)&uDevice);

		if (!uDevice) return false;

		uDevice->GetImmediateContext(&m_pContext);

		if (!m_pContext) return false;

		if (SUCCEEDED(pthis->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pRenderTargetTexture)))
		{
			uDevice->CreateRenderTargetView(pRenderTargetTexture, NULL, &g_pRenderTargetView);
			pRenderTargetTexture->Release();
			uDevice->Release();
		}
	}

	if (g_pRenderTargetView)
	{
		m_pContext->OMSetRenderTargets(1, &g_pRenderTargetView, NULL);

		nk_input_end(g_pNkContext);
		DrawMenu();

		nk_input_begin(g_pNkContext);

		nk_d3d11_render(m_pContext, NK_ANTI_ALIASING_ON);
	}
	return SwapChain(pthis, sync_interval, flags);
}

typedef uintptr_t(__fastcall* LFAT)(uintptr_t* a1, unsigned int a2, char a3);
LFAT oValidateLastFire = NULL;
__int64 hkValidateLastFire(uintptr_t* a1, unsigned int a2, char a3)
{
	__int64 ret = spoof_call(game_rbx_jmp, oValidateLastFire, a1, a2, a3);

	if (a1 && LocalWeapon) return 1;
	else return ret;
}

LRESULT CALLBACK hkWndProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	switch (msg)
	{
	case WM_SIZE:
		UINT width = LOWORD(lparam);
		UINT height = HIWORD(lparam);

		ID3D11Texture2D* back_buffer;
		D3D11_RENDER_TARGET_VIEW_DESC desc;
		HRESULT hr;

		if (g_pRenderTargetView)
		{
			g_pRenderTargetView->Release();
			g_pRenderTargetView = nullptr;
		}

		if (m_pContext)
		{
			m_pContext->OMSetRenderTargets(0, NULL, NULL);
			if (g_pSwapChain)
				g_pSwapChain->ResizeBuffers(0, width, height, DXGI_FORMAT_UNKNOWN, 0);

			nk_d3d11_resize(m_pContext, (int)width, (int)height);
		}
		break;
	}

	nk_d3d11_handle_event(hwnd, msg, wparam, lparam);

	return CallWindowProcW(oWndProc, hwnd, msg, wparam, lparam);
}