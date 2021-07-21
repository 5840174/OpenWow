#include "stdafx.h"

// General
#include "WDBGameObjectCache.h"

CowWDBGameObjectCache::CowWDBGameObjectCache(IFilesManager * FilesManager)
	: CowWDBFile(FilesManager, "gameobjectcache.wdb")
{
	auto file = LoadFile();
	if (file == nullptr)
	{
		Log::Warn("CowWDBGameObjectCache:: File '%s' not found.", m_FileName.c_str());
	}

	while (false == file->isEof())
	{
		uint32 entry;
		file->read(&entry);

		uint32 entrySize;
		file->read(&entrySize);

		if (entrySize == 0)
			continue;

		std::shared_ptr<SGameObjectQueryResult> gameObjectQueryResult = MakeShared(SGameObjectQueryResult, entry);
		gameObjectQueryResult->Load(*file);

		if (entrySize != gameObjectQueryResult->GetSize())
			throw CException("Incorrect query size.");

		Log::Info("GameObject '%s' loaded from cache.", gameObjectQueryResult->Name.c_str());

		m_Cache.insert(std::make_pair(entry, gameObjectQueryResult));
	}

	Log::Print("CowWDBGameObjectCache:: GameObject cache size = '%d'.", m_Cache.size());
}

CowWDBGameObjectCache::~CowWDBGameObjectCache()
{}

bool CowWDBGameObjectCache::Get(CowGuid::EntryType_t Entry, std::shared_ptr<SGameObjectQueryResult> * QueryResult) const
{
	const auto& it = m_Cache.find(Entry);
	if (it == m_Cache.end())
		return false;
	*QueryResult = it->second;
	return true;
}

void CowWDBGameObjectCache::Add(CowGuid::EntryType_t Entry, std::shared_ptr<SGameObjectQueryResult> QueryResult)
{
	auto& it = m_Cache.find(Entry);
	if (it == m_Cache.end())
		m_Cache.insert(std::make_pair(Entry, QueryResult));
	else
		it->second = QueryResult;
}



//
// Protected
//
void CowWDBGameObjectCache::CreateCacheBuffer(IByteBuffer * ByteBuffer) const
{
	for (const auto& it : m_Cache)
	{
		uint32 entry = it.first;
		uint32 entrySize = it.second->GetSize();

		ByteBuffer->write(&entry);
		ByteBuffer->write(&entrySize);

		it.second->Save(*ByteBuffer);
	}

	uint32 lastEntryId = 0;
	ByteBuffer->write(&lastEntryId);

	uint32 lastEntrySize = 0;
	ByteBuffer->write(&lastEntrySize);
}
