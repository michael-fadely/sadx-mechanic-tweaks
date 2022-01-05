#include <SADXModLoader.h>

#include "EnemyBounceThing.h"

const void* EnemyBounceThing_ptr = reinterpret_cast<void*>(0x00441280);

FunctionPointer(void, WorldToPlayer, (EntityData1*, NJS_VECTOR*), 0x43EC00);

void __cdecl EnemyBounceThing_r(Uint8 player_id, float velocity_x, float velocity_y, float velocity_z)
{
#if 0
	const Uint8 index = player_id;
	EntityData2* v5 = EntityData2Ptrs[index];
	EntityData1* data1 = EntityData1Ptrs[index];
	CharObj2* data2 = CharObj2Ptrs[index];

	if (v5 && data2 && data1)
	{
		v5->VelocityDirection = { velocity_x, velocity_y, velocity_z };
		NJS_VECTOR new_speed = v5->VelocityDirection;
		WorldToPlayer(data1, &new_speed);

		// Nullifies horizontal speed
		if (new_speed.x <= 0.001f && new_speed.x >= -0.001f)
		{
			new_speed.x = 0;
		}

		if (new_speed.z <= 0.001f && new_speed.z >= -0.001f)
		{
			new_speed.z = 0;
		}

		data2->Speed = new_speed;
	}
#else
	// TODO: Update EntityData2::VelocityDirection

	EntityData1* data1 = EntityData1Ptrs[player_id];
	CharObj2* data2 = CharObj2Ptrs[player_id];

	if (!data1 || !data2)
	{
		return;
	}

	NJS_VECTOR new_speed = { velocity_x, velocity_y, velocity_z };
	WorldToPlayer(data1, &new_speed);

	switch (data1->CharID)
	{
		case Characters_Gamma:
		case Characters_Big:
		case Characters_Eggman:
		case Characters_Tikal:
			data2->Speed = new_speed;
			return;

		default:
			break;
	}

	if (!(data1->Status & Status_Ground) && (velocity_y > 0.0f && data2->Speed.y < 0.0f && data2->Speed.y < -velocity_y))
	{
		bool set = false;

		switch (data1->CharID)
		{
			case Characters_Sonic:
				set = data1->Action == 6 || data1->Action == 8 || data1->Action == 14;
				break;

			case Characters_Tails:
			case Characters_Knuckles:
				set = data1->Action == 6;
				break;

			case Characters_Amy:
				set = data1->Action == 5 || data1->Action == 22;
				break;

			default:
				break;
		}

		if (set)
		{
			data2->JumpTime = 0;
			new_speed.y = -(data2->Speed.y * ((data1->CharID == Characters_Amy && data1->Action == 21) ? 1.5f : 1.0f));
			PrintDebug("Bounce speed: %f\n", new_speed.y);
		}
	}

	if (data1->Status & Status_Ground || velocity_x != 0.0f || velocity_z != 0.0f)
	{
		data2->Speed = new_speed;
	}
	else
	{
		data2->Speed.y = new_speed.y;
	}
#endif
}