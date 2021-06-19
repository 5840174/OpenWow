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


static glm::vec3 fromRealToGame(glm::vec3 p)
{
	return glm::vec3(
		-p.x + C_ZeroPoint,
		-p.z + C_ZeroPoint,
		(p.y)
	);
}

// Y            X        Z			// DBC
//-618.518, -4251.67, 38.718, 0
// X			Y        Z
//-4251.67, -618.518, 38.718, 0
static glm::vec3 fromGameToReal(glm::vec3 p)
{
	return glm::vec3(
		-p.y + C_ZeroPoint,
		p.z,
		-p.x + C_ZeroPoint
	);
}