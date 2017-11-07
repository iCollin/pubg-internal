#pragma once

namespace Classes
{
	class ATslCharacter;
}

namespace Aimbot
{
	void BeginFrame();
	void EvaluateTarget(Classes::ATslCharacter* p);
	void AimToTarget();
}