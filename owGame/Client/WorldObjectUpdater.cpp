#include "stdafx.h"

// General
#include "WorldObjectUpdater.h"

// Additional
#include "World.h"

// Additional (zlib)
#include "zlib/source/zlib.h"
#pragma comment(lib, "zlib.lib")


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
				std::vector<uint64> outOfRangeGUIDs;

				uint32 outOfRangeGUIDsCount;
				Bytes >> (uint32)outOfRangeGUIDsCount;
				outOfRangeGUIDs.resize(outOfRangeGUIDsCount);
				for (uint32 i = 0; i < outOfRangeGUIDsCount; i++)
				{
					uint64 guid;
					Bytes.ReadPackedUInt64(guid);
					outOfRangeGUIDs[i] = guid;
				}

				auto& worldObjects = m_WoWWorld.GetWorldObjects();
				for (const auto& outOfRangeGUID : outOfRangeGUIDs)
				{
					if (auto object = worldObjects.GetWoWObject(CWoWObjectGuid(outOfRangeGUID)))
					{
						object->Destroy();
						worldObjects.EraseWoWObject(object);
					}
				}
			}
			break;


			default:
				throw CException("Unknown update type '%d.'", updateType);
		}
	}
}