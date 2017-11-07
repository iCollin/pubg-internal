#pragma once

// simple as fuck
template <typename T> T HookVirtual(void* func, T Hook)
{
	DWORD oldProtect = 0;
	VirtualProtectEx(Utilities::curProcess, func, 4, PAGE_EXECUTE_READWRITE, &oldProtect);

	T original = *(T*)func;
	*(T*)func = Hook;

	VirtualProtectEx(Utilities::curProcess, func, 4, oldProtect, &oldProtect);

	return original;
}