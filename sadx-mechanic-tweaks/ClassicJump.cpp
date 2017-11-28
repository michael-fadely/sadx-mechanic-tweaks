#include <SADXModLoader.h>
#include "ClassicJump.h"

int lastButtons[8] = {};
int lastFrame[8] = {};

BOOL __cdecl DoJumpThing_r(EntityData1* a1, CharObj2* a2)
{
	const int i = a1->CharIndex;
	const auto buttons = Controllers[i].HeldButtons;

	if (FrameCounter < lastFrame[i] || FrameCounter - lastFrame[i] > FrameIncrement)
	{
		lastButtons[i] = 0;
	} 

	if (!(a1->Status & Status_DisableControl))
	{
		const float fulljump  = a2->PhysicsData.HangTime * 0.3f * a2->PhysicsData.JumpAddSpeed;
		const float smalljump = fulljump * 0.615f;
		const int   released  = lastButtons[i] & (buttons ^ lastButtons[i]);
		const int   pressed   = buttons & (buttons ^ lastButtons[i]);

		if (!a2->JumpTime && pressed & JumpButtons)
		{
			a2->Speed.y = fulljump + a2->Speed.y;
		}
		else if (released & JumpButtons && (a2->Speed.y > smalljump || a2->JumpTime == 1))
		{
			a2->Speed.y = smalljump;
		}

		++a2->JumpTime;
	}

	lastButtons[i] = buttons;
	lastFrame[i] = FrameCounter;
	return 0;
}
