#pragma once

template<class RECORD_T>
inline DBCFile<RECORD_T>::DBCFile(IFilesManager* FilesManager, const std::string& FileName)
{
	std::shared_ptr<IFile> file = FilesManager->Open(std::string("DBFilesClient\\") + FileName);
	if (file == nullptr)
		throw CException("DBCFile[%s]: Not found.", FileName.c_str());

	m_File = file;

	char header[5];
	file->readBytes(header, 4);
	header[4] = '\0';
	_ASSERT(header[0] == 'W' && header[1] == 'D' && header[2] == 'B' && header[3] == 'C');

	file->readBytes(&recordCount, 4);// Number of records
	file->readBytes(&fieldCount, 4); // Number of fields
	file->readBytes(&recordSize, 4); // Size of a record
	file->readBytes(&stringSize, 4); // String size

	Log::Info("DBCFile[%s]: Fields count = '%d'. Records count = '%d'.", FileName.c_str(), fieldCount, recordCount);

	_ASSERT(fieldCount * 4 == recordSize);

	uint64_t stringTableOffset = file->getPos() + recordSize * recordCount;
	stringTable = file->getData() + stringTableOffset;

	// Fill record table
	for (uint64_t _offset = file->getPos(); _offset != stringTableOffset; _offset += recordSize)
	{
		RECORD_T record(this, const_cast<uint8*>(file->getData() + _offset));
		m_Records.insert(std::make_pair(record.Get_ID(), record));
	}

	_ASSERT(recordCount == m_Records.size());
}

template<class RECORD_T>
inline DBCFile<RECORD_T>::~DBCFile()
{}
