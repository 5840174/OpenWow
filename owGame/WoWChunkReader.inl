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

	uint32 count = buffer->getSize() / sizeof(T);
	T* listT = (T*)buffer->getDataFromCurrent();

	list.assign(listT, listT + count);

	/*_ASSERT(listT != nullptr);
	for (uint32 i = 0; i < count; i++)
	{
		list.push_back(listT[i]);
	}*/

	return list;
}