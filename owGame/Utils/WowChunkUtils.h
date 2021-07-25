#pragma once

/**
  * Some WoW chunks is sequence of string with '\0' delimeter.
*/
inline void PasreChunkAsStringArray(const std::shared_ptr<IByteBuffer>& Bytes, std::vector<std::string> * StringsArray)
{
	if (Bytes == nullptr)
		throw CException("PasreChunkAsStringArray: Bytes is nullptr.");

	if (Bytes->getSize() == 0)
		return;

	if (StringsArray == nullptr)
		throw CException("PasreChunkAsStringArray: StringsArray is nullptr.");

	if (false == StringsArray->empty())
		throw CException("PasreChunkAsStringArray: StringsArray is not empty.");

	std::vector<char> buffer;
	buffer.resize(Bytes->getSize() + 1);
	buffer[Bytes->getSize()] = '\0';

	if (false == Bytes->readBytes(buffer.data(), Bytes->getSize()))
		throw CException("PasreChunkAsStringArray: IByteBuffer::readBytes error. Pos = '%d'. Size = '%d'.", Bytes->getPos(), Bytes->getSize());

	char* tempBuffer = buffer.data();
	while (tempBuffer < buffer.data() + Bytes->getSize())
	{
		std::string string(tempBuffer);
		StringsArray->push_back(string);
		tempBuffer += string.length() + 1;
	}
}
