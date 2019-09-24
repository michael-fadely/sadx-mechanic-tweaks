#include <SADXModLoader.h>
#include <Trampoline.h>
#include <cmath>
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

#pragma pack(push, 1)
struct HomingAttackTarget
{
	EntityData1 *entity;
	float distance;
};
#pragma pack(pop)


DataPointer(NJS_TEXLIST, TARGET_TEXLIST, 0x029384AC);
DataPointer(NJS_SPRITE, TornadoTarget_SPRITE, 0x029384F0);
FunctionPointer(void, LoadNoNamePVM, (NJS_TEXLIST *a1), 0x00420D80);
FunctionPointer(int, sub_4383B0, (int a1, int a2), 0x4383B0);
DataArray(HomingAttackTarget, HomingAttackTarget_Sonic, 0x03B259C0, 657);

NJS_SPRITE target_sprite {};

static __int16 __cdecl RunSceneLogic_r();
static Trampoline RunSceneLogic_t(0x00413D40, 0x00413D45, reinterpret_cast<void*>(&RunSceneLogic_r));

HomingAttackTarget* get_nearest()
{
	HomingAttackTarget* result = nullptr;

	for (int i = 0; i < HomingAttackTarget_Sonic_Length; i++)
	{
		auto& e = HomingAttackTarget_Sonic[i];
		if (e.entity == nullptr)
		{
			break;
		}

		if (result == nullptr || result->distance > e.distance)
		{
			result = &e;
		}
	}

	return result;
}

static void draw_reticle(NJS_VECTOR& target_pos, bool red)
{
	NJS_VECTOR vd {};

	njPushMatrix(reinterpret_cast<NJS_MATRIX_PTR>(0x0389D398));
	njCalcPoint(nullptr, &target_pos, &vd);
	njPopMatrix(1);

	const auto m     = _nj_screen_.dist / vd.z;
	const auto xhalf = HorizontalResolution / 2.0f;
	const auto yhalf = VerticalResolution / 2.0f;

	vd.x = vd.x * m + xhalf;
	vd.y = vd.y * m + yhalf;

	target_sprite.p  = { vd.x, vd.y, 0.0f };
	target_sprite.sx = min(HorizontalStretch, VerticalStretch);
	target_sprite.sy = target_sprite.sx;

	njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_ONE);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_ONE);
	njDrawSprite2D_Queue(&target_sprite, red ? 2 : 0, -1.0f, NJD_SPRITE_ALPHA | NJD_SPRITE_ANGLE, static_cast<QueuedModelFlagsB>(0));
	njColorBlendingMode(NJD_SOURCE_COLOR, NJD_COLOR_BLENDING_SRCALPHA);
	njColorBlendingMode(NJD_DESTINATION_COLOR, NJD_COLOR_BLENDING_INVSRCALPHA);
}

static void draw_reticle(NJS_VECTOR& player_pos, Angle player_angle, NJS_VECTOR& target_pos)
{
	NJS_VECTOR delta = {
		player_pos.x - target_pos.x,
		player_pos.y - target_pos.y,
		player_pos.z - target_pos.z
	};

	const auto distance = njScalor2(&delta);

	if (distance >= 10000.0f || sub_4383B0(NJM_RAD_ANG(atan2(-delta.z, -delta.x)), player_angle) > 20480)
	{
		return;
	}

	draw_reticle(target_pos, false);
}

static void DrawHomingIndicators()
{
	// TODO: maybe show for light speed dash rings

	if (!CharObj1Ptrs[0])
	{
		return;
	}

	LoadNoNamePVM(&TARGET_TEXLIST);

	auto& ppos = CharObj1Ptrs[0]->Position;

	target_sprite.ang += NJM_DEG_ANG(5.625f);
	target_sprite.ang %= 65536;

	DisplayDebugStringFormatted(0, "%04X", CharObj1Ptrs[0]->Status);

	if (CharObj1Ptrs[0]->Status & Status_LightDash && CharObj2Ptrs[0]->Upgrades & (Upgrades_AncientLight | Upgrades_FightingGloves))
	{
		for (int i = 0; i < HomingAttackTarget_Sonic_Length; i++)
		{
			auto& e = HomingAttackTarget_Sonic[i];
			if (e.entity == nullptr)
			{
				break;
			}

			auto l = e.entity->CollisionInfo->List;
			if (l == 2 || l == 3)
			{
				draw_reticle(e.entity->Position, true);
			}
		}
	}

	if (!(CharObj1Ptrs[0]->Status & Status_Ground) && CharObj1Ptrs[0]->Status & Status_Ball)
	{
		auto nearest = get_nearest();
		if (nearest != nullptr)
		{
			draw_reticle(ppos, CharObj1Ptrs[0]->Rotation.y, nearest->entity->Position);
		}
	}
}

static __int16 __cdecl RunSceneLogic_r()
{
	const auto original = reinterpret_cast<decltype(RunSceneLogic_r)*>(RunSceneLogic_t.Target());
	auto result = original();

	ClassicRoll_OnFrame();
	DrawHomingIndicators();
	return result;
}

extern "C"
{
	__declspec(dllexport) PointerList Jumps[] = { { arrayptrandlengthT(jumps, int) } };
	__declspec(dllexport) ModInfo SADXModInfo = { ModLoaderVer };
	__declspec(dllexport) void __cdecl Init(const char* path, const HelperFunctions& helperFunctions)
	{
		target_sprite = TornadoTarget_SPRITE;
		//target_sprite.sx = 0.1f;
		//target_sprite.sy = 0.1f;

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

		WriteData(reinterpret_cast<const int**>(0x00492F56), &jump_cancel);
		WriteData(reinterpret_cast<short*>(0x00497476), static_cast<short>(0xE990i16));

		// Disable all dash panels
		WriteData(reinterpret_cast<Uint8*>(0x007A4450), static_cast<Uint8>(0xC3));
	}
}
