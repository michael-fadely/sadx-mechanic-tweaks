#include <SADXModLoader.h>

#include "EnemyBounceThing.h"

const void* EnemyBounceThing_ptr = reinterpret_cast<void*>(0x00441280);

FunctionPointer(void, sub_43EC00, (EntityData1*, NJS_VECTOR*), 0x43EC00);

void __cdecl EnemyBounceThing_r(Uint8 playerID, float speedX, float speedY, float speedZ)
{
#if 0
	auto index = playerID;
	auto v5 = (EntityData2*)EntityData2Ptrs[index];
	auto data1 = EntityData1Ptrs[index];
	auto data2 = CharObj2Ptrs[index];
	if (v5 && data2 && data1)
	{
		// what
		v5->field_4.x = speedX;
		v5->field_4.y = speedY;
		v5->field_4.z = speedZ;

		// obvious
		NJS_VECTOR newSpeed = { speedX, speedY, speedZ };

		// what
		sub_43EC00(data1, &newSpeed);

		// Nullifies horizontal speed
		if (newSpeed.x <= 0.001 && newSpeed.x >= -0.001)
		{
			newSpeed.x = 0;
		}
		if (newSpeed.z <= 0.001 && newSpeed.z >= -0.001)
		{
			newSpeed.z = 0;
		}
		data2->Speed = newSpeed;
	}
#else
	auto data1 = EntityData1Ptrs[playerID];
	auto data2 = CharObj2Ptrs[playerID];

	if (!data1 || !data2)
	{
		return;
	}

	NJS_VECTOR newSpeed = { speedX, speedY, speedZ };
	sub_43EC00(data1, &newSpeed);

	switch (data1->CharID)
	{
		case Characters_Gamma:
		case Characters_Big:
		case Characters_Eggman:
		case Characters_Tikal:
			data2->Speed = newSpeed;
			return;

		default:
			break;
	}

	if (!(data1->Status & Status_Ground) && (speedY > 0.0f && data2->Speed.y < 0.0f && data2->Speed.y < -speedY))
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
			newSpeed.y = -(data2->Speed.y * ((data1->CharID == Characters_Amy && data1->Action == 21) ? 1.5f : 1.0f));
			PrintDebug("Bounce speed: %f\n", newSpeed.y);
		}
	}

	if (data1->Status & Status_Ground || speedX != 0.0f || speedZ != 0.0f)
	{
		data2->Speed = newSpeed;
	}
	else
	{
		data2->Speed.y = newSpeed.y;
	}
#endif
}