#pragma once

class OW_ENGINE_API WoWChunkReader 
{
public:
	WoWChunkReader(IBaseManager* BaseManager, std::string _fileName);
	WoWChunkReader(IBaseManager* BaseManager, std::shared_ptr<IFile> file);
	virtual ~WoWChunkReader();

	std::shared_ptr<IByteBuffer> OpenChunk(const char* _name);
	std::vector<std::shared_ptr<IByteBuffer>> OpenChunks(const char* _name);

	template<class T>
	std::vector<T> OpenChunkT(const char* _name);

private:
	typedef std::pair<size_t, size_t>  ChunkInfo;
	typedef std::vector<ChunkInfo>     Chunks;

protected:
	void InitMaps();
	std::shared_ptr<IByteBuffer> GetChunk(ChunkInfo chunks);

private:
	std::shared_ptr<IFile> m_File;
	std::unordered_map<std::string, Chunks> m_ChunksMap; // seek and size
};

#include "WoWChunkReader.inl"
