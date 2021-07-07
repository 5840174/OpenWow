#pragma once

#include <libmpq/libmpq/mpq.h>

struct SMPQArchive
{
	SMPQArchive(mpq_archive_s* Archive_)
		: Archive(Archive_)
	{
	}

	mpq_archive_s* Archive;
	std::mutex ArchiveLock;
};

struct SMPQFileLocation
{
	SMPQFileLocation() :
		archive(nullptr),
		fileNumber(0),
		exists(false)
	{}

	SMPQFileLocation(SMPQArchive* _archive, uint32 _fileNumber) :
		archive(_archive),
		fileNumber(_fileNumber),
		exists(true)
	{}

	bool exists;
	SMPQArchive* archive;
	uint32 fileNumber;
};

class ZN_API CMPQFilesStorage 
	: public IznFilesStorage
{
public:
	CMPQFilesStorage(std::string _path);
	virtual ~CMPQFilesStorage();

	// IFilesStorage
	std::shared_ptr<IFile>  Open(std::string FileName) override;
	size_t                  GetSize(std::string FileName) const override;
	bool                    IsExists(std::string FileName) const override;

	// CMPQFilesStorage
	void AddArchive(std::string _filename);
	SMPQFileLocation GetFileLocation(const std::string& _filename) const;

private:
	const std::string           m_Path;

	mutable SMPQFileLocation m_DefaultMPQFileLocation;
	std::vector<std::shared_ptr<SMPQArchive>> m_OpenArchives;
	//mutable std::mutex m_Lock;
};