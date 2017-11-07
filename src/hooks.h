#pragma once

#include "render.h"

typedef HRESULT(__stdcall *tD3D11Present)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);

namespace Hooks
{
	HRESULT PresentHooked(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT flags);

	DWORD WINAPI Initialize(LPVOID lpParam);
}