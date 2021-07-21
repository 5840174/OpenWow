#include "stdafx.h"

// General
#include "WDBCreatureCache.h"

CowWDBCreatureCache::CowWDBCreatureCache(IFilesManager * FilesManager)
	: CowWDBFile(FilesManager, "creaturecache.wdb")
{
	auto file = LoadFile();
	if (file == nullptr)
	{
		Log::Warn("CowWDBCreatureCache:: File '%s' not found.", m_FileName.c_str());
	}

	while (false == file->isEof())
	{
		uint32 entry;
		file->read(&entry);

		uint32 entrySize;
		file->read(&entrySize);

		if (entrySize == 0)
			continue;

		std::shared_ptr<SCreatureQueryResult> creatureQueryResult = MakeShared(SCreatureQueryResult, entry);
		creatureQueryResult->Load(*file);

		if (entrySize != creatureQueryResult->GetSize())
			throw CException("Incorrect query size.");

		Log::Info("Creature '%s' loaded from cache.", creatureQueryResult->Name.c_str());

		m_Cache.insert(std::make_pair(entry, creatureQueryResult));
	}


	Log::Print("CowWDBCreatureCache:: Creature cache size = '%d'.", m_Cache.size());
}

CowWDBCreatureCache::~CowWDBCreatureCache()
{}

bool CowWDBCreatureCache::Get(CowGuid::EntryType_t Entry, std::shared_ptr<SCreatureQueryResult> * QueryResult) const
{
	const auto& it = m_Cache.find(Entry);
	if (it == m_Cache.end())
		return false;
	*QueryResult = it->second;
	return true;
}

void CowWDBCreatureCache::Add(std::shared_ptr<SCreatureQueryResult> QueryResult)
{
	auto& it = m_Cache.find(QueryResult->entry);
	if (it == m_Cache.end())
		m_Cache.insert(std::make_pair(QueryResult->entry, QueryResult));
	else
		it->second = QueryResult;
}



//
// Protected
//
void CowWDBCreatureCache::CreateCacheBuffer(IByteBuffer * ByteBuffer) const
{
	for (const auto& it : m_Cache)
	{
		uint32 entry = it.first;
		ByteBuffer->write(&entry);

		uint32 entrySize = it.second->GetSize();
		ByteBuffer->write(&entrySize);

		it.second->Save(*ByteBuffer);
	}

	uint32 lastEntryId = 0;
	ByteBuffer->write(&lastEntryId);

	uint32 lastEntrySize = 0;
	ByteBuffer->write(&lastEntrySize);
}
