#include "globals.h"

#define FNameCompareFn(name, fnames) bool name(Classes::FName other) { for (auto fn : fnames) { if (other == fn) return true; } return false; }

namespace G
{
	Classes::UWorld* UWorld;
	Classes::UWorld* PWorld;
	Classes::FVector localPos;
	Classes::FRotator localRot;
	Classes::ATslCharacter* localChar;
	Classes::ATslWeapon* localWeapon;
	Classes::APlayerController* PlayerController;
	
	namespace Lib
	{
		Classes::UKismetSystemLibrary* Sys;
	}
}