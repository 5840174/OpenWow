#include "stdafx.h"

// General
#include "MPQFilesStorage.h"

CMPQFilesStorage::CMPQFilesStorage(std::string _path)
	: m_Path(_path)
{
#if WOW_CLIENT_VERSION == WOW_CLASSIC_1_12_1
	AddArchive("base.MPQ");
	AddArchive("dbc.MPQ");
	AddArchive("interface.MPQ");
	AddArchive("misc.MPQ");
	AddArchive("model.MPQ");
	AddArchive("patch.MPQ");
	AddArchive("patch-2.MPQ");
	AddArchive("terrain.MPQ");
	AddArchive("texture.MPQ");
	AddArchive("wmo.MPQ");
#elif WOW_CLIENT_VERSION == WOW_BC_2_4_3
	AddArchive("common.MPQ");
	AddArchive("expansion.MPQ");
	AddArchive("patch.MPQ");
	AddArchive("patch-2.MPQ");

	AddArchive("enGB/backup-enGB.MPQ");
	AddArchive("enGB/base-enGB.MPQ");

	AddArchive("enGB/expansion-locale-enGB.MPQ");
	AddArchive("enGB/expansion-speech-enGB.MPQ");

	AddArchive("enGB/locale-enGB.MPQ");
	AddArchive("enGB/speech-enGB.MPQ");

	AddArchive("enGB/patch-enGB.MPQ");
	AddArchive("enGB/patch-enGB-2.MPQ");

#elif WOW_CLIENT_VERSION == WOW_WOTLK_3_3_5
	AddArchive("common.MPQ");
	AddArchive("common-2.MPQ");
	AddArchive("expansion.MPQ");
	AddArchive("lichking.MPQ");
	AddArchive("patch.MPQ");
	AddArchive("patch-2.MPQ");
	AddArchive("patch-3.MPQ");

	AddArchive("ruRU/backup-ruRU.MPQ");
	AddArchive("ruRU/base-ruRU.MPQ");

	AddArchive("ruRU/expansion-locale-ruRU.MPQ");
	AddArchive("ruRU/expansion-speech-ruRU.MPQ");

	AddArchive("ruRU/lichking-locale-ruRU.MPQ");
	AddArchive("ruRU/lichking-speech-ruRU.MPQ");

	AddArchive("ruRU/locale-ruRU.MPQ");
	AddArchive("ruRU/speech-ruRU.MPQ");

	AddArchive("ruRU/patch-ruRU.MPQ");
	AddArchive("ruRU/patch-ruRU-2.MPQ");
	AddArchive("ruRU/patch-ruRU-3.MPQ");
#endif
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
std::shared_ptr<IFile> CMPQFilesStorage::Open(std::string FileName)
{
	std::lock_guard<std::mutex> lock(m_Lock);

	std::shared_ptr<CFile> file = std::make_shared<CFile>(FileName, shared_from_this());
	CByteBuffer& byteBuffer = file->GetByteBuffer();
	
	SMPQFileLocation location = GetFileLocation(file->Path_Name());
	if (!location.exists)
		return nullptr;

	libmpq__off_t size;
	libmpq__file_size_unpacked(location.archive, location.fileNumber, &size);
	_ASSERT(size < 1024 * 1024 * 500);

	// Allocate space and set data
	std::vector<uint8> buffer;
	buffer.resize(size);
	libmpq__file_read(location.archive, location.fileNumber, &buffer[0], size, &size);

	byteBuffer = std::move(CByteBuffer(std::move(buffer)));

	//Log::Info("File[%s] opened. [mpq]", Path_Name().c_str());

	return file;
}

size_t CMPQFilesStorage::GetSize(std::string FileName) const
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

bool CMPQFilesStorage::IsExists(std::string FileName) const
{
	std::lock_guard<std::mutex> lock(m_Lock);
	return GetFileLocation(FileName).exists;
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
	Log::Info("MPQFile[%s]: Added!", filename.c_str());
}

SMPQFileLocation CMPQFilesStorage::GetFileLocation(const std::string& filename) const
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
