#pragma once

#include <memory>
#include <Windows.h>

namespace Utilities
{
	extern HANDLE curProcess;
	extern HINSTANCE hAppInstance;

	void LoadModule();

	uintptr_t GetBase();
	uintptr_t GetEnd();

	uintptr_t FindPattern(const char* pattern);
}