#include "stdafx.h"

// General
#include "WDBFile.h"

CowWDBFile::CowWDBFile(IFilesManager * FilesManager, const std::string& FileName)
	: m_FilesManager(FilesManager)
	, m_FileName(FileName)
{

}

CowWDBFile::~CowWDBFile()
{}



//
// Protected
//
std::shared_ptr<IFile> CowWDBFile::LoadFile()
{
	auto file = m_FilesManager->Open(m_FileName);
	if (file == nullptr)
		throw CException("CowWDBFile: File %s' not found.", m_FileName.c_str());

	file->readBytes(m_Identifier, 4);
	//std::swap(m_Identifier[0], m_Identifier[3]);
	//std::swap(m_Identifier[1], m_Identifier[2]);
	m_Identifier[4] = '\0';

	file->read(&m_ClientVersion);

	file->readBytes(m_ClientLocale, 4);
	//std::swap(m_ClientLocale[0], m_ClientLocale[3]);
	//std::swap(m_ClientLocale[1], m_ClientLocale[2]);
	m_ClientLocale[4] = '\0';

	file->read(&m_RecordSize);
	file->read(&m_RecordVersion);
	file->read(&m_CacheVersion);

	return file;
}

void CowWDBFile::SaveCache() const
{
	auto file = m_FilesManager->Create(m_FileName + "__", true);
	if (file == nullptr)
		throw CException("CowWDBFile: Unable to create %s' file.", m_FileName.c_str());

	// Header
	file->writeBytes(m_Identifier, 4);
	file->write(&m_ClientVersion);
	file->writeBytes(m_ClientLocale, 4);
	file->write(&m_RecordSize);
	file->write(&m_RecordVersion);
	file->write(&m_CacheVersion);

	// Save data
	CByteBuffer buffer;
	CreateCacheBuffer(&buffer);
	file->writeBytes(buffer.getData(), buffer.getSize());

	if (false == file->Save())
		Log::Warn("Unable to save '%s' file.", file->Path_Name().c_str());
}
