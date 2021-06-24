#include "stdafx.h"

// General
#include "WorldObjectUpdater.h"

// Additional
#include "World.h"

// Additional (zlib)
#include "zlib/source/zlib.h"
#pragma comment(lib, "zlib.lib")


namespace
{
struct ZN_API Block_UPDATETYPE_OUT_OF_RANGE_OBJECTS
{
	void Fill(CByteBuffer& ByteBuffer)
	{
		uint32 OutOfRangeGUIDsCount;
		ByteBuffer >> (uint32)OutOfRangeGUIDsCount;
		OutOfRangeGUIDs.resize(OutOfRangeGUIDsCount);

		for (uint32 i = 0; i < OutOfRangeGUIDsCount; i++)
		{
			//ByteBuffer.seekRelative(sizeof(uint8)); // (uint8)0xFF;

			uint64 guid;
			ByteBuffer.ReadPackedUInt64(guid);
			OutOfRangeGUIDs[i] = guid;
		}
	}

	std::vector<uint64> OutOfRangeGUIDs;
};
}


CWorldObjectUpdater::CWorldObjectUpdater(CWoWWorld& WoWWorld, IScene & Scene)
	: m_Scene(Scene)
	, m_WoWWorld(WoWWorld)
{
	m_WoWWorld.AddHandler(SMSG_COMPRESSED_UPDATE_OBJECT, std::bind(&CWorldObjectUpdater::On_SMSG_COMPRESSED_UPDATE_OBJECT, this, std::placeholders::_1));
	m_WoWWorld.AddHandler(SMSG_UPDATE_OBJECT, std::bind(&CWorldObjectUpdater::On_SMSG_UPDATE_OBJECT, this, std::placeholders::_1));
}

CWorldObjectUpdater::~CWorldObjectUpdater()
{}



void CWorldObjectUpdater::On_SMSG_COMPRESSED_UPDATE_OBJECT(CServerPacket& Buffer)
{
	uint32 dataSize;
	Buffer >> dataSize;

	CByteBuffer uncompressed;
	uncompressed.writeDummy(dataSize);
	uncompressed.seek(0);

	uLongf uncompressedSize = dataSize;
	if (::uncompress(uncompressed.getDataEx(), &uncompressedSize, Buffer.getDataFromCurrent(), Buffer.getSize() - 4) != Z_OK)
		throw CException("CWorldObjectUpdater::On_SMSG_COMPRESSED_UPDATE_OBJECT: Error while ::uncompress data.");

	Buffer.seekRelative(Buffer.getSize() - 4);

	if (dataSize != uncompressedSize)
		throw CException("CWorldObjectUpdater::On_SMSG_COMPRESSED_UPDATE_OBJECT: Uncompressed data size (%d bytes) don't match expected data size (%d bytes).", uncompressedSize, dataSize);

	ProcessUpdatePacket(uncompressed);
}

void CWorldObjectUpdater::On_SMSG_UPDATE_OBJECT(CServerPacket& Buffer)
{
	ProcessUpdatePacket(Buffer);
}



//
// Private
//
void CWorldObjectUpdater::ProcessUpdatePacket(CByteBuffer& Bytes)
{
	uint32 BlocksCount;
	Bytes >> BlocksCount;

	for (uint32 i = 0u; i < BlocksCount; i++)
	{
		OBJECT_UPDATE_TYPE updateType;
		Bytes.read(&updateType);

		switch (updateType)
		{
			case OBJECT_UPDATE_TYPE::UPDATETYPE_VALUES:
			{
				uint64 guidValue;
				Bytes.ReadPackedUInt64(guidValue);
				CWoWObjectGuid guid(guidValue);

				std::shared_ptr<WoWObject> object = m_WoWWorld.GetWorldObjects().GetWoWObject(guid);
				object->UpdateValues(Bytes);
			}
			break;


			case OBJECT_UPDATE_TYPE::UPDATETYPE_MOVEMENT:
			{
				uint64 guidValue;
				Bytes.ReadPackedUInt64(guidValue);
				CWoWObjectGuid guid(guidValue);

				std::shared_ptr<WoWObject> object = m_WoWWorld.GetWorldObjects().GetWoWObject(guid);
				object->ProcessMovementUpdate(Bytes);
			}
			break;


			case OBJECT_UPDATE_TYPE::UPDATETYPE_CREATE_OBJECT:
			{
				uint64 guidValue;
				Bytes.ReadPackedUInt64(guidValue);
				CWoWObjectGuid guid(guidValue);

				EWoWObjectTypeID typeID;
				Bytes.read(&typeID);

				//if (false == IsWoWObjectExists(guid))
				//	throw CException("CWoWWorld::ProcessUpdatePacket: WoWObject '%ull' already exists.", guidValue);

				auto object = m_WoWWorld.GetWorldObjects().GetWoWObject(guid);
				object->ProcessMovementUpdate(Bytes);
				object->UpdateValues(Bytes);

				//if (typeID != EWoWObjectTypeID::TYPEID_UNIT)
					object->AfterCreate(m_Scene);
			}
			break;


			case OBJECT_UPDATE_TYPE::UPDATETYPE_CREATE_OBJECT2: // isNewObject
			{
				uint64 guidValue;
				Bytes.ReadPackedUInt64(guidValue);
				CWoWObjectGuid guid(guidValue);

				EWoWObjectTypeID typeID;
				Bytes.read(&typeID);

				//if (IsWoWObjectExists(guid))
				//	throw CException("CWoWWorld::ProcessUpdatePacket: WoWObject '%ull' already exists.", guidValue);

				auto object = m_WoWWorld.GetWorldObjects().GetWoWObject(guid);
				object->ProcessMovementUpdate(Bytes);
				object->UpdateValues(Bytes);

				//if (typeID != EWoWObjectTypeID::TYPEID_UNIT)
					object->AfterCreate(m_Scene);
			}
			break;


			case OBJECT_UPDATE_TYPE::UPDATETYPE_OUT_OF_RANGE_OBJECTS:
			{
				Block_UPDATETYPE_OUT_OF_RANGE_OBJECTS block;
				block.Fill(Bytes);
			}
			break;


			default:
				throw CException("Unknown update type '%d.'", updateType);
		}
	}
}