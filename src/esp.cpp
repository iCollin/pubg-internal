#include "esp.h"

#include "engine.h"
#include "offsys.h"
#include "definitions.h"

// Credit daemonium @ unknowncheats.me
#define IS_BIT_SET(var,pos) ((var) & (1<<(pos)))

// Credit daemonium @ unknowncheats.me
class TBitArray
{
public:
	uint32_t AllocatorInstanceData[6];
	int32_t NumBits;
	int32_t MaxBits;
};

namespace ESP
{
	void DrawActorBoundingBox(Classes::AActor* actor, Color color)
	{
		Classes::FVector bbOrigin;
		Classes::FVector bbExtends;
		G::Lib::Sys->STATIC_GetActorBounds(actor, &bbOrigin, &bbExtends);

		G::Draw->Box3D(bbOrigin, bbExtends, color);
	}

	void DrawAirdrop(Classes::AActor * actor)
	{
		ESP::DrawActorBoundingBox(actor, DirectX::Colors::Red);

		if (!actor->RootComponent)
			return;

		auto loc = actor->RootComponent->Location;

		Classes::FVector2D sOrigin;
		if (!Engine::W2S(loc, &sOrigin))
			return;

		//float dist = (loc - G::localPos).Size() / 100.f;

		//auto temp = std::wstring(L"Airdrop: " + std::to_wstring((int)dist) + L"m");

		auto itempackage = reinterpret_cast<Classes::AItemPackage*>(actor);
		for (size_t i = 0; i < itempackage->Items.Num(); i++)
		{
			if (!itempackage->Items.IsValidIndex(i))
				continue;

			auto item = itempackage->Items[i];

			if (!item) continue;

			std::wstring name = item->ItemName.Get();

			if (name == L"AWM" || name == L"Mk14")
				G::Draw->Plus(sOrigin.X, sOrigin.Y, 10.f, DirectX::Colors::OrangeRed);

			//temp += L"\n";
			//temp += item->ItemName.Get();
		}

		//G::Draw->Text(sOrigin.X, sOrigin.Y, temp, DirectX::Colors::Red, 0, 8.f);
	}

	void DrawDeathDrop(Classes::AActor * actor)
	{
		ESP::DrawActorBoundingBox(actor, DirectX::Colors::RoyalBlue);

		if (!actor->RootComponent)
			return;

		auto loc = actor->RootComponent->Location;

		Classes::FVector2D sOrigin;
		if (!Engine::W2S(loc, &sOrigin))
			return;

		float dist = (loc - G::localPos).Size() / 100.f;

		//G::Draw->Text(sOrigin.X, sOrigin.Y, std::wstring(L"Death Drop: " + std::to_wstring((int)dist) + L"m"), DirectX::Colors::Red, 0, 8.f);
	}

	void DrawPlayerSkeleton(Classes::ATslCharacter* player)
	{
		// hand - lowerarm - upperarm
		// foot - calf - thigh
		// neck - spine2 - thigh1 & thigh2
		std::vector<std::vector<int>> boneVec =
		{
		{ Bone::hand_l, Bone::lowerarm_l, Bone::upperarm_l, Bone::neck_01 },
		{ Bone::hand_r, Bone::lowerarm_r, Bone::upperarm_r, Bone::neck_01 },
		{ Bone::foot_l, Bone::calf_l, Bone::thigh_l, Bone::spine_01 },
		{ Bone::foot_r, Bone::calf_r, Bone::thigh_r, Bone::spine_01 },
		{ Bone::spine_01, Bone::spine_02, Bone::neck_01, Bone::Head }
		};

		for (auto limb : boneVec)
		{
			Classes::FVector2D prev, cur;
			if (!Engine::GetBoneScreenPosition(player, &prev, limb.at(0)))
				break;

			for (size_t bone = 1; bone < limb.size(); bone++)
			{
				if (!Engine::GetBoneScreenPosition(player, &cur, limb.at(bone)))
					break;

				G::Draw->Line(prev.X, prev.Y, cur.X, cur.Y, Colors::Players);

				prev = cur;
			}
		}
	}

	void DrawActorOnRadar(Classes::FVector deltaFromLocal, Color col)
	{
		// Radar X Max : 1882
		// Radar X Min : 1630

		// Radar Y Max : 1050
		// Radar Y Min : 798

		// Radar Top Left : 1630,798
		// Radar Bottom Right : 1882,1050
		// Radar Center : 1756,924

		// Radar Size : 252

		// Radar Radius : 200.f

		if (deltaFromLocal.X > 20000.f || deltaFromLocal.X < -20000.f)
			return;

		if (deltaFromLocal.Y > 20000.f || deltaFromLocal.Y < -20000.f)
			return;

		float X = deltaFromLocal.X / 20000.f * 126;
		float Y = deltaFromLocal.Y / 20000.f * 126;

		// minus 2 because draw clear starts at corner
		G::Draw->Clear({ 5, 1754 + X, 922 + Y, 5 }, col);
	}

	void DrawPlayer(Classes::ATslCharacter* player)
	{
		if (player->Team)
			return;

		auto origin = player->RootComponent->Location;
		auto delta = origin - G::localPos;
		int dist = (int)delta.Size() / 100;

		DrawActorOnRadar(delta, DirectX::Colors::Red);

		if (dist < 128)
			DrawPlayerSkeleton(player);

		Classes::FVector2D s15;
		if (!Engine::W2S(Engine::GetBonePosition(player, 15), &s15))
			return;

		Color col = Colors::Players;
		col.f[3] = (float)dist / 100.f;
		G::Draw->Clear({ 4, s15.X - 2, s15.Y - 2, 4 }, col);

		//std::wstring ws(std::to_wstring(dist) + L"m\nHP: " + std::to_wstring((int)player->Health));
		//G::Draw->Text(s15.X + 20, s15.Y, ws, DirectX::Colors::Magenta, 0, 10.f);

		// Line ESP

		//Classes::FVector2D sOrigin;
		//if (!Engine::W2S(origin, &sOrigin))
		//	return;

		//G::Draw->Line(G::Draw->mViewport.Width / 2, G::Draw->mViewport.Height, sOrigin.X, sOrigin.Y, DirectX::Colors::Cyan);
	}

	std::wstring GetVehicleName(Classes::ATslWheeledVehicle* vehicle)
	{
		if (vehicle->IsA(Classes::ADacia_C::StaticClass())) return L"Dacia";
		if (vehicle->IsA(Classes::AUaz_C::StaticClass())) return L"UAZ";
		if (vehicle->IsA(Classes::ABuggy_C::StaticClass())) return L"Buggy";
		if (vehicle->IsA(Classes::ATslMotorbikeVehicle_Sidecar::StaticClass())) return L"Chariot";
		if (vehicle->IsA(Classes::ATslMotorbikeVehicle::StaticClass())) return L"Motorbike";
		
		return L"Unknown";
	}

	void DrawWheeledVehicle(Classes::ATslWheeledVehicle* vehicle)
	{
		if (!vehicle->RootComponent)
			return;

		Classes::FVector2D v;
		Classes::FVector pos = vehicle->RootComponent->Location;
		if (!Engine::W2S(pos, &v))
			return;

		DrawActorBoundingBox(vehicle, Colors::Vehicles);

		/*float dist = (pos - G::localPos).Size() / 100.f;
		if (dist > 400.f || !vehicle->VehicleCommonComponent)
		{
			std::wstring temp = GetVehicleName(vehicle) + L" " + std::to_wstring((int)dist) + L"m";
			G::Draw->Text(v.X, v.Y, temp, Colors::Vehicles, 0, 8.f);

			return;
		}

		std::wstring temp = GetVehicleName(vehicle) + L" " + std::to_wstring((int)dist) + L"m\n" + 
			std::to_wstring((int)(100 * vehicle->VehicleCommonComponent->Health / vehicle->VehicleCommonComponent->HealthMax)) + L" HP\n" + 
			std::to_wstring((int)(100 * vehicle->VehicleCommonComponent->Fuel / vehicle->VehicleCommonComponent->FuelMax)) + L" Fuel";

		G::Draw->Text(v.X, v.Y, temp, Colors::Vehicles, 0, 8.f);*/
	}

	std::vector<std::wstring> ARs = { L"AKM", L"M16A4" };
	std::vector<std::wstring> SRs = { L"Kar98k", L"Mini14", L"SKS" };
	std::vector<std::wstring> ARAttachments = { L"Ext. QuickDraw Mag (AR, S12K)", L"Suppressor (AR, S12K)", L"Compensator (AR, S12K)" };
	std::vector<std::wstring> SRAttachments = { L"Ext. QuickDraw Mag (SR)", L"Suppressor (SR)", L"Compensator (SR)" };
	std::vector<std::wstring> Scopes = { L"4x Scope", L"8x Scope" };
	std::vector<std::wstring> Lv3 = { L"Helmet (Lv.3)", L"Backpack (Lv.3)", L"Military Vest (Level 3)" };
	std::vector<std::wstring> Meds = { L"First Aid Kit", L"Med Kit", L"Painkiller", L"Energy Drink" };

	void DrawLootItem(std::wstring name, Classes::FVector absolutePos)
	{
		if (name == L"")
			return;

		Color col = DirectX::Colors::WhiteSmoke;

		if (std::find(ARs.begin(), ARs.end(), name) != ARs.end() || std::find(SRs.begin(), SRs.end(), name) != SRs.end())
			col = DirectX::Colors::Black;
		else if (std::find(ARAttachments.begin(), ARAttachments.end(), name) != ARAttachments.end() || std::find(SRAttachments.begin(), SRAttachments.end(), name) != SRAttachments.end())
			col = DirectX::Colors::Gold;
		else if (std::find(Scopes.begin(), Scopes.end(), name) != Scopes.end())
			col = DirectX::Colors::RoyalBlue;
		else if (std::find(Lv3.begin(), Lv3.end(), name) != Lv3.end())
			col = DirectX::Colors::LimeGreen;
		else if (std::find(Meds.begin(), Meds.end(), name) != Meds.end())
			col = DirectX::Colors::Magenta;
		else
			return;

		Classes::FVector2D sLoot;
		if (!Engine::W2S(absolutePos, &sLoot))
			return;
		
		G::Draw->Clear({ 5, sLoot.X, sLoot.Y, 5 }, col);

		/*int distance = ((absolutePos - G::localPos).Size() / 100.f);

		name += L" ";
		name += std::to_wstring(distance);
		name += L"m";

		G::Draw->Text(sLoot.X, sLoot.Y, name, col, 0, 8.f);*/
	}

	void DrawLootGroup(Classes::AActor* actor)
	{
		if (!actor->RootComponent)
			return;

		Classes::FVector actorPos = actor->RootComponent->Location;

		//if ((actorPos - G::localPos).Size() > 80000.f)
			//return;
		
		uintptr_t diga = *(uintptr_t*)((uintptr_t)actor + 0x2D8);
		int32_t count = *(int32_t*)((uintptr_t)actor + 0x2E0);
		
		TBitArray OwnedComponentsBitArray = *(TBitArray*)((uintptr_t)actor + 0x2E8); // Credit daemonium @ unknowncheats.me

		if (!diga)
			return;

		for (uint32_t i = 1; i < count; i++)
		{
			if (!IS_BIT_SET(OwnedComponentsBitArray.AllocatorInstanceData[i / 32], i % 32)) // Credit daemonium @ unknowncheats.me
				continue;

			auto droppedItem = *(Classes::UDroppedItemInteractionComponent**)(diga + i * 0x10);

			if (!droppedItem || !droppedItem->Item || !droppedItem->IsA(Classes::UDroppedItemInteractionComponent::StaticClass()))
				continue;

			auto item = droppedItem->Item;

			if (!item)
				continue;

			DrawLootItem(item->ItemName.Get(), actorPos + droppedItem->RelativeLocation);
		}
	}
}