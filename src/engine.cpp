#include "engine.h"

#include "globals.h"
#include "offsys.h"

Classes::FVector Engine::GetBonePosition(Classes::ATslCharacter* p, int boneIndex)
{
	Classes::FMatrix m;
	reinterpret_cast<Classes::FMatrix* (__fastcall*)(Classes::USkeletalMeshComponent*, Classes::FMatrix*, int)>(offsys->GetBonePos)(p->Mesh, &m, boneIndex);
	return m.WPlane;
}

bool Engine::GetBoneScreenPosition(Classes::ATslCharacter* p, Classes::FVector2D* result, int boneIndex)
{
	return W2S(GetBonePosition(p, boneIndex), result);
}

bool Engine::W2S(Classes::FVector worldPos, Classes::FVector2D* screenPos)
{
	if (!reinterpret_cast<char(__fastcall*)(Classes::APlayerController*, Classes::FVector, Classes::FVector2D *, char)>(offsys->WorldToScreen)(G::PlayerController, worldPos, screenPos, 0)) // return G::PlayerController->ProjectWorldLocationToScreen(worldPos, true, screenPos);
		return false;

	return screenPos->X > 0.5f && screenPos->Y > 0.5f && screenPos->X < G::Draw->mViewport.Width && screenPos->Y < G::Draw->mViewport.Height;
}

bool Engine::IsVisible(Classes::AActor* p, Classes::FVector viewPoint)
{
	return G::PlayerController->LineOfSightTo(p, viewPoint, false);
}