#include <SADXModLoader.h>
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

namespace CollisionList
{
	enum _enum
	{
		Players,
		Unknown_1,
		Targetable,
		Hazard,
		Unknown_4,
		Unknown_5,
		Minimal,
		Rings,
		Unknown_8,
		Chao,
		COUNT
	};
}

#pragma pack(push, 1)
struct _CollisionData
{
	__int16 field_0;
	__int16 field_2;
	int field_4;
	NJS_VECTOR v;
	float anonymous_1;
	float anonymous_2;
	float anonymous_3;
	int field_20;
	int field_24;
	int field_28;
	int field_2C;
};
struct _CollisionInfo
{
	__int16 List;
	__int16 field_2;
	__int16 Flags;
	__int16 Count;
	float f8;
	_CollisionData *CollisionArray;
	Uint8 field_10[140];
	ObjectMaster *Object;
	__int16 field_A0;
	__int16 field_A2;
	_CollisionInfo *CollidingObject;
};
#pragma pack(pop)

enum CollisionMode : Sint16
{
	Unknown00,
	Unknown01,
	Unknown02,
	Solid,
	Unknown04,
	Unknown05,
	Unknown06,
	Unknown07,
	Unknown08,
	Unknown09,
	CanTarget = 0x40
};

#if 0
static void __cdecl InitCollision_(ObjectMaster *obj, _CollisionData *collisionArray, int count, Uint8 list);
static Trampoline hax(0x0041CAF0, 0x0041CAF5, InitCollision_);
static void __cdecl InitCollision_(ObjectMaster* obj, _CollisionData* collisionArray, int count, Uint8 list)
{
	FunctionPointer(void, original, (ObjectMaster*, _CollisionData*, int, Uint8), hax.Target());
	original(obj, collisionArray, count, list);

	//if (list != 6)
		//return;

	if (collisionArray == nullptr)
		return;

	//auto info = reinterpret_cast<_CollisionInfo*>(obj->Data1->CollisionInfo);
	//if (info != nullptr)
		//info->Flags = 0;

	for (auto i = 0; i < count; i++)
	{
		//collisionArray[i].field_0 = 0;
		collisionArray[i].field_2 = 0;
	}
}
#endif

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
	__declspec(dllexport) void __cdecl OnFrame()
	{
		ClassicRoll_OnFrame();
	}
}
