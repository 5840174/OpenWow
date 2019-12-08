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

class OW_ENGINE_API CMPQFilesStorage : public IFilesStorage, public IFilesStorageEx
{
public:
	CMPQFilesStorage(std::string _path, Priority _priority = PRIOR_NORMAL);
	virtual ~CMPQFilesStorage();

	// IFilesStorage
	std::shared_ptr<IFile>  CreateFile(const std::string& _name) override;
	size_t                  GetFileSize(const std::string& _name) override;
	bool                    IsFileExists(const std::string& _name) override;

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