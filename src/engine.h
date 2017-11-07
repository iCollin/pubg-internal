#pragma once

#include "SDK.hpp"

namespace Engine
{
	Classes::FVector GetBonePosition(Classes::ATslCharacter* p, int boneIndex);
	bool GetBoneScreenPosition(Classes::ATslCharacter* p, Classes::FVector2D* result, int boneIndex);
	bool W2S(Classes::FVector worldPos, Classes::FVector2D* screenPos);
	bool IsVisible(Classes::AActor* p, Classes::FVector viewPoint = Classes::FVector());
}