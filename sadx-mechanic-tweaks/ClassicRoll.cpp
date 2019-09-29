#include <SADXModLoader.h>
#include <Trampoline.h>
#include <cmath>

#include "ClassicRoll.h"

static constexpr float speed_threshold = 1.5f;
static int last_status[8] = {};

static BOOL __stdcall Sonic_ChargeSpindash_(CharObj2* data2, EntityData1* data1);

static void __declspec(naked) Sonic_ChargeSpindash_asm()
{
	__asm
	{
		push edi
		push eax
		call Sonic_ChargeSpindash_
		ret
	}
}

static Trampoline Sonic_ChargeSpindash_t(0x00496EE0, 0x00496EE6, Sonic_ChargeSpindash_asm);

static BOOL __stdcall Sonic_ChargeSpindash_original(CharObj2* data2, EntityData1* data1)
{
	void* original = Sonic_ChargeSpindash_t.Target();
	__asm
	{
		mov edi, data1
		mov eax, data2
		call original
	}
}

static BOOL __stdcall Sonic_ChargeSpindash_(CharObj2* data2, EntityData1* data1)
{
	auto controller = &Controllers[data1->CharIndex];

	// Change AttackButtons here to Buttons_X to allow vanilla behavior with the B button
	if (controller->PressedButtons & AttackButtons && abs(njScalor(&data2->Speed)) >= speed_threshold)
	{
		int last_held = controller->HeldButtons;
		controller->HeldButtons &= ~AttackButtons;

		data1->Status              |= Status_Attack | Status_Ball;
		data1->Action               = 5;
		data2->AnimationThing.Index = 15; // ???
		data2->SpindashSpeed        = data2->Speed.x;
		data2->LightdashTime        = 0;
		data2->LightdashTimer       = 0;
		data2->SonicSpinTimer       = 0;

		BOOL result = Sonic_ReleaseSpindash(data1, data2);

		controller->HeldButtons = last_held;
		return result;
	}

	return Sonic_ChargeSpindash_original(data2, data1);
}

void ClassicRoll_OnFrame()
{
	for (Uint32 i = 0; i < 8; i++)
	{
		auto data1 = EntityData1Ptrs[i];
		auto data2 = CharObj2Ptrs[i];

		if (data1 == nullptr || data1->CharID != Characters_Sonic || data2 == nullptr)
		{
			last_status[i] = 0;
			continue;
		}

		if (data1->Action >= 62 && data1->Action <= 66)
		{
			continue;
		}

		auto controller = &Controllers[i];
		if (data1->Action != 5 && data1->Status & Status_Ground && abs(njScalor(&data2->Speed)) >= speed_threshold)
		{
			int last_pressed = controller->PressedButtons;
			controller->PressedButtons |= controller->HeldButtons & AttackButtons;
			Sonic_ChargeSpindash(data2, data1);
			controller->PressedButtons = last_pressed;
		}

		last_status[i] = data1->Status;
	}
}
