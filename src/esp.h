#pragma once

#include "globals.h"

namespace Classes
{
	class ATslCharacter;
	class ATslWheeledVehicle;
	class AActor;
	class UItem;
	struct FVector;
}

namespace ESP
{
	void DrawPlayerSkeleton(Classes::ATslCharacter* player);
	void DrawPlayer(Classes::ATslCharacter*);
	void DrawWheeledVehicle(Classes::ATslWheeledVehicle*);
	void DrawLootGroup(Classes::AActor*);
	void DrawLootItem(std::wstring name, Classes::FVector absolutePos);
	void DrawActorBoundingBox(Classes::AActor* actor, Color color);
	void DrawAirdrop(Classes::AActor* actor);
	void DrawDeathDrop(Classes::AActor* actor);
	void DrawActorOnRadar(Classes::FVector deltaFromLocal, Color col);

	std::wstring GetVehicleName(Classes::ATslWheeledVehicle* vehicle);
}

