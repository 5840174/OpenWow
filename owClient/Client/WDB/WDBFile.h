#pragma once

class ZN_API CowWDBFile
{
public:
	CowWDBFile(IFilesManager* FilesManager, const std::string& FileName);
	virtual ~CowWDBFile();

public:
	std::shared_ptr<IFile> LoadFile();
	void SaveCache() const;

protected:
	virtual void CreateCacheBuffer(IByteBuffer * ByteBuffer) const = 0;

protected:
	std::string m_FileName;

	char m_Identifier[5];
	uint32 m_ClientVersion;
	char m_ClientLocale[5];
	uint32 m_RecordSize;
	uint32 m_RecordVersion;
	uint32 m_CacheVersion;

private:
	IFilesManager * m_FilesManager;
};