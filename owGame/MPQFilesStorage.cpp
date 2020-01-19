#include "stdafx.h"

// General
#include "MPQFilesStorage.h"
#pragma comment(lib, "libmpq.lib")

CMPQFilesStorage::CMPQFilesStorage(std::string _path, Priority _priority)
	: m_Path(_path)
	, m_Priority(_priority)
{
	AddArchive("base.MPQ");
	AddArchive("dbc.MPQ");
	//AddArchive(std::string("fonts.MPQ"));
	AddArchive(std::string("interface.MPQ"));
	AddArchive("misc.MPQ");
	AddArchive("model.MPQ");
	AddArchive("patch.MPQ");
	AddArchive("patch-2.MPQ");
	//AddArchive(std::string("sound.MPQ"));
	//AddArchive(std::string("speech.MPQ"));
	AddArchive("terrain.MPQ");
	AddArchive("texture.MPQ");
	AddArchive("wmo.MPQ");
}

CMPQFilesStorage::~CMPQFilesStorage()
{
	for (auto it : m_OpenArchives)
	{
		libmpq__archive_close(it);
	}
}



//
// // IFilesStorage
//
std::shared_ptr<IFile> CMPQFilesStorage::OpenFile(std::string FileName, EFileAccessType FileAccessType)
{
	std::lock_guard<std::mutex> lock(m_Lock);

	std::shared_ptr<CFile> file = std::make_shared<CFile>(FileName);
	CByteBuffer& byteBuffer = file->GetByteBuffer();
	
	SMPQFileLocation location = GetFileLocation(file->Path_Name());
	if (!location.exists)
		return nullptr;

	libmpq__off_t size;
	libmpq__file_size_unpacked(location.archive, location.fileNumber, &size);

	// Allocate space and set data
	byteBuffer.Allocate(size);
	libmpq__file_read(location.archive, location.fileNumber, &(byteBuffer.getDataEx())[0], byteBuffer.getSize(), &size);
	_ASSERT(byteBuffer.getSize() == size);
	byteBuffer.SetFilled();

	//Log::Info("File[%s] opened. [mpq]", Path_Name().c_str());

	return file;
}

bool CMPQFilesStorage::SaveFile(std::shared_ptr<IFile> File)
{
	_ASSERT_EXPR(false, L"CMPQFilesStorage: Unable to save file to this file storage.");
	return false;
}

size_t CMPQFilesStorage::GetFileSize(std::string FileName)
{
	std::lock_guard<std::mutex> lock(m_Lock);

	SMPQFileLocation location = GetFileLocation(FileName);
	if (location.exists)
	{
		libmpq__off_t size;
		libmpq__file_size_unpacked(location.archive, location.fileNumber, &size);
		return size;
	}

	return 0;
}

bool CMPQFilesStorage::IsFileExists(std::string FileName)
{
	std::lock_guard<std::mutex> lock(m_Lock);

	return GetFileLocation(FileName).exists;
}



//
// IFilesStorageEx
//
IFilesStorageEx::Priority CMPQFilesStorage::GetPriority() const
{
	return m_Priority;
}



//
// CMPQFilesStorage
//
void CMPQFilesStorage::AddArchive(std::string filename)
{
	mpq_archive_s* mpq_a;
	int result = libmpq__archive_open(&mpq_a, (m_Path + filename).c_str(), -1);
	//Log::Info("Opening %s", filename.c_str());
	if (result)
	{
		switch (result)
		{
		case LIBMPQ_ERROR_OPEN:
			Log::Error("Error opening archive [%s]: Does file really exist?", filename.c_str());
			break;

		case LIBMPQ_ERROR_FORMAT:            /* bad file format */
			Log::Error("Error opening archive [%s]: Bad file format", filename.c_str());
			break;

		case LIBMPQ_ERROR_SEEK:         /* seeking in file failed */
			Log::Error("Error opening archive [%s]: Seeking in file failed", filename.c_str());
			break;

		case LIBMPQ_ERROR_READ:              /* Read error in archive */
			Log::Error("Error opening archive [%s]: Read error in archive", filename.c_str());
			break;

		case LIBMPQ_ERROR_MALLOC:               /* maybe not enough memory? :) */
			Log::Error("Error opening archive [%s]: Maybe not enough memory", filename.c_str());
			break;

		default:
			Log::Error("Error opening archive [%s]: Unknown error\n", filename.c_str());
			break;
		}

		return;
	}

	m_OpenArchives.push_back(mpq_a);
	Log::Green("CMPQFile[%s]: Added!", filename.c_str());
}

SMPQFileLocation CMPQFilesStorage::GetFileLocation(const std::string& filename)
{
	for (auto& i = m_OpenArchives.rbegin(); i != m_OpenArchives.rend(); ++i)
	{
		mpq_archive_s* mpq_a = *i;

		uint32 filenum;
		if (libmpq__file_number(mpq_a, filename.c_str(), &filenum) == LIBMPQ_ERROR_EXIST)
			continue;

		return SMPQFileLocation(mpq_a, filenum);
	}

	return SMPQFileLocation();
}
