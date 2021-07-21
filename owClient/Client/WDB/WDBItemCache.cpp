#include "stdafx.h"

// General
#include "WDBItemCache.h"

CowWDBItemCache::CowWDBItemCache(IFilesManager * FilesManager)
	: CowWDBFile(FilesManager, "itemcache.wdb")
{
	auto file = LoadFile();
	if (file == nullptr)
	{
		Log::Warn("CowWDBItemCache:: File '%s' not found.", m_FileName.c_str());
	}

	while (false == file->isEof())
	{
		uint32 entry;
		file->read(&entry);

		uint32 entrySize;
		file->read(&entrySize);

		if (entrySize == 0)
			continue;

		std::shared_ptr<SItemQueryResult> itemQueryResult = MakeShared(SItemQueryResult, entry);
		itemQueryResult->Load(*file);

		if (entrySize != itemQueryResult->GetSize())
			throw CException("Incorrect query size.");

		Log::Info("Item '%s' loaded from cache.", itemQueryResult->Name.c_str());

		m_Cache.insert(std::make_pair(entry, itemQueryResult));
	}

	Log::Print("CowWDBItemCache:: Item cache size = '%d'.", m_Cache.size());
}

CowWDBItemCache::~CowWDBItemCache()
{}

bool CowWDBItemCache::Get(CowGuid::EntryType_t Entry, std::shared_ptr<SItemQueryResult> * QueryResult) const
{
	const auto& it = m_Cache.find(Entry);
	if (it == m_Cache.end())
		return false;
	*QueryResult = it->second;
	return true;
}

void CowWDBItemCache::Add(std::shared_ptr<SItemQueryResult> QueryResult)
{
	auto& it = m_Cache.find(QueryResult->entryID);
	if (it == m_Cache.end())
		m_Cache.insert(std::make_pair(QueryResult->entryID, QueryResult));
	else
		it->second = QueryResult;
}



//
// Protected
//
void CowWDBItemCache::CreateCacheBuffer(IByteBuffer * ByteBuffer) const
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