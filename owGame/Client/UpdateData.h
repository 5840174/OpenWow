#pragma once

#include "ObjectDefines.h"

enum class ZN_API OBJECT_UPDATE_TYPE : uint8
{
	UPDATETYPE_VALUES = 0,
	UPDATETYPE_MOVEMENT = 1,
	UPDATETYPE_CREATE_OBJECT = 2,
	UPDATETYPE_CREATE_OBJECT2 = 3,
	UPDATETYPE_OUT_OF_RANGE_OBJECTS = 4,
	UPDATETYPE_NEAR_OBJECTS = 5
};

struct ZN_API Block_UPDATETYPE_VALUES
{
	void Fill(CByteBuffer& Bytes)
	{
		Bytes.seekRelative(1); // 0xFF
		Bytes >> guid;

		uint8 blocksCnt;
		Bytes >> (uint8)blocksCnt;
		Bytes.seekRelative((blocksCnt));
	}

	uint64 guid = 0;
};

struct ZN_API Block_UPDATETYPE_CREATE_OBJECT
{
	enum class ZN_API TypeID : uint8
	{
		TYPEID_OBJECT = 0,
		TYPEID_ITEM = 1,
		TYPEID_CONTAINER = 2,
		TYPEID_UNIT = 3,
		TYPEID_PLAYER = 4,
		TYPEID_GAMEOBJECT = 5,
		TYPEID_DYNAMICOBJECT = 6,
		TYPEID_CORPSE = 7,
		TYPEID_AIGROUP = 8,
		TYPEID_AREATRIGGER = 9
	};

	// For UPDATETYPE_CREATE_OBJECT2
	void Fill_UPDATETYPE_MOVEMENT(CByteBuffer& Bytes)
	{
		Bytes >> guid;

		_BuildMovementUpdate(Bytes, TypeID::TYPEID_OBJECT);
	}

	// For UPDATETYPE_CREATE_OBJECT
	void Fill_UPDATETYPE_CREATE_OBJECT(CByteBuffer& Bytes)
	{
		Bytes.seekRelative(1); // 0xFF
		Bytes >> guid;

		TypeID typeID;
		Bytes.read(&typeID);

		switch (typeID)
		{
			case TypeID::TYPEID_OBJECT:
				break;

			case TypeID::TYPEID_ITEM:
			case TypeID::TYPEID_CONTAINER:
				_BuildMovementUpdate(Bytes, typeID);
				break;

			case TypeID::TYPEID_UNIT:
				_BuildMovementUpdate(Bytes, typeID);
				break;

			case TypeID::TYPEID_PLAYER:
				_BuildMovementUpdate(Bytes, typeID);
				break;

			case TypeID::TYPEID_CORPSE:
			case TypeID::TYPEID_GAMEOBJECT:
			case TypeID::TYPEID_DYNAMICOBJECT:
				_BuildMovementUpdate(Bytes, typeID);
				break;
		}
	}

	// For UPDATETYPE_CREATE_OBJECT2
	void Fill_UPDATETYPE_CREATE_OBJECT2(CByteBuffer& Bytes)
	{
		Bytes.seekRelative(1); // 0xFF
		Bytes >> guid;

		TypeID typeID;
		Bytes.read(&typeID);
		
		_BuildMovementUpdate(Bytes, typeID);
	}

	void _BuildMovementUpdate(CByteBuffer& Bytes, TypeID typeID)
	{
		Bytes >> (uint8)flags;

		if (typeID == TypeID::TYPEID_PLAYER)
		{
			Bytes >> (uint32)flags2;
			Bytes >> (uint32)msTime;

			if ((flags2 & 0x02000000) == 0) // GetTransport() == nullptr
			{
				Bytes >> PositionX;
				Bytes >> PositionY;
				Bytes >> PositionZ;
				Bytes >> Orientation;
			}
			else
			{
				float transportX, transportY, transportZ, transportO;
				uint64 transportGUID;
				Bytes >> transportX;
				Bytes >> transportY;
				Bytes >> transportZ;
				Bytes >> transportO;
				Bytes >> transportGUID;

				Bytes >> PositionX;
				Bytes >> PositionY;
				Bytes >> PositionZ;
				Bytes >> Orientation;
			}

			Bytes.seekRelative(sizeof(float)); // (float)0;

			if (flags2 & 0x2000) //update self
			{
				Bytes.seekRelative(4 * sizeof(float));
				//Bytes >> (float)0;
				//Bytes >> (float)1.0;
				//Bytes >> (float)0;
				//Bytes >> (float)0;
			}

			Bytes >> Speed_MOVE_WALK;
			Bytes >> Speed_MOVE_RUN;
			Bytes >> Speed_MOVE_SWIM_BACK;
			Bytes >> Speed_MOVE_SWIM;
			Bytes >> Speed_MOVE_RUN_BACK;
			Bytes >> Speed_MOVE_TURN_RATE;
		}
		if (typeID == TypeID::TYPEID_UNIT)
		{
			Bytes >> (uint32)flags2;
			Bytes.seekRelative(sizeof(uint32)); // (uint32)0xB5771D7F;
			Bytes >> PositionX;
			Bytes >> PositionY;
			Bytes >> PositionZ;
			Bytes >> Orientation;
			Bytes.seekRelative(sizeof(float)); //(float)0
			Bytes >> Speed_MOVE_WALK;
			Bytes >> Speed_MOVE_RUN;
			Bytes >> Speed_MOVE_SWIM_BACK;
			Bytes >> Speed_MOVE_SWIM;
			Bytes >> Speed_MOVE_RUN_BACK;
			Bytes >> Speed_MOVE_TURN_RATE;

			
			if (flags2 & 0x400000)
			{
				Bytes.seekRelative(sizeof(uint32)); //(uint32)0x0;
				Bytes.seekRelative(sizeof(uint32)); //(uint32)0x659;
				Bytes.seekRelative(sizeof(uint32)); //(uint32)0xB7B;
				Bytes.seekRelative(sizeof(uint32)); //(uint32)0xFDA0B4;

				uint32 PosCount;
				Bytes >> (uint32)PosCount;
				for (int i = 0; i < PosCount + 1; i++)
					Bytes.seekRelative(3 * sizeof(float)); // (float)0;
			}
		}

		if ((typeID == TypeID::TYPEID_CORPSE) || (typeID == TypeID::TYPEID_GAMEOBJECT) || (typeID == TypeID::TYPEID_DYNAMICOBJECT))
		{
			Bytes >> PositionX;
			Bytes >> PositionY;
			Bytes >> PositionZ;
			Bytes >> Orientation;
		}

		Bytes.seekRelative(sizeof(uint32)); //(uint32)0x1;

		if (GUID_HIPART(guid) == HIGHGUID_TRANSPORT)
		{
			Bytes >> (uint32)msTime;
		}

		if (GUID_HIPART(guid) == HIGHGUID_PLAYER_CORPSE)
		{
			Bytes.seekRelative(sizeof(uint32)); // (uint32)0xBD38BA14
		}
	}

	uint64 guid = 0;
	
	uint8 flags = 0;
	uint32 flags2 = 0;

	uint32 msTime = 0;

	float PositionX = 0.0f;
	float PositionY = 0.0f;
	float PositionZ = 0.0f;
	float Orientation = 0.0f;

	float Speed_MOVE_WALK = 0.0f;
	float Speed_MOVE_RUN = 0.0f;
	float Speed_MOVE_SWIM_BACK = 0.0f;
	float Speed_MOVE_SWIM = 0.0f;
	float Speed_MOVE_RUN_BACK = 0.0f;
	float Speed_MOVE_TURN_RATE = 0.0f;
};

struct ZN_API Block_UPDATETYPE_OUT_OF_RANGE_OBJECTS
{
	void Fill(CByteBuffer& ByteBuffer)
	{
		uint32 OutOfRangeGUIDsCount;
		ByteBuffer >> (uint32)OutOfRangeGUIDsCount;
		OutOfRangeGUIDs.resize(OutOfRangeGUIDsCount);

		for (uint32 i = 0; i < OutOfRangeGUIDsCount; i++)
		{
			ByteBuffer.seekRelative(sizeof(uint8)); // (uint8)0xFF;

			uint64 guid;
			ByteBuffer >> (uint64)guid;
			OutOfRangeGUIDs[i] = guid;
		}
	}

	std::vector<uint64> OutOfRangeGUIDs;
};




struct ZN_API SUpdateData
{
	void Fill(CByteBuffer& Bytes)
	{
		Bytes >> BlocksCount;
		Bytes >> HasTransport;

		for (uint32 i = 0u; i < BlocksCount; i++)
		{
			ProcessBlock(Bytes);
		}
	}

	void ProcessBlock(CByteBuffer& Bytes)
	{
		OBJECT_UPDATE_TYPE updateType;
		Bytes.read(&updateType);

		switch (updateType)
		{
			case OBJECT_UPDATE_TYPE::UPDATETYPE_VALUES:
			{
				Block_UPDATETYPE_VALUES block;
				block.Fill(Bytes);
				UPDATETYPE_VALUES.push_back(block);

				//Log::Warn("UPDATETYPE_VALUES");
			}
			break;
			case OBJECT_UPDATE_TYPE::UPDATETYPE_MOVEMENT:
			{
				Block_UPDATETYPE_CREATE_OBJECT block;
				block.Fill_UPDATETYPE_MOVEMENT(Bytes);
				UPDATETYPE_CREATE_OBJECT.push_back(block);
				//Log::Warn("UPDATETYPE_MOVEMENT");
			}
			break;
			case OBJECT_UPDATE_TYPE::UPDATETYPE_CREATE_OBJECT:
			{
				Block_UPDATETYPE_CREATE_OBJECT block;
				block.Fill_UPDATETYPE_CREATE_OBJECT(Bytes);
				UPDATETYPE_CREATE_OBJECT.push_back(block);
				//Log::Warn("UPDATETYPE_CREATE_OBJECT");
			}
			break;
			case OBJECT_UPDATE_TYPE::UPDATETYPE_CREATE_OBJECT2:
			{
				Block_UPDATETYPE_CREATE_OBJECT block;
				block.Fill_UPDATETYPE_CREATE_OBJECT2(Bytes);
				UPDATETYPE_CREATE_OBJECT.push_back(block);
				//Log::Warn("UPDATETYPE_CREATE_OBJECT2");
			}
			break;
			case OBJECT_UPDATE_TYPE::UPDATETYPE_OUT_OF_RANGE_OBJECTS:
			{
				Block_UPDATETYPE_OUT_OF_RANGE_OBJECTS block;
				block.Fill(Bytes);
				UPDATETYPE_OUT_OF_RANGE_OBJECTS.push_back(block);
				//Log::Warn("UPDATETYPE_OUT_OF_RANGE_OBJECTS");
			}
			break;
			//default:
			//	_ASSERT(false);
		}
	}

	uint32 BlocksCount;
	uint8 HasTransport;

	std::vector<Block_UPDATETYPE_VALUES> UPDATETYPE_VALUES;
	std::vector<Block_UPDATETYPE_CREATE_OBJECT> UPDATETYPE_CREATE_OBJECT;
	std::vector<Block_UPDATETYPE_OUT_OF_RANGE_OBJECTS> UPDATETYPE_OUT_OF_RANGE_OBJECTS;
};