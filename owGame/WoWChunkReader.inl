#pragma once

template<class T>
inline std::vector<T> WoWChunkReader::OpenChunkT(const char * _name)
{
	std::vector<T> list;

	std::shared_ptr<IByteBuffer> buffer = OpenChunk(_name);
	if (buffer == nullptr)
	{
		return list;
	}

	size_t count = buffer->getSize() / sizeof(T);
	T* listT = (T*)buffer->getDataFromCurrent();

	list.assign(listT, listT + count);

	return list;
}