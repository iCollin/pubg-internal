#pragma once

#include "SDK.hpp"

#include "render.h"

namespace G
{
	extern Classes::UWorld* UWorld;
	extern Classes::UWorld* PWorld;
	extern Classes::FVector localPos;
	extern Classes::FRotator localRot;
	extern Classes::ATslCharacter* localChar;
	extern Classes::ATslWeapon* localWeapon;
	extern Classes::APlayerController* PlayerController;

	extern class Render* Draw;

	namespace Lib
	{
		extern Classes::UKismetSystemLibrary* Sys;
	}
}