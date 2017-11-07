#include "aimbot.h"
#include "globals.h"
#include "SDK.hpp"

#include "engine.h"

#define M_PI		3.14159265358979323846
#define M_PI_F		((float)(M_PI))
#define DEG2RAD(x)  ( (float)(x) * (float)(M_PI_F / 180.f) )
#define RAD2DEG(x)  ( (float)(x) * (float)(180.f / M_PI_F) )

namespace Aimbot
{
	float bestFOV = 0.f;
	Classes::FRotator idealAngDelta;

	const float AimbotFOV = 15.f;

	void BeginFrame()
	{
		bestFOV = AimbotFOV;
		idealAngDelta = { 0,0,0 };
	}

	Classes::FRotator Clamp(Classes::FRotator r)
	{
		if (r.Yaw > 180.f)
			r.Yaw -= 360.f;
		else if (r.Yaw < -180.f)
			r.Yaw += 360.f;

		if (r.Pitch > 180.f)
			r.Pitch -= 360.f;
		else if (r.Pitch < -180.f)
			r.Pitch += 360.f;

		if (r.Pitch < -89.f)
			r.Pitch = -89.f;
		else if (r.Pitch > 89.f)
			r.Pitch = 89.f;

		r.Roll = 0.f;

		return r;
	}

	Classes::FRotator Vec2Rot(Classes::FVector vec)
	{
		Classes::FRotator rot;

		rot.Yaw = RAD2DEG(std::atan2f(vec.Y, vec.X));
		rot.Pitch = RAD2DEG(std::atan2f(vec.Z, std::sqrtf(vec.X * vec.X + vec.Y * vec.Y)));
		rot.Roll = 0.f;

		return rot;
	}

	Classes::FVector GetPlayerVelocity(Classes::ATslCharacter* p)
	{
		/*if (p->VehicleRiderComponent && p->VehicleRiderComponent->SeatIndex >= 0)
			if (p->VehicleRiderComponent->LastVehiclePawn)
				return p->VehicleRiderComponent->LastVehiclePawn->GetVelocity();*/

		if (p->RootComponent)
			return p->RootComponent->ComponentVelocity;

		return { 0.f, 0.f, 0.f };
	}

	void EvaluateTarget(Classes::ATslCharacter * p)
	{
		if (p->Team)
			return;

		if (!Engine::IsVisible(p))
			return;

		auto pos = Engine::GetBonePosition(p, 15);
		auto delta = (pos - G::localPos);
		auto dist = delta.Size();

		if (p->Health < 1.f)
			return;

		/*if (G::localWeapon && G::localWeapon->IsA(Classes::ATslWeapon_Trajectory::StaticClass()))
		{
			auto traj = reinterpret_cast<Classes::ATslWeapon_Trajectory*>(G::localWeapon)->TrajectoryConfig;
			float bulletspeed = traj.InitialSpeed * 100.f;

			float airtime = dist / bulletspeed;

			auto velocity = GetPlayerVelocity(p);
			if (velocity.X > 0.0001f)
				delta += velocity * airtime;
		}*/

		Classes::FRotator ideal = Clamp(Vec2Rot(delta));
		auto angDelta = Clamp(ideal - G::localRot);
		float angFOV = angDelta.Size();

		if (angFOV < bestFOV)
		{
			bestFOV = angFOV;
			idealAngDelta = angDelta;
		}
	}

	void SetViewAngles(Classes::FRotator ang)
	{
		G::localChar->Controller->ControlRotation = Clamp(ang);
	}

	void AimToTarget()
	{
		if (bestFOV >= AimbotFOV)
			return;

		// nosway (should be always on, not jsut when aimbotting)
		/*if (G::localWeapon)
		{
			auto sway = G::localWeapon->GetSwayModifier();
			G::localRot.Yaw -= sway.X;
			G::localRot.Pitch -= sway.Y;
		}*/

		SetViewAngles(G::localRot + idealAngDelta * 0.4f);
	}
}