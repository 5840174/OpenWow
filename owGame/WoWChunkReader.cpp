#include "stdafx.h"

// General
#include "WoWChunkReader.h"

WoWChunkReader::WoWChunkReader(IBaseManager* BaseManager, std::string _fileName)
{
	m_File = BaseManager->GetManager<IFilesManager>()->Open(_fileName);
	_ASSERT(m_File != nullptr);

	InitMaps();
}

WoWChunkReader::WoWChunkReader(IBaseManager* BaseManager, std::shared_ptr<IFile> file)
	: m_File(file)
{
	InitMaps();
}

WoWChunkReader::~WoWChunkReader()
{
}

std::shared_ptr<IByteBuffer> WoWChunkReader::OpenChunk(const char * _name)
{
	_ASSERT(m_File != nullptr);

	auto chunkIterator = m_ChunksMap.find(_name);
	if (chunkIterator == m_ChunksMap.end())
		return std::shared_ptr<IByteBuffer>();

	const auto& chunkInfos = chunkIterator->second;
	_ASSERT(chunkInfos.size() == 1);
	return GetChunk(chunkInfos[0]);
}

std::vector<std::shared_ptr<IByteBuffer>> WoWChunkReader::OpenChunks(const char * _name)
{
	std::vector<std::shared_ptr<IByteBuffer>> result;

	auto chunkIterator = m_ChunksMap.find(_name);
	if (chunkIterator == m_ChunksMap.end())
	{
		return result;
	}

	auto& chunkOffsets = chunkIterator->second;
	_ASSERT(!chunkOffsets.empty());
	for (auto& subChunk : chunkOffsets)
	{
		result.push_back(GetChunk(subChunk));
	}

	return result;
}

void WoWChunkReader::InitMaps()
{
	char fourcc[5];
	uint32 size = 0;
	while (!m_File->isEof())
	{
		memset(fourcc, 0x00, 4);
		m_File->readBytes(fourcc, 4);
		flipcc(fourcc);
		fourcc[4] = 0;

		size = 0;
		m_File->readBytes(&size, 4);
		if (size == 0)
			continue;

		uint32_t nextpos = m_File->getPos() + size;

		_ASSERT(nextpos <= m_File->getSize());
		_ASSERT(size <= m_File->getSize());
		m_ChunksMap[fourcc].push_back(std::make_pair(m_File->getPos(), size));

		m_File->seek(nextpos);
	}
}

std::shared_ptr<IByteBuffer> WoWChunkReader::GetChunk(ChunkInfo chunks)
{
	size_t offset = chunks.first;
	size_t size = chunks.second;

	m_File->seek(offset);

	return std::make_shared<CByteBufferOnlyPointer>(m_File->getDataFromCurrent(), size);
}
