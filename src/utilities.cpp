#include "utilities.h"

#include "Psapi.h"
#include <stdio.h>

#include "globals.h"

#define INRANGE(x,a,b)    (x >= a && x <= b) 
#define getBits( x )    (INRANGE((x&(~0x20)),'A','F') ? ((x&(~0x20)) - 'A' + 0xA) : (INRANGE(x,'0','9') ? x - '0' : 0))
#define getByte( x )    (getBits(x[0]) << 4 | getBits(x[1]))

namespace Utilities
{
	HMODULE Tsl = 0;
	MODULEINFO TslInfo;

	uintptr_t GetBase() { return (uintptr_t)Tsl; }
	uintptr_t GetEnd() { return GetBase() + TslInfo.SizeOfImage; }

	uintptr_t FindPattern(const char* pattern)
	{
		char* pat = const_cast<char*>(pattern);
		uintptr_t firstMatch = 0;
		uintptr_t rangeEnd = GetEnd();

		for (auto pCur = GetBase(); pCur < rangeEnd; pCur++)
		{
			if (!*pat)
				return firstMatch;

			if (*(BYTE*)pat == '\?' || *(BYTE*)pCur == getByte(pat))
			{
				if (!firstMatch)
					firstMatch = pCur;

				if (!pat[2])
					return firstMatch;

				if (*(BYTE*)pat == '\?')
					pat += 2;
				else
					pat += 3;
			}
			else
			{
				pat = const_cast<char*>(pattern);
				firstMatch = 0;
			}
		}

		return 0;
	}

	void LoadModule()
	{
		HMODULE hMods[1024];
		DWORD cb;

		if (EnumProcessModulesEx(curProcess, hMods, sizeof(hMods), &cb, LIST_MODULES_64BIT))
		{
			Tsl = hMods[0];
			GetModuleInformation(curProcess, Tsl, &TslInfo, sizeof(MODULEINFO));
		}
	}
}