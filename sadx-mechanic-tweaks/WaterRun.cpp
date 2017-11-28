#include <SADXModLoader.h>
#include <cmath>
#include "WaterRun.h"

const void* loc_4496E1 = reinterpret_cast<void*>(0x004496E1);

static BOOL __stdcall WaterRun_c(EntityData1* data1, CharObj2* data2)
{
	if (data1->CharID == Characters_Sonic && (data2->Upgrades & Upgrades_SuperSonic) != 0)
	{
		return 1;
	}

	if (data1->CharID == Characters_Gamma)
	{
		return 1;
	}

	if (data2->Speed.x >= 5.0f && abs(data2->Speed.y) <= 2.0f)
	{
		switch (data1->Action)
		{
			case 2:
			case 12:
				return 1;

			default:
				return 0;
		}
	}

	return 0;
}

static const auto surface_solid = reinterpret_cast<void*>(0x004496E7);
static const auto surface_water = reinterpret_cast<void*>(0x004497B6);

void __declspec(naked) WaterRun_asm()
{
	__asm
	{
	//	// If Gamma, treat surface as solid
	//	jnz not_gamma
	//	jmp surface_solid

	//not_gamma:
		// Save whatever's in EAX
		push eax

		push [esp + 6A8h + 4h + 0Ch] // CharObj2
		push ebx                     // EntityData1
		call WaterRun_c

		test eax, eax

		jnz is_true

		// Restore EAX
		pop eax

		jmp surface_water

	is_true:
		// Restore EAX
		pop eax
		jmp surface_solid
	}
}
