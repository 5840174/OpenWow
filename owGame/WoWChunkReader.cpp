#include "stdafx.h"

// General
#include "WoWChunkReader.h"

WoWChunkReader::WoWChunkReader(const IBaseManager& BaseManager, std::string FileName)
{
	m_ByteBuffer = BaseManager.GetManager<IFilesManager>()->Open(FileName);
	if (m_ByteBuffer == nullptr)
		throw CException("WoWChunkReader: Unable to initialize WoWChunkReader. ByteBuffer is nullptr. Filename = '%s'.", FileName.c_str());

	InitMaps();
}

WoWChunkReader::WoWChunkReader(const IBaseManager& BaseManager, const std::shared_ptr<IByteBuffer>& ByteBuffer)
{
	m_ByteBuffer = ByteBuffer;
	if (m_ByteBuffer == nullptr)
		throw CException("WoWChunkReader: Unable to initialize WoWChunkReader. ByteBuffer is nullptr.");

	InitMaps();
}

WoWChunkReader::WoWChunkReader(const IBaseManager& BaseManager, const void* DataPtr, size_t DataSize)
{
	m_ByteBuffer = MakeShared(CByteBuffer, DataPtr, DataSize);
	if (m_ByteBuffer == nullptr)
		throw CException("WoWChunkReader: Unable to initialize WoWChunkReader. ByteBuffer is nullptr.");
	
	InitMaps();
}

WoWChunkReader::~WoWChunkReader()
{
}

std::shared_ptr<IByteBuffer> WoWChunkReader::OpenChunk(const char * _name)
{
	const auto& chunkIterator = m_ChunksMap.find(_name);
	if (chunkIterator == m_ChunksMap.end())
		return nullptr;

	const auto& chunkInfos = chunkIterator->second;
	if (chunkInfos.size() != 1)
		throw CException("WoWChunkReader::OpenChunk: Reader don't contain single chunk '%s'. Count = '%d'.", _name, chunkInfos.size());

	return GetChunk(chunkInfos[0]);
}

std::vector<std::shared_ptr<IByteBuffer>> WoWChunkReader::OpenChunks(const char * _name)
{
	std::vector<std::shared_ptr<IByteBuffer>> result;

	const auto& chunkIterator = m_ChunksMap.find(_name);
	if (chunkIterator == m_ChunksMap.end())
		return result;

	auto& chunkOffsets = chunkIterator->second;
	_ASSERT(!chunkOffsets.empty());
	for (auto& subChunk : chunkOffsets)
		result.push_back(GetChunk(subChunk));

	return result;
}

void WoWChunkReader::InitMaps()
{
	char fourcc[5] = { 0 };
	uint32 size = 0;
	while (false == m_ByteBuffer->isEof())
	{
		memset(fourcc, 0x00, 4);
		m_ByteBuffer->readBytes(fourcc, 4);
		std::swap(fourcc[0], fourcc[3]);
		std::swap(fourcc[1], fourcc[2]);

		size = 0;
		m_ByteBuffer->readBytes(&size, 4);
		if (size == 0)
			continue;

		size_t nextpos = m_ByteBuffer->getPos() + static_cast<size_t>(size);
		if (size > m_ByteBuffer->getSize() || nextpos > m_ByteBuffer->getSize())
			throw CException("WoWChunkReader: Unable to initialize WoWChunkReader. Incorrect chunk size. Name = '%s'. Size = '%d'. NextPos = '%d'. BufferSize = '%d'.", fourcc, size, nextpos, m_ByteBuffer->getSize());

		m_ChunksMap[fourcc].push_back(std::make_pair(m_ByteBuffer->getPos(), size));

		m_ByteBuffer->seek(nextpos);
	}
}

std::shared_ptr<IByteBuffer> WoWChunkReader::GetChunk(ChunkInfo chunks)
{
	size_t offset = chunks.first;
	size_t size = chunks.second;

	m_ByteBuffer->seek(offset);

	return std::make_shared<CByteBufferOnlyPointer>(m_ByteBuffer->getDataFromCurrent(), size);
}
