#include "hooks.h"

#include "offsys.h"
#include "esp.h"
#include "globals.h"
#include "aimbot.h"
#include "vmt.h"
#include "engine.h"

namespace Hooks
{
	tD3D11Present Present = nullptr;

	ID3D11Device *pDevice = nullptr;
	ID3D11DeviceContext *pContext = nullptr;
	IDXGISwapChain* pSwapChain = nullptr;

	DWORD_PTR* pSwapChainVtable = nullptr;

	ID3D11Texture2D* RenderTargetTexture = nullptr;
	ID3D11RenderTargetView* RenderTargetView = nullptr;
}

Render* G::Draw = nullptr;
std::unique_ptr<OffsetSystem> offsys = nullptr;

HRESULT Hooks::PresentHooked(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT flags)
{
	static bool bRunning = false;
	static bool bOnce = true;

	if (bOnce)
	{
		if (SUCCEEDED(pSwapChain->GetDevice(__uuidof(ID3D11Device), (void **)&pDevice)))
		{
			pSwapChain->GetDevice(__uuidof(pDevice), (void**)&pDevice);
			pDevice->GetImmediateContext(&pContext);
		}

		G::Draw = new Render(pDevice);
		offsys = std::make_unique<OffsetSystem>();

		if (SUCCEEDED(pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&RenderTargetTexture)))
		{
			pDevice->CreateRenderTargetView(RenderTargetTexture, NULL, &RenderTargetView);
			RenderTargetTexture->Release();
		}

		bOnce = false;
	}

	if (!bRunning && GetAsyncKeyState(VK_INSERT) & 0x8000)
	{
		bRunning = true;

		Classes::FName::GNames = *reinterpret_cast<Classes::TNameEntryArray**>(offsys->GNamesAddress);
		Classes::UObject::GObjects = reinterpret_cast<Classes::FUObjectArray*>(offsys->GObjsAddress);
		G::Lib::Sys = reinterpret_cast<Classes::UKismetSystemLibrary*>(Classes::UKismetSystemLibrary::StaticClass());
		G::UWorld = *reinterpret_cast<Classes::UWorld**>(offsys->UWorldAddress);
		G::PWorld = G::UWorld->OwningGameInstance->LocalPlayers[0]->ViewportClient->World;
	}

	// begin
	pContext->OMSetRenderTargets(1, &RenderTargetView, NULL);
	G::Draw->Begin();

	if (bRunning)
	{
		Aimbot::BeginFrame();

		G::localPos = G::PWorld->OwningGameInstance->LocalPlayers[0]->PlayerController->PlayerCameraManager->CameraCache.POV.Location;
		G::localRot = G::PWorld->OwningGameInstance->LocalPlayers[0]->PlayerController->PlayerCameraManager->CameraCache.POV.Rotation;
		G::localChar = reinterpret_cast<Classes::ATslCharacter*>(G::UWorld->OwningGameInstance->LocalPlayers[0]->PlayerController->AcknowledgedPawn);
		G::PlayerController = G::PWorld->OwningGameInstance->LocalPlayers[0]->PlayerController;
		//G::localWeapon = G::localChar ? G::localChar->GetWeapon() : nullptr;

		/*if (G::localChar && G::localChar->WeaponProcessor)
		{
			uint32_t weaponIndex = G::localChar->WeaponProcessor->CurrentWeaponIndex;
			if (0 <= weaponIndex && 2 >= weaponIndex && G::localChar->WeaponProcessor->EquippedWeapons.IsValidIndex(weaponIndex))
			{
				G::Draw->Text(10, 45, L"weap valid", DirectX::Colors::Magenta);
				G::localWeapon = G::localChar->WeaponProcessor->EquippedWeapons[weaponIndex];
			}
		}*/
		// crashing when jump out of plane

		for (size_t i = 0; i < G::PWorld->PersistentLevel->AActors.Num(); i++)
		{
			if (!G::PWorld->PersistentLevel->AActors.IsValidIndex(i))
				continue;

			Classes::AActor* actor = G::PWorld->PersistentLevel->AActors[i];

			if (!actor)
				continue;

			if ((uintptr_t)actor == (uintptr_t)G::PlayerController->AcknowledgedPawn)
				continue;

			if (actor->IsA(Classes::ATslCharacter::StaticClass()))
			{
				ESP::DrawPlayer(reinterpret_cast<Classes::ATslCharacter*>(actor));
				Aimbot::EvaluateTarget(reinterpret_cast<Classes::ATslCharacter*>(actor));
			}
			else if (actor->IsA(Classes::ATslWheeledVehicle::StaticClass()))
			{
				ESP::DrawWheeledVehicle(reinterpret_cast<Classes::ATslWheeledVehicle*>(actor));
			}
			else if (actor->IsA(Classes::ADroppedItemGroup::StaticClass()))
			{
				ESP::DrawLootGroup(actor);
			}
			else if (actor->IsA(Classes::ACarePackageItem::StaticClass()))
			{
				ESP::DrawAirdrop(actor);
			}
			else if (actor->IsA(Classes::ADeathDropItemPackage_C::StaticClass()))
			{
				ESP::DrawDeathDrop(actor);
			}
		}

		if ((GetAsyncKeyState(VK_XBUTTON1) & 0x8000) || (GetAsyncKeyState(VK_XBUTTON2) & 0x8000) || (GetAsyncKeyState(VK_RBUTTON) & 0x8000))
			Aimbot::AimToTarget();
	}

	G::Draw->Draw();

	// end
	G::Draw->End();

	return Present(pSwapChain, SyncInterval, flags);
}

LRESULT CALLBACK DXGIMsgProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

#pragma warning(default:4996)
DWORD WINAPI Hooks::Initialize(LPVOID lpParam)
{
	//AllocConsole();
	//freopen("CONOUT$", "w", stdout);

	//printf("Initializing Hook\n");

	//Sleep(1000);

	HMODULE hDXGIDLL = 0;
	do
	{
		hDXGIDLL = GetModuleHandle(rxorc("dxgi.dll"));
		Sleep(100);
	} while (!hDXGIDLL);
	Sleep(100);

	IDXGISwapChain* pSwapChain;

	WNDCLASSEXA wc = { sizeof(WNDCLASSEX), CS_CLASSDC, DXGIMsgProc, 0L, 0L, GetModuleHandleA(NULL), NULL, NULL, NULL, NULL, " ", NULL };
	RegisterClassExA(&wc);

	D3D_FEATURE_LEVEL requestedLevels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1 };
	D3D_FEATURE_LEVEL obtainedLevel;

	HWND hWnd = CreateWindowA(" ", NULL, WS_OVERLAPPEDWINDOW, 5, 5, 7, 8, NULL, NULL, wc.hInstance, NULL);

	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(scd));

	scd.BufferCount = 1;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.OutputWindow = hWnd;
	scd.BufferDesc.Width = 1;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	scd.OutputWindow = hWnd;
	scd.SampleDesc.Count = 1;
	scd.Windowed = true;
	scd.BufferDesc.Height = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.RefreshRate.Numerator = 0;

#ifdef _DEBUG
	// You have to plug thiss into D3DCreate if you want to use the flag.
	UINT createFlags = D3D11_CREATE_DEVICE_DEBUG;
#endif

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		requestedLevels,
		sizeof(requestedLevels) / sizeof(D3D_FEATURE_LEVEL),
		D3D11_SDK_VERSION,
		&scd,
		&pSwapChain,
		&pDevice,
		&obtainedLevel,
		&pContext);

	if (FAILED(hr))
	{
		//MsgBoxA
		return 0xC0000005;
	}

	pSwapChainVtable = (DWORD_PTR*)pSwapChain;
	pSwapChainVtable = (DWORD_PTR*)pSwapChainVtable[0];

	Hooks::Present = HookVirtual((void*)&pSwapChainVtable[8], PresentHooked);

	return 0;
}
#pragma warning(default:4996)
