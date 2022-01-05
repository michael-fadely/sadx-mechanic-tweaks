#include <SADXModLoader.h>
#include "ClassicJump.h"

static uint32_t last_buttons[EntityData1Ptrs_Length] = {};
static int last_frame[EntityData1Ptrs_Length] = {};

BOOL __cdecl DoJumpThing_r(EntityData1* a1, CharObj2* a2)
{
	const int i = a1->CharIndex;
	const uint32_t buttons = Controllers[i].HeldButtons;

	if (FrameCounter < last_frame[i] || FrameCounter - last_frame[i] > FrameIncrement)
	{
		last_buttons[i] = 0;
	}

	if (!(a1->Status & Status_DisableControl))
	{
		const float    full_jump  = static_cast<float>(a2->PhysicsData.HangTime) * 0.3f * a2->PhysicsData.JumpAddSpeed;
		const float    small_jump = full_jump * 0.615f;
		const uint32_t released   = last_buttons[i] & (buttons ^ last_buttons[i]);
		const uint32_t pressed    = buttons & (buttons ^ last_buttons[i]);

		if (!a2->JumpTime && pressed & JumpButtons)
		{
			a2->Speed.y = full_jump + a2->Speed.y;
		}
		else if (released & JumpButtons && (a2->Speed.y > small_jump || a2->JumpTime == 1))
		{
			a2->Speed.y = small_jump;
		}

		++a2->JumpTime;
	}

	last_buttons[i] = buttons;
	last_frame[i] = FrameCounter;
	return 0;
}
