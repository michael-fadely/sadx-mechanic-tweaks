#include <SADXModLoader.h>
#include <Trampoline.h>
#include "EnemyBounceThing.h"
#include "WaterRun.h"
#include "ClassicJump.h"
#include "ClassicRoll.h"

PointerInfo jumps[] = {
	// this didn't work somehow
	ptrdecl(loc_4496E1, WaterRun_asm),
	ptrdecl(EnemyBounceThing_ptr, EnemyBounceThing_r),
	ptrdecl(DoJumpThing, DoJumpThing_r)
};

static const int jump_cancel = Buttons_B;

static __int16 __cdecl RunSceneLogic_r();
static Trampoline RunSceneLogic_t(0x00413D40, 0x00413D45, reinterpret_cast<void*>(&RunSceneLogic_r));
static __int16 __cdecl RunSceneLogic_r()
{
	const auto original = reinterpret_cast<decltype(RunSceneLogic_r)*>(RunSceneLogic_t.Target());
	auto result = original();

	ClassicRoll_OnFrame();

	return result;
}

extern "C"
{
	__declspec(dllexport) PointerList Jumps[] = { { arrayptrandlengthT(jumps, int) } };
	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
		PhysicsArray[Characters_Amy] = PhysicsArray[Characters_Knuckles];

		PhysicsData* p = &PhysicsArray[Characters_Sonic];

		p->GroundAccel = 4.0f * 0.0078125f;
		p->GroundDecel = -p->GroundAccel;
		p->AirAccel = p->GroundAccel * 2.0f;
		// Deliberately ignoring AirDecel here because it breaks everything forever
		//p->RollCancel = p->GroundAccel;
		//p->RollDecel = -(p->GroundAccel / 2.0f);
		//p->RollEnd = 0.0f;
		p->Brake = -(p->GroundAccel * 10.0f);

		WriteData((const int**)0x00492F56, &jump_cancel);
		WriteData((short*)0x00497476, (short)0xE990);

		// Disable all dash panels
		WriteData((char*)0x007A4450, 0xC3i8);
	}
}
