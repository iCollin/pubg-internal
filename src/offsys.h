#pragma once

#include "utilities.h"

class OffsetSystem
{
public:
	uintptr_t GNamesAddress;
	uintptr_t GObjsAddress;
	uintptr_t UWorldAddress;
	uintptr_t GetBonePos;
	uintptr_t WorldToScreen;
	uintptr_t GetBoundingBox;

	uintptr_t Add3(const char* sig)
	{
		uintptr_t _sig = Utilities::FindPattern(sig);

		if (!_sig) return _sig;

		uintptr_t _off = *(uint32_t*)(_sig + 3);

		return _sig + _off + 7;
	}

	OffsetSystem()
	{
		Utilities::LoadModule();

		GNamesAddress = Add3(rxorc("48 89 1D ? ? ? ? 48 8B 5C 24 ? 48 83 C4 28 C3 48 8B 5C 24 ? 48 89 05 ? ? ? ? 48 83 C4 28 C3"));

		GObjsAddress = Add3(rxorc("48 8D 0D ? ? ? ? E8 ? ? ? ? E8 ? ? ? ? 48 8B D6"));

		UWorldAddress = Add3(rxorc("48 8B 1D ? ? ? ? 74 40"));

		GetBonePos = Utilities::FindPattern(rxorc("48 89 5C 24 ? 48 89 74 24 ? 57 48 81 EC ? ? ? ? F6 81"));

		WorldToScreen = Utilities::FindPattern(rxorc("48 89 5C 24 ? 48 89 74 24 ? 57 48 81 EC ? ? ? ? 41 0F B6 F9"));

		GetBoundingBox = Utilities::FindPattern(rxorc("48 8B C4 48 89 58 10 48 89 70 18 55 57 41 54 41 55"));
	}
};

extern std::unique_ptr<OffsetSystem> offsys;
