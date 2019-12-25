#pragma once

// MPQ Files
#include <libmpq/mpq.h>

struct SMPQFileLocation
{
	SMPQFileLocation() :
		archive(nullptr),
		fileNumber(0),
		exists(false)
	{}

	SMPQFileLocation(mpq_archive* _archive, uint32 _fileNumber) :
		archive(_archive),
		fileNumber(_fileNumber),
		exists(true)
	{}

	bool exists;
	mpq_archive* archive;
	uint32 fileNumber;
};

class ZN_API CMPQFilesStorage : public IFilesStorage, public IFilesStorageEx
{
public:
	CMPQFilesStorage(std::string _path, Priority _priority = Priority::PRIOR_NORMAL);
	virtual ~CMPQFilesStorage();

	// IFilesStorage
	std::shared_ptr<IFile>  OpenFile(std::string FileName, EFileAccessType FileAccessType = EFileAccessType::Read) override;
	size_t                  GetFileSize(std::string FileName) override;
	bool                    IsFileExists(std::string FileName) override;

	// IFilesStorageEx
	Priority GetPriority() const;

	// CMPQFilesStorage
	void AddArchive(std::string _filename);
	SMPQFileLocation GetFileLocation(const std::string& _filename);

private:
	const std::string           m_Path;
	const Priority              m_Priority;

	std::vector<mpq_archive_s*> m_OpenArchives;
	std::mutex                  m_Lock;
};